#include <stdlib.h>
#include <stdio.h>
#include "main.h"


void setup_stretch(struct stretch_data *s,
                   float *wavdata,
                   int frames,
                   int channels,
                   int window_size,
                   float ratio) {

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
}

/*
   reads data from the input buffer,
   splits it into seperate channels
   fills the channel buffers
   each is "window_size" long
*/
void next_input_section(struct stretch_data *s) {

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

void add_output(struct stretch_data *s) {

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


#ifdef STRETCH_TEST
void print_next_section(struct stretch_data *s) {

    int i,j;
    for (i = 0; i < s->window_size; i++) {
        printf("%i ", i);
        for (j = 0; j < s->channels; j++) {
            printf("%i:%.1f ", j, s->buffers[j][i]);
        }
        printf("\n");
    }

}

void print_output(struct stretch_data *s) {

    int i;
    for (i = 0; i < (s->channels * s->output_frames); i++) {
        printf("%i:%.2f ", i, s->output_data[i]);
    }
    printf("\n");
}

void test_next_input_section() {

    int channels = 2;
    int frames  = 128;
    int window_size = 8;
    float ratio = 1.0;

    float *wdata;
    wdata = (float*) malloc(sizeof(float) * frames * channels);

    int i,j,k;
    for (i = 0; i < frames; i++) {
        j = (i * channels);
        for (k = 0; k < channels; k++) {
            wdata[j+k] = (float)i + ((float)k/10);
        }
    }

    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, frames, channels, window_size, ratio);

    next_input_section(&sdata);
    print_next_section(&sdata);

    next_input_section(&sdata);
    print_next_section(&sdata);

    next_input_section(&sdata);
    print_next_section(&sdata);

}

void test_add_output() {

    int channels = 2;
    int frames  = 128;
    int datasize = channels * frames;
    int window_size = 8;
    float ratio = 1.0;

    float *wdata;
    wdata = (float*) malloc(sizeof(float) * datasize);

    int i,j,k;
    for (i = 0; i < frames; i++) {
        j = (i * channels);
        for (k = 0; k < channels; k++) {
            wdata[j+k] = 1 + ((float)k/10);
        }
    }

    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, frames, channels, window_size, ratio);

    next_input_section(&sdata);
    add_output(&sdata);
    print_output(&sdata);

    next_input_section(&sdata);
    add_output(&sdata);
    print_output(&sdata);

}

void test_finish() {

    int channels = 2;
    int frames  = 14;
    int datasize = channels * frames;
    int window_size = 8;
    float ratio = 1.0;

    float *wdata;
    wdata = (float*) malloc(sizeof(float) * datasize);

    int i,j,k;
    for (i = 0; i < frames; i++) {
        j = (i * channels);
        for (k = 0; k < channels; k++) {
            wdata[j+k] = (float)i + ((float)k/10);
        }
    }

    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, frames, channels, window_size, ratio);

    printf("finished %i\n", sdata.finished);

    while (sdata.finished != 1) {
        next_input_section(&sdata);
        print_next_section(&sdata);
    }

    printf("finished %i\n", sdata.finished);

}

void test_stretch() {

    int channels = 2;
    int frames  = 14;
    int datasize = channels * frames;
    int window_size = 8;
    float ratio = 0.8;

    float *wdata;
    wdata = (float*) malloc(sizeof(float) * datasize);

    int i,j,k;
    for (i = 0; i < frames; i++) {
        j = (i * channels);
        for (k = 0; k < channels; k++) {
            wdata[j+k] = (float)i + ((float)k/10);
        }
    }

    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, frames, channels, window_size, ratio);

    printf("finished %i\n", sdata.finished);

    while (sdata.finished != 1) {
        next_input_section(&sdata);
        print_next_section(&sdata);
    }

    printf("finished %i\n", sdata.finished);

}

void main () {
//    test_next_input_section();
//    test_add_output();
//    test_finish();
    test_stretch();
}
#endif

