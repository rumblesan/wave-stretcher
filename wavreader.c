#include "main.h"
#include <stdlib.h>
#include <stdio.h>



void read_wav(char filename[], struct wavefiledata *wavedata) {

    FILE *fp;
    fp = fopen(filename, "r");

    fread(&wavedata->ChunkID,4, 1, fp);
    fread(&wavedata->ChunkDataSize,4, 1, fp);
    fread(&wavedata->RiffType,4, 1, fp);

    fread(&wavedata->SubChunk1ID,4, 1, fp);
    fread(&wavedata->SubChunk1Size,4, 1, fp);

    fread(&wavedata->AudioFormat,2, 1, fp);
    fread(&wavedata->NumberChannels,2, 1, fp);
    fread(&wavedata->SampleRate,4, 1, fp);
    fread(&wavedata->ByteRate,4, 1, fp);
    fread(&wavedata->BlockAlign,2, 1, fp);
    fread(&wavedata->BitsPerSample,2, 1, fp);

    fread(&wavedata->SubChunk2ID,4, 1, fp);
    fread(&wavedata->SubChunk2Size,4, 1, fp);

    wavedata->waveData = (short*)malloc(wavedata->SubChunk2Size);
    fread(wavedata->waveData,2, wavedata->SubChunk2Size, fp);

    fclose (fp);
}

void write_wav(char filename[], struct wavefiledata *wavedata) {

    FILE *fp;
    fp = fopen(filename, "w");

    fwrite(&wavedata->ChunkID,4, 1, fp);
    fwrite(&wavedata->ChunkDataSize,4, 1, fp);
    fwrite(&wavedata->RiffType,4, 1, fp);

    fwrite(&wavedata->SubChunk1ID,4, 1, fp);
    fwrite(&wavedata->SubChunk1Size,4, 1, fp);

    fwrite(&wavedata->AudioFormat,2, 1, fp);
    fwrite(&wavedata->NumberChannels,2, 1, fp);
    fwrite(&wavedata->SampleRate,4, 1, fp);
    fwrite(&wavedata->ByteRate,4, 1, fp);
    fwrite(&wavedata->BlockAlign,2, 1, fp);
    fwrite(&wavedata->BitsPerSample,2, 1, fp);

    fwrite(&wavedata->SubChunk2ID,4, 1, fp);
    fwrite(&wavedata->SubChunk2Size,4, 1, fp);

    fwrite(wavedata->waveData,2, wavedata->SubChunk2Size, fp);

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

