#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <string.h>
#include "vad.h"
#include "pav_analysis.h"


#define DEBUG_VAD 0x1
  float  compute_power(const  float *x, unsigned  int N) {
    float sumP = 0;
    for (int i = 0; i < N; i++) {
      sumP=sumP+(*(x+i))*(*(x+i));
    }
    return 10*log10((1./N)*sumP);
  }

  float compute_am(const float *x, unsigned int N){
    float sumA = 0;
    for (int i = 0; i < N; i++) {
      sumA = sumA + fabs(*(x+i));
    }
    return (1./N)*sumA;
  }

  float compute_zcr(const float *x, unsigned int N){
    float sumT = 0;
    for (int i = 1; i <= N; i++) {
      if(*(x+i-1) > 0){
        if(*(x+i) < 0){
          sumT += 1;
        }
      }
      else if(*(x+i-1) < 0){
        if(*(x+i) > 0){
          sumT += 1;
        }
      }
    }
     return (1./(2*(N-1)))*sumT;
  }
