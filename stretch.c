#include <stdlib.h>
#include <stdio.h>


struct stretch_data {

    float *data;
    int datasize;

    int window_size;

    float ratio;

    int channels;

    int finished;

    float offset;

    float *loutput, *routput;

};

void setup_stretch (struct stretch_data *s, float *wavdata, int windowsize, int datasize, int channels, float ratio) {

    s->window_size = windowsize;
    s->channels    = channels;
    s->ratio       = ratio;
    s->offset      = 0.0;

    s->finished    = 0;

    s->data = wavdata;
    s->datasize = datasize;

    s->loutput = (float*) malloc(sizeof(float) * windowsize);
    s->routput = (float*) malloc(sizeof(float) * windowsize);
}

void get_next_section (struct stretch_data *s) {

    int i, j, k;
    float left,right;
    for (i = 0; i < s->window_size; i++) {
        j = (2 * i) + (int)s->offset;
        k = j + 1;
        if (k >= s->datasize) {
            left  = 0;
            right = 0;
            s->finished = 1;
        } else {
            left  = s->data[j];
            right = s->data[k];
        }
        s->loutput[i] = left;
        s->routput[i] = right;
    }

    s->offset += s->ratio * (s->window_size * 0.5);
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
        printf("%i  %.1f\n", i, s->data[i]);
    }

}

/*
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

