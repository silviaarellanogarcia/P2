#include <math.h>
#include <stdlib.h> //los que están entre <> son includes propios dell sistema
#include <stdio.h> 

#include "pav_analysis.h"
#include "vad.h"

const float FRAME_TIME = 10.0F; /* in ms. */

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT", "MBV", "MBS"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) {
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  feat.zcr = compute_zcr(x,N,16000);
  feat.p = compute_power(x,N);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate, float alpha0, float alpha1, int frames) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->alpha0 = alpha0;
  vad_data->alpha1 = alpha1;
  vad_data->num_total_frame = frames;
  vad_data->num_frame = 0;
  vad_data->pot = 0.0;
  vad_data->zcr1 = 0.0;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x) {
  /* Se llama solo una vez por trama a esta función
  x es la señal.
  f.p = feature de potencia
  Como vad_data es un puntero, para acceder a su contenido hacemos ->
  */

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */ //Guardamos aquí la potencia
  

  switch (vad_data->state) { //*** Autómata ***
    case ST_INIT:
      if (vad_data->num_frame < vad_data->num_total_frame) {
        vad_data->pot += pow(10, f.p/10);
        vad_data->num_frame++;
        vad_data->zcr1 += f.zcr;
      } else {
        vad_data->state = ST_SILENCE;
        vad_data->pot = 10*log10(vad_data->pot/vad_data->num_total_frame);
        vad_data->p1 = vad_data->pot + vad_data->alpha1; //p1 será alpha1 dBs más que el nivel de potencia que tenemos
        vad_data->p0 = vad_data->pot + vad_data->alpha0; 
        vad_data->zcr1 = vad_data->zcr1/vad_data->num_total_frame;
      }
      break;

    case ST_SILENCE:
      if (f.p > vad_data->p1)
        vad_data->state = ST_VOICE;
      else if (f.p > vad_data->p0)
        vad_data->state = ST_MB_VOICE;
      break;

    case ST_VOICE:
      if (f.p < vad_data->p0 && f.zcr < vad_data->zcr1)
        vad_data->state = ST_SILENCE;
      else if (f.p < vad_data->p1)
        vad_data->state = ST_MB_SILENCE;
      break;

    case ST_MB_SILENCE:
      if (f.p > vad_data->p1)
        vad_data->state = ST_VOICE;
      else if (f.p < vad_data->p0)
        vad_data->state = ST_SILENCE;
      break;

    case ST_MB_VOICE:
      if (f.p > vad_data->p1)
        vad_data->state = ST_VOICE;
      else if (f.p < vad_data->p0)
        vad_data->state = ST_SILENCE;
      break;

    case ST_UNDEF:
      break;
  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE ||
      vad_data->state == ST_MB_SILENCE ||
      vad_data->state == ST_MB_VOICE)
    return vad_data->state;
  if (vad_data->state == ST_INIT)
    return ST_SILENCE;
  return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
