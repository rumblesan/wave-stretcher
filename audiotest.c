#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sndfile.h>
#include <mad.h>
#include <lame/lame.h>
#include "wavefile.h"
#include "mp3file.h"
#include "sample.h"




void wav_to_mp3() {

    Samples smps;
    AudioFile af      = read_audio_file("input.wav");
    MP3OutputFile of  = mp3_write_file("test_output.mp3",
                                       af->info.channels,
                                       af->info.samplerate,
                                       128,
                                       0,
                                       0,
                                       1024);

    while (af->finished != 1) {
        smps = get_audio_data(af, 64);
        printf("size %i\n", smps->size);
        if (smps != NULL) {
            mp3_write_data(of, smps);
        }
    }

    cleanup_audio_file(af);
    mp3_output_cleanup(of);
}



void wav_to_wav() {

    int read_size = 20;
    Samples smps;
    int format = (SF_FORMAT_WAV|SF_FORMAT_PCM_16);
    AudioFile af = read_audio_file("input.wav");
    AudioFile of = write_audio_file("output.wav",
                                    44100,
                                    2,
                                    format);

    while (af->finished != 1) {
        smps = get_audio_data(af, read_size);
        write_audio_data(of, smps);
    }

    cleanup_audio_file(af);
    cleanup_audio_file(of);
}


int mp3_to_wav() {

    Samples smps;
    int format = (SF_FORMAT_WAV|SF_FORMAT_PCM_16);

    MP3InputFile inf = mp3_read_file("test_input.mp3");
    AudioFile     of = write_audio_file("test_wav_output.wav",
                                        44100,
                                        2,
                                        format);


    while (inf->finished != 1) {
        smps = mp3_get_audio(inf);
        if (smps->size > 0) {
        printf("size %i\n", smps->size);
            write_audio_data(of, smps);
        }
    }

    mp3_input_cleanup(inf);
    cleanup_audio_file(of);

    return 0;
}


int main() {

    mp3_to_wav();
    return 0;
}



