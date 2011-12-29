#include "main.h"
#include <stdlib.h>
#include <stdio.h>



void read_wav(char filename[], struct wavefiledata *wavedata) {

    FILE *fp;
    fp = fopen(filename, "r");

    fread(&wavedata->ChunkID,        sizeof(char),  4, fp);
    fread(&wavedata->ChunkDataSize,  sizeof(int),   1, fp);
    fread(&wavedata->RiffType,       sizeof(char),  4, fp);

    fread(&wavedata->SubChunk1ID,    sizeof(char),  4, fp);
    fread(&wavedata->SubChunk1Size,  sizeof(int),   1, fp);

    fread(&wavedata->AudioFormat,    sizeof(short), 1, fp);

    fread(&wavedata->NumberChannels, sizeof(short), 1, fp);
    fread(&wavedata->SampleRate,     sizeof(int),   1, fp);
    fread(&wavedata->ByteRate,       sizeof(int),   1, fp);
    fread(&wavedata->BlockAlign,     sizeof(short), 1, fp);
    fread(&wavedata->BitsPerSample,  sizeof(short), 1, fp);

    fread(&wavedata->SubChunk2ID,    sizeof(char),  4, fp);
    fread(&wavedata->SubChunk2Size,  sizeof(int),   1, fp);

    wavedata->waveData = (short*)malloc(sizeof(char) * wavedata->SubChunk2Size);
    fread(wavedata->waveData,sizeof(char), wavedata->SubChunk2Size, fp);

    fclose (fp);
}

void write_wav(char filename[], struct wavefiledata *wavedata) {

    FILE *fp;
    fp = fopen(filename, "w");

    fwrite(&wavedata->ChunkID,        sizeof(char),  4, fp);
    fwrite(&wavedata->ChunkDataSize,  sizeof(int),   1, fp);
    fwrite(&wavedata->RiffType,       sizeof(char),  4, fp);

    fwrite(&wavedata->SubChunk1ID,    sizeof(char),  4, fp);
    fwrite(&wavedata->SubChunk1Size,  sizeof(int),   1, fp);

    fwrite(&wavedata->AudioFormat,    sizeof(short), 1, fp);

    fwrite(&wavedata->NumberChannels, sizeof(short), 1, fp);
    fwrite(&wavedata->SampleRate,     sizeof(int),   1, fp);
    fwrite(&wavedata->ByteRate,       sizeof(int),   1, fp);
    fwrite(&wavedata->BlockAlign,     sizeof(short), 1, fp);
    fwrite(&wavedata->BitsPerSample,  sizeof(short), 1, fp);

    fwrite(&wavedata->SubChunk2ID,    sizeof(char),  4, fp);
    fwrite(&wavedata->SubChunk2Size,  sizeof(int),   1, fp);

    printf("dataoutput size  %i\n", wavedata->SubChunk2Size);
    fwrite(wavedata->waveData,sizeof(char), wavedata->SubChunk2Size, fp);

    fclose (fp);
}


void print_wav(struct wavefiledata *wavedata) {

    printf("chunk ID         %.4s\n",wavedata->ChunkID);
    printf("chunk data size  %i\n",wavedata->ChunkDataSize);
    printf("riff type        %.4s\n",wavedata->RiffType);

    printf("subchunk1 id     %.4s\n",wavedata->SubChunk1ID);
    printf("subchunk1 size   %i\n",wavedata->SubChunk1Size);
    printf("audio format     %i\n",wavedata->AudioFormat);

    printf("channels         %i\n",wavedata->NumberChannels);
    printf("samplerate       %i\n",wavedata->SampleRate);
    printf("byterate         %i\n",wavedata->ByteRate);
    printf("block align      %i\n",wavedata->BlockAlign);
    printf("bits per sample  %i\n",wavedata->BitsPerSample);

    printf("subchunk2 id     %.4s\n",wavedata->SubChunk2ID);
    printf("subchunk2 size   %i\n",wavedata->SubChunk2Size);

}

#ifdef TEST_SUITE

void main() {

    struct wavefiledata wavedata;

    read_wav(&wavedata);
    print_wav(&wavedata);
    write_wav(&wavedata);
}

#endif

