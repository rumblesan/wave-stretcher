#include <fftw3.h>

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

void read_wav(char filename[], struct wavefiledata *wavedata);
void write_wav(char filename[], struct wavefiledata *wavedata);

void print_wav(struct wavefiledata *wavedata);



struct stretch_data {

    float *input_data;
    int    input_size;
    float  input_offset;


    float *output_data;
    int    output_size;
    int    output_offset;

    int finished;

    float ratio;

    int channels;

    int window_size;

    float *buffers[];
};

void setup_stretch(struct stretch_data *s,
                   float *wavdata,
                   int input_size,
                   int window_size,
                   int channels,
                   float ratio);

void next_input_section(struct stretch_data *s);
void add_output(struct stretch_data *s, float *buffers[]);

#ifdef TEST_SUITE
void print_next_section(struct stretch_data *s);
void print_output(struct stretch_data *s);
void test_next_input_section();
void test_add_output()
#endif



struct fft_data {

    int num_samples;

    float *window;

    float *data;
    fftwf_plan forward, inverse;

};

void setup_fft (struct fft_data *d, int N);




