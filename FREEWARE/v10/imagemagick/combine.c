/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                CCCC   OOO   M   M  BBBB   IIIII  N   N  EEEEE               %
%               C      O   O  MM MM  B   B    I    NN  N  E                   %
%               C      O   O  M M M  BBBB     I    N N N  EEE                 %
%               C      O   O  M   M  B   B    I    N  NN  E                   %
%                CCCC   OOO   M   N  BBBB   IIIII  N   N  EEEEE               %
%                                                                             %
%                                                                             %
%                        Digitally combine two images.                        %
%                                                                             %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               January 1993                                  %
%                                                                             %
%                                                                             %
%  Copyright 1994 E. I. Dupont de Nemours and Company                         %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. Dupont de Nemours     %
%  and Company not be used in advertising or publicity pertaining to          %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. Dupont de Nemours and Company makes no representations  %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. Dupont de Nemours and Company disclaims all warranties with regard   %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. Dupont de Nemours and Company be      %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortuous action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The combine program syntax is:
%
%  Usage: combine [options ...] input_file1 input_file2 output_file
%
%  Where options include:
%    -blend value        blend the two images a given percent
%    -colors value       preferred number of colors in the image
%    -compose operator   composite operator
%    -colorspace type    GRAY, OHTA, RGB, XYZ, YCbCr, YIQ, YPbPr, or YUV
%    -comment string     annotate image with comment
%    -compress type      RunlengthEncoded or QEncoded
%    -density geometry   vertical and horizontal density of the image
%    -display server     obtain image or font from this X server
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -font name          X11 font for displaying text
%    -geometry geometry  perferred size or location of the image
%    -interlace type     NONE, LINE, or PLANE
%    -label name         assign a label to an image
%    -matte              store matte channel if the image has one
%    -monochrome         transform image to black and white
%    -page geometry      size and location of the Postscript page
%    -quality value      JPEG quality setting
%    -scene value        image scene number
%    -size geometry      width and height of image
%    -stereo             combine two images to form red-green stereo image
%    -treedepth value    depth of the color classification tree
%    -verbose            print detailed information about the image
%
%  Change '-' to '+' in any option above to reverse its effect.  For
%  example,  specify +matte to store the image without its matte channel.
%
%
*/

#include "magick.h"
#include "image.h"
#include "X.h"

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
%  Procedure Usage displays the program usage;
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
      "-blend value        blend the two images a given percent",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    GRAY, OHTA, RGB, XYZ, YCbCr, YIQ, YPbPr, or YUV",
      "-comment string     annotate image with comment",
      "-compose operator   composite operator",
      "-compress type      RunlengthEncoded or QEncoded",
      "-density geometry   vertical and horizontal density of the image",
      "-display server     obtain image or font from this X server",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-font name          X11 font for displaying text",
      "-geometry geometry  perferred size or location of the image",
      "-interlace type     NONE, LINE, or PLANE",
      "-label name         assign a label to an image",
      "-matte              store matte channel if the image has one",
      "-monochrome         transform image to black and white",
      "-page geometry      size and location of the Postscript page",
      "-quality value      JPEG quality setting",
      "-scene value        image scene number",
      "-size geometry      width and height of image",
      "-stereo             combine two images to form red-green stereo image",
      "-treedepth value    depth of the color classification tree",
      "-verbose            print detailed information about the image",
      (char *) NULL
    };

  (void) fprintf(stderr,"Version: %s\n\n",Version);
  (void) fprintf(stderr,
    "Usage: %s [options ...] input_file1 input_file2 output_file\n",
    client_name);
  (void) fprintf(stderr,"\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example,  specify +matte to store the image without an matte channel.\n");
  exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a i n                                                                    %
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
  *argv[];
{
#define NotInitialized  (unsigned int) (~0)

  char
    *filename,
    *geometry,
    *option,
    *write_filename;

  double
    blend;

  Image
    *alpha_image,
    *beta_image,
    *combined_image;

  ImageInfo
    image_info;

  int
    i,
    status,
    x;

  unsigned int
    colorspace,
    compose,
    compression,
    matte,
    scene,
    stereo;

  unsigned long
    total_colors;

  /*
    Initialize program variables.
  */
  client_name=argv[0];
  if (argc < 4)
    Usage();
  /*
    Read image and convert to MIFF format.
  */
  alpha_image=(Image *) NULL;
  beta_image=(Image *) NULL;
  blend=0.0;
  colorspace=RGBColorspace;
  compose=ReplaceCompositeOp;
  compression=UndefinedCompression;
  geometry=(char *) NULL;
  GetImageInfo(&image_info);
  matte=NotInitialized;
  scene=0;
  stereo=False;
  write_filename=argv[argc-1];
  /*
    Check command syntax.
  */
  filename=(char *) NULL;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if (((int) strlen(option) < 2) || ((*option != '-') && (*option != '+')))
      {
        /*
          Read input images.
        */
        filename=argv[i];
        (void) strcpy(image_info.filename,filename);
        if (alpha_image == (Image *) NULL)
          {
            alpha_image=ReadImage(&image_info);
            if (alpha_image == (Image *) NULL)
              exit(1);
            continue;
          }
        if (beta_image != (Image *) NULL)
          Error("input images already specified",filename);
        beta_image=ReadImage(&image_info);
        if (beta_image == (Image *) NULL)
          exit(1);
      }
    else
      switch(*(option+1))
      {
        case 'b':
        {
          if (strncmp("blend",option+1,3) == 0)
            {
              blend=0.0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing value on -blend",(char *) NULL);
                  blend=atof(argv[i]);
                  compose=OverCompositeOp;
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'c':
        {
          if (strncmp("colors",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing colors on -colors",(char *) NULL);
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
                    Error("Missing type on -colorspace",(char *) NULL);
                  option=argv[i];
                  colorspace=UndefinedColorspace;
                  if (Latin1Compare("gray",option) == 0)
                    colorspace=GRAYColorspace;
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
                  if (colorspace == UndefinedColorspace)
                    Error("Invalid colorspace type on -colorspace",option);
                }
              break;
            }
          if (strncmp("comment",option+1,4) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing comment on -comment",(char *) NULL);
                }
              break;
            }
          if (strncmp("compose",option+1,5) == 0)
            {
              compose=ReplaceCompositeOp;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing type on -compose",(char *) NULL);
                  option=argv[i];
                  compose=UndefinedCompositeOp;
                  if (Latin1Compare("over",option) == 0)
                    compose=OverCompositeOp;
                  if (Latin1Compare("in",option) == 0)
                    compose=InCompositeOp;
                  if (Latin1Compare("out",option) == 0)
                    compose=OutCompositeOp;
                  if (Latin1Compare("atop",option) == 0)
                    compose=AtopCompositeOp;
                  if (Latin1Compare("xor",option) == 0)
                    compose=XorCompositeOp;
                  if (Latin1Compare("plus",option) == 0)
                    compose=PlusCompositeOp;
                  if (Latin1Compare("minus",option) == 0)
                    compose=MinusCompositeOp;
                  if (Latin1Compare("add",option) == 0)
                    compose=AddCompositeOp;
                  if (Latin1Compare("subtract",option) == 0)
                    compose=SubtractCompositeOp;
                  if (Latin1Compare("difference",option) == 0)
                    compose=DifferenceCompositeOp;
                  if (Latin1Compare("replace",option) == 0)
                    compose=ReplaceCompositeOp;
                  if (compose == UndefinedCompositeOp)
                    Error("Invalid compose type on -compose",option);
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
                    Error("Missing type on -compress",(char *) NULL);
                  option=argv[i];
                  if (Latin1Compare("runlengthencoded",option) == 0)
                    compression=RunlengthEncodedCompression;
                  else
                    if (Latin1Compare("qencoded",option) == 0)
                      compression=QEncodedCompression;
                    else
                      Error("Invalid compression type on -compress",option);
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'd':
        {
          if (strncmp("density",option+1,3) == 0)
            {
              image_info.density=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -density",(char *) NULL);
                  image_info.density=argv[i];
                }
              break;
            }
          if (strncmp("display",option+1,3) == 0)
            {
              image_info.server_name=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing server name on -display",(char *) NULL);
                  image_info.server_name=argv[i];
                }
              break;
            }
          if (strncmp("dither",option+1,3) == 0)
            {
              image_info.dither=(*option == '-');
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'f':
        {
          image_info.font=(char *) NULL;
          if (*option == '-')
            {
              i++;
              if (i == argc)
                Error("Missing font name on -font",(char *) NULL);
              image_info.font=argv[i];
            }
          break;
        }
        case 'g':
        {
          if (strncmp("geometry",option+1,2) == 0)
            {
              geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -geometry",(char *) NULL);
                  geometry=argv[i];
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'h':
        {
          Usage();
          break;
        }
        case 'i':
        {
          if (strncmp("interlace",option+1,3) == 0)
            {
              image_info.interlace=NoneInterlace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing type on -interlace",(char *) NULL);
                  option=argv[i];
                  image_info.interlace=UndefinedInterlace;
                  if (Latin1Compare("none",option) == 0)
                    image_info.interlace=NoneInterlace;
                  if (Latin1Compare("line",option) == 0)
                    image_info.interlace=LineInterlace;
                  if (Latin1Compare("plane",option) == 0)
                    image_info.interlace=PlaneInterlace;
                  if (image_info.interlace == UndefinedInterlace)
                    Error("Invalid interlace type on -interlace",option);
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'm':
        {
          if (strncmp("matte",option+1,5) == 0)
            {
              matte=(*option == '-');
              break;
            }
          if (strncmp("monochrome",option+1,2) == 0)
            break;
          Error("Unrecognized option",option);
          break;
        }
        case 'l':
        {
          if (strncmp("label",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing label name on -label",(char *) NULL);
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'p':
        {
          if (strncmp("page",option+1,2) == 0)
            {
              image_info.page=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing page geometry on -page",(char *) NULL);
                  image_info.page=argv[i];
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'q':
        {
          i++;
          if ((i == argc) || !sscanf(argv[i],"%d",&x))
            Error("Missing quality on -quality",(char *) NULL);
          image_info.quality=atoi(argv[i]);
          break;
        }
        case 's':
        {
          if (strncmp("scene",option+1,2) == 0)
            {
              scene=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing scene number on -scene",(char *) NULL);
                  scene=atoi(argv[i]);
                }
              break;
            }
          if (strncmp("size",option+1,2) == 0)
            {
              image_info.size=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -size",(char *) NULL);
                  image_info.size=argv[i];
                }
              break;
            }
          if (strncmp("stereo",option+1,2) == 0)
            {
              stereo=(*option == '-');
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (strncmp("treedepth",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing depth on -treedepth",(char *) NULL);
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'v':
        {
          image_info.verbose=(*option == '-');
          break;
        }
        default:
        {
          Error("Unrecognized option",option);
          break;
        }
      }
  }
  if ((alpha_image == (Image *) NULL) || (beta_image == (Image *) NULL))
    Usage();
  if (blend != 0.0)
    {
      register RunlengthPacket
        *p;

      unsigned short
	index;

      /*
        Create mattes for blending.
      */
      index=(unsigned short) (blend*MaxRGB/100.0);
      alpha_image->class=DirectClass;
      alpha_image->matte=True;
      p=alpha_image->pixels;
      for (i=0; i < alpha_image->packets; i++)
      {
        p->index=index;
        p++;
      }
      index=MaxRGB-(unsigned short) (blend*MaxRGB/100.0);
      beta_image->class=DirectClass;
      beta_image->matte=True;
      p=beta_image->pixels;
      for (i=0; i < beta_image->packets; i++)
      {
        p->index=index;
        p++;
      }
    }
  /*
    Combine image.
  */
  if (stereo)
    combined_image=StereoImage(alpha_image,beta_image);
  else
    {
      int
        x_offset,
        y_offset;

      unsigned int
        size;

      /*
        Digitally composite image.
      */
      x_offset=0;
      y_offset=0;
      if (geometry != (char *) NULL)
        (void) XParseGeometry(geometry,&x_offset,&y_offset,&size,&size);
      CompositeImage(beta_image,compose,alpha_image,x_offset,y_offset);
      combined_image=beta_image;
    }
  if (combined_image == (Image *) NULL)
    exit(1);
  /*
    Write image.
  */
  total_colors=0;
  if (matte != NotInitialized)
    combined_image->matte=matte;
  if (compression != UndefinedCompression)
    combined_image->compression=compression;
  if (scene != 0)
    combined_image->scene=scene;
  (void) strcpy(combined_image->filename,write_filename);
  /*
    Transmogrify image as defined by the image processing options.
  */
  MogrifyImage(&image_info,argc,argv,&combined_image);
  status=WriteImage(&image_info,combined_image);
  if (image_info.verbose)
    DescribeImage(combined_image);
  (void) free((char *) image_info.filename);
  exit(0);
  return(False);
}
