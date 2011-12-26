#include <stdio.h>
#include <stdlib.h>
#include "main.h"


void main () {

    struct wavefiledata wavedata;

    read_wav(&wavedata);

    print_wav(&wavedata);

}


