/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             M   M   OOO   N   N  TTTTT   AAA    GGGG  EEEEE                 %
%             MM MM  O   O  NN  N    T    A   A  G      E                     %
%             M M M  O   O  N N N    T    AAAAA  G  GG  EEE                   %
%             M   M  O   O  N  NN    T    A   A  G   G  E                     %
%             M   M   OOO   N   N    T    A   A   GGGG  EEEEE                 %
%                                                                             %
%                                                                             %
%              Montage Magick Image File Format Image via X11.                %
%                                                                             %
%                                                                             %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                              July 1992                                      %
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
%  Montage creates a composite image by combining several separate
%  images.  The images are tiled on the composite image with the name of
%  the image appearing just above the individual tile.
%
%  The composite image is constructed in the following manner.  First,
%  each image specified on the command line, except for the last, is
%  scaled to fit the maximum tile size.  The maximum tile size by default
%  is 256x256.  It can be modified with the -geometry command line
%  argument or X resource.  Note that the maximum tile size need not be a
%  square.  To respect the aspect ratio of each image append ~ to the
%  geometry specification.
%
%  Next the composite image is initialized with the color specified by the
%  -background command line argument or X resource.  The width and height
%  of the composite image is determined by the maximum tile size, the
%  number of tiles per row, the tile border width and height, the image
%  border width, and the label height.  The number of tiles per row specifies
%  how many images are to appear in each row of the composite image.  The
%  default is to have an equal number of images in each row and column of the
%  composite.  This value can be specified with -tiles.  The tile border
%  width and height, and the image border width defaults to the value of the
%  X resource -borderwidth.  It can be changed with the -borderwidth or
%  -geometry command line argument or X resource.  The label height is
%  determined by the font you specify with the -font command line argument or
%  X resource.  If you do not specify a font, a font is choosen that allows
%  the name of the image to fit the maximum width of a tiled area.  The label
%  colors is determined by the -background and -foreground command line
%  argument or X resource.  Note, that if the background and foreground colors
%  are the same, labels will not appear.
%
%  Finally, each image is set onto the composite image, surrounded by its
%  border color, with its name centered just below it.  The individual images
%  are centered within the width of the tiled area.  The final argument on the
%  command line is the name assigned to the composite image.  The image is
%  written in the MIFF format and may by viewed or printed with `display'.
%
%  The Montage program command syntax is:
%
%  Usage: montage [options ...] file [ [options ...] file ...] file
%
%  Where options include:
%    -colors value         preferred number of colors in the image
%    -colorspace type      GRAY, OHTA, RGB, XYZ, YCbCr, YIQ, YPbPr, or YUV
%    -comment string       annotate image with comment
%    -compose operator     composite operator
%    -compress type        RunlengthEncoded or QEncoded
%    -crop geometry        preferred size and location of the cropped image
%    -density geometry     vertical and horizontal density of the image
%    -display server       query fonts from this X server
%    -dither               apply Floyd/Steinberg error diffusion to image
%    -frame                surround image with an ornamental border
%    -gamma value          level of gamma correction
%    -geometry geometry    preferred tile and border sizes
%    -gravity direction    which direction to gravitate towards
%    -interlace type       NONE, LINE, or PLANE
%    -label name           assign a label to an image
%    -monochrome           transform image to black and white
%    -page geometry        size and location of the Postscript page
%    -quality value        JPEG quality setting
%    -rotate degrees       apply Paeth rotation to the image
%    -scene value          image scene number
%    -shadow               add a shadow beneath a tile to simulate depth
%    -size geometry        width and height of image
%    -tile geometry        number of tiles per row and column
%    -treedepth value      depth of the color classification tree
%    -verbose              print detailed information about the image
%
%  In addition to those listed above, you can specify these standard X
%  resources as command line options:  -background, -bordercolor -borderwidth,
%  -font, -foreground, -mattecolor, or -title.
%
%  Change '-' to '+' in any option above to reverse its effect.  For
%  example, specify +compress to store the image as uncompressed.
%
%  By default, the image format of `file' is determined by its magic
%  number.  To specify a particular image format, precede the filename
%  with an image format name and a colon (i.e. ps:image) or specify the
%  image type as the filename suffix (i.e. image.ps).  Specify 'file' as
%  '-' for standard input or output.
%
%
*/

/*
  Include declarations.
*/
#include "magick.h"
#include "image.h"
#include "X.h"
#include "compress.h"

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
      "-colors value         preferred number of colors in the image",
      "-colorspace type      GRAY, OHTA, RGB, XYZ, YCbCr, YIQ, YPbPr, or YUV",
      "-comment string       annotate image with comment",
      "-compose operator     composite operator",
      "-compress type        RunlengthEncoded or QEncoded",
      "-crop geometry        preferred size and location of the cropped image",
      "-density geometry     vertical and horizontal density of the image",
      "-display server       query font from this X server",
      "-dither               apply Floyd/Steinberg error diffusion to image",
      "-frame                surround image with an ornamental border",
      "-gamma value          level of gamma correction",
      "-geometry geometry    preferred tile and border sizes",
      "-gravity direction    which direction to gravitate towards",
      "-interlace type       NONE, LINE, or PLANE",
      "-label name           assign a label to an image",
      "-monochrome           transform image to black and white",
      "-page geometry        size and location of the Postscript page",
      "-quality value        JPEG quality setting",
      "-rotate degrees       apply Paeth rotation to the image",
      "-scene value          image scene number",
      "-shadow               add a shadow beneath a tile to simulate depth",
      "-size geometry        width and height of image",
      "-tile geometry        number of tiles per row and column",
      "-treedepth value      depth of the color classification tree",
      "-verbose              print detailed information about the image",
      (char *) NULL
    };

  (void) fprintf(stderr,"Version: %s\n\n",Version);
  (void) fprintf(stderr,
    "Usage: %s [-options ...] file [ [-options ...] file ...] file\n",
    client_name);
  (void) fprintf(stderr,"\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) fprintf(stderr,
    "resources as command line options:  -background, -bordercolor,\n");
  (void) fprintf(stderr,
    "-borderwidth, -font, -foreground, -mattecolor, or -title\n");
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example, specify +compress to store the image as uncompressed.\n");
  (void) fprintf(stderr,
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) fprintf(stderr,
    "number.  To specify a particular image format, precede the filename\n");
  (void) fprintf(stderr,
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) fprintf(stderr,
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
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
  char
    *crop_geometry,
    *option,
    *server_name;

  Display
    *display;

  Image
    *image,
    **images,
    *montage_image;

  ImageInfo
    image_info;

  int
    i,
    x;

  unsigned int
    compression,
    display_image,
    first_scene,
    last_scene,
    maximum_images,
    scene;

  XMontageInfo
    montage_info;

  XrmDatabase
    resource_database;

  XResourceInfo
    resource_info;

  /*
    Display usage profile if there are no command line arguments.
  */
  client_name=(*argv);
  if (argc < 3)
    Usage();
  /*
    Set defaults.
  */
  XGetMontageInfo(&montage_info);
  GetImageInfo(&image_info);
  compression=UndefinedCompression;
  crop_geometry=(char *) NULL;
  display=(Display *) NULL;
  display_image=True;
  first_scene=0;
  last_scene=0;
  resource_database=(XrmDatabase) NULL;
  resource_info.border_color=(char *) NULL;
  resource_info.border_width=0;
  resource_info.colorspace=RGBColorspace;
  resource_info.dither=False;
  resource_info.image_geometry=(char *) NULL;
  resource_info.gravity=CenterGravity;
  resource_info.monochrome=False;
  resource_info.number_colors=0;
  resource_info.server_name=(char *) NULL;
  resource_info.title=(char *) NULL;
  resource_info.tree_depth=0;
  scene=0;
  server_name=(char *) NULL;
  maximum_images=2048;
  images=(Image **) malloc(maximum_images*sizeof(Image *));
  if (images == (Image **) NULL)
    Error("Unable to montage images","Memory allocation failed");
  /*
    Check for server name specified on the command line.
  */
  ExpandFilenames(&argc,&argv);
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if (((int) strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      if (strncmp("display",option+1,3) == 0)
        {
          /*
            User specified server name.
          */
          display_image=(*option == '-');
          if (display_image)
            {
              i++;
              if (i == argc)
                Error("Missing server name on -display",(char *) NULL);
              server_name=argv[i];
            }
          break;
        }
  }
  if (display_image)
    {
      char
        *resource_value;

      XrmDatabase
        server_database;

      /*
        Open X server connection.
      */
      display=XOpenDisplay(server_name);
      if (display == (Display *) NULL)
        Error("Unable to connect to X server",XDisplayName(server_name));
      /*
        Set our forgiving error handler.
      */
      XSetErrorHandler(XError);
      /*
        Initialize resource database.
      */
      XrmInitialize();
      XGetDefault(display,client_name,"dummy");
      resource_database=XrmGetDatabase(display);
      resource_value=XResourceManagerString(display);
      if (resource_value == (char *) NULL)
        resource_value="";
      server_database=XrmGetStringDatabase(resource_value);
      XrmMergeDatabases(server_database,&resource_database);
      /*
        Get user defaults from X resource database.
      */
      XGetResourceInfo(resource_database,client_name,&resource_info);
      resource_info.background_color=XGetResourceInstance(resource_database,
        client_name,"background",DefaultTileBackground);
      resource_value=XGetResourceClass(resource_database,client_name,
        "borderWidth",DefaultTileBorderwidth);
      resource_info.border_width=atoi(resource_value);
      resource_value=XGetResourceClass(resource_database,client_name,
        "compression","RunlengthEncoded");
      if (Latin1Compare("qencoded",resource_value) == 0)
        compression=QEncodedCompression;
      else
        compression=RunlengthEncodedCompression;
      image_info.density=XGetResourceClass(resource_database,client_name,
        "density",(char *) NULL);
      resource_info.foreground_color=XGetResourceInstance(resource_database,
        client_name,"foreground",DefaultTileForeground);
      resource_value=
        XGetResourceClass(resource_database,client_name,"frame","True");
      montage_info.frame=IsTrue(resource_value);
      resource_value=
        XGetResourceClass(resource_database,client_name,"interlace","none");
      image_info.interlace=UndefinedInterlace;
      if (Latin1Compare("none",resource_value) == 0)
        image_info.interlace=NoneInterlace;
      if (Latin1Compare("line",resource_value) == 0)
        image_info.interlace=LineInterlace;
      if (Latin1Compare("plane",resource_value) == 0)
        image_info.interlace=PlaneInterlace;
      if (image_info.interlace == UndefinedInterlace)
        Warning("Unrecognized interlace type",resource_value);
      resource_info.image_geometry=XGetResourceInstance(resource_database,
        client_name,"imageGeometry",DefaultTileGeometry);
      resource_info.matte_color=XGetResourceInstance(resource_database,
        client_name,"mattecolor",DefaultTileMatte);
      image_info.page=XGetResourceClass(resource_database,client_name,
        "pageGeometry",(char *) NULL);
      resource_value=
        XGetResourceClass(resource_database,client_name,"rotate","0");
      resource_value=
        XGetResourceClass(resource_database,client_name,"quality","85");
      image_info.quality=atoi(resource_value);
      resource_value=
        XGetResourceClass(resource_database,client_name,"shadow","True");
      montage_info.shadow=IsTrue(resource_value);
      montage_info.tile=XGetResourceClass(resource_database,client_name,"tile",
        montage_info.tile);
      resource_value=
        XGetResourceClass(resource_database,client_name,"verbose","False");
      image_info.verbose=IsTrue(resource_value);
    }
  /*
    Parse command line.
  */
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if (((int) strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'b':
        {
          if (strncmp("background",option+1,5) == 0)
            {
              resource_info.background_color=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing color on -background",(char *) NULL);
                  resource_info.background_color=argv[i];
                }
              break;
            }
          if (strncmp("bordercolor",option+1,7) == 0)
            {
              resource_info.border_color=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing color on -bordercolor",(char *) NULL);
                  resource_info.border_color=argv[i];
                }
              break;
            }
          if (strncmp("borderwidth",option+1,7) == 0)
            {
              resource_info.border_width=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing width on -borderwidth",(char *) NULL);
                  resource_info.border_width=atoi(argv[i]);
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
              resource_info.number_colors=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing colors on -colors",(char *) NULL);
                  resource_info.number_colors=atoi(argv[i]);
                }
              break;
            }
          if (strncmp("colorspace",option+1,7) == 0)
            {
              resource_info.colorspace=RGBColorspace;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing type on -colorspace",(char *) NULL);
                  option=argv[i];
                  resource_info.colorspace=UndefinedColorspace;
                  if (Latin1Compare("gray",option) == 0)
                    {
                      resource_info.colorspace=GRAYColorspace;
                      resource_info.number_colors=256;
                      resource_info.tree_depth=8;
                    }
                  if (Latin1Compare("ohta",option) == 0)
                    resource_info.colorspace=OHTAColorspace;
                  if (Latin1Compare("rgb",option) == 0)
                    resource_info.colorspace=RGBColorspace;
                  if (Latin1Compare("xyz",option) == 0)
                    resource_info.colorspace=XYZColorspace;
                  if (Latin1Compare("ycbcr",option) == 0)
                    resource_info.colorspace=YCbCrColorspace;
                  if (Latin1Compare("yiq",option) == 0)
                    resource_info.colorspace=YIQColorspace;
                  if (Latin1Compare("ypbpr",option) == 0)
                    resource_info.colorspace=YPbPrColorspace;
                  if (Latin1Compare("yuv",option) == 0)
                    resource_info.colorspace=YUVColorspace;
                  if (resource_info.colorspace == UndefinedColorspace)
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
          if (strncmp("compress",option+1,5) == 0)
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
          if (strncmp("compose",option+1,5) == 0)
            {
              montage_info.compose=ReplaceCompositeOp;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing type on -compose",(char *) NULL);
                  option=argv[i];
                  montage_info.compose=UndefinedCompositeOp;
                  if (Latin1Compare("over",option) == 0)
                    montage_info.compose=OverCompositeOp;
                  if (Latin1Compare("in",option) == 0)
                    montage_info.compose=InCompositeOp;
                  if (Latin1Compare("out",option) == 0)
                    montage_info.compose=OutCompositeOp;
                  if (Latin1Compare("atop",option) == 0)
                    montage_info.compose=AtopCompositeOp;
                  if (Latin1Compare("xor",option) == 0)
                    montage_info.compose=XorCompositeOp;
                  if (Latin1Compare("plus",option) == 0)
                    montage_info.compose=PlusCompositeOp;
                  if (Latin1Compare("minus",option) == 0)
                    montage_info.compose=MinusCompositeOp;
                  if (Latin1Compare("add",option) == 0)
                    montage_info.compose=AddCompositeOp;
                  if (Latin1Compare("subtract",option) == 0)
                    montage_info.compose=SubtractCompositeOp;
                  if (Latin1Compare("difference",option) == 0)
                    montage_info.compose=DifferenceCompositeOp;
                  if (Latin1Compare("replace",option) == 0)
                    montage_info.compose=ReplaceCompositeOp;
                  if (montage_info.compose == UndefinedCompositeOp)
                    Error("Invalid compose type on -compose",option);
                }
              break;
            }
          if (strncmp("crop",option+1,2) == 0)
            {
              crop_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -crop",(char *) NULL);
                  crop_geometry=argv[i];
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
              resource_info.server_name=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing server name on -display",(char *) NULL);
                  resource_info.server_name=argv[i];
                }
              break;
            }
          if (strncmp("dither",option+1,3) == 0)
            {
              resource_info.dither=(*option == '-');
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'f':
        {
          if (strncmp("font",option+1,3) == 0)
            {
              resource_info.font=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing font name on -font",(char *) NULL);
                  resource_info.font=argv[i];
                }
              break;
            }
          if (strncmp("foreground",option+1,3) == 0)
            {
              resource_info.foreground_color=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing foreground on -foreground",(char *) NULL);
                  resource_info.foreground_color=argv[i];
                }
              break;
            }
          if (strncmp("frame",option+1,2) == 0)
            {
              montage_info.frame=(*option == '-');
              break;
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
              resource_info.image_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -geometry",(char *) NULL);
                  resource_info.image_geometry=argv[i];
                }
              break;
            }
          if (strncmp("gravity",option+1,2) == 0)
            {
              resource_info.gravity=CenterGravity;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing type on -gravity",(char *) NULL);
                  option=argv[i];
                  resource_info.gravity=(-1);
                  if (Latin1Compare("Forget",option) == 0)
                    resource_info.gravity=ForgetGravity;
                  if (Latin1Compare("NorthWest",option) == 0)
                    resource_info.gravity=NorthWestGravity;
                  if (Latin1Compare("North",option) == 0)
                    resource_info.gravity=NorthGravity;
                  if (Latin1Compare("NorthEast",option) == 0)
                    resource_info.gravity=NorthEastGravity;
                  if (Latin1Compare("West",option) == 0)
                    resource_info.gravity=WestGravity;
                  if (Latin1Compare("Center",option) == 0)
                    resource_info.gravity=CenterGravity;
                  if (Latin1Compare("East",option) == 0)
                    resource_info.gravity=EastGravity;
                  if (Latin1Compare("SouthWest",option) == 0)
                    resource_info.gravity=SouthWestGravity;
                  if (Latin1Compare("South",option) == 0)
                    resource_info.gravity=SouthGravity;
                  if (Latin1Compare("SouthEast",option) == 0)
                    resource_info.gravity=SouthEastGravity;
                  if (Latin1Compare("Static",option) == 0)
                    resource_info.gravity=StaticGravity;
                  if (resource_info.gravity == (-1))
                    Error("Invalid gravity type on -gravity",option);
                }
              break;
            }
          Error("Unrecognized option",option);
          break;
        }
        case 'h':
        {
          if (strncmp("help",option+1,2) == 0)
            {
              Usage();
              break;
            }
          Error("Unrecognized option",option);
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
          if (strncmp("mattecolor",option+1,6) == 0)
            {
              resource_info.matte_color=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing color on -matte",(char *) NULL);
                  resource_info.matte_color=argv[i];
                }
              break;
            }
          if (strncmp("monochrome",option+1,2) == 0)
            {
              resource_info.monochrome=(*option == '-');
              if (resource_info.monochrome)
                {
                  resource_info.number_colors=2;
                  resource_info.tree_depth=8;
                  resource_info.colorspace=GRAYColorspace;
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
        case 'r':
        {
          if (*option == '-')
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%f",(float *) &x))
                Error("Missing degrees on -rotate",(char *) NULL);
            }
          break;
        }
        case 's':
        {
          if (strncmp("scene",option+1,3) == 0)
            {
              first_scene=0;
              last_scene=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing scene number on -scene",(char *) NULL);
                  first_scene=atoi(argv[i]);
                  last_scene=first_scene;
                  (void) sscanf(argv[i],"%d-%d",&first_scene,&last_scene);
                }
              break;
            }
          if (strncmp("shadow",option+1,2) == 0)
            {
              montage_info.shadow=(*option == '-');
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
          if (strncmp("tile",option+1,3) == 0)
            {
              montage_info.tile=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing geometry on -tile",(char *) NULL);
                  montage_info.tile=argv[i];
                }
              break;
            }
          if (strncmp("title",option+1,3) == 0)
            {
              resource_info.title=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("Missing title on -title",(char *) NULL);
                  resource_info.title=argv[i];
                }
              break;
            }
          if (strncmp("treedepth",option+1,2) == 0)
            {
              resource_info.tree_depth=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("Missing depth on -treedepth",(char *) NULL);
                  resource_info.tree_depth=atoi(argv[i]);
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
    else
      for (scene=first_scene; scene <= last_scene ; scene++)
      {
        /*
          Option is a file name: begin by reading image from specified file.
        */
        (void) strcpy(image_info.filename,argv[i]);
        if (first_scene != last_scene)
          {
            char
              filename[MaxTextLength];

            /*
              Form filename for multi-part images.
            */
            (void) sprintf(filename,image_info.filename,scene);
            if (strcmp(filename,image_info.filename) == 0)
              (void) sprintf(filename,"%s.%u",image_info.filename,scene);
            (void) strcpy(image_info.filename,filename);
          }
        image_info.server_name=resource_info.server_name;
        image_info.font=resource_info.font;
        image_info.dither=resource_info.dither;
        image_info.monochrome=resource_info.monochrome;
        image=ReadImage(&image_info);
        if (image == (Image *) NULL)
          if (*option == '-')
            break;
          else
            continue;
        do
        {
          if (image->scene == 0)
            image->scene=montage_info.number_tiles;
          LabelImage(image,"%f");
          /*
            Transmogrify image as defined by the image processing options.
          */
          MogrifyImage(&image_info,i,argv,&image);
          TransformImage(&image,crop_geometry,resource_info.image_geometry);
          if (image_info.verbose)
            DescribeImage(image);
          /*
            Pack image data to conserve memory (memory <=> speed).
          */
          (void) RunlengthEncodeImage(image);
          (void) free((char *) image->pixels);
          image->pixels=(RunlengthPacket *) NULL;
          if (montage_info.number_tiles == maximum_images)
            {
              /*
                Increase size of images array.
              */
              maximum_images<<=1;
              images=(Image **)
                realloc((char *) images,maximum_images*sizeof(Image *));
              if (images == (Image **) NULL)
                Error("Unable to montage images","Memory allocation failed");
            }
          images[montage_info.number_tiles++]=image;
          image=image->next;
        } while (image != (Image *) NULL);
      }
    }
  if (montage_info.number_tiles == 0)
    Error("Missing an image file name",(char *) NULL);
  /*
    Create composite image.
  */
  montage_image=
    XMontageImage(display,&resource_info,&montage_info,argv[argc-1],images);
  if (montage_image != (Image *) NULL)
    {
      Image
        *next_image;

      /*
        Write image.
      */
      image=montage_image;
      do
      {
        if (compression != UndefinedCompression)
          image->compression=compression;
        (void) strcpy(image->filename,argv[argc-1]);
        (void) WriteImage(&image_info,image);
        if (image_info.verbose)
          DescribeImage(image);
        next_image=image->next;
        image=next_image;
      } while (image != (Image *) NULL);
      DestroyImage(montage_image);
    }
  (void) free((char *) images);
  (void) free((char *) image_info.filename);
  if (display != (Display *) NULL)
    XCloseDisplay(display);
  exit(0);
  return(False);
}
