#include <stdio.h>
#include <stdlib.h>
#include "main.h"


void main () {

    struct audio_file af;
    af.filename = "test.wav";

    read_wav(&af);

    int window_size = 1024;
    float ratio = 0.2;
    struct stretch_data sdata;
    setup_stretch(&sdata,
                  af.sound_data,
                  af.info.frames,
                  af.info.channels,
                  window_size,
                  ratio);

    struct fft_data fft;
    setup_fft(&fft, window_size);

    int i;
    while (sdata.finished != 1) {
        next_input_section(&sdata);
        for (i = 0; i < af.info.channels; i++) {
            get_data(&fft, sdata.buffers[i]);
            samp_to_freq(&fft);
            freq_to_samp(&fft);
            return_data(&fft, sdata.buffers[i]);
        }
        add_output(&sdata);
    }

    struct audio_file of;
    of.filename        = "output.wav";
    of.sf              = af.sf;
    of.info.samplerate = af.info.samplerate;
    of.info.channels   = af.info.channels;
    of.info.format     = af.info.format;

    write_wav(&of);

    cleanup_fft(&fft);
    free_wav(&af);

}


