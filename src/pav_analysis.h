#ifndef PAV_ANALYSIS_H
#define PAV_ANALYSIS_H

float compute_power(const float *x, unsigned int N);
float compute_am(const float *x, unsigned int N);
float compute_zcr(const float *x, unsigned int N, float fm);

// Ampliación
float compute_window_power(const float *x, unsigned int N);
float hamming_window(int n, int L);

#endif	/* PAV_ANALYSIS_H	*/
