#ifndef AMPLIACION_H
#define AMPLIACION_H

#include <vector>

using namespace std;

namespace mejoras{
  class PitchAmpliacion{
    public:
      void medianFilter(std::vector<float> &f0, int windowSize);
      void centerClipping(float &x, float umbral);
      void cepstrum(std::vector<float> &x, std::vector<float> &c);
      void inclusionVad (std::vector<float> &x, int rate, float pot_media,
                        unsigned int n_len, unsigned int n_shift, int npitch_max);
      float getPower(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end,
                    unsigned int frameLen, int npitch_max);

    private:
      unsigned int n_len, n_shift;

    public:
      PitchAmpliacion(unsigned int frameLen, unsigned int frameShift) {
        n_len = frameLen;
        n_shift = frameShift;
      }
  };
}
#endif
