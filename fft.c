#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include "main.h"


void setup_fft (struct fft_data *d, int N) {

    d->num_samples = N;

    d->window = (float*) malloc(N);


    d->data    = (float*) fftwf_malloc(sizeof(float) * N);
    d->forward = fftwf_plan_r2r_1d(N, d->data, d->data, FFTW_FORWARD,  FFTW_ESTIMATE);
    d->inverse = fftwf_plan_r2r_1d(N, d->data, d->data, FFTW_BACKWARD, FFTW_ESTIMATE);


    int i;
    for (i = 0; i < N; i++) {
        d->window[i] = 0.5 * (1.0 - cos(2 * M_PI * i / (N-1.0)) );
    }
}

/*

void forward_fft (struct fft_data *d) {
    fftw_execute(d->forward);
}

void inverse_fft (struct fft_data *d) {
    fftw_execute(d->inverse);
}

void cleanup_fft (struct fft_data *d) {

    fftw_destroy_plan(d->forward);
    fftw_destroy_plan(d->inverse);

    fftw_free(d->data);

}
*/


