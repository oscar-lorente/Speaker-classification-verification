#include <iostream>
#include <cmath>
#include <vector>
#include "ampliacion.h"
#include "/home/oscar/Escritorio/3B/PAV/P3/prj/get_pitch/pitch_analyzer.h"
#include "ffft/FFTReal.h"

using namespace std;
using namespace upc;

namespace mejoras {

  /*Técnica de post-procesado: filtro de mediana. Una técnica muy sencilla
  que puede utilizarse tanto para eliminar errores de la detección de sonoridad,
  como para corregir los valores de F0 de los tramos sonoros*/
  void PitchAmpliacion::medianFilter(vector<float> &f0, int windowSize) {
    //Para controlar que el tamaño de la ventana sea un número impar
    if(windowSize%2 == 0)
      return;

    float aux;
    int N = f0.size(), windowSizeAux = ((windowSize-1)/2);

    //   Move window through all elements of the signal
    for (int i = windowSizeAux; i < N - windowSizeAux; ++i) {
       //   Pick up window elements
       float window[windowSize];
       for (int j = 0; j < windowSize; ++j)
          window[j] = f0[i-windowSizeAux+j];
       //   Order elements (only half of them)
       for (int j = 0; j < ((windowSize+1)/2); ++j) {
          //   Find position of minimum element
          int min = j;
          for (int k = j + 1; k < windowSize; ++k)
             if (window[k] < window[min])
                min = k;
          //   Put found minimum element in its place
          aux = window[j];
          window[j] = window[min];
          window[min] = aux;
       }
       //   Get result - the middle element
       f0[i - windowSizeAux] = window[windowSizeAux];
    }
  }

  /*Técnica de preprocesado: center clipping. Utilizada para obtener una onda más útil
  de la función de autocorrelación, y así encontrar más fácilmente los máximos de esta*/
  void PitchAmpliacion::centerClipping(float &x, float umbral) {
    //Si x se encuentra entre el umbral: -umbral < x < umbral --> x = 0
    if(abs(x) < umbral)
      x = 0.0;
    else{
      if(x > umbral)
        x -= umbral;
      else if(x < -umbral)
        x += umbral;
    }
  }

  /*Detector cepstral: función correspondiente a la implementación de un detector
  basado en el cepstrum (c) de tramo de señal (x). En este, se calcula la transformada
  de Fourier de x --> X, se obtiene el logaritmo del valor absoluto de X, y finalmente
  se realiza la transformada inversa de X --> c, correspondiente al cepstrum de la señal*/
  void PitchAmpliacion::cepstrum(vector<float> &x, vector<float> &c) {
    x.resize(1024); //x debe ser potencia de 2
    int N = x.size();
    float x2[N], X[N], S[N/2], c2[N/2], eps = 1e-20; //eps: epsilon --> para no tener log(0)
    //copia local del vector x
    for (int i=0; i<N; ++i){
      x2[i] = x[i];
    }

    ffft::FFTReal <float> fft_object (N);
    fft_object.do_fft (X, x2);     // x (real) --FFT---> f (complex)

    //log del valor absoluto de la TF de x: X --> almacenado en S
    S[0] = 10*log10(X[0] * X[0] + eps);
    for (int i=1; i<N/2-1; i++) {
      float re = X[i];
      float im = X[N/2+i];
      S[i] = 10*log10(re*re+im*im + eps);
    }
    S[N/2] = 10*log10(X[N/2]*X[N/2] + eps);

    fft_object.do_ifft (S, c2);    // f (complex) --IFFT--> x (real)
    fft_object.rescale (c2);       // Post-scaling should be done after FFT+IFFT

    for (int w = 0; w < N/2; w++){
      c.push_back(c2[w]);
    }
  }

  /*Técnica de preprocesado: inclusión VAD. Se utiliza el algoritmo programado en la P2
  para poner a 0 las partes de silencio (eliminar ruido de fondo), y así tener una señal
  más limpia sobre la cual buscar el pitch. De esta manera evitamos obtener el pitch de
  tramos en el que solamente hay ruido y que erróneamente son considerados como voiced
  por la función unvoiced(). Para una explicación más detallada consultar memoria de la P2 */
  void PitchAmpliacion::inclusionVad (vector<float> &x, int rate, float pot_media,
                    unsigned int n_len, unsigned int n_shift, int npitch_max) {
    int i = 0, counterU = 0, counterS = 0;

    //States: 0 -> silence, 1 -> voz, 2 -> undef, 3 -> init
    int official_last_state = 0, state = 3, last_state = 2;

    float potenciaVAD;
    vector<float>::iterator iX;

    for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift){ /* For each frame ... */
      potenciaVAD = getPower(iX, iX + n_len, n_len, npitch_max); //Obtenemos la potencia del tramo
      switch (state) {
      case 3: //Si state: init
        state = 0; //--> silence
        break;

      case 0: //Si state: silence
        if (potenciaVAD > (pot_media+15.2)) //y umbralVoz < potencia
          state = 1; //--> voz
        else if (potenciaVAD > (pot_media+8.7)) //y umbralSilencio < potencia < umbralVoz
          state = 2; //--> undef
        break;

      case 1: //Si state: voz
        if (potenciaVAD < (pot_media+8.7)) //y potencia < umbralSilencio
          state = 0; //--> silence
        else if (potenciaVAD < (pot_media+15.2)) //y umbtalSilencio < potencia < umbralVoz
          state = 2; //--> undef
        break;

      case 2: //Si state: undef
        if (potenciaVAD > (pot_media+15.2)) //y umbralVoz < potencia
          state = 1; //--> voz
        else if (potenciaVAD < (pot_media+8.7)) //y potencia < umbralSilencio
          state = 0; //--> silence
        break;
      }

      if(state == 1){ //voz
        if(last_state != state){
          if(official_last_state == 0){ //silencio
            if (counterS >=20) { // = (30ms + 19*15ms) = 315ms de silencio seguidos
              for (unsigned int i = 0; i < (n_shift)*counterS; i++){
                x[(iX-x.begin())-(n_shift*(counterS+counterU))+i] = 0.0; //se eliminan los silencios
              }
            }
            official_last_state = state;
          }
          counterU = 0;
          counterS = 0;
        }
      }
      else if (state == 0){ //silencio
        if((official_last_state == 1) && (counterS >= 20)){
          official_last_state = state;
        }
        if(last_state == 2){ //undef
          counterS = counterU + counterS;
          counterU = 0;
        }
        counterS++;
      }
      else counterU++;

      if (state != last_state) {
        last_state = state;
      }
      i++;
    }

    //Para eliminar el silencio del final
    for (unsigned int i = 0; i < (n_shift)*(counterS+counterU+1); i++)
      x[(iX-x.begin())-(n_shift*(counterS+counterU))+i] = 0.0;
  }

  //Para obtener la potencia de un tramo de señal de audio, utilizando la función de autocorrelación
  float PitchAmpliacion::getPower(vector<float>::const_iterator begin, vector<float>::const_iterator end,
                                  unsigned int frameLen, int npitch_max) {
    if (end-begin != frameLen)
      return -1.0F;

    std::vector<float> x(end-begin);
    std::copy(begin, end, x.begin());
    std::vector<float> r(npitch_max);

    //Se obtiene la autocorrelación de x y se almacena en r
    short K = r.size();
    short N = x.size();
    for (int k=0; k < K; k++) {
      r[k] = 0;
      for(int i=0; i < N-k; i++){
        r[k] += x[i]*x[i+k];
      }
    }

    if (r[0] == 0.0F)
      r[0] = 1e-10;
    return 10 * log10(r[0]); // = potencia de r (en dB)
  }
}
