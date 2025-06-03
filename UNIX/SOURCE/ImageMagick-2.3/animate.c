/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%              AAA   N   N  IIIII  M   M   AAA   TTTTT  EEEEE                 %
%             A   A  NN  N    I    MM MM  A   A    T    E                     %
%             AAAAA  N N N    I    M M M  AAAAA    T    EEE                   %
%             A   A  N  NN    I    M   M  A   A    T    E                     %
%             A   A  N   N  IIIII  M   M  A   A    T    EEEEE                 %
%                                                                             %
%                                                                             %
%          Animate Machine Independent File Format Image via X11.             %
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
%  Animate displays a sequence of images in the MIFF format on any
%  workstation display running an X server.  Animate first determines the
%  hardware capabilities of the workstation.  If the number of unique
%  colors in an image is less than or equal to the number the workstation
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
%  The Animate program command syntax is:
%
%  Usage: animate [options ...] file [ [options ...] file ...]
%
%  Where options include:
%    -backdrop            display image centered on a backdrop
%    -clip geometry       preferred size and location of the clipped image
%    -colormap type       Shared or Private
%    -colors value        preferred number of colors in the image
%    -colorspace type     GRAY, RGB, XYZ, YIQ, or YUV
%    -delay milliseconds  display the next image after pausing
%    -density geometry    vertical and horizonal density of the image
%    -display server      display image to this X server
%    -dither              apply Floyd/Steinberg error diffusion to image
%    -gamma value         level of gamma correction
%    -geometry geometry   preferred size and location of the image window
%    -map type            display image using this Standard Colormap
%    -monochrome          transform image to black and white
%    -reflect             reverse image scanlines
%    -rotate degrees      apply Paeth rotation to the image
%    -scale geometry      preferred size factors of the image
%    -treedepth value     depth of the color classification tree
%    -verbose             print detailed information about the image
%    -visual type         display image using this visual type
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
%
%  Keyboard accelerators:
%    p    press to animate the sequence of images
%    s    press to display the next image in the sequence
%    .    press to continually display the sequence of images
%    a    press to automatically reverse the sequence of images
%    <    press to slow the display of the images
%    >    press to speed-up the display of the images
%    f    press to animate in the forward direction
%    r    press to animate in the reverse direction
%    i    press to display information about the image
%    q    press to discard all images and exit program
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
#define AutoReverseAnimationState 0x0001
#define ConfigureWindowState  0x0002
#define DefaultState  0x0004
#define ExitState  0x0008
#define ForwardAnimationState 0x0010
#define HighlightState  0x0020
#define InfoMappedState  0x0040
#define PlayAnimationState 0x0080
#define RepeatAnimationState 0x0100
#define StepAnimationState 0x0200

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
%   D e l a y                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Delay suspends animation for the number of milliseconds specified.
%
%  The format of the Delay routine is:
%
%      Delay(milliseconds)
%
%  A description of each parameter follows:
%
%    o milliseconds: Specifies the number of milliseconds to delay before
%      returning.
%
%
*/
static void Timer()
{
}

static void Delay(milliseconds)
unsigned long
  milliseconds;
{
#ifdef unix
#ifdef SYSV
#include <sys/poll.h>
  if (milliseconds == 0)
    return;
  (void) poll((struct pollfd *) NULL,(unsigned long) NULL,
    (int) (milliseconds/1000));
#else
  struct timeval
    timer;

  if (milliseconds == 0)
    return;
  timer.tv_sec=milliseconds/1000;
  timer.tv_usec=(milliseconds % 1000)*1000;
  (void) select(0,(fd_set *) NULL,(fd_set *) NULL,(fd_set *) NULL,&timer);
#endif
#endif
}

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
%    o terminate: The program will exit if the value is not zero.
%
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
      (char *) NULL
    },
    *keys[]=
    {
      "0-9  press to change the level of delay",
      "p    press to animate the sequence of images",
      "s    press to display the next image in the sequence",
      ".    press to continually display the sequence of images",
      "a    press to automatically reverse the sequence of images",
      "<    press to slow the display of the images",
      ">    press to speed-up the display of images",
      "f    press to animate in the forward direction",
      "r    press to animate in the reverse direction",
      "i    press to display information about the image",
      "q    press to discard all images and exit program",
      (char *) NULL
    },
    *options[]=
    {
      "-backdrop            display image centered on a backdrop",
      "-clip geometry       preferred size and location of the clipped image",
      "-colormap type       Shared or Private",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     GRAY, RGB, XYZ, YIQ, or YUV",
      "-delay milliseconds  display the next image after pausing",
      "-density geometry    vertical and horizonal density of the image",
      "-display server      display image to this X server",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred size and location of the image window",
      "-map type            display image using this Standard Colormap",
      "-monochrome          transform image to black and white",
      "-reflect             reflect the image scanlines",
      "-rotate degrees      apply Paeth rotation to the image",
      "-scale geometry      preferred size factors of the image",
      "-treedepth value     depth of the color classification tree",
      "-verbose             print detailed information about the image",
      "-visual type         display image using this visual type",
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
%    UserCommand(display,resource_info,window,image,command,state);
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

unsigned int
  *state;
{
  if (*state & InfoMappedState)
    XWithdrawWindow(display,window->info.id,window->info.screen);
  /*
    Process user command.
  */
  switch (command)
  {
    case ' ':
    case '\0':
      break;
    case '<':
    {
      resource_info->delay<<=1;
      if (resource_info->delay == 0)
        resource_info->delay=1;
      break;
    }
    case '>':
    {
      resource_info->delay>>=1;
      break;
    }
    case '.':
    {
      *state|=RepeatAnimationState;
      *state&=(~AutoReverseAnimationState);
      *state|=PlayAnimationState;
      break;
    }
    case 'a':
    {
      *state|=AutoReverseAnimationState;
      *state&=(~RepeatAnimationState);
      *state|=PlayAnimationState;
      break;
    }
    case 'f':
    {
      *state=ForwardAnimationState;
      *state&=(~AutoReverseAnimationState);
      break;
    }
    case 'i':
    {
      char
        text[2048];

      /*
        Display information about the image in the info window.
      */
      (void) sprintf(text," [%u] %s %ux%u \0",(*image)->scene,
        (*image)->filename,window->image.width,window->image.height);
      if ((*image)->colors > 0)
        (void) sprintf(text,"%s%uc \0",text,(*image)->colors);
      (void) strcat(text,(*image)->magick);
      XSetWindowExtents(window->info,text,2);
      XMapWindow(display,window->info.id);
      XDrawImageString(display,window->info.id,window->info.graphic_context,2,
        window->info.font_info->ascent+2,text,(unsigned int) strlen(text));
      break;
    }
    case 'p':
    {
      *state|=PlayAnimationState;
      *state&=(~AutoReverseAnimationState);
      break;
    }
    case 's':
    case '\n':
    {
      *state|=StepAnimationState;
      *state&=(~PlayAnimationState);
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
    case '\r':
    {
      *state&=(~ForwardAnimationState);
      *state&=(~AutoReverseAnimationState);
      break;
    }
    default:
    {
      XBell(display,0);
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n i m a t e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XAnimateImage displays an image via X11.
%
%  The format of the XAnimateImage routine is:
%
%      XAnimateImage(display,resource_info,argv,argc,image,number_scenes)
%
%  A description of each parameter follows:
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
%    o image: Specifies a pointer to a Image structure; returned from
%      ReadImage.
%
%    o number_scenes: Specifies the number of scenes to animate.
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

static void XAnimateImage(display,resource_info,argv,argc,images,number_scenes)
Display
  *display;

XResourceInfo
  *resource_info;

char
  **argv;

int
  argc;

Image
  **images;

unsigned int
  number_scenes;
{
#define MaxWindows  9

  Atom
    delete_property,
    protocols_property;

  char
    text[2048];

  Cursor
    arrow_cursor,
    watch_cursor;

  GC
    graphic_context;

  int
    i,
    scene;

  unsigned int
    number_windows,
    state,
    status;

  Window
    root_window;

  XClassHint
    *class_hint;

  XEvent
    event;

  XFontStruct
    *font_info;

  XGCValues
    graphic_context_value;

  XPixelInfo
    pixel_info;

  XStandardColormap
    *map_info;

  XVisualInfo
    *visual_info;

  XWindowInfo
    *magick_windows[MaxWindows];

  XWindows
    *window;

  XWMHints
    *manager_hints;

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
    Get the best visual this server supports.
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
        visual_info->red_mask,visual_info->green_mask,visual_info->blue_mask);
      (void) fprintf(stderr,"  significant bits in color: %d bits\n",
        visual_info->bits_per_rgb);
    }
  /*
    Initialize cursor.
  */
  arrow_cursor=XCreateFontCursor(display,XC_arrow);
  watch_cursor=XCreateFontCursor(display,XC_watch);
  if ((arrow_cursor == (Cursor) NULL) || (watch_cursor == (Cursor) NULL))
    Error("unable to create cursor",(char *) NULL);
  /*
    Initialize atoms.
  */
  protocols_property=XInternAtom(display,"WM_PROTOCOLS",False);
  delete_property=XInternAtom(display,"WM_DELETE_WINDOW",False);
  if ((protocols_property == (Atom) NULL) || (delete_property == (Atom) NULL))
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
  magick_windows[number_windows++]=(&window->popup);
  for (i=0; i < number_windows; i++)
    magick_windows[i]->id=(Window) NULL;
  /*
    Sort images by increasing scene number.
  */
  i=0;
  for (scene=0; scene < number_scenes; scene++)
    i+=images[scene]->scene;
  if (i > 0)
    (void) qsort((void *) images,(int) number_scenes,sizeof(Image *),
      LinearCompare);
  if (resource_info->map_type == (char *) NULL)
    if ((visual_info->class != TrueColor) &&
        (visual_info->class != DirectColor))
      {
        unsigned int
          identical_colormap;

        /*
          Determine if the sequence of images has the identical colormap.
        */
        identical_colormap=True;
        for (scene=0; scene < number_scenes; scene++)
        {
          if ((images[scene]->class == DirectClass) ||
              (images[scene]->colors > visual_info->colormap_size))
            {
              /*
                Image has more colors than the visual supports.
              */
              status=UnpackImage(images[scene]);
              if (status == False)
                Error("unable to unpack image",(char *) NULL);
              QuantizeImage(images[scene],(unsigned int)
                visual_info->colormap_size,resource_info->tree_depth,
                resource_info->dither,resource_info->colorspace,False);
            }
          if (images[scene]->signature == (char *) NULL)
            ColormapSignature(images[scene]);
          status=strcmp(images[scene]->signature,images[0]->signature);
          if (status != 0)
            identical_colormap=False;
        }
        if (!identical_colormap)
          {
            /*
              Create a single colormap for the sequence of images.
            */
            for (scene=0; scene < number_scenes; scene++)
              if (images[scene]->packed_pixels != (unsigned char *) NULL)
                {
                  status=UnpackImage(images[scene]);
                  if (status == False)
                    Error("unable to unpack image",(char *) NULL);
                }
            QuantizeImages(images,number_scenes,(Image *) NULL,(unsigned int)
              visual_info->colormap_size,resource_info->tree_depth,
              resource_info->dither,resource_info->colorspace,False);
          }
      }
  /*
    Initialize Standard Colormap.
  */
  if (images[0]->packed_pixels != (unsigned char *) NULL)
    {
      status=UnpackImage(images[0]);
      if (status == False)
        Error("unable to unpack image",(char *) NULL);
    }
  XMakeStandardColormap(display,visual_info,resource_info,&pixel_info,
    images[0],map_info);
  /*
    Color cursors.
  */
  XRecolorCursor(display,arrow_cursor,&pixel_info.background_color,
    &pixel_info.foreground_color);
  XRecolorCursor(display,watch_cursor,&pixel_info.background_color,
    &pixel_info.foreground_color);
  /*
    Initialize font info.
  */
  (void) sprintf(text," [%u] %s %ux%u \0",images[0]->scene,images[0]->filename,
    images[0]->columns,images[0]->rows);
  if (images[0]->colors > 0)
    (void) sprintf(text,"%s%uc \0",text,images[0]->colors);
  font_info=XBestFont(display,resource_info,text,images[0]->columns);
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
  window->superclass.border_width=WindowBorderWidth;
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
  window->superclass.graphic_context=(GC) NULL;
  window->superclass.ximage=(XImage *) NULL;
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
  graphic_context_value.graphics_exposures=False;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context=XCreateGC(display,window->superclass.id,GCBackground |
    GCFont | GCForeground | GCFunction | GCGraphicsExposures | GCLineWidth |
    GCPlaneMask,&graphic_context_value);
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
  XBestIconSize(display,&window->icon,images[0]);
  window->icon.attributes.event_mask=ExposureMask | StructureNotifyMask;
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
      register char
        *p;

      (void) strcpy(window->image.name,"ImageMagick: ");
      (void) strcat(window->image.name,images[0]->filename);
      p=window->image.name;
      while (*p != '\0')
      {
        if (*p == '.')
          {
            *p='\0';
            break;
          }
        p++;
      }
      (void) strcpy(window->image.icon_name,images[0]->filename);
      p=window->image.icon_name;
      while (*p != '\0')
      {
        if (*p == '.')
          {
            *p='\0';
            break;
          }
        p++;
      }
    }
  window->image.geometry=resource_info->image_geometry;
  window->image.width=images[0]->columns;
  if (window->image.width > XDisplayWidth(display,visual_info->screen))
    window->image.width=XDisplayWidth(display,visual_info->screen);
  window->image.height=images[0]->rows;
  if (window->image.height > XDisplayHeight(display,visual_info->screen))
    window->image.height=XDisplayHeight(display,visual_info->screen);
  window->image.border_width=resource_info->border_width;
  XGetWindowInfo(&window->superclass,&window->backdrop);
  if (resource_info->backdrop)
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
      window->backdrop.name="ImageMagick Background";
      window->backdrop.flags=USSize | USPosition;
      window->backdrop.width=XDisplayWidth(display,visual_info->screen);
      window->backdrop.height=XDisplayHeight(display,visual_info->screen);
      window->backdrop.border_width=0;
      window->backdrop.immutable=True;
      window->backdrop.attributes.cursor=window->backdrop.cursor;
      window->backdrop.attributes.do_not_propagate_mask=
        ButtonPressMask | ButtonReleaseMask;
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
      XSetTransientForHint(display,window->backdrop.id,window->backdrop.id);
      XMapWindow(display,window->backdrop.id);
      XInstallColormap(display,map_info->colormap);
      XSetInputFocus(display,window->backdrop.id,RevertToNone,CurrentTime);
      /*
        Position image in the center the backdrop.
      */
      window->image.flags|=USPosition;
      window->image.x=0;
      width=images[0]->columns+window->image.border_width;
      if (width < XDisplayWidth(display,visual_info->screen))
        window->image.x=XDisplayWidth(display,visual_info->screen)/2-width/2;
      window->image.y=0;
      height=images[0]->rows+window->image.border_width;
      if (height < XDisplayHeight(display,visual_info->screen))
        window->image.y=XDisplayHeight(display,visual_info->screen)/2-height/2;
    }
  window->image.immutable=False;
  window->image.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    LeaveWindowMask | OwnerGrabButtonMask | StructureNotifyMask;
  manager_hints->flags=IconWindowHint | InputHint | StateHint;
  manager_hints->icon_window=window->icon.id;
  manager_hints->input=True;
  manager_hints->initial_state=
    resource_info->iconic ? IconicState : NormalState;
  XMakeWindow(display,(resource_info->backdrop ? window->backdrop.id :
    root_window),argv,argc,class_hint,manager_hints,delete_property,
    &window->image);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (image)\n",window->image.id);
  XMapWindow(display,window->image.id);
  window->image.x=0;
  window->image.y=0;
  /*
    Initialize image X image structure.
  */
  status=XMakeImage(display,resource_info,&window->image,images[0],
    images[0]->columns,images[0]->rows);
  status|=XMakePixmap(display,resource_info,&window->image);
  if (status == False)
    Error("unable to create X image",(char *) NULL);
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"Image: [%u] %s %ux%u ",images[0]->scene,
        images[0]->filename,images[0]->columns,images[0]->rows);
      if (images[0]->colors > 0)
        (void) fprintf(stderr,"%uc ",images[0]->colors);
      (void) fprintf(stderr,"%s\n",images[0]->magick);
    }
  XRefreshWindow(display,&window->image,(XEvent *) NULL);
  /*
    Initialize popup window.
  */
  XGetWindowInfo(&window->superclass,&window->popup);
  window->popup.name="ImageMagick Popup";
  window->popup.flags=PSize | PPosition;
  window->popup.attributes.override_redirect=True;
  window->popup.attributes.save_under=True;
  window->popup.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | LeaveWindowMask |
    OwnerGrabButtonMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=False;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=window->image.id;
  XMakeWindow(display,root_window,argv,argc,class_hint,manager_hints,
    delete_property,&window->popup);
  if (resource_info->debug)
    (void) fprintf(stderr,"Window id: 0x%lx (popup)\n",window->popup.id);
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
  /*
    Initialize image pixmaps structure.
  */
  XDefineCursor(display,window->image.id,window->image.busy_cursor);
  XMapWindow(display,window->info.id);
  window->image.pixmaps=(Pixmap *) malloc(number_scenes*sizeof(Pixmap));
  if (window->image.pixmaps == (Pixmap *) NULL)
    Error("unable to animate images","memory allocation failed");
  window->image.pixmaps[0]=window->image.pixmap;
  for (scene=1; scene < number_scenes; scene++)
  {
    /*
      Display information about the image in the info window.
    */
    (void) sprintf(text," [%u] %s %ux%u \0",images[scene]->scene,
      images[scene]->filename,window->image.width,window->image.height);
    if (images[scene]->colors > 0)
      (void) sprintf(text,"%s%uc \0",text,images[scene]->colors);
    XSetWindowExtents(window->info,text,2);
    XDrawImageString(display,window->info.id,window->info.graphic_context,2,
      window->info.font_info->ascent+2,text,(unsigned int) strlen(text));
    XFlush(display);
    /*
      Create X image.
    */
    window->image.pixmap=(Pixmap) NULL;
    if (images[scene]->packed_pixels != (unsigned char *) NULL)
      {
        status=UnpackImage(images[scene]);
        if (status == False)
          Error("unable to unpack image",(char *) NULL);
      }
    status=XMakeImage(display,resource_info,&window->image,images[scene],
      images[scene]->columns,images[scene]->rows);
    status|=XMakePixmap(display,resource_info,&window->image);
    if (status == False)
      Error("unable to create X image",(char *) NULL);
    if (resource_info->debug)
      {
        (void) fprintf(stderr,"Image: [%u] %s %ux%u ",images[scene]->scene,
          images[scene]->filename,images[scene]->columns,images[scene]->rows);
        if (images[scene]->colors > 0)
          (void) fprintf(stderr,"%uc ",images[scene]->colors);
        (void) fprintf(stderr,"%s\n",images[scene]->magick);
      }
    /*
      Free image pixels.
    */
    (void) free((char *) images[scene]->pixels);
    images[scene]->pixels=(RunlengthPacket *) NULL;
    /*
      Refresh image window.
    */
    window->image.pixmaps[scene]=window->image.pixmap;
    XRefreshWindow(display,&window->image,(XEvent *) NULL);
    XSync(display,False);
  }
  XWithdrawWindow(display,window->info.id,window->info.screen);
  XDefineCursor(display,window->image.id,window->image.cursor);
  /*
    Respond to events.
  */
  state=DefaultState;
  scene=0;
  do
  {
    if (XEventsQueued(display,QueuedAfterFlush) == 0)
      if ((state & PlayAnimationState) || (state & StepAnimationState))
        {
          if (state & InfoMappedState)
            XWithdrawWindow(display,window->info.id,window->info.screen);
          /*
            Copy X pixmap to image window.
          */
          window->image.pixmap=window->image.pixmaps[scene];
          XRefreshWindow(display,&window->image,(XEvent *) NULL);
          XSync(display,False);
          if (state & StepAnimationState)
            {
              state&=(~StepAnimationState);
              UserCommand(display,resource_info,window,'i',&images[scene],
                &state);
            }
          if (resource_info->delay > 0)
            Delay((unsigned long) resource_info->delay);
          if (state & ForwardAnimationState)
            {
              /*
                Forward animation:  increment scene number.
              */
              scene++;
              if (scene == number_scenes)
                if (state & AutoReverseAnimationState)
                  {
                    state&=(~ForwardAnimationState);
                    scene--;
                  }
                else
                  {
                    if (!(state & RepeatAnimationState))
                      state&=(~PlayAnimationState);
                    scene=0;
                  }
            }
          else
            {
              /*
                Reverse animation:  decrement scene number.
              */
              scene--;
              if (scene < 0)
                if (state & AutoReverseAnimationState)
                  {
                    state|=ForwardAnimationState;
                    scene=0;
                  }
                else
                  {
                    if (!(state & RepeatAnimationState))
                      state&=(~PlayAnimationState);
                    scene=number_scenes-1;
                  }
            }
          continue;
        }
    /*
      Handle a window event.
    */
    XNextEvent(display,&event);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.window == window->image.id)
          switch (event.xbutton.button)
          {
            case Button1:
            {
              static char
                command[2048],
                *MenuCommand="ips.a<>frq",
                *MenuSelections[]=
                {
                  "Image Info",
                  "Play",
                  "Step",
                  "Repeat",
                  "Auto Reverse",
                  "Slower",
                  "Faster",
                  "Forward",
                  "Reverse",
                  "Quit"
                };

              static int
                command_number;

              /*
                Select a command from the pop-up menu.
              */
              command_number=XPopupMenu(display,&window->popup,
                event.xbutton.x_root,event.xbutton.y_root,"Commands",
                MenuSelections,sizeof(MenuSelections)/sizeof(char *),command);
              if (*command != '\0')
                UserCommand(display,resource_info,window,
                  MenuCommand[command_number],&images[scene],&state);
              break;
            }
            default:
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
              state|=ExitState;
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
            window->image.width=event.xconfigure.width;
            window->image.height=event.xconfigure.height;
            break;
          }
        if (event.xconfigure.window == window->icon.id)
          {
            /*
              Icon window has a new configuration.
            */
            window->icon.width=event.xconfigure.width;
            window->icon.height=event.xconfigure.height;
            break;
          }
      }
      case EnterNotify:
      {
        /*
          Selectively install colormap.
        */
        if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
          if (event.xcrossing.mode != NotifyUngrab)
            XInductColormap(display,map_info->colormap);
        if (window->backdrop.id != (Window) NULL)
          if (event.xbutton.window == window->image.id)
            {
              XInstallColormap(display,map_info->colormap);
              XSetInputFocus(display,window->image.id,RevertToNone,CurrentTime);
              break;
            }
        break;
      }
      case Expose:
      {
        if (resource_info->debug)
          (void) fprintf(stderr,"Expose: 0x%lx %dx%d+%d+%d\n",
            event.xexpose.window,event.xexpose.width,event.xexpose.height,
            event.xexpose.x,event.xexpose.y);
        /*
          Repaint windows that are now exposed.
        */
        if (event.xexpose.window == window->image.id)
          {
            window->image.pixmap=window->image.pixmaps[scene];
            XRefreshWindow(display,&window->image,&event);
            break;
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
        if (state & ConfigureWindowState)
          {
            XBell(display,0);
            break;
          }
        *command='\0';
        XLookupString((XKeyEvent *) &event.xkey,command,sizeof(command),
          &key_symbol,(XComposeStatus *) NULL);
        if (key_symbol == XK_Help)
          Usage(False);
        else
          if (!IsCursorKey(key_symbol))
            UserCommand(display,resource_info,window,*command,&images[scene],
              &state);
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
            state=ForwardAnimationState | PlayAnimationState;
            break;
          }
        if (event.xmap.window == window->info.id)
          {
            state|=InfoMappedState;
            break;
          }
        if (event.xmap.window == window->icon.id)
          {
            /*
              Create icon pixmap.
            */
            status=XMakeImage(display,resource_info,&window->icon,images[0],
              window->icon.width,window->icon.height);
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
        if (event.xunmap.window == window->info.id)
          {
            state&=(~InfoMappedState);
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
  }
  while (!(state & ExitState));
  /*
    Free X resources.
  */
  (void) free((char *) window->image.name);
  (void) free((char *) window->image.icon_name);
  if (resource_info->backdrop)
    XFreeCursor(display,window->backdrop.cursor);
  XFreeGC(display,window->superclass.graphic_context);
  XFreeGC(display,window->superclass.highlight_context);
  XFreeFont(display,font_info);
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
  for (scene=0; scene < number_scenes; scene++)
  {
    XFreePixmap(display,window->image.pixmaps[scene]);
    DestroyImage(images[scene]);
  }
  (void) free((char *) window->image.pixmaps);
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
  XFlush(display);
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
    *server_name;

  Display
    *display;

  double
    gamma;

  Image
    **images;

  int
    degrees,
    i,
    x;

  unsigned int
    image_number,
    maximum_images,
    reflect,
    verbose;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database,
    server_database;

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
  clip_geometry=(char *) NULL;
  degrees=0;
  display=(Display *) NULL;
  gamma=0.0;
  reflect=False;
  scale_geometry=(char *) NULL;
  server_name=(char *) NULL;
  verbose=False;
  maximum_images=2048;
  images=(Image **) malloc(maximum_images*sizeof(Image *));
  if (images == (Image **) NULL)
    Error("unable to animate images","memory allocation failed");
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
          if (*option == '-')
            {
              i++;
              if (i == argc)
                Error("missing server name on -display",(char *) NULL);
              server_name=argv[i];
              break;
            }
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
  resource_value=XGetResource(resource_database,client_name,"gamma","0.0");
  gamma=atof(resource_value);
  resource_value=XGetResource(resource_database,client_name,"reflect","False");
  reflect=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"rotate","0");
  degrees=atoi(resource_value);
  scale_geometry=
    XGetResource(resource_database,client_name,"scaleGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,client_name,"verbose","False");
  verbose=IsTrue(resource_value);
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
          Error("unrecognized option",option);
          break;
        }
        case 'm':
        {
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
          if (gamma > 0.0)
            GammaImage(image,gamma);
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
          /*
            Pack image data to conserve memory (memory <=> speed).
          */
          image->alpha=False;
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
                Error("unable to animate images","memory allocation failed");
            }
          images[image_number++]=image;
          next_image=image->next;
          if (next_image != (Image *) NULL)
            image=next_image;
        } while (next_image != (Image *) NULL);
      }
  }
  if (image_number == 0)
    Error("missing an image file name",(char *) NULL);
  XAnimateImage(display,&resource_info,argv,argc,images,image_number);
  (void) free((char *) images);
  XCloseDisplay(display);
  return(False);
}
