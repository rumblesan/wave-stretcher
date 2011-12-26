

struct wavefiledata {

	char ChunkID[4];
	int ChunkDataSize;
	char RiffType[4];

	char SubChunk1ID[4];
	int SubChunk1Size;
	short AudioFormat;

	short NumberChannels;
	int SampleRate;
	int ByteRate;
	short BlockAlign;
	short BitsPerSample;

	char SubChunk2ID[4];
	int SubChunk2Size;

	short *waveData;

};

void read_wav(struct wavefiledata *wavedata);

void print_wav(struct wavefiledata *wavedata);

