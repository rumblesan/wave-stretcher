#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stretch.h"
#include "sample.h"


Stretch create_stretch(int channels,
                       int window_size,
                       float stretch) {

    Stretch s       = (Stretch) malloc(sizeof(Stretch_Data));
    s->window_size  = window_size;
    s->channels     = channels;
    s->speed        = 1.0/stretch;

    s->need_more_audio = 1;
    s->input_offset    = 0.0;
    /*
       create buffers
    */
    s->input = create_sample_buffer(s->channels, s->window_size);
    s->old_output = create_sample_buffer(s->channels, s->window_size);
    int i,j;

    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < s->window_size; j++) {
            s->input->buffers[i][j] = 0.0;
            s->old_output->buffers[i][j] = 0.0;
        }
    }

    return s;
}

void add_samples(Stretch s, Samples smps) {

    Samples tmp_samples;
    int i,j;
    int offset = (int) floor(s->input_offset);
    int size   = s->input->size;
    int rem    = size - offset;

    tmp_samples = create_sample_buffer(s->channels, rem+smps->size);
    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < rem; j++) {
            tmp_samples->buffers[i][j] = s->input->buffers[i][j+offset];
        }
        for (j = 0; j < smps->size; j++) {
            tmp_samples->buffers[i][j+rem] = smps->buffers[i][j];
        }
    }

    cleanup_sample_buffer(s->input);
    s->input           = tmp_samples;
    s->input_offset   -= floor(s->input_offset);
    s->need_more_audio = 0;

    cleanup_sample_buffer(smps);

}

Samples next_window(Stretch s) {

    int i, j;
    int offset = (int) floor(s->input_offset);

    Samples smps = create_sample_buffer(s->channels, s->window_size);

    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < s->window_size; j++) {
            smps->buffers[i][j] = s->input->buffers[i][j+offset];
        }
    }

    float offset_inc = s->speed * ((float)s->window_size * 0.5);
    s->input_offset += offset_inc;

    if (floor(s->input_offset)+offset_inc >= s->window_size) {
        s->need_more_audio = 1;
    }

    return smps;
}

Samples create_output_buffer(Stretch s, Samples smps) {

    int i,j;
    float data;
    int halfwindow = s->window_size/2;
    Samples output = create_sample_buffer(s->channels, halfwindow);
    for (i = 0; i < s->channels;i++) {
        for (j = 0; j < halfwindow;j++) {
            data  = s->old_output->buffers[i][j+halfwindow];
            data += smps->buffers[i][j];
            output->buffers[i][j] = data;
        }
    }
    cleanup_sample_buffer(s->old_output);
    s->old_output = smps;
    return output;
}

void cleanup_stretch(Stretch s) {

    cleanup_sample_buffer(s->input);
    cleanup_sample_buffer(s->old_output);

    free(s);
}

