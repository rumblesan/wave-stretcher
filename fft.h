#ifndef FFT_H
#define FFT_H

#include <fftw3.h>
#include "sample.h"

typedef struct fft_data *FFT;

typedef struct fft_data {

    int window_size;
    int rand_seed;

    float *window;

    float *smps;
    float *data;
    float *freq;

    fftwf_plan forward, inverse;

} FFT_Data;

FFT fft_create (int window_size);
void fft_run(FFT f, Samples smps);
void fft_window_data(FFT f);
void fft_normalise_data(FFT f);
void fft_samp_to_freq(FFT f);
void fft_pauls_algo(FFT f);
void fft_freq_to_samp(FFT f);
void fft_cleanup(FFT f);

#endif
