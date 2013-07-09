#ifndef SAMPLE_H
#define SAMPLE_H

typedef struct audio_samples *Samples;
typedef struct audio_samples {

    int size;
    int channels;

    float **buffers;

} Samples_Data;

Samples sbuffer_create(int channels, int size);
void sbuffer_cleanup(Samples smps);

#endif
