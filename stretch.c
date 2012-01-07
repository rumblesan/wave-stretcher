#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stretch.h"
#include "audiodata.h"


Stretch create_stretch(int channels,
                       int window_size,
                       float stretch) {

    Stretch s       = (Stretch) malloc(sizeof(Stretch_Data));
    s->window_size  = window_size;
    s->channels     = channels;
    s->speed        = 1.0/stretch;

    s->need_more_audio = 1;
    s->input_offset    = 0.0;
    s->buffer_size     = window_size;
    /*
       create buffers. one for each channel
    */
    s->buffers = (float**) malloc(sizeof(float*) * s->channels);
    int i,j;
    for (i = 0; i < s->channels; i++) {
        s->buffers[i] = (float*) malloc(sizeof(float) * s->window_size);
        for (j = 0; j < s->window_size; j++) {
            s->buffers[i][j] = 0.0;
        }
    }

    return s;
}

void add_samples(Stretch s, Samples smps) {

    float *tmp_buffer;
    int i,j;
    int offset = (int) floor(s->input_offset);
    int size   = s->buffer_size;
    int rem    = size - offset;

    for (i = 0; i < s->channels; i++) {
        tmp_buffer = (float*) malloc(sizeof(float) * (rem+smps->size));
        for (j = 0; j < rem; j++) {
            tmp_buffer[j] = s->buffers[i][j+offset];
        }
        for (j = 0; j < smps->size; j++) {
            tmp_buffer[j+rem] = smps->buffers[i][j];
        }
        free(s->buffers[i]);
        s->buffers[i] = tmp_buffer;
    }

    s->input_offset   -= floor(s->input_offset);
    s->buffer_size     = rem+smps->size;
    s->need_more_audio = 0;

}

Samples next_window(Stretch s) {

    int i, j;
    int offset = (int) floor(s->input_offset);

    Samples smps = create_sample_buffer(s->channels, s->window_size);

    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < s->window_size; j++) {
            smps->buffers[i][j] = s->buffers[i][j+offset];
        }
    }

    s->input_offset += s->speed * ((float)s->window_size * 0.5);

    if (floor(s->input_offset) >= s->window_size) {
        s->need_more_audio = 1;
    }

    return smps;
}

void cleanup_stretch(Stretch s) {

    int i;
    for (i = 0; i < s->channels; i++) {
        free(s->buffers[i]);
    }
    free(s->buffers);

    free(s);
}

