#include <fftw3.h>
#include <stdio.h>
#include <sndfile.h>

struct input_args {
    char  *input_file;
    char  *output_file;
    int   window_size;
    float speed_ratio;
};


struct audio_file {
    SNDFILE *sf;
    char *filename;
    SF_INFO info;
    float *sound_data;
};

void read_wav(struct audio_file *af);
void write_wav(struct audio_file *af);
void print_wav(struct audio_file *af);



struct stretch_data {

    float *input_data;
    int    input_frames;
    float  input_offset;


    float *output_data;
    int    output_frames;
    int    output_offset;

    float ratio;

    int channels;

    int finished;

    int window_size;

    float *buffers[];
};

void setup_stretch(struct stretch_data *s,
                   float *wavdata,
                   int frames,
                   int channels,
                   int window_size,
                   float ratio);

void next_input_section(struct stretch_data *s);
void add_output(struct stretch_data *s);

#ifdef STRETCH_TEST
void print_next_section(struct stretch_data *s);
void print_output(struct stretch_data *s);
void test_next_input_section();
void test_add_output();
#endif



struct fft_data {

    int window_size;
    int rand_seed;

    float *window;

    float *smps;
    float *data;
    float *freq;

    fftwf_plan forward, inverse;

};

void setup_fft (struct fft_data *d, int window_size);
void get_data(struct fft_data *f, float *data);
void return_data(struct fft_data *f, float *data);
void window_data(struct fft_data *f);
void normalise_data(struct fft_data *f);
void freq_to_samp(struct fft_data *f);
void samp_to_freq(struct fft_data *f);
void cleanup_fft(struct fft_data *f);



