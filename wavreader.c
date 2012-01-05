#include <stdlib.h>
#include <sndfile.h>
#include "wavreader.h"


AudioFile read_audio_file(char *filename) {

    AudioFile af = (AudioFile) malloc(sizeof(AudioFile_Data));

    af->filename = filename;
    af->finished = 0;

    af->info.format = 0;
    af->sf = sf_open(af->filename, SFM_READ, &af->info);

    return af;
}

Samples get_audio_data(AudioFile af, int size) {

    int channels = af->info.channels;
    int i,j;

    Samples smps = (Samples) malloc(sizeof(Samples_Data));
    smps->size = size;
    smps->channels = channels;

    smps->buffers = (float**) malloc(sizeof(float*) * channels);
    for (i = 0; i < channels; i++) {
        smps->buffers[i] = (float*) malloc(sizeof(float*) * size);
    }

    int buffer_size = size * channels;

    float iobuffer[buffer_size];
    int read_amount = sf_readf_float(af->sf, iobuffer, size);
    if (read_amount < buffer_size) {
        af->finished = 1;
        for (i = read_amount; i < buffer_size; i++) {
            iobuffer[i] = 0.0;
        }
    }

    for (i = 0; i < channels; i++) {
        for (j = 0; j < size; j++) {
            smps->buffers[i][j] = iobuffer[i+j];
        }
    }

    return smps;

}

void write_wav(AudioFile af) {

    /*
    sf_count_t frame_num = af->info.frames;
    af->sf = sf_open(af->filename, SFM_WRITE, &af->info);
    if (af->sf == NULL) {
        printf("ERROR OPENING\n");
    } else {
        sf_writef_float(af->sf, af->sound_data, frame_num);
        sf_close(af->sf);
    }
    */
}

void cleanup_audio_file(AudioFile af) {
    sf_close(af->sf);
    free(af);
}

void cleanup_sample_buffer(Samples smps) {
    int i;
    for (i = 0; i < smps->channels; i++) {
        free(smps->buffers[i]);
    }
    free(smps->buffers);
    free(smps);
}

