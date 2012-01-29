#ifndef MP3FILE_H
#define MP3FILE_H

#include <mad.h>
#include <stdio.h>
#include "sample.h"

typedef struct mp3_output_file *MP3OutputFile;

typedef struct mp3_output_file {
    FILE *sf;
    char *filename;

    int channels;
    int samplerate;

} MP3File_Output_Data;

typedef struct mp3_input_file *MP3InputFile;

typedef struct mp3_input_file {
    FILE *sf;
    char *filename;

    int samples;
    int channels;
    int samplerate;

    struct mad_stream stream;
    struct mad_frame  frame;
    struct mad_synth  synth;

    int finished;
    int pcm_remaining;

} MP3File_Input_Data;


MP3InputFile mp3_read_file(char *filename);
Samples mp3_get_audio(MP3InputFile af, int size);
void mp3_input_cleanup(MP3InputFile af);

//MP3File mp3_write_file(char *filename,
//                       int samplerate,
//                       int channels,
//                       int format);
//void mp3_write_data(MP3File af, Samples smps);
//void mp3_cleanup_file(MP3File af);

#endif
