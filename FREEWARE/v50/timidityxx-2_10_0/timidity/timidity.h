#ifndef ___TIMIDITY_H_
#define ___TIMIDITY_H_
/*
    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999,2000 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#if defined(__WIN32__) && !defined(__W32__)
#define __W32__
#endif


/* You could specify a complete path, e.g. "/etc/timidity.cfg", and
   then specify the library directory in the configuration file. */
/* #define CONFIG_FILE "/etc/timidity.cfg" */
#ifndef CONFIG_FILE

#ifndef VMS
#ifdef DEFAULT_PATH
#define CONFIG_FILE DEFAULT_PATH "/timidity.cfg"
#else
#define CONFIG_FILE PKGDATADIR "/timidity.cfg"
#endif /* DEFAULT_PATH */
#else
#define CONFIG_FILE	"timidity_dir:[000000]timidity.cfg"
#define DEFAULT_PATH	"timidity_dir:[000000]"
#endif /* VMS */
#endif /* CONFIG_FILE */

/* Filename extension, followed by command to run decompressor so that
   output is written to stdout. Terminate the list with a 0.

   Any file with a name ending in one of these strings will be run
   through the corresponding decompressor. If you don't like this
   behavior, you can undefine DECOMPRESSOR_LIST to disable automatic
   decompression entirely. */

#ifndef VMS
#define DECOMPRESSOR_LIST { \
			      ".gz",  "gunzip -c %s", \
			      ".bz2", "bunzip2 -c %s", \
			      ".Z",   "zcat %s", \
			      ".zip", "unzip -p %s", \
			      ".lha", "lha -pq %s", \
			      ".lzh", "lha -pq %s", \
			      ".shn", "shorten -x %s -", \
			     0 }
#endif /* VMS */

/* Define GUS/patch converter. */
#define PATCH_CONVERTERS { \
			     ".wav", "wav2pat %s", \
			     0 }

/* When a patch file can't be opened, one of these extensions is
   appended to the filename and the open is tried again.

   This is ignored for Windows, which uses only ".pat" (see the bottom
   of this file if you need to change this.) */
#ifndef VMS
#define PATCH_EXT_LIST { \
			   ".pat", \
			   ".shn", ".pat.shn", \
			   ".gz", ".pat.gz", \
			   ".bz2", ".pat.bz2", \
			   0 }
#else
#define PATCH_EXT_LIST { \
			   ".pat", \
			   ".shn", ".pat-shn", \
			   ".gz", ".pat-gz", \
			   ".bz2", ".pat-bz2", \
			   0 }
#endif   /* VMS */


/* Acoustic Grand Piano seems to be the usual default instrument. */
#define DEFAULT_PROGRAM 0


/* Specify drum channels (terminated with -1).
   10 is the standard percussion channel.
   Some files (notably C:\WINDOWS\CANYON.MID) think that 16 is one too.
   On the other hand, some files know that 16 is not a drum channel and
   try to play music on it. This is now a runtime option, so this isn't
   a critical choice anymore. */
#define DEFAULT_DRUMCHANNELS {10, -1}
/* #define DEFAULT_DRUMCHANNELS {10, 16, -1} */

/* type of floating point number */
typedef double FLOAT_T;
/* typedef float FLOAT_T; */


/* A somewhat arbitrary frequency range. The low end of this will
   sound terrible as no lowpass filtering is performed on most
   instruments before resampling. */
#define MIN_OUTPUT_RATE 	4000
#define MAX_OUTPUT_RATE 	65000


/* Master volume in percent. */
#define DEFAULT_AMPLIFICATION 	70


/* Default sampling rate, default polyphony, and maximum polyphony.
   All but the last can be overridden from the command line. */
#ifndef DEFAULT_RATE
#ifdef AU_AMD 
#define DEFAULT_RATE	 8000
#else
#define DEFAULT_RATE	22050   /* You can put 32000 or 44100 on fast Alphas */
#endif
#endif /* DEFAULT_RATE */

#define DEFAULT_VOICES	48
#define MAX_VOICES	256

/* The size of the internal buffer is 2^AUDIO_BUFFER_BITS samples.
   This determines maximum number of samples ever computed in a row.

   For Linux and FreeBSD users:

   This also specifies the size of the buffer fragment.  A smaller
   fragment gives a faster response in interactive mode -- 10 or 11 is
   probably a good number. Unfortunately some sound cards emit a click
   when switching DMA buffers. If this happens to you, try increasing
   this number to reduce the frequency of the clicks.

   For other systems:

   You should probably use a larger number for improved performance.

*/

#define AUDIO_BUFFER_BITS 12	/* Maxmum audio buffer size (2^bits) */

#ifndef DEFAULT_AUDIO_BUFFER_BITS
#ifdef __W32__
#define DEFAULT_AUDIO_BUFFER_BITS 12
#else
#ifdef AU_AMD
#define DEFAULT_AUDIO_BUFFER_BITS 10
#else
#define DEFAULT_AUDIO_BUFFER_BITS 12
#endif
#endif
#endif


/* 1000 here will give a control ratio of 22:1 with 22 kHz output.
   Higher CONTROLS_PER_SECOND values allow more accurate rendering
   of envelopes and tremolo. The cost is CPU time. */
#ifdef AU_AMD
#define CONTROLS_PER_SECOND  100
#else
#define CONTROLS_PER_SECOND 250
#endif


#if !defined(LINEAR_INTERPOLATION) && \
    !defined(CSPLINE_INTERPOLATION) && \
    !defined(LAGRANGE_INTERPOLATION) && \
    !defined(NO_INTERPOLATION)
/* Strongly recommended. This option increases CPU usage by half, but
   without it sound quality is very poor. */
#define LINEAR_INTERPOLATION

/* These option enable a multi-point interpolation in resampling.
   Defining CSPLINE_INTERPOLATION cause 4-point interpolation by cubic
   spline curve.  Defining LAGRANGE_INTERPOLATION cause 4-point
   interpolation by Lagrange method. */
/*#define CSPLINE_INTERPOLATION */
/* #define LAGRANGE_INTERPOLATION */
#endif


/* This is an experimental kludge that needs to be done right, but if
   you've got an 8-bit sound card, or cheap multimedia speakers hooked
   to your 16-bit output device, you should definitely give it a try.

   Defining LOOKUP_HACK causes table lookups to be used in mixing
   instead of multiplication. We convert the sample data to 8 bits at
   load time and volumes to logarithmic 7-bit values before looking up
   the product, which degrades sound quality noticeably.

   Defining LOOKUP_HACK should save ~20% of CPU on an Intel machine.
   LOOKUP_INTERPOLATION might give another ~5% */
/* #define LOOKUP_HACK
   #define LOOKUP_INTERPOLATION */
#ifdef AU_AMD                       /* Only with 8 bit interface on DEC/VMS */
#define LOOKUP_HACK
#define LOOKUP_INTERPOLATION
#endif


/* Make envelopes twice as fast. Saves ~20% CPU time (notes decay
   faster) and sounds more like a GUS. There is now a command line
   option to toggle this as well. */
#define FAST_DECAY


/* How many bits to use for the fractional part of sample positions.
   This affects tonal accuracy. The entire position counter must fit
   in 32 bits, so with FRACTION_BITS equal to 12, the maximum size of
   a sample is 1048576 samples (2 megabytes in memory). The GUS gets
   by with just 9 bits and a little help from its friends...
   "The GUS does not SUCK!!!" -- a happy user :) */
#define FRACTION_BITS 12


/* For some reason the sample volume is always set to maximum in all
   patch files. Define this for a crude adjustment that may help
   equalize instrument volumes. */
#define ADJUST_SAMPLE_VOLUMES


/* If you have root access, you can define DANGEROUS_RENICE and chmod
   timidity setuid root to have it automatically raise its priority
   when run -- this may make it possible to play MIDI files in the
   background while running other CPU-intensive jobs. Of course no
   amount of renicing will help if the CPU time simply isn't there.

   The root privileges are used and dropped at the beginning of main()
   in timidity.c -- please check the code to your satisfaction before
   using this option. (And please check sections 11 and 12 in the
   GNU General Public License (under GNU Emacs, hit ^H^W) ;) */
/* #define DANGEROUS_RENICE -15 */


/* The number of samples to use for ramping out a dying note. Affects
   click removal. */
#define MAX_DIE_TIME 20


/* On some machines (especially PCs without math coprocessors),
   looking up sine values in a table will be significantly faster than
   computing them on the fly. Uncomment this to use lookups. */
#define LOOKUP_SINE


/* Shawn McHorse's resampling optimizations. These may not in fact be
   faster on your particular machine and compiler. You'll have to run
   a benchmark to find out. */
#define PRECALC_LOOPS


/* If calling ldexp() is faster than a floating point multiplication
   on your machine/compiler/libm, uncomment this. It doesn't make much
   difference either way, but hey -- it was on the TODO list, so it
   got done. */
/* #define USE_LDEXP */


/* Define the pre-resampling cache size.
 * This value is default. You can change the cache saze with
 * command line option.
 */
#define DEFAULT_CACHE_DATA_SIZE (2*1024*1024)


#ifdef SUPPORT_SOCKET
/* Please define your mail domain address. */
#ifndef MAIL_DOMAIN
#define MAIL_DOMAIN "@someware.domain.com"
#endif /* MAIL_DOMAIN */

/* Please define your mail name if you are at Windows.
 * Otherwise (maybe unix), undefine this macro
 */
/* #define MAIL_NAME "somebody" */
#endif /* SUPPORT_SOCKET */


/* Where do you want to put temporary file into ?
 * If you are in UNIX, you can undefine this macro. If TMPDIR macro is
 * undefined, the value is used in environment variable `TMPDIR'.
 * If both macro and environment variable is not set, the directory is
 * set to /tmp.
 */
/* #define TMPDIR "/var/tmp" */
#ifdef VMS
#define TMPDIR "sys$scratch:"
#endif

/* To use GS drumpart setting. */
#define GS_DRUMPART

/**** Japanese section ****/
/* To use Japanese kanji code. */
/*#define JAPANESE*/

/* Select output text code:
 * "AUTO"	- Auto conversion by `LANG' environment variable (UNIX only)
 * "ASCII"	- Convert unreadable characters to '.'(0x2e)
 * "NOCNV"	- No conversion
 * "EUC"	- EUC
 * "JIS"	- JIS
 * "SJIS"	- shift JIS
 */

#ifndef JAPANESE
/* Not japanese (Select "ASCII" or "NOCNV") */
#define OUTPUT_TEXT_CODE "NOCNV"
#else
/* Japanese */
#ifndef __W32__
/* UNIX (Select "AUTO" or "ASCII" or "NOCNV" or "EUC" or "JIS" or "SJIS") */
#define OUTPUT_TEXT_CODE "AUTO"
#else
/* Windows (Select "ASCII" or "NOCNV" or "SJIS") */
#define OUTPUT_TEXT_CODE "SJIS"
#endif
#endif


/* Undefine if you don't use modulation wheel MIDI controls.
 * There is a command line option to enable/disable this mode.
 */
#define MODULATION_WHEEL_ALLOW


/* Undefine if you don't use portamento MIDI controls.
 * There is a command line option to enable/disable this mode.
 */
#define PORTAMENTO_ALLOW


/* Undefine if you don't use NRPN vibrato MIDI controls
 * There is a command line option to enable/disable this mode.
 */
#define NRPN_VIBRATO_ALLOW


/* Define if you want to use reverb controls in defaults.
 * This mode needs high CPU power.
 * There is a command line option to enable/disable this mode.
 */
/*#define REVERB_CONTROL_ALLOW*/


/* Define if you want to use chorus controls in defaults.
 * This mode needs high CPU power.
 * There is a command line option to enable/disable this mode.
 */
/*#define CHORUS_CONTROL_ALLOW*/


/* Define if you want to use channel pressure.
 * Channel pressure effect is different in sequencers.
 * There is a command line option to enable/disable this mode.
 */
/* #define GM_CHANNEL_PRESSURE_ALLOW */


/* Define if you want to trace text meta event at playing.
 * There is a command line option to enable/disable this mode.
 */
/* #define ALWAYS_TRACE_TEXT_META_EVENT */


/* Define if you want to allow overlapped voice.
 * There is a command line option to enable/disable this mode.
 */
#define OVERLAP_VOICE_ALLOW


/**************************************************************************/
/* Anything below this shouldn't need to be changed unless you're porting
   to a new machine with other than 32-bit, big-endian words. */
/**************************************************************************/


#include <stdio.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */
extern int errno;

#ifdef HAVE_MACHINE_ENDIAN_H
#include <machine/endian.h> /* for __byte_swap_*() */
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifndef NO_VOLATILE
#define VOLATILE_TOUCH(val) /* do nothing */
#define VOLATILE volatile
#else
extern int volatile_touch(void* dmy);
#define VOLATILE_TOUCH(val) volatile_touch(&(val))
#define VOLATILE
#endif /* NO_VOLATILE */

#define HAVE_SAFE_MALLOC

/* change FRACTION_BITS above, not these */
#define INTEGER_BITS (32 - FRACTION_BITS)
#define INTEGER_MASK (0xFFFFFFFF << FRACTION_BITS)
#define FRACTION_MASK (~ INTEGER_MASK)

/* This is enforced by some computations that must fit in an int */
#define MAX_CONTROL_RATIO 255

/* Audio buffer size has to be a power of two to allow DMA buffer
   fragments under the VoxWare (Linux & FreeBSD) audio driver */
#define AUDIO_BUFFER_SIZE (1<<AUDIO_BUFFER_BITS)

/* Byte order */
#ifdef WORDS_BIGENDIAN
#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif
#undef LITTLE_ENDIAN
#else
#undef BIG_ENDIAN
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif
#endif

/* DEC MMS has 64 bit long words */
/* Linux-Axp has also 64 bit long words */
#if defined(DEC) || defined(__alpha__)
typedef unsigned int uint32;
typedef int int32;
#else
typedef unsigned long uint32;
typedef long int32;
#endif
#ifdef HPUX
typedef unsigned short uint16;
typedef short int16;
typedef unsigned char uint8;
typedef char int8;
#else
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned char uint8;
typedef signed char int8;
#endif

/* Instrument files are little-endian, MIDI files big-endian, so we
   need to do some conversions. */

#define XCHG_SHORT(x) ((((x)&0xFF)<<8) | (((x)>>8)&0xFF))
#if defined(__i486__) && !defined(__i386__)
# define XCHG_LONG(x) \
     ({ int32 __value; \
        asm ("bswap %1; movl %1,%0" : "=g" (__value) : "r" (x)); \
       __value; })
#else
# define XCHG_LONG(x) ((((x)&0xFF)<<24) | \
		      (((x)&0xFF00)<<8) | \
		      (((x)&0xFF0000)>>8) | \
		      (((x)>>24)&0xFF))
#endif

#ifdef LITTLE_ENDIAN
#define LE_SHORT(x) (x)
#define LE_LONG(x) (x)
#ifdef __FreeBSD__
#define BE_SHORT(x) __byte_swap_word(x)
#define BE_LONG(x) __byte_swap_long(x)
#else
#define BE_SHORT(x) XCHG_SHORT(x)
#define BE_LONG(x) XCHG_LONG(x)
#endif
#else /* BIG_ENDIAN */
#define BE_SHORT(x) (x)
#define BE_LONG(x) (x)
#ifdef __FreeBSD__
#define LE_SHORT(x) __byte_swap_word(x)
#define LE_LONG(x) __byte_swap_long(x)
#else
#define LE_SHORT(x) XCHG_SHORT(x)
#define LE_LONG(x) XCHG_LONG(x)
#endif
#endif

#ifndef VMS
#define MAX_AMPLIFICATION 800
#else
#define MAX_AMPLIFICATION 256
#endif

#define MAX_CHANNELS 32
/*#define MAX_CHANNELS 256*/

#if MAX_CHANNELS <= 32
typedef uint32 ChannelBitMask; /* 32-bit bitvector */
#define IS_SET_CHANNELMASK(bits, c) ((bits) &   (1u << (c)))
#define SET_CHANNELMASK(bits, c)    ((bits) |=  (1u << (c)))
#define UNSET_CHANNELMASK(bits, c)  ((bits) &= ~(1u << (c)))
#define TOGGLE_CHANNELMASK(bits, c) ((bits) ^=  (1u << (c)))
#else
typedef struct _ChannelBitMask
{
    uint32 b[8];		/* 256-bit bitvector */
} ChannelBitMask;
#define IS_SET_CHANNELMASK(bits, c) \
	((bits).b[((c) >> 5) & 0x7] &   (1u << ((c) & 0x1F)))
#define SET_CHANNELMASK(bits, c) \
	((bits).b[((c) >> 5) & 0x7] |=  (1u << ((c) & 0x1F)))
#define UNSET_CHANNELMASK(bits, c) \
	((bits).b[((c) >> 5) & 0x7] &= ~(1u << ((c) & 0x1F)))
#define TOGGLE_CHANNELMASK(bits, c) \
	((bits).b[((c) >> 5) & 0x7] ^=  (1u << ((c) & 0x1F)))
#endif

#define MAXMIDIPORT 16

/* These affect general volume */
#define GUARD_BITS 3
#define AMP_BITS (15-GUARD_BITS)

#ifdef LOOKUP_HACK
   typedef int8 sample_t;
   typedef uint8 final_volume_t;
#  define FINAL_VOLUME(v) (~_l2u[v])
#  define MIXUP_SHIFT 5
#  define MAX_AMP_VALUE 4095
#else
   typedef int16 sample_t;
   typedef int32 final_volume_t;
#  define FINAL_VOLUME(v) (v)
#  define MAX_AMP_VALUE ((1<<(AMP_BITS+1))-1)
#endif
#define MIN_AMP_VALUE (MAX_AMP_VALUE >> 9)

#ifdef USE_LDEXP
#  define TIM_FSCALE(a,b) ldexp((double)(a),(b))
#  define TIM_FSCALENEG(a,b) ldexp((double)(a),-(b))
#  include <math.h>
#else
#  define TIM_FSCALE(a,b) ((a) * (double)(1<<(b)))
#  define TIM_FSCALENEG(a,b) ((a) * (1.0L / (double)(1<<(b))))
#endif

/* Vibrato and tremolo Choices of the Day */
#define SWEEP_TUNING 38
#define VIBRATO_AMPLITUDE_TUNING 1.0L
#define VIBRATO_RATE_TUNING 38
#define TREMOLO_AMPLITUDE_TUNING 1.0L
#define TREMOLO_RATE_TUNING 38

#define SWEEP_SHIFT 16
#define RATE_SHIFT 5

#define VIBRATO_SAMPLE_INCREMENTS 32

#define MODULATION_WHEEL_RATE (1.0/6.0)
/* #define MODULATION_WHEEL_RATE (midi_time_ratio/8.0) */
/* #define MODULATION_WHEEL_RATE (current_play_tempo/500000.0/32.0) */

#define VIBRATO_DEPTH_TUNING (1.0/4.0)

#ifdef HPUX
#undef mono
#endif

#ifdef sun
#ifndef SOLARIS
/* SunOS 4.x */
#include <sys/stdtypes.h>
#include <memory.h>
#define memcpy(x, y, n) bcopy(y, x, n)
#else
/* Solaris */
int usleep(unsigned int useconds); /* shut gcc warning up */
#endif
#endif /* sun */


#ifdef __W32__
#undef PATCH_EXT_LIST
#define PATCH_EXT_LIST { ".pat", 0 }

#define URL_DIR_CACHE_DISABLE
#endif

/* The path separator (D.M.) */
/* Windows: "\"
 * Cygwin:  both "\" and "/"
 * Macintosh: ":"
 * Unix: "/"
 */
#if defined(__W32__)
#  define PATH_SEP '\\'
#  define PATH_STRING "\\"
#if defined(__CYGWIN32__) || defined(__MINGW32__)
#  define PATH_SEP2 '/'
#endif
#elif defined(__MACOS__)
#  define PATH_SEP ':'
#  define PATH_STRING ":"
#else
#  define PATH_SEP '/'
#  define PATH_STRING "/"
#endif

#ifdef PATH_SEP2
#define IS_PATH_SEP(c) ((c) == PATH_SEP || (c) == PATH_SEP2)
#else
#define IS_PATH_SEP(c) ((c) == PATH_SEP)
#endif

/* new line code */
#ifndef NLS
#ifdef __W32__
#if defined(__BORLANDC__) || defined(__CYGWIN32__) || defined(__MINGW32__)
#  define NLS "\n"
#else
#  define NLS "\r\n"
#endif
#else /* !__W32__ */
#  define NLS "\n"
#endif
#endif /* NLS */

#define MAX_SAFE_MALLOC_SIZE (1<<22) /* 4M */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif /* M_PI */

#ifndef __W32__
#undef MAIL_NAME
#endif /* __W32__ */


#ifdef __BORLANDC__
/* strncasecmp() -> strncmpi(char *,char *,size_t) */
#define strncasecmp(a,b,c) strncmpi(a,b,c)
#define strcasecmp(a,b) strcmpi(a,b)
#endif /* __BORLANDC__ */

#ifdef _MSC_VER
#define strncasecmp(a,b,c)	_strnicmp((a),(b),(c))
#define strcasecmp(a,b)		_stricmp((a),(b))
#define open _open
#define close _close
#define write _write
#define lseek _lseek
#define unlink _unlink
#endif /* _MSC_VER */

#define SAFE_CONVERT_LENGTH(len) (6 * (len) + 1)

#define DEFAULT_SOUNDFONT_ORDER 0

#ifdef __MACOS__
#include "mac_com.h"
#endif

#if !defined(HAVE_POPEN)
#undef DECOMPRESSOR_LIST
#undef PATCH_CONVERTERS
#endif

#include "support.h"

#ifdef VMS
#include <errno.h>
#if __DECC_VER < 50200000
extern int sys_nerr = 0;  
extern char *sys_errlist[]; 
#else
#include <perror.h>
#endif
#define rindex(s,c) strrchr(s,c)
#define SUPPORT_SOUNDSPEC
#endif

#endif /* ___TIMIDITY_H_ */
