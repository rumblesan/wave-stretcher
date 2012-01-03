#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "fft.h"
#include "stretch.h"

struct input_args parse_args(int argc, char *argv[]) {

    struct input_args args;
    int i;
    int ifile,ofile,win,spd;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            args.input_file = argv[i+1];
            ifile = 1;
            printf("input file  : %s\n",args.input_file);
        } else if (strcmp(argv[i], "-o") == 0) {
            args.output_file = argv[i+1];
            ofile = 1;
            printf("output file : %s\n",args.output_file);
        } else if (strcmp(argv[i], "-s") == 0) {
            args.speed_ratio = (float)atof(argv[i+1]);
            printf("speed ratio : %f\n",args.speed_ratio);
            spd = 1;
        } else if (strcmp(argv[i], "-w") == 0) {
            win = 1;
            int window = atoi(argv[i+1]);
            if (window%2 != 0) {
                printf("window size needs to be divisible by 2\n");
                window += 1;
                printf("increasing it to %i\n", window);
            }
            args.window_size = window;
            printf("window size : %i\n",args.window_size);
        }
    }

    if ((ifile+ofile+win+spd) != 4) {
        printf("Didnt give all args. Exiting!!");
        exit(1);
    }

    return args;
}

void main (int argc, char *argv[]) {

    struct input_args args = parse_args(argc, argv);

    struct audio_file af;
    af.filename = args.input_file;

    read_wav(&af);

    int window_size = args.window_size;
    float ratio = args.speed_ratio;
    Stretch stretch = create_stretch(af.sound_data,
                                     af.info.frames,
                                     af.info.channels,
                                     window_size,
                                     ratio);

    FFT fft = create_FFT(window_size);

    int i;
    while (stretch->finished != 1) {
        next_input_section(stretch);
        for (i = 0; i < af.info.channels; i++) {
            get_data(fft, stretch->buffers[i]);
            samp_to_freq(fft);
            pauls_algo(fft);
            freq_to_samp(fft);
            return_data(fft, stretch->buffers[i]);
        }
        add_output(stretch);
    }

    struct audio_file of;
    of.filename        = args.output_file;
    of.sound_data      = stretch->output_data;
    of.info.samplerate = af.info.samplerate;
    of.info.channels   = af.info.channels;
    of.info.format     = af.info.format;
    of.info.frames     = stretch->output_frames;

    write_wav(&of);

    cleanup_fft(fft);
    free_wav(&af);

}


