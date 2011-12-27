#include <stdlib.h>
#include <stdio.h>


struct stretch_input {

    float *input;
    int input_size;

    int window_size;
    int ratio;
    float offset;

    int finished;

    float *lbuffer, *rbuffer;

};


struct stretch_output {

    float *output;

    int output_size;

    int window_size;
    int offset;

};

struct stretch_data {
    struct stretch_input  in;
    struct stretch_output out;
};



void setup_stretch_input (struct stretch_input *s,
                          float *wavdata,
                          int input_size,
                          int window_size,
                          float ratio) {

    s->input      = wavdata;
    s->input_size = input_size;

    s->window_size = window_size;
    s->ratio       = ratio;
    s->offset      = 0.0;

    s->finished    = 0;

    s->lbuffer = (float*) malloc(sizeof(float) * s->window_size);
    s->rbuffer = (float*) malloc(sizeof(float) * s->window_size);
}

void setup_stretch_output(struct stretch_output *s,
                          int window_size) {
    s->window_size = window_size;
    s->offset = 0;
    s->output_size = 0;

    s->output = (float*) malloc(sizeof(float)*(2*(s->window_size/2)));
    int i;
    for (i = 0; i < (2*(s->window_size/2)); i++) {
        s->output[i] = 0;
    }

}

void next_input_section (struct stretch_input *s) {

    int i, j, k;
    float left,right;
    for (i = 0; i < s->window_size; i++) {
        j = (2 * i) + (int)s->offset;
        k = j + 1;
        if (k >= s->input_size) {
            left  = 0;
            right = 0;
            s->finished = 1;
        } else {
            left  = s->input[j];
            right = s->input[k];
        }
        s->lbuffer[i] = left;
        s->rbuffer[i] = right;
    }

    s->offset += s->ratio * (s->window_size * 0.5);
}

void add_output(struct stretch_output *o, float *lbuffer, float *rbuffer) {

    int i,j;
    /*
       because the windows are being overlapped we extend the output buffer by one window size
       should be increased by half a windowsize for each channel
    */
    o->output = (float*) realloc(o->output, sizeof(float)*(o->output_size+(2*(o->window_size/2))));
    for (i = 0; i < (2*2*(o->window_size/2)); i++) {
        j = i + o->offset + (o->window_size/2);
        o->output[j] = 0;
    }

    for (i = 0; i < o->window_size; i++) {
        j = (2 * i) + (int)o->offset;
        o->output[j]   += lbuffer[i];
        o->output[j+1] += rbuffer[i];
    }
    o->offset += (o->window_size/2);
}


/*
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

    float *wdata;
    wdata = (float*) malloc(sizeof(float) * 128 * 2);

    int i;
    for (i = 0; i < (2 * 128); i += 2) {
        wdata[i]   = i;
        wdata[i+1] = i + 1.0;
    }

    struct stretch_data sdata;
    setup_stretch(&sdata, wdata, 8, 256, 2, 1);

    get_next_section(&sdata);
    print_next_section(&sdata);

    get_next_section(&sdata);
    print_next_section(&sdata);

}
*/

