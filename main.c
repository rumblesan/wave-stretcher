#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"
#include "fft.h"
#include "stretch.h"
#include "wavreader.h"

void usage(int exitval) {
    printf("Wave Stretch usage:\n");
    printf("wavstretch -i inputfile -o outputfile [-w WINDOW] [-s STRETCH]\n");
    printf("            WINDOW defaults to 2048\n");
    printf("           STRETCH defaults to 5\n");
    exit(exitval);
}

Args parse_args(int argc, char *argv[]) {

    Args args = {"","",2048,5};

    int c;
    while ( (c = getopt(argc, argv, "i:o:s:w:h")) != -1) {
        switch (c)
        {
            case 'i':
                args.input_file = optarg;
                break;
            case 'o':
                args.output_file = optarg;
                break;
            case 'w':
                args.window_size = atoi(optarg);
                break;
            case 's':
                args.stretch = atof(optarg);
                break;
            case 'h':
                usage(0);
                break;
        }
    }

    if (*args.input_file == '\0') {
        printf("Need to specify an input file\n");
        usage(1);
    } else {
        printf("Stretching %s\n", args.input_file);
    }

    if (*args.output_file == '\0') {
        printf("Need to specify an output file\n");
        usage(1);
    } else {
        printf("Writing to %s\n", args.output_file);
    }

    printf("Using a window of %i samples\n", args.window_size);
    printf("Stretching by %.3f times\n", args.stretch);

    return args;
}

void main (int argc, char *argv[]) {

    Args input_args args = parse_args(argc, argv);

    AudioFile af = read_wav(args.input_file);

    Stretch stretch = create_stretch(af->sound_data,
                                     af->info.frames,
                                     af->info.channels,
                                     args.window_size,
                                     args.stretch);

    FFT fft = create_FFT(args.window_size);

    int i;
    while (stretch->finished != 1) {
        next_input_section(stretch);
        for (i = 0; i < af->info.channels; i++) {
            get_data(fft, stretch->buffers[i]);
            samp_to_freq(fft);
            pauls_algo(fft);
            freq_to_samp(fft);
            return_data(fft, stretch->buffers[i]);
        }
        add_output(stretch);
    }

    AudioFile_Data of;
    of.filename        = args.output_file;
    of.sound_data      = stretch->output_data;
    of.info.samplerate = af->info.samplerate;
    of.info.channels   = af->info.channels;
    of.info.format     = af->info.format;
    of.info.frames     = stretch->output_frames;

    write_wav(&of);

    cleanup_fft(fft);
    cleanup_stretch(stretch);
    cleanup_wav(af);

}


