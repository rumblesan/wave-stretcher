#include <stdlib.h>
#include <sndfile.h>
#include "wavreader.h"


AudioFile read_wav(char *filename) {

    AudioFile af = (AudioFile) malloc(sizeof(AudioFile_Data));

    af->filename = filename;

    af->info.format = 0;
    af->sf = sf_open(af->filename, SFM_READ, &af->info);

    af->sound_data = (float*) malloc(sizeof(float) * af->info.channels * (int)af->info.frames);
    sf_readf_float(af->sf, af->sound_data, af->info.frames);

    return af;
}

void free_wav(AudioFile af) {

    free(af->sound_data);
    sf_close(af->sf);
}

void write_wav(AudioFile af) {

    sf_count_t frame_num = af->info.frames;
    af->sf = sf_open(af->filename, SFM_WRITE, &af->info);
    if (af->sf == NULL) {
        printf("ERROR OPENING\n");
    } else {
        sf_writef_float(af->sf, af->sound_data, frame_num);
        sf_close(af->sf);
    }
}

