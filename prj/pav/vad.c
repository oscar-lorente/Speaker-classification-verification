#include <math.h>
#include "vad.h"
#include <stdlib.h>
#include <stdio.h>
#include "pav_analysis.h" //path de pav_analysis.h

const float FRAME_TIME = 10.0F; /* in ms. */

/*
   As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
   only this labels are needed. You need to add all labels, in case
   you want to print the internal state in string format */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

Features compute_features(const float *x, int N) {
  Features feat;
  feat.p = compute_power(x, N);
  feat.am = compute_am(x, N);
  feat.zcr = compute_zcr(x, N);
  return feat;
}


/*
   TODO: Init the values of vad_data
 */


VAD_DATA * vad_open(float rate) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  VAD_STATE state = ST_SILENCE;
  /* TODO: decide what to do with the last undecided frames */

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}



/*
   TODO: Implement the Voice Activity Detection
   using a Finite State Automata
*/

VAD_STATE vad(VAD_DATA *vad_data, float *x, float pot_media) {

  /* TODO
     You can change this, using your own features,
     program finite state automaton, define conditions, etc.
  */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state) {
  case ST_INIT:
    vad_data->state = ST_SILENCE;
    break;

  case ST_SILENCE:
    if (f.p > (pot_media+15.2))
      vad_data->state = ST_VOICE;
    else if (f.p > (pot_media+8.7))
      vad_data->state = ST_UNDEF;
    break;

  case ST_VOICE:
    if (f.p < (pot_media+8.7))
      vad_data->state = ST_SILENCE;
    else if (f.p < (pot_media+15.2))
      vad_data->state = ST_UNDEF;
    break;

  case ST_UNDEF:
    if (f.p > (pot_media+15.2))
      vad_data->state = ST_VOICE;
    else if (f.p < (pot_media+8.7))
      vad_data->state = ST_SILENCE;
    break;
  }

  return vad_data->state;
}


void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
