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

    f->forward = fftwf_plan_r2r_1d(f->window_size, f->data, f->data, FFTW_R2HC, FFTW_ESTIMATE);
    f->inverse = fftwf_plan_r2r_1d(f->window_size, f->data, f->data, FFTW_HC2R, FFTW_ESTIMATE);

    int i;
    for (i = 0; i < f->window_size; i++) {
        f->window[i] = 0.5 * (1.0 - cos(2 * M_PI * i / (f->window_size-1.0)) );
    }
}

void get_data(struct fft_data *f, float *input) {
    int i;
    for (i = 0; i < f->window_size; i++) {
        f->smps[i] = input[i];
    }
}

void return_data(struct fft_data *f, float *output) {
    int i;
    for (i = 0; i < f->window_size; i++) {
        output[i] = f->smps[i];
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

void samp_to_freq(struct fft_data *f) {

    window_data(f);

    int i;
    for (i = 0; i < f->window_size; i++) {
        f->data[i] = f->smps[i];
    }

    fftwf_execute(f->forward);

    float c,s;
    for (i = 0; i < f->window_size/2; i++ ) {
        c = f->data[i];
        s = f->data[f->window_size-i];
        f->freq[i] = sqrt(c*c+s*s);
    }
    f->freq[0] = 0.0;

}

void pauls_algo(struct fft_data *f) {

    int i;
    float inv_2p15_2pi=1.0/16384.0*M_PI;
    float phase;
    unsigned int rand;

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

}

void freq_to_samp(struct fft_data *f) {

    fftwf_execute(f->inverse);

    int i;
    for (i = 0; i < f->window_size; i++) {
        f->smps[i] = f->data[i];
    }

    normalise_data(f);
    window_data(f);

}

void cleanup_fft (struct fft_data *f) {

    fftwf_destroy_plan(f->forward);
    fftwf_destroy_plan(f->inverse);

    fftwf_free(f->window);

}

#ifdef TEST_FFT

void test_freq () {

    int window_size = 1024;
    struct fft_data fft;
    setup_fft(&fft, window_size);

    float testdata[window_size];
    float outdata[window_size];
    int i;
    for (i = 0; i < window_size; i++) {
        testdata[i]  = 0.1*(cos((10 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
        testdata[i] += 0.1*(cos((13 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
        testdata[i] += 0.1*(cos((7.35 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
        testdata[i] += 0.1*(cos((5.11 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
    }
    get_data(&fft, testdata);
    samp_to_freq(&fft);

    for (i = 0; i < window_size/2; i++ ) {
        printf("%.3f\n", fft.freq[i]);
    }

}

void test_normalise () {

    int window_size = 1024;
    struct fft_data fft;
    setup_fft(&fft, window_size);

    float testdata[window_size];
    float outdata[window_size];
    int i;
    for (i = 0; i < window_size; i++) {
        testdata[i]  = 0.1*(cos((10 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
        testdata[i] += 0.1*(cos((13 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
        testdata[i] += 0.1*(cos((7.35 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
        testdata[i] += 0.1*(cos((5.11 * 2 * M_PI * i / (window_size-1.0))+(0.5 * M_PI)) );
    }
    get_data(&fft, testdata);
    samp_to_freq(&fft);
    pauls_algo(&fft);
    freq_to_samp(&fft);

    return_data(&fft, outdata);
    for (i = 0; i < window_size; i++) {
        printf("%f    %f\n", testdata[i], outdata[i]);
    }

}

void main() {
    test_normalise();
}

#endif


