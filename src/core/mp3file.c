#include <stdlib.h>
#include <mad.h>
#include <stdio.h>
#include <string.h>
#include <lame/lame.h>
#include <limits.h>
#include "core/sample.h"
#include "core/mp3file.h"

MP3InputFile mp3_read_file(char *filename) {

    MP3InputFile af = (MP3InputFile) malloc(sizeof(MP3File_Input_Data));

    af->filename = filename;

    af->sf = fopen(af->filename, "rb");
    fseek(af->sf,0,SEEK_END);

    af->samples = ftell(af->sf);
    rewind(af->sf);

    af->GuardPtr = NULL;
    af->channels = 0;
    af->samplerate = 0;

    af->finished = 0;
    af->pcm_remaining = 0;

    mad_stream_init(&af->stream);
    mad_frame_init(&af->frame);
    mad_synth_init(&af->synth);

    return af;
}


short int madpcm2short(mad_fixed_t x){
	if (x>=MAD_F_ONE) x=MAD_F_ONE-1;
	else if (x<=-MAD_F_ONE) x=-MAD_F_ONE+1;
	int result= x >> (MAD_F_FRACBITS + 1 - 16);
	return result;
};


Samples mp3_get_audio(MP3InputFile af)
{

    int i, j, channels, nsmps;

    Samples smps;


	/* This is the decoding loop. */
	do
	{
		/* The input bucket must be filled if it becomes empty or if
		 * it's the first execution of the loop.
		 */
		if(af->stream.buffer == NULL || af->stream.error == MAD_ERROR_BUFLEN)
		{
			size_t			read_size,
							remaining,
                            read_amount;
			unsigned char	*read_start;

			/* {2} libmad may not consume all bytes of the input
			 * buffer. If the last frame in the buffer is not wholly
			 * contained by it, then that frame's start is pointed by
			 * the next_frame member of the Stream structure. This
			 * common situation occurs when mad_frame_decode() fails,
			 * sets the stream error code to MAD_ERROR_BUFLEN, and
			 * sets the next_frame pointer to a non NULL value. (See
			 * also the comment marked {4} bellow.)
			 *
			 * When this occurs, the remaining unused bytes must be
			 * put back at the beginning of the buffer and taken in
			 * account before refilling the buffer. This means that
			 * the input buffer must be large enough to hold a whole
			 * frame at the highest observable bit-rate (currently 448
			 * kb/s). XXX=XXX Is 2016 bytes the size of the largest
			 * frame? (448000*(1152/32000))/8
			 */

            remaining = 0;

			if(af->stream.next_frame != NULL)
			{
				remaining  = af->stream.bufend - af->stream.next_frame;
				memmove(af->in_buffer, af->stream.next_frame, remaining);
			}

            read_size  = INPUT_BUFFER_SIZE - remaining;
            read_start = af->in_buffer + remaining;

			/* Fill-in the buffer. If an error occurs print a message
			 * and leave the decoding loop. If the end of stream is
			 * reached we also leave the loop but the return status is
			 * left untouched.
			 */

            read_amount = fread(read_start, 1, read_size, af->sf);

            if (read_amount < read_size)
            {
                if (ferror(af->sf))
                {
                    printf("File Error");
                    exit(0);
                }

                if (feof(af->sf))
                {
                    af->finished = 1;

                    af->GuardPtr = read_start + read_size;
                    memset(af->GuardPtr, 0, MAD_BUFFER_GUARD);
                    read_size += MAD_BUFFER_GUARD;
                }
            }



			/* {3} When decoding the last frame of a file, it must be
			 * followed by MAD_BUFFER_GUARD zero bytes if one wants to
			 * decode that last frame. When the end of file is
			 * detected we append that quantity of bytes at the end of
			 * the available data. Note that the buffer can't overflow
			 * as the guard size was allocated but not used the the
			 * buffer management code. (See also the comment marked
			 * {1}.)
			 *
			 * In a message to the mad-dev mailing list on May 29th,
			 * 2001, Rob Leslie explains the guard zone as follows:
			 *
			 *    "The reason for MAD_BUFFER_GUARD has to do with the
			 *    way decoding is performed. In Layer III, Huffman
			 *    decoding may inadvertently read a few bytes beyond
			 *    the end of the buffer in the case of certain invalid
			 *    input. This is not detected until after the fact. To
			 *    prevent this from causing problems, and also to
			 *    ensure the next frame's main_data_begin pointer is
			 *    always accessible, MAD requires MAD_BUFFER_GUARD
			 *    (currently 8) bytes to be present in the buffer past
			 *    the end of the current frame in order to decode the
			 *    frame."
			 */

			/* Pipe the new buffer content to libmad's stream decoder
             * facility.
			 */
			mad_stream_buffer(&af->stream, af->in_buffer, read_size + remaining);
			af->stream.error = 0;
		}

		/* Decode the next MPEG frame. The streams is read from the
		 * buffer, its constituents are break down and stored the the
		 * Frame structure, ready for examination/alteration or PCM
		 * synthesis. Decoding options are carried in the Frame
		 * structure from the Stream structure.
		 *
		 * Error handling: mad_frame_decode() returns a non zero value
		 * when an error occurs. The error condition can be checked in
		 * the error member of the Stream structure. A mad error is
		 * recoverable or fatal, the error status is checked with the
		 * MAD_RECOVERABLE macro.
		 *
		 * {4} When a fatal error is encountered all decoding
		 * activities shall be stopped, except when a MAD_ERROR_BUFLEN
		 * is signaled. This condition means that the
		 * mad_frame_decode() function needs more input to complete
		 * its work. One shall refill the buffer and repeat the
		 * mad_frame_decode() call. Some bytes may be left unused at
		 * the end of the buffer if those bytes forms an incomplete
		 * frame. Before refilling, the remaining bytes must be moved
		 * to the beginning of the buffer and used for input for the
		 * next mad_frame_decode() invocation. (See the comments
		 * marked {2} earlier for more details.)
		 *
		 * Recoverable errors are caused by malformed bit-streams, in
		 * this case one can call again mad_frame_decode() in order to
		 * skip the faulty part and re-sync to the next frame.
		 */
		if(mad_frame_decode(&af->frame, &af->stream))
		{
            printf("************************************************\n");
			if(MAD_RECOVERABLE(af->stream.error))
			{
				/* Do not print a message if the error is a loss of
				 * synchronization and this loss is due to the end of
				 * stream guard bytes. (See the comments marked {3}
				 * supra for more informations about guard bytes.)
				 */
				if(af->stream.error != MAD_ERROR_LOSTSYNC ||
				   af->stream.this_frame != af->GuardPtr)
				{
					fprintf(stderr,"recoverable frame level error\n");
					fflush(stderr);
				}
                return sbuffer_create(0, 0);
			}
			else
            {
				if(af->stream.error == MAD_ERROR_BUFLEN)
                {
                    return sbuffer_create(0, 0);
                }
				else
				{
					fprintf(stderr,"unrecoverable frame level error\n");
					break;
				}
            }
		}


		/* Once decoded the frame is synthesized to PCM samples. No errors
		 * are reported by mad_synth_frame();
		 */
		mad_synth_frame(&af->synth, &af->frame);


        if (af->synth.pcm.samplerate != 0) {
            af->samplerate = af->synth.pcm.samplerate;
        }

        if (af->synth.pcm.channels != 0) {
            af->channels = af->synth.pcm.channels;
        }

        nsmps    = af->synth.pcm.length;
        channels = af->synth.pcm.channels;
        smps     = sbuffer_create(channels, nsmps);

        for (i = 0; i < channels; i++) {
            for (j = 0; j < nsmps; j++) {
                smps->buffers[i][j] = madpcm2short(af->synth.pcm.samples[i][j])/32768.0;
            }
        }

        return smps;

	}while(1);


    return sbuffer_create(0, 0);

}



void mp3_input_cleanup(MP3InputFile af) {

    mad_synth_finish(&af->synth);
    mad_frame_finish(&af->frame);
    mad_stream_finish(&af->stream);

    fclose(af->sf);

    free(af);
}



MP3OutputFile mp3_write_file(char *filename,
                             int channels,
                             int samplerate,
                             int bitrate,
                             int mode,
                             int quality,
                             int window_size) {

    MP3OutputFile af = (MP3OutputFile) malloc(sizeof(MP3File_Output_Data));

    af->filename = filename;
    af->sf = fopen(af->filename, "wb+");


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

    af->buf_size = (1.25 * (float) window_size) + 7200;

    af->buf = (unsigned char *) malloc(sizeof(unsigned char) * af->buf_size);

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



#ifdef MP3_AUDIO_TEST

int main() {

    Samples smps;
    MP3InputFile  inf = mp3_read_file("test_input.mp3");
    MP3OutputFile of  = mp3_write_file("test_output.mp3",
                                       inf->channels,
                                       inf->samplerate,
                                       128,
                                       0,
                                       0,
                                       1024);

    while (inf->finished != 1) {
        smps = mp3_get_audio(inf);
        printf("size %i\n", smps->size);
        if (smps != NULL) {
            mp3_write_data(of, smps);
        }
    }

    mp3_input_cleanup(inf);
    mp3_output_cleanup(of);

    return 0;
}

#endif


