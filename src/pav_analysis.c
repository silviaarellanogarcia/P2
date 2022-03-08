#include <math.h>
#include "pav_analysis.h"

float compute_power(const float *x, unsigned int N) {
    float sqr_sum = 1e-12;
    int i;
    for(i = 0; i < N; i++) {
    	sqr_sum += x[i]*x[i];
    }
    return 10*log10(sqr_sum/N);
}

float compute_window_power(const float *x, unsigned int N) {
    float sqr_sum_num = 1e-12, sqr_win = 1e-12, w;
    int i;
    for (i = 0; i < N; i++) {
        w = hamming_window(i, N);
        sqr_sum_num += pow(x[i]*w,2);
        sqr_win += w*w;
    }

    return 10*log10(sqr_sum_num/sqr_win);
}

float compute_am(const float *x, unsigned int N) {
    float sum = 0.0;
    int i;
    for(i = 0; i < N; i++) {
    	sum += fabs(x[i]);
    }
    return sum/N;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
	int i;
    float sum = 0.0;
    for(i = 1; i < N; i++) {
    	if(x[i]*x[i-1] < 0)
    		sum += 1;
    }
    return fm*sum/(2*(N-1));
}

float hamming_window(int n, int L) {
    if(0<=n && n<=L-1){
        return(0.54-0.46*cos(2*M_PI*n/L)); //Fórmula para calcular la ventana de Hamming
    }
    return 0;
}
