
#ifndef FFT_H
#define FFT_H

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

FFT create_FFT (int window_size);
void get_data(FFT f, float *input);
void return_data(FFT f, float *output);
void window_data(FFT f);
void normalise_data(FFT f);
void freq_to_samp(FFT f);
void samp_to_freq(FFT f);
void cleanup_fft(FFT f);

#endif

