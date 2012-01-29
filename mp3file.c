#include <stdlib.h>
#include <mad.h>
#include <stdio.h>
#include <string.h>
#include "sample.h"
#include "mp3file.h"

#define BUFSIZE 4096

MP3InputFile mp3_read_file(char *filename) {

    MP3File af = (MP3File) malloc(sizeof(MP3File_Data));

    af->filename = filename;

    af->sf = fopen(af->filename, "rb");
    fseek(af->sf,0,SEEK_END);

    af->samples = ftell(af->sf);
    rewind(af->sf);

    af->channels = 2;
    af->samplerate = 44100;

    af->finished = 0;
    af->pcm_remaining = 0;

    mad_stream_init(&af->stream);
    mad_frame_init(&af->frame);
    mad_synth_init(&af->synth);

    int n = 8192;
    short int temp[n*2];

    return af;
}

Samples mp3_get_audio(MP3InputFile af, int nsmps) {

    int i,j;
    int channels;

    int read_samples = nsmps;
    int read_size;
    int buf_read_amount;

    unsigned char iobuffer[BUFSIZE];

    int mad_remaining = 0;

    int sample_offset = 0;
    int pcm_sample_offset;
    int pos;

    Samples smps = NULL;

    if (af->pcm_remaining > 0) {
        buf_read_amount = af->pcm_remaining;
        if (buf_read_amount > nsmps) {
            buf_read_amount = nsmps;
        }

        channels = af->synth.pcm.channels;
        pcm_sample_offset = af->synth.pcm.length - af->pcm_remaining;
        mad_fixed_t *pcm_buf = af->synth.pcm.samples[0];
        pcm_buf += pcm_sample_offset;
        smps = sbuffer_create(channels, nsmps);
        for (i = 0; i < channels; i++) {
            for (j = 0; j < buf_read_amount; j++) {
                pos = (j * channels) + i;
                smps->buffers[i][j] = mad_f_todouble(pcm_buf[pos]);
            }
        }

        sample_offset      = af->pcm_remaining;
        af->pcm_remaining -= buf_read_amount;
        read_samples      -= buf_read_amount;

        if (read_samples <= 0) {
            return smps;
        }
    }

    while (read_samples > 0) {

        if (af->stream.buffer == NULL || af->stream.error == MAD_ERROR_BUFLEN)
        {
            if (af->stream.next_frame != NULL) {
                mad_remaining = af->stream.bufend - af->stream.next_frame;
                memmove(iobuffer, af->stream.next_frame, mad_remaining);
                read_size = BUFSIZE - mad_remaining;
            } else {
                read_size = BUFSIZE;
            }

            int read_amount = fread(iobuffer+mad_remaining,1,read_size,af->sf);

            if (feof(af->sf))
            {
                af->finished = 1;
            }

            if (read_amount != read_size)
            {
                for (i = read_amount; i < read_size; i++)
                {
                    iobuffer[i+mad_remaining] = 0;
                }
            }

            mad_stream_buffer(&af->stream, iobuffer, BUFSIZE);
        }

        if (mad_frame_decode(&af->frame, &af->stream) != 0) {
            if (af->stream.error == MAD_ERROR_BUFLEN) {
                continue;
            }
            if (af->stream.error == MAD_ERROR_LOSTSYNC) {
                continue;
            }
            if (!MAD_RECOVERABLE(af->stream.error)) {
                printf("Unrecoverable MP3 Error");
                exit(0);
            }
        }

        mad_synth_frame(&af->synth, &af->frame);

        if (af->synth.pcm.samplerate != 0) {
            af->samplerate = af->synth.pcm.samplerate;
        }

        buf_read_amount = af->synth.pcm.length;
        if (read_samples < af->synth.pcm.length) {
            af->pcm_remaining = af->synth.pcm.length - read_samples;
            buf_read_amount = read_samples;
        } else {
            af->pcm_remaining = 0;
        }

        channels = af->synth.pcm.channels;
        if (smps == NULL) {
            smps = sbuffer_create(channels, nsmps);
        }
        mad_fixed_t *pcm_buf = af->synth.pcm.samples[0];
        for (i = 0; i < channels; i++) {
            for (j = 0; j < buf_read_amount; j++) {
                pos = (j * channels) + i;
                smps->buffers[i][j+sample_offset] = mad_f_todouble(pcm_buf[pos]);
            }
        }

        sample_offset += buf_read_amount;
        read_samples  -= buf_read_amount;

    }

    return smps;

}


void mp3_input_cleanup(MP3InputFile af)
{

}



