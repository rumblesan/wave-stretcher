#ifndef FFT_H
#define FFT_H

#include <fftw3.h>
#include "audiodata.h"

typedef struct fft_data *FFT;

typedef struct fft_data {

    int window_size;
    int rand_seed;

    float *window;

    Samples sample_buffer;
    float *smps;
    float *data;
    float *freq;

    fftwf_plan forward, inverse;

} FFT_Data;

FFT create_FFT (int window_size);
void get_data(FFT f, Samples smps);
Samples return_data(FFT f);
void run_fft(FFT f);
void window_data(FFT f);
void normalise_data(FFT f);
void samp_to_freq(FFT f);
void pauls_algo(FFT f);
void freq_to_samp(FFT f);
void cleanup_fft(FFT f);

#endif
