#ifndef WAVREADER_H
#define WAVREADER_H

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

#endif
