#ifndef WAVESTRETCH_H
#define WAVESTRETCH_H

typedef struct input_args {
    char  *input_file;
    char  *output_file;
    int   window_size;
    float stretch;
} Args;

#endif
