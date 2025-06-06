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
%          Montage Machine Independent File Format Image via X11.             %
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
%  Montage creates a composite image by combining several separate
%  images.  The images are tiled on the composite image with the name of
%  the image appearing just above the individual tile.
%
%  The composite image is constructed in the following manner.  First,
%  each image specified on the command line, except for the last, is
%  scaled to fit the maximum tile size.  The maximum tile size by default
%  is 256x256.  It can be modified with the -geometry command line
%  argument or X resource.  Note that the maximum tile size need not be a
%  square.  The original aspect ratio of each image is maintainted unless
%  +aspect_ratio is specfified.
%
%  Next the composite image is initialized with the color specified by the
%  -background command line argument or X resource.  The width and height
%  of the composite image is determined by the maximum tile size, the
%  number of tiles per row, the tile border width and height, the image
%  border width, and the label height.  The number of tiles per row specifies
%  how many images are to appear in each row of the composite image.  The
%  default is to have an equal number of images in each row and column of the
%  composite.  This value can be specified with -tiles_per_row.  The tile
%  border width and height, and the image border width defaults to the value
%  of the X resource -borderwidth.  It can be changed with the -borderwidth or
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
%    -aspect_ratio         respect aspect ratio of the image
%    -clip geometry        preferred size and location of the clipped image
%    -colors value         preferred number of colors in the image
%    -colorspace type      GRAY, RGB, XYZ, YIQ, or YUV
%    -compose operator     composite operator
%    -compress type        RunlengthEncoded or QEncoded
%    -density geometry     vertical and horizonal density of the image
%    -display server       query fonts from this X server
%    -dither               apply Floyd/Steinberg error diffusion to image
%    -gamma value          level of gamma correction
%    -geometry geometry    preferred tile and border sizes
%    -gravity direction    which direction to gravitate towards
%    -monochrome           transform image to black and white
%    -rotate degrees       apply Paeth rotation to the image
%    -tiles_per_row value  number of image tiles per row
%    -treedepth value      depth of the color classification tree
%    -verbose              print detailed information about the image
%
%  In addition to those listed above, you can specify these standard X
%  resources as command line options:  -background, -bordercolor -borderwidth,
%  -font, -foreground, or -title.
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
%   M o n t a g e I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function MontageImage creates a composite image by combining several
%  separate images.
%
%  The format of the MontageImage routine is:
%
%      MontageImage(display,resource_info,compose,tiles_per_row,image,
%        number_tiles)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o compose: Specifies an image composite operator.
%
%    o tiles_per_row: Specifies the number of arguments.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%    o number_tiles: Specifies the number of tiles to tile.
%
%
*/
static int LinearCompare(x,y)
void
  *x,
  *y;
{
  Image
    **image_1,
    **image_2;

  image_1=(Image **) x;
  image_2=(Image **) y;
  return((int) (*image_1)->scene-(int) (*image_2)->scene);
}

static Image *MontageImage(display,resource_info,compose,tiles_per_row,images,
  number_tiles)
Display
  *display;

XResourceInfo
  *resource_info;

unsigned int
  compose,
  tiles_per_row;

Image
  **images;

unsigned int
  number_tiles;
{
  ColorPacket
    border_color;

  Image
    *image,
    *montage_image;

  int
    tile_border_height,
    tile_border_width,
    x,
    x_offset,
    y,
    y_offset;

  register char
    *q;

  register int
    i;

  register RunlengthPacket
    *p;

  unsigned int
    border_width,
    count,
    status,
    tile,
    tile_height,
    tile_width;

  XAnnotateInfo
    annotate_info;

  XColor
    background_color;

  XWindowInfo
    image_window;

  /*
    Determine tile sizes.
  */
  border_width=resource_info->border_width;
  tile_border_width=resource_info->border_width;
  tile_border_height=resource_info->border_width;
  tile_width=256;
  tile_height=256;
  if (resource_info->image_geometry != (char *) NULL)
    {
      XParseGeometry(resource_info->image_geometry,&tile_border_width,
        &tile_border_height,&tile_width,&tile_height);
      if (tile_border_width < 0)
        tile_border_width=0;
      if (tile_border_height < 0)
        tile_border_height=0;
    }
  if (tiles_per_row == 0)
    {
      /*
        Compute tiles per row.
      */
      tiles_per_row=1;
      while ((tiles_per_row*tiles_per_row) < number_tiles)
        tiles_per_row++;
    }
  /*
    Initialize tile colors.
  */
  background_color.red=0;
  background_color.green=0;
  background_color.blue=0;
  border_color.red=0;
  border_color.green=0;
  border_color.blue=0;
  XGetAnnotateInfo(&annotate_info);
  if (display)
    {
      char
        text[2048];

      XFontStruct
        *font_info;

      XPixelInfo
        pixel_info;

      XStandardColormap
        map_info;

      XVisualInfo
        *visual_info;

      /*
        Initialize visual info.
      */
      visual_info=XBestVisualInfo(display,"default",(char *) NULL,
        (XStandardColormap *) NULL);
      if (visual_info == (XVisualInfo *) NULL)
         Error("unable to get visual",resource_info->visual_type);
      /*
        Initialize font info.
      */
      tile=0;
      for (i=1; i < number_tiles; i++)
        if ((int) strlen(images[i]->filename) >
            (int) strlen(images[tile]->filename))
          tile=i;
      (void) strcpy(text,images[tile]->filename);
      font_info=XBestFont(display,resource_info,text,tile_width);
      if (font_info == (XFontStruct *) NULL)
        Error("unable to load font",resource_info->font);
      annotate_info.text=(char *) malloc(2048*sizeof(char));
      if (annotate_info.text == (char *) NULL)
        Error("unable to montage images","memory allocation failed");
      annotate_info.font_info=font_info;
      annotate_info.height=font_info->ascent+font_info->descent;
      /*
        Determine background, border, and foreground colors.
      */
      map_info.colormap=XDefaultColormap(display,visual_info->screen);
      XGetPixelInfo(display,visual_info,&map_info,resource_info,(Image *) NULL,
        &pixel_info);
      background_color=pixel_info.background_color;
      border_color.red=pixel_info.border_color.red >> 8;
      border_color.green=pixel_info.border_color.green >> 8;
      border_color.blue=pixel_info.border_color.blue >> 8;
      /*
        Window superclass.
      */
      image_window.id=XRootWindow(display,visual_info->screen);
      image_window.screen=visual_info->screen;
      image_window.depth=visual_info->depth;
      image_window.visual_info=visual_info;
      image_window.pixel_info=(&pixel_info);
      image_window.font_info=font_info;
    }
  /*
    Allocate image structure.
  */
  montage_image=AllocateImage("MIFF");
  if (montage_image == (Image *) NULL)
    Error("memory allocation error",(char *) NULL);
  /*
    Initialize Image structure.
  */
  montage_image->comments=(char *) malloc(2048*sizeof(char));
  montage_image->columns=
    (tile_width+(tile_border_width+border_width)*2)*tiles_per_row;
  montage_image->rows=
    (tile_height+(tile_border_height+border_width)*2+annotate_info.height+4)*
    (number_tiles/tiles_per_row+((number_tiles % tiles_per_row) != 0))+
    ((tile_border_height+border_width) >> 1);
  if (resource_info->title != (char *) NULL)
    montage_image->rows+=((annotate_info.height+4) << 1)+
      (tile_border_height << 1);
  montage_image->montage=(char *) malloc(2048*sizeof(char));
  count=0;
  for (tile=0; tile < number_tiles; tile++)
    count+=strlen(images[tile]->filename)+1;
  montage_image->directory=(char *) malloc(count*sizeof(char));
  montage_image->packets=montage_image->columns*montage_image->rows;
  montage_image->pixels=(RunlengthPacket *)
    malloc((unsigned int) montage_image->packets*sizeof(RunlengthPacket));
  if ((montage_image->comments == (char *) NULL) ||
      (montage_image->montage == (char *) NULL) ||
      (montage_image->directory == (char *) NULL) ||
      (montage_image->pixels == (RunlengthPacket *) NULL))
    Error("memory allocation error",(char *) NULL);
  (void) sprintf(montage_image->comments,"\n  ImageMagick image montage.\n");
  /*
    Set montage geometry.
  */
  x_offset=0;
  y_offset=((tile_border_height+border_width) >> 1);
  if (resource_info->title != (char *) NULL)
    y_offset+=((annotate_info.height+4) << 1)+(tile_border_height << 1);
  *montage_image->directory='\0';
  (void) sprintf(montage_image->montage,"%dx%d%+d%+d",
    tile_width+(tile_border_width+border_width)*2,
    (tile_height+(tile_border_height+border_width)*2+annotate_info.height+4),
    x_offset,y_offset);
  /*
    Initialize montage image to background color.
  */
  p=montage_image->pixels;
  for (i=0; i < montage_image->packets; i++)
  {
    p->red=background_color.red >> 8;
    p->green=background_color.green >> 8;
    p->blue=background_color.blue >> 8;
    p->index=0;
    p->length=0;
    p++;
  }
  /*
    Sort images by increasing tile number.
  */
  (void) qsort((void *) images,(int) number_tiles,sizeof(Image *),
    LinearCompare);
  if (display && (resource_info->title != (char *) NULL))
    {
      /*
        Copy title to the composite image.
      */
      (void) strcpy(annotate_info.text,resource_info->title);
      annotate_info.width=XTextWidth(image_window.font_info,annotate_info.text,
        strlen(annotate_info.text));
      if ((annotate_info.width << 1) > montage_image->columns)
        {
          /*
            Label is too wide-- shorten.
          */
          q=annotate_info.text+strlen(annotate_info.text);
          do
          {
            *--q='\0';
            if ((int) strlen(annotate_info.text) > 2)
              (void) strcpy(q-2,"...");
            annotate_info.width=XTextWidth(image_window.font_info,
              annotate_info.text,strlen(annotate_info.text));
          } while ((annotate_info.width << 1) > montage_image->columns);
        }
      (void) sprintf(annotate_info.geometry,"%ux%u%+d%+d\0",
        annotate_info.width << 1,annotate_info.height << 1,tile_border_width+
        (int) (montage_image->columns >> 1)-(int) annotate_info.width,
        tile_border_height+4);
      (void) XAnnotateImage(display,&image_window,&annotate_info,True,
        montage_image);
    }
  /*
    Copy tile images to the composite image.
  */
  x_offset=tile_border_width;
  y_offset=tile_border_height;
  if (resource_info->title != (char *) NULL)
    y_offset+=((annotate_info.height+4) << 1)+(tile_border_height << 1);
  *montage_image->directory='\0';
  for (tile=0; tile < number_tiles; tile++)
  {
    /*
      Copy this tile to the composite image.
    */
    image=images[tile];
    (void) strcat(montage_image->directory,image->filename);
    (void) strcat(montage_image->directory,"\n");
    status=UnpackImage(image);
    if (status == False)
      Error("unable to unpack image",(char *) NULL);
    if (border_width != 0)
      {
        Image
          *bordered_image;

        /*
          Put a border around the image.
        */
        bordered_image=
          BorderImage(image,border_width,border_width,border_color);
        if (bordered_image != (Image *) NULL)
          {
            DestroyImage(image);
            image=bordered_image;
          }
      }
    /*
      Gravitate image as specified by the tile gravity.
    */
    switch (resource_info->gravity)
    {
      case NorthWestGravity:
      {
        x=0;
        y=0;
        break;
      }
      case NorthGravity:
      {
        x=((tile_width+border_width*2)-image->columns)/2;
        y=0;
        break;
      }
      case NorthEastGravity:
      {
        x=(tile_width+border_width*2)-image->columns;
        y=0;
        break;
      }
      case WestGravity:
      {
        x=0;
        y=((tile_height+border_width*2)-image->rows)/2;
        break;
      }
      case ForgetGravity:
      case StaticGravity:
      case CenterGravity:
      {
        x=((tile_width+border_width*2)-image->columns)/2;
        y=((tile_height+border_width*2)-image->rows)/2;
        break;
      }
      case EastGravity:
      {
        x=(tile_width+border_width*2)-image->columns;
        y=((tile_height+border_width*2)-image->rows)/2;
        break;
      }
      case SouthWestGravity:
      {
        x=0;
        y=(tile_height+border_width*2)-image->rows;
        break;
      }
      case SouthGravity:
      {
        x=((tile_width+border_width*2)-image->columns)/2;
        y=(tile_height+border_width*2)-image->rows;
        break;
      }
      case SouthEastGravity:
      {
        x=(tile_width+border_width*2)-image->columns;
        y=(tile_height+border_width*2)-image->rows;
        break;
      }
    }
    /*
      Composite background image with tile image.
    */
    CompositeImage(montage_image,compose,image,x_offset+x,y_offset+y);
    if (display)
      {
        /*
          Copy tile label to the composite image.
        */
        (void) strcpy(annotate_info.text,image->label);
        annotate_info.width=XTextWidth(image_window.font_info,
          annotate_info.text,strlen(annotate_info.text));
        if (annotate_info.width > (tile_width+border_width*2))
          {
            /*
              Label is too wide-- shorten.
            */
            q=annotate_info.text+strlen(annotate_info.text);
            do
            {
              *--q='\0';
              if ((int) strlen(annotate_info.text) > 2)
                (void) strcpy(q-2,"...");
              annotate_info.width=XTextWidth(image_window.font_info,
                annotate_info.text,strlen(annotate_info.text));
            } while (annotate_info.width > (tile_width+border_width*2));
          }
        (void) sprintf(annotate_info.geometry,"%ux%u%+d%+d\0",
          annotate_info.width,annotate_info.height,x_offset+
          ((tile_width+border_width*2) >> 1)-(annotate_info.width >> 1),
          y_offset+y+(int) image->rows+tile_border_height+2);
        (void) XAnnotateImage(display,&image_window,&annotate_info,True,
          montage_image);
      }
    DestroyImage(image);
    if (((tile+1) % tiles_per_row) != 0)
      x_offset+=tile_width+(tile_border_width+border_width)*2;
    else
      {
        x_offset=tile_border_width;
        y_offset+=tile_height+(tile_border_height+border_width)*2+
          (annotate_info.height+4);
      }
  }
  (void) free((char *) annotate_info.text);
  if (display)
    {
      /*
        Free X resources.
      */
      XFreeFont(display,image_window.font_info);
      XFree((void *) image_window.visual_info);
    }
  return(montage_image);
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
      "-aspect_ratio         respect aspect ratio of the image",
      "-clip geometry        preferred size and location of the clipped image",
      "-colors value         preferred number of colors in the image",
      "-colorspace type      GRAY, RGB, XYZ, YIQ, or YUV",
      "-compose operator     composite operator",
      "-compress type        RunlengthEncoded or QEncoded",
      "-density geometry     vertical and horizonal density of the image",
      "-display server       query font from this X server",
      "-dither               apply Floyd/Steinberg error diffusion to image",
      "-gamma value          level of gamma correction",
      "-geometry geometry    preferred tile and border sizes",
      "-gravity direction    which direction to gravitate towards",
      "-monochrome           transform image to black and white",
      "-rotate degrees       apply Paeth rotation to the image",
      "-tiles_per_row value  number of image tiles per row",
      "-treedepth value      depth of the color classification tree",
      "-verbose              print detailed information about the image",
      (char *) NULL
    };
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
    "-borderwidth, -font, -foreground, or -title\n");
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.  For\n");
  (void) fprintf(stderr,
    "example, specify +aspect_ratio to ignore the aspect ratio of an image.\n");
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
    *clip_geometry,
    *option,
    *server_name,
    *write_filename;

  Display
    *display;

  double
    gamma;

  Image
    **images,
    *montage_image;

  int
    degrees,
    i,
    x;

  time_t
    start_time;

  unsigned int
    aspect_ratio,
    compose,
    compression,
    display_image,
    image_number,
    maximum_images,
    tiles_per_row,
    verbose;

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
  GetAlienInfo(&alien_info);
  aspect_ratio=True;
  clip_geometry=(char *) NULL;
  compose=ReplaceCompositeOp;
  compression=UndefinedCompression;
  degrees=0;
  display=(Display *) NULL;
  display_image=True;
  gamma=0.0;
  resource_database=(XrmDatabase) NULL;
  resource_info.border_width=0;
  resource_info.colorspace=RGBColorspace;
  resource_info.dither=False;
  resource_info.image_geometry=(char *) NULL;
  resource_info.monochrome=False;
  resource_info.number_colors=0;
  resource_info.tree_depth=0;
  server_name=(char *) NULL;
  tiles_per_row=0;
  verbose=False;
  maximum_images=2048;
  images=(Image **) malloc(maximum_images*sizeof(Image *));
  if (images == (Image **) NULL)
    Error("unable to montage images","memory allocation failed");
  /*
    Check for server name specified on the command line.
  */
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
                Error("missing server name on -display",(char *) NULL);
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
        Error("unable to connect to X server",XDisplayName(server_name));
      /*
        Set our forgiving error handler.
      */
      XSetErrorHandler(XError);
      /*
        Initialize resource database.
      */
      XrmInitialize();
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
      resource_value=
        XGetResource(resource_database,client_name,"aspect_ratio","True");
      aspect_ratio=IsTrue(resource_value);
      clip_geometry=XGetResource(resource_database,client_name,"clipGeometry",
        (char *) NULL);
      resource_value=XGetResource(resource_database,client_name,
        "compression","RunlengthEncoded");
      if (Latin1Compare("qencoded",resource_value) == 0)
        compression=QEncodedCompression;
      else
        compression=RunlengthEncodedCompression;
      resource_value=XGetResource(resource_database,client_name,"gamma",
        "0.0");
      gamma=atof(resource_value);
      resource_value=XGetResource(resource_database,client_name,"rotate",
        "0");
      degrees=atoi(resource_value);
      resource_value=XGetResource(resource_database,client_name,
        "tiles_per_row","0");
      tiles_per_row=atoi(resource_value);
      resource_value=
        XGetResource(resource_database,client_name,"verbose","False");
      verbose=IsTrue(resource_value);
    }
  /*
    Composite image is the last item on the command line.
  */
  write_filename=argv[argc-1];
  if (access(write_filename,0) == 0)
    {
      char
        answer[2];

      (void) fprintf(stderr,"Overwrite %s? ",write_filename);
      (void) gets(answer);
      if (!((*answer == 'y') || (*answer == 'Y')))
        exit(1);
    }
  /*
    Parse command line.
  */
  image_number=0;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if (((int) strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'a':
        {
          aspect_ratio=(*option == '-');
          break;
        }
        case 'b':
        {
          if (strncmp("background",option+1,5) == 0)
            {
              resource_info.background_color=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing color on -background",(char *) NULL);
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
                    Error("missing color on -bordercolor",(char *) NULL);
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
                    Error("missing width on -borderwidth",(char *) NULL);
                  resource_info.border_width=atoi(argv[i]);
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
          if (strncmp("colors",option+1,7) == 0)
            {
              resource_info.number_colors=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("missing colors on -colors",(char *) NULL);
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
                    Error("missing type on -colorspace",(char *) NULL);
                  option=argv[i];
                  resource_info.colorspace=UndefinedColorspace;
                  if (Latin1Compare("gray",option) == 0)
                    resource_info.colorspace=GRAYColorspace;
                  if (Latin1Compare("rgb",option) == 0)
                    resource_info.colorspace=RGBColorspace;
                  if (Latin1Compare("yiq",option) == 0)
                    resource_info.colorspace=YIQColorspace;
                  if (Latin1Compare("yuv",option) == 0)
                    resource_info.colorspace=YUVColorspace;
                  if (Latin1Compare("xyz",option) == 0)
                    resource_info.colorspace=XYZColorspace;
                  if (resource_info.colorspace == UndefinedColorspace)
                    Error("invalid colorspace type on -colorspace",option);
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
              server_name=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing server name on -display",(char *) NULL);
                  server_name=argv[i];
                }
              alien_info.server_name=server_name;
              break;
            }
          if (strncmp("dither",option+1,3) == 0)
            {
              resource_info.dither=(*option == '-');
              break;
            }
          Error("unrecognized option",option);
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
                    Error("missing font name on -font",(char *) NULL);
                  resource_info.font=argv[i];
                }
              alien_info.font=resource_info.font;
              break;
            }
          if (strncmp("foreground",option+1,3) == 0)
            {
              resource_info.foreground_color=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing foreground on -foreground",(char *) NULL);
                  resource_info.foreground_color=argv[i];
                }
              break;
            }
          Error("unrecognized option",option);
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
              resource_info.image_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing geometry on -geometry",(char *) NULL);
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
                    Error("missing type on -gravity",(char *) NULL);
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
                    Error("invalid gravity type on -gravity",option);
                }
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'h':
        {
          Usage();
          break;
        }
        case 'm':
        {
          resource_info.monochrome=(*option == '-');
          break;
        }
        case 'r':
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
        case 't':
        {
          if (strncmp("tiles_per_row",option+1,3) == 0)
            {
              tiles_per_row=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("missing value on -tiles_per_row",(char *) NULL);
                  tiles_per_row=atoi(argv[i]);
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
                    Error("missing title on -title",(char *) NULL);
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
                    Error("missing depth on -treedepth",(char *) NULL);
                  resource_info.tree_depth=atoi(argv[i]);
                }
              break;
            }
          Error("unrecognized option",option);
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
        char
          geometry[2048];

        Image
          *image,
          info_image;

        unsigned int
          scale_factor,
          tile_height,
          tile_width;

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
          if (image->scene == 0)
            image->scene=image_number;
          info_image=(*image);
          /*
            Allocate and initialize image label.
          */
          image->label=(char *)
            malloc(((strlen(image->filename)+2048)*sizeof(char)));
          if (image->label == (char *) NULL)
            Error("unable to montage images","memory allocation failed");
          (void) sprintf(image->label,"%s  %ux%u\0",image->filename,
            image->columns,image->rows);
          /*
            Tile size maintains the aspect ratio of the image.
          */
          tile_width=256;
          tile_height=256;
          if (resource_info.image_geometry != (char *) NULL)
            {
              int
                y;

              XParseGeometry(resource_info.image_geometry,&x,&y,&tile_width,
                &tile_height);
              if ((tile_width == 0) || (tile_height == 0))
                Error("invalid image geometry",resource_info.image_geometry);
            }
          if (aspect_ratio)
            {
              /*
                Respect aspect ratio of the image.
              */
              scale_factor=UpShift(tile_width)/image->columns;
              if (scale_factor > (UpShift(tile_height)/image->rows))
                scale_factor=UpShift(tile_height)/image->rows;
              tile_width=DownShift(image->columns*scale_factor);
              tile_height=DownShift(image->rows*scale_factor);
            }
          (void) sprintf(geometry,"%ux%u\0",tile_width,tile_height);
          TransformImage(&image,clip_geometry,geometry,(char *) NULL);
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
          if (gamma > 0.0)
            GammaImage(image,gamma);
          if (verbose)
            {
              /*
                Display detailed info about the image.
              */
              (void) fprintf(stderr,"[%u] %s",
                image->scene == 0 ? image_number : image->scene,
                image->filename);
              (void) fprintf(stderr," %ux%u",info_image.columns,
                info_image.rows);
              if ((info_image.columns != image->columns) ||
                  (info_image.rows != image->rows))
                (void) fprintf(stderr,"=>%ux%u",image->columns,image->rows);
              if (image->class == DirectClass)
                (void) fprintf(stderr," DirectClass");
              else
                (void) fprintf(stderr," PseudoClass %uc",image->colors);
              (void) fprintf(stderr," %s\n",image->magick);
            }
          /*
            Pack image data to conserve memory (memory <=> speed).
          */
          (void) PackImage(image);
          (void) free((char *) image->pixels);
          image->pixels=(RunlengthPacket *) NULL;
          if (image_number == maximum_images)
            {
              /*
                Increase size of images array.
              */
              maximum_images<<=1;
              images=(Image **)
                realloc((char *) images,maximum_images*sizeof(Image *));
              if (images == (Image **) NULL)
                Error("unable to montage images","memory allocation failed");
            }
          images[image_number++]=image;
          image=image->next;
        } while (image != (Image *) NULL);
      }
    }
  if (image_number == 0)
    Error("missing an image file name",(char *) NULL);
  /*
    Create composite image.
  */
  montage_image=MontageImage(display,&resource_info,compose,tiles_per_row,
    images,image_number);
  if (resource_info.colorspace == GRAYColorspace)
    QuantizeImage(montage_image,256,8,resource_info.dither,GRAYColorspace,True);
  if (resource_info.monochrome)
    QuantizeImage(montage_image,2,8,resource_info.dither,GRAYColorspace,True);
  if (resource_info.number_colors != 0)
    {
      QuantizeImage(montage_image,resource_info.number_colors,
        resource_info.tree_depth,resource_info.dither,resource_info.colorspace,
        True);
      SyncImage(montage_image);
    }
  if (compression != UndefinedCompression)
    montage_image->compression=compression;
  (void) strcpy(montage_image->filename,write_filename);
  (void) WriteAlienImage(montage_image);
  if (verbose)
    {
      /*
        Display detailed info about the image.
      */
      if (montage_image->class == DirectClass)
        montage_image->colors=NumberColors(montage_image,False);
      (void) fprintf(stderr,"[%u] %s %ux%u",montage_image->scene,
        montage_image->filename,montage_image->columns,montage_image->rows);
      if (montage_image->class == DirectClass)
        (void) fprintf(stderr," DirectClass ");
      else
        (void) fprintf(stderr," PseudoClass ");
      (void) fprintf(stderr,"%uc %s %lds\n",montage_image->colors,
        montage_image->magick,time((time_t *) 0)-start_time+1);
    }
  DestroyImage(montage_image);
  (void) free((char *) images);
  if (display != (Display *) NULL)
    XCloseDisplay(display);
  return(False);
}
