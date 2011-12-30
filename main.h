#include <fftw3.h>
#include <stdio.h>
#include <sndfile.h>

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

    int finished;

    float ratio;

    int channels;

    int window_size;

    float *buffers[];
};

void setup_stretch(struct stretch_data *s,
                   float *wavdata,
                   int input_size,
                   int window_size,
                   int channels,
                   float ratio);

void next_input_section(struct stretch_data *s);
void add_output(struct stretch_data *s);

#ifdef TEST_SUITE
void print_next_section(struct stretch_data *s);
void print_output(struct stretch_data *s);
void test_next_input_section();
void test_add_output()
#endif



struct fft_data {

    int window_size;

    float *window;

    float *smps;
    float *data;
    float *freq;

    fftwf_plan forward, inverse;

};

void setup_fft (struct fft_data *d, int window_size);
void copy_data(struct fft_data *f, short *data);
void return_data(struct fft_data *f, short *data);
void window_data(struct fft_data *f);
void normalise_data(struct fft_data *f);
void forward_fft(struct fft_data *f);
void inverse_fft(struct fft_data *f);
void pauls_algo(struct fft_data *f);
void cleanup_fft(struct fft_data *f);



