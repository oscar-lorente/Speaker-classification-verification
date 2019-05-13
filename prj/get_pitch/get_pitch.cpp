#include <iostream>
#include <fstream>
#include <cstring>
#include "wavfile_mono.h"
#include "pitch_analyzer.h"
#include "ampliacion.h"
// extern "C"{
//   #include "/home/oscar/Escritorio/3B/PAV/P3/prj/include/pav_analysis.h"
// }

#define FRAME_LEN 0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;
using namespace mejoras;

/**
   Main program:
   Arguments:
   - input (wav) file
   - output (txt) file with f0
     one value per line with the f0 for each frame
     (or 0 for unvoiced frames)
*/

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " input_file.wav output_file.f0\n";
    return -1;
  }

  //--------------------------------------------------------------------
  /*Para adaptar los valores de npitch_min y/o npitch_max en función de si el
  audio ha sido grabado por un hombre (50-260Hz aprox) o una mujer (100-500 aprox)*/
  int pitchMin = 50, pitchMax = 500;
  if(strstr(argv[1], "rl"))
    pitchMax = 260;
  else if(strstr(argv[1], "sb"))
    pitchMin = 100;
  //--------------------------------------------------------------------

  /// Read input sound file
  unsigned int rate;
  vector<float> x;
  int retv = readwav_mono(argv[1], rate, x);
  if (retv != 0) {
    cerr << "Error reading input file: %d\n"
	 << "Error value: " << retv << endl;
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;

  ///Define analyzer
  PitchAnalyzer analyzer(n_len, n_shift, rate, PitchAnalyzer::HAMMING, pitchMin, pitchMax);
  /*Se define un objeto de la clase PitchAmpliacion, en la cual se han programado
  las funciones correspondientes a los métodos de pre/post procesado utilizados*/
  PitchAmpliacion analyzerAmpliacion(n_len, n_shift);

  //Para aplicar un filtro de mediana --> post-procesado
  bool medianFilter = false;
  //Para aplicar centerClipping --> preprocesado
  bool centerClipping = false;
  //Para utilizar un método basado en el cepstrum de la señal para encontrar el pitch
  bool cepstrum = false;
  //Para poner a 0 las partes de silencio  --> eliminar el ruido de fondo --> preprocesado
  bool inclusionVad = true;

  vector<float>::iterator iX = x.begin();

  //--------------------------------------------------------------------
  /*Para obtener la potencia media de los primeros 150ms de la señal de audio,
  correspondientes al primer tramo de silencio --> potencia del ruido de fondo*/
  float pot_media = 0.0;
  int counterAux = 0, j;
  /*Teniendo en cuenta que cada frame es de 30ms (FRAME_LEN) y que se va desplazando
  de 15 en 15ms (FRAME_SHIFT), se habrán analizado los primeros 150ms cuando se llegue
  a los (30ms + 8*15ms) --> n_len + 8*n_shift*/
    for (iX = x.begin(); (iX + n_len)-x.begin() <= n_len+8*n_shift; iX = iX + n_shift) {
    j = 0;
    //Enventanado
    for (unsigned int i = iX-x.begin(); i<(iX + n_len)-x.begin(); ++i){
      x[i] *= analyzer.window[j];
      j++;
    }
    pot_media += analyzerAmpliacion.getPower(iX, iX + n_len, n_len, pitchMax);
    counterAux += 1;
  }
  pot_media = pot_media / (float) counterAux;
  //--------------------------------------------------------------------

  /*Preprocesado --> Utilizando el algoritmo creado en la P2 (VAD), poner a 0 las
  partes de silencio --> eliminar el ruido de fondo*/
  if(inclusionVad)
    analyzerAmpliacion.inclusionVad(x, rate, pot_media, n_len, n_shift, pitchMax);

  vector<float> f0;
  float f = 0.0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    f = analyzer(iX, iX + n_len, pot_media, centerClipping, cepstrum, inclusionVad, x.begin());
    f0.push_back(f);
  }

  /*Post-procesado --> Filtro de mediana (con una ventana de tamaño 5) aplicado al
  vector f0, que contiene el pitch de cada tramo de señal*/
  if(medianFilter)
    analyzerAmpliacion.medianFilter(f0, 5);

  ///Write f0 contour into the output file
  ofstream os(argv[2]);
  if (!os.good()) {
    cerr << "Error opening output file " << argv[2] << endl;
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX)
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
