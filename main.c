#include <stdio.h>
#include <stdlib.h>
#include "main.h"


void main () {

    struct audio_file af;
    af.filename = "test.wav";

    read_wav(&af);

    print_wav(&af);

    af.filename = "output.wav";
    write_wav(&af);

}


