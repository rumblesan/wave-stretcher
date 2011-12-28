#include <stdlib.h>
#include <stdio.h>


struct stretch_data {

    float *input_data;
    int    input_size;
    float  input_offset;


    float *output_data;
    int    output_size;
    int    output_offset;

    int finished;

    float ratio;

    int channels;

    int window_size;

    float *buffers[];
};



void setup_stretch (struct stretch_data *s,
                    float *wavdata,
                    int input_size,
                    int window_size,
                    int channels,
                    float ratio) {

    s->window_size  = window_size;
    s->channels     = channels;
    s->ratio        = 0.0;

    s->finished     = 0;

    s->input_data   = wavdata;
    s->input_size   = input_size;
    s->input_offset = 0.0;

    /*
       creating initial output buffer
       initial size needs to be number:
       channels * (window_size / 2)
    */
    s->output_size   = s->channels * (s->window_size / 2);
    s->output_data   = (float*) malloc(sizeof(float) * s->output_size);
    s->output_offset = 0;
    int i;
    for (i = s->output_offset; i < s->output_size; i++) {
        s->output_data[i] = 0;
    }


    /*
       create buffers. one for each channel
    */
    for (i = 0; i < s->channels; i++) {
        s->buffers[i] = (float*) malloc(sizeof(float) * s->window_size);
    }
}

void next_input_section (struct stretch_data *s) {

    int i, j, k;
    float data;

    for (i = 0; i < s->window_size; i++) {
        j = (s->channels * i) + (int)s->input_offset;
        for (k = 0; k < s->channels; k++) {
            data  = s->input_data[j+k];
            s->buffers[k][i] = data;
        }
    }

    s->input_offset += s->ratio * (s->window_size * 0.5);
}

void add_output(struct stretch_data *s, float *buffers[]) {

    int i, j, k, N;
    float data;

    /*
       because the windows are being overlapped we extend the output
       buffer by half a windowsize for each channel
   */

    N = s->output_size + s->channels * (s->window_size / 2);
    s->output_data = (float*) realloc(s->output_data, sizeof(float) * N);
    for (i = s->output_size; i < N; i++) {
        s->output_data[i] = 0;
    }
    s->output_size = N;

    for (i = 0; i < s->window_size; i++) {
        j = (s->channels * i) + (int)s->output_offset;
        for (k = 0; k < s->channels; k++) {
            data  = buffers[k][i];
        }
        s->output_data[j] += data;
    }
    s->output_offset += s->channels * (s->window_size / 2);
}


void print_next_section (struct stretch_data *s) {

    int i;
    for (i = 0; i < s->window_size; i++) {
        printf("%i  %.1f %.1f\n", i, s->loutput[i], s->routput[i]);
    }

}

void print_data (struct stretch_data *s) {

    int i;
    for (i = 0; i < 16; i++) {
        printf("%i  %.1f\n", i, s->input[i]);
    }

}

void main () {

    int channels = 2;
    int samples  = 128;
    int datasize = channels * samples;
    float *wdata;
    wdata = (float*) malloc(sizeof(float) * datasize);

    int i,j,k;
    for (i = 0; i < samples; i++) {
        j = (i * channels);
        for (k = 0; k < channels; k++) {
            wdata[j+k] = 1 + (k/10);
        }
    }

void setup_stretch (struct stretch_data *s,
                    float *wavdata,
                    int input_size,
                    int window_size,
                    int channels,
                    float ratio) {
    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, 256, 2, 1);

    get_next_section(&sdata);
    print_next_section(&sdata);

    get_next_section(&sdata);
    print_next_section(&sdata);

}

