/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                     IIIII  M   M   AAA   GGGG  EEEEE                        %
%                       I    MM MM  A   A G      E                            %
%                       I    M M M  AAAAA G  GG  EEE                          %
%                       I    M   M  A   A G   G  E                            %
%                     IIIII  M   M  A   A  GGGG  EEEEE                        %
%                                                                             %
%                                                                             %
%                    Utiltity Image Routines for Display                      %
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
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"
#include "alien.h"
#include "X.h"
#include "compress.h"

/*
  External declarations.
*/
extern char
  *client_name;

/*
  Forward declarations.
*/
static Image
  *ZoomImage _Declare((Image *));

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function AllocateImage allocates an Image structure and initializes each
%  field to a default value.
%
%  The format of the AllocateImage routine is:
%
%      allocated_image=AllocateImage(magick)
%
%  A description of each parameter follows:
%
%    o allocated_image: Function AllocateImage returns a pointer to an image
%      structure initialized to default values.  A null image is returned if
%      there is a memory shortage.
%
%    o magick: Specifies the image format (i.e. MIFF, GIF, JPEG, etc.).
%
%
*/
Image *AllocateImage(magick)
char
  *magick;
{
  Image
    *allocated_image;

  /*
    Allocate image structure.
  */
  allocated_image=(Image *) malloc(sizeof(Image));
  if (allocated_image == (Image *) NULL)
    {
      Warning("unable to allocate image","memory allocation error");
      return((Image *) NULL);
    }
  /*
    Initialize Image structure.
  */
  allocated_image->file=(FILE *) NULL;
  *allocated_image->filename='\0';
  if (strlen(magick) < sizeof(allocated_image->magick))
    (void) strcpy(allocated_image->magick,magick);
  allocated_image->comments=(char *) NULL;
  allocated_image->label=(char *) NULL;
  allocated_image->id=UndefinedId;
  allocated_image->class=DirectClass;
  allocated_image->alpha=False;
  allocated_image->compression=RunlengthEncodedCompression;
  allocated_image->columns=0;
  allocated_image->rows=0;
  allocated_image->colors=0;
  allocated_image->scene=0;
  allocated_image->quality=75;
  allocated_image->montage=(char *) NULL;
  allocated_image->directory=(char *) NULL;
  allocated_image->colormap=(ColorPacket *) NULL;
  allocated_image->signature=(char *) NULL;
  allocated_image->pixels=(RunlengthPacket *) NULL;
  allocated_image->packet=(RunlengthPacket *) NULL;
  allocated_image->packets=0;
  allocated_image->packet_size=0;
  allocated_image->packed_pixels=(unsigned char *) NULL;
  allocated_image->orphan=False;
  allocated_image->last=(Image *) NULL;
  allocated_image->next=(Image *) NULL;
  return(allocated_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B o r d e r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function BorderImage takes an image and puts a border around it of a
%  particular color.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the BorderImage routine is:
%
%      bordered_image=BorderImage(image,border_width,border_height,
%        border_color)
%
%  A description of each parameter follows:
%
%    o bordered_image: Function BorderImage returns a pointer to the bordered
%      image.  A null image is returned if there is a a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o border_width: An integer that specifies the number of pixels in width of
%      the bordered image.
%
%    o border_height: An integer that specifies the number of pixels in height
%      of the bordered image.
%
%    o border_color: A pointer to a ColorPacket which contains the red, green,
%      and blue components of the border color.
%
%
*/
Image *BorderImage(image,border_width,border_height,border_color)
Image
  *image;

unsigned int
  border_width,
  border_height;

ColorPacket
  border_color;
{
  Image
    *bordered_image;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  /*
    Initialize bordered image attributes.
  */
  bordered_image=CopyImage(image,image->columns+border_width*2,
    image->rows+border_height*2,False);
  if (bordered_image == (Image *) NULL)
    {
      Warning("unable to border image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Copy image and put border around it.
  */
  p=image->pixels;
  q=bordered_image->pixels;
  image->runlength=p->length+1;
  for (i=0; i < (bordered_image->columns*border_height); i++)
  {
    q->red=border_color.red;
    q->green=border_color.green;
    q->blue=border_color.blue;
    q->index=border_color.index;
    q->length=0;
    q++;
  }
  for (y=0; y < image->rows; y++)
  {
    /*
      Initialize scanline with border color.
    */
    for (i=0; i < border_width; i++)
    {
      q->red=border_color.red;
      q->green=border_color.green;
      q->blue=border_color.blue;
      q->index=border_color.index;
      q->length=0;
      q++;
    }
    /*
      Transfer scanline.
    */
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q->index=p->index;
      q->length=0;
      q++;
    }
    for (i=0; i < border_width; i++)
    {
      q->red=border_color.red;
      q->green=border_color.green;
      q->blue=border_color.blue;
      q->index=border_color.index;
      q->length=0;
      q++;
    }
  }
  for (i=0; i < (bordered_image->columns*border_height); i++)
  {
    q->red=border_color.red;
    q->green=border_color.green;
    q->blue=border_color.blue;
    q->index=border_color.index;
    q->length=0;
    q++;
  }
  return(bordered_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ClipImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.  The pixels are copied from the source
%  image as defined by the region formed from x_offset, y_offset, width, and
%  height.
%
%  The format of the ClipImage routine is:
%
%      clipped_image=ClipImage(image,x_offset,y_offset,width,height)
%
%  A description of each parameter follows:
%
%    o clipped_image: Function ClipImage returns a pointer to the clipped
%      image.  A null image is returned if there is a a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the column offset of the
%      clipped image.
%
%    o y_offset: An integer that specifies the row offset of the clipped
%      image.
%
%    o width: An integer that specifies the number of pixels in width of the
%      clipped image.
%
%    o height: An integer that specifies the number of pixels in height of the
%      clipped image.
%
%
*/
Image *ClipImage(image,x_offset,y_offset,width,height)
Image
  *image;

int
  x_offset,
  y_offset;

unsigned int
  width,
  height;
{
  Image
    *clipped_image;

  register int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  /*
    Check clip geometry.
  */
  if ((width == 0) || (height == 0))
    {
      Warning("unable to clip image","image size is zero");
      return((Image *) NULL);
    }
  if (((x_offset+(int) width) < 0) || ((y_offset+(int) height) < 0) ||
      (x_offset > (int) image->columns) || (y_offset > (int) image->rows))
    {
      Warning("unable to clip image","geometry does not contain image");
      return((Image *) NULL);
    }
  if ((x_offset+(int) width) > (int) image->columns)
    width=(unsigned int) ((int) image->columns-x_offset);
  if ((y_offset+(int) height) > (int) image->rows)
    height=(unsigned int) ((int) image->rows-y_offset);
  if (x_offset < 0)
    {
      width-=(unsigned int) (-x_offset);
      x_offset=0;
    }
  if (y_offset < 0)
    {
      height-=(unsigned int) (-y_offset);
      y_offset=0;
    }
  /*
    Initialize clipped image attributes.
  */
  clipped_image=CopyImage(image,width,height,False);
  if (clipped_image == (Image *) NULL)
    {
      Warning("unable to clip image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Skip pixels up to the clipped image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  for (x=0; x < (y_offset*image->columns+x_offset); x++)
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
  /*
    Extract clipped image.
  */
  q=clipped_image->pixels;
  for (y=0; y < clipped_image->rows; y++)
  {
    /*
      Transfer scanline.
    */
    for (x=0; x < clipped_image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q->index=p->index;
      q->length=0;
      q++;
    }
    /*
      Skip to next scanline.
    */
    for (x=0; x < (image->columns-clipped_image->columns); x++)
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
  }
  return(clipped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o s e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CloseImage closes a file associated with the image.
%
%  The format of the CloseImage routine is:
%
%      CloseImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void CloseImage(image)
Image
  *image;
{
  /*
    Close image file.
  */
  if (image->file != (FILE *) NULL)
    if (((int) strlen(image->filename) < 3) ||
        (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0))
      (void) fclose(image->file);
    else
      (void) pclose(image->file);
  image->file=(FILE *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p r e s s C o l o r m a p                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CompressColormap compresses an image colormap removing any
%  unused color entries.
%
%  The format of the CompressColormap routine is:
%
%      CompressColormap(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void CompressColormap(image)
Image
  *image;
{
  ColorPacket
    *colormap;

  int
    number_colors;

  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  /*
    Determine if colormap can be compressed.
  */
  if (image->class != PseudoClass)
    return;
  number_colors=image->colors;
  for (i=0; i < image->colors; i++)
    image->colormap[i].flags=False;
  image->colors=0;
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    if (!image->colormap[p->index].flags)
      {
        image->colormap[p->index].index=image->colors;
        image->colormap[p->index].flags=True;
        image->colors++;
      }
    p++;
  }
  if (image->colors == number_colors)
    return;  /* no unused entries */
  /*
    Compress colormap.
  */
  colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
  if (colormap == (ColorPacket *) NULL)
    {
      Warning("unable to compress colormap","memory allocation error");
      image->colors=number_colors;
      return;
    }
  for (i=0; i < number_colors; i++)
    if (image->colormap[i].flags)
      {
        index=image->colormap[i].index;
        colormap[index].red=image->colormap[i].red;
        colormap[index].green=image->colormap[i].green;
        colormap[index].blue=image->colormap[i].blue;
      }
  /*
    Remap pixels.
  */
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    p->index=image->colormap[p->index].index;
    p++;
  }
  (void) free((char *) image->colormap);
  image->colormap=colormap;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p r e s s I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CompressImage compresses an image to the minimum number of
%  runlength-encoded packets.
%
%  The format of the CompressImage routine is:
%
%      CompressImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void CompressImage(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p,
    *q;

  /*
    Compress image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  image->packets=0;
  q=image->pixels;
  q->length=MaxRunlength;
  for (i=0; i < (image->columns*image->rows); i++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    if ((p->red == q->red) && (p->green == q->green) &&
        (p->blue == q->blue) && (q->length < MaxRunlength))
      q->length++;
    else
      {
        if (image->packets > 0)
          q++;
        image->packets++;
        q->red=p->red;
        q->green=p->green;
        q->blue=p->blue;
        q->index=p->index;
        q->length=0;
      }
  }
  image->pixels=(RunlengthPacket *) realloc((char *) image->pixels,
    (unsigned int) image->packets*sizeof(RunlengthPacket));
  /*
    Runlength-encode only if it consumes less memory than no compression.
  */
  if (image->compression == RunlengthEncodedCompression)
    if (image->class == DirectClass)
      {
        if (image->packets >= ((image->columns*image->rows*3) >> 2))
          image->compression=NoCompression;
      }
    else
      if (image->packets >= ((image->columns*image->rows) >> 1))
        image->compression=NoCompression;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p o s i t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CompositeImage returns the second image composited onto the
%  first at the specified offsets.
%
%  The format of the CompositeImage routine is:
%
%      CompositeImage(image,compose,composite_image,x_offset,y_offset)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o compose: Specifies an image composite operator.
%
%    o composite_image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the column offset of the composited
%      image.
%
%    o y_offset: An integer that specifies the row offset of the composited
%      image.
%
%
*/
void CompositeImage(image,compose,composite_image,x_offset,y_offset)
Image
  *image;

unsigned int
  compose;

Image
  *composite_image;

int
  x_offset,
  y_offset;
{
  int
    blue,
    green,
    red;

  register int
    i,
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  register short int
    index;

  /*
    Check composite geometry.
  */
  if (((x_offset+(int) image->columns) < 0) ||
      ((y_offset+(int) image->rows) < 0) ||
      (x_offset > (int) image->columns) || (y_offset > (int) image->rows))
    {
      Warning("unable to composite image","geometry does not contain image");
      return;
    }
  /*
    Image must be uncompressed.
  */
  if (!UncompressImage(image))
    return;
  if (!image->alpha)
    {
      /*
        Initialize image 1 alpha data.
      */
      q=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        q->index=MaxRGB;
        q++;
      }
      image->class=DirectClass;
      image->alpha=True;
    }
  if (!composite_image->alpha)
    {
      /*
        Initialize composite image alpha data.
      */
      p=composite_image->pixels;
      red=(p+composite_image->packets-1)->red;
      green=(p+composite_image->packets-1)->green;
      blue=(p+composite_image->packets-1)->blue;
      if ((p->red != red) || (p->green != green) || (p->blue != blue))
        for (i=0; i < composite_image->packets; i++)
        {
          p->index=MaxRGB;
          p++;
        }
      else
        for (i=0; i < composite_image->packets; i++)
        {
          p->index=MaxRGB;
          if ((p->red == red) && (p->green == green) && (p->blue == blue))
            p->index=0;
          p++;
        }
      composite_image->class=DirectClass;
      composite_image->alpha=True;
    }
  /*
    Initialize composited image.
  */
  p=composite_image->pixels;
  composite_image->runlength=p->length+1;
  for (y=0; y < composite_image->rows; y++)
  {
    if (((y_offset+y) < 0) || ((y_offset+y) >= image->rows))
      continue;
    q=image->pixels+(y_offset+y)*image->columns+x_offset;
    for (x=0; x < composite_image->columns; x++)
    {
      if (composite_image->runlength > 0)
        composite_image->runlength--;
      else
        {
          p++;
          composite_image->runlength=p->length;
        }
      if (((x_offset+x) < 0) || ((x_offset+x) >= image->columns))
        {
          q++;
          continue;
        }
      switch (compose)
      {
        case OverCompositeOp:
        default:
        {
          if (p->index == 0)
            {
              red=q->red;
              green=q->green;
              blue=q->blue;
              index=q->index;
            }
          else
            if (p->index == MaxRGB)
              {
                red=p->red;
                green=p->green;
                blue=p->blue;
                index=p->index;
              }
            else
              {
                red=(int) (p->red*MaxRGB+q->red*(MaxRGB-p->index))/MaxRGB;
                green=(int) (p->green*MaxRGB+q->green*(MaxRGB-p->index))/MaxRGB;
                blue=(int) (p->blue*MaxRGB+q->blue*(MaxRGB-p->index))/MaxRGB;
                index=(int) (p->index*MaxRGB+q->index*(MaxRGB-p->index))/MaxRGB;
              }
          break;
        }
        case InCompositeOp:
        {
          red=(int) (p->red*q->index)/MaxRGB;
          green=(int) (p->green*q->index)/MaxRGB;
          blue=(int) (p->blue*q->index)/MaxRGB;
          index=(int) (p->index*q->index)/MaxRGB;
          break;
        }
        case OutCompositeOp:
        {
          red=(int) (p->red*(MaxRGB-q->index))/MaxRGB;
          green=(int) (p->green*(MaxRGB-q->index))/MaxRGB;
          blue=(int) (p->blue*(MaxRGB-q->index))/MaxRGB;
          index=(int) (p->index*(MaxRGB-q->index))/MaxRGB;
          break;
        }
        case AtopCompositeOp:
        {
          red=(int) (p->red*q->index+q->red*(MaxRGB-p->index))/MaxRGB;
          green=(int) (p->green*q->index+q->green*(MaxRGB-p->index))/MaxRGB;
          blue=(int) (p->blue*q->index+q->blue*(MaxRGB-p->index))/MaxRGB;
          index=(int) (p->index*q->index+q->index*(MaxRGB-p->index))/MaxRGB;
          break;
        }
        case XorCompositeOp:
        {
          red=(int) (p->red*(MaxRGB-q->index)+q->red*(MaxRGB-p->index))/MaxRGB;
          green=(int) (p->green*(MaxRGB-q->index)+q->green*(MaxRGB-p->index))/
            MaxRGB;
          blue=(int) (p->blue*(MaxRGB-q->index)+q->blue*(MaxRGB-p->index))/
            MaxRGB;
          index=(int) (p->index*(MaxRGB-q->index)+q->index*(MaxRGB-p->index))/
            MaxRGB;
          break;
        }
        case PlusCompositeOp:
        {
          red=(int) p->red+(int) q->red;
          green=(int) p->green+(int) q->green;
          blue=(int) p->blue+(int) q->blue;
          index=(int) p->index+(int) q->index;
          break;
        }
        case MinusCompositeOp:
        {
          red=(int) p->red-(int) q->red;
          green=(int) p->green-(int) q->green;
          blue=(int) p->blue-(int) q->blue;
          index=255;
          break;
        }
        case AddCompositeOp:
        {
          red=(int) p->red+(int) q->red;
          if (red > MaxRGB)
            red-=(MaxRGB+1);
          green=(int) p->green+(int) q->green;
          if (green > MaxRGB)
            green-=(MaxRGB+1);
          blue=(int) p->blue+(int) q->blue;
          if (blue > MaxRGB)
            blue-=(MaxRGB+1);
          index=(int) p->index+(int) q->index;
          if (index > MaxRGB)
            index-=(MaxRGB+1);
          break;
        }
        case SubtractCompositeOp:
        {
          red=(int) p->red-(int) q->red;
          if (red < 0)
            red+=(MaxRGB+1);
          green=(int) p->green-(int) q->green;
          if (green < 0)
            green+=(MaxRGB+1);
          blue=(int) p->blue-(int) q->blue;
          if (blue < 0)
            blue+=(MaxRGB+1);
          index=(int) p->index-(int) q->index;
          if (index < 0)
            index+=(MaxRGB+1);
          break;
        }
        case DifferenceCompositeOp:
        {
          red=AbsoluteValue((int) p->red-(int) q->red);
          green=AbsoluteValue((int) p->green-(int) q->green);
          blue=AbsoluteValue((int) p->blue-(int) q->blue);
          index=AbsoluteValue((int) p->index-(int) q->index);
          break;
        }
        case ReplaceCompositeOp:
        {
          red=p->red;
          green=p->green;
          blue=p->blue;
          index=p->index;
          break;
        }
      }
      if (red > MaxRGB)
        q->red=MaxRGB;
      else
        if (red < 0)
          q->red=0;
        else
          q->red=red;
      if (green > MaxRGB)
        q->green=MaxRGB;
      else
        if (green < 0)
          q->green=0;
        else
          q->green=green;
      if (blue > MaxRGB)
        q->blue=MaxRGB;
      else
        if (blue < 0)
          q->blue=0;
        else
          q->blue=blue;
      if (index > 255)
        q->index=255;
      else
        if (index < 0)
          q->index=0;
        else
          q->index=index;
      q->length=0;
      q++;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o p y I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CopyImage returns a copy of all fields of the input image.  The
%  the pixel memory is allocated but the pixel data is not copied.
%
%  The format of the CopyImage routine is:
%
%      copy_image=CopyImage(image,columns,rows,copy_pixels)
%
%  A description of each parameter follows:
%
%    o copy_image: Function CopyImage returns a pointer to the image after
%      copying.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the copied
%      image.
%
%    o rows: An integer that specifies the number of rows in the copied
%      image.
%
%    o copy_pixels: Specifies whether the pixel data is copied.  Must be
%      either True or False;
%
%
*/
Image *CopyImage(image,columns,rows,copy_pixels)
Image
  *image;

unsigned int
  columns,
  rows,
  copy_pixels;
{
  Image
    *copy_image;

  register int
    i;

  /*
    Allocate image structure.
  */
  copy_image=(Image *) malloc(sizeof(Image));
  if (copy_image == (Image *) NULL)
    return((Image *) NULL);
  *copy_image=(*image);
  if (image->comments != (char *) NULL)
    {
      /*
        Allocate and copy the image comments.
      */
      copy_image->comments=(char *)
        malloc(((strlen(image->comments)+1)*sizeof(char)));
      if (copy_image->comments == (char *) NULL)
        return((Image *) NULL);
      (void) strcpy(copy_image->comments,image->comments);
    }
  if (image->label != (char *) NULL)
    {
      /*
        Allocate and copy the image label.
      */
      copy_image->label=(char *)
        malloc(((strlen(image->label)+1)*sizeof(char)));
      if (copy_image->label == (char *) NULL)
        return((Image *) NULL);
      (void) strcpy(copy_image->label,image->label);
    }
  copy_image->columns=columns;
  copy_image->rows=rows;
  if (image->montage != (char *) NULL)
    if ((image->columns != columns) || (image->rows != rows))
      copy_image->montage=(char *) NULL;
    else
      {
        /*
          Allocate and copy the image montage.
        */
        copy_image->montage=(char *)
          malloc(((strlen(image->montage)+1)*sizeof(char)));
        if (copy_image->montage == (char *) NULL)
          return((Image *) NULL);
        (void) strcpy(copy_image->montage,image->montage);
      }
  if (image->directory != (char *) NULL)
    if ((image->columns != columns) || (image->rows != rows))
      copy_image->directory=(char *) NULL;
    else
      {
        /*
          Allocate and copy the image directory.
        */
        copy_image->directory=(char *)
          malloc(((strlen(image->directory)+1)*sizeof(char)));
        if (copy_image->directory == (char *) NULL)
          return((Image *) NULL);
        (void) strcpy(copy_image->directory,image->directory);
      }
  if (image->colormap != (ColorPacket *) NULL)
    {
      /*
        Allocate and copy the image colormap.
      */
      copy_image->colormap=(ColorPacket *)
        malloc(image->colors*sizeof(ColorPacket));
      if (copy_image->colormap == (ColorPacket *) NULL)
        return((Image *) NULL);
      for (i=0; i < image->colors; i++)
        copy_image->colormap[i]=image->colormap[i];
    }
  if (image->signature != (char *) NULL)
    {
      /*
        Allocate and copy the image signature.
      */
      copy_image->signature=(char *)
        malloc(((strlen(image->signature)+1)*sizeof(char)));
      if (copy_image->signature == (char *) NULL)
        return((Image *) NULL);
      (void) strcpy(copy_image->signature,image->signature);
    }
  /*
    Allocate the image pixels.
  */
  if (!copy_pixels)
    copy_image->packets=copy_image->columns*copy_image->rows;
  copy_image->pixels=(RunlengthPacket *)
    malloc((unsigned int) copy_image->packets*sizeof(RunlengthPacket));
  if (copy_image->pixels == (RunlengthPacket *) NULL)
    return((Image *) NULL);
  if (copy_pixels)
    {
      register RunlengthPacket
        *p,
        *q;

      if ((image->columns != columns) || (image->rows != rows))
        return((Image *) NULL);
      /*
        Copy the image pixels.
      */
      p=image->pixels;
      q=copy_image->pixels;
      for (i=0; i < image->packets; i++)
        *q++=(*p++);
    }
  if (!image->orphan)
    {
      /*
        Link image into image list.
      */
      if (copy_image->last != (Image *) NULL)
        copy_image->last->next=copy_image;
      if (copy_image->next != (Image *) NULL)
        copy_image->next->last=copy_image;
    }
  return(copy_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function DestroyImage deallocates memory associated with an image.
%
%  The format of the DestroyImage routine is:
%
%      DestroyImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void DestroyImage(image)
Image
  *image;
{
  if (image->file != (FILE *) NULL)
    if (image->file != stdin)
      if (((int) strlen(image->filename) < 3) ||
          (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0))
        (void) fclose(image->file);
      else
        (void) pclose(image->file);
  /*
    Deallocate the image comments.
  */
  if (image->comments != (char *) NULL)
    (void) free((char *) image->comments);
  /*
    Deallocate the image label.
  */
  if (image->label != (char *) NULL)
    (void) free((char *) image->label);
  /*
    Deallocate the image montage directory.
  */
  if (image->montage != (char *) NULL)
    (void) free((char *) image->montage);
  if (image->directory != (char *) NULL)
    (void) free((char *) image->directory);
  /*
    Deallocate the image colormap.
  */
  if (image->colormap != (ColorPacket *) NULL)
    (void) free((char *) image->colormap);
  /*
    Deallocate the image signature.
  */
  if (image->signature != (char *) NULL)
    (void) free((char *) image->signature);
  /*
    Deallocate the image pixels.
  */
  if (image->pixels != (RunlengthPacket *) NULL)
    (void) free((char *) image->pixels);
  if (image->packed_pixels != (unsigned char *) NULL)
    (void) free((char *) image->packed_pixels);
  /*
    Deallocate the image structure.
  */
  (void) free((char *) image);
  image=(Image *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y I m a g e s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function DestroyImages deallocates memory associated with a linked list
%  of images.
%
%  The format of the DestroyImages routine is:
%
%      DestroyImages(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void DestroyImages(image)
Image
  *image;
{
  Image
    *next_image;

  /*
    Proceed to the top of the image list.
  */
  while (image->last != (Image *) NULL)
    image=image->last;
  do
  {
    /*
      Destroy this image.
    */
    next_image=image->next;
    DestroyImage(image);
    image=next_image;
  } while (image != (Image *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E n h a n c e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function EnhanceImage creates a new image that is a copy of an existing
%  one with the noise reduced.  It allocates the memory necessary for the new
%  Image structure and returns a pointer to the new image.
%
%  EnhanceImage does a weighted average of pixels in a 5x5 cell around each
%  target pixel.  Only pixels in the 5x5 cell that are within a RGB distance
%  threshold of the target pixel are averaged.
%
%  Weights assume that the importance of neighboring pixels is inversely
%  proportional to the square of their distance from the target pixel.
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the EnhanceImage routine is:
%
%      enhanced_image=EnhanceImage(image)
%
%  A description of each parameter follows:
%
%    o enhanced_image: Function EnhanceImage returns a pointer to the image
%      after it is enhanced.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Image *EnhanceImage(image)
Image
  *image;
{
#define Esum(weight) \
  red_distance=s->red-red; \
  green_distance=s->green-green; \
  blue_distance=s->blue-blue; \
  distance=red_distance*red_distance+green_distance*green_distance+ \
    blue_distance*blue_distance; \
  if (distance < Threshold) \
    { \
      total_red+=weight*(s->red); \
      total_green+=weight*(s->green); \
      total_blue+=weight*(s->blue); \
      total_weight+=weight; \
    } \
  s++;
#define Threshold  2500

  ColorPacket
    *scanline;

  Image
    *enhanced_image;

  int
    blue_distance,
    green_distance,
    red_distance;

  register ColorPacket
    *s,
    *s0,
    *s1,
    *s2,
    *s3,
    *s4;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned char
    blue,
    green,
    red;

  unsigned int
    y;

  unsigned long
    distance,
    total_blue,
    total_green,
    total_red,
    total_weight;

  if ((image->columns < 5) || (image->rows < 5))
    {
      Warning("unable to enhance image","image size must exceed 4x4");
      return((Image *) NULL);
    }
  /*
    Initialize enhanced image attributes.
  */
  enhanced_image=CopyImage(image,image->columns,image->rows,False);
  if (enhanced_image == (Image *) NULL)
    {
      Warning("unable to enhance image","memory allocation failed");
      return((Image *) NULL);
    }
  enhanced_image->class=DirectClass;
  /*
    Allocate scan line buffer for 5 rows of the image.
  */
  scanline=(ColorPacket *) malloc(5*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to enhance image","memory allocation failed");
      DestroyImage(enhanced_image);
      return((Image *) NULL);
    }
  /*
    Read the first 4 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns*4); x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Dump first 2 scanlines of image.
  */
  q=enhanced_image->pixels;
  s=scanline;
  for (x=0; x < (2*image->columns); x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  /*
    Enhance each row.
  */
  for (y=2; y < (image->rows-2); y++)
  {
    /*
      Initialize sliding window pointers.
    */
    s0=scanline+image->columns*((y-2) % 5);
    s1=scanline+image->columns*((y-1) % 5);
    s2=scanline+image->columns*(y % 5);
    s3=scanline+image->columns*((y+1) % 5);
    s4=scanline+image->columns*((y+2) % 5);
    /*
      Read another scan line.
    */
    s=s4;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Transfer first 2 pixels of the scanline.
    */
    s=s2;
    for (x=0; x < 2; x++)
    {
      q->red=0;
      q->green=0;
      q->blue=0;
      q->index=0;
      q->length=0;
      q++;
      s++;
    }
    for (x=2; x < (image->columns-2); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      total_weight=0;
      s=s2+2;
      red=s->red;
      green=s->green;
      blue=s->blue;
      s=s0;
      Esum(5);  Esum(8);  Esum(10); Esum(8);  Esum(5);
      s=s1;
      Esum(8);  Esum(20); Esum(40); Esum(20); Esum(8);
      s=s2;
      Esum(10); Esum(40); Esum(80); Esum(40); Esum(10);
      s=s3;
      Esum(8);  Esum(20); Esum(40); Esum(20); Esum(8);
      s=s4;
      Esum(5);  Esum(8);  Esum(10); Esum(8);  Esum(5);
      q->red=(unsigned char) ((total_red+(total_weight >> 1)-1)/total_weight);
      q->green=
        (unsigned char) ((total_green+(total_weight >> 1)-1)/total_weight);
      q->blue=(unsigned char) ((total_blue+(total_weight >> 1)-1)/total_weight);
      q->index=0;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
      s3++;
      s4++;
    }
    /*
      Transfer last 2 pixels of the scanline.
    */
    s=s2;
    for (x=0; x < 2; x++)
    {
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=s->index;
      q->length=0;
      q++;
      s++;
    }
  }
  /*
    Dump last 2 scanlines of pixels.
  */
  s=scanline+image->columns*(y % 3);
  for (x=0; x < (2*image->columns); x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  (void) free((char *) scanline);
  return(enhanced_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a m m a I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure GammaImage converts the reference image to gamma corrected colors.
%
%  The format of the GammaImage routine is:
%
%      GammaImage(image,gamma)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o gamma: A double precision value indicating the level of gamma
%      correction.
%
%
*/
void GammaImage(image,gamma)
Image
  *image;

double
  gamma;
{
  register int
    i;

  register RunlengthPacket
    *p;

  unsigned char
    gamma_map[MaxRGB+1];

  /*
    Initialize gamma table.
  */
  for (i=0; i <= MaxRGB; i++)
    gamma_map[i]=(unsigned char)
      ((pow((double) i/MaxRGB,1.0/gamma)*MaxRGB)+0.5);
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Gamma-correct DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=gamma_map[p->red];
        p->green=gamma_map[p->green];
        p->blue=gamma_map[p->blue];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Gamma-correct PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=gamma_map[image->colormap[i].red];
        image->colormap[i].green=gamma_map[image->colormap[i].green];
        image->colormap[i].blue=gamma_map[image->colormap[i].blue];
      }
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I n v e r s e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure InverseImage inverses the colors in the reference image.
%
%  The format of the InverseImage routine is:
%
%      InverseImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void InverseImage(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p;

  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Inverse DirectClass packets.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=(~p->red);
        p->green=(~p->green);
        p->blue=(~p->blue);
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Inverse PseudoClass packets.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=(~image->colormap[i].red);
        image->colormap[i].green=(~image->colormap[i].green);
        image->colormap[i].blue=(~image->colormap[i].blue);
      }
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N o i s y I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function NoisyImage creates a new image that is a copy of an existing
%  one with the noise reduced with a noise peak elimination filter.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The principal function of noise peak elimination filter is to smooth the
%  objects within an image without losing edge information and without
%  creating undesired structures.  The central idea of the algorithm is to
%  replace a pixel with its next neighbor in value within a 3 x 3 window,
%  if this pixel has been found to be noise.  A pixel is defined as noise
%  if and only if this pixel is a maximum or minimum within the 3 x 3
%  window.
%
%  The format of the NoisyImage routine is:
%
%      noisy_image=NoisyImage(image)
%
%  A description of each parameter follows:
%
%    o noisy_image: Function NoisyImage returns a pointer to the image after
%      the noise is reduced.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
static int NoisyCompare(x,y)
const void
  *x,
  *y;
{
  ColorPacket
    *color_1,
    *color_2;

  color_1=(ColorPacket *) x;
  color_2=(ColorPacket *) y;
  return((int) Intensity(*color_1)-(int) Intensity(*color_2));
}

Image *NoisyImage(image)
Image
  *image;
{
  ColorPacket
    pixel,
    *scanline,
    window[9];

  Image
    *noisy_image;

  int
    i;

  register ColorPacket
    *s,
    *s0,
    *s1,
    *s2;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    y;

  if ((image->columns < 3) || (image->rows < 3))
    {
      Warning("unable to reduce noise","the image size must exceed 2x2");
      return((Image *) NULL);
    }
  /*
    Initialize noisy image attributes.
  */
  noisy_image=CopyImage(image,image->columns,image->rows,False);
  if (noisy_image == (Image *) NULL)
    {
      Warning("unable to reduce noise","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scanline buffer for 3 rows of the image.
  */
  scanline=(ColorPacket *) malloc(3*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to reduce noise","memory allocation failed");
      DestroyImage(noisy_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 2 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (2*image->columns); x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Dump first scanline of image.
  */
  q=noisy_image->pixels;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  /*
    Reduce noise in each row.
  */
  for (y=1; y < (image->rows-1); y++)
  {
    /*
      Initialize sliding window pointers.
    */
    s0=scanline+image->columns*((y-1) % 3);
    s1=scanline+image->columns*(y % 3);
    s2=scanline+image->columns*((y+1) % 3);
    /*
      Read another scan line.
    */
    s=s2;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    s=s1;
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    for (x=1; x < (image->columns-1); x++)
    {
      /*
        Sort window pixels by increasing intensity.
      */
      s=s0;
      window[0]=(*s++);
      window[1]=(*s++);
      window[2]=(*s++);
      s=s1;
      window[3]=(*s++);
      window[4]=(*s++);
      window[5]=(*s++);
      s=s2;
      window[6]=(*s++);
      window[7]=(*s++);
      window[8]=(*s++);
      pixel=window[4];
      (void) qsort((void *) window,9,sizeof(ColorPacket),NoisyCompare);
      if (Intensity(pixel) == Intensity(window[0]))
        {
          /*
            Pixel is minimum noise; replace with next neighbor in value.
          */
          for (i=1; i < 8; i++)
            if (Intensity(window[i]) != Intensity(window[0]))
              break;
          pixel=window[i];
        }
      else
        if (Intensity(pixel) == Intensity(window[8]))
          {
            /*
              Pixel is maximum noise; replace with next neighbor in value.
            */
            for (i=7; i > 0; i--)
              if (Intensity(window[i]) != Intensity(window[8]))
                break;
            pixel=window[i];
          }
      q->red=pixel.red;
      q->green=pixel.green;
      q->blue=pixel.blue;
      q->index=pixel.index;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
    }
    /*
      Transfer last pixel of the scanline.
    */
    s=s1;
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  s=scanline+image->columns*(y % 3);
  for (x=0; x < image->columns; x++)
  {
    q->red=s->red;
    q->green=s->green;
    q->blue=s->blue;
    q->index=s->index;
    q->length=0;
    q++;
    s++;
  }
  (void) free((char *) scanline);
  return(noisy_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N o r m a l i z e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure NormalizeImage normalizes the pixel values to span the full
%  range of color values.  This is a contrast enhancement technique.
%
%  The format of the NormalizeImage routine is:
%
%      NormalizeImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void NormalizeImage(image)
Image
  *image;
{
  int
    histogram[MaxRGB+1],
    threshold_intensity;

  register int
    i,
    intensity;

  register RunlengthPacket
    *p;

  unsigned char
    gray_value,
    high,
    low,
    normalize_map[MaxRGB+1];

  /*
    Form histogram.
  */
  for (i=0; i <= MaxRGB; i++)
    histogram[i]=0;
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    gray_value=Intensity(*p);
    histogram[gray_value]+=p->length+1;
    p++;
  }
  /*
    Find the histogram boundaries by locating the 1 percent levels.
  */
  threshold_intensity=(image->columns*image->rows)/100;
  intensity=0;
  for (low=0; low < MaxRGB; low++)
  {
    intensity+=histogram[low];
    if (intensity > threshold_intensity)
      break;
  }
  intensity=0;
  for (high=MaxRGB; high != 0; high--)
  {
    intensity+=histogram[high];
    if (intensity > threshold_intensity)
      break;
  }
  if (low == high)
    {
      /*
        Unreasonable contrast;  use zero threshold to determine boundaries.
      */
      threshold_intensity=0;
      intensity=0;
      for (low=0; low < MaxRGB; low++)
      {
        intensity+=histogram[low];
        if (intensity > threshold_intensity)
          break;
      }
      intensity=0;
      for (high=MaxRGB; high != 0; high--)
      {
        intensity+=histogram[high];
        if (intensity > threshold_intensity)
          break;
      }
      if (low == high)
        return;  /* zero span bound */
    }
  /*
    Stretch the histogram to create the normalized image mapping.
  */
  for (i=0; i <= MaxRGB; i++)
    if (i < (int) low)
      normalize_map[i]=0;
    else
      if (i > (int) high)
        normalize_map[i]=MaxRGB-1;
      else
        normalize_map[i]=(MaxRGB-1)*(i-(int) low)/(int) (high-low);
  /*
    Normalize the image.
  */
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Normalize DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=normalize_map[p->red];
        p->green=normalize_map[p->green];
        p->blue=normalize_map[p->blue];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Normalize PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=normalize_map[image->colormap[i].red];
        image->colormap[i].green=normalize_map[image->colormap[i].green];
        image->colormap[i].blue=normalize_map[image->colormap[i].blue];
      }
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   O p e n I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function OpenImage open a file associated with the image.
%
%  The format of the OpenImage routine is:
%
%      OpenImage(image,type)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o type: 'r' for reading; 'w' for writing.
%
*/
void OpenImage(image,type)
Image
  *image;

char
  *type;
{
  if (*image->filename == '-')
    image->file=(*type == 'r') ? stdin : stdout;
  else
    if (((int) strlen(image->filename) < 3) ||
        (strcmp(image->filename+strlen(image->filename)-2,".Z") != 0))
      image->file=fopen(image->filename,type);
    else
      {
        char
          command[2048];

        /*
          Image file is compressed-- uncompress it.
        */
        if (*type == 'r')
          (void) sprintf(command,"uncompress -c %s\0",image->filename);
        else
          (void) sprintf(command,"compress -c > %s\0",image->filename);
        image->file=(FILE *) popen(command,type);
      }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a c k I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function PackImage packs the runlength-encoded pixel packets into the
%  minimum number of bytes.
%
%  The format of the PackImage routine is:
%
%      status=PackImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function PackImage return True if the image is packed.  False
%      is returned if an error occurs.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int PackImage(image)
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

  unsigned long int
    count,
    packets;

  if (image->pixels == (RunlengthPacket *) NULL)
    {
      Warning("unable to pack pixels","no image pixels");
      return(False);
    }
  /*
    Runlength-encode only if it consumes less memory than no compression.
  */
  if (image->compression == RunlengthEncodedCompression)
    if (image->class == DirectClass)
      {
        if (image->packets >= ((image->columns*image->rows*3) >> 2))
          image->compression=NoCompression;
      }
    else
      if (image->packets >= ((image->columns*image->rows) >> 1))
        image->compression=NoCompression;
  /*
    Determine packed packet size.
  */
  if (image->class == PseudoClass)
    {
      image->packet_size=1;
      if (image->colors > 256)
        image->packet_size++;
    }
  else
    {
      image->packet_size=3;
      if (image->alpha)
        image->packet_size++;
    }
  if (image->compression == RunlengthEncodedCompression)
    image->packet_size++;
  /*
    Allocate packed pixel memory.
  */
  if (image->packed_pixels != (unsigned char *) NULL)
    (void) free((char *) image->packed_pixels);
  packets=image->packets;
  if (image->compression != RunlengthEncodedCompression)
    packets=image->columns*image->rows;
  image->packed_pixels=(unsigned char *)
    malloc((unsigned int) packets*image->packet_size*sizeof(unsigned char));
  if (image->packed_pixels == (unsigned char *) NULL)
    {
      Warning("unable to pack pixels","memory allocation failed");
      return(False);
    }
  /*
    Packs the runlength-encoded pixel packets into the minimum number of bytes.
  */
  p=image->pixels;
  q=image->packed_pixels;
  count=0;
  if (image->class == DirectClass)
    {
      register int
        alpha;

      alpha=image->alpha;
      if (image->compression == RunlengthEncodedCompression)
        for (i=0; i < image->packets; i++)
        {
          *q++=p->red;
          *q++=p->green;
          *q++=p->blue;
          if (alpha)
            *q++=(unsigned char) p->index;
          *q++=p->length;
          count+=(p->length+1);
          p++;
        }
      else
        for (i=0; i < image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
          {
            *q++=p->red;
            *q++=p->green;
            *q++=p->blue;
            if (alpha)
              *q++=(unsigned char) p->index;
          }
          count+=(p->length+1);
          p++;
        }
    }
  else
    if (image->compression == RunlengthEncodedCompression)
      {
        if (image->colors <= 256)
          for (i=0; i < image->packets; i++)
          {
            *q++=(unsigned char) p->index;
            *q++=p->length;
            count+=(p->length+1);
            p++;
          }
        else
          for (i=0; i < image->packets; i++)
          {
            *q++=(unsigned char) (p->index >> 8);
            *q++=(unsigned char) p->index;
            *q++=p->length;
            count+=(p->length+1);
            p++;
          }
      }
    else
      if (image->colors <= 256)
        for (i=0; i < image->packets; i++)
        {
          for (j=0; j <= ((int) p->length); j++)
            *q++=(unsigned char) p->index;
          count+=(p->length+1);
          p++;
        }
      else
        {
          register unsigned char
            xff00,
            xff;

          for (i=0; i < image->packets; i++)
          {
            xff00=(unsigned char) (p->index >> 8);
            xff=(unsigned char) p->index;
            for (j=0; j <= ((int) p->length); j++)
            {
              *q++=xff00;
              *q++=xff;
            }
            count+=(p->length+1);
            p++;
          }
        }
  /*
    Guarentee the correct number of pixel packets.
  */
  if (count < (image->columns*image->rows))
    {
      Warning("insufficient image data in",image->filename);
      return(False);
    }
  else
    if (count > (image->columns*image->rows))
      {
        Warning("too much image data in",image->filename);
        return(False);
      }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r i n t I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function PrintImage translates a MIFF image to encapsulated Postscript for
%  printing.  If the supplied geometry is null, the image is centered on the
%  Postscript page.  Otherwise, the image is positioned as specified by the
%  geometry.
%
%  The format of the PrintImage routine is:
%
%      status=PrintImage(image,geometry)
%
%  A description of each parameter follows:
%
%    o status: Function PrintImage return True if the image is printed.
%      False is returned if the image file cannot be opened for printing.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o geometry: A pointer to a standard window geometry string.
%
%
*/
unsigned int PrintImage(image,geometry)
Image
  *image;

char
  *geometry;
{
#define PageSideMargin 16
#define PageTopMargin 92
#define PageWidth  612
#define PageHeight 792

  static char
    *Postscript[]=
    {
      "%",
      "% Display a color image.  The image is displayed in color on",
      "% Postscript viewers or printers that support color, otherwise",
      "% it is displayed as grayscale.",
      "%",
      "/buffer 512 string def",
      "/byte 1 string def",
      "/color_packet 3 string def",
      "/compression 1 string def",
      "/gray_packet 1 string def",
      "/pixels 768 string def",
      "",
      "/DirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet.",
      "  %",
      "  % Parameters: ",
      "  %   red.",
      "  %   green.",
      "  %   blue.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/DirectClassImage",
      "{",
      "  %",
      "  % Display a DirectClass image.",
      "  %",
      "  systemdict /colorimage known",
      "  {",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { DirectClassPacket } false 3 colorimage",
      "  }",
      "  {",
      "    %",
      "    % No colorimage operator;  convert to grayscale.",
      "    %",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { GrayDirectClassPacket } image",
      "  } ifelse",
      "} bind def",
      "",
      "/GrayDirectClassPacket",
      "{",
      "  %",
      "  % Get a DirectClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters: ",
      "  %   red",
      "  %   green",
      "  %   blue",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile color_packet readhexstring pop pop",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/GrayPseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet;  convert to grayscale.",
      "  %",
      "  % Parameters: ",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  color_packet 0 get 0.299 mul",
      "  color_packet 1 get 0.587 mul add",
      "  color_packet 2 get 0.114 mul add",
      "  cvi",
      "  /gray_packet exch def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 1 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add def",
      "  } ifelse",
      "  0 1 number_pixels 1 sub",
      "  {",
      "    pixels exch gray_packet put",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassPacket",
      "{",
      "  %",
      "  % Get a PseudoClass packet.",
      "  %",
      "  % Parameters: ",
      "  %   index: index into the colormap.",
      "  %   length: number of pixels minus one of this color (optional).",
      "  %",
      "  currentfile byte readhexstring pop 0 get",
      "  /offset exch 3 mul def",
      "  /color_packet colormap offset 3 getinterval def",
      "  compression 0 gt",
      "  {",
      "    /number_pixels 3 def",
      "  }",
      "  {",
      "    currentfile byte readhexstring pop 0 get",
      "    /number_pixels exch 1 add 3 mul def",
      "  } ifelse",
      "  0 3 number_pixels 1 sub",
      "  {",
      "    pixels exch color_packet putinterval",
      "  } for",
      "  pixels 0 number_pixels getinterval",
      "} bind def",
      "",
      "/PseudoClassImage",
      "{",
      "  %",
      "  % Display a PseudoClass image.",
      "  %",
      "  % Parameters: ",
      "  %   colors: number of colors in the colormap.",
      "  %   colormap: red, green, blue color packets.",
      "  %",
      "  currentfile buffer readline pop",
      "  token pop /colors exch def pop",
      "  /colors colors 3 mul def",
      "  /colormap colors string def",
      "  currentfile colormap readhexstring pop pop",
      "  systemdict /colorimage known",
      "  {",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { PseudoClassPacket } false 3 colorimage",
      "  }",
      "  {",
      "    %",
      "    % No colorimage operator;  convert to grayscale.",
      "    %",
      "    columns rows 8",
      "    [",
      "      columns 0 0",
      "      rows neg 0 rows",
      "    ]",
      "    { GrayPseudoClassPacket } image",
      "  } ifelse",
      "} bind def",
      "",
      "/DisplayImage",
      "{",
      "  %",
      "  % Display a DirectClass or PseudoClass image.",
      "  %",
      "  % Parameters: ",
      "  %   x & y translation.",
      "  %   x & y scale.",
      "  %   image columns & rows.",
      "  %   class: 0-DirectClass or 1-PseudoClass.",
      "  %   compression: 0-RunlengthEncodedCompression or 1-NoCompression.",
      "  %   hex color packets.",
      "  %",
      "  gsave",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y scale",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "  class 0 gt { PseudoClassImage } { DirectClassImage } ifelse",
      "  grestore",
      "  showpage",
      "} bind def",
      "",
      "DisplayImage",
      NULL
    };

  char
    **q;

  int
    x,
    y;

  register RunlengthPacket
    *p;

  register int
    i,
    j;

  unsigned int
    height,
    width;

  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      return(False);
    }
  if (geometry != (char *) NULL)
    {
      /*
        User specified Postscript page position.
      */
      x=0;
      y=0;
      width=image->columns;
      height=image->rows;
      (void) XParseGeometry(geometry,&x,&y,&width,&height);
    }
  else
    {
      int
        delta_x,
        delta_y;

      unsigned long
        scale_factor;

      /*
        Scale image to size of Postscript page.
      */
      scale_factor=UpShift(PageWidth-(2*PageSideMargin))/image->columns;
      if (scale_factor > (UpShift(PageHeight-(2*PageTopMargin))/image->rows))
        scale_factor=UpShift(PageHeight-(2*PageTopMargin))/image->rows;
      width=DownShift(image->columns*scale_factor);
      height=DownShift(image->rows*scale_factor);
      /*
        Center image on Postscript page.
      */
      delta_x=PageWidth-(width+(2*PageSideMargin));
      delta_y=PageHeight-(height+(2*PageTopMargin));
      if (delta_x >= 0)
        x=delta_x/2+PageSideMargin;
      else
        x=PageSideMargin;
      if (delta_y >= 0)
        y=delta_y/2+PageTopMargin;
      else
        y=PageTopMargin;
    }
  /*
    Output encapsulated Postscript header.
  */
  (void) fprintf(image->file,"%%!PS-Adobe-3.0 EPSF-2.0\n");
  (void) fprintf(image->file,"%%%%Title: %s\n",image->filename);
  (void) fprintf(image->file,"%%%%Creator: ImageMagick\n");
  (void) fprintf(image->file,"%%%%BoundingBox: %d %d %d %d\n",x,y,x+(int) width,
    y+(int) height);
  (void) fprintf(image->file,"%%%%EndComments\n");
  /*
    Output encapsulated Postscript commands.
  */
  for (q=Postscript; *q; q++)
    (void) fprintf(image->file,"%s\n",*q);
  /*
    Output image data.
  */
  if (image->compression == RunlengthEncodedCompression)
    CompressImage(image);
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    {
      (void) fprintf(image->file,"%d %d\n%u %u\n%u %u\n%d\n%d\n",x,y,width,
        height,image->columns,image->rows,(image->class == PseudoClass),
        image->compression == NoCompression);
      switch (image->compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Dump runlength-encoded DirectColor packets.
          */
          x=0;
          for (i=0; i < image->packets; i++)
          {
            x++;
            (void) fprintf(image->file,"%02x%02x%02x%02x",p->red,p->green,
              p->blue,p->length);
            if (x == 9)
              {
                x=0;
                (void) fprintf(image->file,"\n");
              }
            p++;
          }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed DirectColor packets.
          */
          x=0;
          for (i=0; i < image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              x++;
              (void) fprintf(image->file,"%02x%02x%02x",p->red,p->green,
                p->blue);
              if (x == 12)
                {
                  x=0;
                  (void) fprintf(image->file,"\n");
                }
            }
            p++;
          }
          break;
        }
      }
      break;
    }
    case PseudoClass:
    {
      (void) fprintf(image->file,"%d %d\n%u %u\n%u %u\n%d\n%d\n",x,y,width,
        height,image->columns,image->rows,(image->class == PseudoClass),
        image->compression == NoCompression);
      /*
        Dump number of colors and colormap.
      */
      (void) fprintf(image->file,"%u\n",image->colors);
      for (i=0; i < image->colors; i++)
        (void) fprintf(image->file,"%02x%02x%02x\n",image->colormap[i].red,
          image->colormap[i].green,image->colormap[i].blue);
      switch (image->compression)
      {
        case RunlengthEncodedCompression:
        default:
        {
          /*
            Dump runlength-encoded PseudoColor packets.
          */
          x=0;
          for (i=0; i < image->packets; i++)
          {
            x++;
            (void) fprintf(image->file,"%02x%02x",p->index,p->length);
            if (x == 18)
              {
                x=0;
                (void) fprintf(image->file,"\n");
              }
            p++;
          }
          break;
        }
        case NoCompression:
        {
          /*
            Dump uncompressed PseudoColor packets.
          */
          x=0;
          for (i=0; i < image->packets; i++)
          {
            for (j=0; j <= ((int) p->length); j++)
            {
              x++;
              (void) fprintf(image->file,"%02x",p->index);
              if (x == 36)
                {
                  x=0;
                  (void) fprintf(image->file,"\n");
                }
            }
            p++;
          }
        }
        break;
      }
    }
  }
  (void) fprintf(image->file,"\n\n");
  (void) fprintf(image->file,"%%%%Trailer\n");
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d D a t a                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadData reads data from the image file and returns it.  If it
%  cannot read the requested number of items, False is returned indicating
%  an error.
%
%  The format of the ReadData routine is:
%
%      status=ReadData(data,size,number_items,file)
%
%  A description of each parameter follows:
%
%    o status:  Function ReadData returns True if all the data requested
%      is obtained without error, otherwise False.
%
%    o data:  Specifies an area to place the information reuested from
%      the file.
%
%    o size:  Specifies an integer representing the length of an
%      individual item to be read from the file.
%
%    o number_items:  Specifies an integer representing the number of items
%      to read from the file.
%
%    o file:  Specifies a file to read the data.
%
%
*/
unsigned int ReadData(data,size,number_items,file)
char
  *data;

int
  size,
  number_items;

FILE
  *file;
{
  size*=number_items;
  while (size > 0)
  {
    number_items=fread(data,1,size,file);
    if (number_items <= 0)
      return(False);
    size-=number_items;
    data+=number_items;
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d D a t a B l o c k                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadDataBlock reads data from the image file and returns it.  The
%  amount of data is determined by first reading a count byte.  If
%  ReadDataBlock cannot read the requested number of items, `-1' is returned
%  indicating an error.
%
%  The format of the ReadData routine is:
%
%      status=ReadData(data,file)
%
%  A description of each parameter follows:
%
%    o status:  Function ReadData returns the number of characters read
%      unless the is an error, otherwise `-1'.
%
%    o data:  Specifies an area to place the information reuested from
%      the file.
%
%    o file:  Specifies a file to read the data.
%
%
*/
int ReadDataBlock(data,file)
char
  *data;

FILE
  *file;
{
  unsigned char
    count;

  unsigned int
    status;

  status=ReadData((char *) &count,1,1,file);
  if (status == False)
    return(-1);
  if (count == 0)
    return(0);
  status=ReadData(data,1,(int) count,file);
  if (status == False)
    return(-1);
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadImage reads an image file and returns it.  It allocates the
%  memory necessary for the new Image structure and returns a pointer to the
%  new image.
%
%  The format of the ReadImage routine is:
%
%      image=ReadImage(filename)
%
%  A description of each parameter follows:
%
%    o image: Function ReadImage returns a pointer to the image after reading.
%      A null image is returned if there is a a memory shortage or if the
%      image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%
*/
Image *ReadImage(filename)
char
  *filename;
{
#define MaxKeywordLength  2048

  char
    keyword[MaxKeywordLength],
    value[MaxKeywordLength];

  Image
    *image;

  register int
    c,
    i;

  register unsigned char
    *p;

  unsigned int
    max_characters,
    packet_size,
    status;

  unsigned long int
    count,
    packets;

  /*
    Allocate image structure.
  */
  image=AllocateImage("MIFF");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  c=fgetc(image->file);
  if (c == EOF)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    image->compression=NoCompression;
    while (isgraph(c) && (c != ':'))
    {
      register char
        *p;

      if (c == '{')
        {
          /*
            Comment.
          */
          max_characters=2048;
          image->comments=(char *) malloc(max_characters*sizeof(char));
          if (image->comments == (char *) NULL)
            {
              Warning("unable to read image","memory allocation failed");
              DestroyImages(image);
              return((Image *) NULL);
            }
          p=image->comments;
          *p='\0';
          c=fgetc(image->file);
          while ((isgraph(c) || isspace(c)) && (c != '}'))
          {
            if (p >= (image->comments+max_characters-1))
              {
                /*
                  Allocate more memory for the comment.
                */
                max_characters<<=1;
                image->comments=(char *)
                  realloc((char *) image->comments,max_characters);
                if (image->comments == (char *) NULL)
                  {
                    Warning("unable to read image","memory allocation failed");
                    DestroyImages(image);
                    return((Image *) NULL);
                  }
                p=image->comments+strlen(image->comments);
              }
            *p++=(unsigned char) c;
            c=fgetc(image->file);
          }
          *p='\0';
          c=fgetc(image->file);
        }
      else
        if (isalnum(c))
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
            } while (isalnum(c));
            *p='\0';
            while (isspace(c) || (c == '='))
              c=fgetc(image->file);
            p=value;
            while (!isspace(c))
            {
              if ((p-value) < (MaxKeywordLength-1))
                *p++=(char) c;
              c=fgetc(image->file);
            }
            *p='\0';
            /*
              Assign a value to the specified keyword.
            */
            if (strcmp(keyword,"alpha") == 0)
              if ((strcmp(value,"True") == 0) || (strcmp(value,"true") == 0))
                image->alpha=True;
              else
                image->class=False;
            if (strcmp(keyword,"class") == 0)
              if (strcmp(value,"PseudoClass") == 0)
                image->class=PseudoClass;
              else
                if (strcmp(value,"DirectClass") == 0)
                  image->class=DirectClass;
                else
                  image->class=UndefinedClass;
            if (strcmp(keyword,"colors") == 0)
              image->colors=(unsigned int) atoi(value);
            if (strcmp(keyword,"compression") == 0)
              if (strcmp(value,"QEncoded") == 0)
                image->compression=QEncodedCompression;
              else
                if (strcmp(value,"RunlengthEncoded") == 0)
                  image->compression=RunlengthEncodedCompression;
                else
                  image->compression=UndefinedCompression;
            if (strcmp(keyword,"columns") == 0)
              image->columns=(unsigned int) atoi(value);
            if (strcmp(keyword,"id") == 0)
              if (strcmp(value,"ImageMagick") == 0)
                image->id=ImageMagickId;
              else
                image->id=UndefinedId;
            if (strcmp(keyword,"montage") == 0)
              {
                image->montage=(char *) malloc(strlen(value)+1*sizeof(char));
                if (image->montage == (char *) NULL)
                  {
                    Warning("unable to read image","memory allocation failed");
                    DestroyImages(image);
                    return((Image *) NULL);
                  }
                (void) strcpy(image->montage,value);
              }
            if (strcmp(keyword,"packets") == 0)
              image->packets=(unsigned int) atoi(value);
            if (strcmp(keyword,"rows") == 0)
              image->rows=(unsigned int) atoi(value);
            if (strcmp(keyword,"scene") == 0)
              image->scene=(unsigned int) atoi(value);
            if (strcmp(keyword,"signature") == 0)
              {
                image->signature=(char *)
                  malloc((strlen(value)+1)*sizeof(char));
                if (image->signature == (char *) NULL)
                  {
                    Warning("unable to read image","memory allocation failed");
                    DestroyImages(image);
                    return((Image *) NULL);
                  }
                (void) strcpy(image->signature,value);
              }
          }
        else
          c=fgetc(image->file);
      while (isspace(c))
        c=fgetc(image->file);
    }
    (void) fgetc(image->file);
    /*
      Verify that required image information is defined.
    */
    if ((image->id == UndefinedId) || (image->class == UndefinedClass) ||
        (image->compression == UndefinedCompression) || (image->columns == 0) ||
        (image->rows == 0))
      {
        Warning("incorrect image header in file",image->filename);
        DestroyImages(image);
        return((Image *) NULL);
      }
    if ((image->columns*image->rows) > MaxImageSize)
      {
        Warning("unable to read image","image size too large");
        DestroyImages(image);
        return((Image *) NULL);
      }
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Tiling directory.
        */
        max_characters=2048;
        image->directory=(char *) malloc(max_characters*sizeof(char));
        if (image->directory == (char *) NULL)
          {
            Warning("unable to read image","memory allocation failed");
            DestroyImages(image);
            return((Image *) NULL);
          }
        p=image->directory;
        do
        {
          if (p >= (image->directory+max_characters-1))
            {
              /*
                Allocate more memory for the comment.
              */
              max_characters<<=1;
              image->directory=(char *)
                realloc((char *) image->directory,max_characters);
              if (image->directory == (char *) NULL)
                {
                  Warning("unable to read image","memory allocation failed");
                  DestroyImages(image);
                  return((Image *) NULL);
                }
              p=image->comments+strlen(image->comments);
            }
          c=fgetc(image->file);
          *p++=(unsigned char) c;
        } while (c != '\0');
      }
    if (image->class == PseudoClass)
      {
        unsigned int
          colors;

        /*
          PseudoClass image cannot have alpha data or be QEncoded.
        */
        if (image->alpha)
          {
            Warning("unable to read image","alpha images must be DirectClass");
            DestroyImages(image);
            return((Image *) NULL);
          }
        if (image->compression == QEncodedCompression)
          {
            Warning("unable to read image",
              "QEncoded images must be DirectClass");
            DestroyImages(image);
            return((Image *) NULL);
          }
        /*
          Create image colormap.
        */
        colors=image->colors;
        if (colors == 0)
          colors=256;
        image->colormap=(ColorPacket *) malloc(colors*sizeof(ColorPacket));
        if (image->colormap == (ColorPacket *) NULL)
          {
            Warning("unable to read image","memory allocation failed");
            DestroyImages(image);
            return((Image *) NULL);
          }
        if (image->colors == 0)
          for (i=0; i < colors; i++)
          {
            image->colormap[i].red=(unsigned char) i;
            image->colormap[i].green=(unsigned char) i;
            image->colormap[i].blue=(unsigned char) i;
            image->colors++;
          }
        else
          {
            unsigned char
              *colormap;

            /*
              Read image colormap from file.
            */
            colormap=(unsigned char *)
              malloc(3*image->colors*sizeof(unsigned char));
            if (colormap == (unsigned char *) NULL)
              {
                Warning("unable to read image","memory allocation failed");
                DestroyImages(image);
                return((Image *) NULL);
              }
            (void) ReadData((char *) colormap,1,(int) (3*image->colors),
              image->file);
            p=colormap;
            for (i=0; i < image->colors; i++)
            {
              image->colormap[i].red=(*p++);
              image->colormap[i].green=(*p++);
              image->colormap[i].blue=(*p++);
            }
            (void) free((char *) colormap);
          }
      }
    /*
      Determine packed packet size.
    */
    if (image->class == PseudoClass)
      {
        image->packet_size=1;
        if (image->colors > 256)
          image->packet_size++;
      }
    else
      {
        image->packet_size=3;
        if (image->alpha)
          image->packet_size++;
      }
    if (image->compression == RunlengthEncodedCompression)
      image->packet_size++;
    packet_size=image->packet_size;
    if (image->compression == QEncodedCompression)
      packet_size=1;
    /*
      Allocate image pixels.
    */
    if (image->compression == NoCompression)
      image->packets=image->columns*image->rows;
    packets=image->packets;
    if (image->packets == 0)
      packets=image->columns*image->rows;
    image->packed_pixels=(unsigned char *)
      malloc((unsigned int) packets*packet_size*sizeof(unsigned char));
    if (image->packed_pixels == (unsigned char *) NULL)
      {
        Warning("unable to read image","memory allocation failed");
        DestroyImages(image);
        return((Image *) NULL);
      }
    /*
      Read image pixels from file.
    */
    if ((image->compression != RunlengthEncodedCompression) ||
        (image->packets != 0))
      (void) ReadData((char *) image->packed_pixels,1,
        (int) (packets*packet_size),image->file);
    else
      {
        /*
          Number of runlength packets is unspecified.
        */
        count=0;
        p=image->packed_pixels;
        do
        {
          (void) ReadData((char *) p,1,(int) packet_size,image->file);
          image->packets++;
          p+=(packet_size-1);
          count+=(*p+1);
          p++;
        }
        while (count < (image->columns*image->rows));
      }
    if (image->compression ==  QEncodedCompression)
      {
        unsigned char
          *compressed_pixels;

        /*
          Uncompress image pixels with Q encoding.
        */
        image->packets=image->columns*image->rows;
        compressed_pixels=image->packed_pixels;
        image->packed_pixels=(unsigned char *) malloc((unsigned int)
          image->packets*image->packet_size*sizeof(unsigned char));
        if (image->packed_pixels == (unsigned char *) NULL)
          {
            Warning("unable to write image","memory allocation failed");
            DestroyImage(image);
            return(False);
          }
        packets=QDecodeImage(compressed_pixels,image->packed_pixels,
          image->columns*(int) image->packet_size,image->rows);
        if (packets != (image->packets*image->packet_size))
          {
            Warning("Q encoding failed",image->filename);
            DestroyImages(image);
            return((Image *) NULL);
          }
        (void) free((char *) compressed_pixels);
      }
    /*
      Unpack the packed image pixels into runlength-encoded pixel packets.
    */
    status=UnpackImage(image);
    if (status == False)
      {
        DestroyImages(image);
        return((Image *) NULL);
      }
    /*
      Proceed to next image.
    */
    do
    {
      c=fgetc(image->file);
    } while (!isgraph(c) && (c != EOF));
    if (c != EOF)
      {
        /*
          Allocate image structure.
        */
        image->next=AllocateImage("MIFF");
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->file=image->file;
        (void) sprintf(image->next->filename,"%s.%u\0",filename,image->scene+1);
        image->next->scene=image->scene+1;
        image->next->last=image;
        image=image->next;
      }
  } while (c != EOF);
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
%   R e d u c e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReduceImage creates a new image that is a integral size less than
%  an existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  ReduceImage scans the reference image to create a reduced image by computing
%  the weighted average of a 4x4 cell centered at each reference pixel.  The
%  target pixel requires two columns and two rows of the reference pixels.
%  Therefore the reduced image columns and rows become:
%
%    number_columns/2
%    number_rows/2
%
%  Weights assume that the importance of neighboring pixels is inversely
%  proportional to the square of their distance from the target pixel.
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the ReduceImage routine is:
%
%      reduced_image=ReduceImage(image)
%
%  A description of each parameter follows:
%
%    o reduced_image: Function ReduceImage returns a pointer to the image
%      after reducing.  A null image is returned if there is a a memory
%      shortage or if the image size is less than IconSize*2.
%
%    o image: The address of a structure of type Image.
%
%
*/
static Image *ReduceImage(image)
Image
  *image;
{
#define Rsum(weight) \
  total_red+=weight*(s->red); \
  total_green+=weight*(s->green); \
  total_blue+=weight*(s->blue); \
  total_alpha+=weight*(s->index); \
  s++;

  ColorPacket
    *scanline;

  Image
    *reduced_image;

  register ColorPacket
    *s,
    *s0,
    *s1,
    *s2,
    *s3;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    y;

  unsigned long
    total_alpha,
    total_blue,
    total_green,
    total_red;

  if ((image->columns < 4) || (image->rows < 4))
    {
      Warning("unable to reduce image","image size must exceed 3x3");
      return((Image *) NULL);
    }
  /*
    Initialize reduced image attributes.
  */
  reduced_image=CopyImage(image,image->columns >> 1,image->rows >> 1,False);
  if (reduced_image == (Image *) NULL)
    {
      Warning("unable to reduce image","memory allocation failed");
      return((Image *) NULL);
    }
  reduced_image->class=DirectClass;
  /*
    Allocate image buffer and scanline buffer for 4 rows of the image.
  */
  scanline=(ColorPacket *) malloc(4*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to reduce image","memory allocation failed");
      DestroyImage(reduced_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 2 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (2*image->columns); x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Reduce each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=reduced_image->pixels;
  for (y=0; y < (image->rows-1); y+=2)
  {
    /*
      Initialize sliding window pointers.
    */
    s0=scanline+image->columns*((y+0) % 4);
    s1=scanline+image->columns*((y+1) % 4);
    s2=scanline+image->columns*((y+2) % 4);
    s3=scanline+image->columns*((y+3) % 4);
    /*
      Read another scan line.
    */
    s=s2;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Read another scan line.
    */
    s=s3;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    for (x=0; x < (image->columns-1); x+=2)
    {
      /*
        Compute weighted average of target pixel color components.

        These particular coefficients total to 128.  Use 128/2-1 or 63 to
        insure correct round off.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      total_alpha=0;
      s=s0;
      Rsum(3); Rsum(7);  Rsum(7);  Rsum(3);
      s=s1;
      Rsum(7); Rsum(15); Rsum(15); Rsum(7);
      s=s2;
      Rsum(7); Rsum(15); Rsum(15); Rsum(7);
      s=s3;
      Rsum(3); Rsum(7);  Rsum(7);  Rsum(3);
      s0+=2;
      s1+=2;
      s2+=2;
      s3+=2;
      q->red=(unsigned char) ((total_red+63) >> 7);
      q->green=(unsigned char) ((total_green+63) >> 7);
      q->blue=(unsigned char) ((total_blue+63) >> 7);
      q->index=(unsigned char) ((total_alpha+63) >> 7);
      q->length=0;
      q++;
    }
  }
  (void) free((char *) scanline);
  return(reduced_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f l e c t I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReflectImage creates a new image that refelects each scanline of an
%  existing one.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReflectImage routine is:
%
%      reflected_image=ReflectImage(image)
%
%  A description of each parameter follows:
%
%    o reflected_image: Function ReflectImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Image *ReflectImage(image)
Image
  *image;
{
  ColorPacket
    *scanline;

  Image
    *reflected_image;

  register ColorPacket
    *s;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x,
    y;

  /*
    Initialize reflected image attributes.
  */
  reflected_image=CopyImage(image,image->columns,image->rows,False);
  if (reflected_image == (Image *) NULL)
    {
      Warning("unable to reflect image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(ColorPacket *) malloc(image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to reflect image","memory allocation failed");
      DestroyImage(reflected_image);
      return((Image *) NULL);
    }
  /*
    Reflect each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=reflected_image->pixels;
  for (y=0; y < reflected_image->rows; y++)
  {
    /*
      Read a scan line.
    */
    s=scanline;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
    }
    /*
      Reflect each column.
    */
    s=scanline+image->columns;
    for (x=0; x < reflected_image->columns; x++)
    {
      s--;
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=s->index;
      q->length=0;
      q++;
    }
  }
  (void) free((char *) scanline);
  return(reflected_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.
%
%  The format of the RGBTransformImage routine is:
%
%      RGBTransformImage(image,colorspace)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates which colorspace
%      to transform the image.
%
%
*/
void RGBTransformImage(image,colorspace)
Image
  *image;

unsigned int
  colorspace;
{
#define X 0
#define Y (MaxRGB+1)
#define Z (MaxRGB+1)*2

  long int
    tx,
    ty,
    tz,
    *x,
    *y,
    *z;

  register int
    blue,
    green,
    i,
    red;

  register RunlengthPacket
    *p;

  register unsigned char
    *range_limit;

  unsigned char
    *range_table;

  if (colorspace == RGBColorspace)
    return;
  /*
    Allocate the tables.
  */
  x=(long int *) malloc(3*(MaxRGB+1)*sizeof(long int));
  y=(long int *) malloc(3*(MaxRGB+1)*sizeof(long int));
  z=(long int *) malloc(3*(MaxRGB+1)*sizeof(long int));
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if ((x == (long int *) NULL) || (y == (long int *) NULL) ||
      (z == (long int *) NULL) || (range_table == (unsigned char *) NULL))
    {
      Warning("unable to transform color space","memory allocation failed");
      return;
    }
  /*
    Pre-compute conversion tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(unsigned char) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  tx=0;
  ty=0;
  tz=0;
  switch (colorspace)
  {
    case GRAYColorspace:
    {
      /*
        Initialize GRAY tables:

          G = 0.29900*R+0.58700*G+0.11400*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=UpShifted(0.29900)*i;
        y[i+Y]=UpShifted(0.58700)*i;
        z[i+Y]=UpShifted(0.11400)*i;
        x[i+Z]=UpShifted(0.29900)*i;
        y[i+Z]=UpShifted(0.58700)*i;
        z[i+Z]=UpShifted(0.11400)*i;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29900*R+0.58700*G+0.11400*B
          I = 0.59600*R-0.27400*G-0.32200*B
          Q = 0.21100*R-0.52300*G+0.31200*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=UpShifted(0.59600)*i;
        y[i+Y]=(-UpShifted(0.27400))*i;
        z[i+Y]=(-UpShifted(0.32200))*i;
        x[i+Z]=UpShifted(0.21100)*i;
        y[i+Z]=(-UpShifted(0.52300))*i;
        z[i+Z]=UpShifted(0.31200)*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29900*R+0.58700*G+0.11400*B
          U = -0.16874*R-0.33126*G+0.50000*B
          V =  0.50000*R-0.41869*G-0.08131*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1)/2);
      tz=UpShifted((MaxRGB+1)/2);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29900)*i;
        y[i+X]=UpShifted(0.58700)*i;
        z[i+X]=UpShifted(0.11400)*i;
        x[i+Y]=(-UpShifted(0.16874))*i;
        y[i+Y]=(-UpShifted(0.33126))*i;
        z[i+Y]=UpShifted(0.50000)*i;
        x[i+Z]=UpShifted(0.50000)*i;
        y[i+Z]=(-UpShifted(0.41869))*i;
        z[i+Z]=(-UpShifted(0.08131))*i;
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize XYZ tables:

          X = 0.49000*R+0.31000*G+0.20000*B
          Y = 0.17700*R+0.81300*G+0.01100*B
          Z = 0.00000*R+0.01000*G+0.99000*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.49000)*i;
        y[i+X]=UpShifted(0.31000)*i;
        z[i+X]=UpShifted(0.20000)*i;
        x[i+Y]=UpShifted(0.17700)*i;
        y[i+Y]=UpShifted(0.81300)*i;
        z[i+Y]=UpShifted(0.01100)*i;
        x[i+Z]=0;
        y[i+Z]=UpShifted(0.01000)*i;
        z[i+Z]=UpShifted(0.99000)*i;
      }
      break;
    }
  }
  /*
    Convert from RGB.
  */
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Convert DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        red=p->red;
        green=p->green;
        blue=p->blue;
        p->red=range_limit[DownShift(x[red+X]+y[green+X]+z[blue+X]+tx)];
        p->green=range_limit[DownShift(x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        p->blue=range_limit[DownShift(x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
      {
        red=image->colormap[i].red;
        green=image->colormap[i].green;
        blue=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[DownShift(x[red+X]+y[green+X]+z[blue+X]+tx)];
        image->colormap[i].green=
          range_limit[DownShift(x[red+Y]+y[green+Y]+z[blue+Y]+ty)];
        image->colormap[i].blue=
          range_limit[DownShift(x[red+Z]+y[green+Z]+z[blue+Z]+tz)];
      }
      SyncImage(image);
      break;
    }
  }
  /*
    Free allocated memory.
  */
  (void) free((char *) range_table);
  (void) free((char *) z);
  (void) free((char *) y);
  (void) free((char *) x);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o l l I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function RollImage rolls an image vertically and horizontally.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the RollImage routine is:
%
%      rolled_image=RollImage(image,columns,rows)
%
%  A description of each parameter follows:
%
%    o rolled_image: Function RollImage returns a pointer to the image after
%      rolling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o x_offset: An integer that specifies the number of columns to roll
%      in the horizonal direction.
%
%    o y_offset: An integer that specifies the number of rows to roll in the
%      veritical direction.
%
%
*/
Image *RollImage(image,x_offset,y_offset)
Image
  *image;

int
  x_offset,
  y_offset;
{
  Image
    *rolled_image;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    packets,
    x;

  unsigned int
    y;

  /*
    Initialize rolled image attributes.
  */
  rolled_image=CopyImage(image,image->columns,image->rows,False);
  if (rolled_image == (Image *) NULL)
    {
      Warning("unable to roll image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Roll image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  packets=image->columns*image->rows;
  for (y=0; y < image->rows; y++)
  {
    /*
      Transfer scanline.
    */
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength > 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      q=rolled_image->pixels+(y_offset+y)*image->columns+x+x_offset;
      if (q < rolled_image->pixels)
        q+=packets;
      else
        if (q >= (rolled_image->pixels+packets))
          q-=packets;
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q->index=p->index;
      q->length=0;
    }
  }
  return(rolled_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S c a l e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ScaleImage creates a new image that is a scaled size of an
%  existing one using pixel replication.  It allocates the memory necessary
%  for the new Image structure and returns a pointer to the new image.
%
%  The format of the ScaleImage routine is:
%
%      scaled_image=ScaleImage(image,columns,rows)
%
%  A description of each parameter follows:
%
%    o scaled_image: Function ScaleImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the scaled
%      image.
%
%    o rows: An integer that specifies the number of rows in the scaled
%      image.
%
%
*/
Image *ScaleImage(image,columns,rows)
Image
  *image;

unsigned int
  columns,
  rows;
{
  ColorPacket
    *scanline;

  Image
    *scaled_image;

  register ColorPacket
    *s;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    *x_offset,
    y,
    *y_offset;

  unsigned long
    scale_factor;

  if ((columns == 0) || (rows == 0))
    {
      Warning("unable to scale image","image dimensions are zero");
      return((Image *) NULL);
    }
  if ((columns > MaxImageSize) || (rows > MaxImageSize))
    {
      Warning("unable to scale image","image too large");
      return((Image *) NULL);
    }
  /*
    Initialize scaled image attributes.
  */
  scaled_image=CopyImage(image,columns,rows,False);
  if (scaled_image == (Image *) NULL)
    {
      Warning("unable to scale image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(ColorPacket *) malloc(image->columns*sizeof(ColorPacket));
  x_offset=(unsigned int *) malloc(scaled_image->columns*sizeof(unsigned int));
  y_offset=(unsigned int *) malloc(scaled_image->rows*sizeof(unsigned int));
  if ((scanline == (ColorPacket *) NULL) ||
      (x_offset == (unsigned int *) NULL) ||
      (y_offset == (unsigned int *) NULL))
    {
      Warning("unable to scale image","memory allocation failed");
      DestroyImage(scaled_image);
      return((Image *) NULL);
    }
  /*
    Initialize column pixel offsets.
  */
  scale_factor=UpShift(image->columns-1)/scaled_image->columns;
  columns=0;
  for (x=0; x < scaled_image->columns; x++)
  {
    x_offset[x]=DownShift((x+1)*scale_factor)-columns;
    columns+=x_offset[x];
  }
  /*
    Initialize row pixel offsets.
  */
  scale_factor=UpShift(image->rows-1)/scaled_image->rows;
  rows=0;
  for (y=0; y < scaled_image->rows; y++)
  {
    y_offset[y]=DownShift((y+1)*scale_factor)-rows;
    rows+=y_offset[y];
  }
  /*
    Preload first scanline.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    if (image->runlength > 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
  }
  /*
    Scale each row.
  */
  q=scaled_image->pixels;
  for (y=0; y < scaled_image->rows; y++)
  {
    /*
      Scale each column.
    */
    s=scanline;
    for (x=0; x < scaled_image->columns; x++)
    {
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=s->index;
      q->length=0;
      q++;
      s+=x_offset[x];
    }
    if (y_offset[y] > 0)
      {
        /*
          Skip a scan line.
        */
        for (x=0; x < (image->columns*(y_offset[y]-1)); x++)
          if (image->runlength > 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
        /*
          Read a scan line.
        */
        s=scanline;
        for (x=0; x < image->columns; x++)
        {
          if (image->runlength > 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
          s->red=p->red;
          s->green=p->green;
          s->blue=p->blue;
          s->index=p->index;
          s++;
        }
      }
  }
  (void) free((char *) scanline);
  (void) free((char *) x_offset);
  (void) free((char *) y_offset);
  return(scaled_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S o r t C o l o r m a p B y I n t e n t s i t y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function SortColormapByIntensity sorts the colormap of a PseudoClass image
%  by decreasing color intensity.
%
%  The format of the SortColormapByIntensity routine is:
%
%      SortColormapByIntensity(image)
%
%  A description of each parameter follows:
%
%    o image: A pointer to a Image structure.
%
%
*/
static int IntensityCompare(x,y)
const void
  *x,
  *y;
{
  ColorPacket
    *color_1,
    *color_2;

  color_1=(ColorPacket *) x;
  color_2=(ColorPacket *) y;
  return((int) Intensity(*color_2)-(int) Intensity(*color_1));
}

void SortColormapByIntensity(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  unsigned short
    *pixels;

  if (image->class != PseudoClass)
    return;
  /*
    Allocate memory for pixel indexes.
  */
  pixels=(unsigned short *) malloc(image->colors*sizeof(unsigned short));
  if (pixels == (unsigned short *) NULL)
    {
      Warning("unable to sort colormap","memory allocation failed");
      return;
    }
  /*
    Assign index values to colormap entries.
  */
  for (i=0; i < image->colors; i++)
    image->colormap[i].index=(unsigned short) i;
  /*
    Sort image colormap by decreasing color popularity.
  */
  (void) qsort((void *) image->colormap,(int) image->colors,sizeof(ColorPacket),
    IntensityCompare);
  /*
    Update image colormap indexes to sorted colormap order.
  */
  for (i=0; i < image->colors; i++)
    pixels[image->colormap[i].index]=(unsigned short) i;
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    index=pixels[p->index];
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p->index=index;
    p++;
  }
  (void) free((char *) pixels);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t e r e o I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function StereoImage combines two images and produces a single image that
%  is the composite of a left and right image of a stereo pair.  The left
%  image is converted to grayscale and written to the red channel of the
%  stereo image.  The right image is converted to grayscale and written to the
%  blue channel of the stereo image.  View the composite image with red-blue
%  glasses to create a stereo effect.
%
%  The format of the StereoImage routine is:
%
%      stereo_image=StereoImage(left_image,right_image)
%
%  A description of each parameter follows:
%
%    o stereo_image: Function StereoImage returns a pointer to the stereo
%      image.  A null image is returned if there is a memory shortage.
%
%    o left_image: The address of a structure of type Image.
%
%    o right_image: The address of a structure of type Image.
%
%
*/
Image *StereoImage(left_image,right_image)
Image
  *left_image,
  *right_image;
{
  Image
    *stereo_image;

  register int
    i;

  register RunlengthPacket
    *p,
    *q,
    *r;

  if ((left_image->columns != right_image->columns) ||
      (left_image->rows != right_image->rows))
    {
      Warning("unable to create stereo image",
        "left and right image sizes differ");
      return((Image *) NULL);
    }
  /*
    Initialize stereo image attributes.
  */
  stereo_image=CopyImage(left_image,left_image->columns,left_image->rows,False);
  if (stereo_image == (Image *) NULL)
    {
      Warning("unable to create stereo image","memory allocation failed");
      return((Image *) NULL);
    }
  stereo_image->class=DirectClass;
  /*
    Copy left image to red channel and right image to blue channel.
  */
  QuantizeImage(left_image,256,8,False,GRAYColorspace,True);
  p=left_image->pixels;
  left_image->runlength=p->length+1;
  QuantizeImage(right_image,256,8,False,GRAYColorspace,True);
  q=right_image->pixels;
  right_image->runlength=q->length+1;
  r=stereo_image->pixels;
  for (i=0; i < (stereo_image->columns*stereo_image->rows); i++)
  {
    if (left_image->runlength > 0)
      left_image->runlength--;
    else
      {
        p++;
        left_image->runlength=p->length;
      }
    if (right_image->runlength > 0)
      right_image->runlength--;
    else
      {
        q++;
        right_image->runlength=q->length;
      }
    r->red=(unsigned int) (p->red*12) >> 4;
    r->green=0;
    r->blue=q->blue;
    r->index=0;
    r->length=0;
    r++;
  }
  return(stereo_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y n c I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function SyncImage initializes the red, green, and blue intensities of each
%  pixel as defined by the colormap index.
%
%  The format of the SyncImage routine is:
%
%      SyncImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void SyncImage(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned short
    index;

  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    index=p->index;
    p->red=image->colormap[index].red;
    p->green=image->colormap[index].green;
    p->blue=image->colormap[index].blue;
    p++;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s f o r m I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function TransformImage creates a new image that is a transformed size of
%  of existing one as specified by the clip, image and scale geometries.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  If a clip geometry is specified a subregion of the image is obtained.
%  If the specified image size, as defined by the image and scale geometries,
%  is smaller than the actual image size, the image is first reduced to an
%  integral of the specified image size with an antialias digital filter.  The
%  image is then scaled to the exact specified image size with pixel
%  replication.  If the specified image size is greater than the actual image
%  size, the image is first enlarged to an integral of the specified image
%  size with bilinear interpolation.  The image is then scaled to the exact
%  specified image size with pixel replication.
%
%  The format of the TransformImage routine is:
%
%      TransformImage(image,clip_geometry,image_geometry,scale_geometry)
%
%  A description of each parameter follows:
%
%    o image: The address of an address of a structure of type Image.  The
%      transformed image is returned as this parameter.
%
%    o clip_geometry: Specifies a pointer to a clip geometry string.
%      This geometry defines a subregion of the image.
%
%    o image_geometry: Specifies a pointer to a image geometry string.
%      The specified width and height of this geometry string are absolute.
%
%    o scale_geometry: Specifies a pointer to a scale geometry string.
%      The specified width and height of this geometry string are relative.
%
%
*/
void TransformImage(image,clip_geometry,image_geometry,scale_geometry)
Image
  **image;

char
  *clip_geometry,
  *image_geometry,
  *scale_geometry;
{
  Image
    *transformed_image;

  int
    flags,
    x,
    y;

  unsigned int
    height,
    width;

  transformed_image=(*image);
  if (clip_geometry != (char *) NULL)
    {
      Image
        *clipped_image;

      /*
        Clip transformed_image to a user specified size.
      */
      x=0;
      y=0;
      flags=XParseGeometry(clip_geometry,&x,&y,&width,&height);
      if ((flags & WidthValue) == 0)
        width=(unsigned int) ((int) transformed_image->columns-x);
      if ((flags & HeightValue) == 0)
        height=(unsigned int) ((int) transformed_image->rows-y);
      if ((flags & XNegative) != 0)
        x+=transformed_image->columns-width;
      if ((flags & YNegative) != 0)
        y+=transformed_image->rows-height;
      clipped_image=ClipImage(transformed_image,x,y,width,height);
      if (clipped_image != (Image *) NULL)
        {
          DestroyImage(transformed_image);
          transformed_image=clipped_image;
        }
    }
  /*
    Scale image to a user specified size.
  */
  width=transformed_image->columns;
  height=transformed_image->rows;
  if (scale_geometry != (char *) NULL)
    {
      float
        height_factor,
        width_factor;

      width_factor=0.0;
      height_factor=0.0;
      (void) sscanf(scale_geometry,"%fx%f",&width_factor,&height_factor);
      if (height_factor == 0.0)
        height_factor=width_factor;
      width=(unsigned int) (width*width_factor);
      height=(unsigned int) (height*height_factor);
    }
  if (image_geometry != (char *) NULL)
    (void) XParseGeometry(image_geometry,&x,&y,&width,&height);
  while ((transformed_image->columns >= (width << 1)) &&
         (transformed_image->rows >= (height << 1)))
  {
    Image
      *reduced_image;

    /*
      Reduce image with a antialias digital filter.
     */
    reduced_image=ReduceImage(transformed_image);
    if (reduced_image == (Image *) NULL)
      break;
    DestroyImage(transformed_image);
    transformed_image=reduced_image;
  }
  while ((transformed_image->columns <= (width >> 1)) &&
         (transformed_image->rows <= (height >> 1)))
  {
    Image
      *zoomed_image;

    /*
      Zoom transformed_image with bilinear interpolation.
    */
    zoomed_image=ZoomImage(transformed_image);
    if (zoomed_image == (Image *) NULL)
      break;
    DestroyImage(transformed_image);
    transformed_image=zoomed_image;
  }
  if ((transformed_image->columns != width) ||
      (transformed_image->rows != height))
    {
      Image
        *scaled_image;

      /*
        Scale image with pixel replication.
      */
      scaled_image=ScaleImage(transformed_image,width,height);
      if (scaled_image != (Image *) NULL)
        {
          DestroyImage(transformed_image);
          transformed_image=scaled_image;
        }
    }
  *image=transformed_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T r a n s f o r m R G B I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure TransformRGBImage converts the reference image from an alternate
%  colorspace to RGB.
%
%  The format of the TransformRGBImage routine is:
%
%      TransformRGBImage(image,colorspace)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates the colorspace
%      the image is currently in.  On return the image is in the RGB
%      color space.
%
%
*/
void TransformRGBImage(image,colorspace)
Image
  *image;

unsigned int
  colorspace;
{
#define R 0
#define G (MaxRGB+1)
#define B (MaxRGB+1)*2

  long int
    *blue,
    *green,
    *red;

  register int
    i,
    x,
    y,
    z;

  register RunlengthPacket
    *p;

  register unsigned char
    *range_limit;

  unsigned char
    *range_table;

  if (colorspace == RGBColorspace)
    return;
  if (colorspace == GRAYColorspace)
    {
      if (image->colors == 2)
        {
          unsigned int
            polarity;

          /*
            Monochrome image.
          */
          polarity=Intensity(image->colormap[0]) >
             Intensity(image->colormap[1]);
          image->colormap[polarity].red=0;
          image->colormap[polarity].green=0;
          image->colormap[polarity].blue=0;
          image->colormap[!polarity].red=MaxRGB;
          image->colormap[!polarity].green=MaxRGB;
          image->colormap[!polarity].blue=MaxRGB;
        }
      return;
    }
  /*
    Allocate the tables.
  */
  red=(long int *) malloc(3*(MaxRGB+1)*sizeof(long int));
  green=(long int *) malloc(3*(MaxRGB+1)*sizeof(long int));
  blue=(long int *) malloc(3*(MaxRGB+1)*sizeof(long int));
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if ((red == (long int *) NULL) || (green == (long int *) NULL) ||
      (blue == (long int *) NULL) || (range_table == (unsigned char *) NULL))
    {
      Warning("unable to transform color space","memory allocation failed");
      return;
    }
  /*
    Initialize tables.
  */
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(unsigned char) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  switch (colorspace)
  {
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = Y+0.95600*I+0.62100*Q
          G = Y-0.27300*I-0.64700*Q
          B = Y-1.10400*I+1.70100*Q
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=UpShifted(0.95600)*i;
        blue[i+R]=UpShifted(0.62100)*i;
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.27300))*i;
        blue[i+G]=(-UpShifted(0.64700))*i;
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=(-UpShifted(1.10400))*i;
        blue[i+B]=UpShifted(1.70100)*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.40200*V
          G = Y-0.34414*U-0.71414*V
          B = Y+1.77200*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.40200/2)*(2*i-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.34414/2))*(2*i-MaxRGB);
        blue[i+G]=(-UpShifted(0.71414/2))*(2*i-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=UpShifted(1.77200/2)*(2*i-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize XYZ tables:

          R =  2.36444*X-0.89680*Y-0.46770*Z
          G = -0.51483*X+1.42523*Y+0.08817*Z
          B =  0.00500*X-0.01439*Y+1.00921*Z
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(2.36444)*i;
        green[i+R]=(-UpShifted(0.89680))*i;
        blue[i+R]=(-UpShifted(0.46770))*i;
        red[i+G]=(-UpShifted(0.51483))*i;
        green[i+G]=UpShifted(1.42523)*i;
        blue[i+G]=UpShifted(0.08817)*i;
        red[i+B]=UpShifted(0.00500)*i;
        green[i+B]=(-UpShifted(0.01439))*i;
        blue[i+B]=UpShifted(1.00921)*i;
      }
      break;
    }
  }
  /*
    Convert to RGB.
  */
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Convert DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        x=p->red;
        y=p->green;
        z=p->blue;
        p->red=range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        p->green=range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        p->blue=range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Convert PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
      {
        x=image->colormap[i].red;
        y=image->colormap[i].green;
        z=image->colormap[i].blue;
        image->colormap[i].red=
          range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        image->colormap[i].green=
          range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        image->colormap[i].blue=
          range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
      }
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        x=p->red;
        y=p->green;
        z=p->blue;
        p->red=range_limit[DownShift(red[x+R]+green[y+R]+blue[z+R])];
        p->green=range_limit[DownShift(red[x+G]+green[y+G]+blue[z+G])];
        p->blue=range_limit[DownShift(red[x+B]+green[y+B]+blue[z+B])];
        p++;
      }
      break;
    }
  }
  /*
    Free allocated memory.
  */
  (void) free((char *) range_table);
  (void) free((char *) blue);
  (void) free((char *) green);
  (void) free((char *) red);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function WriteImage writes an image to a file.
%
%  The format of the WriteImage routine is:
%
%      status=WriteImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function WriteImage return True if the image is written.
%      False is returned if there is a memory shortage or if the image file
%      fails to write.
%
%    o image: A pointer to a Image structure.
%
%
*/
unsigned int WriteImage(image)
Image
  *image;
{
  register int
    i;

  register long int
    count;

  unsigned int
    status;

  unsigned long int
    packets;

  if ((image->class != DirectClass) && (image->class != PseudoClass))
    {
      Warning("unable to write image","unknown image class");
      return(False);
    }
  if ((image->compression != RunlengthEncodedCompression) &&
      (image->compression != QEncodedCompression) &&
      (image->compression != NoCompression))
    {
      Warning("unable to write image","unknown image compression");
      return(False);
    }
  /*
    Open output image file.
  */
  OpenImage(image,"w");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return(False);
    }
  /*
    Pack image pixels.
  */
  if ((image->class == PseudoClass) &&
      (image->compression == QEncodedCompression))
    image->class=DirectClass;
  if (image->compression == RunlengthEncodedCompression)
    CompressImage(image);
  status=PackImage(image);
  if (status == False)
    {
      Warning("unable to write image",image->filename);
      DestroyImage(image);
      return(False);
    }
  packets=image->packets;
  if (image->compression != RunlengthEncodedCompression)
    packets=image->columns*image->rows;
  if (image->compression == QEncodedCompression)
    {
      unsigned char
        *compressed_pixels;

      /*
        Compress image pixels with Q encoding.
      */
      compressed_pixels=(unsigned char *)
        malloc((unsigned int) packets*image->packet_size*sizeof(unsigned char));
      if (compressed_pixels == (unsigned char *) NULL)
        {
          Warning("unable to write image","memory allocation failed");
          DestroyImage(image);
          return(False);
        }
      packets=QEncodeImage(image->packed_pixels,compressed_pixels,
        image->columns*(int) image->packet_size,image->rows);
      (void) free((char *) image->packed_pixels);
      image->packed_pixels=compressed_pixels;
      image->packet_size=1;
    }
  if (image->class == PseudoClass)
    ColormapSignature(image);
  /*
    Write header to file.
  */
  if (image->comments != (char *) NULL)
    (void) fprintf(image->file,"{%s}\n",image->comments);
  (void) fprintf(image->file,"id=ImageMagick\n");
  if (image->class == PseudoClass)
    (void) fprintf(image->file,"class=PseudoClass  colors=%u  signature=%s\n",
      image->colors,image->signature);
  else
    if (image->alpha)
      (void) fprintf(image->file,"class=DirectClass  alpha=True\n");
    else
      (void) fprintf(image->file,"class=DirectClass\n");
  if (image->compression == RunlengthEncodedCompression)
    (void) fprintf(image->file,"compression=RunlengthEncoded  packets=%lu\n",
      packets);
  else
    if (image->compression == QEncodedCompression)
      (void) fprintf(image->file,"compression=QEncoded  packets=%lu\n",packets);
  (void) fprintf(image->file,"columns=%u  rows=%u\n",image->columns,
    image->rows);
  if (image->scene > 0)
    (void) fprintf(image->file,"scene=%u\n",image->scene);
  if (image->montage != (char *) NULL)
    (void) fprintf(image->file,"montage=%s\n",image->montage);
  (void) fprintf(image->file,"\f\n:\n");
  if (image->montage != (char *) NULL)
    {
      /*
        Write montage tile directory.
      */
      if (image->directory != (char *) NULL)
        (void) fprintf(image->file,"%s",image->directory);
      (void) fwrite("\0",1,1,image->file);
    }
  if (image->class == PseudoClass)
    {
      register unsigned char
        *q;

      unsigned char
        *colormap;

      /*
        Allocate colormap.
      */
      colormap=(unsigned char *) malloc(3*image->colors*sizeof(unsigned char));
      if (colormap == (unsigned char *) NULL)
        {
          Warning("unable to write image","memory allocation failed");
          DestroyImage(image);
          return(False);
        }
      q=colormap;
      for (i=0; i < image->colors; i++)
      {
        *q++=image->colormap[i].red;
        *q++=image->colormap[i].green;
        *q++=image->colormap[i].blue;
      }
      /*
        Write colormap to file.
      */
      count=fwrite((char *) colormap,sizeof(unsigned char),
        (int) (3*image->colors),image->file);
      if (count != (3*image->colors))
        {
          Warning("unable to write image","file write error");
          DestroyImage(image);
          return(False);
        }
      (void) free((char *) colormap);
    }
  /*
    Write image pixels to file.
  */
  count=fwrite((char *) image->packed_pixels,(int) image->packet_size,
    (int) packets,image->file);
  if (count != packets)
    {
      Warning("unable to write image","file write error");
      DestroyImage(image);
      return(False);
    }
  (void) free((char *) image->packed_pixels);
  image->packed_pixels=(unsigned char *) NULL;
  CloseImage(image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n C o m p r e s s I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function UncompressImage uncompresses runlength-encoded pixels packets to
%  a rectangular array of pixels.
%
%  The format of the UncompressImage routine is:
%
%      status=UncompressImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function UncompressImage returns True if the image is
%      uncompressed otherwise False.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int UncompressImage(image)
Image
  *image;
{
  register int
    i,
    j,
    length;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    *uncompressed_pixels;

  if (image->packets == (image->columns*image->rows))
    return(True);
  /*
    Uncompress runlength-encoded packets.
  */
  uncompressed_pixels=(RunlengthPacket *) realloc((char *) image->pixels,
    image->columns*image->rows*sizeof(RunlengthPacket));
  if (uncompressed_pixels == (RunlengthPacket *) NULL)
    return(False);
  image->pixels=uncompressed_pixels;
  p=image->pixels+image->packets-1;
  q=uncompressed_pixels+image->columns*image->rows-1;
  for (i=0; i < image->packets; i++)
  {
    length=p->length;
    for (j=0; j <= length; j++)
    {
      q->red=p->red;
      q->green=p->green;
      q->blue=p->blue;
      q->index=p->index;
      q->length=0;
      q--;
    }
    p--;
  }
  image->packets=image->columns*image->rows;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n p a c k I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function UnpackImage unpacks the packed image pixels into runlength-encoded
%  pixel packets.  The packed image pixel memory is then freed.
%
%  The format of the UnpackImage routine is:
%
%      status=UnpackImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function UnpackImage return True if the image is unpacked.
%      False is returned if there is an error occurs.
%
%    o image: The address of a structure of type Image.
%
%
*/
unsigned int UnpackImage(image)
Image
  *image;
{
  register int
    i;

  register RunlengthPacket
    *q;

  register unsigned char
    *p;

  unsigned long int
    count;

  if (image->packed_pixels == (unsigned char *) NULL)
    {
      Warning("unable to unpack pixels","no packed image pixels");
      return(False);
    }
  /*
    Allocate pixels.
  */
  if (image->pixels != (RunlengthPacket *) NULL)
    (void) free((char *) image->pixels);
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      Warning("unable to unpack pixels","memory allocation failed");
      return(False);
    }
  /*
    Unpack the packed image pixels into runlength-encoded pixel packets.
  */
  p=image->packed_pixels;
  q=image->pixels;
  count=0;
  if (image->class == DirectClass)
    {
      register int
        alpha;

      alpha=image->alpha;
      if (image->compression == RunlengthEncodedCompression)
        for (i=0; i < image->packets; i++)
        {
          q->red=(*p++);
          q->green=(*p++);
          q->blue=(*p++);
          q->index=(unsigned short) (alpha ? (*p++) : 0);
          q->length=(*p++);
          count+=(q->length+1);
          q++;
        }
      else
        for (i=0; i < image->packets; i++)
        {
          q->red=(*p++);
          q->green=(*p++);
          q->blue=(*p++);
          q->index=(unsigned short) (alpha ? (*p++) : 0);
          q->length=0;
          count++;
          q++;
        }
    }
  else
    {
      register unsigned short
        index;

      if (image->compression == RunlengthEncodedCompression)
        {
          if (image->colors <= 256)
            for (i=0; i < image->packets; i++)
            {
              index=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=(*p++);
              count+=(q->length+1);
              q++;
            }
          else
            for (i=0; i < image->packets; i++)
            {
              index=(*p++) << 8;
              index|=(*p++);
              q->red=image->colormap[index].red;
              q->green=image->colormap[index].green;
              q->blue=image->colormap[index].blue;
              q->index=index;
              q->length=(*p++);
              count+=(q->length+1);
              q++;
            }
        }
      else
        if (image->colors <= 256)
          for (i=0; i < image->packets; i++)
          {
            index=(*p++);
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->length=0;
            count++;
            q++;
          }
        else
          for (i=0; i < image->packets; i++)
          {
            index=(*p++) << 8;
            index|=(*p++);
            q->red=image->colormap[index].red;
            q->green=image->colormap[index].green;
            q->blue=image->colormap[index].blue;
            q->index=index;
            q->length=0;
            count++;
            q++;
          }
    }
  /*
    Free packed pixels memory.
  */
  (void) free((char *) image->packed_pixels);
  image->packed_pixels=(unsigned char *) NULL;
  /*
    Guarentee the correct number of pixel packets.
  */
  if (count > (image->columns*image->rows))
    {
      Warning("insufficient image data in file",image->filename);
      return(False);
    }
  else
    if (count < (image->columns*image->rows))
      {
        Warning("too much image data in file",image->filename);
        return(False);
      }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Z o o m I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ZoomImage creates a new image that is a integral size greater
%  than an existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  ZoomImage scans the reference image to create a zoomed image by bilinear
%  interpolation.  The zoomed image columns and rows become:
%
%    number_columns << 1
%    number_rows << 1
%
%  The format of the ZoomImage routine is:
%
%      zoomed_image=ZoomImage(image)
%
%  A description of each parameter follows:
%
%    o zoomed_image: Function ZoomImage returns a pointer to the image after
%      zooming.  A null image is returned if there is a a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
static Image *ZoomImage(image)
Image
  *image;
{
  ColorPacket
    *scanline;

  Image
    *zoomed_image;

  register ColorPacket
    *cs,
    *ns,
    *s;

  register RunlengthPacket
    *p,
    *q;

  register unsigned int
    x;

  unsigned int
    y;

  if (((image->columns*image->rows) << 1) > MaxImageSize)
    {
      Warning("unable to zoom image","image size too large");
      return((Image *) NULL);
    }
  /*
    Initialize scaled image attributes.
  */
  zoomed_image=CopyImage(image,image->columns << 1,image->rows << 1,False);
  if (zoomed_image == (Image *) NULL)
    {
      Warning("unable to zoom image","memory allocation failed");
      return((Image *) NULL);
    }
  zoomed_image->class=DirectClass;
  /*
    Allocate scan line buffer.
  */
  scanline=(ColorPacket *) malloc(4*image->columns*sizeof(ColorPacket));
  if (scanline == (ColorPacket *) NULL)
    {
      Warning("unable to zoom image","memory allocation failed");
      DestroyImage(zoomed_image);
      return((Image *) NULL);
    }
  /*
    Preload a scan line and interpolate.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    s->red=p->red;
    s->green=p->green;
    s->blue=p->blue;
    s->index=p->index;
    s++;
    if (image->runlength > 0)
      {
        image->runlength--;
        s->red=p->red;
        s->green=p->green;
        s->blue=p->blue;
        s->index=p->index;
      }
    else
      {
        p++;
        image->runlength=p->length;
        s->red=(unsigned char) ((int) (p->red+(s-1)->red) >> 1);
        s->green=(unsigned char) ((int) (p->green+(s-1)->green) >> 1);
        s->blue=(unsigned char) ((int) (p->blue+(s-1)->blue) >> 1);
        s->index=(unsigned short) ((int) (p->index+(s-1)->index) >> 1);
      }
    s++;
  }
  /*
    Zoom each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=zoomed_image->pixels;
  for (y=0; y < image->rows; y++)
  {
    cs=scanline+image->columns*((y+0) % 2)*2;
    ns=scanline+image->columns*((y+1) % 2)*2;
    /*
      Read a scan line and interpolate.
    */
    s=ns;
    for (x=0; x < image->columns; x++)
    {
      s->red=p->red;
      s->green=p->green;
      s->blue=p->blue;
      s->index=p->index;
      s++;
      if (image->runlength > 0)
        {
          image->runlength--;
          s->red=p->red;
          s->green=p->green;
          s->blue=p->blue;
          s->index=p->index;
        }
      else
        {
          p++;
          image->runlength=p->length;
          s->red=(unsigned char) ((int) (p->red+(s-1)->red) >> 1);
          s->green=(unsigned char) ((int) (p->green+(s-1)->green) >> 1);
          s->blue=(unsigned char) ((int) (p->blue+(s-1)->blue) >> 1);
          s->index=(unsigned short) ((int) (p->index+(s-1)->index) >> 1);
        }
      s++;
    }
    /*
      Dump column interpolation values.
    */
    s=cs;
    for (x=0; x < zoomed_image->columns; x++)
    {
      q->red=s->red;
      q->green=s->green;
      q->blue=s->blue;
      q->index=s->index;
      q->length=0;
      q++;
      s++;
    }
    /*
      Dump row interpolation values.
    */
    for (x=0; x < zoomed_image->columns; x++)
    {
      q->red=(unsigned char) ((int) (cs->red+ns->red) >> 1);
      q->green=(unsigned char) ((int) (cs->green+ns->green) >> 1);
      q->blue=(unsigned char) ((int) (cs->blue+ns->blue) >> 1);
      q->index=(unsigned short) ((int) (cs->index+ns->index) >> 1);
      q->length=0;
      q++;
      cs++;
      ns++;
    }
  }
  (void) free((char *) scanline);
  return(zoomed_image);
}
