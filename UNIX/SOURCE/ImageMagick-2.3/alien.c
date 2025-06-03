/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                        AAA   L      IIIII EEEEE  N   N                      %
%                       A   A  L        I   E      NN  N                      %
%                       AAAAA  L        I   EEE    N N N                      %
%                       A   A  L        I   E      N  NN                      %
%                       A   A  LLLLL  IIIII EEEEE  N   N                      %
%                                                                             %
%                                                                             %
%                  Utility Routines to Read Alien Image Formats               %
%                                                                             %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              January 1992                                   %
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
%  Functions in this library convert to and from `alien' image formats to the
%  MIFF image format.
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"
#include "alien.h"
#include "compress.h"
#include "X.h"
#include "XWDFile.h"

/*
  Global declarations.
*/
static char
  *AlienTypes[]=
  {
    "AVS",
    "CMYK",
    "FAX",
    "GIF",
    "GRAY",
    "HISTOGRAM",
    "JPEG",
    "JPG",
    "MIFF",
    "MTV",
    "PNM",
    "PS",
    "RGB",
    "RLE",
    "SUN",
    "TEXT",
    "TIFF",
    "VICAR",
    "YUV",
    "X",
    "XC",
    "XBM",
    "XWD",
    (char *) NULL,
  };

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
%  M S B F i r s t O r d e r L o n g                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MSBFirstOrderLong converts a least-significant byte first buffer
%  of long integers to most-significant byte first.
%
%  The format of the MSBFirstOrderLong routine is:
%
%       MSBFirstOrderLong(p,length);
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
static void MSBFirstOrderLong(p,length)
register char
  *p;

register unsigned
  length;
{
  register char
    c,
    *q,
    *sp;

  q=p+length;
  while (p < q)
  {
    sp=p+3;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    sp=p+1;
    c=(*sp);
    *sp=(*p);
    *p++=c;
    p+=2;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M S B F i r s t O r d e r S h o r t                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MSBFirstOrderShort converts a least-significant byte first buffer
%  of short integers to most-significant byte first.
%
%  The format of the MSBFirstOrderShort routine is:
%
%       MSBFirstOrderLongShort(p,length);
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
static void MSBFirstOrderShort(p,length)
register char
  *p;

register unsigned
  length;
{
  register char
    c,
    *q;

  q=p+length;
  while (p < q)
  {
    c=(*p);
    *p=(*(p+1));
    p++;
    *p++=c;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d A V S I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadAVSImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadAVSImage routine is:
%
%      image=ReadAVSImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadAVSImage returns a pointer to the image after
%      reading. A null image is returned if there is a a memory shortage or if
%      the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadAVSImage(alien_info)
AlienInfo
  *alien_info;
{
  typedef struct _Rasterfile
  {
    int
      width,
      height;
  } Rasterfile;

  Image
    *image;

  Rasterfile
    avs_header;

  register int
    i;

  register unsigned char
    *p;

  register RunlengthPacket
    *q;

  unsigned char
    *avs_pixels;

  unsigned int
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage("AVS");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Read AVS image.
  */
  status=ReadData((char *) &avs_header,1,sizeof(Rasterfile),image->file);
  if (status == False)
    {
      Warning("not a AVS image file",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    avs_pixels=(unsigned char *)
      malloc(4*avs_header.width*avs_header.height*sizeof(unsigned char));
    if (avs_pixels == (unsigned char *) NULL)
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    (void) ReadData((char *) avs_pixels,sizeof(unsigned char),
      avs_header.width*avs_header.height*4,image->file);
    /*
      Create image.
    */
    image->alpha=True;
    image->columns=avs_header.width;
    image->rows=avs_header.height;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
    image->comments=(char *)
      malloc((strlen(image->filename)+2048)*sizeof(char));
    if ((image->pixels == (RunlengthPacket *) NULL) ||
        (image->comments == (char *) NULL))
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    (void) sprintf(image->comments,"\n  Imported from AVS raster image:  %s\n",
      image->filename);
    /*
      Convert AVS raster image to runlength-encoded packets.
    */
    p=avs_pixels;
    q=image->pixels;
    for (i=0; i < (image->columns*image->rows); i++)
    {
      q->index=(unsigned char) (*p++);
      q->red=(*p++);
      q->green=(*p++);
      q->blue=(*p++);
      q->length=0;
      q++;
    }
    (void) free((char *) avs_pixels);
    status=ReadData((char *) &avs_header,1,sizeof(Rasterfile),image->file);
    if (status == True)
      {
        /*
          Allocate image structure.
        */
        image->next=AllocateImage("AVS");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",alien_info->filename,
          image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
  } while (status == True);
  while (image->last != (Image *) NULL)
    image=image->last;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d C M Y K I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadCMYKImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadCMYKImage routine is:
%
%      image=ReadCMYKImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadCMYKImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadCMYKImage(alien_info)
AlienInfo
  *alien_info;
{
  Image
    *image;

  int
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    black,
    *cmyk_pixels,
    cyan,
    magenta,
    yellow;

  unsigned int
    height,
    width;

  /*
    Allocate image structure.
  */
  image=AllocateImage("CMYK");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Create image.
  */
  width=512;
  height=512;
  if (alien_info->geometry != (char *) NULL)
    (void) XParseGeometry(alien_info->geometry,&x,&y,&width,&height);
  image->columns=width;
  image->rows=height;
  image->packets=image->columns*image->rows;
  cmyk_pixels=(unsigned char *)
    malloc((unsigned int) image->packets*4*sizeof(unsigned char));
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if ((cmyk_pixels == (unsigned char *) NULL) ||
      (image->pixels == (RunlengthPacket *) NULL))
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Convert raster image to runlength-encoded packets.
  */
  (void) ReadData((char *) cmyk_pixels,4,(int) (image->columns*image->rows),
    image->file);
  p=cmyk_pixels;
  q=image->pixels;
  for (i=0; i < (image->columns*image->rows); i++)
  {
    cyan=(*p++);
    magenta=(*p++);
    yellow=(*p++);
    black=(*p++);
    if ((unsigned int) (cyan+black) > MaxRGB)
      q->red=0;
    else
      q->red=MaxRGB-(cyan+black);
    if ((unsigned int) (magenta+black) > MaxRGB)
      q->green=0;
    else
      q->green=MaxRGB-(magenta+black);
    if ((unsigned int) (yellow+black) > MaxRGB)
      q->blue=0;
    else
      q->blue=MaxRGB-(yellow+black);
    q->index=0;
    q->length=0;
    q++;
  }
  (void) free((char *) cmyk_pixels);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d G I F I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadGIFImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadGIFImage routine is:
%
%      image=ReadGIFImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadGIFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      an error occurs.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadGIFImage(alien_info)
AlienInfo
  *alien_info;
{
#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
#define LSBFirstOrder(x,y)  (((y) << 8) | (x))

  Image
    *image;

  int
    pass,
    status,
    x,
    y;

  register int
    i;

  unsigned char
    c,
    *colormap,
    header[2048],
    magick[12];

  unsigned int
    image_count,
    interlace,
    local_colormap;

  /*
    Allocate image structure.
  */
  image=AllocateImage("GIF");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Determine if this is a GIF file.
  */
  status=ReadData((char *) magick,1,6,image->file);
  if ((status == False) || ((strncmp((char *) magick,"GIF87",5) != 0) &&
      (strncmp((char *) magick,"GIF89",5) != 0)))
    {
      Warning("not a GIF image file",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Read the screen descriptor.
  */
  status=ReadData((char *) header,1,7,image->file);
  if (status == False)
    {
      Warning("failed to read screen descriptor",(char *) NULL );
      DestroyImage(image);
      return((Image *) NULL);
    }
  image->colors=1 << ((header[4] & 0x07)+1);
  if (BitSet(header[4],0x80))
    {
      unsigned char
        *p;

      /*
        Read global colormap.
      */
      image->class=PseudoClass;
      colormap=(unsigned char *) malloc(3*image->colors*sizeof(unsigned char));
      image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
      if ((colormap == (unsigned char *) NULL) ||
          (image->colormap == (ColorPacket *) NULL))
        {
          Warning("unable to read image colormap","memory allocation failed");
          DestroyImage(image);
          return((Image *) NULL);
        }
      (void) ReadData((char *) colormap,1,(int) image->colors*3,image->file);
      p=colormap;
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=(*p++);
        image->colormap[i].green=(*p++);
        image->colormap[i].blue=(*p++);
      }
      (void) free((char *) colormap);
    }
  image_count=0;
  for ( ; ; )
  {
    status=ReadData((char *) &c,1,1,image->file);
    if (status == False)
      break;
    if (c == ';')
      break;  /* terminator */
    if (c == '!')
      {
        /*
          GIF Extension block.
        */
        status=ReadData((char *) &c,1,1,image->file);
        if (status == False)
          {
            Warning("unable to read extention block",(char *) NULL);
            DestroyImage(image);
            return((Image *) NULL);
          }
        if (c != 0xfe)
          while (ReadDataBlock((char *) header,image->file) != 0);
        else
          while (ReadDataBlock((char *) header,image->file) != 0)
          {
            /*
              Comment extension block.
            */
            if (image->comments != (char *) NULL)
              image->comments=(char *) realloc((char *) image->comments,
                (unsigned int) (strlen(image->comments)+
                strlen((char *) header)+1));
            else
              {
                image->comments=(char *)
                  malloc((strlen((char *) header)+1)*sizeof(char));
                *image->comments='\0';
              }
            if (image->comments == (char *) NULL)
              {
                Warning("memory allocation error",(char *) NULL);
                DestroyImage(image);
                return((Image *) NULL);
              }
            (void) strcat(image->comments,(char *) header);
          }
      }
    if (c != ',')
      continue;
    /*
      Read image attributes.
    */
    if (image_count != 0)
      {
        /*
          Allocate image structure.
        */
        CompressColormap(image);
        image->next=AllocateImage("GIF");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",alien_info->filename,
          image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
    image_count++;
    status=ReadData((char *) header,1,9,image->file);
    if (status == False)
      {
        Warning("unable to read left/top/width/height",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    interlace=BitSet(header[8],0x40);
    local_colormap=BitSet(header[8],0x80);
    /*
      Allocate image.
    */
    image->columns=LSBFirstOrder(header[4],header[5]);
    image->rows=LSBFirstOrder(header[6],header[7]);
    image->packets=image->columns*image->rows;
    if (image->pixels != (RunlengthPacket *) NULL)
      (void) free((char *) image->pixels);
    image->pixels=(RunlengthPacket *)
      malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    if (local_colormap)
      {
        unsigned char
          *p;

        /*
          Read local colormap.
        */
        image->class=PseudoClass;
        image->colors=1 << ((header[8] & 0x07)+1);
        colormap=(unsigned char *)
          malloc(3*image->colors*sizeof(unsigned char));
        if (image->colormap != (ColorPacket *) NULL)
          (void) free((char *) image->colormap);
        image->colormap=(ColorPacket *)
          malloc(image->colors*sizeof(ColorPacket));
        if ((colormap == (unsigned char *) NULL) ||
            (image->colormap == (ColorPacket *) NULL))
          {
            Warning("unable to read image","memory allocation failed");
            DestroyImage(image);
            return((Image *) NULL);
          }
        (void) ReadData((char *) colormap,1,(int) image->colors*3,image->file);
        p=colormap;
        for (i=0; i < image->colors; i++)
        {
          image->colormap[i].red=(*p++);
          image->colormap[i].green=(*p++);
          image->colormap[i].blue=(*p++);
        }
        (void) free((char *) colormap);
      }
    /*
      Decode image.
    */
    status=LZWDecodeImage(image);
    if (status == False)
      {
        Warning("unable to read image data",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    if (interlace)
      {
        Image
          *interlaced_image;

        register RunlengthPacket
          *p,
          *q;

        static int
          interlace_rate[4] = { 8, 8, 4, 2 },
          interlace_start[4] = { 0, 4, 2, 1 };

        /*
          Interlace image.
        */
        interlaced_image=image;
        image=CopyImage(interlaced_image,interlaced_image->columns,
          interlaced_image->rows,False);
        if (image == (Image *) NULL)
          {
            Warning("unable to read image","memory allocation failed");
            DestroyImage(interlaced_image);
            return((Image *) NULL);
          }
        p=interlaced_image->pixels;
        q=image->pixels;
        for (pass=0; pass < 4; pass++)
        {
          y=interlace_start[pass];
          while (y < image->rows)
          {
            q=image->pixels+(y*image->columns);
            for (x=0; x < image->columns; x++)
              *q++=(*p++);
            y+=interlace_rate[pass];
          }
        }
        DestroyImage(interlaced_image);
      }
  }
  CompressColormap(image);
  while (image->last != (Image *) NULL)
    image=image->last;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d G R A Y I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadGRAYImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadGRAYImage routine is:
%
%      image=ReadGRAYImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadGRAYImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadGRAYImage(alien_info)
AlienInfo
  *alien_info;
{
  Image
    *image;

  int
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    index,
    *p;

  unsigned char
    *gray_pixels;

  unsigned int
    height,
    width;

  /*
    Allocate image structure.
  */
  image=AllocateImage("GRAY");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Create linear colormap.
  */
  image->class=PseudoClass;
  image->colors=256;
  image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  for (i=0; i < image->colors; i++)
  {
    image->colormap[i].red=(unsigned char) i;
    image->colormap[i].green=(unsigned char) i;
    image->colormap[i].blue=(unsigned char) i;
  }
  /*
    Create image.
  */
  width=512;
  height=512;
  if (alien_info->geometry != (char *) NULL)
    (void) XParseGeometry(alien_info->geometry,&x,&y,&width,&height);
  image->columns=width;
  image->rows=height;
  image->packets=image->columns*image->rows;
  gray_pixels=(unsigned char *)
    malloc((unsigned int) image->packets*sizeof(unsigned char));
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if ((gray_pixels == (unsigned char *) NULL) ||
      (image->pixels == (RunlengthPacket *) NULL))
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Convert raster image to runlength-encoded packets.
  */
  (void) ReadData((char *) gray_pixels,1,(int) (image->columns*image->rows),
    image->file);
  p=gray_pixels;
  q=image->pixels;
  for (i=0; i < (image->columns*image->rows); i++)
  {
    index=(*p++);
    q->red=index;
    q->green=index;
    q->blue=index;
    q->index=(unsigned short) index;
    q->length=0;
    q++;
  }
  (void) free((char *) gray_pixels);
  CompressColormap(image);
  CloseImage(image);
  return(image);
}

#ifdef AlienJPEG
#undef FREAD
#undef FWRITE
#undef const
#include "jinclude.h"
static Image
  *jpeg_image;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d J P E G I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadJPEGImage reads an image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadJPEGImage routine is:
%
%      image=ReadJPEGImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadJPEGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o filename:  Specifies the name of the jpeg_image to read.
%
%
*/

static void MIFFInitializeImage(jpeg_info)
decompress_info_ptr
  jpeg_info;
{
  /*
    Create jpeg_image.
  */
  jpeg_image->columns=jpeg_info->image_width;
  jpeg_image->rows=jpeg_info->image_height;
  jpeg_image->packets=jpeg_image->columns*jpeg_image->rows;
  jpeg_image->pixels=(RunlengthPacket *)
    malloc(jpeg_image->packets*sizeof(RunlengthPacket));
  jpeg_image->comments=(char *)
    malloc((strlen(jpeg_image->filename)+2048)*sizeof(char));
  if ((jpeg_image->pixels == (RunlengthPacket *) NULL) ||
      (jpeg_image->comments == (char *) NULL))
    {
      Warning("memory allocation error",(char *) NULL);
      exit(1);
    }
  (void) sprintf(jpeg_image->comments,
    "\n  Imported from JFIF JPEG image:  %s\n",jpeg_image->filename);
  jpeg_image->packet=jpeg_image->pixels;
}

static void MIFFOutputTermMethod(jpeg_info)
decompress_info_ptr
  jpeg_info;
{
}

static void MIFFWriteGRAY(jpeg_info,number_rows,pixel_data)
decompress_info_ptr
  jpeg_info;

int
  number_rows;

JSAMPIMAGE
  pixel_data;
{
  register int
    column,
    row;

  register JSAMPROW
    gray;

  register RunlengthPacket
    *p;

  /*
    Transfer grayscale JPEG pixel data to MIFF pixels.
  */
  p=jpeg_image->packet;
  for (row=0; row < number_rows; row++)
  {
    gray=pixel_data[0][row];
    for (column=jpeg_info->image_width; column > 0; column--)
    {
      p->red=GETJSAMPLE(*gray);
      p->green=GETJSAMPLE(*gray);
      p->blue=GETJSAMPLE(*gray);
      p->index=(unsigned short) GETJSAMPLE(*gray);
      p->length=0;
      p++;
      gray++;
    }
  }
  jpeg_image->packet=p;
}

static void MIFFWriteRGB(jpeg_info,number_rows,pixel_data)
decompress_info_ptr
  jpeg_info;

int
  number_rows;

JSAMPIMAGE
  pixel_data;
{
  register int
    column,
    row;

  register JSAMPROW
    blue,
    green,
    red;

  register RunlengthPacket
    *p;

  /*
    Transfer JPEG pixel data to MIFF pixels.
  */
  p=jpeg_image->packet;
  for (row=0; row < number_rows; row++)
  {
    red=pixel_data[0][row];
    green=pixel_data[1][row];
    blue=pixel_data[2][row];
    for (column=jpeg_info->image_width; column > 0; column--)
    {
      p->red=GETJSAMPLE(*red++);
      p->green=GETJSAMPLE(*green++);
      p->blue=GETJSAMPLE(*blue++);
      p->index=0;
      p->length=0;
      p++;
    }
  }
  jpeg_image->packet=p;
}

static void MIFFSelectMethod(jpeg_info)
decompress_info_ptr
  jpeg_info;
{
  jpeg_info->methods->put_pixel_rows=MIFFWriteRGB;
  jpeg_info->out_color_space=CS_RGB;
  if (jpeg_info->jpeg_color_space == CS_GRAYSCALE)
    {
      jpeg_info->out_color_space=CS_GRAYSCALE;
      jpeg_info->methods->put_pixel_rows=MIFFWriteGRAY;
    }
  jpeg_info->data_precision=8;
}

static Image *ReadJPEGImage(alien_info)
AlienInfo
  *alien_info;
{
  struct Decompress_info_struct
    jpeg_info;

  struct Decompress_methods_struct
    jpeg_methods;

  struct External_methods_struct
    external_methods;

  /*
    Allocate jpeg_image structure.
  */
  jpeg_image=AllocateImage("JPEG");
  if (jpeg_image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(jpeg_image->filename,alien_info->filename);
  OpenImage(jpeg_image,"r");
  if (jpeg_image->file == (FILE *) NULL)
    {
      Warning("unable to open file",jpeg_image->filename);
      DestroyImage(jpeg_image);
      return((Image *) NULL);
    }
  /*
    Initialize the JPEG system-dependent methods.
  */
  jpeg_info.methods=(&jpeg_methods);
  jpeg_info.emethods=(&external_methods);
  jselerror(&external_methods);
  jselmemmgr(&external_methods);
  jpeg_info.methods->output_init=MIFFInitializeImage;
  jpeg_info.methods->output_term=MIFFOutputTermMethod;
  jpeg_methods.d_ui_method_selection=MIFFSelectMethod;
  j_d_defaults(&jpeg_info,True);
  /*
    Read a JFIF JPEG file.
  */
  jpeg_info.input_file=jpeg_image->file;
  jpeg_info.output_file=(FILE *) NULL;
  jselrjfif(&jpeg_info);
  jpeg_decompress(&jpeg_info);
  if (jpeg_info.jpeg_color_space == CS_GRAYSCALE)
    {
      register int
        i;

      /*
        Initialize grayscale colormap.
      */
      jpeg_image->class=PseudoClass;
      jpeg_image->colors=256;
      jpeg_image->colormap=(ColorPacket *)
        malloc(jpeg_image->colors*sizeof(ColorPacket));
      if (jpeg_image->colormap == (ColorPacket *) NULL)
        {
          Warning("unable to create image colormap","memory allocation failed");
          DestroyImage(jpeg_image);
          return((Image *) NULL);
        }
      for (i=0; i < jpeg_image->colors; i++)
      {
        jpeg_image->colormap[i].red=(unsigned short) i;
        jpeg_image->colormap[i].green=(unsigned short) i;
        jpeg_image->colormap[i].blue=(unsigned short) i;
      }
    }
  CloseImage(jpeg_image);
  return(jpeg_image);
}
#else
static Image *ReadJPEGImage(alien_info)
AlienInfo
  *alien_info;
{
  Warning("JPEG library is not available",alien_info->filename);
  return(ReadImage(alien_info->filename));
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d M T V I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadMTVImage reads an image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadMTVImage routine is:
%
%      image=ReadMTVImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadMTVImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadMTVImage(alien_info)
AlienInfo
  *alien_info;
{
  Image
    *image;

  int
    count;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *mtv_pixels;

  unsigned int
    columns,
    rows;

  /*
    Allocate image structure.
  */
  image=AllocateImage("MTV");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Read MTV image.
  */
  count=fscanf(image->file,"%u %u\n",&columns,&rows);
  if (count == 0)
    {
      Warning("not a MTV image,",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    /*
      Allocate image pixels.
    */
    mtv_pixels=(unsigned char *) malloc(3*columns*rows*sizeof(unsigned char));
    if (mtv_pixels == (unsigned char *) NULL)
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    /*
      Read image pixels.
    */
    (void) ReadData((char *) mtv_pixels,1,(int) (columns*rows*3),image->file);
    /*
      Create image.
    */
    image->columns=columns;
    image->rows=rows;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
    image->comments=(char *)
      malloc((strlen(image->filename)+2048)*sizeof(char));
    if ((image->pixels == (RunlengthPacket *) NULL) ||
        (image->comments == (char *) NULL))
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    (void) sprintf(image->comments,"\n  Imported from MTV raster image:  %s\n",
      image->filename);
    /*
      Convert MTV raster image to runlength-encoded packets.
    */
    p=mtv_pixels;
    q=image->pixels;
    for (i=0; i < image->columns*image->rows; i++)
    {
      q->red=(*p++);
      q->green=(*p++);
      q->blue=(*p++);
      q->index=0;
      q->length=0;
      q++;
    }
    (void) free((char *) mtv_pixels);
    /*
      Proceed to next image.
    */
    count=fscanf(image->file,"%u %u\n",&columns,&rows);
    if (count > 0)
      {
        /*
          Allocate next image structure.
        */
        image->next=AllocateImage("MTV");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",alien_info->filename,
          image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
  } while (count > 0);
  while (image->last != (Image *) NULL)
    image=image->last;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d P N M I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadPNMImage reads an image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadPNMImage routine is:
%
%      image=ReadPNMImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadPNMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/

static unsigned int GetInteger(file)
FILE
  *file;
{
  char
    c;

  int
    status;

  unsigned int
    value;

  /*
    Skip any leading whitespace.
  */
  do
  {
    status=ReadData(&c,1,1,file);
    if (status == False)
      return(0);
    if (c == '#')
      do
      {
        /*
          Skip any comments.
        */
        status=ReadData(&c,1,1,file);
        if (status == False)
          return(0);
      } while (c != '\n');
  } while (!isdigit(c));
  /*
    Evaluate number.
  */
  value=0;
  do
  {
    value*=10;
    value+=c-'0';
    status=ReadData(&c,1,1,file);
    if (status == False)
      return(0);
  }
  while (isdigit(c));
  return(value);
}

static Image *ReadPNMImage(alien_info)
AlienInfo
  *alien_info;
{
  char
    format;

  Image
    *image;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  register unsigned short int
    index;

  unsigned char
    *pixels,
    *scale;

  unsigned int
    max_value,
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage("PNM");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Read PNM image.
  */
  status=ReadData((char *) &format,1,1,image->file);
  if ((status == False) || (format != 'P'))
    {
      Warning("not a PNM image file",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    /*
      Create image.
    */
    status=ReadData((char *) &format,1,1,image->file);
    image->columns=GetInteger(image->file);
    image->rows=GetInteger(image->file);
    if ((image->columns*image->rows) == 0)
      {
        Warning("unable to read image","image dimensions are zero");
        return((Image *) NULL);
      }
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
    image->comments=(char *)
      malloc((strlen(image->filename)+2048)*sizeof(char));
    if ((image->pixels == (RunlengthPacket *) NULL) ||
        (image->comments == (char *) NULL))
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    (void) sprintf(image->comments,"\n  Imported from PNM raster image:  %s\n",
      image->filename);
    if ((format == '1') || (format == '4'))
      max_value=1;  /* bitmap */
    else
      max_value=GetInteger(image->file);
    scale=(unsigned char *) NULL;
    if (max_value > MaxRGB)
      {
        /*
          Compute pixel scaling table.
        */
        scale=(unsigned char *) malloc((max_value+1)*sizeof(unsigned char));
        if (scale == (unsigned char *) NULL)
          {
            Warning("unable to read image","memory allocation failed");
            DestroyImages(image);
            return((Image *) NULL);
          }
        for (i=0; i <= max_value; i++)
          scale[i]=(unsigned char) ((i*MaxRGB+(max_value >> 1))/max_value);
      }
    if ((format != '3') && (format != '6'))
      {
        /*
          Create gray scale colormap.
        */
        image->class=PseudoClass;
        image->colors=Min(max_value,MaxRGB)+1;
        image->colormap=(ColorPacket *)
          malloc(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImages(image);
            return((Image *) NULL);
          }
        for (i=0; i < image->colors; i++)
        {
          image->colormap[i].red=(MaxRGB*i)/(image->colors-1);
          image->colormap[i].green=(MaxRGB*i)/(image->colors-1);
          image->colormap[i].blue=(MaxRGB*i)/(image->colors-1);
        }
      }
    /*
      Convert PNM pixels to runlength-encoded MIFF packets.
    */
    q=image->pixels;
    switch (format)
    {
      case '1':
      {
        /*
          Convert PBM image to runlength-encoded packets.
        */
        for (i=0; i < image->packets; i++)
        {
          index=GetInteger(image->file);
          if (index > 1)
            index=1;
          q->red=image->colormap[index].red;
          q->green=image->colormap[index].green;
          q->blue=image->colormap[index].blue;
          q->index=index;
          q->length=0;
          q++;
        }
        break;
      }
      case '2':
      {
        /*
          Convert PGM image to runlength-encoded packets.
        */
        if (max_value <= MaxRGB)
          for (i=0; i < image->packets; i++)
          {
            index=GetInteger(image->file);
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->length=0;
            q++;
          }
        else
          for (i=0; i < image->packets; i++)
          {
            index=scale[GetInteger(image->file)];
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->index=0;
            q->length=0;
            q++;
          }
        break;
      }
      case '3':
      {
        /*
          Convert PNM image to runlength-encoded packets.
        */
        if (max_value <= MaxRGB)
          for (i=0; i < image->packets; i++)
          {
            q->red=GetInteger(image->file);
            q->green=GetInteger(image->file);
            q->blue=GetInteger(image->file);
            q->index=0;
            q->length=0;
            q++;
          }
        else
          for (i=0; i < image->packets; i++)
          {
            q->red=scale[GetInteger(image->file)];
            q->green=scale[GetInteger(image->file)];
            q->blue=scale[GetInteger(image->file)];
            q->index=0;
            q->length=0;
            q++;
          }
        break;
      }
      case '4':
      {
        unsigned char
          bit,
          byte;

        unsigned int
          x,
          y;

        /*
          Convert PBM raw image to runlength-encoded packets.
        */
        for (y=0; y < image->rows; y++)
        {
          bit=0;
          for (x=0; x < image->columns; x++)
          {
            if (bit == 0)
              (void) ReadData((char *) &byte,1,1,image->file);
            q->index=(byte & 0x80) ? 0 : 1;
            byte<<=1;
            q->red=image->colormap[q->index].red;
            q->green=image->colormap[q->index].green;
            q->blue=image->colormap[q->index].blue;
            q->length=0;
            q++;
            bit++;
            if (bit == 8)
              bit=0;
          }
        }
        break;
      }
      case '5':
      {
        /*
          Convert PGM raw image to runlength-encoded packets.
        */
        pixels=(unsigned char *)
          malloc((unsigned int) image->packets*sizeof(unsigned char));
        if (pixels == (unsigned char *) NULL)
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImages(image);
            return((Image *) NULL);
          }
        status=ReadData((char *) pixels,1,(int) image->packets,image->file);
        if (status == False)
          {
            Warning("insufficient image data in file",image->filename);
            DestroyImages(image);
            return((Image *) NULL);
          }
        /*
          Convert PNM raw image to runlength-encoded packets.
        */
        p=pixels;
        if (max_value <= MaxRGB)
          for (i=0; i < image->packets; i++)
          {
            index=(*p++);
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->length=0;
            q++;
          }
        else
          for (i=0; i < image->packets; i++)
          {
            index=scale[*p++];
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->index=0;
            q->length=0;
            q++;
          }
        break;
      }
      case '6':
      {
        /*
          Convert PNM raster image to runlength-encoded packets.
        */
        pixels=(unsigned char *)
          malloc((unsigned int) image->packets*3*sizeof(unsigned char));
        if (pixels == (unsigned char *) NULL)
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImages(image);
            return((Image *) NULL);
          }
        status=ReadData((char *) pixels,1,(int) image->packets*3,image->file);
        if (status == False)
          {
            Warning("insufficient image data in file",image->filename);
            DestroyImages(image);
            return((Image *) NULL);
          }
        p=pixels;
        if (max_value <= MaxRGB)
          for (i=0; i < image->packets; i++)
          {
            q->red=(*p++);
            q->green=(*p++);
            q->blue=(*p++);
            q->index=0;
            q->length=0;
            q++;
          }
        else
          for (i=0; i < image->packets; i++)
          {
            q->red=scale[*p++];
            q->green=scale[*p++];
            q->blue=scale[*p++];
            q->index=0;
            q->length=0;
            q++;
          }
        break;
      }
      default:
      {
        Warning("not a PNM image file",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    }
    if (scale != (unsigned char *) NULL)
      (void) free((char *) scale);
    if (image->class == PseudoClass)
      CompressColormap(image);
    /*
      Proceed to next image.
    */
    status=ReadData((char *) &format,1,1,image->file);
    if ((status == True) && (format == 'P'))
      {
        /*
          Allocate image structure.
        */
        image->next=AllocateImage("PNM");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",alien_info->filename,
          image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
  } while ((status == True) && (format == 'P'));
  while (image->last != (Image *) NULL)
    image=image->last;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d P S I m a g e                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadPSImage reads a Postscript image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadPSImage routine is:
%
%      image=ReadPSImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadPSImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%    o density: Specifies a pointer to a image density string;  horizonal
%      and vertical dots per inch.
%
%
*/
static Image *ReadPSImage(alien_info)
AlienInfo
  *alien_info;
{
#define XResolution  72
#define YResolution  72

  char
    command[2048],
    clip_geometry[2048],
    *directory,
    filename[2048],
    options[2048];

  Image
    *image,
    *next_image;

  unsigned int
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage("PS");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Determine if Postscript specifies a bounding box.
  */
  *clip_geometry='\0';
  while (fgets(command,sizeof(command)-1,image->file) != (char *) NULL)
    if (strncmp("%%BoundingBox:",command,strlen("%%BoundingBox:")) == 0)
      {
        int
          count,
          lower_x,
          lower_y,
          upper_x,
          upper_y,
          x,
          y;

        unsigned int
          x_resolution,
          y_resolution;

        count=sscanf(command,"%%%%BoundingBox: %d %d %d %d",&lower_x,&lower_y,
          &upper_x,&upper_y);
        if (count != 4)
          break;
        /*
          Set clip geometry as specified by the bounding box.
        */
        x_resolution=XResolution;
        y_resolution=YResolution;
        if (alien_info->density != (char *) NULL)
          {
            int
              flags;

            /*
              User specified density.
            */
            flags=XParseGeometry(alien_info->density,&x,&y,&x_resolution,
              &y_resolution);
            if ((flags & WidthValue) == 0)
              x_resolution=XResolution;
            if ((flags & HeightValue) == 0)
              y_resolution=x_resolution;
          }
        (void) sprintf(clip_geometry,"%ux%u+%u-%u\0",
          ((upper_x-lower_x)*x_resolution+(XResolution >> 1))/XResolution,
          ((upper_y-lower_y)*y_resolution+(YResolution >> 1))/YResolution,
          (lower_x*x_resolution+(XResolution >> 1))/XResolution,
          ((lower_y*y_resolution+(YResolution >> 1))/YResolution));
        break;
      }
  CloseImage(image);
  /*
    Rendered Postscript goes to temporary PNM file.
  */
  directory=(char *) getenv("TMPDIR");
  if (directory == (char *) NULL)
    directory="/tmp";
  (void) sprintf(filename,"%s/magickXXXXXX",directory);
  (void) mktemp(filename);
  /*
    Determine if geometry or density options are specified.
  */
  *options='\0';
  if (alien_info->geometry != (char *) NULL)
    {
      (void) strcat(options," -g");
      (void) strcat(options,alien_info->geometry);
    }
  if (alien_info->density != (char *) NULL)
    {
      (void) strcat(options," -r");
      (void) strcat(options,alien_info->density);
    }
  /*
    Use Ghostscript to convert Postscript image.
  */
  (void) sprintf(command,"gs -q -sDEVICE=ppmraw -sOutputFile=%s %s %s",
    filename,options,alien_info->filename);
  (void) strcat(command," < /dev/null > /dev/null");
  status=system(command);
  if (status != 0)
    {
      Warning("unable to execute ghostscript (gs)",alien_info->filename);
      return((Image *) NULL);
    }
  (void) strcpy(alien_info->filename,filename);
  (void) strcpy(filename,image->filename);
  DestroyImage(image);
  image=ReadPNMImage(alien_info);
  (void) unlink(alien_info->filename);
  if (image == (Image *) NULL)
    {
      Warning("Postscript translation failed",alien_info->filename);
      return((Image *) NULL);
    }
  do
  {
    if (image->last == (Image *) NULL)
      (void) strcpy(image->filename,filename);
    else
      (void) sprintf(image->filename,"%s.%u\0",filename,image->scene);
    (void) strcpy(image->magick,"PS");
    if (*clip_geometry != '\0')
      {
        /*
          Clip image as specified by the bounding box.
        */
        TransformImage(&image,clip_geometry,(char *) NULL,(char *) NULL);
        if (image->next != (Image *) NULL)
          image->next->last=image;
      }
    next_image=image->next;
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  while (image->last != (Image *) NULL)
    image=image->last;
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d R G B I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadRGBImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadRGBImage routine is:
%
%      image=ReadRGBImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadRGBImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadRGBImage(alien_info)
AlienInfo
  *alien_info;
{
  Image
    *image;

  int
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *rgb_pixels;

  unsigned int
    height,
    width;

  /*
    Allocate image structure.
  */
  image=AllocateImage("RGB");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Create image.
  */
  width=512;
  height=512;
  if (alien_info->geometry != (char *) NULL)
    (void) XParseGeometry(alien_info->geometry,&x,&y,&width,&height);
  image->columns=width;
  image->rows=height;
  image->packets=image->columns*image->rows;
  rgb_pixels=(unsigned char *)
    malloc((unsigned int) image->packets*3*sizeof(unsigned char));
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if ((rgb_pixels == (unsigned char *) NULL) ||
      (image->pixels == (RunlengthPacket *) NULL))
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Convert raster image to runlength-encoded packets.
  */
  (void) ReadData((char *) rgb_pixels,3,(int) (image->columns*image->rows),
    image->file);
  p=rgb_pixels;
  q=image->pixels;
  for (i=0; i < (image->columns*image->rows); i++)
  {
    q->red=(*p++);
    q->green=(*p++);
    q->blue=(*p++);
    q->index=0;
    q->length=0;
    q++;
  }
  (void) free((char *) rgb_pixels);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d R L E I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadRLEImage reads a run-length encoded Utah Raster Toolkit
%  image file and returns it.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  The format of the ReadRLEImage routine is:
%
%      image=ReadRLEImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadRLEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadRLEImage(alien_info)
AlienInfo
  *alien_info;
{
#define SkipLinesOp  0x01
#define SetColorOp  0x02
#define SkipPixelsOp  0x03
#define ByteDataOp  0x05
#define RunDataOp  0x06
#define EOFOp  0x07

  char
    magick[12];

  Image
    *image;

  int
    opcode,
    operand,
    status,
    x,
    y;

  register int
    i,
    j;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    background_color[256],
    *colormap,
    header[2048],
    integer[2],
    pixel,
    plane,
    *rle_pixels;

  unsigned int
    bits_per_pixel,
    flags,
    map_length,
    number_colormaps,
    number_planes;

  /*
    Allocate image structure.
  */
  image=AllocateImage("RLE");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Determine if this is a RLE file.
  */
  status=ReadData((char *) magick,1,2,image->file);
  if ((status == False) || (strncmp(magick,"\122\314",2) != 0))
    {
      Warning("not a RLE image file",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    /*
      Read image header.
    */
    status=ReadData((char *) header,1,13,image->file);
    if (status == False)
      {
        Warning("unable to read RLE image header",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    image->columns=header[4]+(header[5] << 8);
    image->rows=header[6]+(header[7] << 8);
    image->packets=image->columns*image->rows;
    flags=header[8];
    image->alpha=flags & 0x04;
    number_planes=header[9];
    bits_per_pixel=header[10];
    number_colormaps=header[11];
    map_length=1 << header[12];
    if ((number_planes == 0) || (number_planes == 2) || (bits_per_pixel != 8) ||
        ((image->columns*image->rows) == 0))
      {
        Warning("unsupported RLE image file",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    if (flags & 0x02)
      {
        /*
          No background color-- initialize to black.
        */
        for (i=0; i < number_planes; i++)
          background_color[i]=(unsigned char) 0;
        (void) fgetc(image->file);
      }
    else
      {
        /*
          Initialize background color.
        */
        p=background_color;
        for (i=0; i < number_planes; i++)
          *p++=(unsigned char) fgetc(image->file);
        if ((number_planes % 2) == 0)
          (void) fgetc(image->file);
      }
    if (number_colormaps != 0)
      {
        /*
          Read image colormaps.
        */
        colormap=(unsigned char *)
          malloc(number_colormaps*map_length*sizeof(unsigned char));
        if (colormap == (unsigned char *) NULL)
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImage(image);
            return((Image *) NULL);
          }
        p=colormap;
        for (i=0; i < number_colormaps; i++)
          for (j=0; j < map_length; j++)
          {
            (void) fgetc(image->file);
            *p++=(unsigned char) fgetc(image->file);
          }
      }
    /*
      Allocate image comment.
    */
    image->comments=(char *)
      malloc((strlen(image->filename)+2048)*sizeof(char));
    if (image->comments == (char *) NULL)
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    (void) sprintf(image->comments,"\n  Imported from Utah raster image:  %s\n",
      image->filename);
    if (flags & 0x08)
      {
        unsigned int
          comment_length;

        /*
          Read image comment.
        */
        (void) ReadData((char *) integer,1,2,image->file);
        comment_length=integer[0]+(integer[1] << 8);
        image->comments=(char *) realloc((char *) image->comments,
          (unsigned int) (strlen(image->comments)+comment_length+2048));
        if (image->comments == (char *) NULL)
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImage(image);
            return((Image *) NULL);
          }
        (void) strcat(image->comments,"\n  ");
        status=ReadData((char *) image->comments+strlen(image->comments),1,
          (int) comment_length+(comment_length % 2 ? 0 : 1),image->file);
        if (status == False)
          {
            Warning("unable to read RLE comment",(char *) NULL);
            DestroyImage(image);
            return((Image *) NULL);
          }
        (void) strcat(image->comments,"\n");
      }
    /*
      Allocate RLE pixels.
    */
    if (image->alpha)
      number_planes++;
    rle_pixels=(unsigned char *)
      malloc((unsigned int) image->packets*number_planes*sizeof(unsigned char));
    if (rle_pixels == (unsigned char *) NULL)
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    if ((flags & 0x01) && ((~flags) & 0x02))
      {
        /*
          Set background color.
        */
        p=rle_pixels;
        for (i=0; i < image->packets; i++)
        {
          if (!image->alpha)
            for (j=0; j < number_planes; j++)
              *p++=background_color[j];
          else
            {
              for (j=0; j < (number_planes-1); j++)
                *p++=background_color[j];
              *p++=0;  /* initialize alpha channel */
            }
        }
      }
    /*
      Read runlength-encoded image.
    */
    plane=0;
    x=0;
    y=0;
    (void) fgetc(image->file);
    opcode=fgetc(image->file);
    while (((opcode & 0x3f) != EOFOp) && (opcode != EOF))
    {
      switch (opcode & 0x3f)
      {
        case SkipLinesOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            {
              (void) ReadData((char *) integer,1,2,image->file);
              operand=integer[0]+(integer[1] << 8);
            }
          x=0;
          y+=operand;
          break;
        }
        case SetColorOp:
        {
          operand=fgetc(image->file);
          plane=operand;
          if (plane == 255)
            plane=number_planes-1;
          x=0;
          break;
        }
        case SkipPixelsOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            {
              (void) ReadData((char *) integer,1,2,image->file);
              operand=integer[0]+(integer[1] << 8);
            }
          x+=operand;
          break;
        }
        case ByteDataOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            {
              (void) ReadData((char *) integer,1,2,image->file);
              operand=integer[0]+(integer[1] << 8);
            }
          p=rle_pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          operand++;
          for (i=0; i < operand; i++)
          {
            pixel=fgetc(image->file);
            if ((y < image->rows) && ((x+i) < image->columns))
              *p=pixel;
            p+=number_planes;
          }
          if (operand & 0x01)
            (void) fgetc(image->file);
          x+=operand;
          break;
        }
        case RunDataOp:
        {
          operand=fgetc(image->file);
          if (opcode & 0x40)
            {
              (void) ReadData((char *) integer,1,2,image->file);
              operand=integer[0]+(integer[1] << 8);
            }
          pixel=fgetc(image->file);
          (void) fgetc(image->file);
          operand++;
          p=rle_pixels+((image->rows-y-1)*image->columns*number_planes)+
            x*number_planes+plane;
          for (i=0; i < operand; i++)
          {
            if ((y < image->rows) && ((x+i) < image->columns))
              *p=pixel;
            p+=number_planes;
          }
          x+=operand;
          break;
        }
        default:
        {
          Warning("Unrecognized RLE opcode",image->filename);
          DestroyImage(image);
          return((Image *) NULL);
        }
      }
      opcode=fgetc(image->file);
    }
    if (number_colormaps != 0)
      {
        unsigned char
          pixel;

        unsigned int
          mask;

        /*
          Apply colormap transformation to image.
        */
        mask=(map_length-1);
        p=rle_pixels;
        for (i=0; i < image->packets; i++)
          for (j=0; j < number_planes; j++)
          {
            pixel=colormap[j*map_length+(*p & mask)];
            *p++=pixel;
          }
        (void) free((char *) colormap);
      }
    /*
      Create image.
    */
    image->pixels=(RunlengthPacket *)
      malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
    if (image->pixels == (RunlengthPacket *) NULL)
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImage(image);
        return((Image *) NULL);
      }
    p=rle_pixels;
    q=image->pixels;
    if (number_planes >= 3)
      {
        /*
          Convert raster image to DirectClass runlength-encoded packets.
        */
        for (i=0; i < (image->columns*image->rows); i++)
        {
          q->red=(*p++);
          q->green=(*p++);
          q->blue=(*p++);
          q->index=(unsigned short) (image->alpha ? (*p++) : 0);
          q->length=0;
          q++;
        }
      }
    else
      {
        unsigned short
          index;

        /*
          Create grayscale map.
        */
        image->class=PseudoClass;
        image->colors=256;
        image->colormap=(ColorPacket *)
          malloc(image->colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          {
            Warning("unable to read image","memory allocation failed");
            DestroyImage(image);
            return((Image *) NULL);
          }
        for (i=0; i < image->colors; i++)
        {
          image->colormap[i].red=(unsigned char) i;
          image->colormap[i].green=(unsigned char) i;
          image->colormap[i].blue=(unsigned char) i;
        }
        /*
          Convert raster image to PseudoClass runlength-encoded packets.
        */
        for (i=0; i < (image->columns*image->rows); i++)
        {
          index=(unsigned short) (*p++);
          q->red=image->colormap[index].red;
          q->green=image->colormap[index].green;
          q->blue=image->colormap[index].blue;
          q->index=index;
          q->length=0;
          q++;
        }
      }
    (void) free((char *) rle_pixels);
    /*
      Proceed to next image.
    */
    (void) fgetc(image->file);
    status=ReadData((char *) magick,1,2,image->file);
    if ((status == True) && (strncmp(magick,"\122\314",2) == 0))
      {
        /*
          Allocate next image structure.
        */
        image->next=AllocateImage("RLE");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",alien_info->filename,
          image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
  } while ((status == True) && (strncmp(magick,"\122\314",2) == 0));
  while (image->last != (Image *) NULL)
    image=image->last;
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d S U N I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadSUNImage reads an image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadSUNImage routine is:
%
%      image=ReadSUNImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadSUNImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadSUNImage(alien_info)
AlienInfo
  *alien_info;
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_ENCODED  2
#define RT_FORMAT_RGB  3

  typedef struct _Rasterfile
  {
    int
      magic,
      width,
      height,
      depth,
      length,
      type,
      maptype,
      maplength;
  } Rasterfile;

  Image
    *image;

  Rasterfile
    sun_header;

  register int
    bit,
    i,
    x,
    y;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *sun_data,
    *sun_pixels;

  unsigned long
    lsb_first;

  unsigned short
    index;

  unsigned int
    status;

  /*
    Allocate image structure.
  */
  image=AllocateImage("SUN");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Read raster image.
  */
  status=ReadData((char *) &sun_header,1,sizeof(Rasterfile),image->file);
  if (status == False)
    {
      Warning("not a SUN image file",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    /*
      Ensure the header byte-order is most-significant byte first.
    */
    lsb_first=1;
    if (*(char *) &lsb_first)
      MSBFirstOrderLong((char *) &sun_header,sizeof(sun_header));
    if (sun_header.magic != 0x59a66a95)
      {
        Warning("not a SUN raster,",image->filename);
        DestroyImages(image);
        return((Image *) NULL);
      }
    switch (sun_header.maptype)
    {
      case RMT_NONE:
      {
        if (sun_header.depth < 24)
          {
            /*
              Create linear color ramp.
            */
            image->colors=1 << sun_header.depth;
            image->colormap=(ColorPacket *)
              malloc(image->colors*sizeof(ColorPacket));
            if (image->colormap == (ColorPacket *) NULL)
              {
                Warning("memory allocation error",(char *) NULL);
                return((Image *) NULL);
              }
            for (i=0; i < image->colors; i++)
            {
              image->colormap[i].red=(255*i)/(image->colors-1);
              image->colormap[i].green=(255*i)/(image->colors-1);
              image->colormap[i].blue=(255*i)/(image->colors-1);
            }
          }
        break;
      }
      case RMT_EQUAL_RGB:
      {
        unsigned char
          *sun_colormap;

        /*
          Read Sun raster colormap.
        */
        image->colors=sun_header.maplength/3;
        image->colormap=(ColorPacket *)
          malloc(image->colors*sizeof(ColorPacket));
        sun_colormap=(unsigned char *)
          malloc(image->colors*sizeof(unsigned char));
        if ((image->colormap == (ColorPacket *) NULL) ||
            (sun_colormap == (unsigned char *) NULL))
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImages(image);
            return((Image *) NULL);
          }
        (void) ReadData((char *) sun_colormap,1,(int) image->colors,
          image->file);
        for (i=0; i < image->colors; i++)
          image->colormap[i].red=sun_colormap[i];
        (void) ReadData((char *) sun_colormap,1,(int) image->colors,
          image->file);
        for (i=0; i < image->colors; i++)
          image->colormap[i].green=sun_colormap[i];
        (void) ReadData((char *) sun_colormap,1,(int) image->colors,
          image->file);
        for (i=0; i < image->colors; i++)
          image->colormap[i].blue=sun_colormap[i];
        (void) free((char *) sun_colormap);
        break;
      }
      case RMT_RAW:
      {
        unsigned char
          *sun_colormap;

        /*
          Read Sun raster colormap.
        */
        sun_colormap=(unsigned char *)
          malloc(sun_header.maplength*sizeof(unsigned char));
        if (sun_colormap == (unsigned char *) NULL)
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImages(image);
            return((Image *) NULL);
          }
        (void) ReadData((char *) sun_colormap,1,sun_header.maplength,
          image->file);
        (void) free((char *) sun_colormap);
        break;
      }
      default:
      {
        Warning("colormap type is not supported",image->filename);
        DestroyImages(image);
        return((Image *) NULL);
      }
    }
    sun_data=(unsigned char *) malloc(sun_header.length*sizeof(unsigned char));
    if (sun_data == (unsigned char *) NULL)
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    (void) ReadData((char *) sun_data,1,sun_header.length,image->file);
    sun_pixels=sun_data;
    if (sun_header.type == RT_ENCODED)
      {
        register int
          count,
          number_pixels;

        register unsigned char
          byte,
          *q;

        /*
          Read run-length encoded raster pixels.
        */
        number_pixels=(sun_header.width+(sun_header.width % 2))*
          sun_header.height*(((sun_header.depth-1) >> 3)+1);
        sun_pixels=(unsigned char *)
          malloc(number_pixels*sizeof(unsigned char));
        if (sun_pixels == (unsigned char *) NULL)
          {
            Warning("memory allocation error",(char *) NULL);
            DestroyImages(image);
            return((Image *) NULL);
          }
        p=sun_data;
        q=sun_pixels;
        while ((q-sun_pixels) <= number_pixels)
        {
          byte=(*p++);
          if (byte != 128)
            *q++=byte;
          else
            {
              /*
                Runlength-encoded packet: <count><byte>
              */
              count=(*p++);
              if (count > 0)
                byte=(*p++);
              while (count >= 0)
              {
                *q++=byte;
                count--;
              }
           }
        }
      (void) free((char *) sun_data);
    }
    /*
      Create image.
    */
    image->alpha=(sun_header.depth == 32);
    image->class=(sun_header.depth < 24 ? PseudoClass : DirectClass);
    image->columns=sun_header.width;
    image->rows=sun_header.height;
    image->packets=image->columns*image->rows;
    image->pixels=(RunlengthPacket *)
      malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
    image->comments=(char *)
      malloc((strlen(image->filename)+2048)*sizeof(char));
    (void) sprintf(image->comments,
      "\n  Imported from Sun raster image:  %s\n\0",image->filename);
    if ((image->pixels == (RunlengthPacket *) NULL) ||
        (image->comments == (char *) NULL))
      {
        Warning("memory allocation error",(char *) NULL);
        DestroyImages(image);
        return((Image *) NULL);
      }
    /*
      Convert Sun raster image to runlength-encoded packets.
    */
    p=sun_pixels;
    q=image->pixels;
    if (sun_header.depth == 1)
      for (y=0; y < image->rows; y++)
      {
        /*
          Convert bitmap scanline to runlength-encoded color packets.
        */
        for (x=0; x < (image->columns >> 3); x++)
        {
          for (bit=7; bit >= 0; bit--)
          {
            index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->length=0;
            q++;
          }
          p++;
        }
        if ((image->columns % 8) != 0)
          {
            for (bit=7; bit >= (8-(image->columns % 8)); bit--)
            {
              index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=0;
              q++;
            }
            p++;
          }
        if ((((image->columns/8)+(image->columns % 8 ? 1 : 0)) % 2) != 0)
          p++;
      }
    else
      if (image->class == PseudoClass)
        for (y=0; y < image->rows; y++)
        {
          /*
            Convert PseudoColor scanline to runlength-encoded color packets.
          */
          for (x=0; x < image->columns; x++)
          {
            index=(*p++);
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->length=0;
            q++;
          }
          if ((image->columns % 2) != 0)
            p++;
        }
      else
        for (y=0; y < image->rows; y++)
        {
          /*
            Convert DirectColor scanline to runlength-encoded color packets.
          */
          for (x=0; x < image->columns; x++)
          {
            q->index=(unsigned short) (image->alpha ? (*p++) : 0);
            if (sun_header.type == RT_STANDARD)
              {
                q->blue=(*p++);
                q->green=(*p++);
                q->red=(*p++);
              }
            else
              {
                q->red=(*p++);
                q->green=(*p++);
                q->blue=(*p++);
              }
            if (image->colors > 0)
              {
                q->red=image->colormap[q->red].red;
                q->green=image->colormap[q->green].green;
                q->blue=image->colormap[q->blue].blue;
              }
            q->length=0;
            q++;
          }
          if (((image->columns % 2) != 0) && (image->alpha == False))
            p++;
        }
    (void) free((char *) sun_pixels);
    if (image->class == PseudoClass)
      CompressColormap(image);
    /*
      Proceed to next image.
    */
    status=ReadData((char *) &sun_header,1,sizeof(Rasterfile),image->file);
    if (status == True)
      {
        /*
          Allocate image structure.
        */
        image->next=AllocateImage("SUN");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",alien_info->filename,
          image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
  } while (status == True);
  while (image->last != (Image *) NULL)
    image=image->last;
  CloseImage(image);
  return(image);
}

#ifdef AlienTIFF
#include "tiff.h"
#include "tiffio.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d T I F F I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadTIFFImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadTIFFImage routine is:
%
%      image=ReadTIFFImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadTIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadTIFFImage(alien_info)
AlienInfo
  *alien_info;
{
  Image
    *image;

  int
    range;

  register int
    i,
    quantum,
    x,
    y;

  register RunlengthPacket
    *q;

  TIFF
    *tiff;

  unsigned int
    status;

  unsigned long
    height,
    width;

  unsigned short
    bits_per_sample,
    max_sample_value,
    min_sample_value,
    photometric,
    samples_per_pixel;

  /*
    Allocate image structure.
  */
  image=AllocateImage("TIFF");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open TIFF image tiff.
  */
  (void) strcpy(image->filename,alien_info->filename);
  tiff=TIFFOpen(image->filename,"r");
  if (tiff == (TIFF *) NULL)
    {
      Warning("unable to open tiff image",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    if (alien_info->verbose)
      TIFFPrintDirectory(tiff,stderr,False);
    /*
      Allocate memory for the image and pixel buffer.
    */
    TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&width);
    TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&height);
    for (quantum=1; quantum <= 16; quantum<<=1)
    {
      image->columns=width/quantum;
      image->rows=height/quantum;
      image->packets=image->columns*image->rows;
      image->pixels=(RunlengthPacket *)
        malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
      if ((image->pixels != (RunlengthPacket *) NULL))
        break;
    }
    image->comments=(char *)
      malloc((strlen(image->filename)+2048)*sizeof(char));
    if ((image->pixels == (RunlengthPacket *) NULL) ||
        (image->comments == (char *) NULL))
      {
        Warning("unable to allocate memory",(char *) NULL);
        DestroyImages(image);
        TIFFClose(tiff);
        return((Image *) NULL);
      }
    (void) sprintf(image->comments,"\n  Imported from TIFF image %s.\n\0",
      image->filename);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_MINSAMPLEVALUE,&min_sample_value);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_MAXSAMPLEVALUE,&max_sample_value);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric);
    TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,&samples_per_pixel);
    range=max_sample_value-min_sample_value;
    if ((bits_per_sample > 8) || (samples_per_pixel > 1) || TIFFIsTiled(tiff))
      {
        register unsigned long
          *p,
          *pixels;

        /*
          Convert TIFF image to DirectClass MIFF image.
        */
        image->alpha=samples_per_pixel > 3;
        pixels=(unsigned long *)
          malloc(image->columns*image->rows*sizeof(unsigned long));
        if (pixels == (unsigned long *) NULL)
          {
            Warning("unable to allocate memory",(char *) NULL);
            DestroyImages(image);
            TIFFClose(tiff);
            return((Image *) NULL);
          }
        if (quantum > 1)
          Warning("not enough memory","cropping required");
        status=TIFFReadRGBAImage(tiff,image->columns,image->rows,pixels,0);
        if (status == False)
          {
            Warning("unable to read TIFF image",(char *) NULL);
            (void) free((char *) pixels);
            DestroyImages(image);
            TIFFClose(tiff);
            return((Image *) NULL);
          }
        /*
          Convert image to DirectClass runlength-encoded packets.
        */
        q=image->pixels;
        for (y=image->rows-1; y >= 0; y--)
        {
          p=pixels+y*image->columns;
          for (x=0; x < image->columns; x++)
          {
            q->red=TIFFGetR(*p);
            q->green=TIFFGetG(*p);
            q->blue=TIFFGetB(*p);
            q->index=(unsigned short) (image->alpha ? TIFFGetA(*p) : 0);
            q->length=0;
            p++;
            q++;
          }
        }
        (void) free((char *) pixels);
        if (samples_per_pixel == 1)
          QuantizeImage(image,(unsigned int) range,8,False,RGBColorspace,True);
      }
    else
      {
        unsigned char
          *p,
          *scanline;

        /*
          Convert TIFF image to PseudoClass MIFF image.
        */
        image->class=PseudoClass;
        image->colors=range+1;
        image->colormap=(ColorPacket *)
          malloc(image->colors*sizeof(ColorPacket));
        scanline=(unsigned char *) malloc(TIFFScanlineSize(tiff));
        if ((image->colormap == (ColorPacket *) NULL) ||
            (scanline == (unsigned char *) NULL))
          {
            Warning("unable to allocate memory",(char *) NULL);
            DestroyImages(image);
            TIFFClose(tiff);
            return((Image *) NULL);
          }
        /*
          Create colormap.
        */
        switch (photometric)
        {
          case PHOTOMETRIC_MINISBLACK:
          {
            for (i=0; i < image->colors; i++)
            {
              image->colormap[i].red=(MaxRGB*i)/range;
              image->colormap[i].green=(MaxRGB*i)/range;
              image->colormap[i].blue=(MaxRGB*i)/range;
            }
            break;
          }
          case PHOTOMETRIC_MINISWHITE:
          {
            for (i=0; i < image->colors; i++)
            {
              image->colormap[i].red=((range-i)*MaxRGB)/range;
              image->colormap[i].green=((range-i)*MaxRGB)/range;
              image->colormap[i].blue=((range-i)*MaxRGB)/range;
            }
            break;
          }
          case PHOTOMETRIC_PALETTE:
          {
            unsigned short
              *blue_colormap,
              *green_colormap,
              *red_colormap;

            TIFFGetField(tiff,TIFFTAG_COLORMAP,&red_colormap,&green_colormap,
              &blue_colormap);
            for (i=0; i < image->colors; i++)
            {
              image->colormap[i].red=((int) red_colormap[i]*MaxRGB)/65535;
              image->colormap[i].green=((int) green_colormap[i]*MaxRGB)/65535;
              image->colormap[i].blue=((int) blue_colormap[i]*MaxRGB)/65535;
            }
            break;
          }
          default:
            break;
        }
        /*
          Convert image to PseudoClass runlength-encoded packets.
        */
        if (quantum > 1)
          Warning("not enough memory","subsampling required");
        q=image->pixels;
        for (y=0; y < image->rows; y++)
        {
          for (i=0; i < quantum; i++)
            TIFFReadScanline(tiff,scanline,y*quantum+i,0);
          p=scanline;
          switch (photometric)
          {
            case PHOTOMETRIC_MINISBLACK:
            case PHOTOMETRIC_MINISWHITE:
            {
              switch (bits_per_sample)
              {
                case 1:
                {
                  register int
                    bit;

                  for (x=0; x < (image->columns-7); x+=8)
                  {
                    for (bit=7; bit >= 0; bit--)
                    {
                      q->index=((*p) & (0x01 << bit) ? 0x01 : 0x00);
                      q->red=image->colormap[q->index].red;
                      q->green=image->colormap[q->index].green;
                      q->blue=image->colormap[q->index].blue;
                      q->length=0;
                      q++;
                    }
                    p+=quantum;
                  }
                  if ((image->columns % 8) != 0)
                    {
                      for (bit=7; bit >= (8-(image->columns % 8)); bit--)
                      {
                        q->index=((*p) & (0x01 << bit) ? 0x00 : 0x01);
                        q->red=image->colormap[q->index].red;
                        q->green=image->colormap[q->index].green;
                        q->blue=image->colormap[q->index].blue;
                        q->length=0;
                        q++;
                      }
                      p+=quantum;
                    }
                  break;
                }
                case 2:
                {
                  for (x=0; x < image->columns; x+=4)
                  {
                    q->index=(*p >> 6) & 0x3;
                    q->red=image->colormap[q->index].red;
                    q->green=image->colormap[q->index].green;
                    q->blue=image->colormap[q->index].blue;
                    q->length=0;
                    q++;
                    q->index=(*p >> 4) & 0x3;
                    q->red=image->colormap[q->index].red;
                    q->green=image->colormap[q->index].green;
                    q->blue=image->colormap[q->index].blue;
                    q->length=0;
                    q++;
                    q->index=(*p >> 2) & 0x3;
                    q->red=image->colormap[q->index].red;
                    q->green=image->colormap[q->index].green;
                    q->blue=image->colormap[q->index].blue;
                    q->length=0;
                    q++;
                    q->index=(*p) & 0x3;
                    q->red=image->colormap[q->index].red;
                    q->green=image->colormap[q->index].green;
                    q->blue=image->colormap[q->index].blue;
                    q->length=0;
                    q++;
                    p+=quantum;
                  }
                  break;
                }
                case 4:
                {
                  for (x=0; x < image->columns; x+=2)
                  {
                    q->index=(*p >> 4) & 0xf;
                    q->red=image->colormap[q->index].red;
                    q->green=image->colormap[q->index].green;
                    q->blue=image->colormap[q->index].blue;
                    q->length=0;
                    q++;
                    q->index=(*p) & 0xf;
                    q->red=image->colormap[q->index].red;
                    q->green=image->colormap[q->index].green;
                    q->blue=image->colormap[q->index].blue;
                    q->length=0;
                    q++;
                    p+=quantum;
                  }
                  break;
                }
                case 8:
                {
                  for (x=0; x < image->columns; x++)
                  {
                    q->index=(*p);
                    q->red=image->colormap[q->index].red;
                    q->green=image->colormap[q->index].green;
                    q->blue=image->colormap[q->index].blue;
                    q->length=0;
                    q++;
                    p+=quantum;
                  }
                  break;
                }
                default:
                  break;
              }
              break;
            }
            case PHOTOMETRIC_PALETTE:
            {
              for (x=0; x < image->columns; x++)
              {
                q->index=(*p);
                q->red=image->colormap[q->index].red;
                q->green=image->colormap[q->index].green;
                q->blue=image->colormap[q->index].blue;
                q->length=0;
                q++;
                p+=quantum;
              }
              break;
            }
            default:
              break;
          }
        }
        (void) free((char *) scanline);
        CompressColormap(image);
      }
    /*
      Proceed to next image.
    */
    status=TIFFReadDirectory(tiff);
    if (status == True)
      {
        /*
          Allocate image structure.
        */
        image->next=AllocateImage("TIFF");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",alien_info->filename,
          image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
  } while (status == True);
  TIFFClose(tiff);
  while (image->last != (Image *) NULL)
    image=image->last;
  return(image);
}
#else
static Image *ReadTIFFImage(alien_info)
AlienInfo
  *alien_info;
{
  Warning("TIFF library is not available",alien_info->filename);
  return(ReadImage(alien_info->filename));
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d Y U V I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadYUVImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.  U and V, normally -0.5 through 0.5, are expected to be
%  normalized to the range 0 through 255 fit withing a byte.
%
%  The format of the ReadYUVImage routine is:
%
%      image=ReadYUVImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadYUVImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadYUVImage(alien_info)
AlienInfo
  *alien_info;
{
  Image
    *image;

  int
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *rgb_pixels;

  unsigned int
    height,
    width;

  /*
    Allocate image structure.
  */
  image=AllocateImage("YUV");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Create image.
  */
  width=512;
  height=512;
  if (alien_info->geometry != (char *) NULL)
    (void) XParseGeometry(alien_info->geometry,&x,&y,&width,&height);
  image->columns=width;
  image->rows=height;
  image->packets=image->columns*image->rows;
  rgb_pixels=(unsigned char *)
    malloc((unsigned int) image->packets*3*sizeof(unsigned char));
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if ((rgb_pixels == (unsigned char *) NULL) ||
      (image->pixels == (RunlengthPacket *) NULL))
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Convert raster image to runlength-encoded packets.
  */
  (void) ReadData((char *) rgb_pixels,3,(int) (image->columns*image->rows),
    image->file);
  p=rgb_pixels;
  q=image->pixels;
  for (i=0; i < (image->columns*image->rows); i++)
  {
    q->red=(*p++);
    q->green=(*p++);
    q->blue=(*p++);
    q->index=0;
    q->length=0;
    q++;
  }
  (void) free((char *) rgb_pixels);
  TransformRGBImage(image,YUVColorspace);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d X B M I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadXBMImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadXBMImage routine is:
%
%      image=ReadXBMImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadXBMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadXBMImage(alien_info)
AlienInfo
  *alien_info;
{
  char
    data[2048];

  Image
    *image;

  register int
    x,
    y;

  register RunlengthPacket
    *p;

  register unsigned char
    bit;

  unsigned int
    byte;

  /*
    Allocate image structure.
  */
  image=AllocateImage("XBM");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Read X bitmap header.
  */
  while (fgets(data,sizeof(data)-1,image->file) != (char *) NULL)
    if (sscanf(data,"#define %*32s %u",&image->columns) == 1)
      break;
  while (fgets(data,sizeof(data)-1,image->file) != (char *) NULL)
    if (sscanf(data,"#define %*32s %u",&image->rows) == 1)
      break;
  if ((image->columns == 0) || (image->rows == 0))
    {
      Warning("XBM file is not in the correct format",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  while (fgets(data,sizeof(data)-1,image->file) != (char *) NULL)
    if (sscanf(data,"%*[^#] char"))
      break;
  if (feof(image->file))
    {
      Warning("XBM file is not in the correct format",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Create image.
  */
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Create colormap.
  */
  image->class=PseudoClass;
  image->colors=2;
  image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  image->colormap[0].red=0;
  image->colormap[0].green=0;
  image->colormap[0].blue=0;
  image->colormap[1].red=255;
  image->colormap[1].green=255;
  image->colormap[1].blue=255;
  /*
    Initial image comment.
  */
  image->comments=(char *) malloc((strlen(image->filename)+2048)*sizeof(char));
  if (image->comments == (char *) NULL)
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  (void) sprintf(image->comments,"\n  Imported from X11 bitmap file:  %s\n\0",
    image->filename);
  /*
    Convert X bitmap image to runlength-encoded packets.
  */
  p=image->pixels;
  for (y=0; y < image->rows; y++)
  {
    bit=0;
    for (x=0; x < image->columns; x++)
    {
      if (bit == 0)
        (void) fscanf(image->file,"%i,",&byte);
      p->index=(byte & 0x01) ? 0 : 1;
      byte>>=1;
      p->red=image->colormap[p->index].red;
      p->green=image->colormap[p->index].green;
      p->blue=image->colormap[p->index].blue;
      p->length=0;
      p++;
      bit++;
      if (bit == 8)
        bit=0;
    }
  }
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d V I C A R I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadVICARImage reads an image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.
%
%  The format of the ReadVICARImage routine is:
%
%      image=ReadVICARImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image: Function ReadVICARImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or if
%      the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%
*/
static Image *ReadVICARImage(alien_info)
AlienInfo
  *alien_info;
{
#define MaxKeywordLength  2048

  char
    keyword[MaxKeywordLength],
    value[MaxKeywordLength];

  Image
    *image;

  long int
    count;

  register int
    c,
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned char
    *vicar_pixels;

  unsigned int
    header_length,
    status,
    value_expected;

  /*
    Allocate image structure.
  */
  image=AllocateImage("VICAR");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Decode image header.
  */
  c=fgetc(image->file);
  count=1;
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  while (isgraph(c) && ((image->columns*image->rows) == 0))
  {
    register char
      *p;

    if (!isalnum(c))
      {
        c=fgetc(image->file);
        count++;
      }
    else
      {
        /*
          Determine a keyword and its value.
        */
        p=keyword;
        do
        {
          if ((p-keyword) < (MaxKeywordLength-1))
            *p++=(char) c;
          c=fgetc(image->file);
          count++;
        } while (isalnum(c) || (c == '_'));
        *p='\0';
        value_expected=False;
        while (isspace(c) || (c == '='))
        {
          if (c == '=')
            value_expected=True;
          c=fgetc(image->file);
          count++;
        }
        if (value_expected == False)
          continue;
        p=value;
        while (isalnum(c))
        {
          if ((p-value) < (MaxKeywordLength-1))
            *p++=(char) c;
          c=fgetc(image->file);
          count++;
        }
        *p='\0';
        /*
          Assign a value to the specified keyword.
        */
        if (strcmp(keyword,"LABEL_RECORDS") == 0)
          header_length=(unsigned int) atoi(value);
        if (strcmp(keyword,"LBLSIZE") == 0)
          header_length=(unsigned int) atoi(value);
        if (strcmp(keyword,"RECORD_BYTES") == 0)
          image->columns=(unsigned int) atoi(value);
        if (strcmp(keyword,"NS") == 0)
          image->columns=(unsigned int) atoi(value);
        if (strcmp(keyword,"LINES") == 0)
          image->rows=(unsigned int) atoi(value);
        if (strcmp(keyword,"NL") == 0)
          image->rows=(unsigned int) atoi(value);
      }
    while (isspace(c))
    {
      c=fgetc(image->file);
      count++;
    }
  }
  /*
    Read the rest of the header.
  */
  while (count < header_length)
  {
    c=fgetc(image->file);
    count++;
  }
  /*
    Verify that required image information is defined.
  */
  if ((image->columns*image->rows) == 0)
    {
      Warning("incorrect image header in file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Create linear colormap.
  */
  image->class=PseudoClass;
  image->colors=256;
  image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  for (i=0; i < image->colors; i++)
  {
    image->colormap[i].red=(unsigned char) i;
    image->colormap[i].green=(unsigned char) i;
    image->colormap[i].blue=(unsigned char) i;
  }
  /*
    Create image.
  */
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  image->comments=(char *)
    malloc((strlen(image->filename)+2048)*sizeof(char));
  vicar_pixels=(unsigned char *)
    malloc((unsigned int) image->packets*sizeof(unsigned char));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (image->comments == (char *) NULL) ||
      (vicar_pixels == (unsigned char *) NULL))
    {
      Warning("memory allocation error",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  (void) sprintf(image->comments,"\n  Imported from VICAR image:  %s\n",
    image->filename);
  /*
    Convert vicar pixels to runlength-encoded packets.
  */
  status=ReadData((char *) vicar_pixels,1,(int) image->packets,image->file);
  if (status == False)
    {
      Warning("insufficient image data in file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Convert vicar pixels to runlength-encoded packets.
  */
  p=vicar_pixels;
  q=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    q->red=(*p);
    q->green=(*p);
    q->blue=(*p);
    q->index=(unsigned short) *p;
    q->length=0;
    p++;
    q++;
  }
  (void) free((char *) vicar_pixels);
  CompressColormap(image);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d X W D I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadXWDImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadXWDImage routine is:
%
%      image=ReadXWDImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadXWDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
static Image *ReadXWDImage(alien_info)
AlienInfo
  *alien_info;
{
  char
    *window_name;

  Display
    display;

  Image
    *image;

  int
    status,
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned long
    pixel;

  unsigned long int
    lsb_first,
    packets;

  ScreenFormat
    screen_format;

  XColor
    *colors;

  XImage
    *ximage;

  XWDFileHeader
    header;

  /*
    Allocate image structure.
  */
  image=AllocateImage("XWD");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,alien_info->filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
     Read in header information.
  */
  status=ReadData((char *) &header,sizeof(header),1,image->file);
  if (status == False)
    {
      Warning("Unable to read dump file header",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Ensure the header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBFirstOrderLong((char *) &header,sizeof(header));
  /*
    Check to see if the dump file is in the proper format.
  */
  if (header.file_version != XWD_FILE_VERSION)
    {
      Warning("XWD file format version mismatch",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  if (header.header_size < sizeof(header))
    {
      Warning("XWD header size is too small",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  packets=(header.header_size-sizeof(header));
  window_name=(char *) malloc((unsigned int) packets*sizeof(char));
  if (window_name == (char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  status=ReadData((char *) window_name,1,(int) packets,image->file);
  if (status == False)
    {
      Warning("unable to read window name from dump file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Initialize the X image.
  */
  display.byte_order=header.byte_order;
  display.bitmap_unit=header.bitmap_unit;
  display.bitmap_bit_order=header.bitmap_bit_order;
  display.pixmap_format=(&screen_format);
  display.nformats=1;
  screen_format.depth=header.pixmap_depth;
  screen_format.bits_per_pixel=(int) header.bits_per_pixel;
  ximage=XCreateImage(&display,(Visual *) NULL,
    (unsigned int) header.pixmap_depth,(int) header.pixmap_format,
    (int) header.xoffset,(char *) NULL,(unsigned int) header.pixmap_width,
    (unsigned int) header.pixmap_height,(int) header.bitmap_pad,
    (int) header.bytes_per_line);
  ximage->red_mask=header.red_mask;
  ximage->green_mask=header.green_mask;
  ximage->blue_mask=header.blue_mask;
  /*
    Read colormap.
  */
  if (header.ncolors != 0)
    {
      colors=(XColor *) malloc((unsigned int) header.ncolors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        {
          Warning("unable to allocate memory",(char *) NULL);
          DestroyImage(image);
          return((Image *) NULL);
        }
      status=ReadData((char *) colors,sizeof(XColor),(int) header.ncolors,
        image->file);
      if (status == False)
        {
          Warning("unable to read color map from dump file",image->filename);
          DestroyImage(image);
          return((Image *) NULL);
        }
      /*
        Ensure the header byte-order is most-significant byte first.
      */
      lsb_first=1;
      if (*(char *) &lsb_first)
        for (i=0; i < header.ncolors; i++)
        {
          MSBFirstOrderLong((char *) &colors[i].pixel,sizeof(unsigned long));
          MSBFirstOrderShort((char *) &colors[i].red,3*sizeof(unsigned short));
        }
    }
  /*
    Allocate the pixel buffer.
  */
  if (ximage->format == ZPixmap)
    packets=ximage->bytes_per_line*ximage->height;
  else
    packets=ximage->bytes_per_line*ximage->height*ximage->depth;
  ximage->data=(char *) malloc(packets*sizeof(unsigned char));
  if (ximage->data == (char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  status=ReadData(ximage->data,1,(int) packets,image->file);
  if (status == False)
    {
      Warning("unable to read dump pixmap",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Convert image to MIFF format.
  */
  image->columns=ximage->width;
  image->rows=ximage->height;
  /*
    Initial image comment.
  */
  if ((ximage->red_mask > 0) || (ximage->green_mask > 0) ||
      (ximage->blue_mask > 0))
    image->class=DirectClass;
  else
    image->class=PseudoClass;
  image->colors=header.ncolors;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  image->comments=(char *)
    malloc((strlen(image->filename)+2048)*sizeof(char));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (image->comments == (char *) NULL))
    {
      Warning("unable to allocate memory",(char *) NULL);
      DestroyImage(image);
      return((Image *) NULL);
    }
  (void) sprintf(image->comments,"\n  Imported from X11 dump file:  %s\n\0",
    image->filename);
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    {
      register unsigned long
        color;

      unsigned long
        blue_mask,
        blue_shift,
        green_mask,
        green_shift,
        red_mask,
        red_shift;

      /*
        Determine shift and mask for red, green, and blue.
      */
      red_mask=ximage->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=ximage->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=ximage->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if (image->colors > 0)
        for (y=0; y < image->rows; y++)
        {
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            p->index=(unsigned short) ((pixel >> red_shift) & red_mask);
            p->red=(unsigned char) (colors[p->index].red >> 8);
            p->index=(unsigned short) ((pixel >> green_shift) & green_mask);
            p->green=(unsigned char) (colors[p->index].green >> 8);
            p->index=(unsigned short) ((pixel >> blue_shift) & blue_mask);
            p->blue=(unsigned char) (colors[p->index].blue >> 8);
            p->index=0;
            p->length=0;
            p++;
          }
        }
      else
        for (y=0; y < image->rows; y++)
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            color=(pixel >> red_shift) & red_mask;
            p->red=(unsigned char)
              ((((unsigned long) color*65535)/red_mask) >> 8);
            color=(pixel >> green_shift) & green_mask;
            p->green=(unsigned char)
              ((((unsigned long) color*65535)/green_mask) >> 8);
            color=(pixel >> blue_shift) & blue_mask;
            p->blue=(unsigned char)
              ((((unsigned long) color*65535)/blue_mask) >> 8);
            p->index=0;
            p->length=0;
            p++;
          }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert X image to PseudoClass packets.
      */
      image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        {
          Warning("unable to allocate memory",(char *) NULL);
          DestroyImage(image);
          return((Image *) NULL);
        }
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=colors[i].red >> 8;
        image->colormap[i].green=colors[i].green >> 8;
        image->colormap[i].blue=colors[i].blue >> 8;
      }
      for (y=0; y < image->rows; y++)
        for (x=0; x < image->columns; x++)
        {
          pixel=XGetPixel(ximage,x,y);
          p->red=(unsigned char) (colors[pixel].red >> 8);
          p->green=(unsigned char) (colors[pixel].green >> 8);
          p->blue=(unsigned char) (colors[pixel].blue >> 8);
          p->index=(unsigned short) pixel;
          p->length=0;
          p++;
        }
      CompressColormap(image);
      break;
    }
  }
  /*
    Free image and colormap.
  */
  (void) free((char *) window_name);
  if (header.ncolors != 0)
    (void) free((char *) colors);
  XDestroyImage(ximage);
  CloseImage(image);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e A V S I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteAVSImage writes an image to a file on disk in AVS rasterfile
%  format.
%
%  The format of the WriteAVSImage routine is:
%
%      status=WriteAVSImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteAVSImage(image)
Image
  *image;
{
  typedef struct Raster
  {
    int
      width,
      height;
  } Raster;

  Raster
    header;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *avs_pixels;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Initialize raster file header.
  */
  header.width=image->columns;
  header.height=image->rows;
  avs_pixels=(unsigned char *)
    malloc(4*image->columns*image->rows*sizeof(unsigned char));
  if (avs_pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  (void) fwrite((char *) &header,sizeof(header),1,image->file);
  p=image->pixels;
  q=avs_pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= (int) p->length; j++)
    {
      *q++=(unsigned char) (image->alpha ? p->index : 0);
      *q++=p->red;
      *q++=p->green;
      *q++=p->blue;
    }
    p++;
  }
  (void) fwrite((char *) avs_pixels,sizeof(char),(int)
    (image->columns*image->rows*4),image->file);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C M Y K I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteCMYKImage writes an image to a file on disk in red, green,
%  blue rasterfile format.
%
%  The format of the WriteCMYKImage routine is:
%
%      status=WriteCMYKImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteCMYKImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    black,
    cyan,
    magenta,
    *pixels,
    yellow;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Convert MIFF to CMYK raster pixels.
  */
  pixels=(unsigned char *)
    malloc(4*image->columns*image->rows*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  p=image->pixels;
  q=pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      cyan=MaxRGB-p->red;
      magenta=MaxRGB-p->green;
      yellow=MaxRGB-p->blue;
      black=cyan;
      if (magenta < black)
        black=magenta;
      if (yellow < black)
        black=yellow;
      *q++=cyan-black;
      *q++=magenta-black;
      *q++=yellow-black;
      *q++=black;
    }
    p++;
  }
  (void) fwrite((char *) pixels,sizeof(unsigned char),
    (int) (4*image->columns*image->rows),image->file);
  (void) free((char *) pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e F A X I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteFAXImage writes an image to a file on disk in 1 dimensional
%  Huffman encoded format.
%
%  The format of the WriteFAXImage routine is:
%
%      status=WriteFAXImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteFAXImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteFAXImage(image)
Image
  *image;
{
  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Convert MIFF to monochrome.
  */
  QuantizeImage(image,2,8,False,GRAYColorspace,True);
  (void) HuffmanEncodeImage(image);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G I F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteGIFImage writes an image to a file on disk in the GIF image
%  format.
%
%  The format of the WriteGIFImage routine is:
%
%      status=WriteGIFImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteGIFImage(image)
Image
  *image;
{
  int
    status;

  register int
    i;

  unsigned char
    bits_per_pixel,
    buffer[10];

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    GIF colormap must be 256 entries or less.
  */
  if ((image->class == DirectClass) || (image->colors > 256))
    QuantizeImage(image,256,8,False,RGBColorspace,True);
  for (bits_per_pixel=1; bits_per_pixel < 8; bits_per_pixel++)
    if ((1 << bits_per_pixel) >= image->colors)
      break;
  /*
    Write GIF header.
  */
  (void) fwrite("GIF87a",1,6,image->file);
  buffer[0]=(unsigned char) (image->columns & 0xff);
  buffer[1]=(unsigned char) ((image->columns >> 8) & 0xff);
  buffer[2]=(unsigned char) (image->rows & 0xff);
  buffer[3]=(unsigned char) ((image->rows >> 8) & 0xff);
  buffer[4]=0x80;  /* global colormap */
  buffer[4]|=(bits_per_pixel-1) << 4;  /* color resolution */
  buffer[4]|=(bits_per_pixel-1);   /* size of global colormap */
  buffer[5]=0;  /* background color */
  buffer[6]=0;  /* aspect ratio */
  (void) fwrite((char *) buffer,1,7,image->file);
  /*
    Write colormap.
  */
  for (i=0; i < image->colors; i++)
  {
    (void) fwrite((char *) &image->colormap[i].red,1,1,image->file);
    (void) fwrite((char *) &image->colormap[i].green,1,1,image->file);
    (void) fwrite((char *) &image->colormap[i].blue,1,1,image->file);
  }
  buffer[0]=0;
  buffer[1]=0;
  buffer[2]=0;
  for ( ; i < (int) (1 << bits_per_pixel); i++)
    (void) fwrite((char *) buffer,1,3,image->file);
  (void) fwrite(",",1,1,image->file);  /* separator */
  /*
    Write the image header.
  */
  buffer[0]=0;  /* left/top */
  buffer[1]=0;
  buffer[2]=0;
  buffer[3]=0;
  buffer[4]=(unsigned char) (image->columns & 0xff);
  buffer[5]=(unsigned char) ((image->columns >> 8) & 0xff);
  buffer[6]=(unsigned char) (image->rows & 0xff);
  buffer[7]=(unsigned char) ((image->rows >> 8) & 0xff);
  buffer[8]=0;  /* no interlace */
  buffer[9]=Max(bits_per_pixel,2);
  (void) fwrite((char *) buffer,1,10,image->file);
  status=LZWEncodeImage(image,Max(bits_per_pixel,2)+1);
  if (status == False)
    {
      Warning("unable to write image","memory allocation failed");
      DestroyImage(image);
      return(False);
    }
  buffer[0]=0;
  (void) fwrite((char *) buffer,1,1,image->file);  /* EOF */
  (void) fwrite(";",1,1,image->file);  /* terminator */
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G R A Y I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteGRAYImage writes an image to a file on disk in red, green,
%  blue rasterfile format.
%
%  The format of the WriteGRAYImage routine is:
%
%      status=WriteGRAYImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteGRAYImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Convert image to gray scale PseudoColor class.
  */
  pixels=(unsigned char *)
    malloc(image->columns*image->rows*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  p=image->pixels;
  q=pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
      *q++=Intensity(*p);
    p++;
  }
  (void) fwrite((char *) pixels,sizeof(unsigned char),
    (int) (image->columns*image->rows),image->file);
  (void) free((char *) pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e H I S T O G R A M I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteHISTOGRAMImage writes an image to a file in HISTOGRAM format.
%  A list of unique pixel values and the number of times each occurs in the
%  image is written to the file.
%
%  The format of the WriteHISTOGRAMImage routine is:
%
%      status=WriteAVSImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteHISTOGRAMImage(image)
Image
  *image;
{
  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  (void) NumberColors(image,image->file);
  CloseImage(image);
  return(True);
}

#ifdef AlienJPEG
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  W r i t e J P E G I m a g e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteJPEGImage writes an image file and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new jpeg_image.
%
%  The format of the WriteJPEGImage routine is:
%
%      status=WriteJPEGImage(image)
%
%  A description of each parameter follows:
%
%    o status:  Function WriteJPEGImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o jpeg_image:  A pointer to a Image structure.
%
%
*/

METHODDEF void JPEGInitializeImage(jpeg_info)
compress_info_ptr
  jpeg_info;
{
  /*
    Initialize JPEG image.
  */
  jpeg_info->image_width=jpeg_image->columns;
  jpeg_info->image_height=jpeg_image->rows;
  jpeg_info->data_precision=8;
  jpeg_info->input_components=3;
  jpeg_info->in_color_space=CS_RGB;
  if ((jpeg_image->class == PseudoClass) && (jpeg_image->colors <= 256))
    {
      register int
        i;

      unsigned int
        grayscale;

      /*
        Determine if jpeg_image is grayscale.
      */
      grayscale=True;
      for (i=0; i < jpeg_image->colors; i++)
        if ((jpeg_image->colormap[i].red != jpeg_image->colormap[i].green) ||
            (jpeg_image->colormap[i].green != jpeg_image->colormap[i].blue))
          {
            grayscale=False;
            break;
          }
      if (grayscale)
        {
          jpeg_info->input_components=1;
          jpeg_info->in_color_space=CS_GRAYSCALE;
        }
    }
  jpeg_image->packet=jpeg_image->pixels;
  jpeg_image->runlength=jpeg_image->packet->length+1;
}

static void JPEGInputTermMethod(jpeg_info)
compress_info_ptr
  jpeg_info;
{
}

static void JPEGWriteGRAY(jpeg_info,pixel_data)
compress_info_ptr
  jpeg_info;

JSAMPARRAY
  pixel_data;
{
  register int
    column;

  register JSAMPROW
    gray;

  register RunlengthPacket
    *p;

  /*
    Convert run-length encoded grayscale MIFF packets to JPEG pixels.
  */
  gray=pixel_data[0];
  p=jpeg_image->packet;
  for (column=jpeg_info->image_width; column > 0; column--)
  {
    if (jpeg_image->runlength > 0)
      jpeg_image->runlength--;
    else
      {
        p++;
        jpeg_image->runlength=p->length;
      }
    *gray++=(JSAMPLE) p->red;
  }
  jpeg_image->packet=p;
}

static void JPEGWriteRGB(jpeg_info,pixel_data)
compress_info_ptr
  jpeg_info;

JSAMPARRAY
  pixel_data;
{
  register int
    column;

  register JSAMPROW
    blue,
    green,
    red;

  register RunlengthPacket
    *p;

  /*
    Convert run-length encoded MIFF packets to JPEG pixels.
  */
  red=pixel_data[0];
  green=pixel_data[1];
  blue=pixel_data[2];
  p=jpeg_image->packet;
  for (column=jpeg_info->image_width; column > 0; column--)
  {
    if (jpeg_image->runlength > 0)
      jpeg_image->runlength--;
    else
      {
        p++;
        jpeg_image->runlength=p->length;
      }
    *red++=(JSAMPLE) p->red;
    *green++=(JSAMPLE) p->green;
    *blue++=(JSAMPLE) p->blue;
  }
  jpeg_image->packet=p;
}

static void JPEGSelectMethod(jpeg_info)
compress_info_ptr
  jpeg_info;
{
  jpeg_info->methods->get_input_row=JPEGWriteRGB;
  if (jpeg_info->in_color_space == CS_GRAYSCALE)
    {
      j_monochrome_default(jpeg_info);
      jpeg_info->methods->get_input_row=JPEGWriteGRAY;
    }
}

static unsigned int WriteJPEGImage(image)
Image
  *image;
{
  struct Compress_info_struct
    jpeg_info;

  struct Compress_methods_struct
    jpeg_methods;

  struct External_methods_struct
    external_methods;

  /*
    Open image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Initialize the JPEG system-dependent methods.
  */
  jpeg_image=image;
  jpeg_info.methods=(&jpeg_methods);
  jpeg_info.emethods=(&external_methods);
  jselerror(&external_methods);
  jselmemmgr(&external_methods);
  jpeg_info.methods->input_init=JPEGInitializeImage;
  jpeg_info.methods->input_term=JPEGInputTermMethod;
  jpeg_methods.c_ui_method_selection=JPEGSelectMethod;
  j_c_defaults(&jpeg_info,jpeg_image->quality,False);
  jpeg_info.optimize_coding=True;
  jpeg_info.restart_in_rows=1;
  if ((image->class == PseudoClass) && (image->colors <= 256))
    jpeg_info.smoothing_factor=25;
  /*
    Write a JFIF JPEG file.
  */
  jpeg_info.input_file=(FILE *) NULL;
  jpeg_info.output_file=image->file;
  jselwjfif(&jpeg_info);
  jpeg_compress(&jpeg_info);
  CloseImage(image);
  return(True);
}
#else
static unsigned int WriteJPEGImage(image)
Image
  *image;
{
  Warning("JPEG library is not available",image->filename);
  return(WriteImage(image));
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M T V I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteMTVImage writes an image to a file on disk in red, green,
%  blue rasterfile format.
%
%  The format of the WriteMTVImage routine is:
%
%      status=WriteMTVImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteMTVImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Convert MIFF to MTV raster pixels.
  */
  pixels=(unsigned char *)
    malloc(3*image->columns*image->rows*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  p=image->pixels;
  q=pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      *q++=p->red;
      *q++=p->green;
      *q++=p->blue;
    }
    p++;
  }
  (void) fprintf(image->file,"%u %u",image->columns,image->rows);
  (void) fwrite((char *) pixels,sizeof(unsigned char),
    (int) (3*image->columns*image->rows),image->file);
  (void) free((char *) pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WritePNMImage writes an image to a file on disk in PNM
%  rasterfile format.
%
%  The format of the WritePNMImage routine is:
%
%      status=WritePNMImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WritePNMImage(image)
Image
  *image;
{
  int
    count;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    format,
    *pixels;

  unsigned int
    packets;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Write PNM file header.
  */
  packets=image->columns*image->rows;
  if ((image->class == DirectClass) || (image->colors > 256))
    {
      format='6';
      packets*=3;
    }
  else
    {
      /*
        Determine if image is gray scale.
      */
      format='5';
      if (image->colors == 2)
        format='4';
      for (i=0; i < image->colors; i++)
        if ((image->colormap[i].red != image->colormap[i].green) ||
            (image->colormap[i].green != image->colormap[i].blue))
          {
            format='6';
            packets*=3;
            break;
          }
    }
  (void) fprintf(image->file,"P%c\n# created by ImageMagick\n%u %u\n",
    format,image->columns,image->rows);
  /*
    Convert MIFF to PNM raster pixels.
  */
  pixels=(unsigned char *) malloc(packets*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  p=image->pixels;
  q=pixels;
  switch (format)
  {
    case '4':
    {
      register unsigned char
        bit,
        byte,
        polarity;

      unsigned int
        x;

      /*
        Convert image to a PBM image.
      */
      polarity=(Intensity(image->colormap[0]) <
        Intensity(image->colormap[1]) ? 0 : 1);
      bit=0;
      byte=0;
      x=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= (int) p->length; j++)
        {
          byte<<=1;
          if (p->index == polarity)
            byte|=0x01;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == image->columns)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                *q++=byte << (8-bit);
              bit=0;
              byte=0;
              x=0;
           }
        }
        p++;
      }
      packets=q-pixels;
      break;
    }
    case '5':
    {
      /*
        Convert image to a PGM image.
      */
      (void) fprintf(image->file,"%d\n",MaxRGB);
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
          *q++=image->colormap[p->index].red;
        p++;
      }
      break;
    }
    case '6':
    {
      /*
        Convert image to a PNM image.
      */
      (void) fprintf(image->file,"%d\n",MaxRGB);
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          *q++=p->red;
          *q++=p->green;
          *q++=p->blue;
        }
        p++;
      }
      break;
    }
  }
  count=fwrite((char *) pixels,1,(int) packets,image->file);
  if (count != packets)
    {
      Warning("unable to write image","file write error");
      return(False);
    }
  (void) free((char *) pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e R G B I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteRGBImage writes an image to a file on disk in red, green,
%  blue rasterfile format.
%
%  The format of the WriteRGBImage routine is:
%
%      status=WriteRGBImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteRGBImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Convert MIFF to RGB raster pixels.
  */
  pixels=(unsigned char *)
    malloc(3*image->columns*image->rows*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  p=image->pixels;
  q=pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      *q++=p->red;
      *q++=p->green;
      *q++=p->blue;
    }
    p++;
  }
  (void) fwrite((char *) pixels,sizeof(unsigned char),
    (int) (3*image->columns*image->rows),image->file);
  (void) free((char *) pixels);
  CloseImage(image);
  return(True);
}

#ifdef AlienTIFF
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteTIFFImage writes a TIFF image to a file.
%
%  The format of the WriteTIFFImage routine is:
%
%      status=WriteTIFFImage(image,verbose)
%
%  A description of each parameter follows:
%
%    o status:  Function WriteTIFFImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image:  A pointer to a Image structure.
%
%    o  verbose:  A value greater than zero prints detailed information about
%       the image.
%
%
*/
static unsigned int WriteTIFFImage(image,verbose)
Image
  *image;

unsigned int
  verbose;
{
  register RunlengthPacket
    *p;

  register int
    i,
    j,
    x,
    y;

  register unsigned char
    *q;

  TIFF
    *file;

  unsigned char
    *scanline;

  unsigned short
    photometric,
    rows_per_strip;

  /*
    Open TIFF file.
  */
  file=TIFFOpen(image->filename,"w");
  if (file == (TIFF *) NULL)
    return(False);
  /*
    Initialize TIFF fields.
  */
  TIFFSetField(file,TIFFTAG_DOCUMENTNAME,image->filename);
  TIFFSetField(file,TIFFTAG_SOFTWARE,"ImageMagick");
  if (image->comments != (char *) NULL)
    TIFFSetField(file,TIFFTAG_IMAGEDESCRIPTION,image->comments);
  if ((image->class == DirectClass) || (image->colors > 256))
    {
      photometric=PHOTOMETRIC_RGB;
      TIFFSetField(file,TIFFTAG_BITSPERSAMPLE,8);
      TIFFSetField(file,TIFFTAG_SAMPLESPERPIXEL,(image->alpha ? 4 : 3));
    }
  else
    {
      /*
        Determine if image is gray scale.
      */
      photometric=PHOTOMETRIC_MINISBLACK;
      for (i=0; i < image->colors; i++)
        if ((image->colormap[i].red != image->colormap[i].green) ||
            (image->colormap[i].green != image->colormap[i].blue))
          {
            photometric=PHOTOMETRIC_PALETTE;
            break;
          }
      if ((image->colors == 2) && (photometric == PHOTOMETRIC_MINISBLACK))
        TIFFSetField(file,TIFFTAG_BITSPERSAMPLE,1);
      else
        TIFFSetField(file,TIFFTAG_BITSPERSAMPLE,8);
      TIFFSetField(file,TIFFTAG_SAMPLESPERPIXEL,1);
    }
  TIFFSetField(file,TIFFTAG_PHOTOMETRIC,photometric);
  TIFFSetField(file,TIFFTAG_IMAGELENGTH,image->rows);
  TIFFSetField(file,TIFFTAG_IMAGEWIDTH,image->columns);
  TIFFSetField(file,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
  TIFFSetField(file,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
  TIFFSetField(file,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
  TIFFSetField(file,TIFFTAG_COMPRESSION,COMPRESSION_LZW);
  if (image->compression == NoCompression)
    TIFFSetField(file,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
  rows_per_strip=8192/TIFFScanlineSize(file);
  if (rows_per_strip == 0)
    rows_per_strip=1;
  TIFFSetField(file,TIFFTAG_ROWSPERSTRIP,rows_per_strip);
  scanline=(unsigned char *) malloc(TIFFScanlineSize(file));
  if (scanline == (unsigned char *) NULL)
    {
      Warning("memory allocation error",(char *) NULL);
      return(False);
    }
  p=image->pixels;
  q=scanline;
  x=0;
  y=0;
  if (photometric == PHOTOMETRIC_RGB)
    for (i=0; i < image->packets; i++)
    {
      for (j=0; j <= (int) p->length; j++)
      {
        /*
          Convert DirectClass packets to contiguous RGB scanlines.
        */
        *q++=p->red;
        *q++=p->green;
        *q++=p->blue;
        if (image->alpha)
          *q++=(unsigned char) p->index;
        x++;
        if (x == image->columns)
          {
            if (TIFFWriteScanline(file,scanline,y,0) < 0)
              break;
            q=scanline;
            x=0;
            y++;
          }
      }
      p++;
    }
  else
    if (photometric == PHOTOMETRIC_PALETTE)
      {
        unsigned short
          blue[256],
          green[256],
          red[256];

        /*
          Initialize TIFF colormap.
        */
        for (i=0; i < image->colors; i++)
        {
          red[i]=(unsigned short)
            ((image->colormap[i].red*65535)/(unsigned int) MaxRGB);
          green[i]=(unsigned short)
            ((image->colormap[i].green*65535)/(unsigned int) MaxRGB);
          blue[i]=(unsigned short)
            ((image->colormap[i].blue*65535)/(unsigned int) MaxRGB);
        }
        TIFFSetField(file,TIFFTAG_COLORMAP,red,green,blue);
        /*
          Convert PseudoClass packets to contiguous colormap indexed scanlines.
        */
        for (i=0; i < image->packets; i++)
        {
          for (j=0; j <= (int) p->length; j++)
          {
            *q++=(unsigned char) p->index;
            x++;
            if (x == image->columns)
              {
                if (TIFFWriteScanline(file,scanline,y,0) < 0)
                  break;
                q=scanline;
                x=0;
                y++;
              }
          }
          p++;
        }
      }
    else
      if (image->colors > 2)
        for (i=0; i < image->packets; i++)
        {
          for (j=0; j <= (int) p->length; j++)
          {
            /*
              Convert PseudoClass packets to contiguous grayscale scanlines.
            */
            *q++=(unsigned char) image->colormap[p->index].red;
            x++;
            if (x == image->columns)
              {
                if (TIFFWriteScanline(file,scanline,y,0) < 0)
                  break;
                q=scanline;
                x=0;
                y++;
              }
          }
          p++;
        }
      else
        {
          register unsigned char
            bit,
            byte,
            polarity;

          /*
            Convert PseudoClass packets to contiguous monochrome scanlines.
          */
          polarity=(Intensity(image->colormap[0]) >
            Intensity(image->colormap[1]) ? 0 : 1);
          bit=0;
          byte=0;
          x=0;
          for (i=0; i < image->packets; i++)
          {
            for (j=0; j <= (int) p->length; j++)
            {
              byte<<=1;
              if (p->index == polarity)
                byte|=0x01;
              bit++;
              if (bit == 8)
                {
                  *q++=byte;
                  bit=0;
                  byte=0;
                }
              x++;
              if (x == image->columns)
                {
                  /*
                    Advance to the next scanline.
                  */
                  if (bit != 0)
                    *q++=byte << (8-bit);
                  if (TIFFWriteScanline(file,scanline,y,0) < 0)
                    break;
                  q=scanline;
                  bit=0;
                  byte=0;
                  x=0;
                  y++;
               }
            }
            p++;
          }
        }
  (void) free((char *) scanline);
  (void) TIFFFlushData(file);
  if (verbose == True)
    TIFFPrintDirectory(file,stderr,False);
  (void) TIFFClose(file);
  return(True);
}
#else
static unsigned int WriteTIFFImage(image,verbose)
Image
  *image;

unsigned int
  verbose;
{
  Warning("TIFF library is not available",image->filename);
  return(WriteImage(image));
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e S U N I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteSUNImage writes an image to a file on disk in SUN
%  rasterfile format.
%
%  The format of the WriteSUNImage routine is:
%
%      status=WriteSUNImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteSUNImage(image)
Image
  *image;
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_FORMAT_RGB  3

  typedef struct SUN
  {
    int
      magic,
      width,
      height,
      depth,
      length,
      type,
      maptype,
      maplength;
  } SUN;

  SUN
    sun_header;

  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *sun_pixels;

  unsigned int
    grayscale;

  unsigned long
    lsb_first;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Initialize SUN raster file header.
  */
  sun_header.magic=0x59a66a95;
  sun_header.width=image->columns;
  sun_header.height=image->rows;
  sun_header.type=(image->class == DirectClass ? RT_FORMAT_RGB : RT_STANDARD);
  sun_header.maptype=RMT_NONE;
  sun_header.maplength=0;
  if ((image->class == DirectClass) || (image->colors > 256))
    {
      /*
        Full color sun raster.
      */
      sun_header.depth=(image->alpha ? 32 : 24);
      sun_header.length=image->columns*image->rows*(image->alpha ? 4 : 3);
      sun_header.length+=image->columns % 2 ? image->rows : 0;
    }
  else
    {
      /*
        Determine if image is grayscale.
      */
      grayscale=True;
      for (i=0; i < image->colors; i++)
        if ((image->colormap[i].red != image->colormap[i].green) ||
            (image->colormap[i].green != image->colormap[i].blue))
          {
            grayscale=False;
            break;
          }
      if ((image->colors > 2) || !grayscale)
        {
          /*
            Colormapped sun raster.
          */
          sun_header.depth=8;
          sun_header.length=image->columns*image->rows;
          sun_header.maptype=RMT_EQUAL_RGB;
          sun_header.maplength=image->colors*3;
          sun_header.length+=image->columns % 2 ? image->rows : 0;
        }
      else
        {
          /*
            Monochrome sun raster.
          */
          sun_header.depth=1;
          sun_header.length=((image->columns+7) >> 3)*image->rows;
          sun_header.length+=((image->columns/8)+(image->columns % 8 ? 1 : 0)) %
            2 ? image->rows : 0;
        }
    }
  /*
    Ensure the SUN raster header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if ((*(char *) &lsb_first) == 0)
    (void) fwrite((char *) &sun_header,sizeof(sun_header),1,image->file);
  else
    {
      SUN
        reverse_header;

      reverse_header=sun_header;
      MSBFirstOrderLong((char *) &reverse_header,sizeof(reverse_header));
      (void) fwrite((char *) &reverse_header,sizeof(reverse_header),1,
        image->file);
    }
  /*
    Convert MIFF to SUN raster pixels.
  */
  sun_pixels=(unsigned char *) malloc(sun_header.length*sizeof(unsigned char));
  if (sun_pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  p=image->pixels;
  q=sun_pixels;
  x=0;
  if ((image->class == DirectClass) || (image->colors > 256))
    {
      /*
        Convert DirectClass packet to SUN RGB pixel.
      */
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= (int) p->length; j++)
        {
          if (image->alpha)
            *q++=(unsigned char) p->index;
          *q++=p->red;
          *q++=p->green;
          *q++=p->blue;
          x++;
          if (x == image->columns)
            {
              if ((image->columns % 2) != 0)
                q++;  /* pad scanline */
              x=0;
            }
        }
        p++;
      }
    }
  else
    if ((image->colors > 2) || !grayscale)
      {
        unsigned char
          *sun_colormap;

        /*
           Dump colormap to file.
        */
        sun_colormap=(unsigned char *)
          malloc(sun_header.maplength*sizeof(unsigned char));
        if (sun_colormap == (unsigned char *) NULL)
          {
            Warning("unable to allocate memory",(char *) NULL);
            return(False);
          }
        q=sun_colormap;
        for (i=0; i < image->colors; i++)
          *q++=image->colormap[i].red;
        for (i=0; i < image->colors; i++)
          *q++=image->colormap[i].green;
        for (i=0; i < image->colors; i++)
          *q++=image->colormap[i].blue;
        (void) fwrite((char *) sun_colormap,sizeof(char),sun_header.maplength,
          image->file);
        (void) free((char *) sun_colormap);
        /*
          Convert PseudoClass packet to SUN colormapped pixel.
        */
        q=sun_pixels;
        for (i=0; i < image->packets; i++)
        {
          for (j=0; j <= (int) p->length; j++)
          {
            *q++=p->index;
            x++;
            if (x == image->columns)
              {
                if ((image->columns % 2) != 0)
                  q++;  /* pad scanline */
                x=0;
              }
          }
          p++;
        }
      }
    else
      {
        register unsigned char
          bit,
          byte,
          polarity;

        /*
          Convert PseudoClass image to a SUN monochrome image.
        */
        polarity=(Intensity(image->colormap[0]) <
          Intensity(image->colormap[1]) ? 0 : 1);
        bit=0;
        byte=0;
        for (i=0; i < image->packets; i++)
        {
          for (j=0; j <= (int) p->length; j++)
          {
            byte<<=1;
            if (p->index == polarity)
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                *q++=byte;
                bit=0;
                byte=0;
              }
            x++;
            if (x == image->columns)
              {
                /*
                  Advance to the next scanline.
                */
                if (bit != 0)
                  *q++=byte << (8-bit);
                if ((((image->columns/8)+
                    (image->columns % 8 ? 1 : 0)) % 2) != 0)
                  q++;  /* pad scanline */
                bit=0;
                byte=0;
                x=0;
             }
          }
          p++;
        }
      }
  (void) fwrite((char *) sun_pixels,sizeof(char),sun_header.length,image->file);
  (void) free((char *) sun_pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X B M I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure WriteXBMImage writes an image to a file on disk in X
%  bitmap format.
%
%  The format of the WriteXBMImage routine is:
%
%      status=WriteXBMImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteXBMImage(image)
Image
  *image;
{
  char
    name[2048];

  register int
    i,
    j,
    x;

  register char
    *q;

  register RunlengthPacket
    *p;

  register unsigned char
    bit,
    byte;

  register unsigned short
    polarity;

  unsigned int
    count;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Write X bitmap header.
  */
  (void) strcpy(name,image->filename);
  q=name;
  while ((*q != '.') && (*q != '\0'))
    q++;
  if (*q == '.')
    *q='\0';
  (void) fprintf(image->file,"#define %s_width %u\n",name,image->columns);
  (void) fprintf(image->file,"#define %s_height %u\n",name,image->rows);
  (void) fprintf(image->file,"static char %s_bits[] = {\n",name);
  (void) fprintf(image->file," ");
  /*
    Convert MIFF to X bitmap pixels.
  */
  QuantizeImage(image,2,8,False,GRAYColorspace,True);
  p=image->pixels;
  polarity=(Intensity(image->colormap[0]) >
    Intensity(image->colormap[1]) ? 0 : 1);
  bit=0;
  byte=0;
  count=0;
  x=0;
  (void) fprintf(image->file," ");
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      byte>>=1;
      if (p->index == polarity)
        byte|=0x80;
      bit++;
      if (bit == 8)
        {
          /*
            Write a bitmap byte to the image file.
          */
          (void) fprintf(image->file,"0x%02x, ",~byte & 0xff);
          count++;
          if (count == 12)
            {
              (void) fprintf(image->file,"\n  ");
              count=0;
            };
          bit=0;
          byte=0;
        }
      x++;
      if (x == image->columns)
        {
          if (bit != 0)
            {
              /*
                Write a bitmap byte to the image file.
              */
              byte>>=(8-bit);
              (void) fprintf(image->file,"0x%02x, ",~byte & 0xff);
              count++;
              if (count == 12)
                {
                  (void) fprintf(image->file,"\n  ");
                  count=0;
                };
              bit=0;
              byte=0;
            };
          x=0;
        }
    }
    p++;
  }
  (void) fprintf(image->file,"};\n");
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e Y U V I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteYUVImage writes an image to a file on disk in Y, U, and V
%  rasterfile format.  U and V, normally -0.5 through 0.5, is normalized to
%  the range 0 through 255 to fit within a byte.
%
%  The format of the WriteYUVImage routine is:
%
%      status=WriteYUVImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteYUVImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Convert MIFF to YUV raster pixels.
  */
  pixels=(unsigned char *)
    malloc(3*image->columns*image->rows*sizeof(unsigned char));
  if (pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  RGBTransformImage(image,YUVColorspace);
  p=image->pixels;
  q=pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      *q++=p->red;
      *q++=p->green;
      *q++=p->blue;
    }
    p++;
  }
  (void) fwrite((char *) pixels,sizeof(unsigned char),
    (int) (3*image->columns*image->rows),image->file);
  (void) free((char *) pixels);
  TransformRGBImage(image,YUVColorspace);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X W D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteXWDImage writes an image to a file on disk in XWD
%  rasterfile format.
%
%  The format of the WriteXWDImage routine is:
%
%      status=WriteXWDImage(image)
%
%  A description of each parameter follows.
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image:  A pointer to a Image structure.
%
%
*/
static unsigned int WriteXWDImage(image)
Image
  *image;
{
  int
    x;

  register int
    i,
    j;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  unsigned char
    *xwd_pixels;

  unsigned int
    scanline_pad;

  unsigned long
    lsb_first,
    packets;

  XWDFileHeader
    xwd_header;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  /*
    Initialize XWD file header.
  */
  xwd_header.header_size=sizeof(xwd_header)+strlen(image->filename)+1;
  xwd_header.file_version=(unsigned long) XWD_FILE_VERSION;
  xwd_header.pixmap_format=(unsigned long) ZPixmap;
  xwd_header.pixmap_depth=(unsigned long)
    (image->class == DirectClass ? 24 : 8);
  xwd_header.pixmap_width=(unsigned long) image->columns;
  xwd_header.pixmap_height=(unsigned long) image->rows;
  xwd_header.xoffset=(unsigned long) 0;
  xwd_header.byte_order=(unsigned long) MSBFirst;
  xwd_header.bitmap_unit=(unsigned long) (image->class == DirectClass ? 32 : 8);
  xwd_header.bitmap_bit_order=(unsigned long) MSBFirst;
  xwd_header.bitmap_pad=(unsigned long) (image->class == DirectClass ? 32 : 8);
  xwd_header.bits_per_pixel=(unsigned long)
    (image->class == DirectClass ? 24 : 8);
  xwd_header.bytes_per_line=(unsigned long) ((((xwd_header.bits_per_pixel*
    xwd_header.pixmap_width)+((xwd_header.bitmap_pad)-1))/
    (xwd_header.bitmap_pad))*((xwd_header.bitmap_pad) >> 3));
  xwd_header.visual_class=(unsigned long)
    (image->class == DirectClass ? DirectColor : PseudoColor);
  xwd_header.red_mask=(unsigned long)
    (image->class == DirectClass ? 0xff0000 : 0);
  xwd_header.green_mask=(unsigned long)
    (image->class == DirectClass ? 0xff00 : 0);
  xwd_header.blue_mask=(unsigned long) (image->class == DirectClass ? 0xff : 0);
  xwd_header.bits_per_rgb=(unsigned long)
    (image->class == DirectClass ? 24 : 8);
  xwd_header.colormap_entries=(unsigned long)
    (image->class == DirectClass ? 256 : image->colors);
  xwd_header.ncolors=(image->class == DirectClass ? 0 : image->colors);
  xwd_header.window_width=(unsigned long) image->columns;
  xwd_header.window_height=(unsigned long) image->rows;
  xwd_header.window_x=0;
  xwd_header.window_y=0;
  xwd_header.window_bdrwidth=(unsigned long) 0;
  /*
    Ensure the xwd_header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBFirstOrderLong((char *) &xwd_header,sizeof(xwd_header));
  (void) fwrite((char *) &xwd_header,sizeof(xwd_header),1,image->file);
  (void) fwrite((char *) image->filename,1,strlen(image->filename)+1,
    image->file);
  if (image->class == PseudoClass)
    {
      XColor
        *colors;

      /*
         Dump colormap to file.
      */
      colors=(XColor *) malloc(image->colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        {
          Warning("unable to allocate memory",(char *) NULL);
          return(False);
        }
      for (i=0; i < image->colors; i++)
      {
        colors[i].pixel=i;
        colors[i].red=image->colormap[i].red << 8;
        colors[i].green=image->colormap[i].green << 8;
        colors[i].blue=image->colormap[i].blue << 8;
        colors[i].flags=DoRed | DoGreen | DoBlue;
        colors[i].pad=0;
        if (*(char *) &lsb_first)
          {
            MSBFirstOrderLong((char *) &colors[i].pixel,sizeof(long));
            MSBFirstOrderShort((char *) &colors[i].red,3*sizeof(short));
          }
      }
      (void) fwrite((char *) colors,sizeof(XColor),(int) image->colors,
        image->file);
      (void) free((char *) colors);
    }
  /*
    Convert MIFF to XWD raster pixels.
  */
  packets=xwd_header.bytes_per_line*xwd_header.pixmap_height;
  xwd_pixels=(unsigned char *)
    malloc((unsigned int) packets*sizeof(unsigned char));
  if (xwd_pixels == (unsigned char *) NULL)
    {
      Warning("unable to allocate memory",(char *) NULL);
      return(False);
    }
  scanline_pad=(unsigned int) (xwd_header.bytes_per_line-
    ((xwd_header.pixmap_width*xwd_header.bits_per_pixel) >> 3));
  x=0;
  p=image->pixels;
  q=xwd_pixels;
  for (i=0; i < image->packets; i++)
  {
    for (j=0; j <= ((int) p->length); j++)
    {
      if (image->class == PseudoClass)
        *q++=p->index;
      else
        {
          *q++=p->red;
          *q++=p->green;
          *q++=p->blue;
        }
      x++;
      if (x == image->columns)
        {
          q+=scanline_pad;
          x=0;
        }
    }
    p++;
  }
  /*
    Dump pixels to file.
  */
  (void) fwrite((char *) xwd_pixels,sizeof(char),(int) packets,image->file);
  (void) free((char *) xwd_pixels);
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t A l i e n I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function GetAlienInfo initializes the AlienInfo structure.
%
%  The format of the GetAlienInfo routine is:
%
%      GetAlienInfo(alien_info)
%
%  A description of each parameter follows:
%
%    o alien_info: Specifies a pointer to a AlienInfo structure.
%
%
*/
void GetAlienInfo(alien_info)
AlienInfo
  *alien_info;
{
  alien_info->server_name=(char *) NULL;
  alien_info->font=(char *) NULL;
  alien_info->geometry=(char *) NULL;
  alien_info->density=(char *) NULL;
  alien_info->verbose=False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d A l i e n I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadAlienImage reads an image and returns it.  It allocates
%  the memory necessary for the new Image structure and returns a pointer to
%  the new image.  By default, the image format is determined by its magic
%  number. To specify a particular image format, precede the filename with an
%  explicit image format name and a colon (i.e.  mtv:image) or as the filename
%  suffix  (i.e. image.mtv).
%
%  The format of the ReadAlienImage routine is:
%
%      image=ReadAlienImage(alien_info)
%
%  A description of each parameter follows:
%
%    o image: Function ReadAlienImage returns a pointer to the image after
%      reading.  A null image is returned if there is a a memory shortage or
%      if the image cannot be read.
%
%    o alien_info: Specifies a pointer to an AlienInfo structure.
%
%
*/
Image *ReadAlienImage(alien_info)
AlienInfo
  *alien_info;
{
  char
    magick[12],
    magic_number[12],
    *p;

  Image
    alien_image,
    *image;

  register char
    c,
    *q;

  register int
    i;

  unsigned int
    temporary_file;

  /*
    Look for explicit 'format:image' in filename.
  */
  *magick='\0';
  (void) strcpy(alien_image.filename,alien_info->filename);
  p=alien_image.filename;
  while ((*p != ':') && (*p != '\0'))
    p++;
  if ((*p == ':') && ((p-alien_image.filename) < sizeof(magic_number)))
    {
      /*
        User specified alien image format.
      */
      (void) strncpy(magic_number,alien_image.filename,p-alien_image.filename);
      magic_number[p-alien_image.filename]='\0';
      for (q=magic_number; *q != '\0'; q++)
      {
        c=(*q);
        if (isascii(c) && islower(c))
          *q=toupper(c);
      }
      for (i=0; AlienTypes[i] != (char *) NULL; i++)
        if (strcmp(magic_number,AlienTypes[i]) == 0)
          {
            /*
              Strip off image format prefix.
            */
            p++;
            (void) strcpy(alien_image.filename,p);
            (void) strcpy(magick,magic_number);
            break;
          }
    }
  temporary_file=False;
  if (*magick == '\0')
    {
      /*
        Look for 'image.format' in filename.
      */
      (void) strcpy(magick,"MIFF");
      p=alien_image.filename+strlen(alien_image.filename)-1;
      while ((*p != '.') && (p > alien_image.filename))
        p--;
      if ((*p == '.') && (strlen(p) < sizeof(magic_number)))
        {
          /*
            File specified alien image format?
          */
          (void) strcpy(magic_number,p+1);
          for (q=magic_number; *q != '\0'; q++)
          {
            c=(*q);
            if (isascii(c) && islower(c))
              *q=toupper(c);
          }
          for (i=0; AlienTypes[i] != (char *) NULL; i++)
            if (strcmp(magic_number,AlienTypes[i]) == 0)
              {
                (void) strcpy(magick,magic_number);
                break;
              }
        }
      /*
        Determine type from image magic number.
      */
      temporary_file=(*alien_image.filename == '-');
      if (temporary_file)
        {
          char
            *directory;

          int
            c;

          /*
            Copy standard input to temporary file.
          */
          directory=(char *) getenv("TMPDIR");
          if (directory == (char *) NULL)
            directory="/tmp";
          (void) sprintf(alien_image.filename,"%s/magickXXXXXX",directory);
          (void) mktemp(alien_image.filename);
          alien_image.file=fopen(alien_image.filename,"w");
          if (alien_image.file == (FILE *) NULL)
            return((Image *) NULL);
          c=getchar();
          while (c != EOF)
          {
            (void) putc(c,alien_image.file);
            c=getchar();
          }
          (void) fclose(alien_image.file);
        }
      /*
        Open image file.
      */
      *magic_number='\0';
      OpenImage(&alien_image,"r");
      if (alien_image.file != (FILE *) NULL)
        {
          /*
            Read magic number.
          */
          (void) ReadData(magic_number,sizeof(char),sizeof(magic_number),
            alien_image.file);
          CloseImage(&alien_image);
        }
      /*
        Determine the image format.
      */
      if (strncmp(magic_number,"GIF8",4) == 0)
        (void) strcpy(magick,"GIF");
      if (strncmp(magic_number,"\377\330\377",3) == 0)
        (void) strcpy(magick,"JPEG");
      else
        if ((strcmp(magick,"JPEG") == 0) || (strcmp(magick,"JPG") == 0))
          (void) strcpy(magick,"MIFF");
      if ((*magic_number == 'P') && isdigit(magic_number[1]))
        (void) strcpy(magick,"PNM");
      if (strncmp(magic_number,"%!",2) == 0)
        (void) strcpy(magick,"PS");
      if (strncmp(magic_number,"\131\246\152\225",4) == 0)
        (void) strcpy(magick,"SUN");
      if ((strncmp(magic_number,"\115\115",2) == 0) ||
          (strncmp(magic_number,"\111\111",2) == 0))
        (void) strcpy(magick,"TIFF");
      if (strncmp(magic_number,"\122\314",2) == 0)
        (void) strcpy(magick,"RLE");
      if ((strncmp(magic_number,"LBLSIZE",7) == 0) ||
         (strncmp(magic_number,"NJPL1I",6) == 0))
        (void) strcpy(magick,"VICAR");
      if (strncmp(magic_number,"#define",7) == 0)
        (void) strcpy(magick,"XBM");
      if (strncmp(magic_number,"\000\000\000\110\000\000\000\007",8) == 0)
        (void) strcpy(magick,"XWD");
    }
  /*
    Call appropriate image reader based on image type.
  */
  (void) strcpy(alien_info->filename,alien_image.filename);
  switch (*magick)
  {
    case 'A':
    {
      image=ReadAVSImage(alien_info);
      break;
    }
    case 'C':
    {
      image=ReadCMYKImage(alien_info);
      break;
    }
    case 'F':
    {
      Warning("Cannot read FAX images",alien_info->filename);
      image=ReadImage(alien_info->filename);
      break;
    }
    case 'G':
    {
      if (strcmp(magick,"GIF") == 0)
        image=ReadGIFImage(alien_info);
      else
        image=
          ReadGRAYImage(alien_info);
      break;
    }
    case 'H':
    {
      Warning("Cannot read HISTOGRAM images",alien_info->filename);
      image=ReadImage(alien_info->filename);
      break;
    }
    case 'J':
    {
      image=ReadJPEGImage(alien_info);
      break;
    }
    case 'M':
    {
      if (strcmp(magick,"MIFF") == 0)
        image=ReadImage(alien_info->filename);
      else
        image=ReadMTVImage(alien_info);
      break;
    }
    case 'P':
    {
      if (strcmp(magick,"PNM") == 0)
        image=ReadPNMImage(alien_info);
      else
        image=ReadPSImage(alien_info);
      break;
    }
    case 'R':
    {
      if (strcmp(magick,"RGB") == 0)
        image=ReadRGBImage(alien_info);
      else
        image=ReadRLEImage(alien_info);
      break;
    }
    case 'S':
    {
      image=ReadSUNImage(alien_info);
      break;
    }
    case 'T':
    {
      if (strcmp(magick,"TIFF") == 0)
        image=ReadTIFFImage(alien_info);
      else
        image=ReadTEXTImage(alien_info->filename,alien_info->server_name,
          alien_info->font,alien_info->density);
      break;
    }
    case 'V':
    {
      image=ReadVICARImage(alien_info);
      break;
    }
    case 'Y':
    {
      image=ReadYUVImage(alien_info);
      break;
    }
    case 'X':
    {
      if (strcmp(magick,"X") == 0)
        image=ReadXImage(alien_info->filename,alien_info->server_name,False,
          True,False);
      else
        if (strcmp(magick,"XC") == 0)
          image=ReadXCImage(alien_info->filename,alien_info->server_name,
            alien_info->geometry);
        else
          if (strcmp(magick,"XBM") == 0)
            image=ReadXBMImage(alien_info);
          else
            image=ReadXWDImage(alien_info);
      break;
    }
    default:
      image=ReadImage(alien_info->filename);
  }
  if (temporary_file)
    (void) unlink(alien_info->filename);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e A l i e n I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteAlienImage writes an image to a file.  You can specify a
%  particular image format by prefixing the file with the image type and a
%  colon (i.e. mtv:image) or specify the image type as the filename suffix
%  (i.e. image.mtv).
%
%  The format of the WriteAlienImage routine is:
%
%      status=WriteAlienImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function WriteAlienImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image: A pointer to a Image structure.
%
%
*/
unsigned int WriteAlienImage(image)
Image
  *image;
{
  char
    magick[12],
    *p;

  Image
    alien_image;

  register char
    c,
    *q;

  register int
    i;

  unsigned int
    status;

  /*
    Look for 'image.format' in filename.
  */
  alien_image=(*image);
  p=alien_image.filename+strlen(alien_image.filename)-1;
  while ((*p != '.') && (p > alien_image.filename))
    p--;
  if ((*p == '.') && (strlen(p) < sizeof(magick)))
    {
      /*
        User specified alien image format.
      */
      (void) strcpy(magick,p+1);
      for (q=magick; *q != '\0'; q++)
      {
        c=(*q);
        if (isascii(c) && islower(c))
          *q=toupper(c);
      }
      for (i=0; AlienTypes[i] != (char *) NULL; i++)
        if (strcmp(magick,AlienTypes[i]) == 0)
          {
            (void) strcpy(alien_image.magick,magick);
            break;
          }
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  p=alien_image.filename;
  while ((*p != ':') && (*p != '\0'))
    p++;
  if ((*p == ':') && ((p-alien_image.filename) < sizeof(magick)))
    {
      /*
        User specified alien image format.
      */
      (void) strncpy(magick,alien_image.filename,p-alien_image.filename);
      magick[p-alien_image.filename]='\0';
      for (q=magick; *q != '\0'; q++)
      {
        c=(*q);
        if (isascii(c) && islower(c))
          *q=toupper(c);
      }
      for (i=0; AlienTypes[i] != (char *) NULL; i++)
        if (strcmp(magick,AlienTypes[i]) == 0)
          {
            /*
              Strip off image format prefix.
            */
            p++;
            (void) strcpy(alien_image.filename,p);
            (void) strcpy(alien_image.magick,magick);
            break;
          }
    }
  /*
    Call appropriate image writer based on image type.
  */
  switch (*alien_image.magick)
  {
    case 'A':
    {
      status=WriteAVSImage(&alien_image);
      break;
    }
    case 'C':
    {
      status=WriteCMYKImage(&alien_image);
      break;
    }
    case 'F':
    {
      status=WriteFAXImage(&alien_image);
      break;
    }
    case 'G':
    {
      if (strcmp(alien_image.magick,"GIF") == 0)
        status=WriteGIFImage(&alien_image);
      else
        status=WriteGRAYImage(&alien_image);
      break;
    }
    case 'H':
    {
      status=WriteHISTOGRAMImage(&alien_image);
      break;
    }
    case 'J':
    {
      status=WriteJPEGImage(&alien_image);
      break;
    }
    case 'M':
    {
      if (strcmp(alien_image.magick,"MTV") == 0)
        status=WriteMTVImage(&alien_image);
      else
        status=WriteImage(&alien_image);
      break;
    }
    case 'P':
    {
      if (strcmp(alien_image.magick,"PNM") == 0)
        status=WritePNMImage(&alien_image);
      else
        status=PrintImage(&alien_image,"+0+0");
      break;
    }
    case 'R':
    {
      if (strcmp(alien_image.magick,"RGB") == 0)
        status=WriteRGBImage(&alien_image);
      else
        {
          Warning("Cannot write RLE images",alien_image.filename);
          status=WriteImage(&alien_image);
        }
      break;
    }
    case 'S':
    {
      status=WriteSUNImage(&alien_image);
      break;
    }
    case 'T':
    {
      if (strcmp(alien_image.magick,"TIFF") == 0)
        status=WriteTIFFImage(&alien_image,True);
      else
        {
          Warning("Cannot write text images",alien_image.filename);
          status=WriteImage(&alien_image);
        }
      break;
    }
    case 'V':
    {
      Warning("Cannot write VICAR images",alien_image.filename);
      status=WriteImage(&alien_image);
      break;
    }
    case 'Y':
    {
      status=WriteYUVImage(&alien_image);
      break;
    }
    case 'X':
    {
      if (strcmp(alien_image.magick,"X") == 0)
        status=WriteImage(&alien_image);
      else
        if (strcmp(alien_image.magick,"XBM") == 0)
          status=WriteXBMImage(&alien_image);
        else
          if (strcmp(alien_image.magick,"XC") == 0)
            {
              Warning("Cannot write X constant images",alien_image.filename);
              status=WriteImage(&alien_image);
            }
          else
            status=WriteXWDImage(&alien_image);
      break;
    }
    default:
      status=WriteImage(&alien_image);
  }
  return(status);
}
