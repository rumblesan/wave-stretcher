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



void setup_stretch(struct stretch_data *s,
                   float *wavdata,
                   int input_size,
                   int window_size,
                   int channels,
                   float ratio) {

    s->window_size  = window_size;
    s->channels     = channels;
    s->ratio        = ratio;

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
       each needs to be "window_size" number of elements
    */
    for (i = 0; i < s->channels; i++) {
        s->buffers[i] = (float*) malloc(sizeof(float) * s->window_size);
    }
}

void next_input_section(struct stretch_data *s) {

    int i, j, k;
    float data;

    for (i = 0; i < s->window_size; i++) {
        j = (s->channels * i) + (int)s->input_offset;
        for (k = 0; k < s->channels; k++) {
            data  = s->input_data[j+k];
            s->buffers[k][i] = data;
        }
    }

    s->input_offset += s->channels * (s->ratio * ((float)s->window_size * 0.5));
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
            printf("%i %i %.1f %.1f   ", j, k, data, s->output_data[j+k]);
            s->output_data[j+k] += data;
        }
        printf("\n");
    }
    s->output_offset += s->channels * (s->window_size / 2);
}


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
    for (i = 0; i < s->output_size; i++) {
        printf("%i:%.2f ", i, s->output_data[i]);
    }
    printf("\n");
}

void test_next_input_section() {

    int channels = 2;
    int samples  = 128;
    int datasize = channels * samples;
    int window_size = 8;
    float ratio = 1.0;

    float *wdata;
    wdata = (float*) malloc(sizeof(float) * datasize);

    int i,j,k;
    for (i = 0; i < samples; i++) {
        j = (i * channels);
        for (k = 0; k < channels; k++) {
            wdata[j+k] = (float)i + ((float)k/10);
        }
    }

    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, datasize, window_size, channels, ratio);

    next_input_section(&sdata);
    print_next_section(&sdata);

    next_input_section(&sdata);
    print_next_section(&sdata);

    next_input_section(&sdata);
    print_next_section(&sdata);

}

void test_add_output() {

    int channels = 2;
    int samples  = 128;
    int datasize = channels * samples;
    int window_size = 8;
    float ratio = 1.0;

    float *wdata;
    wdata = (float*) malloc(sizeof(float) * datasize);

    int i,j,k;
    for (i = 0; i < samples; i++) {
        j = (i * channels);
        for (k = 0; k < channels; k++) {
            wdata[j+k] = 1 + ((float)k/10);
        }
    }

    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, datasize, window_size, channels, ratio);

    next_input_section(&sdata);
    add_output(&sdata, sdata.buffers);
    print_output(&sdata);

    next_input_section(&sdata);
    add_output(&sdata, sdata.buffers);
    print_output(&sdata);

}

#ifdef TEST_SUITE
void main () {
    test_next_input_section();
    test_add_output();
}
#endif

