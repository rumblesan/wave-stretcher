#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"
#include "fft.h"
#include "stretch.h"
#include "audiodata.h"

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

int main (int argc, char *argv[]) {

    Args args = parse_args(argc, argv);

    AudioFile af = read_audio_file(args.input_file);
    Samples tmp_smps;

    Stretch stretch = create_stretch(af->info.channels,
                                     args.window_size,
                                     args.stretch);

    FFT fft = create_FFT(args.window_size);

    /*
        need to load in data for the stretch
    */
    tmp_smps = get_audio_data(af, stretch->window_size);
    add_samples(stretch, tmp_smps);
    cleanup_sample_buffer(tmp_smps);

    while (af->finished != 1) {
        printf("GOING\n");
        if (stretch->need_more_audio) {
            printf("GETTING DATA\n");
            tmp_smps = get_audio_data(af, stretch->window_size);
            add_samples(stretch, tmp_smps);
            cleanup_sample_buffer(tmp_smps);
        }
        tmp_smps = next_window(stretch);

        cleanup_sample_buffer(tmp_smps);
    }
    printf("cleaning up\n");

    cleanup_fft(fft);
    cleanup_stretch(stretch);
    cleanup_audio_file(af);

    return 0;

}


