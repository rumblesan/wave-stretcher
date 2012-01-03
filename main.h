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


