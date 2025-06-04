/****************************************************************/
/*								*/
/*	wavplay.c						*/
/*	Sample .WAV player for xpat2.				*/
/*	1994 by M. Bischoff					*/
/*								*/
/*	To use this with xpat2, do the following:		*/
/*	1) compile this program with				*/
/*	   $ gcc -s -O wavplay.c -o wavplay			*/
/*	2) Then, make a named pipe /tmp/audio:			*/
/*	   $ mknod /tmp/audio p					*/
/*	3) Compile the module X-sound_SUN.c with the		*/
/*	   additional option -DAUDIO_DEVICE=\"/tmp/audio\"	*/
/*	   to redirect sound output to the named pipe.		*/
/* 	4) start the WAV-player:				*/
/*	   $ wavplay &						*/
/*	5) substitute all .au files with .wav files, but keep   */
/*	   the original name.					*/
/*	6) start xpat2:						*/
/*	   $ xpat2						*/
/*	Have fun! (and don't forget to kill wavplay later)	*/
/*								*/
/****************************************************************/

#include <sys/soundcard.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

struct wavfmt {
    char c_RIFF[4];
    long len_file;
    char c_WAVE[4];

    char c_fmt[4];
    long len_fmt;	/* excluding c_fmt & len_fmt. 16 */
    short monostereo;
    short numchannels;
    long samplerate;
    long bytespersecond;
    short bytespersample;
    short bitspersample;

    char c_data[4];
    long len_data;
} wav_hdr;

static void stat8(unsigned char *p, int *_l, int *_u, int n) {
    int l = 0, u = 0;
    while (n--) {
	int c;
	c = ((int)*p++ - 0x80) << 8;
	if (c < l)
	    l = c;
	if (c > u)
	    u = c;
    }
    *_u = u;
    *_l = l;
}

static void stat16(short *p, int *_l, int *_u, int n) {
    int l = 0, u = 0;
    while (n--) {
	if (*p < l)
	    l = *p;
	if (*p > u)
	    u = *p;
	++p;
    }
    *_u = u;
    *_l = l;
}

#if 0
static void wav_stat(void *data, const char *filename) {
    int l, u;
    printf("%s is %d bit %s at %ld Hz, %ld samples\n",
	   filename,
	   wav_hdr.bitspersample,
	   wav_hdr.monostereo != 1 ? "STEREO" : "MONO",
	   wav_hdr.samplerate,
	   wav_hdr.len_data);

    if (wav_hdr.monostereo != 1)
	return;		/* cannot handle */
    if (wav_hdr.bitspersample == 8)
	stat8((unsigned char *)data, &l, &u, wav_hdr.len_data);
    else if (wav_hdr.bitspersample == 16)
	stat16((short *)data, &l, &u, wav_hdr.len_data / 2);
    else
	return;
    {   double f1, f2, factor;
	f1 = l < 0 ? -32768.0 / l : 65535.0;
	f2 = u > 0 ?  32767.0 / u : 65535.0;
	factor = f1 < f2 ? f1 : f2;
	printf("min is -%04xH, max is %04xH, scale with %f for full amplitude\n", -l, u, factor);
    }
}
#endif

static double abbreviation = 1.0;	/* playing speed factor */
static double volumescale = 1.0;	/* volume scaling factor */
static int writefile = 0;

static int fd_audio;

static void wav_play(unsigned char *data, long len, const char *filename) {
    long i;
    const char *fmt;
    fmt = (wav_hdr.monostereo != 1) ? "STEREO" : "MONO";

    if ((fd_audio=open("/dev/dsp", O_WRONLY, 0)) == EOF) {
	fprintf(stderr, "Cannot open /dev/dsp");
	exit(1);
    }

    /* speed adjust */
    i = wav_hdr.samplerate;
    wav_hdr.samplerate = (long)((double)wav_hdr.samplerate * abbreviation);
    if (wav_hdr.samplerate > 22222) {
	long j;
	wav_hdr.samplerate /= 2;
	len /= 2;
	if (wav_hdr.bitspersample == 8)
	    for (j = 0; j < len; ++j)
		data[j] = data[2*j];
	else
	    for (j = 0; j < len; j += 2) {
		data[j]   = data[2*j];
		data[j+1] = data[2*j+1];
	    }
    }

    /* volume scale */
    if (wav_hdr.bitspersample == 16) {
	/* 16 bit => 8 bit & scaling */
	long j, clip = 0;
	double new;
	unsigned char *wr;
	short *rd;
	wr = data;
	rd = (short *)data;
	len /= 2;
	for (j = 0; j < len; ++j) {
	    new = (*rd++ * volumescale) + 32768.0;
	    if (new < 0.0) {
		new = 0.0;
		++clip;
	    } else if (new > 65535.5) {
		new = 65535.0;
		++clip;
	    }
	    *wr++ = (unsigned char)(new / 256.0);
	    if (clip == 1) {
		printf("warning: clipping at sample %ld\n", j);
		++clip;
	    }
	}
	if (clip)
	    printf("%ld samples clipped\n", clip-1);
	wav_hdr.bytespersecond >>= 1;
	wav_hdr.bytespersample >>= 1;
	wav_hdr.bitspersample >>= 1;
	wav_hdr.len_data >>= 1;
    } else if (volumescale != 1.0) {
	long j, clip = 0;
	double new;
	printf(" *** warning: volume scaling on 8 bit data may reduce quality ***\n");

	for (j = 0; j < len; ++j) {
	    new = ((double)data[j] - 128.0) * volumescale + 128.0;
	    if (new < 0.0) {
		new = 0.0;
		++clip;
	    } else if (new >= 255.5) {
		++clip;
		new = 255.0;
	    }
	    data[j] = (unsigned char)new;
	    if (clip == 1) {
		printf("warning: clipping at sample %ld\n", j);
		++clip;
	    }
	}
	if (clip)
	    printf("%ld samples clipped\n", clip-1);
    }
    ioctl(fd_audio, SNDCTL_DSP_SPEED, &wav_hdr.samplerate);
#if 0
    printf("%8ld bytes (%s, %2d bits) %ld (%ld) Hz, file \"%s\", %d channels\n",
	   len, fmt, wav_hdr.bitspersample, i, wav_hdr.samplerate, filename,
	   wav_hdr.numchannels);
#endif
    if (wav_hdr.monostereo != 1)
	return;		/* cannot handle */
    write(fd_audio, data, len);
    close(fd_audio);

    if (writefile) {
	FILE *fp;
	fp = fopen("/tmp/audio.wav", "wb");
	fwrite(&wav_hdr, 1, sizeof(struct wavfmt), fp);
	fwrite(data, 1, wav_hdr.len_data, fp);
	fclose(fp);
    }
}

static void *readfile(const char *filename) {
    int fd;
    size_t cnt;
    void *data;

    if ((fd = open(filename, O_RDONLY)) < 0) {
	fprintf(stderr, "failed to open %s\n", filename);
	return NULL;
    }
    if ((cnt = read(fd, &wav_hdr, sizeof(wav_hdr))) != sizeof(wav_hdr)) {
	fprintf(stderr, "failed to read %s: only got %d byte\n", filename, cnt);
	close(fd);
	return NULL;
    }
#if 0
    fprintf(stderr, "audio data length is %d byte\n", wav_hdr.len_data);
    fprintf(stderr, "first bytes are %c%c%c%c \n",
	    wav_hdr.c_RIFF[0], wav_hdr.c_RIFF[1],
	    wav_hdr.c_RIFF[2], wav_hdr.c_RIFF[3]);
#endif
    if (!(data = malloc(wav_hdr.len_data))) {
	fprintf(stderr, "not enough memory to read %s\n", filename);
	close(fd);
	return NULL;
    }
    /* read the data. This may loop */
    {   int rest, this;
	char *ptr;
	rest = wav_hdr.len_data;
	ptr = data;
	while (rest && (this = read(fd, ptr, rest))) {
	    ptr += this;
	    rest -= this;
	}
	if (rest) {
	    fprintf(stderr, "cannot read chunk, %s\n", filename);
	    close(fd);
	    free(data);
	    return NULL;
	}
    }
    close(fd);
    return data;
}

int main(int argc, char *argv[]) {
    for (;;) {
	void *data;
	if (!(data = readfile("/tmp/audio")))
	    continue;
	wav_play(data, wav_hdr.len_data, "/tmp/audio");
	free(data);
    }
    return 0;
}
