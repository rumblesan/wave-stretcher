#ifndef AUDIODATA_H
#define AUDIODATA_H

#include <sndfile.h>

typedef struct audio_file *AudioFile;

typedef struct audio_file {
    SNDFILE *sf;
    char *filename;
    SF_INFO info;
    int finished;
} AudioFile_Data;

typedef struct audio_samples *Samples;
typedef struct audio_samples {

    int size;
    int channels;

    float **buffers;

} Samples_Data;

AudioFile read_audio_file(char *filename);
AudioFile write_audio_file(char *filename,
                           int samplerate,
                           int channels,
                           int format);
Samples get_audio_data(AudioFile af, int size);
void write_audio_data(AudioFile af, Samples smps);
void cleanup_audio_file(AudioFile af);
Samples create_sample_buffer();
void cleanup_sample_buffer(Samples smps);

#endif
