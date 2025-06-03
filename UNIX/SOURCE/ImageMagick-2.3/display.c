/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%             DDDD   IIIII  SSSSS  PPPP   L       AAA   Y   Y                 %
%             D   D    I    SS     P   P  L      A   A   Y Y                  %
%             D   D    I     SSS   PPPP   L      AAAAA    Y                   %
%             D   D    I       SS  P      L      A   A    Y                   %
%             DDDD   IIIII  SSSSS  P      LLLLL  A   A    Y                   %
%                                                                             %
%                                                                             %
%          Display Machine Independent File Format Image via X11.             %
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
%  Display is a machine architecture independent image processing
%  and display program.  It can display any image in the MIFF format on
%  any workstation display running X.  Display first determines the
%  hardware capabilities of the workstation.  If the number of unique
%  colors in the image is less than or equal to the number the workstation
%  can support, the image is displayed in an X window.  Otherwise the
%  number of colors in the image is first reduced to match the color
%  resolution of the workstation before it is displayed.
%
%  This means that a continuous-tone 24 bits-per-pixel image can display on a
%  8 bit pseudo-color device or monochrome device.  In most instances the
%  reduced color image closely resembles the original.  Alternatively, a
%  monochrome or pseudo-color image can display on a continuous-tone 24
%  bits-per-pixel device.
%
%  The Display program command syntax is:
%
%  Usage: display [options ...] file [ [options ...] file ...]
%
%  Where options include:
%    -backdrop           display image centered on a backdrop
%    -clip geometry      preferred size and location of the clipped image
%    -colormap type      Shared or Private
%    -colors value       preferred number of colors in the image
%    -colorspace type    GRAY, RGB, XYZ, YIQ, or YUV
%    -compress type      RunlengthEncoded or QEncoded
%    -delay seconds      display the next image after pausing
%    -density geometry   vertical and horizonal density of the image
%    -display server     display image to this X server
%    -dither             apply Floyd/Steinberg error diffusion to image
%    -enhance            apply a digital filter to enhance a noisy image
%    -gamma value        level of gamma correction
%    -geometry geometry  preferred size and location of the image window
%    -inverse            apply color inversion to image
%    -map type           display image using this Standard Colormap
%    -monochrome         transform image to black and white
%    -noise              reduce noise with a noise peak elimination filter
%    -normalize          tranform image to span the full the range of colors
%    -print filename     write image as Postscript to a file
%    -reflect            reverse image scanlines
%    -rotate degrees     apply Paeth rotation to the image
%    -scale geometry     preferred size factors of the image
%    -scene value        image scene number
%    -treedepth value    depth of the color classification tree
%    -verbose            print detailed information about the image
%    -visual type        display image using this visual type
%    -window id          display image to background of this window
%    -write filename     write image to a file
%
%  In addition to those listed above, you can specify these standard X
%  resources as command line options:  -background, -bordercolor,
%  -borderwidth, -font, -foreground, -iconGeometry, -iconic, -name, or
%  -title.
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
%  Buttons:
%    1    press and drag to select a command from a pop-up menu
%    2    press and drag to define a region of the image to clip
%    3    press and drag to define a region of the image to magnify
%
%  Keyboard accelerators:
%    i    display information about the image
%    r    reflect the image scanlines
%    /    rotate the image 90 degrees clockwise
%    \    rotate the image 90 degrees counter-clockwise
%    <    half the image size
%    >    double the image size
%    o    restore the image to its original size
%    a    annotate the image with text
%    c    composite image with another
%    w    write the image to a file
%    p    print the image to a file
%    n    display the next image
%    l    display the last image
%    q    discard all images and exit program
%    1-9  change the level of magnification
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
  State declarations.
*/
#define ControlState  0x0001
#define DefaultState  0x0000
#define EscapeState  0x0002
#define ExitState  0x0004
#define ImageMappedState  0x0008
#define InfoMappedState  0x0010
#define LastImageState  0x0020
#define MagnifyMappedState  0x0040
#define NextImageState  0x0080
#define PanIconMappedState  0x0100
#define ReconfigureImageState  0x0200
#define TileImageState  0x0400
#define UpdateColormapState  0x0800
#define UpdateConfigurationState  0x1000

/*
  Global declarations.
*/
char
  *client_name;

/*
  Forward declarations.
*/
static Cursor
  XMakeCursor _Declare((Display *,Colormap,Window,char *,char *));

static Image
  *XTileImageWindow _Declare((Display *,XResourceInfo *,XWindows *,Image *,
    XEvent *));

static unsigned int
  XAnnotateImageWindow _Declare((Display *,XResourceInfo *,XWindows *,Image *)),
  XCompositeImageWindow _Declare((Display *,XResourceInfo *,XWindows *,
    Image **)),
  XConfigureImageWindow _Declare((Display *,XResourceInfo *,XWindows *,
    unsigned int,unsigned int,Image *)),
  XReflectImageWindow _Declare((Display *,XWindows *,Image **)),
  XRotateImageWindow _Declare((Display *,XWindows *,unsigned int,Image **));

static void
  XDrawPanRectangle _Declare((Display *,XWindows *)),
  XMagnifyImageWindow _Declare((Display *,XResourceInfo *,XWindows *,XEvent *)),
  XMakeMagnifyImage _Declare((Display *,XResourceInfo *,XWindows *)),
  XPanImageWindow _Declare((Display *,XWindows *,XEvent *));

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
%      Usage(terminate)
%
%  A description of each parameter follows:
%
%    o terminate: A value other than zero is returned if the program is to
%      terminate immediately.
%
*/
static void Usage(terminate)
unsigned int
  terminate;
{
  char
    **p;

  static char
    *buttons[]=
    {
      "1    press and drag to select a command from a pop-up menu",
      "2    press and drag to define a region of the image to clip",
      "3    press and drag to define a region of the image to magnify",
      (char *) NULL
    },
    *keys[]=
    {
      "i    display information about the image",
      "r    reflect the image scanlines",
      "/    rotate the image 90 degrees clockwise",
      "\\    rotate the image 90 degrees counter-clockwise",
      "<    half the image size",
      ">    double the image size",
      "o    restore the image to its original size",
      "a    annotate the image with text",
      "c    composite image with another",
      "w    write the image to a file",
      "p    print the image to a file",
      "n    display the next image",
      "l    display the last image",
      "q    discard all images and exit program",
      "1-9  change the level of magnification",
      (char *) NULL
    },
    *options[]=
    {
      "-backdrop           display image centered on a backdrop",
      "-clip geometry      preferred size and location of the clipped image",
      "-colormap type      Shared or Private",
      "-colors value       preferred number of colors in the image",
      "-colorspace type    GRAY, RGB, XYZ, YIQ, or YUV",
      "-compress type      RunlengthEncoded or QEncoded",
      "-delay seconds      display the next image after pausing",
      "-density geometry   vertical and horizonal density of the image",
      "-display server     display image to this X server",
      "-dither             apply Floyd/Steinberg error diffusion to image",
      "-enhance            apply a digital filter to enhance a noisy image",
      "-gamma value        level of gamma correction",
      "-geometry geometry  preferred size and location of the image window",
      "-inverse            apply color inversion to image",
      "-map type           display image using this Standard Colormap",
      "-monochrome         transform image to black and white",
      "-noise              reduce noise with a noise peak elimination filter",
      "-normalize          tranform image to span the full range of colors",
      "-print filename     write image as Postscript to a file",
      "-reflect            reflect the image scanlines",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scale geometry     preferred size factors of the image",
      "-scene value        image scene number",
      "-treedepth value    depth of the color classification tree",
      "-verbose            print detailed information about the image",
      "-visual type        display image using this visual type",
      "-window id          display image to background of this window",
      "-write filename     write image to a file",
      (char *) NULL
    };
  (void) fprintf(stderr,
    "Usage: %s [-options ...] file [ [-options ...] file ...]\n",client_name);
  (void) fprintf(stderr,"\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) fprintf(stderr,
    "resources as command line options:  -background, -bordercolor,\n");
  (void) fprintf(stderr,
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -name, or\n");
  (void) fprintf(stderr,"-title.\n");
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
  (void) fprintf(stderr,"\nButtons: \n");
  for (p=buttons; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,"\nKeyboard accelerators: \n");
  for (p=keys; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  if (terminate)
    exit(1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U s e r C o m m a n d                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function UserCommand makes a transform to the image or image window as
%  specified by a user menu button or keyboard command.
%
%  The format of the UserCommand routine is:
%
%    UserCommand(display,resource_info,window,command,image,state)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o command: Specifies a command to perform.
%
%    o image: Specifies a pointer to a Image structure;  UserCommand
%      may transform the image and return a new image pointer.
%
%    o state: Specifies an unsigned int;  UserCommand may return a
%      modified state.
%
%
*/
static void UserCommand(display,resource_info,window,command,image,state)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;

char
  command;

Image
  **image;

unsigned long int
  *state;
{
  char
    text[2048];

  unsigned int
    height,
    status,
    width;

  XEvent
    event;

  if (*state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  XSync(display,False);
  while (XCheckTypedWindowEvent(display,window->image.id,Expose,&event))
    XRefreshWindow(display,&window->image,&event);
  /*
    Process user command.
  */
  width=0;
  height=0;
  switch (command)
  {
    case ' ':
    case '\0':
      break;
    case '<':
    {
      /*
        Half image size.
      */
      width=window->image.ximage->width >> 1;
      height=window->image.ximage->height >> 1;
      break;
    }
    case '>':
    {
      /*
        Double the image size.
      */
      width=window->image.ximage->width << 1;
      height=window->image.ximage->height << 1;
      break;
    }
    case '/':
    case '\\':
    {
      /*
        Rotate image 90 degrees to clockwise or counter-clockwise.
      */
      status=XRotateImageWindow(display,window,(unsigned int)
        (command == '/' ? 90 : 270),image);
      if (status == False)
        {
          XPopupAlert(display,&window->popup,"unable to rotate X image",
            window->image.name);
          break;
        }
      width=window->image.ximage->height;
      height=window->image.ximage->width;
      break;
    }
    case 'a':
    {
      /*
        Annotate the image with text.
      */
      status=XAnnotateImageWindow(display,resource_info,window,*image);
      if (status == False)
        {
          XPopupAlert(display,&window->popup,"unable to annotate X image",
            window->image.name);
          break;
        }
      width=window->image.ximage->width;
      height=window->image.ximage->height;
      break;
    }
    case 'c':
    {
      /*
        Composite image.
      */
      status=XCompositeImageWindow(display,resource_info,window,image);
      if (status == False)
        {
          XPopupAlert(display,&window->popup,"unable to composite X image",
            window->image.name);
          break;
        }
      XMakeStandardColormap(display,window->image.visual_info,resource_info,
        window->image.pixel_info,*image,window->image.map_info);
      width=window->image.ximage->width;
      height=window->image.ximage->height;
      *state|=UpdateColormapState;
      break;
    }
    case 'i':
    {
      /*
        Display information about the image in the info window.
      */
      (void) sprintf(text," [%u] %s %dx%d %s \0",(*image)->scene,
        (*image)->filename,window->image.ximage->width,
        window->image.ximage->height,
        XVisualClassName(window->info.visual_info));
      if ((*image)->colors > 0)
        (void) sprintf(text,"%s%uc \0",text,(*image)->colors);
      (void) strcat(text,(*image)->magick);
      XSetWindowExtents(window->info,text,2);
      XMapWindow(display,window->info.id);
      XDrawImageString(display,window->info.id,window->info.graphic_context,2,
        window->info.font_info->ascent+2,text,strlen(text));
      break;
    }
    case 'l':
    case '\b':
    {
      /*
        Display last image.
      */
      *state|=LastImageState | ExitState;
      break;
    }
    case 'n':
    case '\r':
    {
      /*
        Display next image.
      */
      *state|=NextImageState | ExitState;
      break;
    }
    case 'o':
    {
      /*
        Restore image window to its original size.
      */
      if (window->image.clip_geometry != (char *) NULL)
        {
          (void) free((char *) window->image.clip_geometry);
          window->image.clip_geometry=(char *) NULL;
          window->image.x=0;
          window->image.y=0;
        }
      width=(*image)->columns;
      height=(*image)->rows;
      break;
    }
    case 'p':
    case 'w':
    {
      char
        filename[2048];

      Image
        *output_image;

      /*
        Write or print image to file.
      */
      (void) strcpy(filename,(*image)->filename);
      if (command == 'p')
        {
          (void) strcpy(text," Printing image... ");
          if (resource_info->print_filename != (char *) NULL)
            (void) strcpy(filename,resource_info->print_filename);
        }
      else
        {
          (void) strcpy(text," Writing image... ");
          if (resource_info->write_filename != (char *) NULL)
            (void) strcpy(filename,resource_info->write_filename);
        }
      XPopupQuery(display,&window->popup,"File name:",filename);
      if (*filename == '\0')
        break;
      /*
        Alert user we are busy.
      */
      XDefineCursor(display,window->image.id,window->image.busy_cursor);
      XSetWindowExtents(window->info,text,2);
      XMapWindow(display,window->info.id);
      XDrawImageString(display,window->info.id,window->info.graphic_context,2,
        window->info.font_info->ascent+2,text,strlen(text));
      XFlush(display);
      /*
        Copy image before applying image transforms.
      */
      output_image=CopyImage(*image,(*image)->columns,(*image)->rows,True);
      if (output_image == (Image *) NULL)
        {
          XPopupAlert(display,&window->popup,"unable to output image",
            window->image.name);
          break;
        }
      if ((window->image.clip_geometry != (char *) NULL) ||
         (output_image->columns != window->image.ximage->width) ||
         (output_image->rows != window->image.ximage->height))
        {
          char
            image_geometry[2048];

          /*
            Clip and/or scale image.
          */
          (void) sprintf(image_geometry,"%dx%d\0",window->image.ximage->width,
            window->image.ximage->height);
          TransformImage(&output_image,window->image.clip_geometry,
            image_geometry,(char *) NULL);
        }
      if (resource_info->colorspace == GRAYColorspace)
        QuantizeImage(output_image,256,8,resource_info->dither,GRAYColorspace,
          True);
      if (resource_info->monochrome)
        QuantizeImage(output_image,2,8,resource_info->dither,GRAYColorspace,
          True);
      if (resource_info->number_colors > 0)
        if ((output_image->class == DirectClass) ||
            (output_image->colors > resource_info->number_colors))
          {
            QuantizeImage(output_image,resource_info->number_colors,
              resource_info->tree_depth,resource_info->dither,
              resource_info->colorspace,True);
            SyncImage(output_image);
          }
      (void) strcpy(output_image->filename,filename);
      if (command == 'p')
        (void) PrintImage(output_image,resource_info->image_geometry);
      else
        (void) WriteAlienImage(output_image);
      DestroyImage(output_image);
      XDefineCursor(display,window->image.id,window->image.cursor);
      XWithdrawWindow(display,window->info.id,window->info.screen);
      break;
    }
    case 'q':
    {
      /*
        Exit program
      */
      *state|=ExitState;  /* exit program */
      break;
    }
    case 'r':
    {
      /*
        Reflect image scanlines.
      */
      status=XReflectImageWindow(display,window,image);
      if (status == False)
        {
          XPopupAlert(display,&window->popup,"unable to reflect X image",
            window->image.name);
          break;
        }
      width=window->image.ximage->width;
      height=window->image.ximage->height;
      break;
    }
    default:
    {
      XBell(display,0);
      break;
    }
  }
  if ((width != 0) && (height != 0))
    {
      unsigned int
        mask;

      XWindowChanges
        window_changes;

      /*
        Image configuration has changed.
      */
      status=XConfigureImageWindow(display,resource_info,window,width,height,
        *image);
      if (status == False)
        XPopupAlert(display,&window->popup,"unable to configure X image",
          window->image.name);
      /*
        Window size must not exceed that of the X server screen.
      */
      if (width > XDisplayWidth(display,window->image.screen))
        width=(XDisplayWidth(display,window->image.screen)*7) >> 3;
      if (height > XDisplayHeight(display,window->image.screen))
        height=(XDisplayHeight(display,window->image.screen)*7) >> 3;
      if ((width == window->image.width) && (height == window->image.height))
        {
          *state|=UpdateConfigurationState;
          return;
        }
      /*
        Notify window manager of the new configuration.
      */
      window_changes.width=width;
      window_changes.height=height;
      mask=CWWidth | CWHeight;
      if (resource_info->backdrop)
        {
          window_changes.x=
            XDisplayWidth(display,window->image.screen)/2-width/2;
          window_changes.y=
            XDisplayHeight(display,window->image.screen)/2-height/2;
          mask|=CWX | CWY;
        }
      XReconfigureWMWindow(display,window->image.id,window->image.screen,mask,
        &window_changes);
      *state|=ReconfigureImageState;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n n o t a t e I m a g e W i n d o w                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XAnnotateImageWindow annotates the image with text.
%
%  The format of the XAnnotateImageWindow routine is:
%
%    XAnnotateImageWindow(display,resource_info,window,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
*/
static unsigned int XAnnotateImageWindow(display,resource_info,window,image)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;

Image
  *image;
{
  char
    *p,
    text[2048];

  Cursor
    cursor;

  GC
    graphic_context;

  int
    i,
    x,
    y;

  static unsigned int
    font_id=0,
    pen_id=0;

  unsigned int
    height,
    mask,
    status,
    width;

  unsigned long int
    state,
    x_factor,
    y_factor;

  Window
    xwindow;

  XAnnotateInfo
    *annotate_info,
    *previous_info;

  XColor
    pen_color;

  XFontStruct
    *font_info;

  XEvent
    event,
    text_event;

  XGCValues
    graphic_context_value;

  /*
    Map info window.
  */
  state=DefaultState;
  (void) sprintf(text," %+u%+u  \0",window->image.width,window->image.height);
  XSetWindowExtents(window->info,text,2);
  XMapWindow(display,window->info.id);
  state|=InfoMappedState;
  /*
    Track pointer until button 1 is pressed.
  */
  XQueryPointer(display,window->image.id,&xwindow,&xwindow,&i,&i,&x,&y,&mask);
  XSelectInput(display,window->image.id,window->image.attributes.event_mask |
    PointerMotionMask);
  do
  {
    if (state & InfoMappedState)
      {
        /*
          Display pointer position.
        */
        (void) sprintf(text," %+d%+d \0",x-window->image.x,y-window->image.y);
        XClearWindow(display,window->info.id);
        XDrawImageString(display,window->info.id,window->info.graphic_context,2,
          window->info.font_info->ascent+2,text,strlen(text));
      }
    /*
      Wait for next event.
    */
    XWindowEvent(display,window->image.id,ButtonPressMask | ButtonReleaseMask |
      ExposureMask | KeyPressMask | PointerMotionMask,&event);
    switch (event.type)
    {
      case ButtonPress:
      {
        if ((event.xbutton.button == Button3) &&
            (event.xbutton.state & Mod1Mask))
          {
            /* 
              Convert Alt-Button3 to Button2.
            */
            event.xbutton.button=Button2;
            event.xbutton.state&=(~Mod1Mask);
          }
        switch (event.xbutton.button)
        {
          case Button1:
          {
            /*
              Change to text entering mode.
            */
            x=event.xbutton.x;
            y=event.xbutton.y;
            state|=ExitState;
            break;
          }
          case Button2:
          {
            char
              font_name[2048],
              *MenuSelections[MaxNumberFonts];

            int
              font_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < MaxNumberFonts; i++)
              MenuSelections[i]=resource_info->font_name[i];
            /*
              Select a font name from the pop-up menu.
            */
            font_number=XPopupMenu(display,&window->popup,event.xbutton.x_root,
              event.xbutton.y_root,"Pen Fonts",MenuSelections,MaxNumberFonts,
              font_name);
            if (*font_name != '\0')
              {
                /*
                  Initialize font info.
                */
                font_info=XLoadQueryFont(display,font_name);
                if (font_info == (XFontStruct *) NULL)
                  XPopupAlert(display,&window->popup,"unable to load font",
                    font_name);
                else
                  {
                    font_id=font_number;
                    XFreeFont(display,font_info);
                  }
              }
            break;
          }
          case Button3:
          {
            char
              color_name[2048],
              *MenuSelections[MaxNumberPens];

            int
              pen_number;

            /*
              Initialize menu selections.
            */
            for (i=0; i < MaxNumberPens; i++)
              MenuSelections[i]=resource_info->pen_color[i];
            /*
              Select a font color from the pop-up menu.
            */
            pen_number=XPopupMenu(display,&window->popup,event.xbutton.x_root,
              event.xbutton.y_root,"Pen Colors",MenuSelections,MaxNumberPens,
              color_name);
            if (*color_name != '\0')
              pen_id=pen_number;
            break;
          }
        }
        break;
      }
      case KeyPress:
      {
        static char
          command[2048];

        static KeySym
          key_symbol;

        /*
          Respond to a user key press.
        */
        *command='\0';
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Escape)
          {
            /*
              Prematurely exit.
            */
            state|=EscapeState;
            state|=ExitState;
            break;
          }
        break;
      }
      case Expose:
      {
        /*
          Refresh image window.
        */
        XRefreshWindow(display,&window->image,&event);
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending pointer motion events.
        */
        while (XCheckMaskEvent(display,PointerMotionMask,&event));
        x=event.xmotion.x;
        y=event.xmotion.y;
        /*
          Map and unmap info window as cursor crosses its boundaries.
        */
        if (state & InfoMappedState)
          {
            if ((x < (window->info.x+window->info.width)) &&
                (y < (window->info.y+window->info.height)))
              {
                XWithdrawWindow(display,window->info.id,window->info.screen);
                state&=(~InfoMappedState);
              }
          }
        else
          if ((x > (window->info.x+window->info.width)) ||
              (y > (window->info.y+window->info.height)))
            {
              XMapWindow(display,window->info.id);
              state|=InfoMappedState;
            }
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XSelectInput(display,window->image.id,window->image.attributes.event_mask);
  if (state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  XFlush(display);
  if (state & EscapeState)
    return(True);
  /*
    Set font info and pen color.
  */
  font_info=XLoadQueryFont(display,resource_info->font_name[font_id]);
  if (font_info == (XFontStruct *) NULL)
    {
      XPopupAlert(display,&window->popup,"unable to load font",
        resource_info->font_name[font_id]);
      font_info=window->image.font_info;
    }
  /*
    Initialize graphic context.
  */
  pen_color=window->image.pixel_info->pen_color[pen_id];
  graphic_context_value.background=
    window->image.pixel_info->background_color.pixel;
  graphic_context_value.foreground=pen_color.pixel;
  graphic_context_value.font=font_info->fid;
  graphic_context_value.function=GXcopy;
  graphic_context_value.line_width=WindowBorderWidth;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context=XCreateGC(display,window->image.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCLineWidth | GCPlaneMask,
    &graphic_context_value);
  if (graphic_context == (GC) NULL)
    return(False);
  /*
    Check boundary conditions.
  */
  if ((x+font_info->max_bounds.width) >= window->image.width)
    x=window->image.width-font_info->max_bounds.width;
  if (y < (font_info->ascent+font_info->descent))
    y=font_info->ascent+font_info->descent;
  if ((font_info->max_bounds.width > window->image.width) ||
      ((font_info->ascent+font_info->descent) >= window->image.height))
    return(False);
  /*
    Initialize annotate structure.
  */
  annotate_info=(XAnnotateInfo *) malloc(sizeof(XAnnotateInfo));
  if (annotate_info == (XAnnotateInfo *) NULL)
    return(False);
  XGetAnnotateInfo(annotate_info);
  annotate_info->x=x;
  annotate_info->y=y;
  annotate_info->height=font_info->ascent+font_info->descent;
  annotate_info->font_info=font_info;
  annotate_info->text=(char *) malloc(
    (window->image.width/Max(font_info->min_bounds.width,1)+2)*sizeof(char));
  if (annotate_info->text == (char *) NULL)
    return(False);
  /*
    Begin annotating the image with text.
  */
  state=DefaultState;
  cursor=XCreateFontCursor(display,XC_pencil);
  XRecolorCursor(display,cursor,&window->image.pixel_info->background_color,
    &window->image.pixel_info->foreground_color);
  XDefineCursor(display,window->image.id,cursor);
  XDrawString(display,window->image.id,graphic_context,x,y,"_",1);
  text_event.xexpose.width=(unsigned int) font_info->max_bounds.width;
  text_event.xexpose.height=annotate_info->height;
  p=annotate_info->text;
  *p='\0';
  do
  {
    /*
      Wait for next event.
    */
    XWindowEvent(display,window->image.id,ExposureMask | KeyPressMask,&event);
    switch (event.type)
    {
      case Expose:
      {
        if (event.xexpose.count == 0)
          {
            /*
              Refresh image window.
            */
            XRefreshWindow(display,&window->image,(XEvent *) NULL);
            XDrawString(display,window->image.id,graphic_context,
              annotate_info->x,annotate_info->y,annotate_info->text,
              strlen(annotate_info->text));
            XDrawString(display,window->image.id,graphic_context,x,y,"_",1);
          }
        break;
      }
      case KeyPress:
      {
        static char
          command[2048];

        static KeySym
          key_symbol;

        /*
          Erase text cursor.
        */
        text_event.xexpose.x=x;
        text_event.xexpose.y=y-font_info->max_bounds.ascent;
        XRefreshWindow(display,&window->image,&text_event);
        /*
          Respond to a user key press.
        */
        *command='\0';
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Control_L)
          {
            state|=ControlState;
            break;
          }
        if (state & ControlState)
          switch (key_symbol)
          {
            case XK_u:
            case XK_U:
            {
              key_symbol=XK_Delete;
              break;
            }
            default:
              break;
          }
        switch (key_symbol)
        {
          case XK_BackSpace:
          {
            /*
              Erase one character.
            */
            if (p == annotate_info->text)
              if (annotate_info->previous == (XAnnotateInfo *) NULL)
                break;
              else
                {
                  /*
                    Go to end of the previous line of text.
                  */
                  annotate_info=annotate_info->previous;
                  p=annotate_info->text;
                  x=annotate_info->x+annotate_info->width;
                  y=annotate_info->y;
                  if (annotate_info->width > 0)
                    p+=strlen(annotate_info->text);
                  break;
                }
            p--;
            x-=XTextWidth(font_info,p,1);
            text_event.xexpose.x=x;
            text_event.xexpose.y=y-font_info->max_bounds.ascent;
            XRefreshWindow(display,&window->image,&text_event);
            break;
          }
          case XK_Delete:
          {
            /*
              Erase the entire line of text.
            */
            while (p != annotate_info->text)
            {
              p--;
              x-=XTextWidth(font_info,p,1);
              text_event.xexpose.x=x;
              XRefreshWindow(display,&window->image,&text_event);
            }
            break;
          }
          case XK_Escape:
          {
            /*
              Finished annotating.
            */
            annotate_info->width=XTextWidth(font_info,annotate_info->text,
              strlen(annotate_info->text));
            XRefreshWindow(display,&window->image,&text_event);
            state|=ExitState;
            break;
          }
          default:
          {
            /*
              Draw a single character on the image window.
            */
            if (*command == '\0')
              break;
            *p=(*command);
            XDrawString(display,window->image.id,graphic_context,x,y,p,1);
            x+=XTextWidth(font_info,p,1);
            p++;
            if ((x+font_info->max_bounds.width) < window->image.width)
              break;
          }
          case XK_Return:
          {
            /*
              Advance to the next line of text.
            */
            *p='\0';
            annotate_info->width=XTextWidth(font_info,annotate_info->text,
              strlen(annotate_info->text));
            if (annotate_info->next != (XAnnotateInfo *) NULL)
              {
                /*
                  Line of text already exists.
                */
                annotate_info=annotate_info->next;
                x=annotate_info->x;
                y=annotate_info->y;
                p=annotate_info->text;
                break;
              }
            annotate_info->next=(XAnnotateInfo *) malloc(sizeof(XAnnotateInfo));
            if (annotate_info->next == (XAnnotateInfo *) NULL)
              return(False);
            *annotate_info->next=(*annotate_info);
            annotate_info->next->previous=annotate_info;
            annotate_info=annotate_info->next;
            annotate_info->text=(char *) malloc((window->image.width/
              Max(font_info->min_bounds.width,1)+2)*sizeof(char));
            if (annotate_info->text == (char *) NULL)
              return(False);
            annotate_info->y+=annotate_info->height;
            if (annotate_info->y > window->image.height)
              annotate_info->y=annotate_info->height;
            annotate_info->next=(XAnnotateInfo *) NULL;
            x=annotate_info->x;
            y=annotate_info->y;
            p=annotate_info->text;
            break;
          }
        }
        /*
          Display text cursor.
        */
        *p='\0';
        XDrawString(display,window->image.id,graphic_context,x,y,"_",1);
        break;
      }
      case KeyRelease:
      {
        static char
          command[2048];

        static KeySym
          key_symbol;

        /*
          Respond to a user key release.
        */
        *command='\0';
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Control_L)
          state&=(~ControlState);
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XFreeCursor(display,cursor);
  XFlush(display);
  /*
    Annotation is relative to image configuration.
  */
  x=0;
  y=0;
  width=image->columns;
  height=image->rows;
  if (window->image.clip_geometry != (char *) NULL)
    (void) XParseGeometry(window->image.clip_geometry,&x,&y,&width,&height);
  /*
    Initialize annotated image.
  */
  while (annotate_info != (XAnnotateInfo *) NULL)
  {
    if (annotate_info->width == 0)
      {
        /*
          No text on this line--  go to the next line of text.
        */
        previous_info=annotate_info->previous;
        (void) free((char *) annotate_info->text);
        (void) free((char *) annotate_info);
        annotate_info=previous_info;
        continue;
      }
    /*
      Determine foreground pixel index for font color.
    */
    window->image.pixel_info->annotate_color=pen_color;
    window->image.pixel_info->annotate_index=0;
    if (window->image.pixel_info->colors > 0)
      for (i=0; i < window->image.pixel_info->colors; i++)
        if (window->image.pixel_info->pixels[i] == pen_color.pixel)
          {
            window->image.pixel_info->annotate_index=i;
            break;
          }
    /*
      Define the annotate geometry string.
    */
    x_factor=UpShift(width)/window->image.ximage->width;
    annotate_info->x+=window->image.x;
    annotate_info->x=DownShift(annotate_info->x*x_factor);
    y_factor=UpShift(height)/window->image.ximage->height;
    annotate_info->y+=(window->image.y-font_info->ascent);
    annotate_info->y=DownShift(annotate_info->y*y_factor);
    (void) sprintf(annotate_info->geometry,"%ux%u%+d%+d\0",
      (unsigned int) DownShift(annotate_info->width*x_factor),
      (unsigned int) DownShift(annotate_info->height*y_factor),
      annotate_info->x+x,annotate_info->y+y);
    /*
      Annotate image with text.
    */
    status=XAnnotateImage(display,&window->image,annotate_info,False,image);
    if (status == 0)
      return(False);
    /*
      Free up memory.
    */
    previous_info=annotate_info->previous;
    (void) free((char *) annotate_info->text);
    (void) free((char *) annotate_info);
    annotate_info=previous_info;
  }
  XDefineCursor(display,window->image.id,window->image.cursor);
  /*
    Free up memory.
  */
  if (font_info != window->image.font_info)
    XFreeFont(display,font_info);
  XFreeGC(display,graphic_context);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C l i p I m a g e W i n d o w                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XClipImageWindow displays a rectangle whose sizes changes as the
%  pointer moves.  When the mouse button is released, the geometry of the image
%  region defined within the rectangle is returned.
%
%  The format of the XClipImageWindow routine is:
%
%    XClipImageWindow(display,resource_info,window,event,image,state)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%    o image: Specifies a pointer to a Image structure.
%
%    o state: Specifies an unsigned int;  XClipImageWindow may return a
%      modified state.
%
%
*/
static void XClipImageWindow(display,resource_info,window,event,image,state)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;

XEvent
  *event;

Image
  *image;

unsigned long int
  *state;
{
#define ClipRectangle(clip_info)  \
{  \
  if ((clip_info.width*clip_info.height) >= MinimumClipArea) \
    {  \
      XSetFunction(display,window->image.graphic_context,GXinvert);  \
      XDrawRectangle(display,window->image.id,window->image.graphic_context,  \
        clip_info.x,clip_info.y,clip_info.width-1,clip_info.height-1);  \
      XSetFunction(display,window->image.graphic_context,GXcopy);  \
    }  \
}
#define MinimumClipArea  (unsigned int) 9

  char
    text[2048];

  int
    x_offset,
    y_offset;

  XRectangle
    clip_info;

  unsigned int
    status;

  /*
    Set the width of info window.
  */
  (void) sprintf(text," %ux%u%+u%+u \0",window->image.width,
    window->image.height,window->image.width,window->image.height);
  XSetWindowExtents(window->info,text,2);
  /*
    Size rectangle as pointer moves until the mouse button is released.
  */
  x_offset=event->xbutton.x;
  y_offset=event->xbutton.y;
  clip_info.x=x_offset;
  clip_info.y=y_offset;
  clip_info.width=0;
  clip_info.height=0;
  do
  {
    if ((clip_info.width*clip_info.height) >= MinimumClipArea)
      {
        /*
          Display info and draw clipping rectangle.
        */
        if (!(*state & InfoMappedState))
          {
            XMapWindow(display,window->info.id);
            *state|=InfoMappedState;
          }
        (void) sprintf(text," %ux%u%+d%+d\0",clip_info.width,clip_info.height,
          clip_info.x,clip_info.y);
        XClearWindow(display,window->info.id);
        XDrawImageString(display,window->info.id,window->info.graphic_context,2,
          window->info.font_info->ascent+2,text,strlen(text));
        ClipRectangle(clip_info);
      }
    else
      if (*state & InfoMappedState)
        {
          /*
            Clipping rectangle is too small;  withdraw info window.
          */
          XWithdrawWindow(display,window->info.id,window->info.screen);
          *state&=(~InfoMappedState);
        }
    /*
      Wait for next event.
    */
    XWindowEvent(display,window->image.id,ButtonPressMask | ButtonMotionMask |
      ButtonReleaseMask | ExposureMask,event);
    ClipRectangle(clip_info);
    switch (event->type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          User has committed to clipping rectangle.
        */
        clip_info.x=event->xbutton.x;
        clip_info.y=event->xbutton.y;
        *state|=ExitState;
        break;
      }
      case Expose:
      {
        /*
          Refresh image window.
        */
        XRefreshWindow(display,&window->image,event);
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,ButtonMotionMask,event));
        clip_info.x=event->xmotion.x;
        clip_info.y=event->xmotion.y;
      }
      default:
        break;
    }
    /*
      Check boundary conditions.
    */
    if (clip_info.x < 0)
      clip_info.x=0;
    else
      if (clip_info.x > window->image.width)
        clip_info.x=window->image.width-1;
    if (clip_info.x < x_offset)
      clip_info.width=(unsigned int) (x_offset-clip_info.x);
    else
      {
        clip_info.width=(unsigned int) (clip_info.x-x_offset);
        clip_info.x=x_offset;
      }
    if (clip_info.y < 0)
      clip_info.y=0;
    else
      if (clip_info.y > window->image.height)
        clip_info.y=window->image.height-1;
    if (clip_info.y < y_offset)
      clip_info.height=(unsigned int) (y_offset-clip_info.y);
    else
      {
        clip_info.height=(unsigned int) (clip_info.y-y_offset);
        clip_info.y=y_offset;
      }
  } while (!(*state & ExitState));
  *state&=(~ExitState);
  ClipRectangle(clip_info);
  ClipRectangle(clip_info);
  if (*state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  if ((clip_info.width*clip_info.height) >= MinimumClipArea)
    {
      int
        x,
        y;

      unsigned int
        height,
        width;

      unsigned long
        scale_factor;

      XWindowChanges
        window_changes;

      /*
        Clipping geometry is relative to any previous clip geometry.
      */
      x=0;
      y=0;
      width=image->columns;
      height=image->rows;
      if (window->image.clip_geometry != (char *) NULL)
        (void) XParseGeometry(window->image.clip_geometry,&x,&y,&width,&height);
      else
        {
          /*
            Allocate clip geometry string.
          */
          window->image.clip_geometry=(char *) malloc(2048*sizeof(char));
          if (window->image.clip_geometry == (char *) NULL)
            Error("unable to clip X image",window->image.name);
        }
      /*
        Define the clip geometry string from the clipping rectangle.
      */
      scale_factor=UpShift(width)/window->image.ximage->width;
      clip_info.x+=window->image.x;
      if (clip_info.x > 0)
        x+=DownShift(clip_info.x*scale_factor);
      width=DownShift(clip_info.width*scale_factor);
      if (width == 0)
        width=1;
      scale_factor=UpShift(height)/window->image.ximage->height;
      clip_info.y+=window->image.y;
      if (clip_info.y > 0)
        y+=DownShift(clip_info.y*scale_factor);
      height=DownShift(clip_info.height*scale_factor);
      if (height == 0)
        height=1;
      (void) sprintf(window->image.clip_geometry,"%ux%u%+d%+d\0",width,height,
        x,y);
      /*
        Image configuration has changed.
      */
      status=XConfigureImageWindow(display,resource_info,window,clip_info.width,
        clip_info.height,image);
      if (status == False)
        XPopupAlert(display,&window->popup,"unable to configure X image",
          window->image.name);
      /*
        Reconfigure image window as defined by clipping rectangle.
      */
      window_changes.width=clip_info.width;
      window_changes.height=clip_info.height;
      XReconfigureWMWindow(display,window->image.id,window->image.screen,
        CWWidth | CWHeight,&window_changes);
      *state|=ReconfigureImageState;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C o m p o s i t e I m a g e W i n d o w                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XCompositeImageWindow requests an image name from the user, reads
%  the image and composites it with the X window image at a location the user
%  chooses with the pointer.
%
%  The format of the XCompositeImageWindow routine is:
%
%    XCompositeImageWindow(display,resource_info,window,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
*/
static unsigned int XCompositeImageWindow(display,resource_info,window,image)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;

Image
  **image;
{
#define CompositeRectangle(composite_info)  \
{  \
  if ((composite_info.width*composite_info.height) > (unsigned int) 0) \
    {  \
      XSetFunction(display,window->image.graphic_context,GXinvert);  \
      XDrawRectangle(display,window->image.id,window->image.graphic_context,  \
        composite_info.x,composite_info.y,composite_info.width-1,  \
        composite_info.height-1);  \
      XSetFunction(display,window->image.graphic_context,GXcopy);  \
    }  \
}

  AlienInfo
    alien_info;

  char
    filename[2048],
    text[2048];

  Cursor
    cursor;

  Image
    *composite_image;

  int
    i,
    x,
    y;

  static unsigned int
    operator = OverCompositeOp;

  unsigned int
    height,
    mask,
    width;

  unsigned long int
    scale_factor,
    state;

  Window
    xwindow;

  XEvent
    event;

  XRectangle
    composite_info;

  /*
    Request image file name from user.
  */
  *filename='\0';
  XPopupQuery(display,&window->popup,"File name:",filename);
  XFlush(display);
  if (*filename == '\0')
    return(True);
  /*
    Read image.
  */
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XFlush(display);
  GetAlienInfo(&alien_info);
  (void) strcpy(alien_info.filename,filename);
  alien_info.server_name=resource_info->server_name;
  alien_info.font=resource_info->font;
  composite_image=ReadAlienImage(&alien_info);
  XDefineCursor(display,window->image.id,window->image.cursor);
  if (composite_image == (Image *) NULL)
    {
      XPopupAlert(display,&window->popup,"unable to read image",filename);
      return(False);
    }
  /*
    Map info window.
  */
  state=DefaultState;
  (void) sprintf(text," %+u%+u  \0",window->image.width,window->image.height);
  XSetWindowExtents(window->info,text,2);
  XMapWindow(display,window->info.id);
  state|=InfoMappedState;
  /*
    Track pointer until button 1 is pressed.
  */
  XQueryPointer(display,window->image.id,&xwindow,&xwindow,&i,&i,&x,&y,&mask);
  composite_info.x=x;
  composite_info.y=y;
  composite_info.width=0;
  composite_info.height=0;
  XSelectInput(display,window->image.id,window->image.attributes.event_mask |
    PointerMotionMask);
  do
  {
    if (state & InfoMappedState)
      {
        /*
          Display pointer position.
        */
        (void) sprintf(text," %+d%+d \0",x-window->image.x,y-window->image.y);
        XClearWindow(display,window->info.id);
        XDrawImageString(display,window->info.id,window->info.graphic_context,2,
          window->info.font_info->ascent+2,text,strlen(text));
      }
    CompositeRectangle(composite_info);
    /*
      Wait for next event.
    */
    XWindowEvent(display,window->image.id,ButtonPressMask | ButtonReleaseMask |
      ExposureMask | KeyPressMask | PointerMotionMask,&event);
    CompositeRectangle(composite_info);
    switch (event.type)
    {
      case ButtonPress:
      {
        if ((event.xbutton.button == Button3) &&
            (event.xbutton.state & Mod1Mask))
          {
            /* 
              Convert Alt-Button3 to Button2.
            */
            event.xbutton.button=Button2;
            event.xbutton.state&=(~Mod1Mask);
          }
        switch (event.xbutton.button)
        {
          case Button1:
          {
            composite_info.width=composite_image->columns;
            composite_info.height=composite_image->rows;
            cursor=XCreateFontCursor(display,XC_ul_angle);
            XRecolorCursor(display,cursor,
              &window->image.pixel_info->background_color,
              &window->image.pixel_info->foreground_color);
            XDefineCursor(display,window->image.id,cursor);
            break;
          }
          case Button2:
          {
            static char
              command[2048],
              *CompositeSelections[]=
              {
                "over",
                "in",
                "out",
                "atop",
                "xor",
                "plus",
                "minus",
                "add",
                "subtract",
                "difference",
                "replace",
              };

            /*
              Select a command from the pop-up menu.
            */
            operator=XPopupMenu(display,&window->popup,event.xbutton.x_root,
              event.xbutton.y_root,"Operations",CompositeSelections,
              sizeof(CompositeSelections)/sizeof(CompositeSelections[0]),
              command);
            break;
          }
          default:
            break;
        }
        break;
      }
      case ButtonRelease:
      {
        if (event.xbutton.button == Button1)
          {
            /*
              User has selected the location of the composite image.
            */
            composite_info.x=event.xbutton.x;
            composite_info.y=event.xbutton.y;
            state|=ExitState;
          }
        break;
      }
      case Expose:
      {
        /*
          Refresh image window.
        */
        XRefreshWindow(display,&window->image,&event);
        break;
      }
      case KeyPress:
      {
        static char
          command[2048];

        static KeySym
          key_symbol;

        /*
          Respond to a user key press.
        */
        *command='\0';
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Escape)
          {
            /*
              Prematurely exit.
            */
            DestroyImage(composite_image);
            state|=EscapeState;
            state|=ExitState;
            break;
          }
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending pointer motion events.
        */
        while (XCheckMaskEvent(display,PointerMotionMask,&event));
        x=event.xmotion.x;
        y=event.xmotion.y;
        /*
          Map and unmap info window as text cursor crosses its boundaries.
        */
        if (state & InfoMappedState)
          {
            if ((x < (window->info.x+window->info.width)) &&
                (y < (window->info.y+window->info.height)))
              {
                XWithdrawWindow(display,window->info.id,window->info.screen);
                state&=(~InfoMappedState);
              }
          }
        else
          if ((x > (window->info.x+window->info.width)) ||
              (y > (window->info.y+window->info.height)))
            {
              XMapWindow(display,window->info.id);
              state|=InfoMappedState;
            }
        composite_info.x=x;
        composite_info.y=y;
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XSelectInput(display,window->image.id,window->image.attributes.event_mask);
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XFreeCursor(display,cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  XFlush(display);
  if (state & EscapeState)
    return(True);
  /*
    Image compositing is relative to image configuration.
  */
  x=0;
  y=0;
  width=(*image)->columns;
  height=(*image)->rows;
  if (window->image.clip_geometry != (char *) NULL)
    (void) XParseGeometry(window->image.clip_geometry,&x,&y,&width,&height);
  scale_factor=UpShift(width)/window->image.ximage->width;
  composite_info.x+=window->image.x;
  composite_info.x=DownShift(composite_info.x*scale_factor);
  composite_info.width=DownShift(composite_info.width*scale_factor);
  scale_factor=UpShift(height)/window->image.ximage->height;
  composite_info.y+=window->image.y;
  composite_info.y=DownShift(composite_info.y*scale_factor);
  composite_info.height=DownShift(composite_info.height*scale_factor);
  if ((composite_info.width != composite_image->columns) ||
      (composite_info.height != composite_image->rows))
    {
      Image
        *scaled_image;

      /*
        Scale composite image.
      */
      scaled_image=
        ScaleImage(composite_image,composite_info.width,composite_info.height);
      if (scaled_image == (Image *) NULL)
        {
          XDefineCursor(display,window->image.id,window->image.cursor);
          DestroyImage(composite_image);
          return(False);
        }
      composite_image=scaled_image;
    }
  /*
    Composite image with X image window.
  */
  CompositeImage(*image,operator,composite_image,composite_info.x+x,
    composite_info.y+y);
  XDefineCursor(display,window->image.id,window->image.cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C o n f i g u r e I m a g e W i n d o w                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XConfigureImageWindow creates a new X image.  It's size is
%  determined by the width and height parameters.  If the size does not
%  change, the image is displayed to the X image window.
%
%  The format of the XConfigureImageWindow routine is:
%
%    status=XConfigureImageWindow(display,resource_info,window,width,height,
%      image)
%
%  A description of each parameter follows:
%
%    o status: Function XConfigureImageWindow returns True if the window is
%      resized.  False is returned is there is a memory shortage or if the
%      window fails to resize.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o width: Specifies the new width in pixels of the image.
%
%    o height: Specifies the new height in pixels of the image.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int XConfigureImageWindow(display,resource_info,window,width,
  height,image)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;

unsigned int
  width,
  height;

Image
  *image;
{
  char
    text[2048];

  unsigned int
    status;

  unsigned long int
    state;

  state=DefaultState;
  if ((window->image.width*window->image.height) > MinInfoSize)
    {
      /*
        Map info window.
      */
      (void) strcpy(text," Configuring image... ");
      XSetWindowExtents(window->info,text,2);
      XMapWindow(display,window->info.id);
      XDrawImageString(display,window->info.id,window->info.graphic_context,2,
        window->info.font_info->ascent+2,text,strlen(text));
      state|=InfoMappedState;
    }
  /*
    Resize image to fit image window dimensions.
  */
  if (resource_info->debug)
    (void) fprintf(stderr,"Configure Image: %dx%d=>%ux%u\n",
      window->image.ximage->width,window->image.ximage->height,width,height);
  status=XMakeImage(display,resource_info,&window->image,image,width,height);
  (void) XMakePixmap(display,resource_info,&window->image);
  if (state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y B a c k g r o u n d I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDisplayBackgroundImage displays an image in the root window.
%
%  The format of the XDisplayBackgroundImage routine is:
%
%      XDisplayBackgroundImage(display,resource_info,window_id,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window_id: Specifies a pointer to a string with a window id or name.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%
*/
static void XDisplayBackgroundImage(display,resource_info,window_id,image)
Display
  *display;

XResourceInfo
  *resource_info;

char
  *window_id;

Image
  *image;
{
  Atom
    property,
    type;

  Cursor
    arrow_cursor,
    watch_cursor;

  int
    format;

  Pixmap
    pixmap;

  unsigned char
    *data;

  unsigned int
    height,
    status,
    width;

  unsigned long
    after,
    length;

  Window
    root_window,
    target_window;

  XGCValues
    graphic_context_value;

  XPixelInfo
    pixel_info;

  XStandardColormap
    *map_info;

  XWindowInfo
    window_info;

  XVisualInfo
    *visual_info;

  /*
    Allocate standard colormap.
  */
  map_info=XAllocStandardColormap();
  if (map_info == (XStandardColormap *) NULL)
    Error("unable to create standard colormap","memory allocation failed");
  map_info->colormap=(Colormap) NULL;
  pixel_info.pixels=(unsigned long *) NULL;
  /*
    Initialize visual info.
  */
  if ((resource_info->visual_type != (char *) NULL) ||
      (resource_info->map_type != (char *) NULL))
    visual_info=XBestVisualInfo(display,resource_info->visual_type,
      resource_info->map_type,map_info);
  else
    {
      int
        number_visuals;

      XVisualInfo
        visual_template;

      /*
        Get the default visual.
      */
      visual_template.visualid=
        XVisualIDFromVisual(XDefaultVisual(display,XDefaultScreen(display)));
      visual_info=XGetVisualInfo(display,VisualIDMask,&visual_template,
        &number_visuals);
    }
  if (visual_info == (XVisualInfo *) NULL)
    Error("unable to get visual",resource_info->visual_type);
  if (visual_info->visual != XDefaultVisual(display,visual_info->screen))
    Error("visual must be server default",resource_info->visual_type);
  /*
    If there are previous resources on the root window, destroy them.
  */
  root_window=XRootWindow(display,visual_info->screen);
  property=XInternAtom(display,"_XSETROOT_ID",False);
  if (property == (Atom) NULL)
    Error("unable to create X property","_XSETROOT_ID");
  (void) XGetWindowProperty(display,root_window,property,0L,1L,True,
    (Atom) AnyPropertyType,&type,&format,&length,&after,&data);
  if ((type == XA_PIXMAP) && (format == 32) && (length == 1) && (after == 0))
    {
      /*
        Free previous resources on the root window.
      */
      XKillClient(display,(XID) (*((Pixmap *) data)));
      XFree((void *) data);
    }
  /*
    Initialize colormap.
  */
  XMakeStandardColormap(display,visual_info,resource_info,&pixel_info,image,
    map_info);
  if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
    Error("unable to display X image on the root window","too many colors");
  /*
    Determine target window.
  */
  target_window=(Window) NULL;
  if (Latin1Compare(window_id,"root") == 0)
    target_window=root_window;
  else
    {
      if (isdigit(*window_id))
        target_window=XWindowByID(display,root_window,
          (Window) strtol(window_id,(char **) NULL,0));
      if (target_window == (Window) NULL)
        target_window=XWindowByName(display,root_window,window_id);
      if (target_window == (Window) NULL)
        Error("No window with specified id exists",window_id);
    }
  /*
    Initialize cursor.
  */
  arrow_cursor=XCreateFontCursor(display,XC_arrow);
  watch_cursor=XCreateFontCursor(display,XC_watch);
  if ((arrow_cursor == (Cursor) NULL) || (watch_cursor == (Cursor) NULL))
    Error("unable to create cursor",(char *) NULL);
  XRecolorCursor(display,arrow_cursor,&pixel_info.background_color,
    &pixel_info.foreground_color);
  XRecolorCursor(display,watch_cursor,&pixel_info.background_color,
    &pixel_info.foreground_color);
  /*
    Initialize image window attributes.
  */
  window_info.id=target_window;
  window_info.visual_info=visual_info;
  window_info.screen=visual_info->screen;
  window_info.depth=visual_info->depth;
  window_info.clip_geometry=(char *) NULL;
  window_info.map_info=map_info;
  window_info.pixel_info=(&pixel_info);
  window_info.cursor=arrow_cursor;
  window_info.busy_cursor=watch_cursor;
  window_info.x=0;
  window_info.y=0;
  window_info.width=image->columns;
  if (window_info.width > XDisplayWidth(display,visual_info->screen))
    window_info.width=XDisplayWidth(display,visual_info->screen);
  window_info.height=image->rows;
  if (window_info.height > XDisplayHeight(display,visual_info->screen))
    window_info.height=XDisplayHeight(display,visual_info->screen);
  window_info.border_width=resource_info->border_width;
  window_info.ximage=(XImage *) NULL;
  window_info.pixmap=(Pixmap) NULL;
  /*
    Graphic context superclass.
  */
  graphic_context_value.background=pixel_info.background_color.pixel;
  graphic_context_value.foreground=pixel_info.foreground_color.pixel;
  graphic_context_value.fill_style=FillSolid;
  graphic_context_value.function=GXcopy;
  graphic_context_value.plane_mask=AllPlanes;
  window_info.graphic_context=XCreateGC(display,window_info.id,
    GCBackground | GCFillStyle | GCForeground | GCFunction | GCPlaneMask,
    &graphic_context_value);
  if (window_info.graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  graphic_context_value.background=pixel_info.foreground_color.pixel;
  graphic_context_value.foreground=pixel_info.background_color.pixel;
  window_info.highlight_context=XCreateGC(display,window_info.id,
    GCBackground | GCForeground | GCFunction | GCPlaneMask,
    &graphic_context_value);
  if (window_info.highlight_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  /*
    Create the X image.
  */
  status=XMakeImage(display,resource_info,&window_info,image,image->columns,
    image->rows);
  if (status == False)
    Error("unable to create X image",(char *) NULL);
  /*
    Adjust image dimensions as specified by backdrop or geometry options.
  */
  width=window_info.width;
  height=window_info.height;
  if (resource_info->backdrop)
    {
      /*
        Center image on root window.
      */
      window_info.x=
        XDisplayWidth(display,visual_info->screen)/2-image->columns/2;
      window_info.y=
        XDisplayHeight(display,visual_info->screen)/2-image->rows/2;
      width=XDisplayWidth(display,visual_info->screen);
      height=XDisplayHeight(display,visual_info->screen);
    }
  if (resource_info->image_geometry != (char *) NULL)
    {
      char
        default_geometry[2048];

      int
        flags,
        gravity;

      XSizeHints
        *size_hints;

      /*
        User specified geometry.
      */
      size_hints=XAllocSizeHints();
      if (size_hints == (XSizeHints *) NULL)
        Error("unable to display on root","memory allocation failed");
      size_hints->flags=(long int) NULL;
      (void) sprintf(default_geometry,"%ux%u\0",width,height);
      flags=XWMGeometry(display,visual_info->screen,
        resource_info->image_geometry,default_geometry,
        window_info.border_width,size_hints,&window_info.x,&window_info.y,
        (int *) &width,(int *) &height,&gravity);
      if (flags & (XValue | YValue))
        {
          width=XDisplayWidth(display,visual_info->screen);
          height=XDisplayHeight(display,visual_info->screen);
        }
      XFree((void *) size_hints);
    }
  /*
    Create the root pixmap.
  */
  pixmap=XCreatePixmap(display,window_info.id,width,height,window_info.depth);
  if (pixmap == (Pixmap) NULL)
    Error("unable to create X pixmap",(char *) NULL);
  /*
    Display pixmap on the root window.
  */
  if ((width > window_info.width) || (height > window_info.height))
    XFillRectangle(display,pixmap,window_info.highlight_context,0,0,width,
      height);
  XPutImage(display,pixmap,window_info.graphic_context,window_info.ximage,
    0,0,window_info.x,window_info.y,window_info.width,window_info.height);
  XSetWindowBackgroundPixmap(display,window_info.id,pixmap);
  XClearWindow(display,window_info.id);
  /*
    Free resources.
  */
  XFreePixmap(display,pixmap);
  XDestroyImage(window_info.ximage);
  XFreeGC(display,window_info.graphic_context);
  XFreeGC(display,window_info.highlight_context);
  XFreeCursor(display,arrow_cursor);
  XFreeCursor(display,watch_cursor);
  XFree((void *) visual_info);
  XFree((void *) map_info);
  if (pixel_info.pixels != (unsigned long *) NULL)
    (void) free((char *) pixel_info.pixels);
  /*
    Put property on root window and set close-down mode to RetainPermanent.
  */
  pixmap=XCreatePixmap(display,root_window,1,1,1);
  if (pixmap == (Pixmap) NULL)
    Error("unable to create X pixmap",(char *) NULL);
  XChangeProperty(display,root_window,property,XA_PIXMAP,32,PropModeReplace,
    (unsigned char *) &pixmap,1);
  XSetCloseDownMode(display,RetainPermanent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D i s p l a y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDisplayImage displays an image via X11.  A new image is created
%  and returned if the user interactively transforms the displayed image.
%
%  The format of the XDisplayImage routine is:
%
%      tile_image=XDisplayImage(display,resource_info,argv,argc,image,state)
%
%  A description of each parameter follows:
%
%    o tile_image:  If the image to display is a composite image and the
%      user selects a particular tile to display, XDisplayImage returns
%      the selected tile.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o argv: Specifies the application's argument list.
%
%    o argc: Specifies the number of arguments.
%
%    o image: Specifies an address to an address of an Image structure;
%      returned from ReadImage.
%
%
*/
static Image *XDisplayImage(display,resource_info,argv,argc,image,state)
Display
  *display;

XResourceInfo
  *resource_info;

char
  **argv;

int
  argc;

Image
  **image;

unsigned long int
  *state;
{
#define MagnifySize  256  /* must be a power of 2 */
#define MaxWindows  9

  char
    command[2048],
    text[2048];

  GC
    graphic_context;

  Image
    *displayed_image,
    *tile_image;

  KeySym
    key_symbol;

  register int
    i;

  static Atom
    delete_property,
    protocols_property;

  static Cursor
    arrow_cursor,
    watch_cursor;

  static XClassHint
    *class_hint;

  static XPixelInfo
    pixel_info;

  static XStandardColormap
    *map_info;

  static XVisualInfo
    *visual_info = (XVisualInfo *) NULL;

  static XWindowInfo
    *magick_windows[MaxWindows];

  static XWindows
    *window;

  static XWMHints
    *manager_hints;

  static unsigned int
    number_windows;

  unsigned int
    status;

  unsigned long
    timeout;

  Window
    root_window;

  XEvent
    event;

  XFontStruct
    *font_info;

  XGCValues
    graphic_context_value;

  XWindowInfo
    previous_window;

  if (visual_info == (XVisualInfo *) NULL)
    {
      /*
        Allocate standard colormap.
      */
      if (resource_info->debug)
        XSynchronize(display,True);
      map_info=XAllocStandardColormap();
      if (map_info == (XStandardColormap *) NULL)
        Error("unable to create standard colormap","memory allocation failed");
      map_info->colormap=(Colormap) NULL;
      pixel_info.pixels=(unsigned long *) NULL;
      /*
        Allocate visual.
      */
      visual_info=XBestVisualInfo(display,resource_info->visual_type,
        resource_info->map_type,map_info);
      if (visual_info == (XVisualInfo *) NULL)
        Error("unable to get visual",resource_info->visual_type);
      if (resource_info->debug)
        {
          (void) fprintf(stderr,"Visual:\n");
          (void) fprintf(stderr,"  visual id: 0x%lx\n",visual_info->visualid);
          (void) fprintf(stderr,"  class: %s\n",XVisualClassName(visual_info));
          (void) fprintf(stderr,"  depth: %d planes\n",visual_info->depth);
          (void) fprintf(stderr,"  size of colormap: %d entries\n",
            visual_info->colormap_size);
          (void) fprintf(stderr,"  red, green, blue masks: 0x%lx 0x%lx 0x%lx\n",
            visual_info->red_mask,visual_info->green_mask,
            visual_info->blue_mask);
          (void) fprintf(stderr,"  significant bits in color: %d bits\n",
            visual_info->bits_per_rgb);
        }
      /*
        Allocate cursors.
      */
      arrow_cursor=XCreateFontCursor(display,XC_arrow);
      watch_cursor=XCreateFontCursor(display,XC_watch);
      if ((arrow_cursor == (Cursor) NULL) || (watch_cursor == (Cursor) NULL))
        Error("unable to create cursor",(char *) NULL);
      /*
        Allocate atoms.
      */
      protocols_property=XInternAtom(display,"WM_PROTOCOLS",False);
      delete_property=XInternAtom(display,"WM_DELETE_WINDOW",False);
      if ((protocols_property == (Atom) NULL) ||
          (delete_property == (Atom) NULL))
        Error("unable to create property",(char *) NULL);
      /*
        Allocate class and manager hints.
      */
      class_hint=XAllocClassHint();
      manager_hints=XAllocWMHints();
      if ((class_hint == (XClassHint *) NULL) ||
          (manager_hints == (XWMHints *) NULL))
        Error("unable to allocate X hints",(char *) NULL);
      /*
        Initialize window id's.
      */
      window=(XWindows *) malloc(sizeof(XWindows));
      if (window == (XWindows *) NULL)
        Error("unable to create X windows","memory allocation failed");
      number_windows=0;
      magick_windows[number_windows++]=(&window->backdrop);
      magick_windows[number_windows++]=(&window->icon);
      magick_windows[number_windows++]=(&window->image);
      magick_windows[number_windows++]=(&window->info);
      magick_windows[number_windows++]=(&window->magnify);
      magick_windows[number_windows++]=(&window->pan);
      magick_windows[number_windows++]=(&window->popup);
      for (i=0; i < number_windows; i++)
        magick_windows[i]->id=(Window) NULL;
    }
  /*
    Initialize Standard Colormap.
  */
  displayed_image=(*image);
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"Image: [%u] %s %ux%u ",displayed_image->scene,
        displayed_image->filename,displayed_image->columns,
        displayed_image->rows);
      if (displayed_image->colors > 0)
        (void) fprintf(stderr,"%uc ",displayed_image->colors);
      (void) fprintf(stderr,"%s\n",displayed_image->magick);
    }
  XMakeStandardColormap(display,visual_info,resource_info,&pixel_info,
    displayed_image,map_info);
  /*
    Color cursor.
  */
  XRecolorCursor(display,arrow_cursor,&pixel_info.background_color,
    &pixel_info.foreground_color);
  XRecolorCursor(display,watch_cursor,&pixel_info.background_color,
    &pixel_info.foreground_color);
  /*
    Initialize font info.
  */
  (void) sprintf(text," [%u] %s %ux%u %s \0",displayed_image->scene,
    displayed_image->filename,displayed_image->columns,displayed_image->rows,
    XVisualClassName(visual_info));
  if (displayed_image->colors > 0)
    (void) sprintf(text,"%s%uc \0",text,displayed_image->colors);
  font_info=XBestFont(display,resource_info,text,displayed_image->columns);
  if (font_info == (XFontStruct *) NULL)
    Error("unable to load font",resource_info->font);
  /*
    Initialize class and manager hints.
  */
  if (resource_info->name == (char *) NULL)
    class_hint->res_name=client_name;
  else
    class_hint->res_name=resource_info->name;
  class_hint->res_class=(char *) "ImageMagick";
  manager_hints->flags=InputHint | StateHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  /*
    Window superclass.
  */
  window->superclass.id=(Window) NULL;
  window->superclass.screen=visual_info->screen;
  window->superclass.depth=visual_info->depth;
  window->superclass.visual_info=visual_info;
  window->superclass.map_info=map_info;
  window->superclass.pixel_info=(&pixel_info);
  window->superclass.font_info=font_info;
  window->superclass.cursor=arrow_cursor;
  window->superclass.busy_cursor=watch_cursor;
  window->superclass.graphic_context=(GC) NULL;
  window->superclass.name=(char *) NULL;
  window->superclass.geometry=(char *) NULL;
  window->superclass.icon_name=(char *) NULL;
  window->superclass.icon_geometry=resource_info->icon_geometry;
  window->superclass.clip_geometry=(char *) NULL;
  window->superclass.flags=PSize;
  window->superclass.x=0;
  window->superclass.y=0;
  window->superclass.width=1;
  window->superclass.height=1;
  window->superclass.min_width=1;
  window->superclass.min_height=1;
  window->superclass.width_inc=1;
  window->superclass.height_inc=1;
  window->superclass.border_width=2;
  window->superclass.immutable=True;
  window->superclass.ximage=(XImage *) NULL;
  window->superclass.pixmap=(Pixmap) NULL;
  window->superclass.mask=CWBackingStore | CWBackPixel | CWBackPixmap |
    CWBitGravity | CWBorderPixel | CWColormap | CWCursor | CWDontPropagate |
    CWEventMask | CWOverrideRedirect | CWSaveUnder | CWWinGravity;
  window->superclass.attributes.background_pixel=
    pixel_info.background_color.pixel;
  window->superclass.attributes.background_pixmap=(Pixmap) NULL;
  window->superclass.attributes.backing_store=WhenMapped;
  window->superclass.attributes.bit_gravity=ForgetGravity;
  window->superclass.attributes.border_pixel=pixel_info.border_color.pixel;
  window->superclass.attributes.colormap=map_info->colormap;
  window->superclass.attributes.cursor=arrow_cursor;
  window->superclass.attributes.do_not_propagate_mask=NoEventMask;
  window->superclass.attributes.event_mask=NoEventMask;
  window->superclass.attributes.override_redirect=False;
  window->superclass.attributes.save_under=True;
  window->superclass.attributes.win_gravity=NorthWestGravity;
  root_window=XRootWindow(display,visual_info->screen);
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&window->superclass);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (superclass)\n",
      window->superclass.id);
  /*
    Initialize graphic context.
  */
  graphic_context_value.background=pixel_info.background_color.pixel;
  graphic_context_value.foreground=pixel_info.foreground_color.pixel;
  graphic_context_value.font=font_info->fid;
  graphic_context_value.function=GXcopy;
  graphic_context_value.line_width=2;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context=XCreateGC(display,window->superclass.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCLineWidth | GCPlaneMask,
    &graphic_context_value);
  if (graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  window->superclass.graphic_context=graphic_context;
  graphic_context_value.background=pixel_info.foreground_color.pixel;
  graphic_context_value.foreground=pixel_info.background_color.pixel;
  graphic_context=XCreateGC(display,window->superclass.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCLineWidth | GCPlaneMask,
    &graphic_context_value);
  if (graphic_context == (GC) NULL)
    Error("unable to create graphic context",(char *) NULL);
  window->superclass.highlight_context=graphic_context;
  XDestroyWindow(display,window->superclass.id);
  window->superclass.id=(Window) NULL;
  /*
    Initialize icon window.
  */
  XGetWindowInfo(&window->superclass,&window->icon);
  XBestIconSize(display,&window->icon,displayed_image);
  window->icon.attributes.event_mask=StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint;
  manager_hints->input=False;
  manager_hints->initial_state=IconicState;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&window->icon);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (icon)\n",window->icon.id);
  /*
    Initialize image window.
  */
  previous_window=window->image;
  XGetWindowInfo(&window->superclass,&window->image);
  window->image.name=(char *) malloc(2048*sizeof(char));
  window->image.icon_name=(char *) malloc(2048*sizeof(char));
  if ((window->image.name == NULL) || (window->image.icon_name == NULL))
    Error("unable to create image window","memory allocation failed");
  if (resource_info->title != (char *) NULL)
    {
      (void) strcpy(window->image.name,resource_info->title);
      (void) strcpy(window->image.icon_name,resource_info->title);
    }
  else
    {
      char
        *icon_name;

      (void) strcpy(window->image.name,"ImageMagick: ");
      (void) strcat(window->image.name,displayed_image->filename);
      icon_name=displayed_image->filename+strlen(displayed_image->filename)-1;
      while ((icon_name > displayed_image->filename) && (*(icon_name-1) != '/'))
        icon_name--;
      (void) strcpy(window->image.icon_name,icon_name);
    }
  window->image.geometry=resource_info->image_geometry;
  window->image.width=displayed_image->columns;
  if (window->image.width > XDisplayWidth(display,visual_info->screen))
    window->image.width=(XDisplayWidth(display,visual_info->screen)*7) >> 3;
  window->image.height=displayed_image->rows;
  if (window->image.height > XDisplayHeight(display,visual_info->screen))
    window->image.height=(XDisplayHeight(display,visual_info->screen)*7) >> 3;
  window->image.border_width=resource_info->border_width;
  XGetWindowInfo(&window->superclass,&window->backdrop);
  if (resource_info->backdrop || (window->backdrop.id != (Window) NULL))
    {
      unsigned int
        height,
        width;

      /*
        Initialize backdrop window.
      */
      window->backdrop.cursor=XMakeInvisibleCursor(display,root_window);
      if (window->backdrop.cursor == (Cursor) NULL)
        Error("unable to create cursor",(char *) NULL);
      window->backdrop.name="ImageMagick Backdrop";
      window->backdrop.flags=USSize | USPosition;
      window->backdrop.width=XDisplayWidth(display,visual_info->screen);
      window->backdrop.height=XDisplayHeight(display,visual_info->screen);
      window->backdrop.border_width=0;
      window->backdrop.attributes.cursor=window->backdrop.cursor;
      window->backdrop.attributes.do_not_propagate_mask=ButtonPressMask |
        ButtonReleaseMask;
      window->backdrop.attributes.event_mask=KeyPressMask | KeyReleaseMask;
      window->backdrop.attributes.override_redirect=True;
      manager_hints->flags=IconWindowHint | InputHint | StateHint;
      manager_hints->icon_window=window->icon.id;
      manager_hints->input=True;
      manager_hints->initial_state=
        resource_info->iconic ? IconicState : NormalState;
      XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
        delete_property,&window->backdrop);
      if (resource_info->debug)
        (void) fprintf(stderr,"Window id: 0x%lx (backdrop)\n",
          window->backdrop.id);
      XMapWindow(display,window->backdrop.id);
      XClearWindow(display,window->backdrop.id);
      /*
        Position image in the center the backdrop.
      */
      window->image.flags|=USPosition;
      window->image.x=0;
      width=displayed_image->columns+window->image.border_width;
      if (width > XDisplayWidth(display,window->image.screen))
        width=(XDisplayWidth(display,window->image.screen)*7) >> 3;
      window->image.x=XDisplayWidth(display,visual_info->screen)/2-width/2;
      window->image.y=0;
      height=displayed_image->rows+window->image.border_width;
      if (height > XDisplayHeight(display,window->image.screen))
        height=(XDisplayHeight(display,window->image.screen)*7) >> 3;
      window->image.y=XDisplayHeight(display,visual_info->screen)/2-height/2;
      if (window->image.id != (Window) NULL)
        {
          XDestroyWindow(display,window->image.id);
          window->image.id=(Window) NULL;
        }
    }
  window->image.immutable=False;
  window->image.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | OwnerGrabButtonMask |
    StructureNotifyMask;
  manager_hints->flags=IconWindowHint | InputHint | StateHint;
  manager_hints->icon_window=window->icon.id;
  manager_hints->input=True;
  manager_hints->initial_state=
    resource_info->iconic ? IconicState : NormalState;
  XMakeWindow(display,
    (resource_info->backdrop ? window->backdrop.id : root_window),argv,argc,
    class_hint,manager_hints,delete_property,&window->image);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (image)\n",window->image.id);
  if (window->backdrop.id == (Window) NULL)
    *state|=ReconfigureImageState;
  window->image.x=0;
  window->image.y=0;
  /*
    Initialize X image structure.
  */
  status=XMakeImage(display,resource_info,&window->image,displayed_image,
    displayed_image->columns,displayed_image->rows);
  (void) XMakePixmap(display,resource_info,&window->image);
  if (status == False)
    Error("unable to create X image",(char *) NULL);
  XMapWindow(display,window->image.id);
  /*
    Initialize magnify window and cursor.
  */
  XGetWindowInfo(&window->superclass,&window->magnify);
  window->magnify.name=(char *) malloc(2048*sizeof(char));
  if (window->magnify.name == NULL)
    Error("unable to create magnify window","memory allocation failed");
  (void) sprintf(window->magnify.name,"Magnify %uX",resource_info->magnify);
  window->magnify.cursor=XMakeCursor(display,window->image.id,
    map_info->colormap,resource_info->background_color,
    resource_info->foreground_color);
  if (window->magnify.cursor == (Cursor) NULL)
    Error("unable to create cursor",(char *) NULL);
  XRecolorCursor(display,window->magnify.cursor,&pixel_info.background_color,
    &pixel_info.foreground_color);
  window->magnify.width=MagnifySize;
  window->magnify.height=MagnifySize;
  window->magnify.min_width=MagnifySize;
  window->magnify.min_height=MagnifySize;
  window->magnify.width_inc=MagnifySize;
  window->magnify.height_inc=MagnifySize;
  window->magnify.immutable=False;
  window->magnify.attributes.cursor=window->magnify.cursor;
  window->magnify.attributes.event_mask=ExposureMask | KeyPressMask |
    StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=window->image.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&window->magnify);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (magnify)\n",window->magnify.id);
  /*
    Initialize panning window.
  */
  XGetWindowInfo(&window->superclass,&window->pan);
  window->pan.name="Pan Icon";
  XBestIconSize(display,&window->pan,displayed_image);
  window->pan.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | StructureNotifyMask | VisibilityChangeMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=window->image.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&window->pan);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (pan)\n",window->pan.id);
  /*
    Initialize popup window.
  */
  XGetWindowInfo(&window->superclass,&window->popup);
  window->popup.name="ImageMagick Popup";
  window->popup.flags=PSize | PPosition;
  window->popup.attributes.override_redirect=True;
  window->popup.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | LeaveWindowMask |
    OwnerGrabButtonMask | VisibilityChangeMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=True;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=window->image.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&window->popup);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (pop up)\n",window->popup.id);
  XSetTransientForHint(display,window->popup.id,window->image.id);
  /*
    Initialize info window.
  */
  XGetWindowInfo(&window->superclass,&window->info);
  window->info.name="ImageMagick Info";
  window->info.flags=PSize | PPosition;
  window->info.x=2;
  window->info.y=2;
  window->info.attributes.event_mask=StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=window->image.id;
  XMakeWindow(display,window->image.id,argv,argc,class_hint,manager_hints,
    delete_property,&window->info);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (info)\n",window->info.id);
  if (*state & ImageMappedState)
    {
      /*
        Image window is already mapped-- refresh window, map pan window.
      */
      if ((window->image.width == previous_window.width) &&
          (window->image.height == previous_window.height))
        {
          XRefreshWindow(display,&window->image,(XEvent *) NULL);
          *state&=(~ReconfigureImageState);
        }
      if ((window->image.width < window->image.ximage->width) ||
          (window->image.height < window->image.ximage->height))
        XMapRaised(display,window->pan.id);
    }
  /*
    Respond to events.
  */
  *state&=(~LastImageState);
  *state&=(~NextImageState);
  *state&=(~TileImageState);
  timeout=(unsigned long) (~0);
  do
  {
    /*
      Handle a window event.
    */
    if (resource_info->delay > 0)
      if (XEventsQueued(display,QueuedAfterFlush) == 0)
        {
          /*
            Block if delay > 0.
          */
          (void) sleep(1);
          continue;
        }
    XNextEvent(display,&event);
    switch (event.type)
    {
      case ButtonPress:
      {
        if ((event.xbutton.button == Button3) &&
            (event.xbutton.state & Mod1Mask))
          {
            /* 
              Convert Alt-Button3 to Button2.
            */
            event.xbutton.button=Button2;
            event.xbutton.state&=(~Mod1Mask);
          }
        if (event.xbutton.window == window->image.id)
          switch (event.xbutton.button)
          {
            case Button1:
            {
              static char
                *MenuCommand="ir/\\<>oacwpnlq",
                *MenuSelections[]=
                {
                  "Image Info",
                  "Reflect",
                  "Rotate Right",
                  "Rotate Left",
                  "Half Size",
                  "Double Size",
                  "Restore",
                  "Annotate",
                  "Composite",
                  "Write",
                  "Print",
                  "Next",
                  "Last",
                  "Quit"
                };

              static int
                command_number;

              /*
                Select a command from the pop-up menu.
              */
              command_number=XPopupMenu(display,&window->popup,
                event.xbutton.x_root,event.xbutton.y_root,"Commands",
                MenuSelections,sizeof(MenuSelections)/sizeof(MenuSelections[0]),
                command);
              if (*command != '\0')
                UserCommand(display,resource_info,window,
                  MenuCommand[command_number],&displayed_image,state);
              break;
            }
            case Button2:
            {
              /*
                User pressed the image clip button.
              */
              XClipImageWindow(display,resource_info,window,&event,
                displayed_image,state);
              break;
            }
            case Button3:
            {
              if (displayed_image->montage != (char *) NULL)
                {
                  /*
                    User picked an image tile to display.
                  */
                  tile_image=XTileImageWindow(display,resource_info,window,
                    displayed_image,&event);
                  if (tile_image != (Image *) NULL)
                    *state|=TileImageState | NextImageState | ExitState;
                  break;
                }
              /*
                User pressed the image magnify button.
              */
              if (*state & MagnifyMappedState)
                XRaiseWindow(display,window->magnify.id);
              else
                {
                  /*
                    Make magnify image.
                  */
                  status=XMakeImage(display,resource_info,&window->magnify,
                    (Image *) NULL,window->magnify.width,
                    window->magnify.height);
                  status|=XMakePixmap(display,resource_info,&window->magnify);
                  if (status == False)
                    Error("unable to create magnify image",(char *) NULL);
                  XMapRaised(display,window->magnify.id);
                }
              XMagnifyImageWindow(display,resource_info,window,&event);
              break;
            }
            default:
              break;
          }
        if (event.xbutton.window == window->pan.id)
          {
            XPanImageWindow(display,window,&event);
            break;
          }
        break;
      }
      case ClientMessage:
      {
        /*
          If client window delete message, exit.
        */
        if (event.xclient.message_type == protocols_property)
          if (*event.xclient.data.l == delete_property)
            if (event.xclient.window == window->image.id)
              *state|=ExitState;
            else
              XWithdrawWindow(display,event.xclient.window,visual_info->screen);
        break;
      }
      case ConfigureNotify:
      {
        if (resource_info->debug)
          (void) fprintf(stderr,"Configure Notify: 0x%lx %dx%d+%d+%d\n",
            event.xconfigure.window,event.xconfigure.width,
            event.xconfigure.height,event.xconfigure.x,event.xconfigure.y);
        if (event.xconfigure.window == window->image.id)
          {
            /*
              Image window has a new configuration.
            */
            window->image.x=0;
            window->image.y=0;
            if ((event.xconfigure.width != window->image.width) ||
                (event.xconfigure.height != window->image.height))
              {
                window->image.width=event.xconfigure.width;
                window->image.height=event.xconfigure.height;
                if (!(*state & ReconfigureImageState))
                  {
                    status=XConfigureImageWindow(display,resource_info,window,
                      window->image.width,window->image.height,displayed_image);
                    if (status == False)
                      XPopupAlert(display,&window->popup,
                        "unable to configure image",window->image.name);
                  }
                *state|=UpdateConfigurationState;
              }
            *state&=(~ReconfigureImageState);
            break;
          }
        if (event.xconfigure.window == window->magnify.id)
          {
            unsigned int
              magnify;

            /*
              Magnify window has a new configuration.
            */
            window->magnify.width=event.xconfigure.width;
            window->magnify.height=event.xconfigure.height;
            if (!(*state & MagnifyMappedState))
              break;
            magnify=1;
            while (magnify <= event.xconfigure.width)
              magnify<<=1;
            while (magnify <= event.xconfigure.height)
              magnify<<=1;
            magnify>>=1;
            if ((magnify != event.xconfigure.width) ||
                (magnify != event.xconfigure.height))
              {
                XResizeWindow(display,window->magnify.id,magnify,magnify);
                break;
              }
            status=XMakeImage(display,resource_info,&window->magnify,
              (Image *) NULL,window->magnify.width,window->magnify.height);
            status|=XMakePixmap(display,resource_info,&window->magnify);
            if (status == False)
              Error("unable to create magnify image",(char *) NULL);
            XMakeMagnifyImage(display,resource_info,window);
            break;
          }
        if (event.xconfigure.window == window->pan.id)
          {
            /*
              Pan window has a new configuration.
            */
            window->pan.width=event.xconfigure.width;
            window->pan.height=event.xconfigure.height;
            break;
          }
        if (event.xconfigure.window == window->icon.id)
          {
            /*
              Icon window has a new configuration.
            */
            window->icon.width=event.xconfigure.width;
            window->icon.height=event.xconfigure.height;
          }
        break;
      }
      case EnterNotify:
      {
        /*
          Selectively install colormap.
        */
        if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
          if (event.xcrossing.mode != NotifyUngrab)
            XInductColormap(display,map_info->colormap);
        break;
      }
      case Expose:
      {
        if (resource_info->debug)
          (void) fprintf(stderr,"Expose: 0x%lx %dx%d+%d+%d\n",
            event.xexpose.window,event.xexpose.width,event.xexpose.height,
            event.xexpose.x,event.xexpose.y);
        /*
          Refresh windows that are now exposed.
        */
        if (event.xexpose.window == window->image.id)
          if (*state & ImageMappedState)
            {
              XRefreshWindow(display,&window->image,&event);
              /*
                Reset timeout after expose.
              */
              if (resource_info->delay != 0)
                timeout=(unsigned long) time((time_t *) 0)+resource_info->delay;
              break;
            }
        if (event.xexpose.window == window->magnify.id)
          if (event.xexpose.count == 0)
            if (*state & MagnifyMappedState)
              {
                XMakeMagnifyImage(display,resource_info,window);
                break;
              }
        if (event.xexpose.window == window->pan.id)
          if (event.xexpose.count == 0)
            if (*state & PanIconMappedState)
              {
                XDrawPanRectangle(display,window);
                break;
              }
        break;
      }
      case KeyPress:
      {
        if (*state & ReconfigureImageState)
          {
            /*
              No key press during image reconfiguration.
            */
            XBell(display,0);
            break;
          }
        /*
          Respond to a user key press.
        */
        *command='\0';
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Control_L)
          *state|=ControlState;
        else
          if (key_symbol == XK_Help)
            Usage(False);
          else
            if (IsCursorKey(key_symbol) || isdigit(*command))
              {
                /*
                  User specified a magnify factor or position.
                */
                if (key_symbol == XK_Home)
                  {
                    window->magnify.x=window->image.width >> 1;
                    window->magnify.y=window->image.height >> 1;
                  }
                if (key_symbol == XK_Left)
                  if (window->magnify.x > 0)
                    window->magnify.x--;
                if (key_symbol == XK_Up)
                  if (window->magnify.y > 0)
                    window->magnify.y--;
                if (key_symbol == XK_Right)
                  if (window->magnify.x < (window->image.width-1))
                    window->magnify.x++;
                if (key_symbol == XK_Down)
                  if (window->magnify.y < (window->image.height-1))
                    window->magnify.y++;
                if (isdigit(*command))
                  resource_info->magnify=atoi(command);
                if (*state & MagnifyMappedState)
                  XMakeMagnifyImage(display,resource_info,window);
              }
            else
              if (event.xkey.window == window->image.id)
                UserCommand(display,resource_info,window,*command,
                  &displayed_image,state);
        break;
      }
      case KeyRelease:
      {
        /*
          Respond to a user key release.
        */
        *command='\0';
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Control_L)
          *state&=(~ControlState);
        break;
      }
      case LeaveNotify:
      {
        /*
          Selectively uninstall colormap.
        */
        if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
          if (event.xcrossing.mode != NotifyUngrab)
            XUninductColormap(display,map_info->colormap);
        break;
      }
      case MapNotify:
      {
        if (resource_info->debug)
          (void) fprintf(stderr,"Map Notify: 0x%lx\n",event.xmap.window);
        if (event.xmap.window == window->image.id)
          {
            if (window->backdrop.id != (Window) NULL)
              {
                /*
                  Install colormap and set input focus for backdrop window.
                */
                XInstallColormap(display,map_info->colormap);
                XSetInputFocus(display,window->image.id,RevertToNone,
                  CurrentTime);
              }
            if ((window->image.width < window->image.ximage->width) ||
                (window->image.height < window->image.ximage->height))
              XMapRaised(display,window->pan.id);
            *state|=ImageMappedState;
            break;
          }
        if (event.xmap.window == window->magnify.id)
          {
            *state|=MagnifyMappedState;
            break;
          }
        if (event.xmap.window == window->pan.id)
          {
            /*
              Create a panning image.
            */
            XDefineCursor(display,window->image.id,window->image.busy_cursor);
            XFlush(display);
            status=XMakeImage(display,resource_info,&window->pan,
              displayed_image,window->pan.width,window->pan.height);
            status|=XMakePixmap(display,resource_info,&window->pan);
            if (status == False)
              Error("unable to create pan icon image",(char *) NULL);
            XSetWindowBackgroundPixmap(display,window->pan.id,
              window->pan.pixmap);
            XClearWindow(display,window->pan.id);
            XDrawPanRectangle(display,window);
            XDefineCursor(display,window->image.id,window->image.cursor);
            *state|=PanIconMappedState;
            break;
          }
        if (event.xmap.window == window->info.id)
          {
            *state|=InfoMappedState;
            break;
          }
        if (event.xmap.window == window->icon.id)
          {
            if (resource_info->colormap == PrivateColormap)
              if ((visual_info->class == GrayScale) ||
                  (visual_info->class == PseudoColor))
                {
                  /*
                    Icons look best with a shared colormap.
                  */
                  resource_info->colormap=SharedColormap;
                  XMakeStandardColormap(display,visual_info,resource_info,
                    &pixel_info,displayed_image,map_info);
                  *state|=UpdateColormapState;
                  resource_info->colormap=PrivateColormap;
                }
            /*
              Create an icon image.
            */
            status=XMakeImage(display,resource_info,&window->icon,
              displayed_image,window->icon.width,window->icon.height);
            status|=XMakePixmap(display,resource_info,&window->icon);
            if (status == False)
              Error("unable to create icon image",(char *) NULL);
            XSetWindowBackgroundPixmap(display,window->icon.id,
              window->icon.pixmap);
            XClearWindow(display,window->icon.id);
            break;
          }
        break;
      }
      case MappingNotify:
      {
        XRefreshKeyboardMapping(&event.xmapping);
        break;
      }
      case NoExpose:
        break;
      case ReparentNotify:
      {
        if (resource_info->debug)
          (void) fprintf(stderr,"Reparent Notify: 0x%lx=>0x%lx\n",
            event.xreparent.parent,event.xreparent.window);
        break;
      }
      case UnmapNotify:
      {
        if (resource_info->debug)
          (void) fprintf(stderr,"Unmap Notify: 0x%lx\n",event.xunmap.window);
        if (event.xunmap.window == window->image.id)
          {
            *state&=(~ImageMappedState);
            if (*state & PanIconMappedState)
              XWithdrawWindow(display,window->pan.id,window->pan.screen);
            if (*state & MagnifyMappedState)
              XWithdrawWindow(display,window->magnify.id,
                window->magnify.screen);
            break;
          }
        if (event.xunmap.window == window->magnify.id)
          {
            *state&=(~MagnifyMappedState);
            break;
          }
        if (event.xunmap.window == window->pan.id)
          {
            *state&=(~PanIconMappedState);
            break;
          }
        if (event.xunmap.window == window->info.id)
          {
            *state&=(~InfoMappedState);
            break;
          }
        if (event.xunmap.window == window->icon.id)
          if (resource_info->colormap == PrivateColormap)
            if ((visual_info->class == GrayScale) ||
                (visual_info->class == PseudoColor))
              {
                XMakeStandardColormap(display,visual_info,resource_info,
                  &pixel_info,displayed_image,map_info);
                *state|=UpdateColormapState;
                break;
              }
        break;
      }
      case VisibilityNotify:
      {
        if (event.xvisibility.window == window->pan.id)
          {
            XMapRaised(display,window->pan.id);
            break;
          }
        break;
      }
      default:
      {
        if (resource_info->debug)
          (void) fprintf(stderr,"Event type: %d\n",event.type);
        break;
      }
    }
  if (*state & UpdateColormapState)
    {
      /*
        Update window colormap and graphic context.
      */
      if (resource_info->debug)
        (void) fprintf(stderr,"Update Colormap\n");
      for (i=0; i < number_windows; i++)
      {
        XSetWindowColormap(display,magick_windows[i]->id,map_info->colormap);
        XSetBackground(display,magick_windows[i]->graphic_context,
          pixel_info.background_color.pixel);
        XSetForeground(display,magick_windows[i]->graphic_context,
          pixel_info.foreground_color.pixel);
        XSetBackground(display,magick_windows[i]->highlight_context,
          pixel_info.foreground_color.pixel);
        XSetForeground(display,magick_windows[i]->highlight_context,
          pixel_info.background_color.pixel);
      }
      *state&=(~UpdateColormapState);
    }
  if (*state & UpdateConfigurationState)
    {
      XWindowChanges
        window_changes;

      if (resource_info->debug)
        (void) fprintf(stderr,"Update Configuration\n");
      /*
        Update panning icon configuration.
      */
      window->pan.clip_geometry=window->image.clip_geometry;
      XBestIconSize(display,&window->pan,displayed_image);
      window_changes.width=window->pan.width;
      window_changes.height=window->pan.height;
      XReconfigureWMWindow(display,window->pan.id,window->pan.screen,CWWidth |
        CWHeight,&window_changes);
      if (*state & PanIconMappedState)
        XWithdrawWindow(display,window->pan.id,window->pan.screen);
      if ((window->image.width < window->image.ximage->width) ||
          (window->image.height < window->image.ximage->height))
        XMapRaised(display,window->pan.id);
      /*
        Update magnifier configuration.
      */
      window->magnify.x=window->image.width >> 1;
      window->magnify.y=window->image.height >> 1;
      if (*state & MagnifyMappedState)
        XMakeMagnifyImage(display,resource_info,window);
      /*
        Update icon configuration.
      */
      window->icon.clip_geometry=window->image.clip_geometry;
      XBestIconSize(display,&window->icon,displayed_image);
      window_changes.width=window->icon.width;
      window_changes.height=window->icon.height;
      XReconfigureWMWindow(display,window->icon.id,window->icon.screen,CWWidth |
        CWHeight,&window_changes);
      /*
        Update font configuration.
      */
      (void) sprintf(text," [%u] %s %ux%u %s \0",displayed_image->scene,
        displayed_image->filename,displayed_image->columns,
        displayed_image->rows,XVisualClassName(visual_info));
      if (displayed_image->colors > 0)
        (void) sprintf(text,"%s%uc \0",text,displayed_image->colors);
      XFreeFont(display,font_info);
      font_info=XBestFont(display,resource_info,text,window->image.width);
      if (font_info == (XFontStruct *) NULL)
        Error("unable to load font",resource_info->font);
      for (i=0; i < number_windows; i++)
      {
        magick_windows[i]->font_info=font_info;
        XSetFont(display,magick_windows[i]->graphic_context,font_info->fid);
        XSetFont(display,magick_windows[i]->highlight_context,font_info->fid);
      }
      XRefreshWindow(display,&window->image,(XEvent *) NULL);
      *state&=(~UpdateConfigurationState);
    }
  }
  while ((timeout > time((time_t *) 0)) && !(*state & ExitState));
  if ((*state & LastImageState) || (*state & NextImageState))
    *state&=(~ExitState);
  if (*state & PanIconMappedState)
    XWithdrawWindow(display,window->pan.id,window->pan.screen);
  if (*state & MagnifyMappedState)
    XWithdrawWindow(display,window->magnify.id,window->magnify.screen);
  /*
    Alert user we are busy.
  */
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XFlush(display);
  if ((resource_info->print_filename != (char *) NULL) ||
      (resource_info->write_filename != (char *) NULL))
    {
      /*
        Update image with user transforms.
      */
      if ((window->image.clip_geometry != (char *) NULL) ||
          (displayed_image->columns != window->image.ximage->width) ||
          (displayed_image->rows != window->image.ximage->height))
        {
          char
            image_geometry[2048];

          /*
            Clip and/or scale displayed_image.
          */
          (void) sprintf(image_geometry,"%dx%d\0",window->image.ximage->width,
            window->image.ximage->height);
          TransformImage(&displayed_image,window->image.clip_geometry,
            image_geometry,(char *) NULL);
        }
      if (resource_info->colorspace == GRAYColorspace)
        QuantizeImage(displayed_image,256,8,resource_info->dither,
          GRAYColorspace,True);
      if (resource_info->monochrome)
        QuantizeImage(displayed_image,2,8,resource_info->dither,GRAYColorspace,
          True);
      if (resource_info->number_colors > 0)
        if ((displayed_image->class == DirectClass) ||
            (displayed_image->colors > resource_info->number_colors))
          {
            QuantizeImage(displayed_image,resource_info->number_colors,
              resource_info->tree_depth,resource_info->dither,
              resource_info->colorspace,True);
            SyncImage(displayed_image);
          }
    }
  /*
    Free X resources.
  */
  XFreeCursor(display,window->magnify.cursor);
  (void) free((char *) window->magnify.name);
  (void) free((char *) window->image.name);
  (void) free((char *) window->image.icon_name);
  if (resource_info->backdrop)
    XFreeCursor(display,window->backdrop.cursor);
  XFreeGC(display,window->superclass.graphic_context);
  XFreeGC(display,window->superclass.highlight_context);
  XFreeFont(display,font_info);
  if (*state & ExitState)
    {
      /*
        Destroy X windows.
      */
      for (i=0; i < number_windows; i++)
      {
        if (magick_windows[i]->id != (Window) NULL)
          XDestroyWindow(display,magick_windows[i]->id);
        if (magick_windows[i]->ximage != (XImage *) NULL)
          XDestroyImage(magick_windows[i]->ximage);
        if (magick_windows[i]->pixmap != (Pixmap) NULL)
          XFreePixmap(display,magick_windows[i]->pixmap);
      }
      /*
        Free Standard Colormap.
      */
      if (resource_info->map_type == (char *) NULL)
        XFreeStandardColormap(display,visual_info,&pixel_info,map_info);
      XFreeCursor(display,arrow_cursor);
      XFreeCursor(display,watch_cursor);
      (void) free((void *) window);
      XFree((void *) manager_hints);
      XFree((void *) class_hint);
      XFree((void *) visual_info);
      XFree((void *) map_info);
      visual_info=(XVisualInfo *) NULL;
    }
  XFlush(display);
  *image=displayed_image;
  return(tile_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X D r a w P a n R e c t a n g l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XDrawPanRectangle draws a rectangle in the pan window.  The pan
%  window displays a scaled image and the rectangle shows which portion of
%  the image is displayed in the image window.
%
%  The format of the XDrawPanRectangle routine is:
%
%    XDrawPanRectangle(display,window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%
*/
static void XDrawPanRectangle(display,window)
Display
  *display;

XWindows
  *window;
{
  unsigned long
    scale_factor;

  int
    x,
    y;

  unsigned int
    height,
    width;

  /*
    Determine dimensions of the panning rectangle.
  */
  scale_factor=(unsigned long)
    (UpShift(window->pan.width)/window->image.ximage->width);
  x=DownShift(window->image.x*scale_factor);
  width=DownShift(window->image.width*scale_factor);
  scale_factor=(unsigned long)
    (UpShift(window->pan.height)/window->image.ximage->height);
  y=DownShift(window->image.y*scale_factor);
  height=DownShift(window->image.height*scale_factor);
  /*
    Display the panning rectangle.
  */
  XClearWindow(display,window->pan.id);
  XSetForeground(display,window->pan.graphic_context,
    window->image.pixel_info->background_color.pixel);
  XDrawRectangle(display,window->pan.id,window->pan.graphic_context,x+1,y+1,
    width-2,height-2);
  XSetForeground(display,window->pan.graphic_context,
    window->image.pixel_info->foreground_color.pixel);
  XDrawRectangle(display,window->pan.id,window->pan.graphic_context,x,y,
    width,height);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a g n i f y I m a g e W i n d o w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMagnifyImageWindow magnifies portions of the image as indicated
%  by the pointer.  The magnified portion is displayed in a separate window.
%
%  The format of the XMagnifyImageWindow routine is:
%
%    XMagnifyImageWindow(display,resource_info,window,event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%
*/
static void XMagnifyImageWindow(display,resource_info,window,event)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;

XEvent
  *event;
{
  char
    text[2048];

  register int
    x,
    y;

  unsigned long int
    state;

  /*
    Map info window.
  */
  state=DefaultState;
  (void) sprintf(text," %+u%+u \0",window->image.width,window->image.height);
  XSetWindowExtents(window->info,text,2);
  XMapWindow(display,window->info.id);
  state|=InfoMappedState;
  /*
    Update magnified image until the mouse button is released.
  */
  XDefineCursor(display,window->image.id,window->magnify.cursor);
  x=event->xbutton.x;
  y=event->xbutton.y;
  window->magnify.x=x+1;
  window->magnify.y=y+1;
  do
  {
    /*
      Check boundary conditions.
    */
    if (x < 0)
      x=0;
    else
      if (x >= window->image.width)
        x=window->image.width-1;
    if (y < 0)
      y=0;
    else
     if (y >= window->image.height)
       y=window->image.height-1;
    if ((window->magnify.x != (window->image.x+x)) ||
        (window->magnify.y != (window->image.y+y)))
      {
        window->magnify.x=window->image.x+x;
        window->magnify.y=window->image.y+y;
        /*
          Map and unmap info window as text cursor crosses its boundaries.
        */
        if (state & InfoMappedState)
          {
            if ((x < (window->info.x+window->info.width)) &&
                (y < (window->info.y+window->info.height)))
              {
                XWithdrawWindow(display,window->info.id,window->info.screen);
                state&=(~InfoMappedState);
              }
          }
        else
          if ((x > (window->info.x+window->info.width)) ||
              (y > (window->info.y+window->info.height)))
            {
              XMapWindow(display,window->info.id);
              state|=InfoMappedState;
            }
        if (state & InfoMappedState)
          {
            /*
              Display pointer position.
            */
            (void) sprintf(text," %+d%+d \0",window->magnify.x,
              window->magnify.y);
            XClearWindow(display,window->info.id);
            XDrawImageString(display,window->info.id,
              window->info.graphic_context,2,window->info.font_info->ascent+2,
              text,strlen(text));
          }
        /*
          Display magnified image.
        */
        XMakeMagnifyImage(display,resource_info,window);
      }
    /*
      Wait for next event.
    */
    XWindowEvent(display,window->image.id,ButtonPressMask | Button3MotionMask |
      ButtonReleaseMask | ExposureMask,event);
    switch (event->type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          User has finished magnifying image.
        */
        if (event->xbutton.button != Button3)
          break;
        x=event->xbutton.x;
        y=event->xbutton.y;
        state|=ExitState;
        break;
      }
      case Expose:
      {
        /*
          Refresh image window.
        */
        XRefreshWindow(display,&window->image,event);
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,Button3MotionMask,event));
        x=event->xmotion.x;
        y=event->xmotion.y;
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  /*
    Check boundary conditions.
  */
  if (x < 0)
    x=0;
  else
    if (x >= window->image.width)
      x=window->image.width-1;
  window->magnify.x=window->image.x+x;
  if (y < 0)
    y=0;
  else
   if (y >= window->image.height)
     y=window->image.height-1;
  window->magnify.y=window->image.y+y;
  /*
    Display magnified image.
  */
  XMakeMagnifyImage(display,resource_info,window);
  /*
    Restore cursor.
  */
  XDefineCursor(display,window->image.id,window->image.cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e C u r s o r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeCursor creates a crosshairs X11 cursor.
%
%  The format of the XMakeCursor routine is:
%
%      XMakeCursor(display,window,colormap,background_color,foreground_color)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the ID of the window for which the cursor is
%      assigned.
%
%    o colormap: Specifies the ID of the colormap from which the background
%      and foreground color will be retrieved.
%
%    o background_color: Specifies the color to use for the cursor background.
%
%    o foreground_color: Specifies the color to use for the cursor foreground.
%
%
*/
static Cursor XMakeCursor(display,window,colormap,background_color,
  foreground_color)
Display
  *display;

Window
  window;

Colormap
  colormap;

char
  *background_color,
  *foreground_color;
{
#define scope_height 17
#define scope_mask_width 17
#define scope_mask_height 17
#define scope_x_hot 8
#define scope_y_hot 8
#define scope_width 17

  static unsigned char scope_bit[] =
    {
      0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x7e, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x00, 0x00
    };

  static unsigned char scope_mask_bit[] =
    {
      0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
      0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x7f, 0xfc, 0x01,
      0x7f, 0xfc, 0x01, 0x7f, 0xfc, 0x01, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
      0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00, 0x80, 0x03, 0x00,
      0x80, 0x03, 0x00
    };

  Cursor
    cursor;

  Pixmap
    mask,
    source;

  XColor
    background,
    foreground;

  source=XCreateBitmapFromData(display,window,(char *) scope_bit,scope_width,
    scope_height);
  mask=XCreateBitmapFromData(display,window,(char *) scope_mask_bit,
    scope_width,scope_height);
  if ((source == (Pixmap) NULL) || (mask == (Pixmap) NULL))
    Error("unable to create pixmap",(char *) NULL);
  XParseColor(display,colormap,background_color,&background);
  XParseColor(display,colormap,foreground_color,&foreground);
  cursor=XCreatePixmapCursor(display,source,mask,&foreground,&background,
    scope_x_hot,scope_y_hot);
  XFreePixmap(display,source);
  XFreePixmap(display,mask);
  return(cursor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e M a g n i f y I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeMagnifyImage magnifies a region of an X image and returns it.
%
%  The format of the XMakeMagnifyImage routine is:
%
%      XMakeMagnifyImage(display,resource_info,window)
%
%  A description of each parameter follows:
%
%    o window: Specifies a pointer to a XWindows structure.
%
%
*/
static void XMakeMagnifyImage(display,resource_info,window)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;
{
  register int
    x,
    y;

  register unsigned char
    *p,
    *q;

  register unsigned int
    j,
    k,
    l;

  static char
    text[2048];

  static unsigned int
    previous_magnify=0;

  static XWindowInfo
    magnify_window;

  unsigned int
    height,
    i,
    magnify,
    scanline_pad,
    width;

  XColor
    color;

  XImage
    *ximage;

  /*
    Check boundry conditions.
  */
  magnify=1;
  for (i=1; i < resource_info->magnify; i++)
    magnify<<=1;
  while ((magnify*window->image.ximage->width) < window->magnify.width)
    magnify<<=1;
  while ((magnify*window->image.ximage->height) < window->magnify.height)
    magnify<<=1;
  while (magnify > window->magnify.width)
    magnify>>=1;
  while (magnify > window->magnify.height)
    magnify>>=1;
  if (magnify != previous_magnify)
    {
      unsigned int
        status;

      XTextProperty
        window_name;

      /*
        New magnify factor:  update magnify window name.
      */
      i=0;
      while ((1 << i) <= magnify)
        i++;
      (void) sprintf(window->magnify.name,"Magnify %uX",i);
      status=XStringListToTextProperty(&window->magnify.name,1,&window_name);
      if (status != 0)
        XSetWMName(display,window->magnify.id,&window_name);
    }
  previous_magnify=magnify;
  ximage=window->image.ximage;
  width=window->magnify.ximage->width;
  height=window->magnify.ximage->height;
  x=window->magnify.x-(width/magnify/2);
  if (x < 0)
    x=0;
  else
    if (x > (ximage->width-(width/magnify)))
      x=ximage->width-width/magnify;
  y=window->magnify.y-(height/magnify/2);
  if (y < 0)
    y=0;
  else
    if (y > (ximage->height-(height/magnify)))
      y=ximage->height-height/magnify;
  q=(unsigned char *) window->magnify.ximage->data;
  scanline_pad=window->magnify.ximage->bytes_per_line-
    ((width*window->magnify.ximage->bits_per_pixel) >> 3);
  if (ximage->bits_per_pixel < 8)
    {
      register unsigned char
        byte,
        p_bit,
        q_bit;

      register unsigned int
        plane;

      switch (ximage->bitmap_bit_order)
      {
        case LSBFirst:
        {
          /*
            Magnify little-endian bitmap.
          */
          for (i=0; i < height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; plane < ximage->bits_per_pixel; plane++)
                  {
                    byte>>=1;
                    if (*p & (0x01 << (p_bit+plane)))
                      byte|=0x80;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte >> (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
        case MSBFirst:
        default:
        {
          /*
            Magnify big-endian bitmap.
          */
          for (i=0; i < height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; plane < ximage->bits_per_pixel; plane++)
                  {
                    byte<<=1;
                    if (*p & (0x80 >> (p_bit+plane)))
                      byte|=0x01;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte << (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
      }
    }
  else
    switch (ximage->bits_per_pixel)
    {
      case 8:
      {
        /*
          Magnify 8 bit X image.
        */
        for (i=0; i < height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                *q++=(*p);
              p++;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
      default:
      {
        register unsigned int
          bytes_per_pixel,
          m;

        /*
          Magnify multi-byte X image.
        */
        bytes_per_pixel=ximage->bits_per_pixel >> 3;
        for (i=0; i < height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                for (m=0; m < bytes_per_pixel; m++)
                  *q++=(*(p+m));
              p+=bytes_per_pixel;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
    }
  /*
    Copy X image to magnify pixmap.
  */
  x=window->magnify.x-(width/magnify/2);
  if (x < 0)
    x=width/2-window->magnify.x*magnify;
  else
    if (x > (ximage->width-(width/magnify)))
      x=(ximage->width-window->magnify.x)*magnify-width/2;
    else
      x=0;
  y=window->magnify.y-(height/magnify/2);
  if (y < 0)
    y=height/2-window->magnify.y*magnify;
  else
    if (y > (ximage->height-(height/magnify)))
      y=(ximage->height-window->magnify.y)*magnify-height/2;
    else
      y=0;
  if ((x != 0) || (y != 0))
    XFillRectangle(display,window->magnify.pixmap,
      window->magnify.highlight_context,0,0,width,height);
  XPutImage(display,window->magnify.pixmap,window->magnify.graphic_context,
    window->magnify.ximage,0,0,x,y,width-x,height-y);
  if ((magnify > 1) && ((magnify <= (width/2)) && (magnify <= (height/2))))
    {
      /*
        Highlight center pixel.
      */
      x=window->magnify.width/2;
      y=window->magnify.height/2;
      XSetForeground(display,window->magnify.graphic_context,
        window->magnify.pixel_info->background_color.pixel);
      XDrawRectangle(display,window->magnify.pixmap,
        window->magnify.graphic_context,x+1,y+1,magnify-2,magnify-2);
      XSetForeground(display,window->magnify.graphic_context,
        window->magnify.pixel_info->foreground_color.pixel);
      XDrawRectangle(display,window->magnify.pixmap,
        window->magnify.graphic_context,x,y,magnify,magnify);
    }
  /*
    Show center pixel color.
  */
  color.pixel=
    XGetPixel(window->image.ximage,window->magnify.x,window->magnify.y);
  XQueryColor(display,window->image.map_info->colormap,&color);
  if (window->magnify.depth > 12)
    (void) sprintf(text,"(%3u,%3u,%3u)\0",color.red >> 8,color.green >> 8,
      color.blue >> 8);
  else
    (void) sprintf(text,"(%3u,%3u,%3u)  %lu \0",color.red >> 8,color.green >> 8,
      color.blue >> 8,color.pixel);
  XDrawImageString(display,window->magnify.pixmap,
    window->magnify.graphic_context,2,window->magnify.font_info->ascent+2,text,
    strlen(text));
  /*
    Refresh magnify window.
  */
  magnify_window=window->magnify;
  magnify_window.x=0;
  magnify_window.y=0;
  XRefreshWindow(display,&magnify_window,(XEvent *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P a n I m a g e W i n d o w                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPanImageWindow pans the image until the mouse button is released.
%
%  The format of the XPanImageWindow routine is:
%
%    XPanImageWindow(display,window,event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
*/
static void XPanImageWindow(display,window,event)
Display
  *display;

XWindows
  *window;

XEvent
  *event;
{
  char
    text[2048];

  Cursor
    cursor;

  unsigned long int
    state;

  unsigned long
    x_factor,
    y_factor;

  XRectangle
    pan_info;

  /*
    Map info window.
  */
  state=DefaultState;
  (void) sprintf(text," %dx%d%+d%+d  \0",window->image.ximage->width,
    window->image.ximage->height,window->image.ximage->width,
    window->image.ximage->height);
  XSetWindowExtents(window->info,text,2);
  XMapWindow(display,window->info.id);
  state|=InfoMappedState;
  /*
    Define cursor.
  */
  if ((window->image.ximage->width > window->image.width) &&
      (window->image.ximage->height > window->image.height))
    cursor=XCreateFontCursor(display,XC_fleur);
  else
    if (window->image.ximage->width > window->image.width)
      cursor=XCreateFontCursor(display,XC_sb_h_double_arrow);
    else
      if (window->image.ximage->height > window->image.height)
        cursor=XCreateFontCursor(display,XC_sb_v_double_arrow);
      else
        cursor=XCreateFontCursor(display,XC_arrow);
  if (cursor == (Cursor) NULL)
    Error("unable to create cursor",(char *) NULL);
  XRecolorCursor(display,cursor,&window->image.pixel_info->background_color,
    &window->image.pixel_info->foreground_color);
  XDefineCursor(display,window->pan.id,cursor);
  /*
    Pan image as pointer moves until the mouse button is released.
  */
  x_factor=(unsigned long)
    UpShift(window->image.ximage->width)/window->pan.width;
  y_factor=(unsigned long)
    UpShift(window->image.ximage->height)/window->pan.height;
  pan_info.x=event->xbutton.x;
  pan_info.y=event->xbutton.y;
  pan_info.width=(unsigned short int) (UpShift(window->image.width)/x_factor);
  pan_info.height=(unsigned short int) (UpShift(window->image.height)/y_factor);
  window->image.x=pan_info.x+1;
  window->image.y=pan_info.y+1;
  do
  {
    /*
      Check boundary conditions.
    */
    pan_info.x=DownShift((pan_info.x-pan_info.width/2)*x_factor);
    if (pan_info.x < 0)
      pan_info.x=0;
    else
      if ((pan_info.x+window->image.width) > window->image.ximage->width)
        pan_info.x=window->image.ximage->width-window->image.width;
    pan_info.y=DownShift((pan_info.y-pan_info.height/2)*y_factor);
    if (pan_info.y < 0)
      pan_info.y=0;
    else
      if ((pan_info.y+window->image.height) > window->image.ximage->height)
        pan_info.y=window->image.ximage->height-window->image.height;
    if ((window->image.x != pan_info.x) || (window->image.y != pan_info.y))
      {
        /*
          Display image pan offset.
        */
        window->image.x=pan_info.x;
        window->image.y=pan_info.y;
        (void) sprintf(text," %dx%d%+d%+d \0",window->image.ximage->width,
          window->image.ximage->height,window->image.x,window->image.y);
        XClearWindow(display,window->info.id);
        XDrawImageString(display,window->info.id,window->info.graphic_context,2,
          window->info.font_info->ascent+2,text,strlen(text));
        /*
          Refresh image window.
        */
        XDrawPanRectangle(display,window);
        XRefreshWindow(display,&window->image,(XEvent *) NULL);
      }
    /*
      Wait for next event.
    */
    XWindowEvent(display,window->pan.id,ButtonPressMask | ButtonMotionMask |
      ButtonReleaseMask,event);
    switch (event->type)
    {
      case ButtonRelease:
      {
        /*
          User has finished panning the image.
        */
        pan_info.x=event->xbutton.x;
        pan_info.y=event->xbutton.y;
        state|=ExitState;
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,ButtonMotionMask,event));
        pan_info.x=event->xmotion.x;
        pan_info.y=event->xmotion.y;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  /*
    Check boundary conditions.
  */
  pan_info.x=DownShift((pan_info.x-pan_info.width/2)*x_factor);
  if (pan_info.x < 0)
    pan_info.x=0;
  else
    if ((pan_info.x+window->image.width) > window->image.ximage->width)
      pan_info.x=window->image.ximage->width-window->image.width;
  pan_info.y=DownShift((pan_info.y-pan_info.height/2)*y_factor);
  if (pan_info.y < 0)
    pan_info.y=0;
  else
    if ((pan_info.y+window->image.height) > window->image.ximage->height)
      pan_info.y=window->image.ximage->height-window->image.height;
  if ((window->image.x != pan_info.x) || (window->image.y != pan_info.y))
    {
      /*
        Refresh image window.
      */
      window->image.x=pan_info.x;
      window->image.y=pan_info.y;
      XDrawPanRectangle(display,window);
      XRefreshWindow(display,&window->image,(XEvent *) NULL);
    }
  /*
    Restore cursor.
  */
  XDefineCursor(display,window->pan.id,window->pan.cursor);
  XFreeCursor(display,cursor);
  XWithdrawWindow(display,window->info.id,window->info.screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e f l e c t I m a g e W i n d o w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XReflectImageWindow reflects the scanlines of an image.
%
%  The format of the XReflectImageWindow routine is:
%
%    status=XReflectImageWindow(display,window,image)
%
%  A description of each parameter follows:
%
%    o status: Function XReflectImageWindow return True if the window scanlines
%      reverse.  False is returned is there is a memory shortage or if the
%      window scanlines fails to reverse.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int XReflectImageWindow(display,window,image)
Display
  *display;

XWindows
  *window;

Image
  **image;
{
  char
    text[2048];

  Image
    *reflected_image;

  unsigned long int
    state;

  state=DefaultState;
  if (((*image)->columns*(*image)->rows) > MinInfoSize)
    {
      /*
        Map image window.
      */
      (void) strcpy(text," Reflecting image... ");
      XSetWindowExtents(window->info,text,2);
      XMapWindow(display,window->info.id);
      XDrawImageString(display,window->info.id,window->info.graphic_context,2,
        window->info.font_info->ascent+2,text,strlen(text));
      state|=InfoMappedState;
    }
  /*
    Reflect image scanlines.
  */
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XFlush(display);
  reflected_image=ReflectImage(*image);
  XDefineCursor(display,window->image.id,window->image.cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  if (reflected_image == (Image *) NULL)
    return(False);
  DestroyImage(*image);
  *image=reflected_image;
  if (window->image.clip_geometry != (char *) NULL)
    {
      int
        x,
        y;

      unsigned int
        height,
        width;

      /*
        Reverse clip geometry.
      */
      (void) XParseGeometry(window->image.clip_geometry,&x,&y,&width,&height);
      (void) sprintf(window->image.clip_geometry,"%ux%u%+d%+d\0",width,height,
        (int) (*image)->columns-(int) width-x,y);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R o t a t e I m a g e W i n d o w                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XRotateImageWindow rotates the X image left or right 90 degrees.
%
%  The format of the XRotateImageWindow routine is:
%
%    status=XRotateImageWindow(display,window,degrees,image)
%
%  A description of each parameter follows:
%
%    o status: Function XRotateImageWindow return True if the window is
%      rotated.  False is returned is there is a memory shortage or if the
%      window fails to rotate.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o degrees: Specifies the number of degrees to rotate the image.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
static unsigned int XRotateImageWindow(display,window,degrees,image)
Display
  *display;

XWindows
  *window;

unsigned int
  degrees;

Image
  **image;
{
  char
    text[2048];

  int
    x,
    y;

  Image
    *rotated_image;

  unsigned long int
    state;

  state=DefaultState;
  if (((*image)->columns*(*image)->rows) > MinInfoSize)
    {
      /*
        Map info window.
      */
      (void) strcpy(text," Rotating image... ");
      XSetWindowExtents(window->info,text,2);
      XMapWindow(display,window->info.id);
      XDrawImageString(display,window->info.id,window->info.graphic_context,2,
        window->info.font_info->ascent+2,text,strlen(text));
      state|=InfoMappedState;
    }
  /*
    Rotate image.
  */
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XFlush(display);
  rotated_image=RotateImage(*image,(double) degrees,True);
  XDefineCursor(display,window->image.id,window->image.cursor);
  if (state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  if (rotated_image == (Image *) NULL)
    return(False);
  DestroyImage(*image);
  *image=rotated_image;
  if (window->image.clip_geometry != (char *) NULL)
    {
      unsigned int
        height,
        width;

      /*
        Rotate clip geometry.
      */
      (void) XParseGeometry(window->image.clip_geometry,&x,&y,&width,&height);
      if (degrees < 180.0)
        (void) sprintf(window->image.clip_geometry,"%ux%u%+d%+d\0",height,
          width,(int) (*image)->columns-(int) height-y,x);
      else
        (void) sprintf(window->image.clip_geometry,"%ux%u%+d%+d\0",height,
          width,y,(int) (*image)->rows-(int) width-x);
    }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X T i l e I m a g e W i n d o w                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XTileImageWindow determines which individual tile of a composite
%  image was choosen with a button press and then displays it.
%
%  The format of the XTileImageWindow routine is:
%
%    tile_image=XTileImageWindow(display,resource_info,window,image,event)
%
%  A description of each parameter follows:
%
%    o tile_image:  XTileImageWindow reads the tiled image and returns
%      it.  A null image is returned if an error occurs.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindows structure.
%
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%
*/
static Image *XTileImageWindow(display,resource_info,window,image,event)
Display
  *display;

XResourceInfo
  *resource_info;

XWindows
  *window;

Image
  *image;

XEvent
  *event;
{
  AlienInfo
    alien_info;

  char
    *p,
    *q;

  Image
    *tile_image;

  int
    tile,
    x,
    y;

  unsigned int
    height,
    width;

  unsigned long int
    scale_factor;

  /*
    Tile image is relative to composite image configuration.
  */
  x=0;
  y=0;
  width=image->columns;
  height=image->rows;
  if (window->image.clip_geometry != (char *) NULL)
    (void) XParseGeometry(window->image.clip_geometry,&x,&y,&width,&height);
  scale_factor=UpShift(width)/window->image.ximage->width;
  event->xbutton.x+=window->image.x;
  event->xbutton.x=DownShift(event->xbutton.x*scale_factor)+x;
  scale_factor=UpShift(height)/window->image.ximage->height;
  event->xbutton.y+=window->image.y;
  event->xbutton.y=DownShift(event->xbutton.y*scale_factor)+y;
  /*
    Determine size and location of individual tiles of the composite.
  */
  x=0;
  y=0;
  width=image->columns;
  height=image->rows;
  (void) XParseGeometry(image->montage,&x,&y,&width,&height);
  tile=((event->xbutton.y-y)/height)*((image->columns-x)/width)+
    (event->xbutton.x-x)/width;
  if (tile < 0)
    {
      /*
        Button press is outside any tile.
      */
      XBell(display,0);
      return((Image *) NULL);
    }
  /*
    Determine file name from the tile directory.
  */
  p=image->directory;
  while ((tile != 0) && (*p != '\0'))
  {
    if (*p == '\n')
      tile--;
    p++;
  }
  if (*p == '\0')
    {
      /*
        Button press is outside any tile.
      */
      XBell(display,0);
      return((Image *) NULL);
    }
  /*
    Read tile image.
  */
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XFlush(display);
  GetAlienInfo(&alien_info);
  q=p;
  while ((*q != '\n') && (*q != '\0'))
    q++;
  (void) strncpy(alien_info.filename,p,q-p);
  alien_info.filename[q-p]='\0';
  alien_info.server_name=resource_info->server_name;
  alien_info.font=resource_info->font;
  tile_image=ReadAlienImage(&alien_info);
  XDefineCursor(display,window->image.id,window->image.cursor);
  if (tile_image == (Image *) NULL)
    {
      XPopupAlert(display,&window->popup,"unable to read image",
        alien_info.filename);
      return((Image *) NULL);
    }
  return(tile_image);
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
    *resource_value,
    *scale_geometry,
    *server_name,
    *window_id;

  Display
    *display;

  double
    gamma;

  int
    degrees,
    i,
    x;

  unsigned int
    compression,
    enhance,
    *image_marker,
    image_number,
    inverse,
    noise,
    normalize,
    reflect,
    scene,
    verbose;

  unsigned long int
    state;

  XrmDatabase
    resource_database,
    server_database;

  XResourceInfo
    resource_info;

  /*
    Display usage profile if there are no command line arguments.
  */
  client_name=(*argv);
  if (argc < 2)
    Usage(True);
  /*
    Set defaults.
  */
  GetAlienInfo(&alien_info);
  display=(Display *) NULL;
  image_marker=(unsigned int *) malloc(argc*sizeof(unsigned int));
  if (image_marker == (unsigned int *) NULL)
    Error("unable to display image","memory allocation failed");
  for (i=0; i < argc; i++)
    image_marker[i]=argc;
  image_number=0;
  resource_database=(XrmDatabase) NULL;
  server_name=(char *) NULL;
  state=DefaultState;
  window_id=(char *) NULL;
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
          i++;
          if (i == argc)
            Error("missing server name on -display",(char *) NULL);
          server_name=argv[i];
          break;
        }
  }
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
  clip_geometry=
    XGetResource(resource_database,client_name,"clipGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,client_name,"compression",
    "RunlengthEncoded");
  if (Latin1Compare("qencoded",resource_value) == 0)
    compression=QEncodedCompression;
  else
    compression=RunlengthEncodedCompression;
  resource_value=XGetResource(resource_database,client_name,"enhance","False");
  enhance=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"gamma","0.0");
  gamma=atof(resource_value);
  resource_value=XGetResource(resource_database,client_name,"inverse","False");
  inverse=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"noise","False");
  noise=IsTrue(resource_value);
  resource_value=
    XGetResource(resource_database,client_name,"normalize","False");
  normalize=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"reflect","False");
  reflect=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"rotate","0");
  degrees=atoi(resource_value);
  scale_geometry=
    XGetResource(resource_database,client_name,"scaleGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,client_name,"scene","0");
  scene=atoi(resource_value);
  resource_value=XGetResource(resource_database,client_name,"verbose","False");
  verbose=IsTrue(resource_value);
  window_id=
    XGetResource(resource_database,client_name,"windowId",(char *) NULL);
  /*
    Parse command line.
  */
  for (i=1; ((i < argc) && !(state & ExitState)); i++)
  {
    option=argv[i];
    if (((int) strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      switch (*(option+1))
      {
        case 'b':
        {
          if (strncmp("backdrop",option+1,5) == 0)
            {
              resource_info.backdrop=(*option == '-');
              break;
            }
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
          if (strncmp("colormap",option+1,6) == 0)
            {
              resource_info.colormap=PrivateColormap;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing type on -colormap",(char *) NULL);
                  option=argv[i];
                  resource_info.colormap=UndefinedColormap;
                  if (Latin1Compare("private",option) == 0)
                    resource_info.colormap=PrivateColormap;
                  if (Latin1Compare("shared",option) == 0)
                    resource_info.colormap=SharedColormap;
                  if (resource_info.colormap == UndefinedColormap)
                    Error("invalid colormap type on -colormap",option);
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
          if (strncmp("debug",option+1,3) == 0)
            {
              resource_info.debug=(*option == '-');
              break;
            }
          if (strncmp("delay",option+1,3) == 0)
            {
              resource_info.delay=0;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("missing seconds on -delay",(char *) NULL);
                  resource_info.delay=atoi(argv[i]);
                }
              break;
            }
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
              resource_info.server_name=server_name;
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
        case 'e':
        {
          enhance=(*option == '-');
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
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
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
              alien_info.geometry=resource_info.image_geometry;
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'h':
        {
          Usage(True);
          break;
        }
        case 'i':
        {
          if (strncmp("iconGeometry",option+1,5) == 0)
            {
              resource_info.icon_geometry=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing geometry on -iconGeometry",(char *) NULL);
                  resource_info.icon_geometry=argv[i];
                }
              break;
            }
          if (strncmp("iconic",option+1,5) == 0)
            {
              resource_info.iconic=(*option == '-');
              break;
            }
          if (strncmp("inverse",option+1,2) == 0)
            {
              inverse=(*option == '-');
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'm':
        {
          if (strncmp("magnify",option+1,3) == 0)
            {
              resource_info.magnify=2;
              if (*option == '-')
                {
                  i++;
                  if ((i == argc) || !sscanf(argv[i],"%d",&x))
                    Error("missing level on -magnify",(char *) NULL);
                  resource_info.magnify=atoi(argv[i]);
                }
              break;
            }
          if (strncmp("map",option+1,3) == 0)
            {
              resource_info.map_type=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing map type on -map",(char *) NULL);
                  resource_info.map_type=argv[i];
                }
              break;
            }
          if (strncmp("monochrome",option+1,2) == 0)
            {
              resource_info.monochrome=(*option == '-');
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'n':
        {
          if (strncmp("name",option+1,2) == 0)
            {
              resource_info.name=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing name on -name",(char *) NULL);
                  resource_info.name=argv[i];
                }
              break;
            }
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
        case 'p':
        {
          if (strncmp("print",option+1,2) == 0)
            {
              resource_info.print_filename=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing file name on -print",(char *) NULL);
                  resource_info.print_filename=argv[i];
                  if (access(resource_info.print_filename,0) == 0)
                    {
                      char
                        answer[2];

                      (void) fprintf(stderr,"Overwrite %s? ",
                        resource_info.print_filename);
                      (void) gets(answer);
                      if (!((*answer == 'y') || (*answer == 'Y')))
                    exit(1);
                    }
                }
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
          Error("unrecognized option",option);
          break;
        }
        case 't':
        {
          if (strncmp("title",option+1,2) == 0)
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
          if (strncmp("verbose",option+1,2) == 0)
            {
              verbose=(*option == '-');
              alien_info.verbose=verbose;
              break;
            }
          if (strncmp("visual",option+1,2) == 0)
            {
              resource_info.visual_type=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing visual class on -visual",(char *) NULL);
                  resource_info.visual_type=argv[i];
                }
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'w':
        {
          if (strncmp("window",option+1,2) == 0)
            {
              window_id=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing id, name, or 'root' on -window",
                      (char *) NULL);
                  window_id=argv[i];
                }
              break;
            }
          if (strncmp("write",option+1,2) == 0)
            {
              resource_info.write_filename=(char *) NULL;
              if (*option == '-')
                {
                  i++;
                  if (i == argc)
                    Error("missing file name on -write",(char *) NULL);
                  resource_info.write_filename=argv[i];
                  if (access(resource_info.write_filename,0) == 0)
                    {
                      char
                        answer[2];

                      (void) fprintf(stderr,"Overwrite %s? ",
                        resource_info.write_filename);
                      (void) gets(answer);
                      if (!((*answer == 'y') || (*answer == 'Y')))
                        exit(1);
                    }
                }
              break;
            }
          Error("unrecognized option",option);
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

        unsigned long int
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
          if (scene != 0)
            image->scene=scene;
          total_colors=0;
          /*
            Transform image as defined by the clip, image and scale geometries.
          */
          TransformImage(&image,clip_geometry,resource_info.image_geometry,
            scale_geometry);
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
          if (resource_info.colorspace == GRAYColorspace)
            QuantizeImage(image,256,8,resource_info.dither,GRAYColorspace,True);
          if (resource_info.monochrome)
            QuantizeImage(image,2,8,resource_info.dither,GRAYColorspace,True);
          if (resource_info.number_colors != 0)
            if ((image->class == DirectClass) ||
                (image->colors > resource_info.number_colors))
              {
                /*
                  Reduce the number of colors in the image.
                */
                QuantizeImage(image,resource_info.number_colors,
                  resource_info.tree_depth,resource_info.dither,
                  resource_info.colorspace,True);
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
          /*
            Display image to X server.
          */
          if (compression != UndefinedCompression)
            image->compression=compression;
          else
            image->compression=info_image.compression;
          if (window_id != (char *) NULL)
            {
              /*
                Display image to a specified X window.
              */
              XDisplayBackgroundImage(display,&resource_info,window_id,image);
              state&=ExitState;
            }
          else
            if (image->montage == (char *) NULL)
              (void) XDisplayImage(display,&resource_info,argv,argc,&image,
                &state);
            else
              do
              {
                Image
                  *tile_image;

                /*
                  Display montage image.
                */
                tile_image=
                  XDisplayImage(display,&resource_info,argv,argc,&image,&state);
                if ((!(state & TileImageState)) || (state & ExitState))
                  break;
                (void) XDisplayImage(display,&resource_info,argv,argc,
                  &tile_image,&state);
                DestroyImage(tile_image);
              } while (!(state & ExitState));
          if (resource_info.print_filename != (char *) NULL)
            {
              /*
                Print image as Encapsulated Postscript.
              */
              (void) strcpy(image->filename,resource_info.print_filename);
              (void) PrintImage(image,resource_info.image_geometry);
            }
          if (resource_info.write_filename != (char *) NULL)
            {
              /*
                Write image in MIFF format.
              */
              (void) strcpy(image->filename,resource_info.write_filename);
              (void) WriteAlienImage(image);
            }
          if (verbose)
            {
              /*
                Display detailed info about the image.
              */
              (void) fprintf(stderr,"[%u] %s",
                image->scene == 0 ? image_number : image->scene,
                info_image.filename);
              if (resource_info.write_filename != (char *) NULL)
                (void) fprintf(stderr,"=>%s",resource_info.write_filename);
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
          /*
            Proceed to next/previous image.
          */
          if (state & LastImageState)
            next_image=image->last;
          else
            next_image=image->next;
          if (next_image != (Image *) NULL)
            image=next_image;
        } while ((next_image != (Image *) NULL) && !(state & ExitState));
        /*
          Free image resources.
        */
        DestroyImages(image);
        if (!(state & LastImageState))
          image_marker[i]=image_number++;
        else
          {
            /*
              Proceed to last image.
            */
            for (i--; i > 0; i--)
              if (image_marker[i] == (image_number-2))
                break;
            if (image_number != 0)
              image_number--;
          }
      }
  }
  if (image_number == 0)
    Error("missing an image file name",(char *) NULL);
  XCloseDisplay(display);
  (void) free((char *) image_marker);
  return(False);
}
