#ifndef STRETCH_H
#define STRETCH_H

#include "audiodata.h"


typedef struct stretch_data *Stretch;

typedef struct stretch_data {

    int window_size;
    int channels;
    float speed;

    int need_more_audio;
    float input_offset;

    Samples input;

    Samples old_output;

} Stretch_Data;

Stretch create_stretch(int channels,
                       int window_size,
                       float ratio);

void add_samples(Stretch s, Samples smps);
Samples next_window(Stretch s);
Samples create_output_buffer(Stretch s, Samples smps);
void cleanup_stretch(Stretch s);

#endif

