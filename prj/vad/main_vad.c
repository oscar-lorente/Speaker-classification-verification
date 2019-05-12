/*
--------------------------------------------------------------
Por favor, lea el fichero READ_ME.txt antes de mirar el código
--------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>
#include "vad.h"
#include "pav_analysis.h"

#define DEBUG_VAD 0x1

int main(int argc, const char *argv[]) {
  int verbose = 0;
  /* To show internal state of vad
     verbose = DEBUG_VAD;
  */

  /*Las variables counterU y counterS se utilizan para contar el número de tramos seguidos (sin
  voz de por medio) en los que el estado es undef o silencio, respectivamente*/
  int counterU = 0;
  int counterS = 0;

  /*En la variable pot_media se almacena la potencia media de los primeros 150ms de la señal de audio*/
  float pot_media = 0;

  /*La variable auxiliar t_aux se usa para guardar los instantes en los que haya un cambio
  de voz a silencio (explicado con detalle más adelante)*/
  float t_aux = 0;

  /*Las variables state y last_state pueden ser voz, silencio o undef, pues almacenan el estado del frame
  actual y del anterior, respectivamente, pero official_last_state solo puede valer voz o silencio (nunca undef).
  Concretamente, el último estado "oficial" pasará a ser voz siempre que el estado del frame actual (state) sea
  voz, pero official_last_state solo pasará a ser silencio cuando haya +300ms de silencio seguidos (explicado
  con detalle más adelante)*/
  VAD_STATE official_last_state = ST_SILENCE;

  SNDFILE *sndfile_in, *sndfile_out = 0;
  SF_INFO sf_info;
  FILE *vadfile;
  int n_read, n_write, i;

  VAD_DATA *vad_data;
  VAD_STATE state, last_state;

  float *buffer, *buffer_zeros;
  int frame_size;        /* in samples */
  float frame_duration;  /* in seconds */
  float t, last_t;

  if (argc != 3 && argc != 4) {
    fprintf(stderr, "Usage: %s input_file.wav output.vad [output_file.wav]\n",
            argv[0]);
    return -1;
  }

  /* Open input sound file */
  sndfile_in = sf_open(argv[1], SFM_READ, &sf_info);
  if (sndfile_in == 0) {
    fprintf(stderr, "Error opening input file: %s\n", argv[1]);
    return -1;
  }

  if (sf_info.channels != 1) {
    fprintf(stderr, "Error: the input file has to be mono: %s\n", argv[1]);
    return -2;
  }

  /* Open vad file */
  vadfile = fopen(argv[2], "wt");
  if (vadfile == 0) {
    fprintf(stderr, "Error opening output vad file: %s\n", argv[2]);
    return -1;
  }

  /* Open output sound file, with same format, channels, etc. than input */
  if (argc == 4) {
    sndfile_out = sf_open(argv[3], SFM_WRITE, &sf_info);
    if (sndfile_out == 0) {
      fprintf(stderr, "Error opening output wav file: %s\n", argv[3]);
      return -1;
    }
  }

  vad_data = vad_open(sf_info.samplerate);
  /* Allocate memory for buffer */
  frame_size   = vad_frame_size(vad_data);
  buffer       = (float *) malloc(frame_size * sizeof(float));
  buffer_zeros = (float *) malloc(frame_size * sizeof(float));
  for (i=0; i< frame_size; ++i) buffer_zeros[i] = 0.0F;

  frame_duration = (float) frame_size/ (float) sf_info.samplerate;
  t = last_t = 0;
  last_state = ST_UNDEF;

  /*De forma iterada, leemos los 15 primeros frames (de 10ms cada uno --> 150ms) de
  la señal de audio y calculamos la potencia de cada frame (mediante la función compute_power),
  para así obtener la potencia media del primer tramo de silencio*/
  for (int i = 0; i < 15; i++) {
    n_read = sf_read_float(sndfile_in, buffer, frame_size);
    /*Por si hay algún error o se acaba la señal de voz*/
    if  (n_read != frame_size)
      break;
    /*A pot_media se le suma la potencia de cada frame de 10ms*/
    pot_media = pot_media + compute_power(buffer, sf_info.samplerate * 10.0F * 1e-3);
  }
  /*Retrocedemos los 15 frames (150ms) que habíamos leído para, más adelante,
  analizar la señal de audio completa (desde el principio)*/
  sf_seek(sndfile_in, -(frame_size*15), SEEK_CUR);
  /*Finalmente se divide la suma de potencias de los 15 frames (variable pot_media) entre
  el número de frames (15) para obtener la potencia media deseada*/
  pot_media = pot_media / 15.0;

  while(1) { /* For each frame ... */
    n_read = sf_read_float(sndfile_in, buffer, frame_size);

    /* End loop when file has finished (or there is an error) */
    if  (n_read != frame_size)
      break;

    if (sndfile_out != 0) {
      /* TODO: copy all the samples into sndfile_out */
      n_write = sf_write_float(sndfile_out, buffer, frame_size);
      /*Por si hay algún error*/
      if  (n_write != frame_size)
        break;
    }

    /*Le pasamos a la función vad() el parámetro pot_media para calcular los valores
    de los umbrales de decisión, y así decidir si el estado actual es voz, silencio o undef*/
    state = vad(vad_data, buffer, pot_media);
    if (verbose & DEBUG_VAD)
      vad_show_state(vad_data, stdout);

    /*Si el estado actual es voz*/
    if(strcmp(state2str(state), "V") == 0){
      /*Si se ha producido un cambio de estado de undef/silencio a voz*/
      if(last_state != state){
        /*Si el último estado oficial era silencio*/
        if(strcmp(state2str(official_last_state), "S") == 0){
          /*Si counterS >= 30, es decir, si llevamos +30 frames (300ms) de
          silencio sin ningún frame de voz de por medio (pueden haber undef's),
          se interpreta que había un silencio prolongado (entre dos frases distintas,
          por ejemplo) que ha sido interrumpido con voz. Si no usaramos esta condición,
          se interpretarían como silencios las pequeñas pausas entre palabras de una misma frase
          o incluso las transiciones entre 2 letras de una misma palabra, cosa que queremos evitar*/
          if (counterS >=30) {
            /*Si hemos definido un fichero wav de salida*/
            if (sndfile_out != 0){
              /*Retrocedemos al primer frame (de este silencio) en el que se había detectado un estado
              de silencio o undef. +1 para compensar el frame que se acaba de analizar y que nos ha
              llevado al estado actual de voz*/
              sf_seek(sndfile_out, -(frame_size*(counterS+counterU+1)), SEEK_CUR);
              /*Se silencia el tramo correspondiente poniendo 0s (buffer_zeros) en los frames. El for
              itera hasta counterS (y no hasta counterS+counterU+1, que es el número de frames que se
              había retrocedido) porque puede pasar que antes de entrar en el estado de voz (estado actual)
              hayan habido 1, 2, 3... estados undef (en los cuales se aumenta el valor de counterU) consecutivos,
              sin ningún silencio de por medio (S...S U U V) y estos deben interpretarse como voz, y por lo tanto
              no silenciarse*/
              for (int i = 0; i < counterS; i++)
                n_write = sf_write_float(sndfile_out, buffer_zeros, frame_size);
              /*Por si ocurre lo mencionado arriba (estados undef justo antes del estado actual de voz)
              se avanza para volver al frame que estabamos antes de silenciar nada (y después de haber analizado ya
              un frame de voz, de ahí el +1), para seguir analizando la señal completa*/
              sf_seek(sndfile_out, frame_size*(counterU+1), SEEK_CUR);
            }
            if (t != last_t){
              /*Se imprime en el fichero .vad el instante de inicio y final del tramo de silencio (que acaba de
              finalizar)*/
              fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(official_last_state));
              last_t = t;
            }
          } //Fin condición de ">=30 frames de silencio"
          /*Independientemente del número de frames de silencio, el estado oficial pasa a ser voz*/
          official_last_state = state;
        } //Fin condición de "último estado ofical = silencio"
        /*Si ha habido un cambio de estado de undef/silencio a voz se reinician sus contadores*/
        counterU = 0;
        counterS = 0;
      } //Fin condición de "estado anterior != estado actual"
    }
    /*Si el estado actual es silencio*/
    else if (strcmp(state2str(state), "S") == 0){
      /*Si el último estado oficial era voz y llevamos +30 frames (300ms) de
      silencio sin ningún frame de voz de por medio (pueden haber undef's) se interpreta que hay
      un silencio prolongado (entre dos frases distintas, por ejemplo), por lo que el estado oficial pasa
      a ser silencio y se imprime en el fichero .vad el instante de inicio y final del tramo de voz*/
      if((strcmp(state2str(official_last_state), "V") == 0) && (counterS >= 30)){
        if ((t != last_t)){
          /*Se calcula el instante de tiempo en el que empezó el tramo de silencio (pues se ha reconocido
          como tramo de silencio después de analizar varios frames, por lo que hay que retroceder el tiempo
          correspondiente). En caso del tiempo no se suma el +1 (cosa que hacíamos a la hora de retroceder
          frames con la función f_seek()) porque el tiempo (variable t) se incrementa al final del while, por
          lo que aunque hayamos analizado un frame de silencio (pues el estado actual es de silencio) aún no
          se ha incrementado el tiempo correspondiente (10ms)*/
          t_aux = t - (0.01*(counterS+counterU));
          fprintf(vadfile, "%f\t%f\t%s\n", last_t, t_aux, state2str(official_last_state));
          /*Se le asigna a last_t el instante en el que ha empezado el nuevo tramo de silencio, ya calculado y
          almacenado en t_aux*/
          last_t = t_aux;
        }
        official_last_state = state;
      }
      /*Si el estado anterior era undef se actualiza el contador de silencios y se reinicia el de undef's*/
      if(strcmp(state2str(last_state),"UNDEF") == 0){
        counterS = counterU + counterS;
        counterU = 0;
      }
      /*Si el estado actual es silencio se suma +1 a su contador*/
      counterS++;
    }
    /*Si el estado actual no es ni voz ni silencio --> es undef --> se suma +1 a su contador*/
    else counterU++;

    if (state != last_state) {
      last_state = state;
    }
    t += frame_duration;
  }

  /*Tal y como se ha programado, en el fichero .wav de salida (si lo hay) solo se silencia un tramo si
  después de este aparece voz, cosa que NO ocurre en el último tramo de silencio de la señal de audio.
  Por eso, en el siguiente if se silencia este tramo de la señal en el fichero de audio de salida.*/
  if (sndfile_out != 0) {
    sf_seek(sndfile_out, -(frame_size*(counterS+counterU+1)), SEEK_CUR);
    for (int i = 0; i < (counterS+counterU+1); i++)
      n_write = sf_write_float(sndfile_out, buffer_zeros, frame_size);
  }

  state = vad_close(vad_data);
  if (t != last_t)
    fprintf(vadfile, "%f\t%f\t%s\n", last_t, t, state2str(state));

  /* clean up: free memory, close open files */
  free(buffer);
  free(buffer_zeros);
  sf_close(sndfile_in);
  fclose(vadfile);
  if (sndfile_out) sf_close(sndfile_out);
  return 0;
}
