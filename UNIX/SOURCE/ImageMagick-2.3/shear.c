/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      SSSSS  H   H  EEEEE   AAA    RRRR                      %
%                      SS     H   H  E      A   A   R   R                     %
%                       SSS   HHHHH  EEE    AAAAA   RRRR                      %
%                         SS  H   H  E      A   A   R R                       %
%                      SSSSS  H   H  EEEEE  A   A   R  R                      %
%                                                                             %
%                                                                             %
%              Shear or rotate a raster image by an arbitrary angle.          %
%                                                                             %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                  July 1992                                  %
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
%  Function RotateImage, XShearImage, and YShearImage is based on the paper
%  "A Fast Algorithm for General Raster Rotatation" by Alan W. Paeth.
%  RotateImage is adapted from a similiar routine based on the Paeth paper
%  written by Michael Halle of the Spatial Imaging Group, MIT Media Lab.
%
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"

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
%   C l i p S h e a r I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ClipShearImage clips the sheared image as determined by the
%  bounding box as defined by width and height and shearing angles.
%
%  The format of the ClipShearImage routine is:
%
%      ClipShearImage(image,x_shear,y_shear,width,height,clip)
%
%  A description of each parameter follows.
%
%    o image: The address of a structure of type Image.
%
%    o x_shear, y_shear, width, height: Defines a region of the image to clip.
%
%    o clip: A value other than zero clips the corners of the rotated
%      image and retains the original image size.
%
%
*/
static Image *ClipShearImage(image,x_shear,y_shear,width,height,clip)
Image
  *image;

double
  x_shear,
  y_shear;

unsigned int
  width,
  height,
  clip;
{
  typedef struct Point
  {
    double
      x,
      y;
  } Point;

  double
    x_max,
    x_min,
    y_max,
    y_min;

  Image
    *clipped_image;

  int
    x_offset,
    y_offset;

  Point
    corners[4];

  register int
    i;

  /*
    Calculate the rotated image size.
  */
  corners[0].x=(-((int) width)/2.0);
  corners[0].y=(-((int) height)/2.0);
  corners[1].x=((int) width)/2.0;
  corners[1].y=(-((int) height)/2.0);
  corners[2].x=(-((int) width)/2.0);
  corners[2].y=((int) height)/2.0;
  corners[3].x=((int) width)/2.0;
  corners[3].y=((int) height)/2.0;
  for (i=0; i < 4; i++)
  {
    corners[i].x+=x_shear*corners[i].y;
    corners[i].y+=y_shear*corners[i].x;
    corners[i].x+=x_shear*corners[i].y;
    corners[i].x+=(image->columns-1)/2.0;
    corners[i].y+=(image->rows-3)/2.0;
  }
  x_min=corners[0].x;
  y_min=corners[0].y;
  x_max=corners[0].x;
  y_max=corners[0].y;
  for (i=1; i < 4; i++)
  {
    if (x_min > corners[i].x)
      x_min=corners[i].x;
    if (y_min > corners[i].y)
      y_min=corners[i].y;
    if (x_max < corners[i].x)
      x_max=corners[i].x;
    if (y_max < corners[i].y)
      y_max=corners[i].y;
  }
  x_min=floor((double) x_min);
  x_max=ceil((double) x_max);
  y_min=floor((double) y_min);
  y_max=ceil((double) y_max);
  if (!clip)
    {
      /*
        Do not clip sheared image.
      */
      width=(unsigned int) (x_max-x_min);
      height=(unsigned int) (y_max-y_min);
    }
  x_offset=(int) x_min+((int) (x_max-x_min)-width)/2;
  y_offset=(int) y_min+((int) (y_max-y_min)-height)/2;
  /*
    Clip image and return.
  */
  clipped_image=ClipImage(image,x_offset,y_offset+1,width,height);
  return(clipped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n t e g r a l R o t a t e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function IntegralRotateImage rotates the image an integral of 90 degrees.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the rotated image.
%
%  The format of the IntegralRotateImage routine is:
%
%      rotated_image=IntegralRotateImage(image,rotations)
%
%  A description of each parameter follows.
%
%    o rotated_image: Function IntegralRotateImage returns a pointer to the
%      rotated image.  A null image is returned if there is a a memory shortage.
%
%    o image: The address of a structure of type Image.
%
%    o rotations: Specifies the number of 90 degree rotations.
%
%
*/
static Image *IntegralRotateImage(image,rotations)
Image
  *image;

unsigned int
  rotations;
{
  Image
    *rotated_image;

  register RunlengthPacket
    *p,
    *q;

  register int
    x,
    y;

  /*
    Initialize rotated image attributes.
  */
  rotations%=4;
  if ((rotations == 1) || (rotations == 3))
    rotated_image=CopyImage(image,image->rows,image->columns,False);
  else
    rotated_image=CopyImage(image,image->columns,image->rows,False);
  if (rotated_image == (Image *) NULL)
    {
      Warning("unable to rotate image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Expand runlength packets into a rectangular array of pixels.
  */
  p=image->pixels;
  image->runlength=p->length+1;
  switch (rotations)
  {
    case 0:
    {
      /*
        Rotate 0 degrees.
      */
      for (y=0; y < image->rows; y++)
      {
        q=rotated_image->pixels+rotated_image->columns*y;
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
      }
      break;
    }
    case 1:
    {
      /*
        Rotate 90 degrees.
      */
      for (x=0; x < rotated_image->columns; x++)
      {
        q=rotated_image->pixels+(rotated_image->columns-x-1);
        for (y=0; y < rotated_image->rows; y++)
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
          q+=rotated_image->columns;
        }
      }
      break;
    }
    case 2:
    {
      /*
        Rotate 180 degrees.
      */
      q=rotated_image->pixels;
      for (y=image->rows-1; y >= 0; y--)
      {
        q=rotated_image->pixels+rotated_image->columns*y+rotated_image->columns;
        for (x=0; x < image->columns; x++)
        {
          if (image->runlength > 0)
            image->runlength--;
          else
            {
              p++;
              image->runlength=p->length;
            }
          q--;
          q->red=p->red;
          q->green=p->green;
          q->blue=p->blue;
          q->index=p->index;
          q->length=0;
        }
      }
      break;
    }
    case 3:
    {
      /*
        Rotate 270 degrees.
      */
      for (x=0; x < rotated_image->columns; x++)
      {
        q=rotated_image->pixels+(rotated_image->columns*rotated_image->rows)-
          (rotated_image->columns-x-1)-1;
        for (y=0; y < rotated_image->rows; y++)
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
          q-=rotated_image->columns;
        }
      }
      break;
    }
  }
  return(rotated_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S h e a r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure XShearImage shears the image in the X direction with a shear angle
%  of 'degrees'.  Positive angles shear counter-clockwise (right-hand rule),
%  and negative angles shear clockwise.  Angles are measured relative to a
%  vertical Y-axis.  X shears will widen an image creating 'empty' triangles
%  on the left and right sides of the source image.
%
%  The format of the XShearImage routine is:
%
%      XShearImage(image,degrees,width,height,x_offset,y_offset,background,
%        range_limit)
%
%  A description of each parameter follows.
%
%    o image: The address of a structure of type Image.
%
%    o degrees: A double representing the shearing angle along the X axis.
%
%    o width, height, x_offset, y_offset: Defines a region of the image
%      to shear.
%
%    o background: Specifies a ColorPacket used to fill empty triangles
%      left over from shearing.
%
%
*/
static void XShearImage(image,degrees,width,height,x_offset,y_offset,background,
  range_limit)
Image
  *image;

double
  degrees;

unsigned int
  width,
  height;

int
  x_offset,
  y_offset;

ColorPacket
  background;

register unsigned char
  *range_limit;
{
  double
    displacement;

  enum {LEFT,RIGHT}
    direction;

  int
    step,
    y;

  long int
    fractional_step;

  register RunlengthPacket
    *p,
    *q;

  register int
    blue,
    green,
    i,
    red;

  RunlengthPacket
    last_pixel;

  y_offset--;
  for (y=0; y < height; y++)
  {
    y_offset++;
    displacement=degrees*(((double) y)-(height-1)/2.0);
    if (displacement == 0.0)
      continue;
    if (displacement > 0.0)
      direction=RIGHT;
    else
      {
        displacement*=(-1.0);
        direction=LEFT;
      }
    step=(int) floor(displacement);
    fractional_step=UpShifted(displacement-(double) step);
    if (fractional_step == 0)
      {
        /*
          No fractional displacement-- just copy.
        */
        switch (direction)
        {
          case LEFT:
          {
            /*
              Transfer pixels left-to-right.
            */
            p=image->pixels+image->columns*y_offset+x_offset;
            q=p-step;
            for (i=0; i < width; i++)
            {
              *q=(*p);
              q++;
              p++;
            }
            /*
              Set old row to background color.
            */
            for (i=0; i < step; i++)
            {
              q->red=background.red;
              q->green=background.green;
              q->blue=background.blue;
              q++;
            }
            break;
          }
          case RIGHT:
          {
            /*
              Transfer pixels right-to-left.
            */
            p=image->pixels+image->columns*y_offset+x_offset+width;
            q=p+step;
            for (i=0; i < width; i++)
            {
              p--;
              q--;
              *q=(*p);
            }
            /*
              Set old row to background color.
            */
            for (i=0; i < step; i++)
            {
              q--;
              q->red=background.red;
              q->green=background.green;
              q->blue=background.blue;
            }
            break;
          }
        }
        continue;
      }
    /*
      Fractional displacement.
    */
    step++;
    last_pixel.red=background.red;
    last_pixel.green=background.green;
    last_pixel.blue=background.blue;
    switch (direction)
    {
      case LEFT:
      {
        /*
          Transfer pixels left-to-right.
        */
        p=image->pixels+image->columns*y_offset+x_offset;
        q=p-step;
        for (i=0; i < width; i++)
        {
          red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+p->red*
            fractional_step);
          green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
            p->green*fractional_step);
          blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+p->blue*
            fractional_step);
          last_pixel=(*p);
          p++;
          q->red=range_limit[red];
          q->green=range_limit[green];
          q->blue=range_limit[blue];
          q++;
        }
        /*
          Set old row to background color.
        */
        red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+
          background.red*fractional_step);
        green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
          background.green*fractional_step);
        blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+
          background.blue*fractional_step);
        q->red=range_limit[red];
        q->green=range_limit[green];
        q->blue=range_limit[blue];
        q++;
        for (i=0; i < step-1; i++)
        {
          q->red=background.red;
          q->green=background.green;
          q->blue=background.blue;
          q++;
        }
        break;
      }
      case RIGHT:
      {
        /*
          Transfer pixels right-to-left.
        */
        p=image->pixels+image->columns*y_offset+x_offset+width;
        q=p+step;
        for (i=0; i < width; i++)
        {
          p--;
          red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+p->red*
            fractional_step);
          green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
            p->green*fractional_step);
          blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+p->blue*
            fractional_step);
          last_pixel=(*p);
          q--;
          q->red=range_limit[red];
          q->green=range_limit[green];
          q->blue=range_limit[blue];
        }
        /*
          Set old row to background color.
        */
        red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+
          background.red*fractional_step);
        green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
          background.green*fractional_step);
        blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+
          background.blue*fractional_step);
        q--;
        q->red=range_limit[red];
        q->green=range_limit[green];
        q->blue=range_limit[blue];
        for (i=0; i < step-1; i++)
        {
          q--;
          q->red=background.red;
          q->green=background.green;
          q->blue=background.blue;
        }
        break;
      }
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Y S h e a r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure YShearImage shears the image in the Y direction with a shear
%  angle of 'degrees'.  Positive angles shear counter-clockwise (right-hand
%  rule), and negative angles shear clockwise.  Angles are measured relative
%  to a horizontal X-axis.  Y shears will increase the height of an image
%  creating 'empty' triangles on the top and bottom of the source image.
%
%  The format of the YShearImage routine is:
%
%      YShearImage(image,degrees,width,height,x_offset,y_offset,background,
%        range_limit)
%
%  A description of each parameter follows.
%
%    o image: The address of a structure of type Image.
%
%    o degrees: A double representing the shearing angle along the Y axis.
%
%    o width, height, x_offset, y_offset: Defines a region of the image
%      to shear.
%
%    o background: Specifies a ColorPacket used to fill empty triangles
%      left over from shearing.
%
%
*/
static void YShearImage(image,degrees,width,height,x_offset,y_offset,background,
  range_limit)
Image
  *image;

double
  degrees;

unsigned int
  width,
  height;

int
  x_offset,
  y_offset;

ColorPacket
  background;

register unsigned char
  *range_limit;
{
  double
    displacement;

  enum {UP,DOWN}
    direction;

  int
    step,
    y;

  long int
    fractional_step;

  register RunlengthPacket
    *p,
    *q;

  register int
    blue,
    green,
    i,
    red;

  RunlengthPacket
    last_pixel;

  x_offset--;
  for (y=0; y < width; y++)
  {
    x_offset++;
    displacement=degrees*(((double) y)-(width-1)/2.0);
    if (displacement == 0.0)
      continue;
    if (displacement > 0.0)
      direction=DOWN;
    else
      {
        displacement*=(-1.0);
        direction=UP;
      }
    step=(int) floor(displacement);
    fractional_step=UpShifted(displacement-(double) step);
    if (fractional_step == 0)
      {
        /*
          No fractional displacement-- just copy the pixels.
        */
        switch (direction)
        {
          case UP:
          {
            /*
              Transfer pixels top-to-bottom.
            */
            p=image->pixels+image->columns*y_offset+x_offset;
            q=p-step*image->columns;
            for (i=0; i < height; i++)
            {
              *q=(*p);
              q+=image->columns;
              p+=image->columns;
            }
            /*
              Set old column to background color.
            */
            for (i=0; i < step; i++)
            {
              q->red=background.red;
              q->green=background.green;
              q->blue=background.blue;
              q+=image->columns;
            }
            break;
          }
          case DOWN:
          {
            /*
              Transfer pixels bottom-to-top.
            */
            p=image->pixels+image->columns*(y_offset+height)+x_offset;
            q=p+step*image->columns;
            for (i=0; i < height; i++)
            {
              q-=image->columns;
              p-=image->columns;
              *q=(*p);
            }
            /*
              Set old column to background color.
            */
            for (i=0; i < step; i++)
            {
              q-=image->columns;
              q->red=background.red;
              q->green=background.green;
              q->blue=background.blue;
            }
            break;
          }
        }
        continue;
      }
    /*
      Fractional displacment.
    */
    step++;
    last_pixel.red=background.red;
    last_pixel.green=background.green;
    last_pixel.blue=background.blue;
    switch (direction)
    {
      case UP:
      {
        /*
          Transfer pixels top-to-bottom.
        */
        p=image->pixels+image->columns*y_offset+x_offset;
        q=p-step*image->columns;
        for (i=0; i < height; i++)
        {
          red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+p->red*
            fractional_step);
          green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
            p->green*fractional_step);
          blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+p->blue*
            fractional_step);
          last_pixel=(*p);
          p+=image->columns;
          q->red=range_limit[red];
          q->green=range_limit[green];
          q->blue=range_limit[blue];
          q+=image->columns;
        }
        /*
          Set old column to background color.
        */
        red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+
          background.red*fractional_step);
        green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
          background.green*fractional_step);
        blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+
          background.blue*fractional_step);
        q->red=range_limit[red];
        q->green=range_limit[green];
        q->blue=range_limit[blue];
        q+=image->columns;
        for (i=0; i < step-1; i++)
        {
          q->red=background.red;
          q->green=background.green;
          q->blue=background.blue;
          q+=image->columns;
        }
        break;
      }
      case DOWN:
      {
        /*
          Transfer pixels bottom-to-top.
        */
        p=image->pixels+image->columns*(y_offset+height)+x_offset;
        q=p+step*image->columns;
        for (i=0; i < height; i++)
        {
          p-=image->columns;
          red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+p->red*
            fractional_step);
          green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
            p->green*fractional_step);
          blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+p->blue*
            fractional_step);
          last_pixel=(*p);
          q-=image->columns;
          q->red=range_limit[red];
          q->green=range_limit[green];
          q->blue=range_limit[blue];
        }
        /*
          Set old column to background color.
        */
        red=DownShift(last_pixel.red*(UpShift(1)-fractional_step)+
          background.red*fractional_step);
        green=DownShift(last_pixel.green*(UpShift(1)-fractional_step)+
          background.green*fractional_step);
        blue=DownShift(last_pixel.blue*(UpShift(1)-fractional_step)+
          background.blue*fractional_step);
        q-=image->columns;
        q->red=range_limit[red];
        q->green=range_limit[green];
        q->blue=range_limit[blue];
        for (i=0; i < step-1; i++)
        {
          q-=image->columns;
          q->red=background.red;
          q->green=background.green;
          q->blue=background.blue;
        }
        break;
      }
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o t a t e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function RotateImage creates a new image that is a rotated copy of an
%  existing one.  Positive angles rotate counter-clockwise (right-hand rule),
%  while negative angles rotate clockwise.  Rotated images are usually larger
%  than the originals and have 'empty' triangular corners.  X axis.  Empty
%  triangles left over from shearing the image are filled with the color
%  defined by the pixel at location (0,0).  RotateImage allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  Function RotateImage is based on the paper "A Fast Algorithm for General
%  Raster Rotatation" by Alan W. Paeth.  RotateImage is adapted from a similiar
%  routine based on the Paeth paper written by Michael Halle of the Spatial
%  Imaging Group, MIT Media Lab.
%
%  The format of the RotateImage routine is:
%
%      RotateImage(image,degrees,clip)
%
%  A description of each parameter follows.
%
%    o status: Function RotateImage returns a pointer to the image after
%      rotating.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o degrees: Specifies the number of degrees to rotate the image.
%
%    o clip: A value other than zero clips the corners of the rotated
%      image and retains the original image size.
%
%
*/
Image *RotateImage(image,degrees,clip)
Image
  *image;

double
  degrees;

unsigned int
  clip;
{
  ColorPacket
    background;

  double
    x_shear,
    y_shear;

  Image
    *clipped_image,
    *integral_image,
    *rotated_image;

  int
    x_offset,
    y_offset;

  register int
    i;

  unsigned char
    *range_limit,
    *range_table;

  unsigned int
    height,
    rotations,
    width,
    y_width;

  /*
    Adjust rotation angle.
  */
  while (degrees < -45.0)
    degrees+=360.0;
  rotations=0;
  while (degrees > 45.0)
  {
    degrees-=90.0;
    rotations++;
  }
  rotations%=4;
  /*
    Calculate shear equations.
  */
  x_shear=(-tan(DegreesToRadians(degrees)/2.0));
  y_shear=sin(DegreesToRadians(degrees));
  integral_image=IntegralRotateImage(image,rotations);
  if ((x_shear == 0.0) || (y_shear == 0.0))
    return(integral_image);
  /*
    Initialize range table.
  */
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if (range_table == (unsigned char *) NULL)
    {
      DestroyImage(integral_image);
      Warning("unable to rotate image","memory allocation failed");
      return((Image *) NULL);
    }
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(unsigned char) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  /*
    Compute image size.
  */
  if ((rotations == 1) || (rotations == 3))
    {
      y_width=image->rows+(int) ceil(fabs(x_shear)*(double) (image->columns-1));
      x_offset=(image->rows+2*
        (int) ceil(fabs(x_shear)*(double) (image->columns-1))-image->rows)/2;
      y_offset=(image->columns+
        (int) ceil(fabs(y_shear)*(double) (y_width-1))-image->columns)/2;
      width=image->rows;
      height=image->columns;
    }
  else
    {
      y_width=image->columns+(int) ceil(fabs(x_shear)*(double) (image->rows-1));
      x_offset=(image->columns+2*
        (int) ceil(fabs(x_shear)*(double) (image->rows-1))-image->columns)/2;
      y_offset=(image->rows+
        (int) ceil(fabs(y_shear)*(double) (y_width-1))-image->rows)/2;
      width=image->columns;
      height=image->rows;
    }
  /*
    Surround image with border of background color.
  */
  background.red=image->pixels[0].red;
  background.green=image->pixels[0].green;
  background.blue=image->pixels[0].blue;
  rotated_image=BorderImage(integral_image,(unsigned int) x_offset,
    (unsigned int) y_offset+1,background);
  DestroyImage(integral_image);
  if (rotated_image == (Image *) NULL)
    {
      Warning("unable to rotate image","memory allocation failed");
      return((Image *) NULL);
    }
  rotated_image->class=DirectClass;
  /*
    Perform a fractional rotation.  First, shear the image rows.
  */
  XShearImage(rotated_image,x_shear,width,height,x_offset,
    (int) (rotated_image->rows-height-2)/2+1,background,range_limit);
  /*
    Shear the image columns.
  */
  YShearImage(rotated_image,y_shear,y_width,height,
    (int) (rotated_image->columns-y_width)/2,y_offset+1,background,range_limit);
  /*
    Shear the image rows again.
  */
  XShearImage(rotated_image,x_shear,y_width,rotated_image->rows-2,
    (int) (rotated_image->columns-y_width)/2,1,background,range_limit);
  (void) free((char *) range_table);
  clipped_image=ClipShearImage(rotated_image,x_shear,y_shear,width,height,clip);
  DestroyImage(rotated_image);
  return(clipped_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S h e a r I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ShearImage creates a new image that is a sheared copy of an
%  existing one.  Shearing slides one edge of an image along the X or Y
%  axis, creating a parallelogram.  An X direction shear slides an edge
%  along the X axis, while a Y direction shear slides an edge along the Y
%  axis.  The amount of the shear is controlled by a shear angle.  For X
%  direction shears, x_shear is measured relative to the Y axis, and
%  similarly, for Y direction shears y_shear is measured relative to the
%  X axis.  Empty triangles left over from shearing the image are filled
%  with the color defined by the pixel at location (0,0).  ShearImage
%  allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  Function ShearImage is based on the paper "A Fast Algorithm for General
%  Raster Rotatation" by Alan W. Paeth.  
%
%  The format of the ShearImage routine is:
%
%      ShearImage(image,x_shear,y_shear,clip)
%
%  A description of each parameter follows.
%
%    o status: Function ShearImage returns a pointer to the image after
%      rotating.  A null image is returned if there is a memory shortage.
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o x_shear, y_shear: Specifies the number of degrees to shear the image.
%
%    o clip: A value other than zero clips the corners of the rotated
%      image and retains the original image size.
%
%
*/
Image *ShearImage(image,x_shear,y_shear,clip)
Image
  *image;

double
  x_shear,
  y_shear;

unsigned int
  clip;
{
  ColorPacket
    background;

  Image
    *clipped_image,
    *sheared_image;

  int
    x_offset,
    y_offset;

  register int
    i;

  unsigned char
    *range_limit,
    *range_table;

  unsigned int
    y_width;

  /*
    Adjust rotation angle.
  */
  while (x_shear < -45.0)
    x_shear+=360.0;
  while (x_shear > 45.0)
    x_shear-=90.0;
  while (y_shear < -45.0)
    y_shear+=360.0;
  while (y_shear > 45.0)
    y_shear-=90.0;
  x_shear=tan(DegreesToRadians(x_shear)/2.0);
  y_shear=(-sin(DegreesToRadians(y_shear)));
  /*
    Initialize range table.
  */
  range_table=(unsigned char *) malloc(3*(MaxRGB+1)*sizeof(unsigned char));
  if (range_table == (unsigned char *) NULL)
    {
      Warning("unable to rotate image","memory allocation failed");
      return((Image *) NULL);
    }
  for (i=0; i <= MaxRGB; i++)
  {
    range_table[i]=0;
    range_table[i+(MaxRGB+1)]=(unsigned char) i;
    range_table[i+(MaxRGB+1)*2]=MaxRGB;
  }
  range_limit=range_table+(MaxRGB+1);
  /*
    Compute image size.
  */
  y_width=image->columns+(int) ceil(fabs(x_shear)*(double) (image->rows-1));
  x_offset=(image->columns+2*
    (int) ceil(fabs(x_shear)*(double) (image->rows-1))-image->columns)/2;
  y_offset=(image->rows+
    (int) ceil(fabs(y_shear)*(double) (y_width-1))-image->rows)/2;
  /*
    Surround image with border of background color.
  */
  background.red=image->pixels[0].red;
  background.green=image->pixels[0].green;
  background.blue=image->pixels[0].blue;
  sheared_image=BorderImage(image,(unsigned int) x_offset,
    (unsigned int) y_offset+1,background);
  if (sheared_image == (Image *) NULL)
    {
      Warning("unable to rotate image","memory allocation failed");
      return((Image *) NULL);
    }
  sheared_image->class=DirectClass;
  /*
    Shear the image rows.
  */
  if (x_shear != 0.0)
    XShearImage(sheared_image,x_shear,image->columns,image->rows,x_offset,
      (int) (sheared_image->rows-image->rows)/2+1,background,range_limit);
  /*
    Shear the image columns.
  */
  if (y_shear != 0.0)
    YShearImage(sheared_image,y_shear,y_width,image->rows,(int)
      (sheared_image->columns-y_width)/2,y_offset+1,background,range_limit);
  (void) free((char *) range_table);
  clipped_image=ClipShearImage(sheared_image,x_shear,y_shear,image->columns,
    image->rows,clip);
  DestroyImage(sheared_image);
  return(clipped_image);
}
