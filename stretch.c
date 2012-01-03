#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "stretch.h"


Stretch create_stretch(float *wavdata,
                       int frames,
                       int channels,
                       int window_size,
                       float ratio) {

    Stretch s = (Stretch) malloc(sizeof(Stretch_Data));
    s->window_size  = window_size;
    s->channels     = channels;
    s->ratio        = ratio;

    s->input_data   = wavdata;
    s->input_frames = frames;
    s->input_offset = 0.0;

    s->finished     = 0;


    /*
       creating initial output buffer
       initial size needs to be number:
       channels * (window_size / 2)
    */
    s->output_frames = s->window_size / 2;
    s->output_offset = 0;

    int buffer_size = s->output_frames * s->channels;
    int i;

    s->output_data   = (float*) malloc(sizeof(float) * buffer_size);
    for (i = 0; i < buffer_size; i++) {
        s->output_data[i] = 0;
    }

    /*
       create buffers. one for each channel
       each needs to be "window_size" number of elements
    */
    for (i = 0; i < s->channels; i++) {
        s->buffers[i] = (float*) malloc(sizeof(float) * s->window_size);
    }
    return s;
}

/*
   reads data from the input buffer,
   splits it into seperate channels
   fills the channel buffers
   each is "window_size" long
*/
void next_input_section(Stretch s) {

    int i, j, k;
    float data;

    for (i = 0; i < s->window_size; i++) {
        j = s->channels * (i + (int)s->input_offset);
        for (k = 0; k < s->channels; k++) {
            if ((i + (int)s->input_offset) >= s->input_frames) {
                data = 0.0;
                s->finished = 1;
            } else {
                data = s->input_data[j+k];
            }
            s->buffers[k][i] = data;
        }
    }

    s->input_offset += s->ratio * ((float)s->window_size * 0.5);
}

void add_output(Stretch s) {

    int i, j, k;
    int buffer_size;
    float data;

    /*
       because the windows are being overlapped we extend the output
       buffer by half a windowsize for each channel
   */

    buffer_size = s->channels * (s->output_frames + (s->window_size / 2));
    s->output_data = (float*) realloc(s->output_data, sizeof(float) * buffer_size);
    for (i = (s->channels * s->output_frames); i < buffer_size; i++) {
        s->output_data[i] = 0;
    }
    s->output_frames += (s->window_size / 2);

    for (i = 0; i < s->window_size; i++) {
        j = s->channels * (i + (int)s->output_offset);
        for (k = 0; k < s->channels; k++) {
            data  = s->buffers[k][i];
            s->output_data[j+k] += data;
        }
    }
    s->output_offset += (s->window_size / 2);
}

void cleanup_stretch(Stretch s) {

    free(s->output_data);

    int i;
    for (i = 0; i < s->channels; i++) {
        free(s->buffers[i]);
    }
    free(s->buffers);

    free(s);
}

