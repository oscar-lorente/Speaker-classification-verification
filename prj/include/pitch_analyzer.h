#ifndef PITCH_ANALYZER_H
#define PITCH_ANALYZER_H

#include <vector>
#include <algorithm>

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

  private:

    std::vector<float> window; ///precomputed window
    unsigned int frameLen, ///length of frame (ins samples). Has to be set in the constructor call
      samplingFreq, ///sampling rate (ins samples). Has to be set in the constructor call
      npitch_min, /// min. value of pitch period, in samples
      npitch_max; /// max. value of pitch period, in samples

    void autocorrelation(const std::vector<float> &x, std::vector<float> &r) const; ///compute correlation from lag=0 to r.size()
    float compute_pitch(std::vector<float> & x) const; ///Returns the pitch (in Hz) of input frame x

    bool unvoiced(float pot, float r1norm, float rmaxnorm) const; ///true is the frame is unvoiced


  public:
    /**
       Declaration of the PitchAnalyzer:
       fLen: frame length, in samples
       sFreq: sampling rate (samples/second)
       w: window type
       min_F0, max_F0: the pitch range can be restricted to this range
    */
    PitchAnalyzer(unsigned int fLen, unsigned int sFreq, Window w=PitchAnalyzer::HAMMING,
                  float min_F0 = MIN_F0, float max_F0 = MAX_F0) {
      frameLen = fLen;
      samplingFreq = sFreq;
      set_f0_range(min_F0, max_F0);
      set_window(w);
    }

    /**
       Operator () : compute the pitch for the given vector x
    */
    float operator()(const std::vector<float> & _x) const {
      if (_x.size() != frameLen)
        return -1.0F;

      std::vector<float> x(_x); //local copy of input frame
      return compute_pitch(x);
    }

    /**
       Operator () : compute the pitch for the given "C" vector (float *).
       N is the size of the vector pointer by pt.
    */
    float operator()(const float * pt, unsigned int N) const {
      if (N != frameLen)
        return -1.0F;

      std::vector<float> x(N); //local copy of input frame, size N
      std::copy(pt, pt+N, x.begin()); ///copy input values into local vector x
      return compute_pitch(x);
    }

    /**
       Operator () : compute the pitch for the given vector, expressed by the begin and end iterators
    */
    float operator()(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end) const {

      if (end-begin != frameLen)
        return -1.0F;

      std::vector<float> x(end-begin); //local copy of input frame, size N
      std::copy(begin, end, x.begin()); ///copy input values into local vector x
      return compute_pitch(x);
    }
    /**
       Set pitch range; (input in Hz, npitch_min and npitch_max, in samples
    */
    void set_f0_range(float min_F0, float max_F0);
  };
}
#endif
