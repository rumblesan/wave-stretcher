#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stretch.h"
#include "sample.h"


Stretch stretch_create(int channels,
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
    s->input      = sbuffer_create(s->channels, s->window_size);
    s->old_output = sbuffer_create(s->channels, s->window_size);
    int i,j;

    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < s->window_size; j++) {
            s->input->buffers[i][j] = 0.0;
            s->old_output->buffers[i][j] = 0.0;
        }
    }

    return s;
}

void stretch_add_samples(Stretch s, Samples smps) {

    Samples tmp_samples;
    int i,j;
    int offset = (int) floor(s->input_offset);
    int size   = s->input->size;
    int rem    = size - offset;

    tmp_samples = sbuffer_create(s->channels, rem+smps->size);
    for (i = 0; i < s->channels; i++) {
        for (j = 0; j < rem; j++) {
            tmp_samples->buffers[i][j] = s->input->buffers[i][j+offset];
        }
        for (j = 0; j < smps->size; j++) {
            tmp_samples->buffers[i][j+rem] = smps->buffers[i][j];
        }
    }

    sbuffer_cleanup(s->input);
    s->input           = tmp_samples;
    s->input_offset   -= floor(s->input_offset);
    s->need_more_audio = 0;

    sbuffer_cleanup(smps);
}

Samples stretch_window(Stretch s) {

    int i, j;
    int offset = (int) floor(s->input_offset);

    Samples smps = sbuffer_create(s->channels, s->window_size);

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

Samples stretch_output(Stretch s, Samples smps) {

    int i,j;
    float data;
    int halfwindow = s->window_size/2;
    Samples output = sbuffer_create(s->channels, halfwindow);
    for (i = 0; i < s->channels;i++) {
        for (j = 0; j < halfwindow;j++) {
            data  = s->old_output->buffers[i][j+halfwindow];
            data += smps->buffers[i][j];
            output->buffers[i][j] = data;
        }
    }
    sbuffer_cleanup(s->old_output);
    s->old_output = smps;
    return output;
}

void stretch_cleanup(Stretch s) {

    sbuffer_cleanup(s->input);
    sbuffer_cleanup(s->old_output);

    free(s);
}

