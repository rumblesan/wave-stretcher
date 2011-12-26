#include <fftw3.h>
#include <math.h>
#include <stdlib.h>

struct fft_data {

    int num_samples;

    float *window;

    float *in, *out;
    fftwf_plan forward, inverse;

};


void setup_fft (struct fft_data *d, int N) {

    d->num_samples = N;

    d->window = (float*) malloc(N);


    d->in      = (float*) fftwf_malloc(sizeof(float) * N);
    d->out     = (float*) fftwf_malloc(sizeof(float) * N);
    d->forward = fftwf_plan_r2r_1d(N, d->in, d->out, FFTW_FORWARD,  FFTW_ESTIMATE);
    d->inverse = fftwf_plan_r2r_1d(N, d->in, d->out, FFTW_BACKWARD, FFTW_ESTIMATE);


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

    fftw_free(d->in);
    fftw_free(d->out);

}
*/


