#ifndef PITCH_ANALYZER_H
#define PITCH_ANALYZER_H

#include <vector>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <iostream>
#include "ampliacion.h"
#include "FFTReal.h"
using namespace std;

namespace upc {
  const float MIN_F0=20.0F;
  const float MAX_F0=10000.0F;

  /*!
    PitchAnalyzer: class that computes the pitch (in Hz) from a signal frame.
    No pre-processing or post-processing has been included
  */

  class PitchAnalyzer {
  public:
    enum Window {RECT, HAMMING}; ///Window type
    void set_window(Window type); ///pre-compute window
    void autocorrelation(const std::vector<float> &x, std::vector<float> &r) const; ///compute correlation from lag=0 to r.size()
    std::vector<float> window; ///precomputed window
  private:
    unsigned int frameLen, ///length of frame (ins samples). Has to be set in the constructor call
      frameShift,
      samplingFreq, ///sampling rate (ins samples). Has to be set in the constructor call
      npitch_min, /// min. value of pitch period, in samples
      npitch_max; /// max. value of pitch period, in samples

    float compute_pitch(std::vector<float> & x, float pot_media, bool centerClipping,
                    bool cepstrum, bool inclusionVad, bool flag) const; ///Returns the pitch (in Hz) of input frame x
    bool unvoiced(float pot, float r1norm, float rmaxnorm, float pot_media, bool inclusionVad,
                                 bool cepstrum, std::vector<float> &c, unsigned int lag) const; ///true if the frame is unvoiced

  public:
    /**
       Declaration of the PitchAnalyzer:
       fLen: frame length, in samples
       sFreq: sampling rate (samples/second)
       w: window type
       min_F0, max_F0: the pitch range can be restricted to this range
    */
    PitchAnalyzer(unsigned int fLen, unsigned int shift, unsigned int sFreq, Window w=PitchAnalyzer::HAMMING,
                  float min_F0 = MIN_F0, float max_F0 = MAX_F0) {
      frameLen = fLen;
      frameShift = shift;
      samplingFreq = sFreq;
      set_f0_range(min_F0, max_F0);
      set_window(w);
    }
    /**
       Operator () : compute the pitch for the given vector, expressed by the begin and end iterators
    */
    float operator()(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end,
                float pot_media, bool centerClipping, bool cepstrum, bool inclusionVad, std::vector<float>::const_iterator x_begin) const {

      if (end-begin != frameLen)
        return -1.0F;

      /*Flag que se activa cuando se llega a la muestra frameLen+8*frameShift de la señal,
      correspondiente al momento t=150ms, para controlar (en la función compute_pitch)
      que no se aplique otra vez el enventanado + centerClipping (opcional) que ya se había
      aplicado a los primeros 150ms de la señal de audio cuando se calculó la potencia media
      en get_pitch.cpp*/
      bool flag = false;

      if(end-x_begin > frameLen+8*frameShift)
        flag = true;

      std::vector<float> x(end-begin); //local copy of input frame, size N
      std::copy(begin, end, x.begin()); ///copy input values into local vector x
      return compute_pitch(x, pot_media, centerClipping, cepstrum, inclusionVad, flag);
    }

    /**
       Set pitch range; (input in Hz, npitch_min and npitch_max, in samples
    */
    void set_f0_range(float min_F0, float max_F0);
  };
}
#endif
