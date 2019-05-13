#include <iostream>
#include <cmath>

#include "ffft/FFTReal.h"

#define N 32

using namespace std;

int main() {
  ffft::FFTReal <float> fft_object (N);

  float x[N], X[N];
  int i, sgn = 1;

  float eps = 1e-20;

  //Create test signal
  for (i=0; i< 20; i++) {
    x[i] = sgn * i;
    sgn = -sgn;
  }
  //Zero padding
  for (; i< N; i++) x[i] = 0;

  fft_object.do_fft (X, x);     // x (real) --FFT---> f (complex)


  for (i=0; i< N; i++)
    cout << "x[" << i << "]= " << x[i] << '\n';

  //Show |X(f)|db

  /*
   FFTReal output | Positive FFT equiv.   | Negative FFT equiv.
   ---------------+-----------------------+-----------------------
   X [0]          | Real (bin 0)          | Real (bin 0)
   X [...]        | Real (bin ...)        | Real (bin ...)
   X [length/2]   | Real (bin length/2)   | Real (bin length/2)
   X [length/2+1] | Imag (bin 1)          | -Imag (bin 1)
   X [...]        | Imag (bin ...)        | -Imag (bin ...)
   X [length-1]   | Imag (bin length/2-1) | -Imag (bin length/2-1)
  */

  cout << "S[0]=\t" << 10*log10(X[0] * X[0] + eps) << " dB\n";
  for (i=1; i<N/2-1; i++) {
    float re = X[i];
    float im = X[N/2+i];
    cout << "S[" << i << "]=\t" << 10*log10(re*re+im*im + eps) << "dB \n";
  }
  cout << "S[" << N/2 << "]=\t" << 10*log10(X[N/2]*X[N/2] + eps) << " dB\n\n";


  fft_object.do_ifft (X, x);    // f (complex) --IFFT--> x (real)
  fft_object.rescale (x);       // Post-scaling should be done after FFT+IFFT

  for (i=0; i< N; i++)
    cout << "x[" << i << "]= " << x[i] << '\n';
  cout << "----------------------------\n";


}
