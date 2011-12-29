#include <stdio.h>
#include <stdlib.h>
#include "main.h"


void main () {

    struct wavefiledata wavedata;
    char infile[] = "test.wav";
    char outfile[] = "output.wav";

    read_wav(infile, &wavedata);

    print_wav(&wavedata);
    write_wav(outfile, &wavedata);

}


