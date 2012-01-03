#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <sndfile.h>

struct input_args {
    char  *input_file;
    char  *output_file;
    int   window_size;
    float speed_ratio;
};

#endif
