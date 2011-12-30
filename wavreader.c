#include <stdlib.h>
#include <stdio.h>
#include <sndfile.h>
#include "main.h"


void read_wav(struct audio_file *af) {

    af->info.format = 0;
    af->sf = sf_open(af->filename, SFM_READ, &af->info);

    af->sound_data = (float*) malloc(sizeof(float) * af->info.channels * (int)af->info.frames);
    sf_readf_float(af->sf, af->sound_data, af->info.frames);

}

void free_wav(struct audio_file *af) {

    free(af->sound_data);
    sf_close(af->sf);
}

void write_wav(struct audio_file *af) {

    SNDFILE *of = sf_open(af->filename, SFM_WRITE, &af->info);
    sf_writef_float(of, af->sound_data, af->info.frames);
    sf_close(of);
}

void print_wav(struct audio_file *af) {

    printf("%i frames\n", (int)af->info.frames);
    printf("%i samplerate\n", af->info.samplerate);
    printf("%i channels\n", af->info.channels);
    printf("%i format\n", af->info.format);
    printf("%i sections\n", af->info.sections);
    printf("%i seekable\n", af->info.seekable);

}

#ifdef WAV_TEST
void main() {

    struct audio_file af;
    af.filename = "test.wav";
    read_wav(&af);
    print_wav(&af);
    af.filename = "output.wav";
    write_wav(&af);

    /*
    int i;
    for (i = 0; i < 2000; i++) {
        printf("%f\n", af.sound_data[i]);
    }
    */
}
#endif


