#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include "main.h"


void setup_fft (struct fft_data *f, int window_size) {

    f->window_size = window_size;

    f->window = (float*) malloc(sizeof(float) * f->window_size);

    f->smps    = (float*) fftwf_malloc(sizeof(float) * f->window_size);
    f->freq    = (float*) fftwf_malloc(sizeof(float) * f->window_size);

    f->forward = fftwf_plan_r2r_1d(f->window_size, f->smps, f->freq, FFTW_FORWARD,  FFTW_ESTIMATE);
    f->inverse = fftwf_plan_r2r_1d(f->window_size, f->freq, f->smps, FFTW_BACKWARD, FFTW_ESTIMATE);


    int i;
    for (i = 0; i < f->window_size; i++) {
        f->window[i] = 0.5 * (1.0 - cos(2 * M_PI * i / (f->window_size-1.0)) );
    }
}

void get_data(struct fft_data *f, short *data) {
    int i;
    float scale = (1.0/32768.0);
    for (i = 0; i < f->window_size; i++) {
        f->smps[i] = ((float)data[i] * scale);
    }
}

void return_data(struct fft_data *f, short *data) {
    int i;
    for (i = 0; i < f->window_size; i++) {
        data[i] = (short)(f->smps[i] * 32768.0);
    }
}

void window_data(struct fft_data *f) {
    int i;
    for (i = 0; i < f->window_size; i++) {
        f->smps[i] *= f->window[i];
    }
}

void normalise_data(struct fft_data *f) {
    int i;
    for (i = 0; i < f->window_size; i++) {
        f->smps[i] /= f->window_size;
    }
}

void forward_fft (struct fft_data *f) {
    window_data(f);
    fftwf_execute(f->forward);
}

void inverse_fft (struct fft_data *f) {
    fftwf_execute(f->inverse);
    normalise_data(f);
    window_data(f);
}

void pauls_algo(struct fft_data *f) {
}

void cleanup_fft (struct fft_data *f) {

    fftwf_destroy_plan(f->forward);
    fftwf_destroy_plan(f->inverse);

    fftwf_free(f->window);

}


