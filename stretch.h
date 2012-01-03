#ifndef STRETCH_H
#define STRETCH_H

typedef struct stretch_data *Stretch;

typedef struct stretch_data {

    float *input_data;
    int    input_frames;
    float  input_offset;


    float *output_data;
    int    output_frames;
    int    output_offset;

    float stretch;
    float speed;

    int channels;

    int finished;

    int window_size;

    float **buffers;

} Stretch_Data;

Stretch create_stretch(float *wavdata,
                       int frames,
                       int channels,
                       int window_size,
                       float ratio);

void next_input_section(Stretch s);
void add_output(Stretch s);
void cleanup_stretch(Stretch s);

#endif

