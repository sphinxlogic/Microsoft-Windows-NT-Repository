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
%                                April 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright 1992 E. I. Dupont de Nemours & Company                           %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee,           %
%  provided that the above Copyright notice appear in all copies and that     %
%  both that Copyright notice and this permission notice appear in            %
%  supporting documentation, and that the name of E. I. Dupont de Nemours     %
%  & Company not be used in advertising or publicity pertaining to            %
%  distribution of the software without specific, written prior               %
%  permission.  E. I. Dupont de Nemours & Company makes no representations    %
%  about the suitability of this software for any purpose.  It is provided    %
%  "as is" without express or implied warranty.                               %
%                                                                             %
%  E. I. Dupont de Nemours & Company disclaims all warranties with regard     %
%  to this software, including all implied warranties of merchantability      %
%  and fitness, in no event shall E. I. Dupont de Nemours & Company be        %
%  liable for any special, indirect or consequential damages or any           %
%  damages whatsoever resulting from loss of use, data or profits, whether    %
%  in an action of contract, negligence or other tortious action, arising     %
%  out of or in connection with the use or performance of this software.      %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  The combine program syntax is:
%
%  Usage: combine [options ...] input_file1 input_file2 output_file
%
%  Where options include:
%    -alpha              store alpha channel if the image has one
%    -colors value       preferred number of colors in the image
%    -compose operator   composite operator
%    -colorspace type    GRAY, RGB, XYZ, YIQ, or YUV
%    -compress type      RunlengthEncoded or QEncoded
%    -density geometry   vertical and horizonal density of the image
%    -display server     obtain image or font from this X server
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -geometry geometry  width and height of the image
%    -quality value      JPEG quality setting
%    -scene value        image scene number
%    -treedepth value    depth of the color classification tree
%    -verbose            print detailed information about the image
%
%  Change '-' to '+' in any option above to reverse its effect.  For
%  example,  specify +alpha to store the image without its alpha channel.
%
%
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
      "-alpha              store alpha channel if the image has one",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    GRAY, RGB, XYZ, YIQ, or YUV",
      "-compose operator   composite operator",
      "-compress type      RunlengthEncoded or QEncoded",
      "-density geometry   vertical and horizonal density of the image",
      "-display server     obtain image or font from this X server",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-geometry geometry  width and height of the image",
      "-quality value      JPEG quality setting",
      "-scene value        image scene number",
      "-treedepth value    depth of the color classification tree",
      "-verbose            print detailed information about the image",
      (char *) NULL
    };
  (void) fprintf(stderr,
    "Usage: %s [options ...] input_file1 input_file2 output_file\n",
    client_name);
  (void) fprintf(stderr,"\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example,  specify +alpha to store the image without an alpha channel.\n");
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

  AlienInfo
    alien_info;

  char
    *filename,
    *option;

  double
    normalized_maximum_error,
    normalized_mean_error;
 
  Image
    *image,
    *alpha_image,
    *beta_image;

  int
    i,
    status,
    x,
    x_offset,
    y_offset;

  time_t
    start_time;

  unsigned int
    alpha,
    colorspace,
    compose,
    compression,
    dither,
    mean_error_per_pixel,
    number_colors,
    quality,
    scene,
    size,
    tree_depth,
    verbose;

  unsigned long int
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
  GetAlienInfo(&alien_info);
  alpha=NotInitialized;
  alpha_image=(Image *) NULL;
  beta_image=(Image *) NULL;
  colorspace=RGBColorspace;
  compose=OverCompositeOp;
  compression=UndefinedCompression;
  dither=False;
  number_colors=0;
  quality=0;
  scene=0;
  start_time=time((time_t *) 0);
  tree_depth=0;
  verbose=False;
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
        filename=option;
        (void) strcpy(alien_info.filename,filename);
        if (alpha_image == (Image *) NULL)
          {
            alpha_image=ReadAlienImage(&alien_info);
            if (alpha_image == (Image *) NULL)
              exit(1);
            continue;
          }
        if (beta_image != (Image *) NULL)
          Error("input images already specified",filename);
        beta_image=ReadAlienImage(&alien_info);
        if (beta_image == (Image *) NULL)
          exit(1);
      }
    else
      switch(*(option+1))
      {
        case 'a':
        {
          alpha=(*option == '-');
          break;
        }
        case 'c':
        {
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
          if (strncmp("compose",option+1,5) == 0)
            {
              compose=ReplaceCompositeOp;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing type on -compose",(char *) NULL);
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
                    Error("invalid compose type on -compose",option);
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
        case 'f':
        {
          alien_info.font=(char *) NULL;
          if (*option == '-')
            {
              i++;
              if (i == argc)
                Error("missing font name on -font",(char *) NULL);
              alien_info.font=argv[i];
            }
          break;
        }
        case 'g':
        {
          alien_info.geometry=(char *) NULL;
          if (*option == '-')
            {
              i++;
              if (i == argc)
                Error("missing geometry on -geometry",(char *) NULL);
              alien_info.geometry=argv[i];
            }
          break;
        }
        case 'h':
        {
          Usage();
          break;
        }
        case 'q':
        {
          i++;
          if ((i == argc) || !sscanf(argv[i],"%d",&x))
            Error("missing quality on -quality",(char *) NULL);
          quality=atoi(argv[i]);
          break;
        }
        case 's':
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
  }
  /*
    Apply digital composite combine.
  */
  x_offset=0;
  y_offset=0;
  if (alien_info.geometry != (char *) NULL)
    (void) XParseGeometry(alien_info.geometry,&x_offset,&y_offset,&size,&size);
  CompositeImage(beta_image,compose,alpha_image,x_offset,y_offset);
  /*
    Write image.
  */
  total_colors=0;
  if (alpha != NotInitialized)
    beta_image->alpha=alpha;
  if (compression != UndefinedCompression)
    beta_image->compression=compression;
  if (quality != 0)
    beta_image->quality=quality;
  if (scene != 0)
    beta_image->scene=scene;
  (void) strcpy(beta_image->filename,argv[i]);
  if (colorspace == GRAYColorspace)
    QuantizeImage(beta_image,256,8,dither,GRAYColorspace,True);
  if (number_colors != 0)
    if ((beta_image->class == DirectClass) ||
        (beta_image->colors > number_colors))
      {
        /*
          Reduce the number of colors in the image.
        */
        QuantizeImage(beta_image,number_colors,tree_depth,dither,colorspace,
          True);
        if (verbose)
          {
            /*
              Measure quantization error.
            */
            QuantizationError(beta_image,&mean_error_per_pixel,
              &normalized_mean_error,&normalized_maximum_error);
            total_colors=NumberColors(beta_image,(FILE *) NULL);
          }
        SyncImage(beta_image);
      }
  status=WriteAlienImage(beta_image);
  if (verbose)
    {
      /*
        Display detailed info about the image.
      */
      (void) fprintf(stderr,"[%u] %s %s=>%s %ux%u",beta_image->scene,
	alpha_image->filename,filename,beta_image->filename,
	beta_image->columns,beta_image->rows);
      if (image->class == DirectClass)
        (void) fprintf(stderr," DirectClass ");
      else
        (void) fprintf(stderr," PseudoClass ");
      if (total_colors == 0)
        (void) fprintf(stderr,"%uc",beta_image->colors);
      else
        {
          (void) fprintf(stderr,"%lu=>%uc",total_colors,beta_image->colors);
          (void) fprintf(stderr," %u/%.6f/%.6fe",mean_error_per_pixel,
            normalized_mean_error,normalized_maximum_error);
        }
      (void) fprintf(stderr," %s %lds\n",beta_image->magick,
        time((time_t *) 0)-start_time+1);
    }
  return(!status);
}
