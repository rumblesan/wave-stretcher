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



