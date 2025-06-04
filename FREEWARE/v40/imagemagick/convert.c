/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                CCCC   OOO   N   N  V   V  EEEEE  RRRR   TTTTT               %
%               C      O   O  NN  N  V   V  E      R   R    T                 %
%               C      O   O  N N N  V   V  EEE    RRRR     T                 %
%               C      O   O  N  NN   V V   E      R R      T                 %
%                CCCC   OOO   N   N    V    EEEEE  R  R     T                 %
%                                                                             %
%                                                                             %
%                Convert an image from one format to another.                 %
%                                                                             %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                April 1992                                   %
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
%  Convert converts an input file using one image format to an output file
%  with a differing image format.
%
%  The convert program syntax is:
%
%  Usage: convert [options ...] input_file output_file
%
%  Where options include:
%    -blur                apply a filter to blur the image
%    -border geometry     surround image with a border of color
%    -colors value        preferred number of colors in the image
%    -colorspace type     GRAY, OHTA, RGB, XYZ, YCbCr, YIQ, YPbPr, or YUV
%    -comment string      annotate image with comment
%    -compress type       RunlengthEncoded or QEncoded
%    -contrast            enhance or reduce the image contrast
%    -crop geometry       preferred size and location of the cropped image
%    -density geometry    vertical and horizontal density of the image
%    -despeckle           reduce the speckles within an image"
%    -display server      obtain image or font from this X server
%    -dither              apply Floyd/Steinberg error diffusion to image
%    -edge                apply a filter to detect edges in the image
%    -enhance             apply a digital filter to enhance a noisy image
%    -equalize            perform histogram equalization to an image
%    -flip                flip image in the vertical direction
%    -flop                flop image in the horizontal direction
%    -font name           X11 font for displaying text
%    -gamma value         level of gamma correction
%    -geometry geometry   perferred size or location of the image
%    -interlace type      NONE, LINE, or PLANE
%    -label name          assign a label to an image
%    -map filename        transform image colors to match this set of colors
%    -matte               store matte channel if the image has one
%    -modulate value      vary the brightness, saturation and hue
%    -monochrome          transform image to black and white
%    -negate              apply color inversion to image
%    -noise               reduce noise with a noise peak elimination filter
%    -normalize           transform image to span the full range of colors
%    -page geometry       size and location of the Postscript page
%    -quality value       JPEG quality setting
%    -roll geometry       roll an image vertically or horizontally
%    -rotate degrees      apply Paeth rotation to the image
%    -sample geometry     scale image with pixel sampling
%    -scene value         image scene number
%    -sharpen             apply a filter to sharpen the image
%    -shear geometry      slide one edge of the image along the X or Y axis
%    -size geometry       width and height of image
%    -treedepth value     depth of the color classification tree
%    -undercolor geometry control undercolor removal and black generation
%    -verbose             print detailed information about the image
%
%  Change '-' to '+' in any option above to reverse its effect.  For
%  example,  specify +matte to store the image without its matte channel.
%
%  By default, the image format of `file' is determined by its magic
%  number.  To specify a particular image format, precede the filename
%  with an image format name and a colon (i.e. ps:image) or specify the
%  image type as the filename suffix (i.e. image.ps).  Specify 'file' as
%  '-' for standard input or output.
%
%  Convert recognizes the following image formats:
%
%    Tag   Description
%    ---------------------------------------------------
%    AVS   AVS X image file.
%    BMP   Microsoft Windows bitmap image file.
%    CMYK  Raw cyan, magenta, yellow, and black bytes.
%    EPS   Adobe Encapsulated PostScript file.
%    EPSF  Adobe Encapsulated PostScript file.
%    EPSI  Adobe Encapsulated PostScript Interchange format.
%    FAX   Group 3.
%    FITS  Flexible Image Transport System.
%    GIF   Compuserve Graphics image file.
%    GIF87 Compuserve Graphics image file (version 87a).
%    GRAY  Raw gray bytes.
%    HISTOGRAM
%    IRIS  SGI RGB image file.
%    JPEG
%    MAP   colormap intensities and indices
%    MATTE Raw matte bytes.
%    MIFF  Magick image file format.
%    MTV
%    NULL  NULL image.
%    PCD   Photo CD
%    PCX   ZSoft IBM PC Paintbrush file
%    PICT  Apple Macintosh QuickDraw/PICT file.
%    PNM   Portable bitmap.
%    PS    Adobe PostScript file.
%    PS2   Adobe Level II PostScript file.
%    RAD   Radiance image file.
%    RGB   Raw red, green, and blue bytes.
%    RLE   Utah Run length encoded image file; read only.
%    SUN   SUN Rasterfile.
%    TEXT  raw text file; read only.
%    TGA   Truevision Targa image file.
%    TIFF  Tagged Image File Format.
%    VICAR read only.
%    VIFF  Khoros Visualization image file.
%    X     select image from X server screen.
%    XC    constant image of X server color.
%    XBM   X11 bitmap file.
%    XPM   X11 pixmap file.
%    XWD   X Window System window dump image file.
%    YUV   CCIR 601 1:1:1 file.
%    YUV3  CCIR 601 2:1:1 files.
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
    *ImageTypes[]=
    {
      "Tag   Description",
      "------------------------------------------------------------",
      "AVS   AVS X image file.",
      "BMP   Microsoft Windows bitmap image file.",
      "CMYK  Raw cyan, magenta, yellow, and black bytes.",
      "EPS   Adobe Encapsulated PostScript file.",
      "EPSF  Adobe Encapsulated PostScript file.",
      "EPSI  Adobe Encapsulated PostScript Interchange format.",
      "FAX   Group 3.",
      "FITS  Flexible Image Transport System.",
      "GIF   Compuserve Graphics image file.",
      "GIF87 Compuserve Graphics image file (version 87a).",
      "GRAY  Raw gray bytes.",
      "HISTOGRAM",
      "IRIS  SGI RGB image file.",
      "JPEG",
      "MAP   colormap intensities and indices.",
      "MATTE Raw matte bytes.",
      "MIFF  Magick image file format.",
      "MTV",
      "NULL  NULL image.",
      "PCD   Photo CD.",
      "PCX   ZSoft IBM PC Paintbrush file.",
      "PICT  Apple Macintosh QuickDraw/PICT file.",
      "PNM   Portable bitmap.",
      "PS    Adobe PostScript file.",
      "PS2   Adobe Level II PostScript file.",
      "RAD   Radiance image file.",
      "RGB   Raw red, green, and blue bytes.",
      "RLE   Utah Run length encoded image file; read only.",
      "SUN   SUN Rasterfile.",
      "TEXT  raw text file; read only.",
      "TGA   Truevision Targa image file.",
      "TIFF  Tagged Image File Format.",
      "VICAR read only.",
      "VIFF  Khoros Visualization image file.",
      "X     select image from X server screen.",
      "XC    constant image of X server color.",
      "XBM   X11 bitmap file.",
      "XPM   X11 pixmap file.",
      "XWD   X Window System window dump image file.",
      "YUV   CCIR 601 1:1:1 file.",
      "YUV3  CCIR 601 2:1:1 files.",
      (char *) NULL,
    },
    *options[]=
    {
      "-blur                apply a filter to blur the image",
      "-border geometry     surround image with a border of color",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     GRAY, OHTA, RGB, XYZ, YCbCr, YIQ, YPbPr, or YUV",
      "-comment string      annotate image with comment",
      "-compress type       RunlengthEncoded or QEncoded",
      "-contrast            enhance or reduce the image contrast",
      "-crop geometry       preferred size and location of the cropped image",
      "-density geometry    vertical and horizontal density of the image",
      "-despeckle           reduce the speckles within an image",
      "-display server      obtain image or font from this X server",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-edge                apply a filter to detect edges in the image",
      "-enhance             apply a digital filter to enhance a noisy image",
      "-equalize            perform histogram equalization to an image",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-font name           X11 font for displaying text",
      "-gamma value         level of gamma correction",
      "-geometry geometry   perferred size or location of the image",
      "-interlace type      NONE, LINE, or PLANE",
      "-label name          assign a label to an image",
      "-map filename        transform image colors to match this set of colors",
      "-matte               store matte channel if the image has one",
      "-modulate value      vary the brightness, saturation, and hue",
      "-monochrome          transform image to black and white",
      "-negate              apply color inversion to image",
      "-noise               reduce noise with a noise peak elimination filter",
      "-normalize           transform image to span the full range of colors",
      "-page geometry       size and location of the Postscript page",
      "-quality value       JPEG quality setting",
      "-roll geometry       roll an image vertically or horizontally",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sample geometry     scale image with pixel sampling",
      "-scene value         image scene number",
      "-sharpen             apply a filter to sharpen the image",
      "-shear geometry      slide one edge of the image along the X or Y axis",
      "-size geometry       width and height of image",
      "-treedepth value     depth of the color classification tree",
      "-undercolor geometry control undercolor removal and black generation",
      "-verbose             print detailed information about the image",
      (char *) NULL
    };

  (void) fprintf(stderr,"Version: %s\n\n",Version);
  (void) fprintf(stderr,"Usage: %s [options ...] input_file output_file\n",
    client_name);
  (void) fprintf(stderr,"\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example,  specify +matte to store the image without an matte channel.\n");
  (void) fprintf(stderr,
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) fprintf(stderr,
    "number.  To specify a particular image format, precede the filename\n");
  (void) fprintf(stderr,
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) fprintf(stderr,
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) fprintf(stderr,"'-' for standard input or output.\n");
  (void) fprintf(stderr,"\nThe following image formats are recognized: \n\n");
  for (p=ImageTypes; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
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
    *option;

  Image
    *image,
    *map_image,
    *next_image;

  ImageInfo
    image_info;

  int
    status,
    x;

  register int
    i;

  unsigned int
    compression,
    matte,
    scene;

  /*
    Initialize program variables.
  */
  client_name=argv[0];
  if (argc < 3)
    Usage();
  /*
    Read image and convert to MIFF format.
  */
  compression=UndefinedCompression;
  image=(Image *) NULL;
  GetImageInfo(&image_info);
  map_image=(Image *) NULL;
  matte=NotInitialized;
  scene=0;
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
          Read input image.
        */
        filename=argv[i];
        (void) strcpy(image_info.filename,filename);
        if (image != (Image *) NULL)
          Error("input image already specified",filename);
        image=ReadImage(&image_info);
        if (image == (Image *) NULL)
          exit(1);
      }
    else
      switch(*(option+1))
      {
        case 'b':
        {
          if (strncmp("blur",option+1,2) == 0)
            break;
          if (strncmp("border",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing geometry on -border",(char *) NULL);
                }
              break;
            }
          if (strncmp("bordercolor",option+1,7) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing color on -bordercolor",(char *) NULL);
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
              if (*option == '-')
                {
                  unsigned int
                    colorspace;

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
          if (strncmp("compress",option+1,4) == 0)
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
          if (strncmp("contrast",option+1,3) == 0)
            break;
          if (strncmp("crop",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -crop",(char *) NULL);
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
          if (strncmp("despeckle",option+1,3) == 0)
            break;
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
        case 'e':
        {
          if (strncmp("edge",option+1,2) == 0)
            break;
          if (strncmp("enhance",option+1,2) == 0)
            break;
          if (strncmp("equalize",option+1,2) == 0)
            break;
          Error("Unrecognized option",option);
          break;
        }
        case 'f':
        {
          if (strncmp("flip",option+1,3) == 0)
            break;
          if (strncmp("flop",option+1,3) == 0)
            break;
          if (strncmp("font",option+1,2) == 0)
            {
              image_info.font=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing font name on -font",(char *) NULL);
                  image_info.font=argv[i];
                }
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'g':
        {
          if (strncmp("gamma",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("Missing value on -gamma",(char *) NULL);
                }
              break;
            }
          if (strncmp("geometry",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -geometry",(char *) NULL);
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
        case 'm':
        {
          if (strncmp("map",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing file name on -map",(char *) NULL);
                  (void) strcpy(image_info.filename,argv[i]);
                  map_image=ReadImage(&image_info);
                  if (map_image == (Image *) NULL)
                    Error("Unable to read map image",argv[i]);
                }
              break;
            }
          if (strncmp("matte",option+1,5) == 0)
            {
              matte=(*option == '-');
              break;
            }
          if (strncmp("modulate",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("Missing value on -modulate",(char *) NULL);
                }
              break;
            }
          if (strncmp("monochrome",option+1,4) == 0)
            {
              image_info.monochrome=(*option == '-');
              break;
            }
          Error("Unrecognized option",option);
        }
        case 'n':
        {
          if (strncmp("negate",option+1,3) == 0)
            break;
          if (strncmp("noise",option+1,3) == 0)
            break;
          if (strncmp("normalize",option+1,3) == 0)
            break;
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
        case 'r':
        {
          if (strncmp("roll",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -roll",(char *) NULL);
                }
              break;
            }
          if (strncmp("rotate",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("Missing degrees on -rotate",(char *) NULL);
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 's':
        {
          if (strncmp("sample",option+1,2) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -sample",(char *) NULL);
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
                    Error("Missing scene number on -scene",(char *) NULL);
                  scene=atoi(argv[i]);
                }
              break;
            }
          if (strncmp("sharpen",option+1,3) == 0)
            break;
          if (strncmp("shear",option+1,3) == 0)
            {
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("Missing shear geometry on -shear",(char *) NULL);
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
          Error("Unrecognized option",option);
          break;
        }
        case 't':
        {
          if (*option == '-')
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                Error("Missing depth on -treedepth",(char *) NULL);
            }
          break;
        }
        case 'u':
        {
          if (strncmp("undercolor",option+1,2) == 0)
            {
              image_info.undercolor=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                    Error("Missing undercolor geometry on -undercolor",
                      (char *) NULL);
                  image_info.undercolor=argv[i];
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'v':
        {
          if (strncmp("verbose",option+1,1) == 0)
            {
              image_info.verbose=(*option == '-');
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        default:
        {
          Error("Unrecognized option",option);
          break;
        }
      }
  }
  if (image == (Image *) NULL)
    Error("Missing an image file name",(char *) NULL);
  /*
    Write images.
  */
  do
  {
    if (matte != NotInitialized)
      image->matte=matte;
    if (compression != UndefinedCompression)
      image->compression=compression;
    if (scene != 0)
      image->scene=scene;
    /*
      Transmogrify image as defined by the image processing options.
    */
    (void) strcpy(image->filename,argv[i]);
    MogrifyImage(&image_info,argc,argv,&image);
    if (map_image != (Image *) NULL)
      MapImage(image,map_image,False);
    status=WriteImage(&image_info,image);
    if (image_info.verbose)
      DescribeImage(image);
    next_image=image->next;
    if (next_image != (Image *) NULL)
      image=next_image;
  } while (next_image != (Image *) NULL);
  /*
    Free image resources.
  */
  while (image->previous != (Image *) NULL)
    image=image->previous;
  DestroyImages(image);
  if (map_image != (Image *) NULL)
    DestroyImage(map_image);
  (void) free((char *) image_info.filename);
  exit(0);
  return(False);
}
