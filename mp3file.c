#include <stdlib.h>
#include <mad.h>
#include <stdio.h>
#include <string.h>
#include "sample.h"
#include "mp3file.h"

#define BUFSIZE 4096

MP3InputFile mp3_read_file(char *filename) {

    MP3InputFile af = (MP3InputFile) malloc(sizeof(MP3File_Input_Data));

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


void mp3_input_cleanup(MP3InputFile af) {

}



MP3OutputFile mp3_write_file(char *filename,
                             int channels,
                             int samplerate,
                             int bitrate,
                             int mode,
                             int quality) {

    MP3OutputFile af = (MP3OutputFile) malloc(sizeof(MP3File_Output_Data));

    af->filename = filename;
    af->sf = fopen(af->filename);


    af->channels = channels;
    af->samplerate = samplerate;
    af->bitrate = bitrate;
    af->mode = mode;
    af->quality = quality;

    af->lame = lame_init();

    lame_set_num_channels(af->lame,af->channels);
    lame_set_in_samplerate(af->lame,af->samplerate);
    lame_set_brate(af->lame,af->bitrate);
    lame_set_mode(af->lame,af->mode);
    lame_set_quality(af->lame,af->quality);   /* 2=high  5 = medium  7=low */ 

    af->lame_init_check = lame_init_params(af->lame);

    af->buf_size = (1.25 * (float) args.window_size) + 7200;

    af->buf = (unsigned char) malloc(sizeof(unsigned char) * af->buf_size);

    return af;

}


void mp3_write_data(MP3OutputFile af, Samples smps) {

    int write;
    write = lame_encode_buffer_ieee_float(af->lame,
                                          smps->buffers[0],
                                          smps->buffers[1],
                                          smps->size,
                                          af->buf,
                                          af->buf_size);
    fwrite(af->buf, write, 1, af->sf);

    sbuffer_cleanup(smps);

}

void mp3_output_cleanup(MP3OutputFile af) {

    int write;
    write = lame_encode_flush(af->lame, af->buf, af->buf_size);
    if (write > 0) {
        fwrite(af->buf, write, 1, af->sf);
    }

    lame_mp3_tags_fid(af->lame, af->sf);

    lame_close(af->lame);
    fclose(af->sf);

    free(af->buf);

    free(af);

}



