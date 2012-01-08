#include <stdlib.h>
#include <sndfile.h>
#include "audiodata.h"


AudioFile read_audio_file(char *filename) {

    AudioFile af = (AudioFile) malloc(sizeof(AudioFile_Data));

    af->filename = filename;
    af->finished = 0;

    af->info.format = 0;
    af->sf = sf_open(af->filename, SFM_READ, &af->info);

    return af;
}

AudioFile write_audio_file(char *filename,
                           int samplerate,
                           int channels,
                           int format) {

    AudioFile af = (AudioFile) malloc(sizeof(AudioFile_Data));

    af->filename = filename;

    af->info.samplerate = samplerate;
    af->info.channels   = channels;
    af->info.format     = format;
    af->sf = sf_open(af->filename, SFM_WRITE, &af->info);

    return af;
}

Samples get_audio_data(AudioFile af, int size) {

    int channels = af->info.channels;
    int i,j;

    int buffer_size = size * channels;
    int read_amount;
    int pos;

    float iobuffer[buffer_size];
    read_amount = (int) sf_readf_float(af->sf, iobuffer, size);
    if (read_amount < size) {
        af->finished = 1;
        for (i = read_amount*channels; i < buffer_size; i++) {
            iobuffer[i] = 0.0;
        }
    }

    Samples smps = create_sample_buffer(channels, size);
    for (i = 0; i < channels; i++) {
        for (j = 0; j < size; j++) {
            pos = (j * channels) + i;
            smps->buffers[i][j] = iobuffer[pos];
        }
    }

    return smps;

}

void write_audio_data(AudioFile af, Samples smps) {

    int i,j;
    int pos;
    int buffer_size = smps->channels * smps->size;
    float iobuffer[buffer_size];
    for (i = 0; i < smps->channels; i++) {
        for (j = 0; j < smps->size; j++) {
            pos = (j * smps->channels) + i;
            iobuffer[pos] = smps->buffers[i][j];
        }
    }

    sf_writef_float(af->sf, iobuffer, smps->size);

    cleanup_sample_buffer(smps);
}

void cleanup_audio_file(AudioFile af) {
    sf_close(af->sf);
    free(af);
}

Samples create_sample_buffer(int channels, int size) {
    Samples smps   = (Samples) malloc(sizeof(Samples_Data));
    smps->size     = size;
    smps->channels = channels;

    smps->buffers = (float**) malloc(sizeof(float*) * channels);
    int i;
    for (i = 0; i < channels; i++) {
        smps->buffers[i] = (float*) malloc(sizeof(float*) * size);
    }

    return smps;
}

void cleanup_sample_buffer(Samples smps) {
    int i;
    for (i = 0; i < smps->channels; i++) {
        free(smps->buffers[i]);
    }
    free(smps->buffers);
    free(smps);
}


#ifdef AUDIO_TEST

int main() {

    int read_size = 20;
    Samples smps;
    AudioFile af = read_audio_file("input.wav");
    AudioFile of = write_audio_file("output.wav",
                                    af->info.samplerate,
                                    af->info.channels,
                                    af->info.format);

    while (af->finished != 1) {
        smps = get_audio_data(af, read_size);
        write_audio_data(of, smps);
    }

    cleanup_audio_file(af);
    cleanup_audio_file(of);

    return 0;
}

#endif


