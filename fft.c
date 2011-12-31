#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include "main.h"


void setup_fft (struct fft_data *f, int window_size) {

    f->window_size = window_size;
    f->rand_seed   = 1;
    f->rand_seed  += 161103;


    f->window = (float*) malloc(sizeof(float) * f->window_size);

    f->smps    = (float*) fftwf_malloc(sizeof(float) * f->window_size);
    f->data    = (float*) fftwf_malloc(sizeof(float) * f->window_size);
    f->freq    = (float*) fftwf_malloc(sizeof(float) * f->window_size / 2+1);

    f->forward = fftwf_plan_r2r_1d(f->window_size, f->smps, f->data, FFTW_FORWARD,  FFTW_ESTIMATE);
    f->inverse = fftwf_plan_r2r_1d(f->window_size, f->freq, f->data, FFTW_BACKWARD, FFTW_ESTIMATE);

    int i;
    for (i = 0; i < f->window_size; i++) {
        f->window[i] = 0.5 * (1.0 - cos(2 * M_PI * i / (f->window_size-1.0)) );
    }
}

void get_data(struct fft_data *f, float *data) {
    int i;
    for (i = 0; i < f->window_size; i++) {
        f->smps[i] = data[i];
    }
}

void return_data(struct fft_data *f, float *data) {
    int i;
    for (i = 0; i < f->window_size; i++) {
        data[i] = f->smps[i];
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

void freq_to_samp(struct fft_data *f) {

    float inv_2p15_2pi=1.0/16384.0*M_PI;
    float phase;
    unsigned int rand;
    int i;
    for (i = 0; i < f->window_size/2; i++ ) {
        f->rand_seed = (f->rand_seed * 1103515245+12345);
        rand=(f->rand_seed>>16)&0x7fff;
        phase = rand * inv_2p15_2pi;
        f->data[i] = f->freq[i]*sin(phase);
        f->data[f->window_size-i] = f->freq[i]*cos(phase);
    }
    f->data[0] = 0.0;
    f->data[f->window_size/2+1] = 0.0;
    f->data[f->window_size/2] = 0.0;

    fftwf_execute(f->inverse);
    //normalise_data(f);
    window_data(f);
}

void samp_to_freq(struct fft_data *f) {

    window_data(f);
    fftwf_execute(f->forward);

    int i;
    float c,s;
    for (i = 0; i < f->window_size/2; i++ ) {
        c = f->data[i];
        s = f->data[f->window_size-i];
        f->freq[i] = sqrt(c*c+s*s);
    }
    f->freq[0] = 0.0;
}

void cleanup_fft (struct fft_data *f) {

    fftwf_destroy_plan(f->forward);
    fftwf_destroy_plan(f->inverse);

    fftwf_free(f->window);

}


