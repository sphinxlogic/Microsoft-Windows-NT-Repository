/*
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */
#include "video.h"
#include <sys/types.h>
#include <signal.h>
#ifndef MIPS
#include <netinet/in.h>
#else
#include <bsd/netinet/in.h>
#endif

#include "util.h"

/* Define buffer length. */

#define BUF_LENGTH 80000

/* Function return type declarations */
void usage();

/* External declaration of main decoding call. */

extern VidStream *mpegVidRsrc();
extern VidStream *NewVidStream();

/* Declaration of global variable to hold dither info. */

int ditherType;

/* Global file pointer to incoming data. */
FILE *input;

/* End of File flag. */
static int EOF_flag = 0;

/* Loop flag. */
int loopFlag = 0;

/* Shared memory flag. */
int shmemFlag = 0;

/* Setjmp/Longjmp env. */
jmp_buf env;


/*
 *--------------------------------------------------------------
 *
 * get_more_data --
 *
 *	Called by correct_underflow in bit parsing utilities to
 *      read in more data.
 *
 * Results:
 *	Input buffer updated, buffer length updated.
 *      Returns 1 if data read, 0 if EOF, -1 if error.
 *
 * Side effects:
 *      None.
 *
 *--------------------------------------------------------------
 */

int 
get_more_data(buf_start, max_length, length_ptr, buf_ptr)
     unsigned int *buf_start;
     int max_length;
     int *length_ptr;
     unsigned int **buf_ptr;
{
  
  int length, num_read, i, request;
  unsigned char *buffer, *mark;
  unsigned int *lmark;

  if (EOF_flag) return 0;

  length = *length_ptr;
  buffer = (unsigned char *) *buf_ptr;

  if (length > 0) {
    memcpy((unsigned char *) buf_start, buffer, (length*4));
    mark = ((unsigned char *) (buf_start + length));
  }
  else {
    mark = (unsigned char *) buf_start;
    length = 0;
  }

  request = (max_length-length)*4;
  
  num_read = fread( mark, 1, request, input);

  if (num_read < 0) {
    return -1;
  }
  else if (num_read == 0) {
    *buf_ptr = buf_start;
    
    /* Make 32 bits after end equal to 0 and 32
       bits after that equal to seq end code
       in order to prevent messy data from infinite
       recursion.
    */

    *(buf_start + length) = 0x0;
    *(buf_start + length+1) = SEQ_END_CODE;

    EOF_flag = 1;
    return 0;
  }

  lmark = (unsigned int *) mark;

  num_read = num_read/4;

  for (i=0; i<num_read; i++) {
    *lmark = htonl(*lmark);
    lmark++;
  }

  *buf_ptr = buf_start;
  *length_ptr = length + num_read;
 
  return 1;
}

/*
 *--------------------------------------------------------------
 *
 * int_handler --
 *
 *	Handles Cntl-C interupts..
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
void
int_handler()
{
  fprintf(stderr, "Interrupted!\n");
  if (curVidStream != NULL)
    DestroyVidStream(curVidStream);
  exit(1);
}


/*
 *--------------------------------------------------------------
 *
 * main --
 *
 *	Parses command line, starts decoding and displaying.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void
main(argc, argv)
     int argc;
     char **argv;
{

  static VidStream *theStream;
  int mark;
  int i;

  mark = 1;
  argc--;

  input = stdin;
  ditherType = ORDERED_DITHER;

#ifdef SH_MEM
  shmemFlag = 1;
#endif

  while (argc) {
    if (strcmp(argv[mark], "-nop") == 0) {
      TogglePFlag();
      argc--; mark++;
    } else if (strcmp(argv[mark], "-nob") == 0) {
      ToggleBFlag();
      argc--; mark++;
    } else if (strcmp(argv[mark], "-dither") == 0) {
      argc--; mark++;
      if (argc < 1) {
	perror("Must specify dither option after -dither flag");
	usage(argv[0]);
      }
      if (strcmp(argv[mark], "hybrid") == 0) {
	argc--; mark++;
	ditherType = HYBRID_DITHER;
      } else if (strcmp(argv[mark], "hybrid2") == 0) {
	argc--; mark++;
	ditherType = HYBRID2_DITHER;
      } else if (strcmp(argv[mark], "fs4") == 0) {
	argc--; mark++;
	ditherType = FS4_DITHER;
      } else if (strcmp(argv[mark], "fs2") == 0) {
	argc--; mark++;
	ditherType = FS2_DITHER;
      } else if (strcmp(argv[mark], "fs2fast") == 0) {
	argc--; mark++;
	ditherType = FS2FAST_DITHER;
      } else if (strcmp(argv[mark], "hybrid2") == 0) {
	argc--; mark++;
	ditherType = HYBRID2_DITHER;
      } else if (strcmp(argv[mark], "2x2") == 0) {
	argc--; mark++;
	ditherType = Twox2_DITHER;
      } else if (strcmp(argv[mark], "gray") == 0) {
	argc--; mark++;
	ditherType = GRAY_DITHER;
      } else if (strcmp(argv[mark], "color") == 0) {
	argc--; mark++;
	ditherType = FULL_COLOR_DITHER;
      } else if (strcmp(argv[mark], "none") == 0) {
	argc--; mark++;
	ditherType = NO_DITHER;
      } else if (strcmp(argv[mark], "ordered") == 0) {
	argc--; mark++;
	ditherType = ORDERED_DITHER;
      } else if (strcmp(argv[mark], "mono") == 0) {
	argc--; mark++;
	ditherType = MONO_DITHER;
      } else if (strcmp(argv[mark], "threshold") == 0) {
	argc--; mark++;
	ditherType = MONO_THRESHOLD;
      } else {
	perror("Illegal dither option.");
	usage(argv[0]);
      }
    } 
    else if (strcmp(argv[mark], "-eachstat") == 0) {
      argc--; mark++;
#ifdef ANALYSIS
      showEachFlag = 1;
#else
      fprintf(stderr, "To use -eachstat, recompile with -DANALYSIS in CFLAGS\n");
      exit(1);
#endif
    }
    else if (strcmp(argv[mark], "-shmem_off") == 0) {
      argc--; mark++;
      shmemFlag = 0;
    }
    else if (strcmp(argv[mark], "-loop") == 0) {
      argc--; mark++;
      loopFlag = 1;
    }
    else if (argv[mark][0] == '-') {
      fprintf(stderr, "Un-recognized flag %s\n",argv[mark]);
      usage(argv[0]);
    }
    else {
      input = fopen(argv[mark], "r");
      if (input == NULL) {
	fprintf(stderr, "Could not open file %s\n", argv[mark]);
	usage(argv[0]);
      }
      argc--; mark++;
    }
  }

  signal(SIGINT, int_handler);

  init_tables();
  
  switch (ditherType) {
    
  case HYBRID_DITHER:
    
    InitColor();
    InitHybridDither();
    InitDisplay();
    break;
    
    case HYBRID2_DITHER:
    InitColor();
    InitHybridErrorDither();
    InitDisplay();
    break;
    
  case FS4_DITHER:
    InitColor();
    InitFS4Dither();
      InitDisplay();
    break;
    
  case FS2_DITHER:
    InitColor();
    InitFS2Dither();
    InitDisplay();
    break;
    
  case FS2FAST_DITHER:
    InitColor();
    InitFS2FastDither();
    InitDisplay();
    break;
    
  case Twox2_DITHER:
    InitColor();
    Init2x2Dither();
    InitDisplay();
    PostInit2x2Dither();
    break;

  case GRAY_DITHER:
    InitGrayDisplay();
    break;

  case FULL_COLOR_DITHER:
    InitColorDither();
    InitColorDisplay();
    break;

  case NO_DITHER:
    shmemFlag = 0;
    break;

  case ORDERED_DITHER:
    InitColor();
    InitOrderedDither();
    InitDisplay();
    break;

  case MONO_DITHER:
  case MONO_THRESHOLD:
    InitMonoDisplay();
    break;
  }

#ifdef SH_MEM
    if (shmemFlag && (display != NULL)) {
      if (!XShmQueryExtension(display)) {
	shmemFlag = 0;
	fprintf(stderr, "Shared memory not supported\n");
	fprintf(stderr, "Reverting to normal Xlib.\n");
      }
    }
#endif

  if (setjmp(env) != 0) {

    DestroyVidStream(theStream);

    rewind(input);

    EOF_flag = 0;
    curBits = 0;
    bitOffset = 0;
    bufLength = 0;
    bitBuffer = NULL;
    totNumFrames = 0;
#ifdef ANALYSIS 
    init_stats();
#endif

  }

  theStream = NewVidStream(BUF_LENGTH);


  mpegVidRsrc(0, theStream);

  if (ditherType == Twox2_DITHER) i = 2;
  else i = 1;  

  ResizeDisplay(curVidStream->h_size*i, curVidStream->v_size*i);

  realTimeStart = ReadSysClock();

  while (1) mpegVidRsrc(0, theStream);
}
 

/*
 *--------------------------------------------------------------
 *
 * usage --
 *
 *	Print mpeg_play usage
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	exits with a return value -1
 *
 *--------------------------------------------------------------
 */

void
usage(s)
char *s;	/* program name */
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "mpeg_play\n");
    fprintf(stderr, "          [-nob]\n");
    fprintf(stderr, "          [-nop]\n");
    fprintf(stderr, "          [-dither {ordered|fs4|fs2|fs2fast|hybrid|hybrid2|2x2|gray|color|none|mono|threshold}]\n");
    fprintf(stderr, "          [-loop]\n");
    fprintf(stderr, "          [-eachstat]\n");
    fprintf(stderr, "          file_name\n");
    exit (-1);
}



/*
 *--------------------------------------------------------------
 *
 * DoDitherImage --
 *
 *	Called when image needs to be dithered. Selects correct
 *      dither routine based on info in ditherType.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */

void
DoDitherImage(l, Cr, Cb, disp, h, w) 
unsigned char *l, *Cr, *Cb, *disp;
int h,w;
{

  switch(ditherType) {
  case HYBRID_DITHER:
    HybridDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case HYBRID2_DITHER:
    HybridErrorDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FS2FAST_DITHER:
    FS2FastDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FS2_DITHER:
    FS2DitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FS4_DITHER:
    FS4DitherImage(l, Cr, Cb, disp, h, w);
    break;

  case Twox2_DITHER:
    Twox2DitherImage(l, Cr, Cb, disp, h, w);
    break;

  case FULL_COLOR_DITHER:
    ColorDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case GRAY_DITHER:
    GrayDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case NO_DITHER:
    break;

  case ORDERED_DITHER:
    OrderedDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case MONO_DITHER:
    MonoDitherImage(l, Cr, Cb, disp, h, w);
    break;

  case MONO_THRESHOLD:
    MonoThresholdImage(l, Cr, Cb, disp, h, w);
    break;
  }
}




