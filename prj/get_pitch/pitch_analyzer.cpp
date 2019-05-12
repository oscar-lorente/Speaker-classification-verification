#include <iostream>
#include <fstream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "pitch_analyzer.h"
#include "ampliacion.h"
#include "pav_analysis.h"
using namespace std;
using namespace mejoras;

namespace upc {
  void PitchAnalyzer::autocorrelation(const vector<float> &x, vector<float> &r) const {
    //Se obtiene la autocorrelación de x y se almacena en r
    short K = r.size();
    short N = x.size();
    for (int k=0; k < K; k++) {
      r[k] = 0;
      for(int i=0; i < N-k; i++){
        r[k] += x[i]*x[i+k];
      }
    }

    if (r[0] == 0.0F) //to avoid log() and divide zero
      r[0] = 1e-10;
  }

  void PitchAnalyzer::set_window(Window win_type) {
    if (frameLen == 0)
      return;

    window.resize(frameLen);

    switch (win_type) {
    case HAMMING:
      {
      //Definición de ventana Hamming
      for (unsigned int i = 0; i < frameLen; i++)
        window[i] = 0.54 - 0.46 * cos((2*M_PI*i)/(frameLen-1));
      }
      break;
    case RECT:
      window.assign(frameLen, 1);
    default:
      window.assign(frameLen, 1);
    }
  }

  void PitchAnalyzer::set_f0_range(float min_F0, float max_F0) {
    npitch_min = (unsigned int) samplingFreq/max_F0;
    if (npitch_min < 2)
      npitch_min = 2;  // samplingFreq/2

    npitch_max = 1 + (unsigned int) samplingFreq/min_F0;

    //frameLen should include at least 2*T0
    if (npitch_max > frameLen/2)
      npitch_max = frameLen/2;
  }

  //Para determinar si el tramo de señal de audio es unvoiced o no
  bool PitchAnalyzer::unvoiced(float pot, float r1norm, float rmaxnorm, float pot_media, bool inclusionVad,
                               bool cepstrum, vector<float> &c, unsigned int lag) const {
    /*Si se ha implementado la mejora de "inclusionVad", las partes de silencio (correspondientes a
    ruido de fondo) han sido eliminadas, puestas a 0, por lo que los umbrales utilizados para determinar
    si el tramo es o no unvoiced son modificados, explicado con detalle en la memoria*/
    if(inclusionVad)
      return ((r1norm < 0.94) && (rmaxnorm < 0.5));
    else
    /*Si la potencia del tramo que se está analizando es inferior a la potencia media del ruido de fondo + 12
    (valor escogido empíricamente) se considera ruido --> se devuelve unvoiced --> pitch = 0. Si por otro lado,
    la potencia es superior al umbral expuesto, y se cumplen las demás condiciones impuestas respecto a r[1]/r[0]
    y r[max]/r[0] --> unvoiced --> pitch = 0*/
      return (((pot > (pot_media+12)) && (r1norm < 0.95) && (rmaxnorm < 0.5)) || (pot < (pot_media+12)));
  }

  float PitchAnalyzer::compute_pitch(vector<float> & x, float pot_media, bool centerClipping,
                                  bool cepstrum, bool inclusionVad, bool flag) const {
    if (x.size() != frameLen)
      return -1.0F;

    /*Objeto de la clase PitchAmpliacion, utilizado para implementar mejoras como el
    centerClipping o cepstrum*/
    PitchAmpliacion analyzerAmpliacion(frameLen, samplingFreq*0.015);

    vector<float>::const_iterator iR , iRMax, iRBegin, iREnd;
    vector<float> r(npitch_max);
    vector<float> c; //c --> cepstrum de x

    /*Si el tramo que se está analizando se corresponde con un t >= 150ms, se aplica el
    enventanado --> véase función operator() de pitch_analyzer.h*/
    if(flag){
      for (unsigned int i=0; i<x.size(); ++i){
        x[i] *= window[i];
      }
    }

    autocorrelation(x, r);
    float rAux[r.size()];
    for(int n = 0; n < r.size(); n++){
      rAux[n] = r[n];
      if(centerClipping){
        analyzerAmpliacion.centerClipping(r[n], rAux[0]*0.2); //Preprocesado --> centerClipping
      }
    }

    /*Si el booleano cepstrum = true, se implementa el detector de pitch basado en el cepstrum*/
    if(cepstrum){
      analyzerAmpliacion.cepstrum(x, c);
      iR = c.begin();
      iRMax = iR;
      iRBegin = iR;
      iREnd = c.end();
    }
    else{
      iR = r.begin();
      iRMax = iR;
      iRBegin = iR;
      iREnd = r.end();
    }

    /*Búsqueda del máximo de r (correspondiente a la autocorrelación de x) o de c (correspondiente
    al cepstrum de c), según si el booleano cepstrum es false o true, respectivamente*/
    bool negativo = false; //Para controlar que se haya pasado por un valor de r/c negativo
    bool rango = false; //Para controlar el rango: npitch_min < pitch < npitch_max

    for (iR; iR != iREnd; iR ++) {
      //Si el valor de r/c es negativo
      if(!negativo && (*iR < 0)){
        negativo=true;
        //Se asigna el valor actual de r/c (negativo) a *iRMax, para asegurar que se encuentre un máximo
        iRMax = iR;
      }
      //Si estamos dentro del rango (npitch_min < pitch < npitch_max) --> rango = true
      if(!rango && ((iR-iRBegin) > npitch_min)){
        rango=true;
      }
      else if (rango && ((iR-iRBegin) >= npitch_max)){
        break;
      }
      //Si se cumplen ambas condiciones (rango y negativo) y el valor de r/c es superior al máximo actual
      if (rango && negativo && (*iR > *iRMax)){
        iRMax = iR;
      }
    }

    unsigned int lag = 0;
    if (iRMax != iREnd) //normal case
      lag = iRMax - iRBegin;

    float pot = 10 * log10(rAux[0]);

    if (unvoiced(pot, rAux[1]/rAux[0], rAux[lag]/rAux[0], pot_media, inclusionVad, cepstrum, c, lag) or lag == 0)
      return 0;
    else
      return (float) samplingFreq/(float) lag;
  }
}
