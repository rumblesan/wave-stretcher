#include <stdlib.h>
#include <sndfile.h>
#include "wavreader.h"


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

    sf_count_t frame_num = af->info.frames;
    af->sf = sf_open(af->filename, SFM_WRITE, &af->info);
    if (af->sf == NULL) {
        printf("ERROR OPENING\n");
    } else {
        sf_writef_float(af->sf, af->sound_data, frame_num);
        sf_close(af->sf);
    }
}

