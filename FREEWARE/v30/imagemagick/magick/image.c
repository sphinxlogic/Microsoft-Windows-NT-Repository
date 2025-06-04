/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                       IIIII  M   M   AAA   GGGG  EEEEE                      %
%                         I    MM MM  A   A G      E                          %
%                         I    M M M  AAAAA G  GG  EEE                        %
%                         I    M   M  A   A G   G  E                          %
%                       IIIII  M   M  A   A  GGGG  EEEEE                      %
%                                                                             %
%                                                                             %
%                          ImageMagick Image Routines                         %
%                                                                             %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                  July 1992                                  %
%                                                                             %
%                                                                             %
%  Copyright 1994 E. I. du Pont de Nemours and Company                        %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. du Pont de Nemours    %
%  and Company not be used in advertising or publicity pertaining to          %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. du Pont de Nemours and Company makes no representations %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. du Pont de Nemours and Company disclaims all warranties with regard  %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. du Pont de Nemours and Company be     %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortuous action, arising     %
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
#include "magick.h"
#include "image.h"
#include "compress.h"
#include "utility.h"
#include "X.h"

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
%      allocated_image=AllocateImage(image_info)
%
%  A description of each parameter follows:
%
%    o allocated_image: Function AllocateImage returns a pointer to an image
%      structure initialized to default values.  A null image is returned if
%      there is a memory shortage.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%
*/
Image *AllocateImage(image_info)
ImageInfo
  *image_info;
{
  Image
    *allocated_image;

  /*
    Allocate image structure.
  */
  allocated_image=(Image *) malloc(sizeof(Image));
  if (allocated_image == (Image *) NULL)
    {
      Warning("Unable to allocate image","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Initialize Image structure.
  */
  allocated_image->file=(FILE *) NULL;
  allocated_image->status=False;
  allocated_image->temporary=False;
  *allocated_image->filename='\0';
  allocated_image->filesize=0;
  allocated_image->pipe=False;
  (void) strcpy(allocated_image->magick,"MIFF");
  allocated_image->comments=(char *) NULL;
  allocated_image->label=(char *) NULL;
  allocated_image->id=UndefinedId;
  allocated_image->class=DirectClass;
  allocated_image->matte=False;
  allocated_image->compression=RunlengthEncodedCompression;
  allocated_image->columns=0;
  allocated_image->rows=0;
  allocated_image->scene=0;
  allocated_image->montage=(char *) NULL;
  allocated_image->directory=(char *) NULL;
  allocated_image->colormap=(ColorPacket *) NULL;
  allocated_image->colors=0;
  allocated_image->colorspace=0;
  allocated_image->normalized_maximum_error=0.0;
  allocated_image->normalized_mean_error=0.0;
  allocated_image->mean_error_per_pixel=0;
  allocated_image->total_colors=0;
  allocated_image->signature=(char *) NULL;
  allocated_image->pixels=(RunlengthPacket *) NULL;
  allocated_image->packet=(RunlengthPacket *) NULL;
  allocated_image->packets=0;
  allocated_image->packet_size=0;
  allocated_image->packed_pixels=(unsigned char *) NULL;
  *allocated_image->magick_filename='\0';
  allocated_image->magick_columns=0;
  allocated_image->magick_rows=0;
  allocated_image->magick_time=time((time_t *) NULL);
  allocated_image->orphan=False;
  allocated_image->previous=(Image *) NULL;
  allocated_image->next=(Image *) NULL;
  if (image_info != (ImageInfo *) NULL)
    {
      (void) strcpy(allocated_image->filename,image_info->filename);
      (void) strcpy(allocated_image->magick,image_info->magick);
    }
  return(allocated_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     B l u r I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function BlurImage creates a new image that is a copy of an existing
%  one with the pixels blurred.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  BlurImage convolves the pixel neighborhood with this blurring mask:
%
%     1  2  1
%     2 20  2
%     1  2  1
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the BlurImage routine is:
%
%      blurred_image=BlurImage(image)
%
%  A description of each parameter follows:
%
%    o blurred_image: Function BlurImage returns a pointer to the image
%      after it is blurred.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Image *BlurImage(image)
Image
  *image;
{
#define Blur(weight) \
  total_red+=(weight)*(int) (s->red); \
  total_green+=(weight)*(int) (s->green); \
  total_blue+=(weight)*(int) (s->blue); \
  s++;

  Image
    *blurred_image;

  long int
    total_blue,
    total_green,
    total_red;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2;

  register unsigned int
    x;

  RunlengthPacket
    background_pixel,
    *scanline;

  unsigned int
    y;

  if ((image->columns < 3) || (image->rows < 3))
    {
      Warning("Unable to blur image","image size must exceed 3x3");
      return((Image *) NULL);
    }
  /*
    Initialize blurred image attributes.
  */
  blurred_image=CopyImage(image,image->columns,image->rows,False);
  if (blurred_image == (Image *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      return((Image *) NULL);
    }
  blurred_image->class=DirectClass;
  /*
    Allocate scan line buffer for 3 rows of the image.
  */
  scanline=(RunlengthPacket *) malloc(3*image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      DestroyImage(blurred_image);
      return((Image *) NULL);
    }
  /*
    Read the first two rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Dump first scanlines of image.
  */
  background_pixel.red=0;
  background_pixel.green=0;
  background_pixel.blue=0;
  background_pixel.index=0;
  background_pixel.length=0;
  q=blurred_image->pixels;
  for (x=0; x < image->columns; x++)
  {
    *q=background_pixel;
    q++;
  }
  /*
    Convolve each row.
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    *q=background_pixel;
    q++;
    for (x=1; x < (image->columns-1); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      s=s0;
      Blur(1);  Blur(2); Blur(1);
      s=s1;
      Blur(2); Blur(20); Blur(2);
      s=s2;
      Blur(1);  Blur(2); Blur(1);
      q->red=(unsigned char) ((total_red+16) >> 5);
      q->green=(unsigned char) ((total_green+16) >> 5);
      q->blue=(unsigned char) ((total_blue+16) >> 5);
      q->index=0;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
    }
    /*
      Transfer last pixel of the scanline.
    */
    *q=background_pixel;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  for (x=0; x < image->columns; x++)
  {
    *q=background_pixel;
    q++;
  }
  (void) free((char *) scanline);
  return(blurred_image);
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
%  structure and returns a pointer to the new image.  Set the border and
%  highlight to the same color to get a solid border.
%
%  The format of the BorderImage routine is:
%
%      bordered_image=BorderImage(image,border_info,border_color)
%
%  A description of each parameter follows:
%
%    o bordered_image: Function BorderImage returns a pointer to the bordered
%      image.  A null image is returned if there is a a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o border_info: Specifies a pointer to a XRectangle which defines the
%      border region.
%
%    o border_color: A pointer to a ColorPacket which contains the red,
%      green, and blue components of the border color.
%
%
*/
Image *BorderImage(image,border_info,border_color)
Image
  *image;

RectangleInfo
  *border_info;

ColorPacket
  *border_color;
{
  Image
    *bordered_image;

  register int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    border;

  /*
    Initialize bordered image attributes.
  */
  bordered_image=CopyImage(image,image->columns+(border_info->width << 1),
    image->rows+(border_info->height << 1),False);
  if (bordered_image == (Image *) NULL)
    {
      Warning("Unable to border image","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Initialize border color.
  */
  border.red=border_color->red;
  border.green=border_color->green;
  border.blue=border_color->blue;
  border.index=border_color->index;
  border.length=0;
  /*
    Copy image and put border around it.
  */
  q=bordered_image->pixels;
  for (y=0; y < border_info->height; y++)
    for (x=0; x < bordered_image->columns; x++)
      *q++=border;
  p=image->pixels;
  image->runlength=p->length+1;
  for (y=0; y < image->rows; y++)
  {
    /*
      Initialize scanline with border color.
    */
    for (x=0; x < border_info->width; x++)
      *q++=border;
    /*
      Transfer scanline.
    */
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *q=(*p);
      q->length=0;
      q++;
    }
    x=0;
    while (x < (bordered_image->columns-image->columns-border_info->width))
    {
      *q++=border;
      x++;
    }
  }
  for (y=(bordered_image->rows-image->rows-border_info->height-1); y >= 0; y--)
    for (x=0; x < bordered_image->columns; x++)
      *q++=border;
  return(bordered_image);
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
%  Function CloseImage closes a file associated with the image.  If the
%  filename prefix is '|', the file is a pipe and is closed with pclose.
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
  if (image->file == (FILE *) NULL)
    return;
  image->status=ferror(image->file);
  if (image->pipe)
    (void) pclose(image->file);
  else
    if ((image->file != stdin) && (image->file != stdout))
      (void) fclose(image->file);
  image->file=(FILE *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m m e n t I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CommentImage initializes an image comment.  Optionally the
%  comment can include the image filename, type, width, height, or scene
%  number by embedding special format characters.  Embed %f for filename,
%  %m for magick, %w for width, %h for height, %s for scene number, or \n
%  for newline.  For example,
%
%     %f  %wx%h
%
%  produces an image comment of
%
%     bird.miff  512x480
%
%  for an image titled bird.miff and whose width is 512 and height is 480.
%
%  The format of the CommentImage routine is:
%
%      CommentImage(image,comments)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o comments: The address of a character string containing the comment
%      format.
%
%
*/
void CommentImage(image,comments)
Image
  *image;

char
  *comments;
{
  register char
    *p,
    *q;

  unsigned int
    indirection,
    length;

  if (image->comments != (char *) NULL)
    (void) free((char *) image->comments);
  image->comments=(char *) NULL;
  if (comments == (char *) NULL)
    return;
  indirection=(*comments == '@');
  if (indirection)
    {
      FILE
        *file;

      int
        c;

      /*
        Read comments from a file.
      */
      file=(FILE *) fopen(comments+1,"r");
      if (file == (FILE *) NULL)
        {
          Warning("Unable to read comments file",comments+1);
          return;
        }
      length=MaxTextLength;
      comments=(char *) malloc(length*sizeof(char));
      for (q=comments ; comments != (char *) NULL; q++)
      {
        c=fgetc(file);
        if (c == EOF)
          break;
        if ((q-comments+1) >= length)
          {
            *q='\0';
            length<<=1;
            comments=(char *) realloc((char *) comments,length*sizeof(char));
            if (comments == (char *) NULL)
              break;
            q=comments+strlen(comments);
          }
        *q=(unsigned char) c;
      }
      (void) fclose(file);
      if (comments == (char *) NULL)
        {
          Warning("Unable to comments image","Memory allocation failed");
          return;
        }
      *q='\0';
    }
  /*
    Allocate and initialize image comments.
  */
  p=comments;
  length=strlen(comments)+MaxTextLength;
  image->comments=(char *) malloc(length*sizeof(char));
  for (q=image->comments; image->comments != (char *) NULL; p++)
  {
    if (*p == '\0')
      break;
    if ((q-image->comments+MaxTextLength) >= length)
      {
        *q='\0';
        length<<=1;
        image->comments=(char *)
          realloc((char *) image->comments,length*sizeof(char));
        if (image->comments == (char *) NULL)
          break;
        q=image->comments+strlen(image->comments);
      }
    /*
      Process formatting characters in comments.
    */
    if (*p != '%')
      {
        *q++=(*p);
        continue;
      }
    p++;
    switch (*p)
    {
      case 'f':
      {
        register char
          *p;

        /*
          Label segment is the base of the filename.
        */
        p=image->filename+strlen(image->filename)-1;
        while ((p > image->filename) && (*(p-1) != '/'))
          p--;
        (void) strcpy(q,p);
        q+=strlen(p);
        break;
      }
      case 'h':
      {
        (void) sprintf(q,"%u",image->rows);
        q=image->comments+strlen(image->comments);
        break;
      }
      case 'm':
      {
        (void) strcpy(q,image->magick);
        q+=strlen(image->magick);
        break;
      }
      case 's':
      {
        (void) sprintf(q,"%u",image->scene);
        q=image->comments+strlen(image->comments);
        break;
      }
      case 'w':
      {
        (void) sprintf(q,"%u",image->columns);
        q=image->comments+strlen(image->comments);
        break;
      }
      default:
      {
        *q++='%';
        *q++=(*p);
        break;
      }
    }
  }
  if (image->comments == (char *) NULL)
    {
      Warning("Unable to comment image","Memory allocation failed");
      return;
    }
  *q='\0';
  if (indirection)
    (void) free((char *) comments);
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
      Warning("Unable to compress colormap","Memory allocation failed");
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
  if (image->matte)
    for (i=0; i < (image->columns*image->rows); i++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      if ((p->red == q->red) && (p->green == q->green) &&
          (p->blue == q->blue) && (p->index == q->index) &&
          (q->length < MaxRunlength))
        q->length++;
      else
        {
          if (image->packets != 0)
            q++;
          image->packets++;
          *q=(*p);
          q->length=0;
        }
    }
  else
    for (i=0; i < (image->columns*image->rows); i++)
    {
      if (image->runlength != 0)
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
          if (image->packets != 0)
            q++;
          image->packets++;
          *q=(*p);
          q->length=0;
        }
    }
  image->pixels=(RunlengthPacket *) realloc((char *) image->pixels,
    image->packets*sizeof(RunlengthPacket));
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

  register short
    index;

  /*
    Check composite geometry.
  */
  if (((x_offset+(int) image->columns) < 0) ||
      ((y_offset+(int) image->rows) < 0) ||
      (x_offset > (int) image->columns) || (y_offset > (int) image->rows))
    {
      Warning("Unable to composite image","geometry does not contain image");
      return;
    }
  /*
    Image must be uncompressed.
  */
  if (!UncompressImage(image))
    return;
  if (compose == ReplaceCompositeOp)
    {
      /*
        Promote image to DirectClass if colormaps differ.
      */
      if (image->class == PseudoClass)
        if (composite_image->class == DirectClass)
          image->class=DirectClass;
        else
          {
            if (image->signature == (char *) NULL)
              ColormapSignature(image);
            if (composite_image->signature == (char *) NULL)
              ColormapSignature(composite_image);
            if (strcmp(image->signature,composite_image->signature) != 0)
              image->class=DirectClass;
          }
    }
  else
    {
      /*
        Initialize image matte data.
      */
      if (!image->matte)
        {
          q=image->pixels;
          red=q->red;
          green=q->green;
          blue=q->blue;
          for (i=0; i < image->packets; i++)
          {
            q->index=Opaque;
            q++;
          }
          image->class=DirectClass;
          image->matte=True;
        }
      if (!composite_image->matte)
        {
          p=composite_image->pixels;
          red=p->red;
          green=p->green;
          blue=p->blue;
          for (i=0; i < composite_image->packets; i++)
          {
            p->index=Opaque;
            if ((p->red == red) && (p->green == green) && (p->blue == blue))
              p->index=Transparent;
            p++;
          }
          composite_image->class=DirectClass;
          composite_image->matte=True;
        }
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
      if (composite_image->runlength != 0)
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
          if (p->index == Transparent)
            {
              red=q->red;
              green=q->green;
              blue=q->blue;
              index=q->index;
            }
          else
            if (p->index == Opaque)
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
          index=Opaque;
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
      if (index > Opaque)
        q->index=Opaque;
      else
        if (index < Transparent)
          q->index=Transparent;
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
%     C o n t r a s t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ContrastImage enhances the intensity differences between the
%  lighter and darker elements of the image.
%
%  The format of the ContrastImage routine is:
%
%      ContrastImage(image,sharpen)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o sharpen: If True, the intensity is increased otherwise it is
%      decreased.
%
%
*/

static void Contrast(sign,red,green,blue)
int
  sign;

unsigned char
  *red,
  *green,
  *blue;
{
  double
    brightness,
    hue,
    minimum,
    saturation,
    theta;

  register double
    b,
    g,
    r;

  /*
    Convert RGB to HSV colorspace.
  */
  hue=(-1.0);
  saturation=0.0;
  r=(double) (*red)/(double) MaxRGB;
  g=(double) (*green)/(double) MaxRGB;
  b=(double) (*blue)/(double) MaxRGB;
  if (r >= g)
    {
      if (r >= b)
        brightness=r;
      else
        brightness=b;
    }
  else
    if (g >= b)
      brightness=g;
    else
      brightness=b;
  if (r <= g)
    {
      if (r <= b)
        minimum=r;
      else
        minimum=b;
    }
  else
    if (g <= b)
      minimum=g;
    else
      minimum=b;
  if (brightness != 0.0)
    saturation=(brightness-minimum)/brightness;
  if (saturation != 0.0)
    {
      if (r == brightness)
        hue=(g-b)/(brightness-minimum);
      else
        if (g == brightness)
          hue=2.0+(b-r)/(brightness-minimum);
        else
          if (b == brightness)
            hue=4.0+(r-g)/(brightness-minimum);
      hue=hue*60.0;
      if (hue < 0.0)
        hue+=360.0;
    }
  /*
    Enhance contrast: dark color become darker, light color become lighter.
  */
  theta=(brightness-0.5)*PI;
  brightness+=(((((sin(theta)+1.0))*0.5)-brightness)*sign)*0.5;
  if (brightness > 1.0)
    brightness=1.0;
  else
    if (brightness < 0)
      brightness=0.0;
  /*
    Convert HSV to RGB
  */
  r=brightness;
  g=brightness;
  b=brightness;
  if ((hue != -1.0) && (saturation != 0.0))
    {
      double
        f,
        j,
        k,
        l,
        v;

      int
        i;

      if (hue == 360.0)
        hue=0.0;
      hue=hue/60.0;
      i=floor(hue);
      if (i < 0)
        i=0;
      f=hue-i;
      j=brightness*(1.0-saturation);
      k=brightness*(1.0-(saturation*f));
      l=brightness*(1.0-(saturation*(1.0-f)));
      v=brightness;
      switch (i)
      {
        case 0:  r=v;  g=l;  b=j;  break;
        case 1:  r=k;  g=v;  b=j;  break;
        case 2:  r=j;  g=v;  b=l;  break;
        case 3:  r=j;  g=k;  b=v;  break;
        case 4:  r=l;  g=j;  b=v;  break;
        case 5:  r=v;  g=j;  b=k;  break;
      }
    }
  *red=(unsigned char) floor((r*(double) MaxRGB)+0.5);
  *green=(unsigned char) floor((g*(double) MaxRGB)+0.5);
  *blue=(unsigned char) floor((b*(double) MaxRGB)+0.5);
}

void ContrastImage(image,sharpen)
Image
  *image;

unsigned int
  sharpen;
{
  int
    sign;

  register int
    i;

  register RunlengthPacket
    *p;

  sign=sharpen ? 1 : -1;
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Contrast enhance DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        Contrast(sign,&p->red,&p->green,&p->blue);
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Contrast enhance PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
        Contrast(sign,&image->colormap[i].red,&image->colormap[i].green,
          &image->colormap[i].blue);
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
  copy_image->montage=(char *) NULL;
  copy_image->directory=(char *) NULL;
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
      {
        *q=(*p);
        p++;
        q++;
      }
    }
  if (image->orphan)
    copy_image->file=(FILE *) NULL;
  else
    {
      /*
        Link image into image list.
      */
      if (copy_image->previous != (Image *) NULL)
        copy_image->previous->next=copy_image;
      if (copy_image->next != (Image *) NULL)
        copy_image->next->previous=copy_image;
    }
  copy_image->orphan=False;
  return(copy_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C r o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CropImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the CropImage routine is:
%
%      cropped_image=CropImage(image,crop_info)
%
%  A description of each parameter follows:
%
%    o cropped_image: Function CropImage returns a pointer to the cropped
%      image.  A null image is returned if there is a a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o crop_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to crop.
%
%
*/
Image *CropImage(image,crop_info)
Image
  *image;

RectangleInfo
  *crop_info;
{
  Image
    *cropped_image;

  register int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    background;

  /*
    Check crop geometry.
  */
  if (((crop_info->x+(int) crop_info->width) < 0) ||
      ((crop_info->y+(int) crop_info->height) < 0) ||
      (crop_info->x > (int) image->columns) ||
      (crop_info->y > (int) image->rows))
    {
      Warning("Unable to crop image","geometry does not contain image");
      return((Image *) NULL);
    }
  if ((crop_info->x+(int) crop_info->width) > (int) image->columns)
    crop_info->width=(unsigned int) ((int) image->columns-crop_info->x);
  if ((crop_info->y+(int) crop_info->height) > (int) image->rows)
    crop_info->height=(unsigned int) ((int) image->rows-crop_info->y);
  if (crop_info->x < 0)
    {
      crop_info->width-=(unsigned int) (-crop_info->x);
      crop_info->x=0;
    }
  if (crop_info->y < 0)
    {
      crop_info->height-=(unsigned int) (-crop_info->y);
      crop_info->y=0;
    }
  if ((crop_info->width == 0) && (crop_info->height == 0))
    {
      /*
        Remove vertical edges that are the background color.
      */
      if (!UncompressImage(image))
        return((Image *) NULL);
      background=image->pixels[0];
      for (x=0; x < image->columns; x++)
      {
        p=image->pixels+x;
        for (y=0; y < image->rows; y++)
        {
          if ((p->red != background.red) || (p->green != background.green) ||
              (p->blue != background.blue))
            break;
          p+=image->columns;
        }
        if (y < image->rows)
          break;
      }
      crop_info->x=x;
      if (crop_info->x == (int) image->columns)
        {
          Warning("Unable to crop image","geometry does not contain image");
          return((Image *) NULL);
        }
      background=image->pixels[image->packets-1];
      for (x=image->columns-1; x > 0; x--)
      {
        p=image->pixels+x;
        for (y=0; y < image->rows; y++)
        {
          if ((p->red != background.red) || (p->green != background.green) ||
              (p->blue != background.blue))
            break;
          p+=image->columns;
        }
        if (y < image->rows)
          break;
      }
      crop_info->width=x-crop_info->x+1;
      /*
        Remove horizontal edges that are the background color.
      */
      background=image->pixels[0];
      p=image->pixels;
      for (y=0; y < image->rows; y++)
      {
        for (x=0; x < image->columns; x++)
        {
          if ((p->red != background.red) || (p->green != background.green) ||
              (p->blue != background.blue))
            break;
          p++;
        }
        if (x < image->columns)
          break;
      }
      crop_info->y=y;
      background=image->pixels[image->packets-1];
      p=image->pixels+(image->columns*image->rows)-1;
      for (y=image->rows-1; y > 0; y--)
      {
        for (x=0; x < image->columns; x++)
        {
          if ((p->red != background.red) || (p->green != background.green) ||
              (p->blue != background.blue))
            break;
          p--;
        }
        if (x < image->columns)
          break;
      }
      crop_info->height=y-crop_info->y+1;
    }
  if ((crop_info->width == 0) || (crop_info->height == 0))
    {
      Warning("Unable to crop image","geometry dimensions are zero");
      return((Image *) NULL);
    }
  /*
    Initialize cropped image attributes.
  */
  cropped_image=CopyImage(image,crop_info->width,crop_info->height,False);
  if (cropped_image == (Image *) NULL)
    {
      Warning("Unable to crop image","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Skip pixels up to the cropped image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  for (x=0; x < (crop_info->y*image->columns+crop_info->x); x++)
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
  /*
    Extract cropped image.
  */
  q=cropped_image->pixels;
  for (y=0; y < (cropped_image->rows-1); y++)
  {
    /*
      Transfer scanline.
    */
    for (x=0; x < cropped_image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *q=(*p);
      q->length=0;
      q++;
    }
    /*
      Skip to next scanline.
    */
    for (x=0; x < (image->columns-cropped_image->columns); x++)
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
  }
  /*
    Transfer last scanline.
  */
  for (x=0; x < cropped_image->columns; x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *q=(*p);
    q->length=0;
    q++;
  }
  return(cropped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C u t I m a g e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function CutImage creates a new image that is a subregion of an existing
%  one.  It allocates the memory necessary for the new Image structure and
%  returns a pointer to the new image.
%
%  The format of the CutImage routine is:
%
%      cut_image=CutImage(image,cut_info)
%
%  A description of each parameter follows:
%
%    o cut_image: Function CutImage returns a pointer to the cut
%      image.  A null image is returned if there is a a memory shortage or
%      if the image width or height is zero.
%
%    o image: The address of a structure of type Image.
%
%    o cut_info: Specifies a pointer to a RectangleInfo which defines the
%      region of the image to crop.
%
%
*/
Image *CutImage(image,cut_info)
Image
  *image;

RectangleInfo
  *cut_info;
{
  Image
    *cut_image;

  register int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  /*
    Check cut geometry.
  */
  if (((cut_info->x+(int) cut_info->width) < 0) ||
      ((cut_info->y+(int) cut_info->height) < 0) ||
      (cut_info->x > (int) image->columns) ||
      (cut_info->y > (int) image->rows))
    {
      Warning("Unable to cut image","geometry does not contain image");
      return((Image *) NULL);
    }
  if ((cut_info->x+(int) cut_info->width) > (int) image->columns)
    cut_info->width=(unsigned int) ((int) image->columns-cut_info->x);
  if ((cut_info->y+(int) cut_info->height) > (int) image->rows)
    cut_info->height=(unsigned int) ((int) image->rows-cut_info->y);
  if (cut_info->x < 0)
    {
      cut_info->width-=(unsigned int) (-cut_info->x);
      cut_info->x=0;
    }
  if (cut_info->y < 0)
    {
      cut_info->height-=(unsigned int) (-cut_info->y);
      cut_info->y=0;
    }
  /*
    Initialize cut image attributes.
  */
  cut_image=CopyImage(image,image->columns-cut_info->width,
    image->rows-cut_info->height,False);
  if (cut_image == (Image *) NULL)
    {
      Warning("Unable to cut image","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Extract cut image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=cut_image->pixels;
  for (y=0; y < cut_info->y; y++)
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      if ((x < cut_info->x) || (x >= (cut_info->x+cut_info->width)))
        {
          *q=(*p);
          q->length=0;
          q++;
        }
    }
  /*
    Skip pixels up to the cut image.
  */
  for (x=0; x < (cut_info->height*image->columns); x++)
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
  /*
    Extract cut image.
  */
  for (y=0; y < (image->rows-(cut_info->y+cut_info->height)); y++)
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      if ((x < cut_info->x) || (x >= (cut_info->x+cut_info->width)))
        {
          *q=(*p);
          q->length=0;
          q++;
        }
    }
  return(cut_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s c r i b e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function DescribeImage describes an image by printing its attributes to
%  stderr.
%
%  The format of the DescribeImage routine is:
%
%      DescribeImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%
*/
void DescribeImage(image)
Image
  *image;
{
  /*
    Display detailed info about the image.
  */
  if (*image->magick_filename != '\0')
    if (strcmp(image->magick_filename,image->filename) != 0)
      (void) fprintf(stderr,"%s=>",image->magick_filename);
   if ((image->previous == (Image *) NULL) && (image->next == (Image *) NULL) &&
       (image->scene == 0))
    (void) fprintf(stderr,"%s ",image->filename);
  else
    (void) fprintf(stderr,"%s[%u] ",image->filename,image->scene);
  if ((image->magick_columns != 0) || (image->magick_rows != 0))
    if ((image->magick_columns != image->columns) ||
        (image->magick_rows != image->rows))
      (void) fprintf(stderr,"%ux%u=>",image->magick_columns,image->magick_rows);
  (void) fprintf(stderr,"%ux%u ",image->columns,image->rows);
  if (image->class == DirectClass)
    {
      (void) fprintf(stderr,"DirectClass ");
      if (image->total_colors != 0)
        (void) fprintf(stderr,"%luc ",image->total_colors);
    }
  else
    if (image->total_colors <= image->colors)
      (void) fprintf(stderr,"PseudoClass %uc ",image->colors);
    else
      {
        (void) fprintf(stderr,"PseudoClass %lu=>%uc ",image->total_colors,
          image->colors);
        (void) fprintf(stderr,"%u/%.6f/%.6fe ",image->mean_error_per_pixel,
          image->normalized_mean_error,image->normalized_maximum_error);
      }
  if (image->filesize != 0)
    (void) fprintf(stderr,"%lub ",image->filesize);
  (void) fprintf(stderr,"%s %lds\n",image->magick,time((time_t *) NULL)-
    image->magick_time+1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D e s p e c k l e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function DespeckleImage creates a new image that is a copy of an existing
%  one with the speckle noise reduced.  It uses the eight hull algorithm
%  described in Applied Optics, Vol. 24, No. 10, 15 May 1985, "Geometric filter
%  for Speckle Reduction", by Thomas R Crimmins.  Each pixel in the image is
%  replaced by one of its eight of its surrounding pixels using a polarity and
%  negative hull function.  DespeckleImage allocates the memory necessary for
%  the new Image structure and returns a pointer to the new image.
%
%  The format of the DespeckleImage routine is:
%
%      despeckled_image=DespeckleImage(image)
%
%  A description of each parameter follows:
%
%    o despeckled_image: Function DespeckleImage returns a pointer to the image
%      after it is despeckled.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/

static void Hull(x_offset,y_offset,polarity,columns,rows,f,g)
int
  x_offset,
  y_offset,
  polarity;

unsigned int
  columns,
  rows;

unsigned char
  *f,
  *g;
{
  int
    y;

  register int
    x;

  register unsigned char
    *p,
    *q,
    *r,
    *s;

  unsigned char
    v;

  p=f+(columns+2);
  q=g+(columns+2);
  r=p+(y_offset*((int) columns+2)+x_offset);
  for (y=0; y < rows; y++)
  {
    p++;
    q++;
    r++;
    if (polarity > 0)
      for (x=0; x < columns; x++)
      {
        v=(*p);
        if (*r > v)
          v++;
        *q=v;
        p++;
        q++;
        r++;
      }
    else
      for (x=0; x < columns; x++)
      {
        v=(*p);
        if (v > (unsigned char) (*r+1))
          v--;
        *q=v;
        p++;
        q++;
        r++;
      }
    p++;
    q++;
    r++;
  }
  p=f+(columns+2);
  q=g+(columns+2);
  r=q+(y_offset*((int) columns+2)+x_offset);
  s=q-(y_offset*((int) columns+2)+x_offset);
  for (y=0; y < rows; y++)
  {
    p++;
    q++;
    r++;
    s++;
    if (polarity > 0)
      for (x=0; x < columns; x++)
      {
        v=(*q);
        if (((unsigned char) (*s+1) > v) && (*r > v))
          v++;
        *p=v;
        p++;
        q++;
        r++;
        s++;
      }
    else
      for (x=0; x < columns; x++)
      {
        v=(*q);
        if (((unsigned char) (*s+1) < v) && (*r < v))
          v--;
        *p=v;
        p++;
        q++;
        r++;
        s++;
      }
    p++;
    q++;
    r++;
    s++;
  }
}

Image *DespeckleImage(image)
Image
  *image;
{
  Image
    *despeckled_image;

  int
    x;

  register int
    i,
    j;

  register RunlengthPacket
    *p,
    *q;

  static int
    X[4]= {0, 1, 1,-1},
    Y[4]= {1, 0, 1, 1};

  unsigned char
    *blue_channel,
    *buffer,
    *green_channel,
    *red_channel;

  unsigned int
    packets;

  /*
    Allocate despeckled image.
  */
  despeckled_image=CopyImage(image,image->columns,image->rows,False);
  if (despeckled_image == (Image *) NULL)
    {
      Warning("Unable to despeckle image","Memory allocation failed");
      return((Image *) NULL);
    }
  despeckled_image->class=DirectClass;
  /*
    Allocate image buffers.
  */
  packets=(image->columns+2)*(image->rows+2);
  red_channel=(unsigned char *) malloc(packets);
  green_channel=(unsigned char *) malloc(packets);
  blue_channel=(unsigned char *) malloc(packets);
  buffer=(unsigned char *) malloc(packets);
  if ((red_channel == (unsigned char *) NULL) ||
      (green_channel == (unsigned char *) NULL) ||
      (blue_channel == (unsigned char *) NULL) ||
      (buffer == (unsigned char *) NULL) || !UncompressImage(image))
    {
      Warning("Unable to despeckle image","Memory allocation failed");
      DestroyImage(despeckled_image);
      return((Image *) NULL);
    }
  /*
    Zero image buffers.
  */
  for (i=0; i < packets; i++)
  {
    red_channel[i]=0;
    green_channel[i]=0;
    blue_channel[i]=0;
    buffer[i]=0;
  }
  /*
    Copy image pixels to color component buffers
  */
  x=image->columns+2;
  p=image->pixels;
  for (j=0; j < image->rows; j++)
  {
    x++;
    for (i=0; i < image->columns; i++)
    {
      red_channel[x]=p->red;
      green_channel[x]=p->green;
      blue_channel[x]=p->blue;
      x++;
      p++;
    }
    x++;
  }
  /*
    Reduce speckle in red channel.
  */
  for (i=0; i < 4; i++)
  {
    Hull(X[i],Y[i],1,image->columns,image->rows,red_channel,buffer);
    Hull(-X[i],-Y[i],1,image->columns,image->rows,red_channel,buffer);
    Hull(-X[i],-Y[i],-1,image->columns,image->rows,red_channel,buffer);
    Hull(X[i],Y[i],-1,image->columns,image->rows,red_channel,buffer);
  }
  /*
    Reduce speckle in green channel.
  */
  for (i=0; i < packets; i++)
    buffer[i]=0;
  for (i=0; i < 4; i++)
  {
    Hull(X[i],Y[i],1,image->columns,image->rows,green_channel,buffer);
    Hull(-X[i],-Y[i],1,image->columns,image->rows,green_channel,buffer);
    Hull(-X[i],-Y[i],-1,image->columns,image->rows,green_channel,buffer);
    Hull(X[i],Y[i],-1,image->columns,image->rows,green_channel,buffer);
  }
  /*
    Reduce speckle in blue channel.
  */
  for (i=0; i < packets; i++)
    buffer[i]=0;
  for (i=0; i < 4; i++)
  {
    Hull(X[i],Y[i],1,image->columns,image->rows,blue_channel,buffer);
    Hull(-X[i],-Y[i],1,image->columns,image->rows,blue_channel,buffer);
    Hull(-X[i],-Y[i],-1,image->columns,image->rows,blue_channel,buffer);
    Hull(X[i],Y[i],-1,image->columns,image->rows,blue_channel,buffer);
  }
  /*
    Copy color component buffers to despeckled image.
  */
  x=image->columns+2;
  q=despeckled_image->pixels;
  for (j=0; j < image->rows; j++)
  {
    x++;
    for (i=0; i < image->columns; i++)
    {
      q->red=red_channel[x];
      q->green=green_channel[x];
      q->blue=blue_channel[x];
      q->index=0;
      q->length=0;
      q++;
      x++;
    }
    x++;
  }
  /*
    Free memory.
  */
  (void) free((char *) buffer);
  (void) free((char *) blue_channel);
  (void) free((char *) green_channel);
  (void) free((char *) red_channel);
  return(despeckled_image);
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
  /*
    Close image.
  */
  CloseImage(image);
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
  while (image->previous != (Image *) NULL)
    image=image->previous;
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
%     E m b o s s I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function EmbossImage creates a new image that is a copy of an existing
%  one with the edge highlighted.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  EmbossImage convolves the pixel neighborhood with this edge detection mask:
%
%    -1 -2  0
%    -2  0  2
%     0  2  1
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the EmbossImage routine is:
%
%      embossed_image=EmbossImage(image)
%
%  A description of each parameter follows:
%
%    o embossed_image: Function EmbossImage returns a pointer to the image
%      after it is embossed.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Image *EmbossImage(image)
Image
  *image;
{
#define Emboss(weight) \
  total_red+=(weight)*(int) (s->red); \
  total_green+=(weight)*(int) (s->green); \
  total_blue+=(weight)*(int) (s->blue); \
  s++;

  Image
    *embossed_image;

  long int
    total_blue,
    total_green,
    total_red;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2;

  register unsigned int
    x;

  RunlengthPacket
    background_pixel,
    *scanline;

  unsigned int
    y;

  if ((image->columns < 3) || (image->rows < 3))
    {
      Warning("Unable to emboss image","image size must exceed 3x3");
      return((Image *) NULL);
    }
  /*
    Initialize embossed image attributes.
  */
  embossed_image=CopyImage(image,image->columns,image->rows,False);
  if (embossed_image == (Image *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      return((Image *) NULL);
    }
  embossed_image->class=DirectClass;
  /*
    Allocate scan line buffer for 3 rows of the image.
  */
  scanline=(RunlengthPacket *) malloc(3*image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      DestroyImage(embossed_image);
      return((Image *) NULL);
    }
  /*
    Read the first two rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Dump first scanlines of image.
  */
  background_pixel.red=0;
  background_pixel.green=0;
  background_pixel.blue=0;
  background_pixel.index=0;
  background_pixel.length=0;
  q=embossed_image->pixels;
  for (x=0; x < image->columns; x++)
  {
    *q=background_pixel;
    q++;
  }
  /*
    Convolve each row.
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    *q=background_pixel;
    q++;
    for (x=1; x < (image->columns-1); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      s=s1+1;
      s=s0;
      Emboss(-1); Emboss(-2); Emboss( 0);
      s=s1;
      Emboss(-2); Emboss( 0); Emboss( 2);
      s=s2;
      Emboss( 0); Emboss( 2); Emboss( 1);
      total_red+=(MaxRGB+1) >> 1;
      if (total_red < 0)
        total_red=0;
      else
        if (total_red > MaxRGB)
          total_red=MaxRGB;
      total_green+=(MaxRGB+1) >> 1;
      if (total_green < 0)
        total_green=0;
      else
        if (total_green > MaxRGB)
          total_green=MaxRGB;
      total_blue+=(MaxRGB+1) >> 1;
      if (total_blue < 0)
        total_blue=0;
      else
        if (total_blue > MaxRGB)
          total_blue=MaxRGB;
      q->red=(unsigned char) total_red;
      q->green=(unsigned char) total_green;
      q->blue=(unsigned char) total_blue;
      q->index=0;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
    }
    /*
      Transfer last pixel of the scanline.
    */
    *q=background_pixel;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  for (x=0; x < image->columns; x++)
  {
    *q=background_pixel;
    q++;
  }
  (void) free((char *) scanline);
  /*
    Convert image to Grayscale and normalize.
  */
  (void) IsGrayImage(embossed_image);
  NormalizeImage(embossed_image);
  return(embossed_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E d g e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function EdgeImage creates a new image that is a copy of an existing
%  one with the edges highlighted.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  EdgeImage convolves the pixel neighborhood with this edge detection mask:
%
%    -1 -2  0
%    -2  0  2
%     0  2  1
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the EdgeImage routine is:
%
%      edged_image=EdgeImage(image)
%
%  A description of each parameter follows:
%
%    o edged_image: Function EdgeImage returns a pointer to the image
%      after it is edged.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Image *EdgeImage(image)
Image
  *image;
{
#define Edge(weight) \
  total_red+=(weight)*(int) (s->red); \
  total_green+=(weight)*(int) (s->green); \
  total_blue+=(weight)*(int) (s->blue); \
  s++;

  Image
    *edged_image;

  long int
    total_blue,
    total_green,
    total_red;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2;

  register unsigned int
    x;

  RunlengthPacket
    background_pixel,
    *scanline;

  unsigned int
    y;

  if ((image->columns < 3) || (image->rows < 3))
    {
      Warning("Unable to detect edges","image size must exceed 3x3");
      return((Image *) NULL);
    }
  /*
    Initialize edged image attributes.
  */
  edged_image=CopyImage(image,image->columns,image->rows,False);
  if (edged_image == (Image *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      return((Image *) NULL);
    }
  edged_image->class=DirectClass;
  /*
    Allocate scan line buffer for 3 rows of the image.
  */
  scanline=(RunlengthPacket *) malloc(3*image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      DestroyImage(edged_image);
      return((Image *) NULL);
    }
  /*
    Read the first two rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Dump first scanlines of image.
  */
  background_pixel.red=0;
  background_pixel.green=0;
  background_pixel.blue=0;
  background_pixel.index=0;
  background_pixel.length=0;
  q=edged_image->pixels;
  for (x=0; x < image->columns; x++)
  {
    *q=background_pixel;
    q++;
  }
  /*
    Convolve each row.
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    *q=background_pixel;
    q++;
    for (x=1; x < (image->columns-1); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      s=s1+1;
      s=s0;
      Edge(-1); Edge(-2); Edge( 0);
      s=s1;
      Edge(-2); Edge( 0); Edge( 2);
      s=s2;
      Edge( 0); Edge( 2); Edge( 1);
      if (total_red < 0)
        total_red=(-total_red);
      if (total_red > MaxRGB)
        total_red=MaxRGB;
      if (total_green < 0)
        total_green=(-total_green);
      if (total_green > MaxRGB)
        total_green=MaxRGB;
      if (total_blue < 0)
        total_blue=(-total_blue);
      if (total_blue > MaxRGB)
        total_blue=MaxRGB;
      q->red=(unsigned char) total_red;
      q->green=(unsigned char) total_green;
      q->blue=(unsigned char) total_blue;
      q->index=0;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
    }
    /*
      Transfer last pixel of the scanline.
    */
    *q=background_pixel;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  for (x=0; x < image->columns; x++)
  {
    *q=background_pixel;
    q++;
  }
  (void) free((char *) scanline);
  /*
    Normalize image.
  */
  NormalizeImage(edged_image);
  return(edged_image);
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
%  Weights assume that the importance of neighboring pixels is negately
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
#define Enhance(weight) \
  red_distance=s->red-red; \
  green_distance=s->green-green; \
  blue_distance=s->blue-blue; \
  distance=red_distance*red_distance+green_distance*green_distance+ \
    blue_distance*blue_distance; \
  if (distance < Threshold) \
    { \
      total_red+=(weight)*(s->red); \
      total_green+=(weight)*(s->green); \
      total_blue+=(weight)*(s->blue); \
      total_weight+=(weight); \
    } \
  s++;
#define Threshold  2500

  Image
    *enhanced_image;

  int
    blue_distance,
    green_distance,
    red_distance;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2,
    *s3,
    *s4;

  register unsigned int
    x;

  RunlengthPacket
    *scanline;

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
      Warning("Unable to enhance image","image size must exceed 4x4");
      return((Image *) NULL);
    }
  /*
    Initialize enhanced image attributes.
  */
  enhanced_image=CopyImage(image,image->columns,image->rows,False);
  if (enhanced_image == (Image *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      return((Image *) NULL);
    }
  enhanced_image->class=DirectClass;
  /*
    Allocate scan line buffer for 5 rows of the image.
  */
  scanline=(RunlengthPacket *) malloc(5*image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
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
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Dump first 2 scanlines of image.
  */
  q=enhanced_image->pixels;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    *q=(*s);
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Transfer first 2 pixels of the scanline.
    */
    s=s2;
    for (x=0; x < 2; x++)
    {
      *q=(*s);
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
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
      s=s1;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      s=s2;
      Enhance(10); Enhance(40); Enhance(80); Enhance(40); Enhance(10);
      s=s3;
      Enhance(8);  Enhance(20); Enhance(40); Enhance(20); Enhance(8);
      s=s4;
      Enhance(5);  Enhance(8);  Enhance(10); Enhance(8);  Enhance(5);
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
      *q=(*s);
      q->length=0;
      q++;
      s++;
    }
  }
  /*
    Dump last 2 scanlines of pixels.
  */
  s=scanline+image->columns*(y % 5);
  for (x=0; x < (image->columns << 1); x++)
  {
    *q=(*s);
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
%     E q u a l i z e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function EqualizeImage performs histogram equalization on the reference
%  image.
%
%  The format of the EqualizeImage routine is:
%
%      EqualizeImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void EqualizeImage(image)
Image
  *image;
{
  register int
    i,
    j;

  register RunlengthPacket
    *p;

  unsigned char
    *equalize_map;

  unsigned int
    high,
    *histogram,
    low,
    *map;

  /*
    Allocate and initialize histogram arrays.
  */
  histogram=(unsigned int *) malloc((MaxRGB+1)*sizeof(unsigned int));
  map=(unsigned int *) malloc((MaxRGB+1)*sizeof(unsigned int));
  equalize_map=(unsigned char *) malloc((MaxRGB+1)*sizeof(unsigned char));
  if ((histogram == (unsigned int *) NULL) ||
      (map == (unsigned int *) NULL) ||
      (equalize_map == (unsigned char *) NULL))
    {
      Warning("Unable to equalize image","Memory allocation failed");
      return;
    }
  /*
    Form histogram.
  */
  for (i=0; i <= MaxRGB; i++)
    histogram[i]=0;
  p=image->pixels;
  for (i=0; i < image->packets; i++)
  {
    histogram[Intensity(*p)]+=(p->length+1);
    p++;
  }
  /*
    Integrate the histogram to get the equalization map.
  */
  j=0;
  for (i=0; i <= MaxRGB; i++)
  {
    j+=histogram[i];
    map[i]=j;
  }
  (void) free((char *) histogram);
  if (map[MaxRGB] == 0)
    {
      (void) free((char *) equalize_map);
      (void) free((char *) map);
      return;
    }
  /*
    Equalize.
  */
  low=map[0];
  high=map[MaxRGB];
  for (i=0; i <= MaxRGB; i++)
    equalize_map[i]=(unsigned char) ((((map[i]-low))*MaxRGB)/Max(high-low,1));
  (void) free((char *) map);
  /*
    Stretch the histogram.
  */
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Equalize DirectClass packets.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        p->red=equalize_map[p->red];
        p->green=equalize_map[p->green];
        p->blue=equalize_map[p->blue];
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Equalize PseudoClass packets.
      */
      for (i=0; i < image->colors; i++)
      {
        image->colormap[i].red=equalize_map[image->colormap[i].red];
        image->colormap[i].green=equalize_map[image->colormap[i].green];
        image->colormap[i].blue=equalize_map[image->colormap[i].blue];
      }
      SyncImage(image);
      break;
    }
  }
  (void) free((char *) equalize_map);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l i p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function FlipImage creates a new image that reflects each scanline in the
%  vertical direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlipImage routine is:
%
%      flipped_image=FlipImage(image)
%
%  A description of each parameter follows:
%
%    o flipped_image: Function FlipImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Image *FlipImage(image)
Image
  *image;
{
  Image
    *flipped_image;

  register RunlengthPacket
    *p,
    *q,
    *s;

  register unsigned int
    x,
    y;

  RunlengthPacket
    *scanline;

  /*
    Initialize flipped image attributes.
  */
  flipped_image=CopyImage(image,image->columns,image->rows,False);
  if (flipped_image == (Image *) NULL)
    {
      Warning("Unable to flip image","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(RunlengthPacket *) malloc(image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to reflect image","Memory allocation failed");
      DestroyImage(flipped_image);
      return((Image *) NULL);
    }
  /*
    Flip each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=flipped_image->pixels+flipped_image->packets-1;
  for (y=0; y < flipped_image->rows; y++)
  {
    /*
      Read a scan line.
    */
    s=scanline;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Flip each column.
    */
    s=scanline+image->columns;
    for (x=0; x < flipped_image->columns; x++)
    {
      s--;
      *q=(*s);
      q->length=0;
      q--;
    }
  }
  (void) free((char *) scanline);
  return(flipped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F l o p I m a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function FlopImage creates a new image that reflects each scanline in the
%  horizontal direction It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FlopImage routine is:
%
%      flopped_image=FlopImage(image)
%
%  A description of each parameter follows:
%
%    o flopped_image: Function FlopImage returns a pointer to the image
%      after reflecting.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image.
%
%
*/
Image *FlopImage(image)
Image
  *image;
{
  Image
    *flopped_image;

  register RunlengthPacket
    *p,
    *q,
    *s;

  register unsigned int
    x,
    y;

  RunlengthPacket
    *scanline;

  /*
    Initialize flopped image attributes.
  */
  flopped_image=CopyImage(image,image->columns,image->rows,False);
  if (flopped_image == (Image *) NULL)
    {
      Warning("Unable to reflect image","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(RunlengthPacket *) malloc(image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to reflect image","Memory allocation failed");
      DestroyImage(flopped_image);
      return((Image *) NULL);
    }
  /*
    Flop each row.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=flopped_image->pixels;
  for (y=0; y < flopped_image->rows; y++)
  {
    /*
      Read a scan line.
    */
    s=scanline;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Flop each column.
    */
    s=scanline+image->columns;
    for (x=0; x < flopped_image->columns; x++)
    {
      s--;
      *q=(*s);
      q->length=0;
      q++;
    }
  }
  (void) free((char *) scanline);
  return(flopped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F r a m e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function FrameImage takes an image and puts a frame around it of a
%  particular color.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.
%
%  The format of the FrameImage routine is:
%
%      framed_image=FrameImage(image,frame_info,bevel_width,matte_color,
%        highlight_color,shadow_color)
%
%  A description of each parameter follows:
%
%    o framed_image: Function FrameImage returns a pointer to the framed
%      image.  A null image is returned if there is a a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o frame_info: Specifies a pointer to a XRectangle which defines the
%      framed region.
%
%    o bevel_width: An unsigned integer that is the width of the border of
%      the frame.
%
%    o matte_color: A pointer to a ColorPacket which contains the red,
%      green, and blue components of the border color.
%
%    o highlight_color: A pointer to a ColorPacket which contains the red,
%      green, and blue components of the highlight color.
%
%    o shadow_color: A pointer to a ColorPacket which contains the red,
%      green, and blue components of the shadow color.
%
%
*/
Image *FrameImage(image,frame_info,bevel_width,matte_color,highlight_color,
  shadow_color)
Image
  *image;

RectangleInfo
  *frame_info;

unsigned int
  bevel_width;

ColorPacket
  *matte_color,
  *highlight_color,
  *shadow_color;
{
  Image
    *framed_image;

  int
    height,
    width;

  register int
    x,
    y;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    black,
    highlight,
    matte,
    shadow;

  /*
    Check frame geometry.
  */
  width=(int) frame_info->width-frame_info->x-(bevel_width << 1);
  height=(int) frame_info->height-frame_info->y-(bevel_width << 1);
  if ((width < image->columns) || (height < image->rows))
    {
      Warning("Unable to frame image","frame is less than image size");
      return((Image *) NULL);
    }
  /*
    Initialize framed image attributes.
  */
  framed_image=CopyImage(image,frame_info->width,frame_info->height,False);
  if (framed_image == (Image *) NULL)
    {
      Warning("Unable to border image","Memory allocation failed");
      return((Image *) NULL);
    }
  image->class=DirectClass;
  /*
    Initialize frame colors.
  */
  black.red=0;
  black.green=0;
  black.blue=0;
  black.index=0;
  black.length=0;
  matte.red=matte_color->red;
  matte.green=matte_color->green;
  matte.blue=matte_color->blue;
  matte.index=0;
  matte.length=0;
  highlight.red=highlight_color->red;
  highlight.green=highlight_color->green;
  highlight.blue=highlight_color->blue;
  highlight.index=0;
  highlight.length=0;
  shadow.red=shadow_color->red;
  shadow.green=shadow_color->green;
  shadow.blue=shadow_color->blue;
  shadow.index=0;
  shadow.length=0;
  /*
    Copy image and put an ornamental border around it.
  */
  q=framed_image->pixels;
  for (x=0; x < framed_image->columns; x++)
    *q++=black;
  for (y=0; y < (bevel_width-1); y++)
  {
    *q++=black;
    for (x=0; x < (framed_image->columns-y-2); x++)
      *q++=highlight;
    for ( ; x < (framed_image->columns-2); x++)
      *q++=shadow;
    *q++=black;
  }
  for (y=0; y < (frame_info->y-(bevel_width << 1)); y++)
  {
    *q++=black;
    for (x=0; x < (bevel_width-1); x++)
      *q++=highlight;
    for (x=0; x < (framed_image->columns-(bevel_width << 1)); x++)
      *q++=matte;
    for (x=0; x < (bevel_width-1); x++)
      *q++=shadow;
    *q++=black;
  }
  for (y=0; y < (bevel_width-1); y++)
  {
    *q++=black;
    for (x=0; x < (bevel_width-1); x++)
      *q++=highlight;
    for (x=0; x < (frame_info->x-(bevel_width << 1)); x++)
      *q++=matte;
    for (x=0; x < (image->columns+(bevel_width << 1)-y); x++)
      *q++=shadow;
    for ( ; x < (image->columns+(bevel_width << 1)); x++)
      *q++=highlight;
    width=frame_info->width-frame_info->x-image->columns-(bevel_width << 1);
    for (x=0; x < width; x++)
      *q++=matte;
    for (x=0; x < (bevel_width-1); x++)
      *q++=shadow;
    *q++=black;
  }
  *q++=black;
  for (x=0; x < (bevel_width-1); x++)
    *q++=highlight;
  for (x=0; x < (frame_info->x-(bevel_width << 1)); x++)
    *q++=matte;
  for (x=0; x < (bevel_width-1); x++)
    *q++=shadow;
  for (x=0; x < (image->columns+2); x++)
    *q++=black;
  for (x=0; x < (bevel_width-1); x++)
    *q++=highlight;
  width=frame_info->width-frame_info->x-image->columns-(bevel_width << 1);
  for (x=0; x < width; x++)
    *q++=matte;
  for (x=0; x < (bevel_width-1); x++)
    *q++=shadow;
  *q++=black;
  p=image->pixels;
  image->runlength=p->length+1;
  for (y=0; y < image->rows; y++)
  {
    /*
      Initialize scanline with border color.
    */
    *q++=black;
    for (x=0; x < (bevel_width-1); x++)
      *q++=highlight;
    for (x=0; x < (frame_info->x-(bevel_width << 1)); x++)
      *q++=matte;
    for (x=0; x < (bevel_width-1); x++)
      *q++=shadow;
    *q++=black;
    /*
      Transfer scanline.
    */
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *q=(*p);
      q->length=0;
      q++;
    }
    *q++=black;
    for (x=0; x < (bevel_width-1); x++)
      *q++=highlight;
    width=frame_info->width-frame_info->x-image->columns-(bevel_width << 1);
    for (x=0; x < width; x++)
      *q++=matte;
    for (x=0; x < (bevel_width-1); x++)
      *q++=shadow;
    *q++=black;
  }
  *q++=black;
  for (x=0; x < (bevel_width-1); x++)
    *q++=highlight;
  for (x=0; x < (frame_info->x-(bevel_width << 1)); x++)
    *q++=matte;
  for (x=0; x < (bevel_width-1); x++)
    *q++=shadow;
  for (x=0; x < (image->columns+2); x++)
    *q++=black;
  for (x=0; x < (bevel_width-1); x++)
    *q++=highlight;
  width=frame_info->width-frame_info->x-image->columns-(bevel_width << 1);
  for (x=0; x < width; x++)
    *q++=matte;
  for (x=0; x < (bevel_width-1); x++)
    *q++=shadow;
  *q++=black;
  for (y=bevel_width-2; y >= 0; y--)
  {
    *q++=black;
    for (x=0; x < (bevel_width-1); x++)
      *q++=highlight;
    for (x=0; x < (frame_info->x-(bevel_width << 1)); x++)
      *q++=matte;
    for (x=0; x < y; x++)
      *q++=shadow;
    for ( ; x < (image->columns+(bevel_width << 1)); x++)
      *q++=highlight;
    width=frame_info->width-frame_info->x-image->columns-(bevel_width << 1);
    for (x=0; x < width; x++)
      *q++=matte;
    for (x=0; x < (bevel_width-1); x++)
      *q++=shadow;
    *q++=black;
  }
  height=frame_info->height-frame_info->y-image->rows-(bevel_width << 1);
  for (y=0; y < height; y++)
  {
    *q++=black;
    for (x=0; x < (bevel_width-1); x++)
      *q++=highlight;
    for (x=0; x < (framed_image->columns-(bevel_width << 1)); x++)
      *q++=matte;
    for (x=0; x < (bevel_width-1); x++)
      *q++=shadow;
    *q++=black;
  }
  for (y=bevel_width-2; y >= 0; y--)
  {
    *q++=black;
    for (x=0; x < y; x++)
      *q++=highlight;
    for ( ; x < (framed_image->columns-2); x++)
      *q++=shadow;
    *q++=black;
  }
  for (x=0; x < framed_image->columns; x++)
    *q++=black;
  return(framed_image);
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
%  Function GammaImage converts the reference image to gamma corrected colors.
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
%    o gamma: A character string indicating the level of gamma correction.
%
%
*/
void GammaImage(image,gamma)
Image
  *image;

char
  *gamma;
{
  ColorPacket
    *gamma_map;

  double
    blue_gamma,
    green_gamma,
    red_gamma;

  int
    count;

  register int
    i;

  register RunlengthPacket
    *p;

  /*
    Allocate and initialize gamma maps.
  */
  gamma_map=(ColorPacket *) malloc((MaxRGB+1)*sizeof(ColorPacket));
  if (gamma_map == (ColorPacket *) NULL)
    {
      Warning("Unable to gamma image","Memory allocation failed");
      return;
    }
  for (i=0; i <= MaxRGB; i++)
  {
    gamma_map[i].red=0;
    gamma_map[i].green=0;
    gamma_map[i].blue=0;
  }
  /*
    Initialize gamma table.
  */
  red_gamma=1.0;
  green_gamma=1.0;
  blue_gamma=1.0;
  count=sscanf(gamma,"%lf,%lf,%lf",&red_gamma,&green_gamma,&blue_gamma);
  if (count == 1)
    {
      green_gamma=red_gamma;
      blue_gamma=red_gamma;
    }
  for (i=0; i <= MaxRGB; i++)
  {
    if (red_gamma != 0.0)
      gamma_map[i].red=(unsigned char)
        ((pow((double) i/MaxRGB,1.0/red_gamma)*MaxRGB)+0.5);
    if (green_gamma != 0.0)
      gamma_map[i].green=(unsigned char)
        ((pow((double) i/MaxRGB,1.0/green_gamma)*MaxRGB)+0.5);
    if (blue_gamma != 0.0)
      gamma_map[i].blue=(unsigned char)
        ((pow((double) i/MaxRGB,1.0/blue_gamma)*MaxRGB)+0.5);
  }
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
        p->red=gamma_map[p->red].red;
        p->green=gamma_map[p->green].green;
        p->blue=gamma_map[p->blue].blue;
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
        image->colormap[i].red=gamma_map[image->colormap[i].red].red;
        image->colormap[i].green=gamma_map[image->colormap[i].green].blue;
        image->colormap[i].blue=gamma_map[image->colormap[i].blue].green;
      }
      SyncImage(image);
      break;
    }
  }
  (void) free((char *) gamma_map);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function GetImageInfo initializes the ImageInfo structure.
%
%  The format of the GetImageInfo routine is:
%
%      GetImageInfo(image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%
*/
void GetImageInfo(image_info)
ImageInfo
  *image_info;
{
  image_info->filename=(char *) malloc(MaxTextLength*sizeof(char));
  if (image_info->filename == (char *) NULL)
    Error("Unable to get image info","Memory allocation failed");
  *image_info->filename='\0';
  image_info->assert=False;
  image_info->subimage=0;
  image_info->server_name=(char *) NULL;
  image_info->font=(char *) NULL;
  image_info->size=(char *) NULL;
  image_info->density=(char *) NULL;
  image_info->page=(char *) NULL;
  image_info->dither=True;
  image_info->interlace=NoneInterlace;
  image_info->monochrome=False;
  image_info->quality=85;
  image_info->verbose=False;
  image_info->undercolor=(char *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     I s G r a y I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function IsGrayImage returns True if the image is grayscale otherwise
%  False is returned.  If the image is DirectClass and grayscale, it is demoted
%  to PseudoClass.
%
%  The format of the IsGrayImage routine is:
%
%      status=IsGrayImage(image)
%
%  A description of each parameter follows:
%
%    o status: Function IsGrayImage returns True if the image is grayscale
%      otherwise False is returned.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
unsigned int IsGrayImage(image)
Image
  *image;
{
  register int
    i;

  unsigned int
    grayscale;

  /*
    Determine if image is grayscale.
  */
  grayscale=True;
  switch (image->class)
  {
    case DirectClass:
    {
      register RunlengthPacket
        *p;

      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        if ((p->red != p->green) || (p->green != p->blue))
          {
            grayscale=False;
            break;
          }
        p++;
      }
      if (grayscale)
        {
          QuantizeImage(image,256,8,False,GRAYColorspace);
          SyncImage(image);
        }
      break;
    }
    case PseudoClass:
    {
      for (i=0; i < image->colors; i++)
        if ((image->colormap[i].red != image->colormap[i].green) ||
            (image->colormap[i].green != image->colormap[i].blue))
          {
            grayscale=False;
            break;
          }
      break;
    }
  }
  return(grayscale);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a b e l I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function LabelImage initializes an image label.  Optionally the label
%  can include the image filename, type, width, height, or scene number by
%  embedding special format characters.  Embed %f for filename, %m for
%  magick, %w for width, %h for height, or %s for scene number.  For
%  example,
%
%     %f  %wx%h
%
%  produces an image label of
%
%     bird.miff  512x480
%
%  for an image titled bird.miff and whose width is 512 and height is 480.
%
%  The format of the LabelImage routine is:
%
%      LabelImage(image,label)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image.
%
%    o label: The address of a character string containing the label format.
%
%
*/
void LabelImage(image,label)
Image
  *image;

char
  *label;
{
  register char
    *p,
    *q;

  unsigned int
    indirection,
    length;

  if (image->label != (char *) NULL)
    (void) free((char *) image->label);
  image->label=(char *) NULL;
  if (label == (char *) NULL)
    return;
  indirection=(*label == '@');
  if (indirection)
    {
      FILE
        *file;

      int
        c;

      /*
        Read label from a file.
      */
      file=(FILE *) fopen(label+1,"r");
      if (file == (FILE *) NULL)
        {
          Warning("Unable to read label file",label+1);
          return;
        }
      length=MaxTextLength;
      label=(char *) malloc(length*sizeof(char));
      for (q=label ; label != (char *) NULL; q++)
      {
        c=fgetc(file);
        if (c == EOF)
          break;
        if ((q-label+1) >= length)
          {
            *q='\0';
            length<<=1;
            label=(char *) realloc((char *) label,length*sizeof(char));
            if (label == (char *) NULL)
              break;
            q=label+strlen(label);
          }
        *q=(unsigned char) c;
      }
      (void) fclose(file);
      if (label == (char *) NULL)
        {
          Warning("Unable to label image","Memory allocation failed");
          return;
        }
      *q='\0';
    }
  /*
    Allocate and initialize image label.
  */
  p=label;
  length=strlen(label)+MaxTextLength;
  image->label=(char *) malloc(length*sizeof(char));
  for (q=image->label; image->label != (char *) NULL; p++)
  {
    if (*p == '\0')
      break;
    if ((q-image->label+MaxTextLength) >= length)
      {
        *q='\0';
        length<<=1;
        image->label=(char *)
          realloc((char *) image->label,length*sizeof(char));
        if (image->label == (char *) NULL)
          break;
        q=image->label+strlen(image->label);
      }
    /*
      Process formatting characters in label.
    */
    if (*p != '%')
      {
        *q++=(*p);
        continue;
      }
    p++;
    switch (*p)
    {
      case 'f':
      {
        register char
          *p;

        /*
          Label segment is the base of the filename.
        */
        p=image->filename+strlen(image->filename)-1;
        while ((p > image->filename) && (*(p-1) != '/'))
          p--;
        (void) strcpy(q,p);
        q+=strlen(p);
        break;
      }
      case 'h':
      {
        (void) sprintf(q,"%u",image->rows);
        q=image->label+strlen(image->label);
        break;
      }
      case 'm':
      {
        (void) strcpy(q,image->magick);
        q+=strlen(image->magick);
        break;
      }
      case 's':
      {
        (void) sprintf(q,"%u",image->scene);
        q=image->label+strlen(image->label);
        break;
      }
      case 'w':
      {
        (void) sprintf(q,"%u",image->columns);
        q=image->label+strlen(image->label);
        break;
      }
      default:
      {
        *q++='%';
        *q++=(*p);
        break;
      }
    }
  }
  if (image->label == (char *) NULL)
    {
      Warning("Unable to label image","Memory allocation failed");
      return;
    }
  *q='\0';
  if (indirection)
    (void) free((char *) label);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o d u l a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ModulateImage modulates the brightness, saturation, and hue of an
%  image.
%
%  The format of the ModulateImage routine is:
%
%      ModulateImage(image,modulate)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o modulate: A character string indicating the percent change in hue,
%      saturation, and brightness.
%
%
%
*/

static void Modulate(percent_brightness,percent_saturation,percent_hue,
  red,green,blue)
double
  percent_brightness,
  percent_saturation,
  percent_hue;

unsigned char
  *red,
  *green,
  *blue;
{
  double
    brightness,
    hue,
    minimum,
    saturation;

  register double
    b,
    g,
    r;

  /*
    Convert RGB to HSV colorspace.
  */
  hue=(-1.0);
  saturation=0.0;
  r=(double) (*red)/(double) MaxRGB;
  g=(double) (*green)/(double) MaxRGB;
  b=(double) (*blue)/(double) MaxRGB;
  if (r >= g)
    {
      if (r >= b)
        brightness=r;
      else
        brightness=b;
    }
  else
    if (g >= b)
      brightness=g;
    else
      brightness=b;
  if (r <= g)
    {
      if (r <= b)
        minimum=r;
      else
        minimum=b;
    }
  else
    if (g <= b)
      minimum=g;
    else
      minimum=b;
  if (brightness != 0.0)
    saturation=(brightness-minimum)/brightness;
  if (saturation != 0.0)
    {
      if (r == brightness)
        hue=(g-b)/(brightness-minimum);
      else
        if (g == brightness)
          hue=2.0+(b-r)/(brightness-minimum);
        else
          if (b == brightness)
            hue=4.0+(r-g)/(brightness-minimum);
      hue=hue*60.0;
      if (hue < 0.0)
        hue+=360.0;
    }
  /*
    Increase or decrease color brightness, saturation, or hue.
  */
  brightness+=percent_brightness/100.0;
  if (brightness < 0.0)
    brightness=0.0;
  else
    if (brightness > 1.0)
      brightness=1.0;
  saturation+=percent_saturation/100.0;
  if (saturation < 0.0)
    saturation=0.0;
  else
    if (saturation > 1.0)
      saturation=1.0;
  if (hue != -1.0)
    {
      hue+=360.0*percent_hue/100.0;
      if (hue < 0.0)
        hue+=360.0;
      else
        if (hue > 360.0)
          hue-=360.0;
    }
  /*
    Convert HSV to RGB
  */
  r=brightness;
  g=brightness;
  b=brightness;
  if ((hue != -1.0) && (saturation != 0.0))
    {
      double
        f,
        j,
        k,
        l,
        v;

      int
        i;

      if (hue == 360.0)
        hue=0.0;
      hue=hue/60.0;
      i=floor(hue);
      if (i < 0)
        i=0;
      f=hue-i;
      j=brightness*(1.0-saturation);
      k=brightness*(1.0-(saturation*f));
      l=brightness*(1.0-(saturation*(1.0-f)));
      v=brightness;
      switch (i)
      {
        case 0:  r=v;  g=l;  b=j;  break;
        case 1:  r=k;  g=v;  b=j;  break;
        case 2:  r=j;  g=v;  b=l;  break;
        case 3:  r=j;  g=k;  b=v;  break;
        case 4:  r=l;  g=j;  b=v;  break;
        case 5:  r=v;  g=j;  b=k;  break;
      }
    }
  *red=(unsigned char) floor((r*(double) MaxRGB)+0.5);
  *green=(unsigned char) floor((g*(double) MaxRGB)+0.5);
  *blue=(unsigned char) floor((b*(double) MaxRGB)+0.5);
}

void ModulateImage(image,modulate)
Image
  *image;

char
  *modulate;
{
  double
    percent_brightness,
    percent_hue,
    percent_saturation;

  register int
    i;

  register RunlengthPacket
    *p;

  /*
    Initialize gamma table.
  */
  percent_brightness=0.0;
  percent_saturation=0.0;
  percent_hue=0.0;
  (void) sscanf(modulate,"%lf,%lf,%lf",&percent_brightness,&percent_saturation,
    &percent_hue);
  switch (image->class)
  {
    case DirectClass:
    {
      /*
        Modulate the color for a DirectClass image.
      */
      p=image->pixels;
      for (i=0; i < image->packets; i++)
      {
        Modulate(percent_brightness,percent_saturation,percent_hue,
          &p->red,&p->green,&p->blue);
        p++;
      }
      break;
    }
    case PseudoClass:
    {
      /*
        Modulate the color for a PseudoClass image.
      */
      for (i=0; i < image->colors; i++)
        Modulate(percent_brightness,percent_saturation,percent_hue,
          &image->colormap[i].red,&image->colormap[i].green,
          &image->colormap[i].blue);
      SyncImage(image);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o g r i f y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MogrifyImage applies image processing options to an image as
%  prescribed by command line options.
%
%  The format of the MogrifyImage routine is:
%
%      MogrifyImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void MogrifyImage(image_info,argc,argv,image)
ImageInfo
  *image_info;

int
  argc;

char
  **argv;

Image
  **image;
{
  ColorPacket
    border_color;

  char
    *option;

  int
    flags,
    x,
    y;

  register int
    i;

  unsigned int
    colorspace,
    number_colors,
    tree_depth;

  if (*image == (Image *) NULL)
    return;
  /*
    Initialize routine variables.
  */
  border_color.red=0;
  border_color.green=0;
  border_color.blue=0;
  number_colors=0;
  tree_depth=0;
  colorspace=RGBColorspace;
  if (image_info->monochrome)
    {
      number_colors=2;
      tree_depth=8;
      colorspace=GRAYColorspace;
    }
  /*
    Transmogrify the image.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if (((int) strlen(option) <= 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (strncmp("-blur",option,3) == 0)
      {
        Image
          *blurred_image;

        /*
          Blur an image.
        */
        blurred_image=BlurImage(*image);
        if (blurred_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=blurred_image;
          }
      }
    if (strcmp("-border",option) == 0)
      {
        Image
          *bordered_image;

        RectangleInfo
          border_info;

        /*
          Surround image with a border of solid color.
        */
        border_info.width=0;
        border_info.height=0;
        flags=XParseGeometry(argv[++i],&border_info.x,&border_info.y,
          &border_info.width,&border_info.height);
        if ((flags & HeightValue) == 0)
          border_info.height=border_info.width;
        bordered_image=BorderImage(*image,&border_info,&border_color);
        if (bordered_image != (Image *) NULL)
          {
            DestroyImage(*image);
            bordered_image->class=DirectClass;
            *image=bordered_image;
          }
      }
    if (strncmp("-bordercolor",option,8) == 0)
      {
        Display
          *display;

        /*
          Open X server.
        */
        display=XOpenDisplay(image_info->server_name);
        if (display != (Display *) NULL)
          {
            Colormap
              colormap;

            XColor
              color;

            /*
              Get border color.
            */
            colormap=XDefaultColormap(display,XDefaultScreen(display));
            (void) XParseColor(display,colormap,argv[++i],&color);
            border_color.red=ColorShift(color.red);
            border_color.green=ColorShift(color.green);
            border_color.blue=ColorShift(color.blue);
          }
      }
    if (strncmp("colors",option+1,7) == 0)
      number_colors=atoi(argv[++i]);
    if (strncmp("-colorspace",option,8) == 0)
      {
        i++;
        option=argv[i];
        if (Latin1Compare("gray",option) == 0)
          {
            colorspace=GRAYColorspace;
            if (number_colors == 0)
              number_colors=256;
            tree_depth=8;
          }
        if (Latin1Compare("ohta",option) == 0)
          colorspace=OHTAColorspace;
        if (Latin1Compare("rgb",option) == 0)
          colorspace=RGBColorspace;
        if (Latin1Compare("xyz",option) == 0)
          colorspace=XYZColorspace;
        if (Latin1Compare("ycbcr",option) == 0)
          colorspace=YCbCrColorspace;
        if (Latin1Compare("yiq",option) == 0)
          colorspace=YIQColorspace;
        if (Latin1Compare("ypbpr",option) == 0)
          colorspace=YPbPrColorspace;
        if (Latin1Compare("yuv",option) == 0)
          colorspace=YUVColorspace;
      }
    if (strncmp("comment",option+1,4) == 0)
      if (*option == '-')
        CommentImage(*image,argv[++i]);
      else
        CommentImage(*image,(char *) NULL);
    if (strncmp("contrast",option+1,3) == 0)
      ContrastImage(*image,*option == '-');
    if (strncmp("-crop",option,3) == 0)
      TransformImage(image,argv[++i],(char *) NULL);
    if (strncmp("-despeckle",option,4) == 0)
      {
        Image
          *despeckled_image;

        /*
          Reduce the speckles within an image.
        */
        despeckled_image=DespeckleImage(*image);
        if (despeckled_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=despeckled_image;
          }
      }
    if (strncmp("-edge",option,3) == 0)
      {
        Image
          *edged_image;

        /*
          Detect edges in the image.
        */
        edged_image=EdgeImage(*image);
        if (edged_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=edged_image;
          }
      }
    if (strncmp("-enhance",option,3) == 0)
      {
        Image
          *enhanced_image;

        /*
          Enhance image.
        */
        enhanced_image=EnhanceImage(*image);
        if (enhanced_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=enhanced_image;
          }
      }
    if (strncmp("-equalize",option,3) == 0)
      EqualizeImage(*image);
    if (strncmp("-flip",option,4) == 0)
      {
        Image
          *flipped_image;

        /*
          Flip image scanlines.
        */
        flipped_image=FlipImage(*image);
        if (flipped_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=flipped_image;
          }
      }
    if (strncmp("-flop",option,4) == 0)
      {
        Image
          *flopped_image;

        /*
          Flop image scanlines.
        */
        flopped_image=FlopImage(*image);
        if (flopped_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=flopped_image;
          }
      }
    if (strncmp("-gamma",option,3) == 0)
      GammaImage(*image,argv[++i]);
    if (strncmp("-geometry",option,4) == 0)
      TransformImage(image,(char *) NULL,argv[++i]);
    if (strncmp("label",option+1,2) == 0)
      if (*option == '-')
        LabelImage(*image,argv[++i]);
      else
        LabelImage(*image,(char *) NULL);
    if (strncmp("-modulate",option,4) == 0)
      ModulateImage(*image,argv[++i]);
    if (strncmp("-negate",option,4) == 0)
      NegateImage(*image);
    if (strncmp("-noise",option,4) == 0)
      {
        Image
          *noisy_image;

        /*
          Reduce noise in image.
        */
        noisy_image=NoisyImage(*image);
        if (noisy_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=noisy_image;
          }
      }
    if (strncmp("-normalize",option,4) == 0)
      NormalizeImage(*image);
    if (strncmp("-roll",option,4) == 0)
      {
        Image
          *rolled_image;

        unsigned int
          height,
          width;

        /*
          Roll image.
        */
        x=0;
        y=0;
        flags=XParseGeometry(argv[++i],&x,&y,&width,&height);
        rolled_image=RollImage(*image,x,y);
        if (rolled_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=rolled_image;
          }
      }
    if (strncmp("-rotate",option,4) == 0)
      {
        double
          degrees;

        Image
          *rotated_image;

        /*
          Rotate image.
        */
        degrees=atof(argv[++i]);
        rotated_image=RotateImage(*image,degrees,&border_color,False);
        if (rotated_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=rotated_image;
          }
      }
    if (strncmp("-sample",option,3) == 0)
      {
        Image
          *sampled_image;

        unsigned int
          height,
          width;

        /*
          Sample image with pixel replication.
        */
        width=(*image)->columns;
        height=(*image)->rows;
        ParseImageGeometry(argv[++i],&width,&height);
        sampled_image=SampleImage(*image,width,height);
        if (sampled_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=sampled_image;
          }
      }
    if (strncmp("-sharpen",option,4) == 0)
      {
        Image
          *sharpened_image;

        /*
          Sharpen an image.
        */
        sharpened_image=SharpenImage(*image);
        if (sharpened_image != (Image *) NULL)
          {
            DestroyImage(*image);
            *image=sharpened_image;
          }
      }
    if (strncmp("-shear",option,4) == 0)
      {
        float
          x_shear,
          y_shear;

        Image
          *sheared_image;

        /*
          Shear image.
        */
        x_shear=0.0;
        y_shear=0.0;
        (void) sscanf(argv[++i],"%fx%f",&x_shear,&y_shear);
        sheared_image=ShearImage(*image,(double) x_shear,
          (double) y_shear,&border_color,False);
        if (sheared_image != (Image *) NULL)
          {
            DestroyImage(*image);
            sheared_image->class=DirectClass;
            *image=sheared_image;
          }
      }
    if (strncmp("-treedepth",option,3) == 0)
      tree_depth=atoi(argv[++i]);
  }
  if (number_colors != 0)
    {
      /*
        Reduce the number of colors in the image.
      */
      if (((*image)->class == DirectClass) ||
          ((*image)->colors > number_colors) || (colorspace == GRAYColorspace))
        QuantizeImage(*image,number_colors,tree_depth,image_info->dither,
          colorspace);
      /*
        Measure quantization error.
      */
      if (image_info->verbose)
        QuantizationError(*image);
      SyncImage(*image);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     N e g a t e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function NegateImage negates the colors in the reference image.
%
%  The format of the NegateImage routine is:
%
%      NegateImage(image)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
void NegateImage(image)
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
        Negate DirectClass packets.
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
        Negate PseudoClass packets.
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
  Image
    *noisy_image;

  int
    i;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2;

  register unsigned int
    x;

  RunlengthPacket
    pixel,
    *scanline,
    window[9];

  unsigned int
    y;

  if ((image->columns < 3) || (image->rows < 3))
    {
      Warning("Unable to reduce noise","the image size must exceed 2x2");
      return((Image *) NULL);
    }
  /*
    Initialize noisy image attributes.
  */
  noisy_image=CopyImage(image,image->columns,image->rows,False);
  if (noisy_image == (Image *) NULL)
    {
      Warning("Unable to reduce noise","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scanline buffer for 3 rows of the image.
  */
  scanline=(RunlengthPacket *) malloc(3*image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to reduce noise","Memory allocation failed");
      DestroyImage(noisy_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 2 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Dump first scanline of image.
  */
  q=noisy_image->pixels;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    *q=(*s);
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    s=s1;
    *q=(*s);
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
      (void) qsort((void *) window,9,sizeof(RunlengthPacket),
        (int (*) _Declare((const void *, const void *))) NoisyCompare);
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
      *q=pixel;
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
    *q=(*s);
    q->length=0;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  s=scanline+image->columns*(y % 3);
  for (x=0; x < image->columns; x++)
  {
    *q=(*s);
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
%  Function NormalizeImage normalizes the pixel values to span the full
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
    normalize_map[MaxRGB+1];

  unsigned int
    high,
    low;

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
        normalize_map[i]=(MaxRGB-1)*(i-low)/(high-low);
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
%  Function OpenImage open a file associated with the image.  A file name of
%  '-' sets the file to stdin for type 'r' and stdout for type 'w'.  If the
%  filename suffix is '.gz' or '.Z', the image is decompressed for type 'r'
%  and compressed for type 'w'.  If the filename prefix is '|', it is piped
%  to or from a system command.
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
  char
    filename[MaxTextLength];

  (void) strcpy(filename,image->filename);
  if (*filename != '|')
    if (strcmp(filename+strlen(filename)-3,".gz") == 0)
      {
        /*
          Uncompress/compress image file with GNU compress utilities.
        */
        if (*type == 'r')
          (void) sprintf(filename,"|gunzip -f -c %s",image->filename);
        else
          (void) sprintf(filename,"|gzip -f -c > %s",image->filename);
      }
    else
      if (strcmp(filename+strlen(filename)-2,".Z") == 0)
        {
          /*
            Uncompress/compress image file with UNIX compress utilities.
          */
          if (*type == 'r')
            (void) sprintf(filename,"|uncompress -c %s",image->filename);
          else
            (void) sprintf(filename,"|compress -c > %s",image->filename);
        }
  /*
    Open image file.
  */
  image->pipe=False;
  if (strcmp(filename,"-") == 0)
    image->file=(*type == 'r') ? stdin : stdout;
  else
    if (*filename != '|')
      {
        if (*type == 'w')
          if ((image->previous != (Image *) NULL) ||
              (image->next != (Image *) NULL))
            {
              /*
                Form filename for multi-part images.
              */
              (void) sprintf(filename,image->filename,image->scene);
              if (strcmp(filename,image->filename) == 0)
                (void) sprintf(filename,"%s.%u",image->filename,image->scene);
              if (image->next != (Image *) NULL)
                (void) strcpy(image->next->magick,image->magick);
              (void) strcpy(image->filename,filename);
            }
        image->file=(FILE *) fopen(filename,type);
        if (image->file != (FILE *) NULL)
          {
            (void) fseek(image->file,0L,2);
            image->filesize=ftell(image->file);
            (void) fseek(image->file,0L,0);
          }
      }
    else
      {
        /*
          Pipe image to or from a system command.
        */
        if (*type == 'w')
          (void) signal(SIGPIPE,SIG_IGN);
        image->file=(FILE *) popen(filename+1,type);
        image->pipe=True;
      }
  image->status=False;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e I m a g e G e o m e t r y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ParseImageGeometry parse a geometry specification and returns the
%  width and height values.
%
%  The format of the ParseImageGeometry routine is:
%
%      ParseImageGeometry(image_geometry,width,height)
%
%  A description of each parameter follows:
%
%    o image_geometry:  Specifies a character string representing the geometry
%      specification.
%
%    o width:  A pointer to an unsigned integer.  The width as determined by
%      the geometry specification is returned here.
%
%    o height:  A pointer to an unsigned integer.  The height as determined by
%      the geometry specification is returned here.
%
%
*/
void ParseImageGeometry(image_geometry,width,height)
char
  *image_geometry;

unsigned int
  *width,
  *height;
{
  char
    geometry[MaxTextLength];

  int
    flags,
    x,
    y;

  register char
    *p;

  unsigned int
    aspect_ratio,
    former_height,
    former_width,
    percentage;

  unsigned long
    scale_factor;

  if (image_geometry == (char *) NULL)
    return;
  /*
    Remove whitespaces and % and ~ characters from geometry specification.
  */
  (void) strcpy(geometry,image_geometry);
  aspect_ratio=True;
  percentage=False;
  p=geometry;
  while ((int) strlen(p) > 0)
  {
    if (isspace(*p))
      (void) strcpy(p,p+1);
    else
      if (*p == '%')
        {
          percentage=True;
          (void) strcpy(p,p+1);
        }
      else
        if (*p == '!')
          {
            aspect_ratio=False;
            (void) strcpy(p,p+1);
          }
        else
          p++;
  }
  /*
    Parse geometry using XParseGeometry.
  */
  former_width=(*width);
  former_height=(*height);
  flags=XParseGeometry(geometry,&x,&y,width,height);
  if (((flags & WidthValue) != 0) && (flags & HeightValue) == 0)
    *height=(*width);
  if (percentage)
    {
      /*
        Geometry is a percentage of the image size.
      */
      *width=(former_width*(*width))/100;
      *height=(former_height*(*height))/100;
      former_width=(*width);
      former_height=(*height);
    }
  if (aspect_ratio)
    {
      /*
        Respect aspect ratio of the image.
      */
      scale_factor=UpShift(*width)/former_width;
      if (scale_factor > (UpShift(*height)/former_height))
        scale_factor=UpShift(*height)/former_height;
      *width=DownShift(former_width*scale_factor);
      *height=DownShift(former_height*scale_factor);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     O i l P a i n t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function OilPaintImage creates a new image that is a copy of an existing
%  one with each pixel component replaced with the color of greatest number in
%  a 5x5 neighborhood.
%
%  The format of the OilPaintImage routine is:
%
%      painted_image=OilPaintImage(image)
%
%  A description of each parameter follows:
%
%    o painted_image: Function OilPaintImage returns a pointer to the image
%      after it is `painted'.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Image *OilPaintImage(image)
Image
  *image;
{
  Image
    *painted_image;

  int
    count;

  register int
    i,
    j;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2,
    *s3,
    *s4;

  register unsigned int
    x;

  RunlengthPacket
    pixel,
    *scanline,
    window[25];

  unsigned int
    *histogram,
    y;

  if ((image->columns < 5) || (image->rows < 5))
    {
      Warning("Unable to oil paint","the image size must exceed 4x4");
      return((Image *) NULL);
    }
  /*
    Initialize painted image attributes.
  */
  painted_image=CopyImage(image,image->columns,image->rows,False);
  if (painted_image == (Image *) NULL)
    {
      Warning("Unable to oil paint","Memory allocation failed");
      return((Image *) NULL);
    }
  painted_image->class=DirectClass;
  /*
    Allocate histogram and scanline.
  */
  histogram=(unsigned int *) malloc((MaxRGB+1)*sizeof(unsigned int));
  scanline=(RunlengthPacket *) malloc(5*image->columns*sizeof(RunlengthPacket));
  if ((histogram == (unsigned int *) NULL) ||
      (scanline == (RunlengthPacket *) NULL))
    {
      Warning("Unable to oil paint","Memory allocation failed");
      DestroyImage(painted_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 4 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns << 2); x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Dump first scanline of image.
  */
  q=painted_image->pixels;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    *q=(*s);
    q->length=0;
    q++;
    s++;
  }
  /*
    Paint each row of the image.
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    s=s2;
    *q=(*s);
    q->length=0;
    q++;
    for (x=1; x < (image->columns-1); x++)
    {
      /*
        Note each pixel in a 5x5 neighborhood.
      */
      s=s0;
      window[0]=(*s++);
      window[1]=(*s++);
      window[2]=(*s++);
      window[3]=(*s++);
      window[4]=(*s++);
      s=s1;
      window[5]=(*s++);
      window[6]=(*s++);
      window[7]=(*s++);
      window[8]=(*s++);
      window[9]=(*s++);
      s=s2;
      window[10]=(*s++);
      window[11]=(*s++);
      window[12]=(*s++);
      window[13]=(*s++);
      window[14]=(*s++);
      s=s3;
      window[15]=(*s++);
      window[16]=(*s++);
      window[17]=(*s++);
      window[18]=(*s++);
      window[19]=(*s++);
      s=s4;
      window[20]=(*s++);
      window[21]=(*s++);
      window[22]=(*s++);
      window[23]=(*s++);
      window[24]=(*s++);
      /*
        Determine most frequent color.
      */
      count=0;
      for (i=0; i < (MaxRGB+1); i++)
        histogram[i]=0;
      for (i=0; i < 25; i++)
      {
         j=Intensity(window[i]) >> 2;
         histogram[j]++;
         j=histogram[j];
         if (j > count)
           {
             pixel=window[i];
             count=j;
           }
      }
      *q=pixel;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
      s3++;
      s4++;
    }
    /*
      Transfer last pixel of the scanline.
    */
    s=s2;
    *q=(*s);
    q->length=0;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  s=scanline+image->columns*(y % 5);
  for (x=0; x < (image->columns << 1); x++)
  {
    *q=(*s);
    q->length=0;
    q++;
    s++;
  }
  (void) free((char *) histogram);
  (void) free((char *) scanline);
  return(painted_image);
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
%  Weights assume that the importance of neighboring pixels is negately
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
Image *ReduceImage(image)
Image
  *image;
{
#define Reduce(weight) \
  total_red+=(weight)*(s->red); \
  total_green+=(weight)*(s->green); \
  total_blue+=(weight)*(s->blue); \
  total_matte+=(weight)*(s->index); \
  s++;

  Image
    *reduced_image;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2,
    *s3;

  register unsigned int
    x;

  RunlengthPacket
    *scanline;

  unsigned int
    y;

  unsigned long
    total_matte,
    total_blue,
    total_green,
    total_red;

  if ((image->columns < 4) || (image->rows < 4))
    {
      Warning("Unable to reduce image","image size must exceed 3x3");
      return((Image *) NULL);
    }
  /*
    Initialize reduced image attributes.
  */
  reduced_image=CopyImage(image,image->columns >> 1,image->rows >> 1,False);
  if (reduced_image == (Image *) NULL)
    {
      Warning("Unable to reduce image","Memory allocation failed");
      return((Image *) NULL);
    }
  reduced_image->class=DirectClass;
  /*
    Allocate image buffer and scanline buffer for 4 rows of the image.
  */
  scanline=(RunlengthPacket *)
    malloc(4*(image->columns+1)*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to reduce image","Memory allocation failed");
      DestroyImage(reduced_image);
      return((Image *) NULL);
    }
  /*
    Preload the first 2 rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Read another scan line.
    */
    s=s3;
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
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
      total_matte=0;
      s=s0;
      Reduce(3); Reduce(7);  Reduce(7);  Reduce(3);
      s=s1;
      Reduce(7); Reduce(15); Reduce(15); Reduce(7);
      s=s2;
      Reduce(7); Reduce(15); Reduce(15); Reduce(7);
      s=s3;
      Reduce(3); Reduce(7);  Reduce(7);  Reduce(3);
      s0+=2;
      s1+=2;
      s2+=2;
      s3+=2;
      q->red=(unsigned char) ((total_red+63) >> 7);
      q->green=(unsigned char) ((total_green+63) >> 7);
      q->blue=(unsigned char) ((total_blue+63) >> 7);
      q->index=(unsigned char) ((total_matte+63) >> 7);
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
%     R G B T r a n s f o r m I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function RGBTransformImage converts the reference image from RGB to
%  an alternate colorspace.  The transformation matrices are not the standard
%  ones: the weights are rescaled to normalized the range of the transformed
%  values to be [0..255].
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

  long
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
  x=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  y=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  z=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if ((x == (long *) NULL) || (y == (long *) NULL) ||
      (z == (long *) NULL) || (range_table == (unsigned char *) NULL))
    {
      Warning("Unable to transform color space","Memory allocation failed");
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

          G = 0.29890*R+0.58660*G+0.11450*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29890)*i;
        y[i+X]=UpShifted(0.58660)*i;
        z[i+X]=UpShifted(0.11450)*i;
        x[i+Y]=UpShifted(0.29890)*i;
        y[i+Y]=UpShifted(0.58660)*i;
        z[i+Y]=UpShifted(0.11450)*i;
        x[i+Z]=UpShifted(0.29890)*i;
        y[i+Z]=UpShifted(0.58660)*i;
        z[i+Z]=UpShifted(0.11450)*i;
      }
      break;
    }
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          I1 = 0.33333*R+0.33334*G+0.33333*B
          I2 = 0.50000*R+0.00000*G-0.50000*B
          I3 =-0.25000*R+0.50000*G-0.25000*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.33333)*i;
        y[i+X]=UpShifted(0.33334)*i;
        z[i+X]=UpShifted(0.33333)*i;
        x[i+Y]=UpShifted(0.50000)*i;
        y[i+Y]=0;
        z[i+Y]=(-UpShifted(0.50000))*i;
        x[i+Z]=(-UpShifted(0.25000))*i;
        y[i+Z]=UpShifted(0.50000)*i;
        z[i+Z]=(-UpShifted(0.25000))*i;
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize XYZ ITU601-1 tables:

          X = 0.43060*X+0.34155*Y+0.17833*Z
          Y = 0.22202*X+0.70666*Y+0.07133*Z
          Z = 0.02018*X+0.12955*Y+0.93918*Z
      */
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.43060)*i;
        y[i+X]=UpShifted(0.34155)*i;
        z[i+X]=UpShifted(0.17833)*i;
        x[i+Y]=UpShifted(0.22202)*i;
        y[i+Y]=UpShifted(0.70666)*i;
        z[i+Y]=UpShifted(0.07133)*i;
        x[i+Z]=UpShifted(0.02018)*i;
        y[i+Z]=UpShifted(0.12955)*i;
        z[i+Z]=UpShifted(0.93918)*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          Y =  0.29890*R+0.58660*G+0.11450*B
          Cb= -0.16874*R-0.33126*G+0.50000*B
          Cr=  0.50000*R-0.41830*G-0.08160*B

        Cb and Cr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29890)*i;
        y[i+X]=UpShifted(0.58660)*i;
        z[i+X]=UpShifted(0.11450)*i;
        x[i+Y]=(-UpShifted(0.16874))*i;
        y[i+Y]=(-UpShifted(0.33126))*i;
        z[i+Y]=UpShifted(0.50000)*i;
        x[i+Z]=UpShifted(0.50000)*i;
        y[i+Z]=(-UpShifted(0.41830))*i;
        z[i+Z]=(-UpShifted(0.08160))*i;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          Y = 0.29890*R+0.58660*G+0.11450*B
          C1=-0.29890*R-0.58660*G+0.88600*B
          C2= 0.70100*R-0.58660*G+0.11450*B

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      ty=UpShifted(156);
      tz=UpShifted(137);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29890)*i;
        y[i+X]=UpShifted(0.58660)*i;
        z[i+X]=UpShifted(0.11450)*i;
        x[i+Y]=(-UpShifted(0.29890))*i;
        y[i+Y]=(-UpShifted(0.58660))*i;
        z[i+Y]=UpShifted(0.88600)*i;
        x[i+Z]=UpShifted(0.70100)*i;
        y[i+Z]=(-UpShifted(0.58660))*i;
        z[i+Z]=(-UpShifted(0.11450))*i;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          Y = 0.29890*R+0.58660*G+0.11450*B
          I = 0.50000*R-0.23000*G-0.27000*B
          Q = 0.20200*R-0.50000*G+0.29800*B

        I and Q, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29890)*i;
        y[i+X]=UpShifted(0.58660)*i;
        z[i+X]=UpShifted(0.11450)*i;
        x[i+Y]=UpShifted(0.50000)*i;
        y[i+Y]=(-UpShifted(0.23000))*i;
        z[i+Y]=(-UpShifted(0.27000))*i;
        x[i+Z]=UpShifted(0.20200)*i;
        y[i+Z]=(-UpShifted(0.50000))*i;
        z[i+Z]=UpShifted(0.29800)*i;
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          Y =  0.21220*R+0.70130*G+0.08650*B
          Pb= -0.11620*R-0.38380*G+0.50000*B
          Pr=  0.50000*R-0.44510*G-0.05490*B

        Pb and Pr, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.21220)*i;
        y[i+X]=UpShifted(0.70130)*i;
        z[i+X]=UpShifted(0.08650)*i;
        x[i+Y]=(-UpShifted(0.11620))*i;
        y[i+Y]=(-UpShifted(0.38380))*i;
        z[i+Y]=UpShifted(0.50000)*i;
        x[i+Z]=UpShifted(0.50000)*i;
        y[i+Z]=(-UpShifted(0.44510))*i;
        z[i+Z]=(-UpShifted(0.05490))*i;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          Y =  0.29890*R+0.58660*G+0.11450*B
          U = -0.14740*R-0.28950*G+0.43690*B
          V =  0.61500*R-0.51500*G-0.10000*B

        U and V, normally -0.5 through 0.5, are normalized to the range 0
        through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
      */
      ty=UpShifted((MaxRGB+1) >> 1);
      tz=UpShifted((MaxRGB+1) >> 1);
      for (i=0; i <= MaxRGB; i++)
      {
        x[i+X]=UpShifted(0.29890)*i;
        y[i+X]=UpShifted(0.58660)*i;
        z[i+X]=UpShifted(0.11450)*i;
        x[i+Y]=(-UpShifted(0.14740))*i;
        y[i+Y]=(-UpShifted(0.28950))*i;
        z[i+Y]=UpShifted(0.43690)*i;
        x[i+Z]=UpShifted(0.61500)*i;
        y[i+Z]=(-UpShifted(0.51500))*i;
        z[i+Z]=(-UpShifted(0.10000))*i;
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
%      in the horizontal direction.
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
      Warning("Unable to roll image","Memory allocation failed");
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
      if (image->runlength != 0)
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
      *q=(*p);
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
%   S a m p l e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function SampleImage creates a new image that is a scaled size of an
%  existing one using pixel sampling.  It allocates the memory necessary
%  for the new Image structure and returns a pointer to the new image.
%
%  The format of the SampleImage routine is:
%
%      sampled_image=SampleImage(image,columns,rows)
%
%  A description of each parameter follows:
%
%    o sampled_image: Function SampleImage returns a pointer to the image after
%      scaling.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o columns: An integer that specifies the number of columns in the sampled
%      image.
%
%    o rows: An integer that specifies the number of rows in the sampled
%      image.
%
%
*/
Image *SampleImage(image,columns,rows)
Image
  *image;

unsigned int
  columns,
  rows;
{
  Image
    *sampled_image;

  register RunlengthPacket
    *p,
    *q,
    *s;

  register unsigned int
    x;

  RunlengthPacket
    *scanline;

  unsigned int
    *x_offset,
    y,
    *y_offset;

  unsigned long
    sample_factor;

  if ((columns == 0) || (rows == 0))
    {
      Warning("Unable to sample image","image dimensions are zero");
      return((Image *) NULL);
    }
  if ((columns > MaxImageSize) || (rows > MaxImageSize))
    {
      Warning("Unable to sample image","image too large");
      return((Image *) NULL);
    }
  /*
    Initialize sampled image attributes.
  */
  sampled_image=CopyImage(image,columns,rows,False);
  if (sampled_image == (Image *) NULL)
    {
      Warning("Unable to sample image","Memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Allocate scan line buffer and column offset buffers.
  */
  scanline=(RunlengthPacket *) malloc(image->columns*sizeof(RunlengthPacket));
  x_offset=(unsigned int *)
    malloc(sampled_image->columns*sizeof(unsigned int));
  y_offset=(unsigned int *) malloc(sampled_image->rows*sizeof(unsigned int));
  if ((scanline == (RunlengthPacket *) NULL) ||
      (x_offset == (unsigned int *) NULL) ||
      (y_offset == (unsigned int *) NULL))
    {
      Warning("Unable to sample image","Memory allocation failed");
      DestroyImage(sampled_image);
      return((Image *) NULL);
    }
  /*
    Initialize column pixel offsets.
  */
  sample_factor=UpShift(image->columns-1)/sampled_image->columns;
  columns=0;
  for (x=0; x < sampled_image->columns; x++)
  {
    x_offset[x]=DownShift((x+1)*sample_factor)-columns;
    columns+=x_offset[x];
  }
  /*
    Initialize row pixel offsets.
  */
  sample_factor=UpShift(image->rows-1)/sampled_image->rows;
  rows=0;
  for (y=0; y < sampled_image->rows; y++)
  {
    y_offset[y]=DownShift((y+1)*sample_factor)-rows;
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
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Sample each row.
  */
  q=sampled_image->pixels;
  for (y=0; y < sampled_image->rows; y++)
  {
    /*
      Sample each column.
    */
    s=scanline;
    for (x=0; x < sampled_image->columns; x++)
    {
      *q=(*s);
      q->length=0;
      q++;
      s+=x_offset[x];
    }
    if (y_offset[y] != 0)
      {
        /*
          Skip a scan line.
        */
        for (x=0; x < (image->columns*(y_offset[y]-1)); x++)
          if (image->runlength != 0)
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
          if (image->runlength != 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
          *s=(*p);
          s++;
        }
      }
  }
  (void) free((char *) scanline);
  (void) free((char *) x_offset);
  (void) free((char *) y_offset);
  return(sampled_image);
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
%  existing one.  It allocates the memory necessary for the new Image
%  structure and returns a pointer to the new image.  To scale a scanline
%  from x pixels to y pixels, each new pixel represents x/y old pixels.  To
%  read x/y pixels, read (x/y rounded up) pixels but only count the required
%  fraction of the last old pixel read in your new pixel.  The remainder
%  of the old pixel will be counted in the next new pixel.
%
%  The scaling algorithm was suggested by rjohnson@shell.com and is adapted
%  from pnmscale(1) of PBMPLUS by Jef Poskanzer.
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
  typedef struct ScaledPacket
  {
    long
      red,
      green,
      blue,
      index;
  } ScaledPacket;

  Image
    *scaled_image;

  int
    next_row,
    number_rows;

  long
    x_scale,
    x_span;

  register RunlengthPacket
    *p,
    *q;

  register ScaledPacket
    *s,
    *t;

  register unsigned int
    x;

  ScaledPacket
    *scaled_scanline,
    *scanline,
    *y_vector,
    *x_vector;

  unsigned int
    y;

  unsigned long
    blue,
    green,
    index,
    red;

  if ((columns == 0) || (rows == 0))
    {
      Warning("Unable to scale image","image dimensions are zero");
      return((Image *) NULL);
    }
  if ((columns > MaxImageSize) || (rows > MaxImageSize))
    {
      Warning("Unable to scale image","image too large");
      return((Image *) NULL);
    }
  /*
    Initialize scaled image attributes.
  */
  scaled_image=CopyImage(image,columns,rows,False);
  if (scaled_image == (Image *) NULL)
    {
      Warning("Unable to scale image","Memory allocation failed");
      return((Image *) NULL);
    }
  scaled_image->class=DirectClass;
  /*
    Allocate memory.
  */
  x_vector=(ScaledPacket *) malloc(image->columns*sizeof(ScaledPacket));
  scanline=x_vector;
  if (scaled_image->rows != image->rows)
    scanline=(ScaledPacket *) malloc(image->columns*sizeof(ScaledPacket));
  scaled_scanline=(ScaledPacket *)
    malloc(scaled_image->columns*sizeof(ScaledPacket));
  y_vector=(ScaledPacket *) malloc(image->columns*sizeof(ScaledPacket));
  if ((x_vector == (ScaledPacket *) NULL) ||
      (scanline == (ScaledPacket *) NULL) ||
      (scaled_scanline == (ScaledPacket *) NULL) ||
      (y_vector == (ScaledPacket *) NULL))
    {
      Warning("Unable to scale image","Memory allocation failed");
      DestroyImage(scaled_image);
      return((Image *) NULL);
    }
  /*
    Scale image.
  */
  number_rows=0;
  next_row=True;
  x_scale=UpShift(scaled_image->rows)/image->rows;
  x_span=UpShift(1);
  for (x=0; x < image->columns; x++)
  {
    y_vector[x].red=0;
    y_vector[x].green=0;
    y_vector[x].blue=0;
    y_vector[x].index=0;
  }
  p=image->pixels;
  image->runlength=p->length+1;
  q=scaled_image->pixels;
  for (y=0; y < scaled_image->rows; y++)
  {
    if (scaled_image->rows == image->rows)
      for (x=0; x < image->columns; x++)
      {
        /*
          Read a new scanline.
        */
        if (image->runlength != 0)
          image->runlength--;
        else
          {
            p++;
            image->runlength=p->length;
          }
        x_vector[x].red=p->red;
        x_vector[x].green=p->green;
        x_vector[x].blue=p->blue;
        x_vector[x].index=p->index;
      }
    else
      {
        /*
          Scale Y direction.
        */
        while (x_scale < x_span)
        {
          if (next_row && (number_rows < image->rows))
            {
              /*
                Read a new scanline.
              */
              for (x=0; x < image->columns; x++)
              {
                if (image->runlength != 0)
                  image->runlength--;
                else
                  {
                    p++;
                    image->runlength=p->length;
                  }
                x_vector[x].red=p->red;
                x_vector[x].green=p->green;
                x_vector[x].blue=p->blue;
                x_vector[x].index=p->index;
              }
              number_rows++;
            }
          for (x=0; x < image->columns; x++)
          {
            y_vector[x].red+=x_scale*x_vector[x].red;
            y_vector[x].green+=x_scale*x_vector[x].green;
            y_vector[x].blue+=x_scale*x_vector[x].blue;
            y_vector[x].index+=x_scale*x_vector[x].index;
          }
          x_span-=x_scale;
          x_scale=UpShift(scaled_image->rows)/image->rows;
          next_row=True;
        }
        if (next_row && (number_rows < image->rows))
          {
            /*
              Read a new scanline.
            */
            for (x=0; x < image->columns; x++)
            {
              if (image->runlength != 0)
                image->runlength--;
              else
                {
                  p++;
                  image->runlength=p->length;
                }
              x_vector[x].red=p->red;
              x_vector[x].green=p->green;
              x_vector[x].blue=p->blue;
              x_vector[x].index=p->index;
            }
            number_rows++;
            next_row=False;
          }
        s=scanline;
        for (x=0; x < image->columns; x++)
        {
          red=DownShift(y_vector[x].red+x_span*x_vector[x].red);
          green=DownShift(y_vector[x].green+x_span*x_vector[x].green);
          blue=DownShift(y_vector[x].blue+x_span*x_vector[x].blue);
          index=DownShift(y_vector[x].index+x_span*x_vector[x].index);
          s->red=red > MaxRGB ? MaxRGB : red;
          s->green=green > MaxRGB ? MaxRGB : green;
          s->blue=blue > MaxRGB ? MaxRGB : blue;
          s->index=index > MaxColormapSize ? MaxColormapSize : index;
          s++;
          y_vector[x].red=0;
          y_vector[x].green=0;
          y_vector[x].blue=0;
          y_vector[x].index=0;
        }
        x_scale-=x_span;
        if (x_scale == 0)
          {
            x_scale=UpShift(scaled_image->rows)/image->rows;
            next_row=True;
          }
        x_span=UpShift(1);
      }
    if (scaled_image->columns == image->columns)
      {
        /*
          Transfer scanline to scaled image.
        */
        s=scanline;
        for (x=0; x < scaled_image->columns; x++)
        {
          q->red=(unsigned char) s->red;
          q->green=(unsigned char) s->green;
          q->blue=(unsigned char) s->blue;
          q->index=(unsigned short) s->index;
          q->length=0;
          q++;
          s++;
        }
      }
    else
      {
        int
          next_column;

        long int
          y_scale,
          y_span;

        /*
          Scale X direction.
        */
        red=0;
        green=0;
        blue=0;
        next_column=False;
        y_span=UpShift(1);
        s=scanline;
        t=scaled_scanline;
        for (x=0; x < image->columns; x++)
        {
          y_scale=UpShift(scaled_image->columns)/image->columns;
          while (y_scale >= y_span)
          {
            if (next_column)
              {
                red=0;
                green=0;
                blue=0;
                index=0;
                t++;
              }
            red=DownShift(red+y_span*s->red);
            green=DownShift(green+y_span*s->green);
            blue=DownShift(blue+y_span*s->blue);
            index=DownShift(index+y_span*s->index);
            t->red=red > MaxRGB ? MaxRGB : red;
            t->green=green > MaxRGB ? MaxRGB : green;
            t->blue=blue > MaxRGB ? MaxRGB : blue;
            t->index=index > MaxColormapSize ? MaxColormapSize : index;
            y_scale-=y_span;
            y_span=UpShift(1);
            next_column=True;
          }
        if (y_scale > 0)
          {
            if (next_column)
              {
                red=0;
                green=0;
                blue=0;
                index=0;
                next_column=False;
                t++;
              }
            red+=y_scale*s->red;
            green+=y_scale*s->green;
            blue+=y_scale*s->blue;
            index+=y_scale*s->index;
            y_span-=y_scale;
          }
        s++;
      }
      if (y_span > 0)
        {
          s--;
          red+=y_span*s->red;
          green+=y_span*s->green;
          blue+=y_span*s->blue;
          index+=y_span*s->index;
        }
      if (!next_column)
        {
          red=DownShift(red);
          green=DownShift(green);
          blue=DownShift(blue);
          index=DownShift(index);
          t->red=red > MaxRGB ? MaxRGB : red;
          t->green=green > MaxRGB ? MaxRGB : green;
          t->blue=blue > MaxRGB ? MaxRGB : blue;
          t->index=index > MaxRGB ? MaxRGB : index;
        }
      /*
        Transfer scanline to scaled image.
      */
      t=scaled_scanline;
      for (x=0; x < scaled_image->columns; x++)
      {
        q->red=(unsigned char) t->red;
        q->green=(unsigned char) t->green;
        q->blue=(unsigned char) t->blue;
        q->index=(unsigned short) t->index;
        q->length=0;
        q++;
        t++;
      }
    }
  }
  /*
    Free allocated memory.
  */
  (void) free((char *) y_vector);
  (void) free((char *) scaled_scanline);
  if (scanline != x_vector)
    (void) free((char *) scanline);
  (void) free((char *) x_vector);
  return(scaled_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t I m a g e M a g i c k                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function SetImageMagick initializes the `magick' field of the ImageInfo
%  structure.  It is set to a type of image format based on the prefix or
%  suffix of the filename.  For example, `ps:image' returns PS indicating
%  a Postscript image.  JPEG is returned for this filename: `image.jpg'.
%  The filename prefix has precedance over the suffix.  Use an optional index
%  enclosed in brackets after a file name to specify a desired subimage of a
%  multi-resolution image format like Photo CD (e.g. img0001.pcd[4]).
%
%  The format of the SetImageMagick routine is:
%
%      SetImageMagick(image_info)
%
%  A description of each parameter follows:
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%
*/
void SetImageMagick(image_info)
ImageInfo
  *image_info;
{
  static char
    *ImageTypes[]=
    {
      "AVS",
      "BMP",
      "CMYK",
      "EPS",
      "EPSF",
      "EPSI",
      "FAX",
      "FITS",
      "GIF",
      "GIF87",
      "GRAY",
      "G3",
      "HISTOGRAM",
      "IRIS",
      "JPEG",
      "JPG",
      "MAP",
      "MATTE",
      "MIFF",
      "MPG",
      "MPEG",
      "MTV",
      "NULL",
      "PBM",
      "PCD",
      "PCX",
      "PGM",
      "PICT",
      "PM",
      "PPM",
      "PNM",
      "PS",
      "PS2",
      "RAD",
      "RAS",
      "RGB",
      "RLE",
      "SUN",
      "TGA",
      "TEXT",
      "TIF",
      "TIFF",
      "VICAR",
      "VID",
      "VIFF",
      "X",
      "XBM",
      "XC",
      "XPM",
      "XV",
      "XWD",
      "YUV",
      "YUV3",
      (char *) NULL,
    };

  char
    c,
    magick[MaxTextLength];

  register char
    *p,
    *q;

  register int
    i;

  /*
    Look for 'image.format' in filename.
  */
  *magick='\0';
  p=image_info->filename+strlen(image_info->filename)-1;
  if (*p == ']')
    {
      /*
        Look for sub-image enclosed in brackets (e.g. img0001.pcd[4]).
      */
      for (q=p-1; q > image_info->filename; q--)
        if (!isdigit(*q))
          break;
      if (*q == '[')
        {
          p=q++;
          image_info->subimage=atoi(q);
          *p='\0';
        }
    }
  while ((*p != '.') && (p > image_info->filename))
    p--;
  if ((strcmp(p,".gz") == 0) || (strcmp(p,".Z") == 0))
    do
    {
      p--;
    } while ((*p != '.') && (p > image_info->filename));
  if ((*p == '.') && (strlen(p) < sizeof(magick)))
    {
      /*
        User specified image format.
      */
      (void) strcpy(magick,p+1);
      for (q=magick; *q != '\0'; q++)
      {
        if (*q == '.')
          {
            *q='\0';
            break;
          }
        c=(*q);
        if (isascii(c) && islower(c))
          *q=toupper(c);
      }
      for (i=0; ImageTypes[i] != (char *) NULL; i++)
        if (strcmp(magick,ImageTypes[i]) == 0)
          {
            /*
              IRIS and RGB are ambiguous.
            */
            if ((strcmp(image_info->magick,"IRIS") != 0) ||
                (strcmp(ImageTypes[i],"RGB") != 0))
              (void) strcpy(image_info->magick,magick);
            break;
          }
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  image_info->assert=False;
  p=image_info->filename;
  while ((*p != ':') && (*p != '\0'))
    p++;
  if ((*p == ':') && ((p-image_info->filename) < sizeof(magick)))
    {
      /*
        User specified image format.
      */
      (void) strncpy(magick,image_info->filename,p-image_info->filename);
      magick[p-image_info->filename]='\0';
      for (q=magick; *q != '\0'; q++)
      {
        c=(*q);
        if (isascii(c) && islower(c))
          *q=toupper(c);
      }
      for (i=0; ImageTypes[i] != (char *) NULL; i++)
        if (strcmp(magick,ImageTypes[i]) == 0)
          {
            /*
              Strip off image format prefix.
            */
            p++;
            (void) strcpy(image_info->filename,p);
            (void) strcpy(image_info->magick,magick);
            image_info->assert=True;
            break;
          }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a r p e n I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function SharpenImage creates a new image that is a copy of an existing
%  one with the pixels sharpened.  It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  SharpenImage convolves the pixel neighborhood with this sharpening mask:
%
%    -1 -2 -1
%    -2 44 -2
%    -1 -2 -1
%
%  The scan only processes pixels that have a full set of neighbors.  Pixels
%  in the top, bottom, left, and right pairs of rows and columns are omitted
%  from the scan.
%
%  The format of the SharpenImage routine is:
%
%      sharpened_image=SharpenImage(image)
%
%  A description of each parameter follows:
%
%    o sharpened_image: Function SharpenImage returns a pointer to the image
%      after it is sharpened.  A null image is returned if there is a memory
%      shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%
*/
Image *SharpenImage(image)
Image
  *image;
{
#define Sharpen(weight) \
  total_red+=(weight)*(int) (s->red); \
  total_green+=(weight)*(int) (s->green); \
  total_blue+=(weight)*(int) (s->blue); \
  s++;

  Image
    *sharpened_image;

  long int
    total_blue,
    total_green,
    total_red;

  register RunlengthPacket
    *p,
    *q,
    *s,
    *s0,
    *s1,
    *s2;

  register unsigned int
    x;

  RunlengthPacket
    *scanline;

  unsigned int
    y;

  if ((image->columns < 3) || (image->rows < 3))
    {
      Warning("Unable to sharpen image","image size must exceed 3x3");
      return((Image *) NULL);
    }
  /*
    Initialize sharpened image attributes.
  */
  sharpened_image=CopyImage(image,image->columns,image->rows,False);
  if (sharpened_image == (Image *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      return((Image *) NULL);
    }
  sharpened_image->class=DirectClass;
  /*
    Allocate scan line buffer for 3 rows of the image.
  */
  scanline=(RunlengthPacket *) malloc(3*image->columns*sizeof(RunlengthPacket));
  if (scanline == (RunlengthPacket *) NULL)
    {
      Warning("Unable to enhance image","Memory allocation failed");
      DestroyImage(sharpened_image);
      return((Image *) NULL);
    }
  /*
    Read the first two rows of the image.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  s=scanline;
  for (x=0; x < (image->columns << 1); x++)
  {
    if (image->runlength != 0)
      image->runlength--;
    else
      {
        p++;
        image->runlength=p->length;
      }
    *s=(*p);
    s++;
  }
  /*
    Dump first scanline of image.
  */
  q=sharpened_image->pixels;
  s=scanline;
  for (x=0; x < image->columns; x++)
  {
    *q=(*s);
    q->index=0;
    q->length=0;
    q++;
    s++;
  }
  /*
    Convolve each row.
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
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *s=(*p);
      s++;
    }
    /*
      Transfer first pixel of the scanline.
    */
    *q=(*s1);
    q->index=0;
    q->length=0;
    q++;
    for (x=1; x < (image->columns-1); x++)
    {
      /*
        Compute weighted average of target pixel color components.
      */
      total_red=0;
      total_green=0;
      total_blue=0;
      s=s0;
      Sharpen(-1); Sharpen(-2); Sharpen(-1);
      s=s1;
      Sharpen(-2); Sharpen(44); Sharpen(-2);
      s=s2;
      Sharpen(-1); Sharpen(-2); Sharpen(-1);
      if (total_red < 0)
        q->red=0;
      else
        if (total_red > (MaxRGB << 5))
          q->red=MaxRGB;
        else
          q->red=(total_red+16) >> 5;
      if (total_green < 0)
        q->green=0;
      else
        if (total_green > (MaxRGB << 5))
          q->green=MaxRGB;
        else
          q->green=(total_green+16) >> 5;
      if (total_blue < 0)
        q->blue=0;
      else
        if (total_blue > (MaxRGB << 5))
          q->blue=MaxRGB;
        else
          q->blue=(total_blue+16) >> 5;
      q->index=0;
      q->length=0;
      q++;
      s0++;
      s1++;
      s2++;
    }
    /*
      Transfer last pixel of the scanline.
    */
    s1++;
    *q=(*s1);
    q->index=0;
    q->length=0;
    q++;
  }
  /*
    Dump last scanline of pixels.
  */
  s=scanline+image->columns*(y % 3);
  for (x=0; x < image->columns; x++)
  {
    *q=(*s);
    q->index=0;
    q->length=0;
    q++;
    s++;
  }
  (void) free((char *) scanline);
  return(sharpened_image);
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
      Warning("Unable to sort colormap","Memory allocation failed");
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
    (int (*) _Declare((const void *, const void *))) IntensityCompare);
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
      Warning("Unable to create stereo image",
        "left and right image sizes differ");
      return((Image *) NULL);
    }
  /*
    Initialize stereo image attributes.
  */
  stereo_image=CopyImage(left_image,left_image->columns,left_image->rows,False);
  if (stereo_image == (Image *) NULL)
    {
      Warning("Unable to create stereo image","Memory allocation failed");
      return((Image *) NULL);
    }
  stereo_image->class=DirectClass;
  /*
    Copy left image to red channel and right image to blue channel.
  */
  QuantizeImage(left_image,256,8,False,GRAYColorspace);
  SyncImage(left_image);
  p=left_image->pixels;
  left_image->runlength=p->length+1;
  QuantizeImage(right_image,256,8,False,GRAYColorspace);
  SyncImage(right_image);
  q=right_image->pixels;
  right_image->runlength=q->length+1;
  r=stereo_image->pixels;
  for (i=0; i < (stereo_image->columns*stereo_image->rows); i++)
  {
    if (left_image->runlength != 0)
      left_image->runlength--;
    else
      {
        p++;
        left_image->runlength=p->length;
      }
    if (right_image->runlength != 0)
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
%  of existing one as specified by the crop and image geometries.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  If a crop geometry is specified a subregion of the image is obtained.
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
%      TransformImage(image,crop_geometry,image_geometry)
%
%  A description of each parameter follows:
%
%    o image: The address of an address of a structure of type Image.  The
%      transformed image is returned as this parameter.
%
%    o crop_geometry: Specifies a pointer to a crop geometry string.
%      This geometry defines a subregion of the image.
%
%    o image_geometry: Specifies a pointer to a image geometry string.
%      The specified width and height of this geometry string are absolute.
%
%
*/
void TransformImage(image,crop_geometry,image_geometry)
Image
  **image;

char
  *crop_geometry,
  *image_geometry;
{
  Image
    *sharpened_image,
    *transformed_image;

  int
    flags;

  unsigned int
    height,
    width;

  transformed_image=(*image);
  if (crop_geometry != (char *) NULL)
    {
      Image
        *cropped_image;

      RectangleInfo
        crop_info;

      /*
        Crop image to a user specified size.
      */
      crop_info.x=0;
      crop_info.y=0;
      flags=XParseGeometry(crop_geometry,&crop_info.x,&crop_info.y,
        &crop_info.width,&crop_info.height);
      if ((flags & WidthValue) == 0)
        crop_info.width=(unsigned int)
          ((int) transformed_image->columns-crop_info.x);
      if ((flags & HeightValue) == 0)
        crop_info.height=(unsigned int)
          ((int) transformed_image->rows-crop_info.y);
      if ((flags & XNegative) != 0)
        crop_info.x+=transformed_image->columns-crop_info.width;
      if ((flags & YNegative) != 0)
        crop_info.y+=transformed_image->rows-crop_info.height;
      cropped_image=CropImage(transformed_image,&crop_info);
      if (cropped_image != (Image *) NULL)
        {
          DestroyImage(transformed_image);
          transformed_image=cropped_image;
        }
    }
  /*
    Scale image to a user specified size.
  */
  width=transformed_image->columns;
  height=transformed_image->rows;
  ParseImageGeometry(image_geometry,&width,&height);
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
    /*
      Sharpen image.
    */
    sharpened_image=SharpenImage(transformed_image);
    if (sharpened_image != (Image *) NULL)
      {
        DestroyImage(transformed_image);
        transformed_image=sharpened_image;
      }
  }
  while ((transformed_image->columns <= (width >> 1)) &&
         (transformed_image->rows <= (height >> 1)))
  {
    Image
      *zoomed_image;

    /*
      Zoom image with bilinear interpolation.
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
      /*
        Sharpen image.
      */
      sharpened_image=SharpenImage(transformed_image);
      if (sharpened_image != (Image *) NULL)
        {
          DestroyImage(transformed_image);
          transformed_image=sharpened_image;
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
%  Function TransformRGBImage converts the reference image from an alternate
%  colorspace.  The transformation matrices are not the standard ones:  the
%  weights are rescaled to normalized the range of the transformed values to
%  be [0..255].
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

  long
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

  if ((colorspace == RGBColorspace) || (colorspace == GRAYColorspace))
    return;
  /*
    Allocate the tables.
  */
  red=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  green=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  blue=(long *) malloc(3*(MaxRGB+1)*sizeof(long));
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if ((red == (long *) NULL) || (green == (long *) NULL) ||
      (blue == (long *) NULL) || (range_table == (unsigned char *) NULL))
    {
      Warning("Unable to transform color space","Memory allocation failed");
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
    case OHTAColorspace:
    {
      /*
        Initialize OHTA tables:

          R = I1+1.00000*I2-0.66668*I3
          G = I1+0.00000*I2+1.33333*I3
          B = I1-1.00000*I2-0.66668*I3

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=UpShifted(1.0000*0.5)*((i << 1)-MaxRGB);
        blue[i+R]=(-UpShifted(0.66668*0.5))*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=0;
        blue[i+G]=UpShifted(1.33333*0.5)*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=(-UpShifted(1.00000*0.5))*((i << 1)-MaxRGB);
        blue[i+B]=(-UpShifted(0.66668*0.5))*((i << 1)-MaxRGB);
      }
      break;
    }
    case XYZColorspace:
    {
      /*
        Initialize XYZ ITU601-1t ables:

          R =  3.06270*R-1.39280*G-0.47590*B
          G = -0.96890*R+1.87560*G+0.04170*B
          B =  0.05850*R-0.22860*G+1.06900*B
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(3.06270)*i;
        green[i+R]=(-UpShifted(1.39280))*i;
        blue[i+R]=(-UpShifted(0.47590))*i;
        red[i+G]=(-UpShifted(0.96890))*i;
        green[i+G]=UpShifted(1.87560)*i;
        blue[i+G]=UpShifted(0.04170)*i;
        red[i+B]=UpShifted(0.05850)*i;
        green[i+B]=(-UpShifted(0.22860))*i;
        blue[i+B]=UpShifted(1.06900)*i;
      }
      break;
    }
    case YCbCrColorspace:
    {
      /*
        Initialize YCbCr tables:

          R = Y           +1.40200*Cr
          G = Y-0.34560*Cb-0.71450*Cr
          B = Y+1.77100*Cb

        Cb and Cr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.40200*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.34560*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.71450*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=UpShifted(1.77100*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
    case YCCColorspace:
    {
      /*
        Initialize YCC tables:

          R = Y           +1.34092*C2
          G = Y-0.31675*C1-0.68253*C2
          B = Y+1.63273*C1

        YCC is scaled by 1.3584.  C1 zero is 156 and C2 is at 137.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.35840)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.82150)*(i-137);
        red[i+G]=UpShifted(1.35840)*i;
        green[i+G]=(-UpShifted(0.43027))*(i-156);
        blue[i+G]=(-UpShifted(0.92715))*(i-137);
        red[i+B]=UpShifted(1.35840)*i;
        green[i+B]=UpShifted(2.21790)*(i-156);
        blue[i+B]=0;
      }
      break;
    }
    case YIQColorspace:
    {
      /*
        Initialize YIQ tables:

          R = 0.97087*Y+1.17782*I+0.59800*Q
          G = 0.97087*Y-0.28626*I-0.72851*Q
          B = 0.97087*Y-1.27870*I+1.72801*Q

        I and Q, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(0.97087)*i;
        green[i+R]=UpShifted(1.17782*0.5)*((i << 1)-MaxRGB);
        blue[i+R]=UpShifted(0.59800*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(0.97087)*i;
        green[i+G]=(-UpShifted(0.28626*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.72851*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(0.97087)*i;
        green[i+B]=(-UpShifted(1.27870*0.5))*((i << 1)-MaxRGB);
        blue[i+B]=UpShifted(1.72801*0.5)*((i << 1)-MaxRGB);
      }
      break;
    }
    case YPbPrColorspace:
    {
      /*
        Initialize YPbPr tables:

          R = Y           +1.57560*C2
          G = Y-0.22530*C1+0.50000*C2
          B = Y+1.82700*C1

        Pb and Pr, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.57560*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.22530*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=UpShifted(0.50000*0.5)*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=UpShifted(1.82700*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
      }
      break;
    }
    case YUVColorspace:
    default:
    {
      /*
        Initialize YUV tables:

          R = Y          +1.13980*V
          G = Y-0.39380*U-0.58050*V
          B = Y+2.02790*U

        U and V, normally -0.5 through 0.5, must be normalized to the range 0
        through MaxRGB.
      */
      for (i=0; i <= MaxRGB; i++)
      {
        red[i+R]=UpShifted(1.00000)*i;
        green[i+R]=0;
        blue[i+R]=UpShifted(1.13980*0.5)*((i << 1)-MaxRGB);
        red[i+G]=UpShifted(1.00000)*i;
        green[i+G]=(-UpShifted(0.39380*0.5))*((i << 1)-MaxRGB);
        blue[i+G]=(-UpShifted(0.58050*0.5))*((i << 1)-MaxRGB);
        red[i+B]=UpShifted(1.00000)*i;
        green[i+B]=UpShifted(2.02790*0.5)*((i << 1)-MaxRGB);
        blue[i+B]=0;
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
      *q=(*p);
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
Image *ZoomImage(image)
Image
  *image;
{
  Image
    *zoomed_image;

  int
    y;

  register int
    x;

  register RunlengthPacket
    *p,
    *q,
    *r;

  if (((image->columns*image->rows) << 1) > MaxImageSize)
    {
      Warning("Unable to zoom image","image size too large");
      return((Image *) NULL);
    }
  /*
    Initialize zoomed image attributes.
  */
  zoomed_image=CopyImage(image,image->columns << 1,image->rows << 1,False);
  if (zoomed_image == (Image *) NULL)
    {
      Warning("Unable to zoom image","Memory allocation failed");
      return((Image *) NULL);
    }
  zoomed_image->class=DirectClass;
  /*
    Initialize zoom image pixels.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  q=zoomed_image->pixels;
  for (y=0; y < image->rows; y++)
  {
    for (x=0; x < image->columns; x++)
    {
      if (image->runlength != 0)
        image->runlength--;
      else
        {
          p++;
          image->runlength=p->length;
        }
      *q=(*p);
      q->length=0;
      q++; 
    }
    q+=image->columns;
  }
  /*
    Zoom each row.
  */
  for (y=0; y < image->rows; y++)
  {
    p=zoomed_image->pixels+(image->rows-1-y)*zoomed_image->columns+
      (image->columns-1);
    q=zoomed_image->pixels+((image->rows-1-y) << 1)*zoomed_image->columns+
      ((image->columns-1) << 1);
    *q=(*p);
    *(q+1)=(*(p));
    for (x=1; x < image->columns; x++)
    {
      p--;
      q-=2;
      *q=(*p);
      (q+1)->red=(((int) p->red)+((int) (p+1)->red)+1) >> 1;
      (q+1)->green=(((int) p->green)+((int) (p+1)->green)+1) >> 1;
      (q+1)->blue=(((int) p->blue)+((int) (p+1)->blue)+1) >> 1;
      (q+1)->index=(((int) p->index)+((int) (p+1)->index)+1) >> 1;
      (q+1)->length=0;
    }
  }
  for (y=0; y < (image->rows-1); y++)
  {
    p=zoomed_image->pixels+(y << 1)*zoomed_image->columns;
    q=p+zoomed_image->columns;
    r=q+zoomed_image->columns;
    for (x=0; x < (image->columns-1); x++)
    {
      q->red=(((int) p->red)+((int) r->red)+1) >> 1;
      q->green=(((int) p->green)+((int) r->green)+1) >> 1;
      q->blue=(((int) p->blue)+((int) r->blue)+1) >> 1;
      q->index=(((int) p->index)+((int) r->index)+1) >> 1;
      q->length=0;
      (q+1)->red=(((int) p->red)+((int) (p+2)->red)+((int) r->red)+
        ((int) (r+2)->red)+2) >> 2;
      (q+1)->green=(((int) p->green)+((int) (p+2)->green)+((int) r->green)+
        ((int) (r+2)->green)+2) >> 2;
      (q+1)->blue=(((int) p->blue)+((int) (p+2)->blue)+((int) r->blue)+
        ((int) (r+2)->blue)+2) >> 2;
      (q+1)->index=(((int) p->index)+((int) (p+2)->index)+((int) r->index)+
        ((int) (r+2)->index)+2) >> 2;
      (q+1)->length=0;
      q+=2;
      p+=2;
      r+=2;
    }
    q->red=(((int) p->red)+((int) r->red)+1) >> 1;
    q->green=(((int) p->green)+((int) r->green)+1) >> 1;
    q->blue=(((int) p->blue)+((int) r->blue)+1) >> 1;
    q->index=(((int) p->index)+((int) r->index)+1) >> 1;
    q->length=0;
    p++;
    q++;
    r++;
    q->red=(((int) p->red)+((int) r->red)+1) >> 1;
    q->green=(((int) p->green)+((int) r->green)+1) >> 1;
    q->blue=(((int) p->blue)+((int) r->blue)+1) >> 1;
    q->index=(((int) p->index)+((int) r->index)+1) >> 1;
    q->length=0;
    p++;
    q++;
    r++;
  }
  p=zoomed_image->pixels+(2*image->rows-2)*zoomed_image->columns;
  q=zoomed_image->pixels+(2*image->rows-1)*zoomed_image->columns;
  for (x=0; x < image->columns; x++)
  {
    *q++=(*p++);
    *q++=(*p++);
  }
  return(zoomed_image);
}
