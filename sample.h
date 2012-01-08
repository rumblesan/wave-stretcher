#ifndef SAMPLE_H
#define SAMPLE_H

typedef struct audio_samples *Samples;
typedef struct audio_samples {

    int size;
    int channels;

    float **buffers;

} Samples_Data;

Samples create_sample_buffer();
void cleanup_sample_buffer(Samples smps);

#endif
