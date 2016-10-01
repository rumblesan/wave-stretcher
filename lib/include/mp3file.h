#ifndef MP3FILE_H
#define MP3FILE_H

#include <mad.h>
#include <stdio.h>
#include "sample.h"
#include "lame/lame.h"

#define INPUT_BUFFER_SIZE	(5*8192)

typedef struct mp3_output_file *MP3OutputFile;

typedef struct mp3_output_file {
    FILE *sf;
    char *filename;

    lame_t lame;

    int channels;
    int samplerate;
    int bitrate;
    int mode;
    int quality;

    int lame_init_check;

    int buf_size;
    unsigned char *buf;

} MP3File_Output_Data;

typedef struct mp3_input_file *MP3InputFile;

typedef struct mp3_input_file {
    FILE *sf;
    char *filename;

    unsigned char in_buffer[INPUT_BUFFER_SIZE+MAD_BUFFER_GUARD];
    unsigned char *GuardPtr;

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
Samples mp3_get_audio(MP3InputFile af);
void mp3_input_cleanup(MP3InputFile af);

MP3OutputFile mp3_write_file(char *filename,
                             int channels,
                             int samplerate,
                             int bitrate,
                             int mode,
                             int quality,
                             int window_size);

void mp3_write_data(MP3OutputFile af, Samples smps);
void mp3_output_cleanup(MP3OutputFile af);

#endif
