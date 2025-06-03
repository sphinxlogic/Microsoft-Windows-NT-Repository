/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              M   M   OOO    GGGG  RRRR   IIIII  FFFFF  Y   Y                %
%              MM MM  O   O  G      R   R    I    F       Y Y                 %
%              M M M  O   O  G GG   RRRRR    I    FFF      Y                  %
%              M   M  O   O  G   G  R R      I    F        Y                  %
%              M   M   OOO    GGG   R  R   IIIII  F        Y                  %
%                                                                             %
%                                                                             %
%               Transmogrify an Image or Sequence of Images.                  %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                            December 1992                                    %
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
%  `Mogrify' applies one or more image transforms to an image or sequence of
%   images and overwrites the original image.
%
%  The Mogrify program command syntax is:
%
%  Usage: mogrify [options ...] file [ [options ...] file ...]
%
%  Where options include:
%    -border geometry    surround image with a border of color
%    -clip geometry      preferred size and location of the clipped image
%    -colormap filename  transform image colors to match this set of colors
%    -colors value       preferred number of colors in the image
%    -colorspace type    GRAY, RGB, XYZ, YIQ, or YUV
%    -compress type      RunlengthEncoded or QEncoded
%    -density geometry   vertical and horizonal density of the image
%    -display server     obtain image or font from this X server
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -enhance            apply a digital filter to enhance a noisy image
%    -gamma value        level of gamma correction
%    -geometry geometry  preferred size of the image
%    -monochrome         transform image to black and white
%    -noise              reduce noise with a noise peak elimination filter
%    -normalize          tranform image to span the full range of colors
%    -reflect            reverse image scanlines
%    -roll geometry       roll an image vertically or horizontally
%    -rotate degrees     apply Paeth rotation to the image
%    -scale geometry     preferred size factors of the image
%    -scene value        image scene number
%    -shear geomatry     slide one edge of the image along the X or Y axis
%    -treedepth value    depth of the color classification tree
%    -verbose            print detailed information about the image
%
%  Change '-' to '+' in any option above to reverse its effect.  For
%  example, specify +compress to store the image as uncompressed.
%
%  By default, the image format of `file' is determined by its magic
%  number.  To specify a particular image format, precede the filename
%  with an image format name and a colon (i.e. mtv:image) or specify the
%  image type as the filename suffix (i.e. image.mtv).  Specify 'file' as
%  '-' for standard input or output.
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

/*
  Global declarations.
*/
char
  *client_name;

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E r r o r                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Error displays an error message and then terminates the program.
%
%  The format of the Error routine is:
%
%      Error(message,qualifier)
%
%  A description of each parameter follows:
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
void Error(message,qualifier)
char
  *message,
  *qualifier;
{
  (void) fprintf(stderr,"%s: %s",client_name,message);
  if (qualifier != (char *) NULL)
    (void) fprintf(stderr," (%s)",qualifier);
  (void) fprintf(stderr,".\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s a g e                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Usage displays the program command syntax.
%
%  The format of the Usage routine is:
%
%      Usage()
%
%
*/
static void Usage()
{
  char
    **p;

  static char
    *options[]=
    {
      "-border geometry    surround image with a border of color",
      "-clip geometry      preferred size and location of the clipped image",
      "-colormap filename  transform image colors to match this set of colors",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    GRAY, RGB, XYZ, YIQ, or YUV",
      "-compress type      RunlengthEncoded or QEncoded",
      "-density geometry   vertical and horizonal density of the image",
      "-display server     obtain image or font from this X server",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-enhance            apply a digital filter to enhance a noisy image",
      "-gamma value        level of gamma correction",
      "-geometry geometry  preferred size of the image",
      "-monochrome         transform image to black and white",
      "-noise              reduce noise with a noise peak elimination filter",
      "-normalize          tranform image to span the full range of colors",
      "-reflect            reflect the image scanlines",
      "-roll geometry      roll an image vertically or horizontally",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scale geometry     preferred size factors of the image",
      "-scene number       image scene number",
      "-shear geomatry     slide one edge of the image along the X or Y axis",
      "-treedepth value    depth of the color classification tree",
      "-verbose            print detailed information about the image",
      (char *) NULL
    };
  (void) fprintf(stderr,
    "Usage: %s [-options ...] file [ [-options ...] file ...]\n",client_name);
  (void) fprintf(stderr,"\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example, specify +compress to store the image as uncompressed.\n");
  (void) fprintf(stderr,
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) fprintf(stderr,
    "number.  To specify a particular image format, precede the filename\n");
  (void) fprintf(stderr,
    "with an image format name and a colon (i.e. mtv:image) or specify the\n");
  (void) fprintf(stderr,
    "image type as the filename suffix (i.e. image.mtv).  Specify 'file' as\n");
  (void) fprintf(stderr,"'-' for standard input or output.\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M a i n                                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/
int main(argc,argv)
int
  argc;

char
  **argv;
{
  AlienInfo
    alien_info;

  char
    *border_geometry,
    *clip_geometry,
    *colormap_filename,
    *image_geometry,
    *option,
    *roll_geometry,
    *scale_geometry,
    *shear_geometry;

  double
    gamma;

  Image
    **images;

  int
    degrees,
    flags,
    i,
    x,
    y;
         
  unsigned int
    colorspace,
    compression,
    dither,
    enhance,
    height,
    image_number,
    inverse,
    maximum_images,
    monochrome,
    noise,
    normalize,
    number_colors,
    reflect,
    scene,
    tree_depth,
    verbose,
    width;

  /*
    Display usage profile if there are no command line arguments.
  */
  client_name=(*argv);
  if (argc < 2)
    Usage();
  /*
    Set defaults.
  */
  GetAlienInfo(&alien_info);
  border_geometry=(char *) NULL;
  clip_geometry=(char *) NULL;
  colormap_filename=(char *) NULL;
  colorspace=RGBColorspace;
  compression=UndefinedCompression;
  degrees=0;
  dither=False;
  enhance=False;
  gamma=0.0;
  image_geometry=(char *) NULL;
  inverse=False;
  monochrome=False;
  noise=False;
  normalize=False;
  number_colors=0;
  reflect=False;
  roll_geometry=(char *) NULL;
  scale_geometry=(char *) NULL;
  shear_geometry=(char *) NULL;
  scene=0;
  tree_depth=0;
  verbose=False;
  maximum_images=2048;
  images=(Image **) malloc(maximum_images*sizeof(Image *));
  if (images == (Image **) NULL)
    Error("unable to mogrify images","memory allocation failed");
  /*
    Parse command line.
  */
  image_number=0;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if (((int) strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'b':
        {
          if (strncmp("border",option+1,2) == 0)
            {
              border_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing geometry on -border",(char *) NULL);
                  border_geometry=argv[i];
                }
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'c':
        {
          if (strncmp("clip",option+1,2) == 0)
            {
              clip_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing geometry on -clip",(char *) NULL);
                  clip_geometry=argv[i];
                }
              break;
            }
          if (strncmp("colormap",option+1,7) == 0)
            {
              colormap_filename=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing file name on -colormap",(char *) NULL);
                  colormap_filename=argv[i];
                }
              break;
            }
          if (strncmp("colors",option+1,7) == 0)
            {
              number_colors=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("missing colors on -colors",(char *) NULL);
                  number_colors=atoi(argv[i]);
                }
              break;
            }
          if (strncmp("colorspace",option+1,7) == 0)
            {
              colorspace=RGBColorspace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing type on -colorspace",(char *) NULL);
                  option=argv[i];
                  colorspace=UndefinedColorspace;
                  if (Latin1Compare("gray",option) == 0)
                    colorspace=GRAYColorspace;
                  if (Latin1Compare("rgb",option) == 0)
                    colorspace=RGBColorspace;
                  if (Latin1Compare("yiq",option) == 0)
                    colorspace=YIQColorspace;
                  if (Latin1Compare("yuv",option) == 0)
                    colorspace=YUVColorspace;
                  if (Latin1Compare("xyz",option) == 0)
                    colorspace=XYZColorspace;
                  if (colorspace == UndefinedColorspace)
                    Error("invalid colorspace type on -colorspace",option);
                }
              break;
            }
          if (strncmp("compress",option+1,3) == 0)
            {
              compression=NoCompression;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing type on -compress",(char *) NULL);
                  option=argv[i];
                  if (Latin1Compare("runlengthencoded",option) == 0)
                    compression=RunlengthEncodedCompression;
                  else
                    if (Latin1Compare("qencoded",option) == 0)
                      compression=QEncodedCompression;
                    else
                      Error("invalid compression type on -compress",option);
                }
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'd':
        {
          if (strncmp("density",option+1,3) == 0)
            {
              alien_info.density=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing density on -density",(char *) NULL);
                  alien_info.density=argv[i];
                }
              break;
            }
          if (strncmp("display",option+1,3) == 0)
            {
              alien_info.server_name=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing server name on -display",(char *) NULL);
                  alien_info.server_name=argv[i];
                }
              break;
            }
          if (strncmp("dither",option+1,3) == 0)
            {
              dither=(*option == '-');
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'e':
        {
          enhance=(*option == '-');
          break;
        }
        case 'g':
        {
          if (strncmp("gamma",option+1,2) == 0)
            {
              gamma=0.0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("missing gamma on -gamma",(char *) NULL);
                  gamma=atof(argv[i]);
                }
              break;
            }
          if (strncmp("geometry",option+1,2) == 0)
            {
              image_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing geometry on -geometry",(char *) NULL);
                  image_geometry=argv[i];
                }
              alien_info.geometry=image_geometry;
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'h':
        {
          if (strncmp("help",option+1,2) == 0)
            {
              Usage();
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'i':
        {
          inverse=(*option == '-');
          break;
        }
        case 'm':
        {
          monochrome=(*option == '-');
          break;
        }
        case 'n':
        {
          if (strncmp("noise",option+1,3) == 0)
            {
              noise=(*option == '-');
              break;
            }
          if (strncmp("normalize",option+1,3) == 0)
            {
              normalize=(*option == '-');
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'r':
        {
          if (strncmp("reflect",option+1,2) == 0)
            {
              reflect=(*option == '-');
              break;
            }
          if (strncmp("roll",option+1,3) == 0)
            {
              roll_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing geometry on -roll",(char *) NULL);
                  roll_geometry=argv[i];
                }
              break;
            }
          if (strncmp("rotate",option+1,3) == 0)
            {
              degrees=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("missing degrees on -rotate",(char *) NULL);
                  degrees=atoi(argv[i]);
                }
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 's':
        {
          if (strncmp("scale",option+1,3) == 0)
            {
              scale_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("missing scale geometry on -scale",(char *) NULL);
                  scale_geometry=argv[i];
                }
              break;
            }
          if (strncmp("scene",option+1,3) == 0)
            {
              scene=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("missing scene number on -scene",(char *) NULL);
                  scene=atoi(argv[i]);
                }
              break;
            }
          if (strncmp("shear",option+1,2) == 0)
            {
              shear_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("missing shear geometry on -shear",(char *) NULL);
                  shear_geometry=argv[i];
                }
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 't':
        {
          tree_depth=0;
          if (*option == '-')
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                Error("missing depth on -treedepth",(char *) NULL);
              tree_depth=atoi(argv[i]);
            }
          break;
        }
        case 'v':
        {
          verbose=(*option == '-');
          alien_info.verbose=verbose;
          break;
        }
        default:
        {
          Error("unrecognized option",option);
          break;
        }
      }
    else
      {
        double
          normalized_maximum_error,
          normalized_mean_error;

        Image
          *image,
          info_image,
          *next_image;

        time_t
          start_time;

        unsigned int
          mean_error_per_pixel;

        unsigned long
          total_colors;

        /*
          Option is a file name: begin by reading image from specified file.
        */
        start_time=time((time_t *) 0);
        (void) strcpy(alien_info.filename,option);
        image=ReadAlienImage(&alien_info);
        if (image == (Image *) NULL)
          if (*option == '-')
            break;
          else
            continue;
        do
        {
          info_image=(*image);
          if (scene > 0)
            image->scene=scene;
          total_colors=0;
          /*
            Transform image as defined by the clip, image and scale geometries.
          */
          TransformImage(&image,clip_geometry,image_geometry,scale_geometry);
          if (reflect)
            {
              Image
                *reflected_image;

              /*
                Reverse image scanlines.
              */
              reflected_image=ReflectImage(image);
              if (reflected_image != (Image *) NULL)
                {
                  DestroyImage(image);
                  image=reflected_image;
                }
            }
          if (roll_geometry != (char *) NULL)
            {
              Image
                *rolled_image;

              /*
                Roll image.
              */
              x=0;
              y=0;
              flags=XParseGeometry(roll_geometry,&x,&y,&width,&height);
              rolled_image=RollImage(image,x,y);
              if (rolled_image != (Image *) NULL)
                {
                  DestroyImage(image);
                  image=rolled_image;
                }
            }
          if (border_geometry != (char *) NULL)
            {
              Image
                *bordered_image;

              /*
                Surround image with a border of solid.
              */
              bordered_image=ReadXCImage("border",(char *) NULL,"1x1");
              if (bordered_image != (Image *) NULL)
                {
                  ColorPacket
                    border_color;

                  border_color.red=bordered_image->pixels->red;
                  border_color.green=bordered_image->pixels->green;
                  border_color.blue=bordered_image->pixels->blue;
                  DestroyImage(bordered_image);
                  width=0;
                  height=0;
                  flags=XParseGeometry(border_geometry,&x,&y,&width,&height);
                  bordered_image=BorderImage(image,width,height,border_color);
                  if (bordered_image != (Image *) NULL)
                    {
                      DestroyImage(image);
                      bordered_image->class=DirectClass;
                      image=bordered_image;
                    }
                }
            }
          if (shear_geometry != (char *) NULL)
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
              (void) sscanf(shear_geometry,"%fx%f",&x_shear,&y_shear);
              sheared_image=ShearImage(image,x_shear,y_shear,False);
              if (sheared_image != (Image *) NULL)
                {
                  DestroyImage(image);
                  image=sheared_image;
                }
            }
          if ((degrees % 360) != 0)
            {
              Image
                *rotated_image;

              /*
                Rotate image.
              */
              rotated_image=RotateImage(image,(double) degrees,False);
              if (rotated_image != (Image *) NULL)
                {
                  DestroyImage(image);
                  image=rotated_image;
                }
            }
          if (enhance)
            {
              Image
                *enhanced_image;

              /*
                Enhance image.
              */
              enhanced_image=EnhanceImage(image);
              if (enhanced_image != (Image *) NULL)
                {
                  DestroyImage(image);
                  image=enhanced_image;
                }
            }
          if (noise)
            {
              Image
                *noisy_image;

              /*
                Reduce noise in image.
              */
              noisy_image=NoisyImage(image);
              if (noisy_image != (Image *) NULL)
                {
                  DestroyImage(image);
                  image=noisy_image;
                }
            }
          if (gamma > 0.0)
            GammaImage(image,gamma);
          if (inverse)
            InverseImage(image);
          if (normalize)
            NormalizeImage(image);
          if (colorspace == GRAYColorspace)
            QuantizeImage(image,256,8,dither,GRAYColorspace,True);
          if (monochrome)
            QuantizeImage(image,2,8,dither,GRAYColorspace,True);
          if (number_colors > 0)
            if ((image->class == DirectClass) ||
                (image->colors > number_colors))
              {
                /*
                  Reduce the number of colors in the image.
                */
                QuantizeImage(image,number_colors,tree_depth,dither,colorspace,
                  True);
                if (verbose)
                  {
                    /*
                      Measure quantization error.
                    */
                    QuantizationError(image,&mean_error_per_pixel,
                      &normalized_mean_error,&normalized_maximum_error);
                    total_colors=NumberColors(image,(FILE *) NULL);
                  }
                SyncImage(image);
              }
          if (verbose)
            {
              /*
                Display detailed info about the image.
              */
              (void) fprintf(stderr,"[%u] %s",
                image->scene == 0 ? image_number : image->scene,
                info_image.filename);
              (void) fprintf(stderr," %ux%u",info_image.columns,
                info_image.rows);
              if ((info_image.columns != image->columns) ||
                  (info_image.rows != image->rows))
                (void) fprintf(stderr,"=>%ux%u",image->columns,image->rows);
              if (image->class == DirectClass)
                (void) fprintf(stderr," DirectClass ");
              else
                (void) fprintf(stderr," PseudoClass ");
              if (total_colors == 0)
                (void) fprintf(stderr,"%uc",image->colors);
              else
                {
                  (void) fprintf(stderr,"%lu=>%uc",total_colors,image->colors);
                  (void) fprintf(stderr," %u/%.6f/%.6fe",mean_error_per_pixel,
                    normalized_mean_error,normalized_maximum_error);
                }
              (void) fprintf(stderr," %s %lds\n",image->magick,
                time((time_t *) 0)-start_time+1);
            }
          if (compression != UndefinedCompression)
            image->compression=compression;
          else
            image->compression=info_image.compression;
          if (number_colors == 0)
            (void) WriteAlienImage(image);
          if (image_number == maximum_images)
            {
              /*
                Increase size of images array.
              */
              maximum_images<<=1;
              images=(Image **)
                realloc((char *) images,maximum_images*sizeof(Image *));
              if (images == (Image **) NULL)
                Error("unable to mogrify images","memory allocation failed");
            }
          images[image_number++]=image;
          next_image=image->next;
          if (next_image != (Image *) NULL)
            image=next_image;
        } while (next_image != (Image *) NULL);
        if (number_colors == 0)
          DestroyImages(image);
      }
    }
  if (image_number == 0)
    Error("missing an image file name",(char *) NULL);
  if (number_colors > 0)
    {
      Image
        *colormap_image;

      /*
        Global colormap option;  reduce colors then write image.
      */
      colormap_image=(Image *) NULL;
      if (colormap_filename != (char *) NULL)
        {
          (void) strcpy(alien_info.filename,colormap_filename);
          colormap_image=ReadAlienImage(&alien_info);
          if (colormap_image == (Image *) NULL)
            Error("unable to mogrify images","cannot read image colormap");
        }
      maximum_images=image_number;
      QuantizeImages(images,maximum_images,colormap_image,number_colors,
        tree_depth,dither,colorspace,True);
      if (colormap_image != (Image *) NULL)
        DestroyImage(colormap_image);
      for (image_number=0; image_number < maximum_images; image_number++)
      {
        (void) WriteAlienImage(images[image_number]);
        DestroyImage(images[image_number]);
      }
    }
  (void) free((char *) images);
  return(False);
}
