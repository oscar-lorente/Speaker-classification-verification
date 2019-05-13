#ifndef  PAV_ANALYSIS_H
#define  PAV_ANALYSIS_H
void foo();
float  compute_power(const  float *x, unsigned  int N);
float  compute_am(const  float *x, unsigned  int N);
float  compute_zcr(const  float *x, unsigned  int N);
void postProcesado(float *iX, float *f0, float pot_media, int N, int rate);
#endif
