#ifndef WAVREADER_H
#define WAVREADER_H

#include <sndfile.h>

typedef struct audio_file *AudioFile;

typedef struct audio_file {
    SNDFILE *sf;
    char *filename;
    SF_INFO info;
    float *sound_data;
} AudioFile_Data;

void read_wav(struct audio_file *af);
void write_wav(struct audio_file *af);
void print_wav(struct audio_file *af);

#endif
