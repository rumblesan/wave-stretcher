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

AudioFile read_wav(char *filename);
void write_wav(AudioFile af);
void cleanup_wav(AudioFile af);

#endif
