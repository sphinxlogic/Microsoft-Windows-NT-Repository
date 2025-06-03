/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%           CCCC   OOO   M   M  PPPP   RRRR   EEEEE   SSSSS  SSSSS            %
%          C      O   O  MM MM  P   P  R   R  E       SS     SS               %
%          C      O   O  M M M  PPPP   RRRR   EEE      SSS    SSS             %
%          C      O   O  M   M  P      R R    E          SS     SS            %
%           CCCC   OOO   M   M  P      R  R   EEEEE   SSSSS  SSSSS            %
%                                                                             %
%                                                                             %
%                      Image Compression Coders                               %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
%                                                                             %
%                                                                             %
%  Copyright 1992 E. I. du Pont de Nemours & Company                          %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. du Pont de Nemours    %
%  & Company not be used in advertising or publicity pertaining to            %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. du Pont de Nemours & Company makes no representations   %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. du Pont de Nemours & Company disclaims all warranties with regard    %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. du Pont de Nemours & Company be       %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortious action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Functions HuffmanEncodeImage is based on the CCITT T.4 format
%  specifications.
%
%  Functions LZWEncodeImage and LZWDecodeImage is based on the GIF image
%  format specifications.
%
%  Functions QEncodeImage and QDecodeImage use a simple predictive method.
%  The predictor combines three neighboring samples (A, B, and C) to form a
%  prediction of the sample X:
%
%    C B
%    A X
%
%  The prediction formula is A + B - C.  The prediction is subtracted from
%  from the actual sample X and the difference is encoded by an arithmetic
%  entropy coding method.
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"
/*
  Define declarations.
*/
#define LowerBound  0
#define MaxContextStates  121
#define MinimumIntervalD  (unsigned short) 0xf000  /* ~-0.75 */
#define MinimumIntervalE  (unsigned short) 0x1000  /* ~0.75 */
#define No  0
#define UpperBound  2
#define Yes  1
/*
  State classification.
*/
#define ZeroState  0
#define SmallPostitiveState  1
#define SmallNegativeState  2
#define LargePostitiveState  3
#define LargeNegativeState  4
/*
  Typedef declarations.
*/
typedef struct _ScanlinePacket
{
  unsigned char
    pixel;

  int
    state;
} ScanlinePacket;
/*
  Initialized declarations.
*/
static int decrement_less_probable[]=
{
  0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 3, 2, 3, 2
};

static int increment_more_probable[]=
{
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

static int more_probable_exchange[]=
{
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int statistics[][5]=
{
   0,  4,  8, 12, 16,
  20, 24, 28, 32, 36,
  40, 44, 48, 52, 56,
  60, 64, 68, 72, 76,
  80, 84, 88, 92, 96,
};

static unsigned short probability[]=
{
  0x0ac1, 0x0a81, 0x0a01, 0x0901, 0x0701, 0x0681,
  0x0601, 0x0501, 0x0481, 0x0441, 0x0381, 0x0301,
  0x02c1, 0x0281, 0x0241, 0x0181, 0x0121, 0x00e1,
  0x00a1, 0x0071, 0x0059, 0x0053, 0x0027, 0x0017,
  0x0013, 0x000b, 0x0007, 0x0005, 0x0003, 0x0001
};
/*
  Declarations and initializations for predictive arithimetic coder.
*/
static int
  code,
  less_probable[MaxContextStates],
  more_probable[MaxContextStates],
  probability_estimate[MaxContextStates];

static unsigned char
  *q;

static unsigned short
  interval;

/*
  External declarations.
*/
extern char
  *client_name;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e c o d e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Decode uncompresses a string.
%
%  The format of the Decode routine is:
%
%      Decode(state,decision)
%
%  A description of each parameter follows:
%
%    o state:  An integer value representing the current state.
%
%    o decision:  A pointer to an integer.  The output of the binary
%      decision (Yes/No) is returned in this value.
%
%
*/
static void Decode(state,decision)
register int
  state,
  *decision;
{
  interval+=probability[probability_estimate[state]];
  if (((code >> 16) & 0xffff) < ((int) interval))
    {
      code-=(interval << 16);
      interval=(-probability[probability_estimate[state]]);
      *decision=less_probable[state];
    }
  else
    {
      *decision=more_probable[state];
      if (interval <= MinimumIntervalD)
        return;
    }
  do
  {
    if ((code & 0xff) == 0)
      {
        code&=0xffff0000;
        if ((*q++) == 0xff)
          code+=((int) (*q) << 9)+0x02;
        else
          code+=((int) (*q) << 8)+0x01;
      }
    interval<<=1;
    code<<=1;
  } while (interval > MinimumIntervalD);
  /*
    Update probability estimates.
  */
  if (*decision == more_probable[state])
    probability_estimate[state]+=
      increment_more_probable[probability_estimate[state]];
  else
    probability_estimate[state]-=
      decrement_less_probable[probability_estimate[state]];
  if (more_probable_exchange[probability_estimate[state]] != 0)
    {
      /*
        Exchange sense of most probable and least probable.
      */
      less_probable[state]=more_probable[state];
      more_probable[state]=1-more_probable[state];
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E n c o d e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Encode generate compressed data string by encoding yes-no decision
%  given state s.
%
%  The format of the Encode routine is:
%
%      Encode(state,decision)
%
%  A description of each parameter follows:
%
%    o state:  An integer value representing the current state.
%
%    o decision:  An integer value representing a binary decision.
%
%
*/
static void Encode(state,decision)
register int
  state,
  decision;
{
  /*
    Test on "most-probable-symbol" for state s(more_probable[state])
  */
  interval-=probability[probability_estimate[state]];
  if (more_probable[state] != decision)
    {
      code-=interval;
      interval=probability[probability_estimate[state]];
    }
  else
    if (interval >= MinimumIntervalE)
      return;
  /*
    Encoder renormalization.
  */
  do
  {
    interval<<=1;
    if (code >= 0)
      code<<=1;
    else
      {
        /*
          Shift unsigned char of data from Code register to compressed string.
        */
        code<<=1;
        if (code > 0)
          {
            /*
              Add eight bits from Code register to compressed data string.
            */
            (*q++)--;
            *q=(unsigned char) (code >> 16);
            code&=0x0000ffff;
            code|=0x01800000;
          }
        else
          {
            code&=0x01ffffff;
            if ((int) interval > code)
              {
                /*
                  Add eight bits from Code register to compressed data string.
                */
                (*q++)--;
                *q=0xff;
                code|=0x01810000;
              }
            else
              if ((*q++) == 0xff)
                {
                  /*
                    Add seven bits from Code register plus one stuffed bit to
                    compressed data string.
                  */
                  *q=(unsigned char) (code >> 17);
                  code&=0x0001ffff;
                  code|=0x03000000;
                }
              else
                {
                  /*
                    Add eight bits from Code register to compressed data string.
                  */
                  *q=(unsigned char) (code >> 16);
                  code&=0x0000ffff;
                  code|=0x01800000;
                }
          }
      }
  } while (interval < MinimumIntervalE);
  /*
    Update probability estimates
  */
  if (decision == more_probable[state])
    probability_estimate[state]+=
      increment_more_probable[probability_estimate[state]];
  else
    probability_estimate[state]-=
      decrement_less_probable[probability_estimate[state]];
  if (more_probable_exchange[probability_estimate[state]] != 0)
    more_probable[state]=1-more_probable[state];
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l u s h                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Flush flushes the final bits of data from the Code register to the
%  compressed data string.
%
%  The format of the Flush routine is:
%
%      Flush()
%
%
*/
static void Flush()
{
  register int
    extra_bits;

  code-=interval;
  extra_bits=24;
  extra_bits--;
  while (code >= 0)
  {
    code<<=1;
    extra_bits--;
  }
  code<<=1;
  if (code > 0)
    (*q)--;
  /*
    Add the final compressed data unsigned chars to the compressed data string.
  */
  do
  {
    if ((*q++) == 0xff)
      {
        /*
          Add seven bits of data plus one stuffed bit to the compressed data
          string during final Flush of Code register.
        */
        *q=(unsigned char) (code >> 17);
        code&=0x0001ffff;
        code<<=7;
        extra_bits-=7;
      }
    else
      {
        /*
           Add eight bits of data to the compressed data string during final
           flush of Code register.
        */
        *q=(unsigned char) (code >> 16);
        code&=0x0000ffff;
        code<<=8;
        extra_bits-=8;
      }
  } while (extra_bits > 0);
  q++;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   H u f f m a n E n c o d e I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function HuffmanEncodeImage compresses an image via Huffman-coding.
%
%  The format of the HuffmanEncodeImage routine is:
%
%      status=HuffmanEncodeImage(image)
%
%  A description of each parameter follows:
%
%    o status:  Function HuffmanEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int HuffmanEncodeImage(image)
Image
  *image;
{
#define HuffmanOutputCode(te)  \
{  \
  mask=1 << (entry->length-1);  \
  while (mask)   \
  {  \
    if (entry->code & mask)  \
      OutputBit(1)  \
    else  \
      OutputBit(0);  \
    mask>>=1;  \
  }  \
}

#define OutputBit(count)  \
{  \
  if(count > 0)  \
    byte=byte | bit;  \
  bit>>=1;  \
  if ((bit & 0xff) == 0)   \
    {  \
      (void) fwrite((char *) &byte,1,1,image->file);  \
      byte=0;  \
      bit=0x80;  \
    }  \
}

  typedef struct HuffmanTable
  {
    int
      id,
      code,
      length,
      count;
  } HuffmanTable;

  static HuffmanTable MBTable[]=
  {
    { 26, 0x0f, 10, 64 }, { 26, 0xc8, 12, 128 }, { 26, 0xc9, 12, 192 },
    { 26, 0x5b, 12, 256 }, { 26, 0x33, 12, 320 }, { 26, 0x34, 12, 384 },
    { 26, 0x35, 12, 448 }, { 26, 0x6c, 13, 512 }, { 26, 0x6d, 13, 576 },
    { 26, 0x4a, 13, 640 }, { 26, 0x4b, 13, 704 }, { 26, 0x4c, 13, 768 },
    { 26, 0x4d, 13, 832 }, { 26, 0x72, 13, 896 }, { 26, 0x73, 13, 960 },
    { 26, 0x74, 13, 1024 }, { 26, 0x75, 13, 1088 }, { 26, 0x76, 13, 1152 },
    { 26, 0x77, 13, 1216 }, { 26, 0x52, 13, 1280 }, { 26, 0x53, 13, 1344 },
    { 26, 0x54, 13, 1408 }, { 26, 0x55, 13, 1472 }, { 26, 0x5a, 13, 1536 },
    { 26, 0x5b, 13, 1600 }, { 26, 0x64, 13, 1664 }, { 26, 0x65, 13, 1728 },
  };

  static HuffmanTable MWTable[]=
  {
    { 24, 0x1b, 5, 64 }, { 24, 0x12, 5, 128 }, { 24, 0x17, 6, 192 },
    { 24, 0x37, 7, 256 }, { 24, 0x36, 8, 320 }, { 24, 0x37, 8, 384 },
    { 24, 0x64, 8, 448 }, { 24, 0x65, 8, 512 }, { 24, 0x68, 8, 576 },
    { 24, 0x67, 8, 640 }, { 24, 0xcc, 9, 704 }, { 24, 0xcd, 9, 768 },
    { 24, 0xd2, 9, 832 }, { 24, 0xd3, 9, 896 }, { 24, 0xd4, 9, 960 },
    { 24, 0xd5, 9, 1024 }, { 24, 0xd6, 9, 1088 }, { 24, 0xd7, 9, 1152 },
    { 24, 0xd8, 9, 1216 }, { 24, 0xd9, 9, 1280 }, { 24, 0xda, 9, 1344 },
    { 24, 0xdb, 9, 1408 }, { 24, 0x98, 9, 1472 }, { 24, 0x99, 9, 1536 },
    { 24, 0x9a, 9, 1600 }, { 24, 0x18, 6, 1664 }, { 24, 0x9b, 9, 1728 },
  };

  static HuffmanTable TBTable[]=
  {
    { 25, 0x37, 10, 0 }, { 25, 0x02, 3, 1 }, { 25, 0x03, 2, 2 },
    { 25, 0x02, 2, 3 }, { 25, 0x03, 3, 4 }, { 25, 0x03, 4, 5 },
    { 25, 0x02, 4, 6 }, { 25, 0x03, 5, 7 }, { 25, 0x05, 6, 8 },
    { 25, 0x04, 6, 9 }, { 25, 0x04, 7, 10 }, { 25, 0x05, 7, 11 },
    { 25, 0x07, 7, 12 }, { 25, 0x04, 8, 13 }, { 25, 0x07, 8, 14 },
    { 25, 0x18, 9, 15 }, { 25, 0x17, 10, 16 }, { 25, 0x18, 10, 17 },
    { 25, 0x8, 10, 18 }, { 25, 0x67, 11, 19 }, { 25, 0x68, 11, 20 },
    { 25, 0x6c, 11, 21 }, { 25, 0x37, 11, 22 }, { 25, 0x28, 11, 23 },
    { 25, 0x17, 11, 24 }, { 25, 0x18, 11, 25 }, { 25, 0xca, 12, 26 },
    { 25, 0xcb, 12, 27 }, { 25, 0xcc, 12, 28 }, { 25, 0xcd, 12, 29 },
    { 25, 0x68, 12, 30 }, { 25, 0x69, 12, 31 }, { 25, 0x6a, 12, 32 },
    { 25, 0x6b, 12, 33 }, { 25, 0xd2, 12, 34 }, { 25, 0xd3, 12, 35 },
    { 25, 0xd4, 12, 36 }, { 25, 0xd5, 12, 37 }, { 25, 0xd6, 12, 38 },
    { 25, 0xd7, 12, 39 }, { 25, 0x6c, 12, 40 }, { 25, 0x6d, 12, 41 },
    { 25, 0xda, 12, 42 }, { 25, 0xdb, 12, 43 }, { 25, 0x54, 12, 44 },
    { 25, 0x55, 12, 45 }, { 25, 0x56, 12, 46 }, { 25, 0x57, 12, 47 },
    { 25, 0x64, 12, 48 }, { 25, 0x65, 12, 49 }, { 25, 0x52, 12, 50 },
    { 25, 0x53, 12, 51 }, { 25, 0x24, 12, 52 }, { 25, 0x37, 12, 53 },
    { 25, 0x38, 12, 54 }, { 25, 0x27, 12, 55 }, { 25, 0x28, 12, 56 },
    { 25, 0x58, 12, 57 }, { 25, 0x59, 12, 58 }, { 25, 0x2b, 12, 59 },
    { 25, 0x2c, 12, 60 }, { 25, 0x5a, 12, 61 }, { 25, 0x66, 12, 62 },
    { 25, 0x67, 12, 63 },
  };

  static HuffmanTable TWTable[]=
  {
    { 23, 0x35, 8, 0 }, { 23, 0x07, 6, 1 }, { 23, 0x07, 4, 2 },
    { 23, 0x08, 4, 3 }, { 23, 0x0b, 4, 4 }, { 23, 0x0c, 4, 5 },
    { 23, 0x0e, 4, 6 }, { 23, 0x0f, 4, 7 }, { 23, 0x13, 5, 8 },
    { 23, 0x14, 5, 9 }, { 23, 0x07, 5, 10 }, { 23, 0x08, 5, 11 },
    { 23, 0x08, 6, 12 }, { 23, 0x03, 6, 13 }, { 23, 0x34, 6, 14 },
    { 23, 0x35, 6, 15 }, { 23, 0x2a, 6, 16 }, { 23, 0x2b, 6, 17 },
    { 23, 0x27, 7, 18 }, { 23, 0x0c, 7, 19 }, { 23, 0x08, 7, 20 },
    { 23, 0x17, 7, 21 }, { 23, 0x03, 7, 22 }, { 23, 0x04, 7, 23 },
    { 23, 0x28, 7, 24 }, { 23, 0x2b, 7, 25 }, { 23, 0x13, 7, 26 },
    { 23, 0x24, 7, 27 }, { 23, 0x18, 7, 28 }, { 23, 0x02, 8, 29 },
    { 23, 0x03, 8, 30 }, { 23, 0x1a, 8, 31 }, { 23, 0x1b, 8, 32 },
    { 23, 0x12, 8, 33 }, { 23, 0x13, 8, 34 }, { 23, 0x14, 8, 35 },
    { 23, 0x15, 8, 36 }, { 23, 0x16, 8, 37 }, { 23, 0x17, 8, 38 },
    { 23, 0x28, 8, 39 }, { 23, 0x29, 8, 40 }, { 23, 0x2a, 8, 41 },
    { 23, 0x2b, 8, 42 }, { 23, 0x2c, 8, 43 }, { 23, 0x2d, 8, 44 },
    { 23, 0x04, 8, 45 }, { 23, 0x05, 8, 46 }, { 23, 0x0a, 8, 47 },
    { 23, 0x0b, 8, 48 }, { 23, 0x52, 8, 49 }, { 23, 0x53, 8, 50 },
    { 23, 0x54, 8, 51 }, { 23, 0x55, 8, 52 }, { 23, 0x24, 8, 53 },
    { 23, 0x25, 8, 54 }, { 23, 0x58, 8, 55 }, { 23, 0x59, 8, 56 },
    { 23, 0x5a, 8, 57 }, { 23, 0x5b, 8, 58 }, { 23, 0x4a, 8, 59 },
    { 23, 0x4b, 8, 60 }, { 23, 0x32, 8, 61 }, { 23, 0x33, 8, 62 },
    { 23, 0x34, 8, 63 },
  };

  HuffmanTable*
    entry;

  int
    c,
    k,
    n,
    x;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register unsigned short
    polarity;

  unsigned int
    mask;

  unsigned char
    bit,
    byte,
    *scanline;

  /*
    Allocate scanline buffer.
  */
  scanline=(unsigned char *)
    malloc(Max(image->columns,1728)*sizeof(unsigned char));
  if (scanline == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  /*
    Compress MIFF to 1D Huffman encoded pixels.
  */
  q=scanline;
  polarity=(Intensity(image->colormap[0]) >
    Intensity(image->colormap[1]) ? 0 : 1);
  for (i=0; i < Max(image->columns,1728); i++)
    *q++=polarity;
  byte=0;
  bit=0x80;
  p=image->pixels;
  q=scanline;
  x=0;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      if (p->index == polarity)
        *q++=(unsigned char) polarity;
       else
        *q++=(unsigned char) !polarity;
      x++;
      if (x == image->columns)
        {
          /*
            Huffman encode scanline.
          */
          q=scanline;
          n=Max(image->columns,1728);
          while (n > 0)
          {
            /*
              Find white run.
            */
            c=0;
            while((*q == polarity) && (n > 0))
            {
              q++;
              c++;
              n--;
            }
            /*
              Output white run.
            */
            if (c >= 64)
              {
                entry=MWTable+((c/64)-1);
                c-=entry->count;
                HuffmanOutputCode(entry);
              }
            entry=TWTable+c;
            HuffmanOutputCode(entry);
            c=0;
            if (n == 0)
              break;
            /*
              Find black run.
            */
            while ((*q != polarity) && (n > 0))
            {
              q++;
              c++;
              n--;
            }
            /*
              Output black run.
            */
            if (c >= 64)
              {
                entry=MBTable+((c/64)-1);
                c-=entry->count;
                HuffmanOutputCode(entry);
              }
            entry=TBTable+c;
            HuffmanOutputCode(entry);
            if (n == 0)
              break;
          }
          /*
            End of line.
          */
          for (k=0; k < 11; k++)
            OutputBit(0);
          OutputBit(1);
          x=0;
          q=scanline;
        }
    }
    p++;
  }
  /*
    End of page.
  */
  for (i=0; i < 6; i++)
  {
    /*
      End of line.
    */
    for (k=0; k < 11; k++)
      OutputBit(0);
    OutputBit(1);
  }
  /*
    Flush bits.
  */
  if (bit != 0x80)
    (void) fwrite((char *) &byte,1,1,image->file);
  (void) free((char *) scanline);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L Z W D e c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function LZWDecodeImage uncompresses an image via LZW-coding.
%
%  The format of the LZWDecodeImage routine is:
%
%      status=LZWDecodeImage(image)
%
%  A description of each parameter follows:
%
%    o status:  Function LZWDecodeImage returns True if all the pixels are
%      uncompressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int LZWDecodeImage(image)
Image
  *image;
{
#define MaxStackSize  4096
#define NullCode  (-1)

  int
    available,
    clear,
    code_mask,
    code_size,
    end_of_information,
    in_code,
    old_code;

  register int
    bits,
    code,
    count,
    i;

  register RunlengthPacket
    *p;

  register unsigned char
    *c;

  register unsigned int
    datum;

  short int
    *prefix;

  unsigned char
    data_size,
    first,
    *packet,
    *pixel_stack,
    *suffix,
    *top_stack;

  unsigned short
    index;

  /*
    Allocate decoder tables.
  */
  packet=(unsigned char *) malloc(256*sizeof(unsigned char));
  prefix=(short int *) malloc(MaxStackSize*sizeof(short int));
  suffix=(unsigned char *) malloc(MaxStackSize*sizeof(unsigned char));
  pixel_stack=(unsigned char *) malloc(MaxStackSize*sizeof(unsigned char));
  if ((packet == (unsigned char *) NULL) ||
      (prefix == (short int *) NULL) ||
      (suffix == (unsigned char *) NULL) ||
      (pixel_stack == (unsigned char *) NULL))
    return(False);
  /*
    Initialize LZW data stream decoder.
  */
  (void) ReadData((char *) &data_size,1,1,image->file);
  clear=1 << data_size;
  end_of_information=clear+1;
  available=clear+2;
  old_code=NullCode;
  code_size=data_size+1;
  code_mask=(1 << code_size)-1;
  for (code=0; code < clear; code++)
  {
    prefix[code]=0;
    suffix[code]=code;
  }
  /*
    Decode LZW pixel stream.
  */
  datum=0;
  bits=0;
  count=0;
  top_stack=pixel_stack;
  p=image->pixels;
  for (i=0; i < image->packets; )
  {
    if (top_stack == pixel_stack)
      {
        if (bits < code_size)
          {
            /*
              Load bytes until there is enough bits for a code.
            */
            if (count == 0)
              {
                /*
                  Read a new data block.
                */
                count=ReadDataBlock((char *) packet,image->file);
                if (count <= 0)
                  break;
                c=packet;
              }
            datum+=(*c) << bits;
            bits+=8;
            c++;
            count--;
            continue;
          }
        /*
          Get the next code.
        */
        code=datum & code_mask;
        datum>>=code_size;
        bits-=code_size;
        /*
          Interpret the code
        */
        if ((code > available) || (code == end_of_information))
          break;
        if (code == clear)
          {
            /*
              Reset decoder.
            */
            code_size=data_size+1;
            code_mask=(1 << code_size)-1;
            available=clear+2;
            old_code=NullCode;
            continue;
          }
        if (old_code == NullCode)
          {
            *top_stack++=suffix[code];
            old_code=code;
            first=code;
            continue;
          }
        in_code=code;
        if (code == available)
          {
            *top_stack++=first;
            code=old_code;
          }
        while (code > clear)
        {
          *top_stack++=suffix[code];
          code=prefix[code];
        }
        first=suffix[code];
        /*
          Add a new string to the string table,
        */
        *top_stack++=first;
        prefix[available]=old_code;
        suffix[available]=first;
        available++;
        if (((available & code_mask) == 0) && (available < MaxStackSize))
          {
            code_size++;
            code_mask+=available;
          }
        old_code=in_code;
      }
    /*
      Pop a pixel off the pixel pixel_stack.
    */
    top_stack--;
    index=(unsigned short) *top_stack;
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p->index=index;
    p->length=0;
    p++;
    i++;
  }
  /*
    Initialize any remaining color packets to a known color.
  */
  for ( ; i < image->packets; i++)
  {
    p->red=image->colormap[0].red;
    p->green=image->colormap[0].green;
    p->blue=image->colormap[0].blue;
    p->index=0;
    p->length=0;
    p++;
  }
  /*
    Free decoder memory.
  */
  (void) free((char *) pixel_stack);
  (void) free((char *) suffix);
  (void) free((char *) prefix);
  (void) free((char *) packet);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L Z W E n c o d e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function LZWEncodeImage compresses an image via LZW-coding.
%
%  The format of the LZWEncodeImage routine is:
%
%      status=LZWEncodeImage(image,data_size)
%
%  A description of each parameter follows:
%
%    o status:  Function LZWEncodeImage returns True if all the pixels are
%      compressed without error, otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int LZWEncodeImage(image,data_size)
Image
  *image;

unsigned int
  data_size;
{
#define MaxCode(number_bits)  ((1 << (number_bits))-1)
#define MaxHashTable  5003
#define MaxLZWBits  12
#define MaxLZWTable  (1 << MaxLZWBits)
#define LZWOutputCode(code) \
{ \
  /*  \
    Emit a code. \
  */ \
  if (bits > 0) \
    datum|=((long int) code << bits); \
  else \
    datum=(long int) code; \
  bits+=number_bits; \
  while (bits >= 8)  \
  { \
    /*  \
      Add a character to current packet. \
    */ \
    byte_count++; \
    packet[byte_count]=(unsigned char) (datum & 0xff); \
    if (byte_count >= 255) \
      { \
        packet[0]=(unsigned char) byte_count++; \
        (void) fwrite((char *) packet,1,byte_count,image->file); \
        byte_count=0; \
      } \
    datum>>=8; \
    bits-=8; \
  } \
  if (free_code > max_code)  \
    { \
      number_bits++; \
      if (number_bits == MaxLZWBits) \
        max_code=MaxLZWTable; \
      else \
        max_code=MaxCode(number_bits); \
    } \
}

  int
    bits,
    byte_count,
    next_pixel,
    number_bits;

  long int
    datum;

  register int
    displacement,
    i,
    j,
    k;

  register RunlengthPacket
    *p;

  short int
    clear_code,
    end_of_information_code,
    free_code,
    *hash_code,
    *hash_prefix,
    index,
    max_code,
    waiting_code;

  unsigned char
    *packet,
    *hash_suffix;

  /*
    Allocate encoder tables.
  */
  packet=(unsigned char *) malloc(256*sizeof(unsigned char));
  hash_code=(short int *) malloc(MaxHashTable*sizeof(short int));
  hash_prefix=(short int *) malloc(MaxHashTable*sizeof(short int));
  hash_suffix=(unsigned char *) malloc(MaxHashTable*sizeof(unsigned char));
  if ((packet == (unsigned char *) NULL) || (hash_code == (short int *) NULL) ||
      (hash_prefix == (short int *) NULL) ||
      (hash_suffix == (unsigned char *) NULL))
    return(False);
  /*
    Initialize LZW encoder.
  */
  number_bits=data_size;
  max_code=MaxCode(number_bits);
  clear_code=((short int) 1 << (data_size-1));
  end_of_information_code=clear_code+1;
  free_code=clear_code+2;
  byte_count=0;
  datum=0;
  bits=0;
  for (i=0; i < MaxHashTable; i++)
    hash_code[i]=0;
  LZWOutputCode(clear_code);
  /*
    Encode pixels.
  */
  p=image->pixels;
  waiting_code=p->index;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      /*
        Probe hash table.
      */
      index=p->index & 0xff;
      k=(int) ((int) index << (MaxLZWBits-8))+waiting_code;
      if (k >= MaxHashTable)
        k-=MaxHashTable;
      if (hash_code[k] > 0)
        {
          if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
            {
              waiting_code=hash_code[k];
              continue;
            }
          if (k == 0)
            displacement=1;
          else
            displacement=MaxHashTable-k;
          next_pixel=False;
          for ( ; ; )
          {
            k-=displacement;
            if (k < 0)
              k+=MaxHashTable;
            if (hash_code[k] == 0)
              break;
            if ((hash_prefix[k] == waiting_code) && (hash_suffix[k] == index))
              {
                waiting_code=hash_code[k];
                next_pixel=True;
                break;
              }
          }
          if (next_pixel == True)
            continue;
        }
      LZWOutputCode(waiting_code);
      if (free_code < MaxLZWTable)
        {
          hash_code[k]=free_code++;
          hash_prefix[k]=waiting_code;
          hash_suffix[k]=index;
        }
      else
        {
          /*
            Fill the hash table with empty entries.
          */
          for (k=0; k < MaxHashTable; k++)
            hash_code[k]=0;
          /*
            Reset compressor and issue a clear code.
          */
          free_code=clear_code+2;
          LZWOutputCode(clear_code);
          number_bits=data_size;
          max_code=MaxCode(number_bits);
        }
      waiting_code=index;
    }
    p++;
  }
  /*
    Flush out the buffered code.
  */
  LZWOutputCode(waiting_code);
  LZWOutputCode(end_of_information_code);
  if (bits > 0)
    {
      /*
        Add a character to current packet.
      */
      byte_count++;
      packet[byte_count]=(unsigned char) (datum & 0xff);
      if (byte_count >= 255)
        {
          packet[0]=(unsigned char) byte_count++;
          (void) fwrite((char *) packet,1,byte_count,image->file);
          byte_count=0;
        }
    }
  /*
    Flush accumulated data.
  */
  if (byte_count > 0)
    {
      packet[0]=(unsigned char) byte_count++;
      (void) fwrite((char *) packet,1,byte_count,image->file);
      byte_count=0;
    }
  /*
    Free encoder memory.
  */
  (void) free((char *) hash_suffix);
  (void) free((char *) hash_prefix);
  (void) free((char *) hash_code);
  (void) free((char *) packet);
  if (i < image->packets)
    return(False);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q D e c o d e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function QDecodeImage uncompresses an image via Q-coding.
%
%  The format of the QDecodeImage routine is:
%
%      count=QDecodeImage(compressed_pixels,pixels,number_columns,number_rows)
%
%  A description of each parameter follows:
%
%    o count:  The QDecodeImage routine returns this integer value.  It is
%      the actual number of pixels created by the decompression process.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of compressed
%      pixel data.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data created by
%      the uncompression process.  The number of bytes in this array
%      must be at least equal to the number columns times the number of rows
%      of the source pixels.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%
*/
unsigned int QDecodeImage(compressed_pixels,pixels,number_columns,number_rows)
unsigned char
  *compressed_pixels,
  *pixels;

unsigned int
  number_columns,
  number_rows;
{
  int
    decision,
    i,
    prediction,
    row;

  register int
    column,
    magnitude,
    sign,
    state,
    value;

  register ScanlinePacket
    *cs,
    *ls;

  register unsigned char
    *p;

  ScanlinePacket
    *scanline;

  for (i=0; i < MaxContextStates; i++)
  {
    probability_estimate[i]=0;
    more_probable[i]=0;
    less_probable[i]=1;
  }
  /*
    Allocate scanline for row values and states
  */
  scanline=(ScanlinePacket *)
    malloc((2*(number_columns+1)*sizeof(ScanlinePacket)));
  if (scanline == (ScanlinePacket *) NULL)
    {
      Warning("unable to compress image, unable to allocate memory",
        (char *) NULL);
      exit(1);
    }
  cs=scanline;
  for (i=0; i < 2*(number_columns+1); i++)
  {
    cs->pixel=0;
    cs->state=ZeroState;
    cs++;
  }
  interval=MinimumIntervalD;
  p=pixels;
  q=compressed_pixels+1;
  /*
    Add a new unsigned char of compressed data to the Code register.
  */
  code=(int) (*q) << 16;
  if ((*q++) == 0xff)
    code+=((int) (*q) << 9)+0x02;
  else
    code+=((*q) << 8)+0x01;
  code<<=4;
  code+=(interval << 16);
  /*
    Decode each image scanline.
  */
  for (row=0; row < number_rows; row++)
  {
    ls=scanline+(number_columns+1)*((row+0) % 2);
    cs=scanline+(number_columns+1)*((row+1) % 2);
    for (column=0; column < number_columns; column++)
    {
      prediction=(int) cs->pixel-(int) ls->pixel;
      ls++;
      prediction+=(int) ls->pixel;
      state=statistics[cs->state][ls->state];
      cs++;
      cs->state=ZeroState;
      /*
        Branch for zero code value
      */
      Decode(state,&decision);
      if (decision == No)
        value=0;
      else
        {
          /*
            Decode sign information
          */
          state++;
          Decode(state,&decision);
          if (decision == Yes)
            sign=(-1);
          else
            {
              sign=1;
              state++;
            }
          state++;
          /*
            Branch for value=+-1
          */
          Decode(state,&decision);
          if (decision == No)
            value=1;
          else
            {
              /*
                Establish magnitude of value.
              */
              magnitude=2;
              state=100;
              Decode(state,&decision);
              while (decision != No)
              {
                if (state < 107)
                  state++;
                magnitude<<=1;
                Decode(state,&decision);
              }
              /*
                Code remaining bits.
              */
              state+=7;
              value=1;
              magnitude>>=2;
              if (magnitude != 0)
                {
                  Decode(state,&decision);
                  state+=6;
                  value=(value << 1) | decision;
                  magnitude>>=1;
                  while (magnitude != 0)
                  {
                    Decode(state,&decision);
                    value=(value << 1) | decision;
                    magnitude>>=1;
                  }
                }
              value++;
            }
          if (value > LowerBound)
            if (value <= UpperBound)
              cs->state=
                (sign < ZeroState ? SmallPostitiveState : SmallNegativeState);
            else
              cs->state=
                (sign < ZeroState ? LargePostitiveState : LargeNegativeState);
          if (sign < 0)
            value=(-value);
        }
      cs->pixel=(unsigned char) (value+prediction);
      *p++=cs->pixel;
    }
  }
  (void) free((char *) scanline);
  return((unsigned int) (p-pixels));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q E n c o d e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function QEncodeImage compresses an image via q-coding.
%
%  The format of the QEncodeImage routine is:
%
%      count=QEncodeImage(pixels,compressed_pixels,number_columns,number_rows)
%
%  A description of each parameter follows:
%
%    o count:  The QEncodeImage routine returns this integer value.  It is
%      the actual number of compressed pixels created by the compression
%      process.
%
%    o pixels:  The address of a byte (8 bits) array of pixel data.
%
%    o compressed_pixels:  The address of a byte (8 bits) array of pixel data
%      created by the compression process.  The number of bytes in this array
%      must be at least equal to the number columns times the number of rows
%      of the source pixels to allow for the possibility that no compression
%      is possible.  The actual number of bytes used is reflected by the
%      count parameter.
%
%    o number_columns:  An integer value that is the number of columns or
%      width in pixels of your source image.
%
%    o number_rows:  An integer value that is the number of rows or
%      heigth in pixels of your source image.
%
%
%
*/
unsigned int QEncodeImage(pixels,compressed_pixels,number_columns,number_rows)
unsigned char
  *pixels,
  *compressed_pixels;

unsigned int
  number_columns,
  number_rows;
{
  int
    i,
    prediction,
    row;

  register int
    column,
    magnitude,
    sign,
    state,
    value;

  register ScanlinePacket
    *cs,
    *ls;

  register unsigned char
    *p;

  ScanlinePacket
    *scanline;

  void
    Flush();

  for (i=0; i < MaxContextStates; i++)
  {
    probability_estimate[i]=0;
    more_probable[i]=0;
  }
  /*
    Allocate scanline for row values and states.
  */
  scanline=(ScanlinePacket *)
    malloc((2*(number_columns+1)*sizeof(ScanlinePacket)));
  if (scanline == (ScanlinePacket *) NULL)
    {
      Warning("unable to compress image, unable to allocate memory",
        (char *) NULL);
      exit(1);
    }
  cs=scanline;
  for (i=0; i < 2*(number_columns+1); i++)
  {
    cs->pixel=0;
    cs->state=ZeroState;
    cs++;
  }
  interval=MinimumIntervalE;
  p=pixels;
  q=compressed_pixels;
  (*q)++;
  code=0x00180000;
  /*
    Encode each scanline.
  */
  for (row=0; row < number_rows; row++)
  {
    ls=scanline+(number_columns+1)*((row+0) % 2);
    cs=scanline+(number_columns+1)*((row+1) % 2);
    for (column=0; column < number_columns; column++)
    {
      prediction=(int) cs->pixel-(int) ls->pixel;
      ls++;
      prediction+=(int) ls->pixel;
      state=statistics[cs->state][ls->state];
      cs++;
      cs->pixel=(*p++);
      cs->state=ZeroState;
      value=(int) cs->pixel-prediction;
      Encode(state,(value == 0 ? No : Yes));
      if (value != 0)
        {
          /*
            Code sign information
          */
          state++;
          sign=(value < 0 ? -1 : 1);
          Encode(state,(sign >= 0 ? No : Yes));
          if (sign < 0)
            value=(-value);
          else
            state++;
          state++;
          value--;
          /*
            Branch for code=+-1
          */
          Encode(state,(value == 0 ? No : Yes));
          if (value != 0)
            {
              /*
                Establish magnitude of value.
              */
              state=100;
              magnitude=2;
              while (value >= magnitude)
              {
                Encode(state,Yes);
                if (state < 107)
                  state++;
                magnitude<<=1;
              }
              Encode(state,No);
              /*
                Code remaining bits
              */
              state+=7;
              magnitude>>=2;
              if (magnitude != 0)
                {
                  Encode(state,((magnitude & value) == 0 ? No : Yes));
                  state+=6;
                  magnitude>>=1;
                  while (magnitude != 0)
                  {
                    Encode(state,((magnitude & value) == 0 ? No : Yes));
                    magnitude>>=1;
                  }
                }
            }
          if (value >= LowerBound)
            if (value < UpperBound)
              cs->state=
                (sign < ZeroState ? SmallPostitiveState : SmallNegativeState);
            else
              cs->state=
                (sign < ZeroState ? LargePostitiveState : LargeNegativeState);
        }
    }
  }
  Flush();
  (void) free((char *) scanline);
  return((unsigned int) (q-compressed_pixels));
}
