#include <stdlib.h>
#include "core/sample.h"

Samples sbuffer_create(int channels, int size) {
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

void sbuffer_cleanup(Samples smps) {
    int i;
    for (i = 0; i < smps->channels; i++) {
        free(smps->buffers[i]);
    }
    free(smps->buffers);
    free(smps);
}

