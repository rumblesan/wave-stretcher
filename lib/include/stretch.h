#ifndef STRETCH_H
#define STRETCH_H

#include "sample.h"


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

Stretch stretch_create(int channels,
                       int window_size,
                       float ratio);

void stretch_add_samples(Stretch s, Samples smps);
Samples stretch_window(Stretch s);
Samples stretch_output(Stretch s, Samples smps);
void stretch_cleanup(Stretch s);

#endif

