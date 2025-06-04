/*
 * April 15, 1992
 * Copyright 1992 Rick Richardson
 * Copyright 1991 Lance Norskog And Sundry Contributors
 * This source code is freely redistributable and may be used for
 * any purpose.  This copyright notice must be maintained. 
 * Lance Norskog And Sundry Contributors are not responsible for 
 * the consequences of using this software.
 */

/*
 * Windows 3.0 .wav format driver
 */

/*
 * Fixed by various contributors:
 * 1) Little-endian handling
 * 2) Skip other kinds of file data
 * 3) Handle 16-bit formats correctly
 * 4) Not go into infinite loop
 */

#include "st.h"
#include "wav.h"

/* Private data for .wav file */
typedef struct wavstuff {
	long	samples;
} *wav_t;

IMPORT float volume, amplitude;
IMPORT long summary, verbose;

/*
 * Do anything required before you start reading samples.
 * Read file header. 
 *	Find out sampling rate, 
 *	size and style of samples, 
 *	mono/stereo/quad.
 */
wavstartread(ft) 
ft_t ft;
{
	wav_t	wav = (wav_t) ft->priv;
	char	magic[4];
	long	len;
	int	littlendian = 1;
	char	*endptr;
	char	c;

	endptr = (char *) &littlendian;
	if (!*endptr) ft->swap = 1;

	/* If you need to seek around the input file. */
	if (0 && ! ft->seekable)
		fail("Sorry, .wav input file must be a file, not a pipe");

	fread(magic, 4, 1, ft->fp);
	if (strncmp("RIFF", magic, 4))
		fail("Sorry, not a RIFF file");

	len = rllong(ft);

	fread(magic, 4, 1, ft->fp);
	if (strncmp("WAVE", magic, 4))
		fail("Sorry, not a WAVE file");

	/* Skip to the next "fmt " or end of file */
	while(1) {
		if (feof(ft->fp))
			fail("Sorry, missing fmt spec");
		fread(magic, 4, 1, ft->fp);
		if (! strncmp("fmt ", magic, 4))
			break;
		for (len = rllong(ft); len>0; len--)
			fread(&c,1,1,ft->fp);
	}

	len = rllong(ft);
	switch (rlshort(ft))
	{
		case WAVE_FORMAT_UNKNOWN:
		fail("Sorry, this WAV file is in Microsoft Official Unknown format.");
		case WAVE_FORMAT_PCM: 	/* this one, at least, I can handle */
			break;
		case WAVE_FORMAT_ADPCM:
		fail("Sorry, this WAV file is in Microsoft ADPCM format.");
		case WAVE_FORMAT_ALAW:
		fail("Sorry, this WAV file is in Microsoft A-law format.");
		case WAVE_FORMAT_MULAW:
		fail("Sorry, this WAV file is in Microsoft U-law format.");
		case WAVE_FORMAT_OKI_ADPCM:
		fail("Sorry, this WAV file is in OKI ADPCM format.");
		case WAVE_FORMAT_DIGISTD:
		fail("Sorry, this WAV file is in Digistd format.");
		case WAVE_FORMAT_DIGIFIX:
		fail("Sorry, this WAV file is in Digifix format.");
		case IBM_FORMAT_MULAW:
		fail("Sorry, this WAV file is in IBM U-law format.");
		case IBM_FORMAT_ALAW:
		fail("Sorry, this WAV file is in IBM A-law format.");
		case IBM_FORMAT_ADPCM:
		fail("Sorry, this WAV file is in IBM ADPCM format.");
	default:	fail("Sorry, don't understand format");
	}
	ft->info.channels = rlshort(ft);
	ft->info.rate = rllong(ft);
	rllong(ft);	/* Average bytes/second */
	rlshort(ft);	/* Block align */
	switch (rlshort(ft))
	{
	case 8:		ft->info.size = BYTE; ft->info.style = UNSIGNED; break;
	case 16:	ft->info.size = WORD; ft->info.style = SIGN2; break;
	case 32:	ft->info.size = LONG; ft->info.style = SIGN2; break;
	default:	fail("Sorry, don't understand size");
	}
	len -= 16;
	while (--len >= 0) getc(ft->fp);

	fread(magic, 4, 1, ft->fp);
	if (strncmp("data", magic, 4))
		fail("Sorry, missing data portion");

	wav->samples = rllong(ft) / ft->info.size;
}

/*
 * Read up to len samples from file.
 * Convert to signed longs.
 * Place in buf[].
 * Return number of samples read.
 */

wavread(ft, buf, len) 
ft_t ft;
long *buf, len;
{
	wav_t	wav = (wav_t) ft->priv;
	int	done;

	if (len > wav->samples) len = wav->samples;
	if (len == 0) return 0;
	done = rawread(ft, buf, len);
	if (done == 0) report("Premature EOF on .wav input file");
	wav->samples -= done;
	return done;
}

/*
 * Do anything required when you stop reading samples.  
 * Don't close input file! 
 */
wavstopread(ft) 
ft_t ft;
{
}

wavstartwrite(ft) 
ft_t ft;
{
	wav_t	wav = (wav_t) ft->priv;
	int	littlendian = 1;
	char	*endptr;

	if (! ft->seekable)
		fail("Sorry, output .wav file must be a file, not a pipe");

	endptr = (char *) &littlendian;
	if (!*endptr) ft->swap = 1;

	wav->samples = 0;
	wavwritehdr(ft);
}

wavwritehdr(ft) 
ft_t ft;
{
	wav_t	wav = (wav_t) ft->priv;
	int	samsize;
	long	datasize;

	switch (ft->info.size)
	{
	case BYTE:	samsize = 8; ft->info.style = UNSIGNED; break;
	case WORD:	samsize = 16; ft->info.style = SIGN2; break;
	default:
	case LONG:	ft->info.size = LONG; ft->info.style = SIGN2; 
			samsize = 32; break;
	}
	

	datasize = ft->info.size * wav->samples;

	fputs("RIFF", ft->fp);
	wllong(ft, datasize + 8+16+12);	/* Waveform chunk size: FIXUP(4) */
	fputs("WAVE", ft->fp);
	fputs("fmt ", ft->fp);
	wllong(ft, (long)16);		/* fmt chunk size */
	wlshort(ft, 1);			/* FormatTag: WAVE_FORMAT_PCM */
	wlshort(ft, ft->info.channels);
	wllong(ft, (long)ft->info.rate);	/* SamplesPerSec */
					/* Average Bytes/sec */
	wllong(ft, ((long)ft->info.rate * ft->info.channels * samsize + 7) / 8);
					/* nBlockAlign */
	wlshort(ft, (ft->info.channels * samsize + 7) / 8);
	wlshort(ft, samsize);		/* BitsPerSample */
	
	fputs("data", ft->fp);
	wllong(ft, datasize);		/* data chunk size: FIXUP(40) */
}

wavwrite(ft, buf, len) 
ft_t ft;
long *buf, len;
{
	wav_t	wav = (wav_t) ft->priv;

	wav->samples += len;
	rawwrite(ft, buf, len);
}

void
wavstopwrite(ft) 
ft_t ft;
{
	/* All samples are already written out. */
	/* If file header needs fixing up, for example it needs the */
 	/* the number of samples in a field, seek back and write them here. */
	if (!ft->seekable)
		return;
	if (fseek(ft->fp, 0L, 0) != 0)
		fail("Sorry, can't rewind output file to rewrite .wav header.");
	wavwritehdr(ft);
}
