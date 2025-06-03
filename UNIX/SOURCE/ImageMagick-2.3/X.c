/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%                                     X   X                                   %
%                                      X X                                    %
%                                       X                                     %
%                                      X X                                    %
%                                     X   X                                   %
%                                                                             %
%                          X11 Utility Routines for Display.                  %
%                                                                             %
%                                                                             %
%                                 Software Design                             %
%                                   John Cristy                               %
%                                    July 1992                                %
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
%
*/

/*
  Include declarations.
*/
#include "display.h"
#include "image.h"
#include "X.h"

/*
  State declarations.
*/
#define ControlState  0x0001
#define DefaultState  0x0000
#define ExitState  0x0002

/*
  External declarations.
*/
extern char
  *client_name;

extern void
  Error _Declare((char *,char *));

/*
  Forward declarations.
*/
int
  Latin1Compare _Declare((char *,char *));

static unsigned int
  XReadColormap _Declare((Display *,XWindowAttributes *,XColor **));

static void
  XMakeImageLSBFirst _Declare((XWindowInfo *,Image *, XImage *)),
  XMakeImageMSBFirst _Declare((XWindowInfo *,Image *, XImage *));

static Window
  XClientWindow _Declare((Display *,Window)),
  XSelectWindow _Declare((Display *,XRectangle *)),
  XWindowByProperty _Declare((Display *,Window,Atom));

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T r u e                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function IsTrue returns True if the boolean is "true", "on", "yes" or "1".
%
%  The format of the IsTrue routine is:
%
%      option=IsTrue(boolean)
%
%  A description of each parameter follows:
%
%    o option: either True or False depending on the boolean parameter.
%
%    o boolean: Specifies a pointer to a character array.
%
%
*/
unsigned int IsTrue(boolean)
char
  *boolean;
{
  if (boolean == (char *) NULL)
    return(False);
  if (Latin1Compare(boolean,"true") == 0)
    return(True);
  if (Latin1Compare(boolean,"on") == 0)
    return(True);
  if (Latin1Compare(boolean,"yes") == 0)
    return(True);
  if (Latin1Compare(boolean,"1") == 0)
    return(True);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a t i n 1 C o m p a r e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Latin1Compare compares two null terminated Latin-1 strings,
%  ignoring case differences, and returns an integer greater than, equal
%  to, or less than 0, according to whether first is lexicographically
%  greater than, equal to, or less than second.  The two strings are
%  assumed to be encoded using ISO 8859-1.
%
%  The format of the Latin1Compare routine is:
%
%      Latin1Compare(first,second)
%
%  A description of each parameter follows:
%
%    o first: A pointer to the string to convert to Latin1 string.
%
%    o second: A pointer to the string to convert to Latin1 string.
%
%
*/
int Latin1Compare(first,second)
char
  *first,
  *second;
{
  register unsigned char
   *p,
   *q;
 
  p=(unsigned char *) first;
  q=(unsigned char *) second;
  while ((*p != '\0') && (*q != '\0'))
  {
    register unsigned char
      c,
      d;

    c=(*p); 
    d=(*q); 
    if (c != d)
      {
        /* 
          Try lowercasing and try again.
        */
        if ((c >= XK_A) && (c <= XK_Z))
          c+=(XK_a-XK_A);
        else 
          if ((c >= XK_Agrave) && (c <= XK_Odiaeresis))
            c+=(XK_agrave-XK_Agrave);
          else 
            if ((c >= XK_Ooblique) && (c <= XK_Thorn))
              c+=(XK_oslash-XK_Ooblique);
        if ((d >= XK_A) && (d <= XK_Z))
          d+=(XK_a-XK_A);
        else
          if ((d >= XK_Agrave) && (d <= XK_Odiaeresis))
            d+=(XK_agrave-XK_Agrave);
          else if ((d >= XK_Ooblique) && (d <= XK_Thorn))
            d+=(XK_oslash-XK_Ooblique);
        if (c != d)
          return(((int) c)-((int) d));
      }
    p++;
    q++;
  }
  return(((int) *p)-((int) *q));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L a t i n 1 U p p e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function Latin1Upper copies a null terminated string from src to dst
%  (including the null), changing all Latin-1 lowercase letters to
%  uppercase.  The string is assumed to be encoded using ISO 8859-1.
%
%  The format of the Latin1Upper routine is:
%
%      Latin1Upper(string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to upper-case Latin1.
%
%
*/
static void Latin1Upper(string)
char
  *string;
{
  unsigned char
    c;
 
  c=(*string);
  while (c != '\0')
  {
    if ((c >= XK_a) && (c <= XK_z))
      *string=c-(XK_a-XK_A);
    else
      if ((c >= XK_agrave) && (c <= XK_odiaeresis))
        *string=c-(XK_agrave-XK_Agrave);
      else
        if ((c >= XK_oslash) && (c <= XK_thorn))
          *string=c-(XK_oslash-XK_Ooblique);
    string++;
    c=(*string);
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d T E X T I m a g e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadTEXTImage reads a text file and returns it as an image.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTEXTImage routine is:
%
%      image=ReadTEXTImage(filename,server_name,font,density)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadTEXTImage returns a pointer to the image after
%      reading. A null image is returned if there is a a memory shortage or if
%      the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%    o server_name: Specifies the name of the X server to read fonts from.
%
%    o font: Specifies the name of an X font to use for writing the text.
%
%    o density: Specifies the vertical and horizonal density of the image.
%
%
*/
Image *ReadTEXTImage(filename,server_name,font,density)
char
  *filename,
  *server_name,
  *font,
  *density;
{
#define PageHeight  60
#define PageWidth  80

  char
    *resource_value,
    *text_status,
    text[2048];

  Display
    *display;

  Image
    *image;

  int
    status,
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *p;

  RunlengthPacket
    background_color;

  unsigned int
    height,
    width;

  XAnnotateInfo
    annotate_info;

  XFontStruct
    *font_info;

  XPixelInfo
    pixel_info;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database,
    server_database;

  XStandardColormap
    map_info;

  XVisualInfo
    *visual_info;

  XWindowInfo
    image_window;

  /*
    Allocate image structure.
  */
  image=AllocateImage("TEXT");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  /*
    Open image file.
  */
  (void) strcpy(image->filename,filename);
  OpenImage(image,"r");
  if (image->file == (FILE *) NULL)
    {
      Warning("unable to open file",image->filename);
      DestroyImage(image);
      return((Image *) NULL);
    }
  /*
    Open X server connection.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    {
      Warning("unable to connect to X server",XDisplayName(server_name));
      return((Image *) NULL);
    }
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
  /*
    Initialize visual info.
  */
  visual_info=XBestVisualInfo(display,"default",(char *) NULL,
    (XStandardColormap *) NULL);
  if (visual_info == (XVisualInfo *) NULL)
    {
      Warning("unable to get visual",resource_info.visual_type);
      return((Image *) NULL);
    }
  /*
    Determine background and foreground colors.
  */
  map_info.colormap=XDefaultColormap(display,visual_info->screen);
  XGetPixelInfo(display,visual_info,&map_info,&resource_info,(Image *) NULL,
    &pixel_info);
  pixel_info.annotate_color=pixel_info.foreground_color;
  pixel_info.annotate_index=1;
  /*
    Initialize font info.
  */
  if (font != (char *) NULL)
    resource_info.font=font;
  font_info=XBestFont(display,&resource_info,(char *) NULL,~0);
  if (font_info == (XFontStruct *) NULL)
    {
      Warning("unable to load font",resource_info.font);
      return((Image *) NULL);
    }
  /*
    Window superclass.
  */
  image_window.id=XRootWindow(display,visual_info->screen);
  image_window.screen=visual_info->screen;
  image_window.depth=visual_info->depth;
  image_window.visual_info=visual_info;
  image_window.pixel_info=(&pixel_info);
  image_window.font_info=font_info;
  /*
    Initialize Image structure.
  */
  width=PageWidth;
  height=PageHeight;
  if (density != (char *) NULL)
    (void) XParseGeometry(density,&x,&y,&width,&height);
  image->columns=width*font_info->max_bounds.width+4;
  image->rows=height*
    (font_info->max_bounds.ascent+font_info->max_bounds.descent)+4;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  image->comments=(char *) malloc((strlen(image->filename)+2048)*sizeof(char));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (image->comments == (char *) NULL))
    {
      Warning("unable to allocate image","memory allocation error");
      DestroyImage(image);
      return((Image *) NULL);
    }
  (void) sprintf(image->comments,"\n  Imported from text file:  %s\n",
    image->filename);
  /*
    Create colormap.
  */
  image->colors=2;
  image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      Warning("unable to read image","memory allocation failed");
      DestroyImage(image);
      return((Image *) NULL);
    }
  image->colormap[0].red=pixel_info.background_color.red >> 8;
  image->colormap[0].green=pixel_info.background_color.green >> 8;
  image->colormap[0].blue=pixel_info.background_color.blue >> 8;
  image->colormap[1].red=pixel_info.foreground_color.red >> 8;
  image->colormap[1].green=pixel_info.foreground_color.green >> 8;
  image->colormap[1].blue=pixel_info.foreground_color.blue >> 8;
  /*
    Initialize text image to background color.
  */
  background_color.red=image->colormap[0].red;
  background_color.green=image->colormap[0].green;
  background_color.blue=image->colormap[0].blue;
  background_color.index=0;
  background_color.length=0;
  p=image->pixels;
  for (i=0; i < image->packets; i++)
    *p++=background_color;
  /*
    Annotate the text image.
  */
  XGetAnnotateInfo(&annotate_info);
  annotate_info.font_info=font_info;
  annotate_info.text=(char *)
    malloc((image->columns/Max(font_info->min_bounds.width,1)+2)*sizeof(char));
  if (annotate_info.text == (char *) NULL)
    {
      Warning("unable to read image","memory allocation failed");
      DestroyImage(image);
      return((Image *) NULL);
    }
  image->colormap[0].red=pixel_info.background_color.red >> 8;
  annotate_info.height=font_info->ascent+font_info->descent;
  x=0;
  y=0;
  text_status=fgets(text,sizeof(text),image->file);
  if ((int) strlen(text) > 0)
    text[strlen(text)-1]='\0';
  while (text_status != (char *) NULL)
  {
    *annotate_info.text='\0';
    if (*text != '\0')
      {
        /*
          Compute width of text.
        */
        (void) strcpy(annotate_info.text,text);
        annotate_info.width=
          XTextWidth(font_info,annotate_info.text,strlen(annotate_info.text));
        if ((annotate_info.width+4) >= image->columns)
          {
            /*
              Reduce text until width is within bounds.
            */
            i=strlen(annotate_info.text);
            for (; (annotate_info.width+4) >= image->columns; i--)
              annotate_info.width=XTextWidth(font_info,annotate_info.text,
                (unsigned int) i);
            annotate_info.text[i]='\0';
            while ((i > 0) && !isspace(annotate_info.text[i]))
              i--;
            if (i > 0)
              annotate_info.text[i]='\0';
            annotate_info.width=XTextWidth(font_info,annotate_info.text,
              strlen(annotate_info.text));
          }
        /*
          Annotate image with text.
        */
        (void) sprintf(annotate_info.geometry,"%ux%u%+d%+d\0",
          annotate_info.width,annotate_info.height,x+2,y+2);
        status=XAnnotateImage(display,&image_window,&annotate_info,False,image);
        if (status == 0)
          {
            Warning("unable to annotate image","memory allocation error");
            DestroyImage(image);
            return((Image *) NULL);
          }
      }
    /*
      Get next string.
    */
    if (strlen(text) != strlen(annotate_info.text))
      (void) strcpy(text,text+strlen(annotate_info.text)+1);
    else
      {
        text_status=fgets(text,sizeof(text),image->file);
        if ((int) strlen(text) > 0)
          text[strlen(text)-1]='\0';
      }
    y+=annotate_info.height;
    if ((text_status != (char *) NULL) &&
        ((y+font_info->ascent+4) > image->rows))
      {
        /*
          Page is full-- allocate next image structure.
        */
        image->orphan=True;
        image->next=CopyImage(image,image->columns,image->rows,False);
        image->orphan=False;
        if (image->next == (Image *) NULL)
          {
            DestroyImages(image);
            return((Image *) NULL);
          }
        image->next->last=image;
        image=image->next;
        /*
          Initialize text image to background color.
        */
        p=image->pixels;
        for (i=0; i < image->packets; i++)
          *p++=background_color;
        y=0;
      }
  }
  /*
    Free resources.
  */
  (void) free((char *) annotate_info.text);
  XFreeFont(display,font_info);
  XFree((void *) visual_info);
  /*
    Force to runlength-encoded PseudoClass image.
  */
  while (image->last != (Image *) NULL)
  {
    image->class=PseudoClass;
    image=image->last;
  }
  image->class=PseudoClass;
  CloseImage(image);
  XCloseDisplay(display);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure ReadXImage reads an image from an X window.
%
%  The format of the ReadXImage routine is:
%
%      image=ReadXImage(filename,server_name,frame,screen,borders)
%
%  A description of each parameter follows:
%
%    o image: Specifies a pointer to the Image structure.
%
%    o filename: Specifies the name of the image to read.
%
%    o server_name: Specifies the name of the X server to read image from.
%
%    o frame: Specifies whether to include the window manager frame with the
%      image.
%
%    o screen: Specifies whether the GetImage request used to obtain the image
%      should be done on the root window, rather than directly on the specified
%      window.
%
%    o borders: Specifies whether borders pixels are to be saved with
%      the image.
%
%
*/
Image *ReadXImage(filename,server_name,frame,screen,borders)
char
  *filename,
  *server_name;

unsigned int
  frame,
  screen,
  borders;
{
  Display
    *display;

  Image
    *image;

  int
    display_width,
    display_height,
    number_colors,
    status,
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *p;

  register unsigned long
    pixel;

  unsigned int
    d;

  Window
    client_window,
    root_window,
    target_window;

  XColor
    *colors;

  XImage
    *ximage;

  XRectangle
    clip_info;

  XTextProperty
    window_name;

  XWindowAttributes
    client_attributes,
    target_attributes;

  /*
    Open X server connection.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    {
      Warning("unable to connect to X server",XDisplayName(server_name));
      return((Image *) NULL);
    }
  /*
    Set our forgiving error handler.
  */
  XSetErrorHandler(XError);
  /*
    Select image window.
  */
  clip_info.x=0;
  clip_info.y=0;
  clip_info.width=0;
  clip_info.height=0;
  root_window=XRootWindow(display,XDefaultScreen(display));
  target_window=(Window) NULL;
  if ((filename != (char *) NULL) && (*filename != '\0'))
    if (Latin1Compare(filename,"root") == 0)
      target_window=root_window;
    else
      {
        if (isdigit(*filename))
          target_window=XWindowByID(display,root_window,
            (Window) strtol(filename,(char **) NULL,0));
        if (target_window == (Window) NULL)
          target_window=XWindowByName(display,root_window,filename);
        if (target_window == (Window) NULL)
          Warning("No window with specified id exists",filename);
      }
  if (target_window == (Window) NULL)
    target_window=XSelectWindow(display,&clip_info);
  /*
    Inform the user not to alter the screen.
  */
  XBell(display,0);
  /*
    Get attributes of the target & client window.
  */
  client_window=target_window;
  if (target_window != root_window)
    if (XGetGeometry(display,target_window,&root_window,&x,&x,&d,&d,&d,&d) != 0)
      {
        /*
          Get client window.
        */
        client_window=XClientWindow(display,target_window);
        if (!frame)
          target_window=client_window;
      }
  status=XGetWindowAttributes(display,target_window,&target_attributes);
  status|=XGetWindowAttributes(display,client_window,&client_attributes);
  if (status == 0)
    {
      Warning("unable to get target window attributes",(char *) NULL);
      return((Image *) NULL);
    }
  /*
    Initialize X image structure.
  */
  if ((clip_info.width*clip_info.height) > (unsigned int) 0)
    ximage=XGetImage(display,root_window,clip_info.x,clip_info.y,
      clip_info.width,clip_info.height,AllPlanes,ZPixmap);
  else
    {
      unsigned int
        height,
        width;

      Window
        child;

      /*
        Image by window id.
      */
      XTranslateCoordinates(display,target_window,root_window,0,0,&x,&y,&child);
      target_attributes.x=x;
      target_attributes.y=y;
      width=target_attributes.width;
      height=target_attributes.height;
      if (borders)
        {
          /*
            Do not include border in image.
          */
          x-=target_attributes.border_width;
          y-=target_attributes.border_width;
          width+=2*target_attributes.border_width;
          height+=2*target_attributes.border_width;
        }
      /*
        clip to window
      */
      if (x < 0)
        {
          width+=x;
          x=0;
        }
      if (y < 0)
        {
          height+=y;
          y=0;
        }
      display_width=DisplayWidth(display,XDefaultScreen(display));
      display_height=DisplayHeight(display,XDefaultScreen(display));
      if ((x+(int) width) > display_width)
        width=display_width-x;
      if ((y+(int) height) > display_height)
        height=display_height-y;
      /*
        Get image from window with XGetImage.
      */
      if (screen)
        ximage=
          XGetImage(display,root_window,x,y,width,height,AllPlanes,ZPixmap);
      else
        {
          x-=target_attributes.x;
          y-=target_attributes.y;
          ximage=XGetImage(display,target_window,x,y,width,height,AllPlanes,
            ZPixmap);
        }
    }
  if (ximage == (XImage *) NULL)
    {
      Warning("unable to read image",(char *) NULL);
      return((Image *) NULL);
    }
  /*
    Obtain the window colormap from the client of the target window.
  */
  number_colors=XReadColormap(display,&client_attributes,&colors);
  XBell(display,0);
  XBell(display,0);
  XFlush(display);
  /*
    Allocate image structure.
  */
  image=AllocateImage("X");
  if (image == (Image *) NULL)
    {
      Warning("unable to read image","memory allocation failed");
      return((Image *) NULL);
    }
  /*
    Convert X image to MIFF format.
  */
  if (XGetWMName(display,target_window,&window_name) != 0)
    {
      if ((filename != (char *) NULL) && (*filename == '\0'))
        (void) strcpy(image->filename,(char *) window_name.value);
      /*
        Initial image comment.
      */
      image->comments=(char *)
        malloc((strlen((char *) window_name.value)+2048)*sizeof(char));
      if (image->comments == (char *) NULL)
        {
          Warning("unable to read image","memory allocation failed");
          DestroyImage(image);
          return((Image *) NULL);
        }
      (void) sprintf(image->comments,"\n  Imported from X11 window:  %s\n\0",
        window_name.value);
    }
  if ((target_attributes.visual->class != TrueColor) &&
      (target_attributes.visual->class != DirectColor))
    image->class=PseudoClass;
  image->columns=ximage->width;
  image->rows=ximage->height;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if (image->pixels == (RunlengthPacket *) NULL)
    {
      Warning("unable to read image","memory allocation failed");
      DestroyImage(image);
      return((Image *) NULL);
    }
  p=image->pixels;
  switch (image->class)
  {
    case DirectClass:
    {
      register unsigned long
        color,
        index;

      unsigned long
        blue_mask,
        blue_shift,
        green_mask,
        green_shift,
        red_mask,
        red_shift;

      /*
        Determine shift and mask for red, green, and blue.
      */
      red_mask=target_attributes.visual->red_mask;
      red_shift=0;
      while ((red_mask & 0x01) == 0)
      {
        red_mask>>=1;
        red_shift++;
      }
      green_mask=target_attributes.visual->green_mask;
      green_shift=0;
      while ((green_mask & 0x01) == 0)
      {
        green_mask>>=1;
        green_shift++;
      }
      blue_mask=target_attributes.visual->blue_mask;
      blue_shift=0;
      while ((blue_mask & 0x01) == 0)
      {
        blue_mask>>=1;
        blue_shift++;
      }
      /*
        Convert X image to DirectClass packets.
      */
      if ((number_colors > 0) &&
          (target_attributes.visual->class == DirectColor))
        for (y=0; y < image->rows; y++)
        {
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            index=(pixel >> red_shift) & red_mask;
            p->red=(unsigned char) (colors[index].red >> 8);
            index=(pixel >> green_shift) & green_mask;
            p->green=(unsigned char) (colors[index].green >> 8);
            index=(pixel >> blue_shift) & blue_mask;
            p->blue=(unsigned char) (colors[index].blue >> 8);
            p->index=0;
            p->length=0;
            p++;
          }
        }
      else
        for (y=0; y < image->rows; y++)
          for (x=0; x < image->columns; x++)
          {
            pixel=XGetPixel(ximage,x,y);
            color=(pixel >> red_shift) & red_mask;
            p->red=(unsigned char)
              ((((unsigned long) color*65535)/red_mask) >> 8);
            color=(pixel >> green_shift) & green_mask;
            p->green=(unsigned char)
              ((((unsigned long) color*65535)/green_mask) >> 8);
            color=(pixel >> blue_shift) & blue_mask;
            p->blue=(unsigned char)
              ((((unsigned long) color*65535)/blue_mask) >> 8);
            p->index=0;
            p->length=0;
            p++;
          }
      break;
    }
    case PseudoClass:
    {
      register unsigned short
        index;

      /*
        Create colormap.
      */
      image->colors=number_colors;
      image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
      if (image->colormap == (ColorPacket *) NULL)
        {
          Warning("unable to read image","memory allocation failed");
          DestroyImage(image);
          return((Image *) NULL);
        }
      for (i=0; i < image->colors; i++)
      {
        image->colormap[colors[i].pixel].red=colors[i].red >> 8;
        image->colormap[colors[i].pixel].green=colors[i].green >> 8;
        image->colormap[colors[i].pixel].blue=colors[i].blue >> 8;
      }
      /*
        Convert X image to PseudoClass packets.
      */
      for (y=0; y < image->rows; y++)
        for (x=0; x < image->columns; x++)
        {
          pixel=XGetPixel(ximage,x,y);
          index=(unsigned short) pixel;
          p->red=image->colormap[index].red;
          p->green=image->colormap[index].green;
          p->blue=image->colormap[index].blue;
          p->index=index;
          p->length=0;
          p++;
        }
      CompressColormap(image);
      break;
    }
  }
  /*
    Free image and colormap.
  */
  XDestroyImage(ximage);
  if (number_colors > 0)
    (void) free((char *) colors);
  XCloseDisplay(display);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  R e a d X C I m a g e                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ReadXCImage creates a constant image and initializes to the
%  background color of the X server and returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadXCImage routine is:
%
%      image=ReadXCImage(filename,server_name,density)
%
%  A description of each parameter follows:
%
%    o image:  Function ReadXCImage returns a pointer to the image after
%      creating it. A null image is returned if there is a a memory shortage
%      or if the image cannot be read.
%
%    o filename: Specifies the name of the image to read.
%
%    o server_name: Specifies the name of the X server to read fonts from.
%
%    o density: Specifies the vertical and horizonal density of the image.
%
%
*/
Image *ReadXCImage(filename,server_name,geometry)
char
  *filename,
  *server_name,
  *geometry;
{
  char
    *resource_value;

  Display
    *display;

  Image
    *image;

  int
    x,
    y;

  register int
    i;

  register RunlengthPacket
    *p;

  unsigned int
    height,
    width;

  XPixelInfo
    pixel_info;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database,
    server_database;

  XStandardColormap
    map_info;

  XVisualInfo
    *visual_info;

  /*
    Allocate image structure.
  */
  image=AllocateImage("XC");
  if (image == (Image *) NULL)
    return((Image *) NULL);
  (void) strcpy(image->filename,filename);
  /*
    Open X server connection.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    {
      Warning("unable to connect to X server",XDisplayName(server_name));
      return((Image *) NULL);
    }
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
  /*
    Initialize visual info.
  */
  visual_info=XBestVisualInfo(display,"default",(char *) NULL,
    (XStandardColormap *) NULL);
  if (visual_info == (XVisualInfo *) NULL)
    {
      Warning("unable to get visual",resource_info.visual_type);
      return((Image *) NULL);
    }
  /*
    Determine border color.
  */
  map_info.colormap=XDefaultColormap(display,visual_info->screen);
  XGetPixelInfo(display,visual_info,&map_info,&resource_info,(Image *) NULL,
    &pixel_info);
  /*
    Initialize Image structure.
  */
  width=512;
  height=512;
  if (geometry != (char *) NULL)
    (void) XParseGeometry(geometry,&x,&y,&width,&height);
  image->columns=width;
  image->rows=height;
  image->packets=image->columns*image->rows;
  image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  image->comments=(char *) malloc((strlen(image->filename)+2048)*sizeof(char));
  if ((image->pixels == (RunlengthPacket *) NULL) ||
      (image->comments == (char *) NULL))
    {
      Warning("unable to allocate image","memory allocation error");
      DestroyImage(image);
      return((Image *) NULL);
    }
  (void) sprintf(image->comments,"\n  Imported from constant file:  %s\n",
    image->filename);
  /*
    Create colormap.
  */
  image->colors=1;
  image->colormap=(ColorPacket *) malloc(image->colors*sizeof(ColorPacket));
  if (image->colormap == (ColorPacket *) NULL)
    {
      Warning("unable to create image","memory allocation failed");
      DestroyImage(image);
      return((Image *) NULL);
    }
  image->colormap[0].red=pixel_info.border_color.red >> 8;
  image->colormap[0].green=pixel_info.border_color.green >> 8;
  image->colormap[0].blue=pixel_info.border_color.blue >> 8;
  p=image->pixels;
  for (i=0; i < (image->columns*image->rows); i++)
  {
    p->red=image->colormap[0].red;
    p->green=image->colormap[0].green;
    p->blue=image->colormap[0].blue;
    p->index=0;
    p->length=0;
    p++;
  }
  /*
    Free resources.
  */
  XFree((void *) visual_info);
  XCloseDisplay(display);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X A n n o t a t e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XAnnotateImage annotates the image with text.
%
%  The format of the XAnnotateImage routine is:
%
%    status=XAnnotateImage(display,window,annotate_info,background,image)
%
%  A description of each parameter follows:
%
%    o status: Function XAnnotateImage returns True if the image is
%      successfully annotated with text.  False is returned is there is a
%      memory shortage.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o text: Specifies the text to annotate the image.
%
%    o annotate_info: Specifies a pointer to a XAnnotateInfo structure.
%
%    o background: Specifies whether the background color is included in
%      the annotation.  Must be either True or False;
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
unsigned int XAnnotateImage(display,window,annotate_info,background,image)
Display
  *display;

XWindowInfo
  *window;

XAnnotateInfo
  *annotate_info;

unsigned int
  background;

Image
  *image;
{
  GC
    graphic_context;

  Image
    *annotate_image;

  int
    x,
    y;

  Pixmap
    annotate_pixmap;

  register short int
    pixel;

  register RunlengthPacket
    *p,
    *q;

  RunlengthPacket
    background_color;

  unsigned int
    height,
    width;

  XGCValues
    graphic_context_value;

  XImage
    *annotate_ximage;

  XPixelInfo
    *pixel_info;

  /*
    Initialize annotated image.
  */
  if (!UncompressImage(image))
    return(False);
  /*
    Initialize annotated pixmap.
  */
  annotate_pixmap=XCreatePixmap(display,window->id,annotate_info->width,
    annotate_info->height,window->depth);
  if (annotate_pixmap == (Pixmap) NULL)
    return(False);
  /*
    Initialize graphics info.
  */
  graphic_context_value.background=0;
  graphic_context_value.foreground=(unsigned long) (~0);
  graphic_context_value.font=annotate_info->font_info->fid;
  graphic_context=XCreateGC(display,window->id,GCBackground | GCFont |
    GCForeground,&graphic_context_value);
  if (graphic_context == (GC) NULL)
    return(False);
  /*
    Draw text to pixmap.
  */
  XDrawImageString(display,annotate_pixmap,graphic_context,0,
    annotate_info->font_info->ascent,annotate_info->text,
    strlen(annotate_info->text));
  XFreeGC(display,graphic_context);
  /*
    Initialize annotated X image.
  */
  annotate_ximage=XGetImage(display,annotate_pixmap,0,0,annotate_info->width,
    annotate_info->height,AllPlanes,ZPixmap);
  if (annotate_ximage == (XImage *) NULL)
    return(False);
  XFreePixmap(display,annotate_pixmap);
  /*
    Initialize annotated image.
  */
  annotate_image=AllocateImage("MIFF");
  if (annotate_image == (Image *) NULL)
    return(False);
  annotate_image->columns=annotate_info->width;
  annotate_image->rows=annotate_info->height;
  annotate_image->packets=annotate_image->columns*annotate_image->rows;
  annotate_image->pixels=(RunlengthPacket *)
    malloc((unsigned int) image->packets*sizeof(RunlengthPacket));
  if (annotate_image->pixels == (RunlengthPacket *) NULL)
    {
      DestroyImage(annotate_image);
      return(False);
    }
  /*
    Transfer annotated X image to image.
  */
  pixel_info=window->pixel_info;
  q=annotate_image->pixels;
  for (y=0; y < annotate_image->rows; y++)
    for (x=0; x < annotate_image->columns; x++)
    {
      pixel=(short int) XGetPixel(annotate_ximage,x,y);
      if (pixel == 0)
        {
          /*
            Set this pixel to the background color.
          */
          q->red=pixel_info->background_color.red >> 8;
          q->green=pixel_info->background_color.green >> 8;
          q->blue=pixel_info->background_color.blue >> 8;
          q->index=pixel_info->background_index;
        }
      else
        {
          /*
            Set this pixel to the pen color.
          */
          q->red=pixel_info->annotate_color.red >> 8;
          q->green=pixel_info->annotate_color.green >> 8;
          q->blue=pixel_info->annotate_color.blue >> 8;
          q->index=pixel_info->annotate_index;
        }
      q->length=0;
      q++;
    }
  XDestroyImage(annotate_ximage);
  /*
    Determine annotate geometry.
  */
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  if ((width != annotate_image->columns) || (height != annotate_image->rows))
    TransformImage(&annotate_image,(char *) NULL,annotate_info->geometry,
      (char *) NULL);
  if (annotate_info->degrees != 0.0)
    {
      Image
        *rotated_image;

      /*
        Rotate image.
      */
      rotated_image=RotateImage(annotate_image,annotate_info->degrees,False);
      if (rotated_image == (Image *) NULL)
        return(False);
      DestroyImage(annotate_image);
      annotate_image=rotated_image;
    }
  /*
    Paste annotated image to image.
  */
  background_color.red=pixel_info->background_color.red >> 8;
  background_color.green=pixel_info->background_color.green >> 8;
  background_color.blue=pixel_info->background_color.blue >> 8;
  image->class=DirectClass;
  p=annotate_image->pixels;
  q=image->pixels+y*image->columns+x;
  for (y=0; y < annotate_image->rows; y++)
  {
    for (x=0; x < annotate_image->columns; x++)
    {
      if ((p->red != background_color.red) ||
          (p->green != background_color.green) ||
          (p->blue != background_color.blue))
        {
          /*
            Set this pixel to the pen color.
          */
          *q=(*p);
          q->index=pixel_info->annotate_index;
        }
      else
        if (background)
          {
            /*
              Set this pixel to the background color.
            */
            *q=(*p);
            q->index=pixel_info->background_index;
          }
      p++;
      q++;
    }
    q+=image->columns-annotate_image->columns;
  }
  DestroyImage(annotate_image);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t F o n t                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestFont returns the "best" font.  "Best" is defined as a font
%  specified in the X resource database or a font such that the text width
%  displayed with the font does not exceed the specified maximum width.
%
%  The format of the XBestFont routine is:
%
%      font=XBestFont(display,resource_info,text,maximum_width)
%
%  A description of each parameter follows:
%
%    o font: XBestFont returns a pointer to a XFontStruct structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o text: Specifies the text whose width is compared to the maximum.
%
%    o maximum_width: Specifies the maximum width in pixels of the text.
%
%
*/
XFontStruct *XBestFont(display,resource_info,text,maximum_width)
Display
  *display;

XResourceInfo
  *resource_info;

char
  *text;

unsigned int
  maximum_width;
{
  static char
    *fonts[]=
    {
      "fixed",
      "9x15",
      "6x13",
      "6x10",
      (char *) NULL
    };

  char
    *font_name,
    **p;

  unsigned int
    width;

  XFontStruct
    *font_info;

  font_info=(XFontStruct *) NULL;
  font_name=resource_info->font;
  if (font_name != (char *) NULL)
    {
      /*
        Load preferred font specified in the X resource database.
      */
      font_info=XLoadQueryFont(display,font_name);
      if (font_info == (XFontStruct *) NULL)
        Warning("unable to load font",font_name);
    }
  else
    if (text != (char *) NULL)
      {
        /*
          Load a font that does not exceed the text width.
        */
        for (p=fonts; *p != (char *) NULL; p++)
        {
          if (font_info != (XFontStruct *) NULL)
            {
              width=XTextWidth(font_info,text,strlen(text))+
                2*font_info->max_bounds.width+2*WindowBorderWidth;
              if (width < maximum_width)
                break;
              font_name=(*p);
              XFreeFont(display,font_info);
            }
          font_info=XLoadQueryFont(display,*p);
        }
      }
  if (font_info == (XFontStruct *) NULL)
    font_info=XLoadQueryFont(display,"fixed");  /* backup font */
  if (font_info == (XFontStruct *) NULL)
    font_info=XLoadQueryFont(display,"variable");  /* backup font */
  return(font_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t I c o n S i z e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestIconSize returns the "best" icon size.  "Best" is defined as
%  an icon size that maintains the aspect ratio of the image.  If the window
%  manager has preferred icon sizes, one of the preferred sizes is used.
%
%  The format of the XBestIconSize routine is:
%
%      XBestIconSize(display,window,image)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%
*/
void XBestIconSize(display,window,image)
Display
  *display;

XWindowInfo
  *window;

Image
  *image;
{
#define MaxIconSize  64

  int
    i,
    number_sizes;

  unsigned int
    height,
    icon_height,
    icon_width,
    width;

  unsigned long int
    scale_factor;

  Window
    root_window;

  XIconSize
    *icon_size,
    *size_list;

  /*
    Determine if the window manager has specified preferred icon sizes.
  */
  window->width=MaxIconSize;
  window->height=MaxIconSize;
  icon_size=(XIconSize *) NULL;
  number_sizes=0;
  root_window=XRootWindow(display,window->screen);
  if (XGetIconSizes(display,root_window,&size_list,&number_sizes) != 0)
    if ((number_sizes > 0) && (size_list != (XIconSize *) NULL))
      icon_size=size_list;
  if (icon_size == (XIconSize *) NULL)
    {
      /*
        Window manager does not restrict icon size.
      */
      icon_size=XAllocIconSize();
      if (icon_size == (XIconSize *) NULL)
        {
          Warning("unable to choose best icon size","memory allocation failed");
          return;
        }
      icon_size->min_width=1;
      icon_size->max_width=MaxIconSize;
      icon_size->min_height=1;
      icon_size->max_height=MaxIconSize;
      icon_size->width_inc=1;
      icon_size->height_inc=1;
    }
  /*
    Determine aspect ratio of image.
  */
  width=image->columns;
  height=image->rows;
  if (window->clip_geometry)
    (void) XParseGeometry(window->clip_geometry,&i,&i,&width,&height);
  /*
    Look for an icon size that maintains the aspect ratio of image.
  */
  scale_factor=UpShift(icon_size->max_width)/width;
  if (scale_factor > (UpShift(icon_size->max_height)/height))
    scale_factor=UpShift(icon_size->max_height)/height;
  icon_width=icon_size->min_width;
  while (icon_width < icon_size->max_width)
  {
    if (icon_width >= (DownShift(width*scale_factor)))
      break;
    icon_width+=icon_size->width_inc;
  }
  icon_height=icon_size->min_height;
  while (icon_height < icon_size->max_height)
  {
    if (icon_height >= (DownShift(height*scale_factor)))
      break;
    icon_height+=icon_size->height_inc;
  }
  XFree((void *) icon_size);
  window->width=icon_width;
  window->height=icon_height;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t P i x e l                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestPixel returns a pixel from an array of pixels that is closest
%  to the requested color.
%
%  The format of the XBestPixel routine is:
%
%      pixel=XBestPixel(colors,number_colors,color)
%
%  A description of each parameter follows:
%
%    o pixel: XBestPixel returns the pixel value closest to the requested
%      color.
%
%    o colors: Specifies an array of XColor structures.
%
%    o number_colors: Specifies the number of XColor structures in the
%      color definition array.
%
%    o color: Specifies the desired RGB value to find in the colors array.
%
%
*/
static void XBestPixel(colors,number_colors,color)
XColor
  *colors;

unsigned int
  number_colors;

XColor
  *color;
{
  register int
    blue_distance,
    green_distance,
    i,
    red_distance;

  register unsigned long
    distance,
    min_distance;

  /*
    Find closest representation for the requested RGB color.
  */
  color->pixel=0;
  min_distance=(unsigned long) (~0);
  for (i=0; i < number_colors; i++)
  {
    red_distance=(colors[i].red >> 8)-(color->red >> 8);
    green_distance=(colors[i].green >> 8)-(color->green >> 8);
    blue_distance=(colors[i].blue >> 8)-(color->blue >> 8);
    distance=red_distance*red_distance+green_distance*green_distance+
      blue_distance*blue_distance;
    if (distance < min_distance)
      {
        min_distance=distance;
        color->pixel=colors[i].pixel;
      }
  }
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X B e s t V i s u a l I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XBestVisualInfo returns visual information for a visual that is
%  the "best" the server supports.  "Best" is defined as:
%
%    1. Restrict the visual list to those supported by the default screen.
%
%    2. If a visual type is specified, restrict the visual list to those of
%       that type.
%
%    3. If a map type is specified, choose the visual that matches the id
%       specified by the Standard Colormap.
%
%    4  From the list of visuals, choose one that can display the most
%       simultaneous colors.  If more than one visual can display the same
%       number of simultaneous colors, one is choosen based on a rank.
%
%  The format of the XBestVisualInfo routine is:
%
%      visual_info=XBestVisualInfo(display,visual_type,map_type,map_info)
%
%  A description of each parameter follows:
%
%    o visual_info: XBestVisualInfo returns a pointer to a X11 XVisualInfo
%      structure.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o visual_type: Specifies the desired visual type.
%
%    o map_type: Specifies the desired Standard Colormap type.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%
*/
XVisualInfo *XBestVisualInfo(display,visual_type,map_type,map_info)
Display
  *display;

char
  *visual_type,
  *map_type;

XStandardColormap
  *map_info;
{
#define XVisualColormapSize(visual_info) \
  ((visual_info->class == TrueColor) || (visual_info->class == DirectColor) ? \
    visual_info->red_mask | visual_info->green_mask | visual_info->blue_mask : \
    visual_info->colormap_size)

  int
    number_visuals;

  register int
    i;

  unsigned int
    visual_mask;

  XVisualInfo
    *visual_info,
    *visual_list,
    visual_template;

  /*
    Restrict visual search by screen number.
  */
  visual_mask=VisualScreenMask;
  visual_template.screen=XDefaultScreen(display);
  if (visual_type != (char *) NULL)
    {
      /*
        Restrict visual search by class or visual id.
      */
      if (Latin1Compare("staticgray",visual_type) == 0)
        {
          visual_mask|=VisualClassMask;
          visual_template.class=StaticGray;
        }
      else
        if (Latin1Compare("grayscale",visual_type) == 0)
          {
            visual_mask|=VisualClassMask;
            visual_template.class=GrayScale;
          }
        else
          if (Latin1Compare("staticcolor",visual_type) == 0)
            {
              visual_mask|=VisualClassMask;
              visual_template.class=StaticColor;
            }
          else
            if (Latin1Compare("pseudocolor",visual_type) == 0)
              {
                visual_mask|=VisualClassMask;
                visual_template.class=PseudoColor;
              }
            else
              if (Latin1Compare("truecolor",visual_type) == 0)
                {
                  visual_mask|=VisualClassMask;
                  visual_template.class=TrueColor;
                }
              else
                if (Latin1Compare("directcolor",visual_type) == 0)
                  {
                    visual_mask|=VisualClassMask;
                    visual_template.class=DirectColor;
                  }
                else
                  if (Latin1Compare("default",visual_type) == 0)
                    {
                      visual_mask|=VisualIDMask;
                      visual_template.visualid=XVisualIDFromVisual(
                        XDefaultVisual(display,XDefaultScreen(display)));
                    }
                  else
                    if (isdigit(*visual_type))
                      {
                        visual_mask|=VisualIDMask;
                        visual_template.visualid=
                          strtol(visual_type,(char **) NULL,0);
                      }
                    else
                      Warning("invalid visual specifier",visual_type);
    }
  /*
    Get all visuals that meet our criteria so far.
  */
  number_visuals=0;
  visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  visual_mask=VisualScreenMask | VisualIDMask;
  if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
    {
      /*
        Failed to get visual;  try using the default visual.
      */
      Warning("unable to get visual",visual_type);
      visual_template.visualid=
        XVisualIDFromVisual(XDefaultVisual(display,XDefaultScreen(display)));
      visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
        &number_visuals);
      if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
        return((XVisualInfo *) NULL);
      Warning("using default visual",XVisualClassName(visual_list));
    }
  if (map_type != (char *) NULL)
    {
      Atom
        map_property;

      char
        map_name[2048];

      int
        j,
        number_maps,
        status;

      Window
        root_window;

      XStandardColormap
        *map_list;

      /*
        Restrict visual search by Standard Colormap visual id.
      */
      (void) sprintf((char *) map_name,"RGB_%s_MAP",map_type);
      Latin1Upper(map_name);
      map_property=XInternAtom(display,(char *) map_name,True);
      if (map_property == (Atom) NULL)
        Error("unable to get Standard Colormap",map_type);
      root_window=XRootWindow(display,XDefaultScreen(display));
      status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
        map_property);
      if (status == 0)
        Error("unable to get Standard Colormap",map_type);
      /*
        Search all Standard Colormaps and visuals for ids that match.
      */
      *map_info=map_list[0];
#ifndef PRE_R4_ICCCM
      visual_template.visualid=XVisualIDFromVisual(visual_list[0].visual);
      for (i=0; i < number_maps; i++)
        for (j=0; j < number_visuals; j++)
          if (map_list[i].visualid ==
              XVisualIDFromVisual(visual_list[j].visual))
            {
              *map_info=map_list[i];
              visual_template.visualid=
                XVisualIDFromVisual(visual_list[j].visual);
              break;
            }
      if (map_info->visualid != visual_template.visualid)
        Error("unable to match visual to Standard Colormap",map_type);
#endif
      if (map_info->colormap == (Colormap) NULL)
        Error("Standard Colormap is not initialized",map_type);
      XFree((void *) map_list);
    }
  else
    {
      static unsigned int
        rank[]=
          {
            StaticGray,
            GrayScale,
            StaticColor,
            DirectColor,
            TrueColor,
            PseudoColor
          };

      XVisualInfo
        *p;

      /*
        Pick one visual that displays the most simultaneous colors.
      */
      visual_info=visual_list;
      p=visual_list;
      for (i=1; i < number_visuals; i++)
      {
        p++;
        if (XVisualColormapSize(p) > XVisualColormapSize(visual_info))
          visual_info=p;
        else
          if (XVisualColormapSize(p) == XVisualColormapSize(visual_info))
            if (rank[p->class] > rank[visual_info->class])
              visual_info=p;
      }
      visual_template.visualid=XVisualIDFromVisual(visual_info->visual);
    }
  XFree((void *) visual_list);
  /*
    Retrieve only one visual by its screen & id number.
  */
  visual_info=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  if ((number_visuals == 0) || (visual_info == (XVisualInfo *) NULL))
    return((XVisualInfo *) NULL);
  return(visual_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X C l i e n t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XClientWindow finds a window, at or below the specified window,
%  which has a WM_STATE property.  If such a window is found, it is returned,
%  otherwise the argument window is returned.
%
%  The format of the XClientWindow function is:
%
%      client_window=XClientWindow(display,target_window)
%
%  A description of each parameter follows:
%
%    o client_window: XClientWindow returns a window, at or below the specified
%      window, which has a WM_STATE property otherwise the argument
%      target_window is returned.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o target_window: Specifies the window to find a WM_STATE property.
%
%
*/
static Window XClientWindow(display,target_window)
Display
  *display;

Window
  target_window;
{
  Atom
    state,
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned long
    after,
    number_items;

  Window
    client_window;

  state=XInternAtom(display,"WM_STATE",True);
  if (state == (Atom) NULL)
    return(target_window);
  type=(Atom) NULL;
  status=XGetWindowProperty(display,target_window,state,0L,0L,False,
    (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
  if ((status == Success) && (type != (Atom) NULL))
    return(target_window);
  client_window=XWindowByProperty(display,target_window,state);
  if (client_window == (Window) NULL)
    return(target_window);
  return(client_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X E r r o r                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XError ignores BadWindow errors for XQueryTree and
%  XGetWindowAttributes, and ignores BadDrawable errors for XGetGeometry, and
%  ignores BadValue errors for XQueryColor.  It returns False in those cases.
%  Otherwise it returns True.
%
%  The format of the XError function is:
%
%      XError(display,error)
%
%  A description of each parameter follows:
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o error: Specifies the error event.
%
%
*/
int XError(display,error)
Display
  *display;

XErrorEvent
  *error;
{
  switch (error->request_code)
  {
    case X_GetGeometry:
    {
      if (error->error_code == BadDrawable)
        return(False);
      break;
    }
    case X_GetWindowAttributes:
    case X_QueryTree:
    {
      if (error->error_code == BadWindow)
        return(False);
      break;
    }
    case X_QueryColors:
    {
      if (error->error_code == BadValue)
        return(False);
      break;
    }
  }
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X F r e e S t a n d a r d C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XFreeStandardColormap frees an X11 colormap.
%
%  The format of the XFreeStandardColormap routine is:
%
%      XFreeStandardColormap(display,visual_info,pixel_info,map_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%
*/
void XFreeStandardColormap(display,visual_info,pixel_info,map_info)
Display
  *display;

XVisualInfo
  *visual_info;

XPixelInfo
  *pixel_info;

XStandardColormap
  *map_info;
{
  /*
    Free colormap.
  */
  XFlush(display);
  if (map_info->colormap != (Colormap) NULL)
    if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
      XFreeColormap(display,map_info->colormap);
    else
      if ((visual_info->class != TrueColor) &&
          (visual_info->class != DirectColor))
        XFreeColors(display,map_info->colormap,pixel_info->pixels,
          (int) pixel_info->colors,0);
  map_info->colormap=(Colormap) NULL;
  if (pixel_info->pixels != (unsigned long *) NULL)
    (void) free((char *) pixel_info->pixels);
  pixel_info->pixels=(unsigned long *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t A n n o t a t e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetAnnotateInfo initializes the AnnotateInfo structure.
%
%  The format of the GetAnnotateInfo routine is:
%
%      XGetAnnotateInfo(alien_info)
%
%  A description of each parameter follows:
%
%    o annotate_info: Specifies a pointer to a XAnnotateInfo structure.
%
%
*/
void XGetAnnotateInfo(annotate_info)
XAnnotateInfo
  *annotate_info;
{
  /*
    Initialize annotate structure.
  */
  annotate_info->x=0;
  annotate_info->y=0;
  annotate_info->width=0;
  annotate_info->height=0;
  annotate_info->degrees=0.0;
  annotate_info->font_info=(XFontStruct *) NULL;
  annotate_info->text=(char *) NULL;
  *annotate_info->geometry=(char) NULL;
  annotate_info->previous=(XAnnotateInfo *) NULL;
  annotate_info->next=(XAnnotateInfo *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t P i x e l I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetPixelInfo initializes the PixelInfo structure.
%
%  The format of the XGetPixelInfo routine is:
%
%      XGetPixelInfo(display,visual_info,map_info,resource_info,image,
%        pixel_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o map_info: If map_type is specified, this structure is initialized
%      with info from the Standard Colormap.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%
*/
void XGetPixelInfo(display,visual_info,map_info,resource_info,image,pixel_info)
Display
  *display;

XVisualInfo
  *visual_info;

XStandardColormap
  *map_info;

XResourceInfo
  *resource_info;

Image
  *image;

XPixelInfo
  *pixel_info;
{
  register int
    i;

  int
    status;

  unsigned int
    packets;

  pixel_info->colors=0;
  if (image != (Image *) NULL)
    if (image->class == PseudoClass)
      pixel_info->colors=image->colors;
  packets=Max(pixel_info->colors,visual_info->colormap_size)+MaxNumberPens;
  pixel_info->pixels=(unsigned long *) malloc(packets*sizeof(unsigned long));
  if (pixel_info->pixels == (unsigned long *) NULL)
    Error("unable to get pixel info","memory allocation failed");
  status=XParseColor(display,map_info->colormap,resource_info->background_color,
    &pixel_info->background_color);
  if (status == 0)
    Warning("color is not known to X server",resource_info->background_color);
  pixel_info->background_color.pixel=
    XStandardPixel(map_info,pixel_info->background_color,16);
  pixel_info->background_color.flags=DoRed | DoGreen | DoBlue;
  status=XParseColor(display,map_info->colormap,resource_info->border_color,
    &pixel_info->border_color);
  if (status == 0)
    Warning("color is not known to X server",resource_info->border_color);
  pixel_info->border_color.pixel=
    XStandardPixel(map_info,pixel_info->border_color,16);
  pixel_info->border_color.flags=DoRed | DoGreen | DoBlue;
  status=XParseColor(display,map_info->colormap,resource_info->foreground_color,
    &pixel_info->foreground_color);
  if (status == 0)
    Warning("color is not known to X server",resource_info->foreground_color);
  pixel_info->foreground_color.pixel=
    XStandardPixel(map_info,pixel_info->foreground_color,16);
  pixel_info->foreground_color.flags=DoRed | DoGreen | DoBlue;
  for (i=0; i < MaxNumberPens; i++)
  {
    status=XParseColor(display,map_info->colormap,resource_info->pen_color[i],
      &pixel_info->pen_color[i]);
    if (status == 0)
      Warning("color is not known to X server",resource_info->pen_color[i]);
    pixel_info->pen_color[i].pixel=
      XStandardPixel(map_info,pixel_info->pen_color[i],16);
    pixel_info->pen_color[i].flags=DoRed | DoGreen | DoBlue;
  }
  pixel_info->annotate_color=pixel_info->foreground_color;
  if (image != (Image *) NULL)
    if (image->class == PseudoClass)
      {
        /*
          Initialize pixel array for images of type PseudoClass.
        */
        for (i=0; i < image->colors; i++)
          pixel_info->pixels[i]=XStandardPixel(map_info,image->colormap[i],8);
        for (i=0; i < MaxNumberPens; i++)
          pixel_info->pixels[image->colors+i]=pixel_info->pen_color[i].pixel;
        pixel_info->colors+=MaxNumberPens;
      }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetResource queries the X server for the specified resource name
%  or class.  If the resource name or class is not defined in the database, the
%  supplied default value is returned.
%
%  The format of the XGetResource routine is:
%
%      value=XGetResource(database,client,keyword,resource_default)
%
%  A description of each parameter follows:
%
%    o value: Function XGetResource returns the resource value associated with
%      the name or class.  If none is found, the supplied default value is
%      returned.
%
%    o database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o keyword: Specifies the keyword of the value being retrieved.
%
%    o resource_default: Specifies the default value to return if the query
%      fails to find the specified keyword/class.
%
%
*/
char *XGetResource(database,client,keyword,resource_default)
XrmDatabase
  database;

char
  *client,
  *keyword,
  *resource_default;
{
  char
    *resource_type,
    resource_class[2048],
    resource_name[2048];

  int
    status;

  XrmValue
    resource_value;

  if (database == (XrmDatabase) NULL)
    return(resource_default);
  *resource_name='\0';
  *resource_class='\0';
  if (keyword != (char *) NULL)
    {
      unsigned char
        c,
        k;
     
      /*
        Initialize resource keyword and class.
      */
      (void) sprintf(resource_name,"%s.%s\0",client,keyword);
      c=(*client);
      if ((c >= XK_a) && (c <= XK_z))
        c-=(XK_a-XK_A);
      else
        if ((c >= XK_agrave) && (c <= XK_odiaeresis))
          c-=(XK_agrave-XK_Agrave);
        else
          if ((c >= XK_oslash) && (c <= XK_thorn))
            c-=(XK_oslash-XK_Ooblique);
      k=(*keyword);
      if ((k >= XK_a) && (k <= XK_z))
        k-=(XK_a-XK_A);
      else
        if ((k >= XK_agrave) && (k <= XK_odiaeresis))
          k-=(XK_agrave-XK_Agrave);
        else
          if ((k >= XK_oslash) && (k <= XK_thorn))
            k-=(XK_oslash-XK_Ooblique);
      (void) sprintf(resource_class,"%c%s.%c%s\0",c,client+1,k,keyword+1);
    }
  status=XrmGetResource(database,resource_name,resource_class,&resource_type,
    &resource_value);
  if (status == False)
    return(resource_default);
  return(resource_value.addr);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t R e s o u r c e I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetResourceInfo initializes the ResourceInfo structure.
%
%  The format of the XGetResourceInfo routine is:
%
%      XGetResourceInfo(resource_database,client_name,resource_info)
%
%  A description of each parameter follows:
%
%    o resource_database: Specifies a resource database; returned from
%      XrmGetStringDatabase.
%
%    o client_name:  Specifies the application name used to retrieve
%      resource info from the X server database.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%
*/
void XGetResourceInfo(resource_database,client_name,resource_info)
XrmDatabase
  resource_database;

char
  *client_name;

XResourceInfo
  *resource_info;
{
  char
    *resource_value;

  /*
    Initialize resource info fields.
  */
  resource_value=XGetResource(resource_database,client_name,"backdrop","False");
  resource_info->backdrop=IsTrue(resource_value);
  resource_info->background_color=
    XGetResource(resource_database,client_name,"background","black");
  resource_info->border_color=
    XGetResource(resource_database,client_name,"borderColor","white");
  resource_value=XGetResource(resource_database,client_name,"borderWidth","2");
  resource_info->border_width=atoi(resource_value);
  resource_value=
    XGetResource(resource_database,client_name,"colormap","shared");
  resource_info->colormap=UndefinedColormap;
  if (Latin1Compare("private",resource_value) == 0)
    resource_info->colormap=PrivateColormap;
  if (Latin1Compare("shared",resource_value) == 0)
    resource_info->colormap=SharedColormap;
  if (resource_info->colormap == UndefinedColormap)
    Warning("unrecognized colormap type",resource_value);
  resource_value=XGetResource(resource_database,client_name,"colors","0");
  resource_info->number_colors=atoi(resource_value);
  resource_value=XGetResource(resource_database,client_name,"colorspace","rgb");
  resource_info->colorspace=UndefinedColorspace;
  if (Latin1Compare("gray",resource_value) == 0)
    resource_info->colorspace=GRAYColorspace;
  if (Latin1Compare("rgb",resource_value) == 0)
    resource_info->colorspace=RGBColorspace;
  if (Latin1Compare("yiq",resource_value) == 0)
    resource_info->colorspace=YIQColorspace;
  if (Latin1Compare("yuv",resource_value) == 0)
    resource_info->colorspace=YUVColorspace;
  if (Latin1Compare("xyz",resource_value) == 0)
    resource_info->colorspace=XYZColorspace;
  if (resource_info->colorspace == UndefinedColorspace)
    Warning("unrecognized colorspace type",resource_value);
  resource_value=XGetResource(resource_database,client_name,"debug","False");
  resource_info->debug=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"delay","0");
  resource_info->delay=atoi(resource_value);
  resource_value=XGetResource(resource_database,client_name,"dither","False");
  resource_info->dither=IsTrue(resource_value);
  resource_info->font=
    XGetResource(resource_database,client_name,"font",(char *) NULL);
  resource_info->font_name[0]=
    XGetResource(resource_database,client_name,"font1","fixed");
  resource_info->font_name[1]=
    XGetResource(resource_database,client_name,"font2","variable");
  resource_info->font_name[2]=
    XGetResource(resource_database,client_name,"font3","5x8");
  resource_info->font_name[3]=
    XGetResource(resource_database,client_name,"font4","6x10");
  resource_info->font_name[4]=
    XGetResource(resource_database,client_name,"font5","7x13bold");
  resource_info->font_name[5]=
    XGetResource(resource_database,client_name,"font6","8x13bold");
  resource_info->font_name[6]=
    XGetResource(resource_database,client_name,"font7","9x15bold");
  resource_info->font_name[7]=
    XGetResource(resource_database,client_name,"font8","10x20");
  resource_info->font_name[8]=
    XGetResource(resource_database,client_name,"font9","12x24");
  resource_info->foreground_color=
    XGetResource(resource_database,client_name,"foreground","white");
  resource_info->icon_geometry=
    XGetResource(resource_database,client_name,"iconGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,client_name,"gravity","Center");
  resource_info->gravity=(-1);
  if (Latin1Compare("Forget",resource_value) == 0)
    resource_info->gravity=ForgetGravity;
  if (Latin1Compare("NorthWest",resource_value) == 0)
    resource_info->gravity=NorthWestGravity;
  if (Latin1Compare("North",resource_value) == 0)
    resource_info->gravity=NorthGravity;
  if (Latin1Compare("NorthEast",resource_value) == 0)
    resource_info->gravity=NorthEastGravity;
  if (Latin1Compare("West",resource_value) == 0)
    resource_info->gravity=WestGravity;
  if (Latin1Compare("Center",resource_value) == 0)
    resource_info->gravity=CenterGravity;
  if (Latin1Compare("East",resource_value) == 0)
    resource_info->gravity=EastGravity;
  if (Latin1Compare("SouthWest",resource_value) == 0)
    resource_info->gravity=SouthWestGravity;
  if (Latin1Compare("South",resource_value) == 0)
    resource_info->gravity=SouthGravity;
  if (Latin1Compare("SouthEast",resource_value) == 0)
    resource_info->gravity=SouthEastGravity;
  if (Latin1Compare("Static",resource_value) == 0)
    resource_info->gravity=StaticGravity;
  if (resource_info->gravity == (-1))
    {
      Warning("unrecognized gravity type",resource_value);
      resource_info->gravity=CenterGravity;
    }
  resource_value=XGetResource(resource_database,client_name,"iconic","False");
  resource_info->iconic=IsTrue(resource_value);
  resource_info->image_geometry=
    XGetResource(resource_database,client_name,"imageGeometry",(char *) NULL);
  resource_value=XGetResource(resource_database,client_name,"magnify","2");
  resource_info->magnify=atoi(resource_value);
  resource_info->map_type=
    XGetResource(resource_database,client_name,"map",(char *) NULL);
  resource_value=
    XGetResource(resource_database,client_name,"monochrome","False");
  resource_info->monochrome=IsTrue(resource_value);
  resource_info->name=
    XGetResource(resource_database,client_name,"name",(char *) NULL);
  resource_info->pen_color[0]=
    XGetResource(resource_database,client_name,"pen1","black");
  resource_info->pen_color[1]=
    XGetResource(resource_database,client_name,"pen2","blue");
  resource_info->pen_color[2]=
    XGetResource(resource_database,client_name,"pen3","cyan");
  resource_info->pen_color[3]=
    XGetResource(resource_database,client_name,"pen4","green");
  resource_info->pen_color[4]=
    XGetResource(resource_database,client_name,"pen5","gray");
  resource_info->pen_color[5]=
    XGetResource(resource_database,client_name,"pen6","red");
  resource_info->pen_color[6]=
    XGetResource(resource_database,client_name,"pen7","magenta");
  resource_info->pen_color[7]=
    XGetResource(resource_database,client_name,"pen8","yellow");
  resource_info->pen_color[8]=
    XGetResource(resource_database,client_name,"pen9","white");
  resource_info->print_filename=
    XGetResource(resource_database,client_name,"printFilename",(char *) NULL);
  resource_info->title=
    XGetResource(resource_database,client_name,"title",(char *) NULL);
  resource_value=XGetResource(resource_database,client_name,"treeDepth","0");
  resource_info->tree_depth=atoi(resource_value);
  resource_info->write_filename=
    XGetResource(resource_database,client_name,"writeFilename",(char *) NULL);
  resource_info->visual_type=
    XGetResource(resource_database,client_name,"visual",(char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X G e t W i n d o w I n f o                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XGetWindowInfo initializes the XWindowInfo structure.
%
%  The format of the XGetWindowInfo routine is:
%
%      XGetWindowInfo(superclass,window)
%
%  A description of each parameter follows:
%
%    o superclass_window: Specifies a pointer to a XWindowInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
void XGetWindowInfo(superclass_window,window)
XWindowInfo
  *superclass_window,
  *window;
{
  Window
    id;

  /*
    Window is initialized from superclass-- protect window ID.
  */
  id=window->id;
  if (id == (Window) NULL)
    *window=(*superclass_window);
  else
    {
      Pixmap
        pixmap;

      XImage
        *ximage;

      /*
        Protect X image, and pixmap.
      */
      ximage=window->ximage;
      pixmap=window->pixmap;
      *window=(*superclass_window);
      window->ximage=ximage;
      window->pixmap=pixmap;
    }
  window->id=id;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImage creates an X11 image.  If the image size differs from
%  the X11 image size, the image is first resized.
%
%  The format of the XMakeImage routine is:
%
%      status=XMakeImage(display,resource_info,window,image,width,height)
%
%  A description of each parameter follows:
%
%    o status: Function XMakeImage returns True if the X image is
%      successfully created.  False is returned is there is a memory shortage.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o width: Specifies the width in pixels of the rectangular area to
%      display.
%
%    o height: Specifies the height in pixels of the rectangular area to
%      display.
%
%
*/
unsigned int XMakeImage(display,resource_info,window,image,width,height)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *window;

Image
  *image;

unsigned int
  width,
  height;
{
  Image
    *transformed_image;

  int
    format;

  XImage
    *ximage;

  if ((window->width == 0) || (window->height == 0))
    return(False);
  /*
    Display busy cursor.
  */
  XDefineCursor(display,window->id,window->busy_cursor);
  XFlush(display);
  if (image != (Image *) NULL)
    {
      /*
        Apply user transforms to the image.
      */
      image->orphan=True;
      transformed_image=image;
      if (window->clip_geometry)
        {
          Image
            *clipped_image;

          int
            clip_x,
            clip_y;

          unsigned int
            clip_height,
            clip_width;

          /*
            Clip image.
          */
          (void) XParseGeometry(window->clip_geometry,&clip_x,&clip_y,
            &clip_width,&clip_height);
          clipped_image=
            ClipImage(transformed_image,clip_x,clip_y,clip_width,clip_height);
          if (clipped_image != (Image *) NULL)
            {
              if (transformed_image != image)
                DestroyImage(transformed_image);
              transformed_image=clipped_image;
            }
        }
      if ((width != transformed_image->columns) ||
          (height != transformed_image->rows))
        {
          Image
            *scaled_image;

          /*
            Scale image.
          */
          scaled_image=ScaleImage(transformed_image,width,height);
          if (scaled_image != (Image *) NULL)
            {
              if (transformed_image != image)
                DestroyImage(transformed_image);
              transformed_image=scaled_image;
            }
        }
      width=transformed_image->columns;
      height=transformed_image->rows;
      image->orphan=False;
    }
  /*
    Create X image.
  */
  format=(window->depth == 1) ? XYBitmap : ZPixmap;
  ximage=XCreateImage(display,window->visual_info->visual,window->depth,format,
    0,(char *) NULL,width,height,XBitmapPad(display),0);
  if (ximage == (XImage *) NULL)
    {
      /*
        Unable to create X image.
      */
      XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"XImage:\n");
      (void) fprintf(stderr,"  width, height: %dx%d\n",ximage->width,
        ximage->height);
      (void) fprintf(stderr,"  format: %d\n",ximage->format);
      (void) fprintf(stderr,"  byte order: %d\n",ximage->byte_order);
      (void) fprintf(stderr,"  bitmap unit, bit order, pad: %d %d %d\n",
        ximage->bitmap_unit,ximage->bitmap_bit_order,ximage->bitmap_pad);
      (void) fprintf(stderr,"  depth: %d\n",ximage->depth);
      (void) fprintf(stderr,"  bytes per line: %d\n",ximage->bytes_per_line);
      (void) fprintf(stderr,"  bits per pixel: %d\n",ximage->bits_per_pixel);
      (void) fprintf(stderr,"  red, green, blue masks: 0x%lx 0x%lx 0x%lx\n",
        ximage->red_mask,ximage->green_mask,ximage->blue_mask);
    }
  /*
    Allocate X image pixel data.
  */
  if (ximage->format == XYBitmap)
    ximage->data=(char *)
      malloc(ximage->bytes_per_line*ximage->height*ximage->depth);
  else
    ximage->data=(char *) malloc(ximage->bytes_per_line*ximage->height);
  if (ximage->data == (char *) NULL)
    {
      /*
        Unable to allocate pixel data.
      */
      XDestroyImage(ximage);
      XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  if (window->ximage != (XImage *) NULL)
    XDestroyImage(window->ximage);
  window->ximage=ximage;
  if (image == (Image *) NULL)
    {
      XDefineCursor(display,window->id,window->cursor);
      return(True);
    }
  /*
    Convert runlength-encoded pixels to X image data.
  */
  if ((ximage->byte_order == LSBFirst) ||
      ((ximage->format == XYBitmap) && (ximage->bitmap_bit_order == LSBFirst)))
    XMakeImageLSBFirst(window,transformed_image,ximage);
  else
    XMakeImageMSBFirst(window,transformed_image,ximage);
  if (transformed_image != image)
    DestroyImage(transformed_image);
  /*
    Restore cursor.
  */
  XDefineCursor(display,window->id,window->cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e L S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImageLSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in least-significant bit and byte first
%  order.  The server's scanline pad is respected.  Rather than using one or
%  two general cases, many special cases are found here to help speed up the
%  image conversion.
%
%  The format of the XMakeImageLSBFirst routine is:
%
%      XMakeImageLSBFirst(window,image,ximage)
%
%  A description of each parameter follows:
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
static void XMakeImageLSBFirst(window,image,ximage)
XWindowInfo
  *window;

Image
  *image;

XImage
  *ximage;
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register unsigned long
    pixel;

  unsigned int
    scanline_pad;

  unsigned long
    *pixels;

  pixels=window->pixel_info->pixels;
  p=image->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned char
        background,
        bit,
        byte,
        foreground;

      register unsigned short
        polarity;

      /*
        Convert image to big-endian bitmap.
      */
      background=(Intensity(window->pixel_info->foreground_color) >
        Intensity(window->pixel_info->background_color) ? 1 : 0) << 7;
      foreground=(Intensity(window->pixel_info->background_color) >
        Intensity(window->pixel_info->foreground_color) ? 1 : 0) << 7;
      polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
      scanline_pad=ximage->bytes_per_line-(ximage->width >> 3);
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte>>=1;
          if (p->index == polarity)
            byte|=foreground;
          else
            byte|=background;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == ximage->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                *q=byte >> (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
  else
    {
      XStandardColormap
        *map_info;

      /*
        Convert image to little-endian color-mapped X image.
      */
      map_info=window->map_info;
      scanline_pad=ximage->bytes_per_line-
        ((ximage->width*ximage->bits_per_pixel) >> 3);
      if (window->pixel_info->colors > 0)
        switch (ximage->bits_per_pixel)
        {
          case 2:
          {
            register unsigned int
              nibble;

            /*
              Convert to 2 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= ((int) p->length); j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) pixel;
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) (pixel << 2);
                    nibble++;
                    break;
                  }
                  case 2:
                  {
                    *q|=(unsigned char) (pixel << 4);
                    nibble++;
                    break;
                  }
                  case 3:
                  {
                    *q|=(unsigned char) (pixel << 6);
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 4:
          {
            register unsigned int
              nibble;

            /*
              Convert to 4 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= ((int) p->length); j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) pixel;
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) (pixel << 4);
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 6:
          case 8:
          {
            /*
              Convert to 8 bit color-mapped X image.
            */
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (j=0; j <= ((int) p->length); j++)
              {
                *q++=(unsigned char) pixel;
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          default:
          {
            register int
              k;

            register unsigned int
              bytes_per_pixel;

            unsigned char
              channel[sizeof(unsigned long)];

            /*
              Convert to multi-byte color-mapped X image.
            */
            bytes_per_pixel=ximage->bits_per_pixel >> 3;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (k=0; k < bytes_per_pixel; k++)
              {
                channel[k]=(unsigned char) pixel;
                pixel>>=8;
              }
              for (j=0; j <= ((int) p->length); j++)
              {
                for (k=0; k < bytes_per_pixel; k++)
                  *q++=channel[k];
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
        }
      else
        {
          /*
            Convert image to little-endian continuous-tone X image.
          */
          switch (ximage->bits_per_pixel)
          {
            case 2:
            {
              register unsigned int
                nibble;

              /*
                Convert to contiguous 2 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= ((int) p->length); j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) pixel;
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) (pixel << 2);
                      nibble++;
                      break;
                    }
                    case 2:
                    {
                      *q|=(unsigned char) (pixel << 4);
                      nibble++;
                      break;
                    }
                    case 3:
                    {
                      *q|=(unsigned char) (pixel << 6);
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 4:
            {
              register unsigned int
                nibble;

              /*
                Convert to contiguous 4 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= ((int) p->length); j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) pixel;
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) (pixel << 4);
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 6:
            case 8:
            {
              /*
                Convert to contiguous 8 bit continuous-tone X image.
              */
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                for (j=0; j <= ((int) p->length); j++)
                {
                  *q++=(unsigned char) pixel;
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            default:
            {
              if ((ximage->bits_per_pixel == 32) &&
                  (map_info->red_max == 255) &&
                  (map_info->green_max == 255) &&
                  (map_info->blue_max == 255) &&
                  (map_info->red_mult == 65536) &&
                  (map_info->green_mult == 256) &&
                  (map_info->blue_mult == 1))
                {
                  /*
                    Convert to 32 bit continuous-tone X image.
                  */
                  for (i=0; i < image->packets; i++)
                  {
                    for (j=0; j <= ((int) p->length); j++)
                    {
                      *q++=p->blue;
                      *q++=p->green;
                      *q++=p->red;
                      *q++=0;
                    }
                    p++;
                  }
                }
              else
                {
                  register int
                    k;

                  register unsigned int
                    bytes_per_pixel;

                  unsigned char
                    channel[sizeof(unsigned long)];

                  /*
                    Convert to multi-byte continuous-tone X image.
                  */
                  bytes_per_pixel=ximage->bits_per_pixel >> 3;
                  for (i=0; i < image->packets; i++)
                  {
                    pixel=XStandardPixel(map_info,(*p),8);
                    for (k=0; k < bytes_per_pixel; k++)
                    {
                      channel[k]=(unsigned char) pixel;
                      pixel>>=8;
                    }
                    for (j=0; j <= ((int) p->length); j++)
                    {
                      for (k=0; k < bytes_per_pixel; k++)
                        *q++=channel[k];
                      x++;
                      if (x == ximage->width)
                        {
                          x=0;
                          q+=scanline_pad;
                        }
                    }
                    p++;
                  }
                }
              break;
            }
          }
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I m a g e M S B F i r s t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeImageMSBFirst initializes the pixel data of an X11 Image.
%  The X image pixels are copied in most-significant bit and byte first order.
%  The server's scanline pad is also resprected. Rather than using one or two
%  general cases, many special cases are found here to help speed up the image
%  conversion.
%
%  The format of the XMakeImageMSBFirst routine is:
%
%      XMakeImageMSBFirst(window,image,ximage)
%
%  A description of each parameter follows:
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o ximage: Specifies a pointer to a XImage structure;  returned from
%      XCreateImage.
%
%
*/
static void XMakeImageMSBFirst(window,image,ximage)
XWindowInfo
  *window;

Image
  *image;

XImage
  *ximage;
{
  register int
    i,
    j,
    x;

  register RunlengthPacket
    *p;

  register unsigned char
    *q;

  register unsigned long
    pixel;

  unsigned int
    scanline_pad;

  unsigned long
    *pixels;

  pixels=window->pixel_info->pixels;
  p=image->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  if (ximage->format == XYBitmap)
    {
      register unsigned char
        background,
        bit,
        byte,
        foreground;

      register unsigned short
        polarity;

      /*
        Convert image to big-endian bitmap.
      */
      background=(Intensity(window->pixel_info->foreground_color) >
        Intensity(window->pixel_info->background_color));
      foreground=(Intensity(window->pixel_info->background_color) >
        Intensity(window->pixel_info->foreground_color));
      polarity=Intensity(image->colormap[0]) > Intensity(image->colormap[1]);
      scanline_pad=ximage->bytes_per_line-(ximage->width >> 3);
      bit=0;
      byte=0;
      for (i=0; i < image->packets; i++)
      {
        for (j=0; j <= ((int) p->length); j++)
        {
          byte<<=1;
          if (p->index == polarity)
            byte|=foreground;
          else
            byte|=background;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          x++;
          if (x == ximage->width)
            {
              /*
                Advance to the next scanline.
              */
              if (bit != 0)
                *q=byte << (8-bit);
              q+=scanline_pad;
              bit=0;
              byte=0;
              x=0;
            }
        }
        p++;
      }
    }
  else
    {
      XStandardColormap
        *map_info;

      /*
        Convert image to big-endian X image.
      */
      map_info=window->map_info;
      scanline_pad=ximage->bytes_per_line-
        ((ximage->width*ximage->bits_per_pixel) >> 3);
      if (window->pixel_info->colors > 0)
        switch (ximage->bits_per_pixel)
        {
          case 2:
          {
            register unsigned int
              nibble;

            /*
              Convert to 2 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= ((int) p->length); j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) (pixel << 6);
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) (pixel << 4);
                    nibble++;
                    break;
                  }
                  case 2:
                  {
                    *q|=(unsigned char) (pixel << 2);
                    nibble++;
                    break;
                  }
                  case 3:
                  {
                    *q|=(unsigned char) pixel;
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 4:
          {
            register unsigned int
              nibble;

            /*
              Convert to 4 bit color-mapped X image.
            */
            nibble=0;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index] & 0xf;
              for (j=0; j <= ((int) p->length); j++)
              {
                switch (nibble)
                {
                  case 0:
                  {
                    *q=(unsigned char) (pixel << 4);
                    nibble++;
                    break;
                  }
                  case 1:
                  {
                    *q|=(unsigned char) pixel;
                    q++;
                    nibble=0;
                    break;
                  }
                }
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    nibble=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          case 8:
          {
            /*
              Convert to 8 bit color-mapped X image.
            */
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (j=0; j <= ((int) p->length); j++)
              {
                *q++=(unsigned char) pixel;
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
          default:
          {
            register int
              k;

            register unsigned int
              bytes_per_pixel;

            unsigned char
              channel[sizeof(unsigned long)];

            /*
              Convert to 8 bit color-mapped X image.
            */
            bytes_per_pixel=ximage->bits_per_pixel >> 3;
            for (i=0; i < image->packets; i++)
            {
              pixel=pixels[p->index];
              for (k=bytes_per_pixel-1; k >= 0; k--)
              {
                channel[k]=(unsigned char) pixel;
                pixel>>=8;
              }
              for (j=0; j <= ((int) p->length); j++)
              {
                for (k=0; k < bytes_per_pixel; k++)
                  *q++=channel[k];
                x++;
                if (x == ximage->width)
                  {
                    x=0;
                    q+=scanline_pad;
                  }
              }
              p++;
            }
            break;
          }
        }
      else
        {
          /*
            Convert to big-endian continuous-tone X image.
          */
          switch (ximage->bits_per_pixel)
          {
            case 2:
            {
              register unsigned int
                nibble;

              /*
                Convert to 4 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= ((int) p->length); j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) (pixel << 6);
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) (pixel << 4);
                      nibble++;
                      break;
                    }
                    case 2:
                    {
                      *q|=(unsigned char) (pixel << 2);
                      nibble++;
                      break;
                    }
                    case 3:
                    {
                      *q|=(unsigned char) pixel;
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 4:
            {
              register unsigned int
                nibble;

              /*
                Convert to 4 bit continuous-tone X image.
              */
              nibble=0;
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                pixel&=0xf;
                for (j=0; j <= ((int) p->length); j++)
                {
                  switch (nibble)
                  {
                    case 0:
                    {
                      *q=(unsigned char) (pixel << 4);
                      nibble++;
                      break;
                    }
                    case 1:
                    {
                      *q|=(unsigned char) pixel;
                      q++;
                      nibble=0;
                      break;
                    }
                  }
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      nibble=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            case 8:
            {
              /*
                Convert to 8 bit continuous-tone X image.
              */
              for (i=0; i < image->packets; i++)
              {
                pixel=XStandardPixel(map_info,(*p),8);
                for (j=0; j <= ((int) p->length); j++)
                {
                  *q++=(unsigned char) pixel;
                  x++;
                  if (x == ximage->width)
                    {
                      x=0;
                      q+=scanline_pad;
                    }
                }
                p++;
              }
              break;
            }
            default:
            {
              if ((ximage->bits_per_pixel == 32) &&
                  (map_info->red_max == 255) &&
                  (map_info->green_max == 255) &&
                  (map_info->blue_max == 255) &&
                  (map_info->red_mult == 65536) &&
                  (map_info->green_mult == 256) &&
                  (map_info->blue_mult == 1))
                {
                  /*
                    Convert to 32 bit continuous-tone X image.
                  */
                  for (i=0; i < image->packets; i++)
                  {
                    for (j=0; j <= ((int) p->length); j++)
                    {
                      *q++=0;
                      *q++=p->red;
                      *q++=p->green;
                      *q++=p->blue;
                    }
                    p++;
                  }
                }
              else
                {
                  register int
                    k;

                  register unsigned int
                    bytes_per_pixel;

                  unsigned char
                    channel[sizeof(unsigned long)];

                  /*
                    Convert to multi-byte continuous-tone X image.
                  */
                  bytes_per_pixel=ximage->bits_per_pixel >> 3;
                  for (i=0; i < image->packets; i++)
                  {
                    pixel=XStandardPixel(map_info,(*p),8);
                    for (k=bytes_per_pixel-1; k >= 0; k--)
                    {
                      channel[k]=(unsigned char) pixel;
                      pixel>>=8;
                    }
                    for (j=0; j <= ((int) p->length); j++)
                    {
                      for (k=0; k < bytes_per_pixel; k++)
                        *q++=channel[k];
                      x++;
                      if (x == ximage->width)
                        {
                          x=0;
                          q+=scanline_pad;
                        }
                    }
                    p++;
                  }
                }
              break;
            }
          }
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e I n v i s i b l e C u r s o r                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeInvisibleCursor creates an invisible X11 cursor.
%
%  The format of the XMakeInvisibleCursor routine is:
%
%      XMakeInvisibleCursor(display,window)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies the ID of the window for which the cursor is
%      assigned.
%
%
*/
Cursor XMakeInvisibleCursor(display,window)
Display
  *display;

Window
  window;
{
  Cursor
    cursor;

  Pixmap
    pixmap;

  XColor
    color;

  static char
    bits[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

  color.red=0;
  color.green=0;
  color.blue=0;
  pixmap=XCreateBitmapFromData(display,window,bits,8,8);
  cursor=XCreatePixmapCursor(display,pixmap,pixmap,&color,&color,0,0);
  XFreePixmap(display,pixmap);
  return(cursor);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e P i x m a p                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakePixmap creates an X11 pixmap.
%
%  The format of the XMakePixmap routine is:
%
%      status=XMakePixmap(display,resource_info,window)
%
%  A description of each parameter follows:
%
%    o status: Function XMakePixmap returns True if the X pixmap is
%      successfully created.  False is returned is there is a memory shortage.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%
*/
unsigned int XMakePixmap(display,resource_info,window)
Display
  *display;

XResourceInfo
  *resource_info;

XWindowInfo
  *window;
{
  if (window->ximage == (XImage *) NULL)
    return(False);
  /*
    Display busy cursor.
  */
  XDefineCursor(display,window->id,window->busy_cursor);
  XFlush(display);
  /*
    Create pixmap.
  */
  if (window->pixmap != (Pixmap) NULL)
    XFreePixmap(display,window->pixmap);
  window->pixmap=XCreatePixmap(display,window->id,(unsigned int)
    window->ximage->width,(unsigned int) window->ximage->height,window->depth);
  if (window->pixmap == (Pixmap) NULL)
    {
      /*
        Unable to allocate pixmap.
      */
      XDefineCursor(display,window->id,window->cursor);
      return(False);
    }
  /*
    Copy X image to pixmap.
  */
  XPutImage(display,window->pixmap,window->graphic_context,window->ximage,
    0,0,0,0,(unsigned int) window->ximage->width,
    (unsigned int) window->ximage->height);
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"Pixmap:\n");
      (void) fprintf(stderr,"  width, height: %dx%d\n",window->ximage->width,
        window->ximage->height);
    }
  /*
    Restore cursor.
  */
  XDefineCursor(display,window->id,window->cursor);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e S t a n d a r d C o l o r m a p                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeStandardColormap creates an X11 Standard Colormap.
%
%  The format of the XMakeStandardColormap routine is:
%
%      XMakeStandardColormap(display,visual_info,resource_info,pixel_info,
%        image,map_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%    o resource_info: Specifies a pointer to a X11 XResourceInfo structure.
%
%    o pixel_info: Specifies a pointer to a XPixelInfo structure.
%
%    o image: Specifies a pointer to a Image structure;  returned from
%      ReadImage.
%
%    o map_info: If a Standard Colormap type is specified, this structure is
%      initialized with info from the Standard Colormap.
%
%
*/
static int IntensityCompare(x,y)
const void
  *x,
  *y;
{
  DiversityPacket
    *color_1,
    *color_2;
 
  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  return((int) Intensity(*color_2)-(int) Intensity(*color_1));
}

static int PopularityCompare(x,y)
const void
  *x,
  *y;
{
  DiversityPacket
    *color_1,
    *color_2;

  color_1=(DiversityPacket *) x;
  color_2=(DiversityPacket *) y;
  return((int) color_2->count-(int) color_1->count);
}

void XMakeStandardColormap(display,visual_info,resource_info,pixel_info,image,
  map_info)
Display
  *display;

XVisualInfo
  *visual_info;

XResourceInfo
  *resource_info;

XPixelInfo
  *pixel_info;

Image
  *image;

XStandardColormap
  *map_info;
{
  Colormap
    colormap;

  int
    status;

  register int
    i;

  unsigned int
    gray_value,
    number_colors,
    retain_colors;

  XColor
    color,
    *colors,
    *p;

  if (resource_info->map_type != (char *) NULL)
    {
      /*
        Standard Colormap is already defined (i.e. xstdcmap).
      */
      if (pixel_info->pixels != (unsigned long *) NULL)
        (void) free((char *) pixel_info->pixels);
      XGetPixelInfo(display,visual_info,map_info,resource_info,image,
        pixel_info);
      return;
    }
  if ((visual_info->class != DirectColor) && (visual_info->class != TrueColor))
    if ((image->class == DirectClass) ||
        (image->colors > visual_info->colormap_size))
      {
        /*
          Image has more colors than the visual supports.
        */
        QuantizeImage(image,(unsigned int) visual_info->colormap_size,
          resource_info->tree_depth,resource_info->dither,
          resource_info->colorspace,False);
        image->class=DirectClass;  /* promote to DirectClass */
      }
  /*
    Free previous and create new colormap.
  */
  XFreeStandardColormap(display,visual_info,pixel_info,map_info);
  colormap=XDefaultColormap(display,visual_info->screen);
  if (visual_info->visual != XDefaultVisual(display,visual_info->screen))
    colormap=XCreateColormap(display,
      XRootWindow(display,visual_info->screen),visual_info->visual,
      visual_info->class == DirectColor ? AllocAll : AllocNone);
  if (colormap == (Colormap) NULL)
    Error("unable to create colormap",(char *) NULL);
  /*
    Initialize the Standard Colormap attributes.
  */
  map_info->colormap=colormap;
  map_info->red_max=visual_info->red_mask;
  map_info->red_mult=map_info->red_max > 0 ? 1 : 0;
  if (map_info->red_max > 0)
    while ((map_info->red_max & 0x01) == 0)
    {
      map_info->red_max>>=1;
      map_info->red_mult<<=1;
    }
  map_info->green_max=visual_info->green_mask;
  map_info->green_mult=map_info->green_max > 0 ? 1 : 0;
  if (map_info->green_max > 0)
    while ((map_info->green_max & 0x01) == 0)
    {
      map_info->green_max>>=1;
      map_info->green_mult<<=1;
    }
  map_info->blue_max=visual_info->blue_mask;
  map_info->blue_mult=map_info->blue_max > 0 ? 1 : 0;
  if (map_info->blue_max > 0)
    while ((map_info->blue_max & 0x01) == 0)
    {
      map_info->blue_max>>=1;
      map_info->blue_mult<<=1;
    }
  map_info->base_pixel=0;
  XGetPixelInfo(display,visual_info,map_info,resource_info,image,pixel_info);
  /*
    Allocating colors in server colormap is based on visual class.
  */
  switch (visual_info->class)
  {
    case StaticGray:
    case StaticColor:
    {
      /*
        Define Standard Colormap for StaticGray or StaticColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *) malloc(visual_info->colormap_size*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("unable to create colormap","memory allocation failed");
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      if (visual_info->class == StaticColor)
        for (i=0; i < image->colors; i++)
        {
          color.red=(unsigned short) (image->colormap[i].red << 8);
          color.green=(unsigned short) (image->colormap[i].green << 8);
          color.blue=(unsigned short) (image->colormap[i].blue << 8);
          status=XAllocColor(display,colormap,&color);
          if (status == 0)
            {
              colormap=XCopyColormapAndFree(display,colormap);
              XAllocColor(display,colormap,&color);
            }
          pixel_info->pixels[i]=color.pixel;
          *p++=color;
        }
      else
        for (i=0; i < image->colors; i++)
        {
          gray_value=Intensity(image->colormap[i]);
          color.red=(unsigned short) (gray_value << 8);
          color.green=(unsigned short) (gray_value << 8);
          color.blue=(unsigned short) (gray_value << 8);
          status=XAllocColor(display,colormap,&color);
          if (status == 0)
            {
              colormap=XCopyColormapAndFree(display,colormap);
              XAllocColor(display,colormap,&color);
            }
          pixel_info->pixels[i]=color.pixel;
          *p++=color;
        }
      break;
    }
    case GrayScale:
    case PseudoColor:
    {
      unsigned int
        colormap_type;

      /*
        Define Standard Colormap for GrayScale or PseudoColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *) malloc(visual_info->colormap_size*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("unable to create colormap","memory allocation failed");
      /*
        Determine if image colors will "fit" into X server colormap.
      */
      colormap_type=resource_info->colormap;
      status=XAllocColorCells(display,colormap,False,
        (unsigned long *) NULL,0,pixel_info->pixels,image->colors);
      if (status != 0)
        colormap_type=PrivateColormap;
      if (colormap_type == SharedColormap)
        {
          DiversityPacket
            *diversity;

          register RunlengthPacket
            *q;

          unsigned short
            index;

          /*
            Define Standard colormap for shared GrayScale or PseudoColor visual:
          */
          diversity=(DiversityPacket *)
            malloc(image->colors*sizeof(DiversityPacket));
          if (diversity == (DiversityPacket *) NULL)
            Error("unable to create colormap","memory allocation failed");
          for (i=0; i < image->colors; i++)
          {
            diversity[i].red=image->colormap[i].red;
            diversity[i].green=image->colormap[i].green;
            diversity[i].blue=image->colormap[i].blue;
            diversity[i].index=(unsigned short) i;
            diversity[i].count=0;
          }
          q=image->pixels;
          for (i=0; i < image->packets; i++)
          {
            diversity[q->index].count+=(q->length+1);
            q++;
          }
          /*
            Sort colors by decreasing intensity.
          */
          (void) qsort((void *) diversity,(int) image->colors,
            sizeof(DiversityPacket),IntensityCompare);
          for (i=0; i < image->colors; i+=Max(image->colors >> 4,2))
            diversity[i].count<<=4;  /* increase this colors popularity */
          diversity[image->colors-1].count<<=4;
          (void) qsort((void *) diversity,(int) image->colors,
            sizeof(DiversityPacket),PopularityCompare);
          /*
            Allocate colors.
          */
          p=colors;
          color.flags=DoRed | DoGreen | DoBlue;
          if (visual_info->class == PseudoColor)
            for (i=0; i < image->colors; i++)
            {
              index=diversity[i].index;
              color.red=(unsigned short) (image->colormap[index].red << 8);
              color.green=(unsigned short) (image->colormap[index].green << 8);
              color.blue=(unsigned short) (image->colormap[index].blue << 8);
              status=XAllocColor(display,colormap,&color);
              if (status == 0)
                break;
              pixel_info->pixels[index]=color.pixel;
              *p++=color;
            }
          else
            for (i=0; i < image->colors; i++)
            {
              index=diversity[i].index;
              gray_value=Intensity(image->colormap[index]);
              color.red=(unsigned short) (gray_value << 8);
              color.green=(unsigned short) (gray_value << 8);
              color.blue=(unsigned short) (gray_value << 8);
              status=XAllocColor(display,colormap,&color);
              if (status == 0)
                break;
              pixel_info->pixels[index]=color.pixel;
              *p++=color;
            }
          if (i < image->colors)
            {
              register int
                j;

              XColor
                *server_colors;

              /*
                Read X server colormap.
              */
              server_colors=(XColor *)
                malloc(visual_info->colormap_size*sizeof(XColor));
              if (server_colors == (XColor *) NULL)
                Error("unable to create colormap","memory allocation failed");
              for (j=0; j < visual_info->colormap_size; j++)
                server_colors[j].pixel=(unsigned long) j;
              XQueryColors(display,colormap,server_colors,
                (int) Min(visual_info->colormap_size,256));
              /*
                Select remaining colors from X server colormap.
              */
              if (visual_info->class == PseudoColor)
                for (; i < image->colors; i++)
                {
                  index=diversity[i].index;
                  color.red=(unsigned short) (image->colormap[index].red << 8);
                  color.green=(unsigned short)
                    (image->colormap[index].green << 8);
                  color.blue=(unsigned short)
                    (image->colormap[index].blue << 8);
                  XBestPixel(server_colors,(unsigned int)
                    Min(visual_info->colormap_size,256),&color);
                  XAllocColor(display,colormap,&server_colors[color.pixel]);
                  pixel_info->pixels[index]=color.pixel;
                  *p++=color;
                }
              else
                for (; i < image->colors; i++)
                {
                  index=diversity[i].index;
                  gray_value=Intensity(image->colormap[index]);
                  color.red=(unsigned short) (gray_value << 8);
                  color.green=(unsigned short) (gray_value << 8);
                  color.blue=(unsigned short) (gray_value << 8);
                  XBestPixel(server_colors,(unsigned int)
                    Min(visual_info->colormap_size,256),&color);
                  XAllocColor(display,colormap,&server_colors[color.pixel]);
                  pixel_info->pixels[index]=color.pixel;
                  *p++=color;
                }
              if (image->colors < visual_info->colormap_size)
                {
                  /*
                    Fill up colors array-- more choices for pen colors.
                  */
                  retain_colors=
                    Min(visual_info->colormap_size-image->colors,256);
                  for (i=0; i < retain_colors; i++)
                    *p++=server_colors[i];
                  number_colors+=retain_colors;
                }
              (void) free((char *) server_colors);
            }
          (void) free((char *) diversity);
          break;
        }
      /*
        Define Standard colormap for private GrayScale or PseudoColor visual.
      */
      if (status == 0)
        {
          /*
            Not enough colormap entries in the colormap-- Create a new colormap.
          */
          colormap=XCreateColormap(display,
            XRootWindow(display,visual_info->screen),visual_info->visual,
            AllocNone);
          if (colormap == (Colormap) NULL)
            Error("unable to create colormap",(char *) NULL);
          map_info->colormap=colormap;
          if (image->colors < visual_info->colormap_size)
            {
              /*
                Retain colors from the default colormap to help lessens the
                effects of colormap flashing.
              */
              retain_colors=Min(visual_info->colormap_size-image->colors,256);
              p=colors+image->colors;
              for (i=0; i < retain_colors; i++)
              {
                p->pixel=(unsigned long) i;
                p++;
              }
              XQueryColors(display,
                XDefaultColormap(display,visual_info->screen),
                colors+image->colors,(int) retain_colors);
              /*
                Transfer colors from default to private colormap.
              */
              XAllocColorCells(display,colormap,False,(unsigned long *) NULL,0,
                pixel_info->pixels,retain_colors);
              p=colors+image->colors;
              for (i=0; i < retain_colors; i++)
              {
                p->pixel=pixel_info->pixels[i];
                p++;
              }
              XStoreColors(display,colormap,colors+image->colors,retain_colors);
              number_colors+=retain_colors;
            }
          XAllocColorCells(display,colormap,False,(unsigned long *) NULL,0,
            pixel_info->pixels,image->colors);
        }
      /*
        Store the image colormap.
      */
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      if (visual_info->class == PseudoColor)
        for (i=0; i < image->colors; i++)
        {
          color.red=(unsigned short) (image->colormap[i].red << 8);
          color.green=(unsigned short) (image->colormap[i].green << 8);
          color.blue=(unsigned short) (image->colormap[i].blue << 8);
          color.pixel=pixel_info->pixels[i];
          *p++=color;
        }
      else
        for (i=0; i < image->colors; i++)
        {
          gray_value=Intensity(image->colormap[i]);
          color.red=(unsigned short) (gray_value << 8);
          color.green=(unsigned short) (gray_value << 8);
          color.blue=(unsigned short) (gray_value << 8);
          color.pixel=pixel_info->pixels[i];
          *p++=color;
        }
      XStoreColors(display,colormap,colors,image->colors);
      break;
    }
    case TrueColor:
    case DirectColor:
    default:
    {
      unsigned int
        linear_colormap;

      /*
        Define Standard Colormap for TrueColor or DirectColor visual.
      */
      number_colors=(unsigned int) ((map_info->red_max*map_info->red_mult)+
        (map_info->green_max*map_info->green_mult)+
        (map_info->blue_max*map_info->blue_mult)+1);
      linear_colormap=
        ((map_info->red_max+1) == visual_info->colormap_size) &&
        ((map_info->green_max+1) == visual_info->colormap_size) &&
        ((map_info->blue_max+1) == visual_info->colormap_size);
      if (linear_colormap)
        number_colors=visual_info->colormap_size;
      /*
        Allocate color array.
      */
      colors=(XColor *) malloc(number_colors*sizeof(XColor));
      if (colors == (XColor *) NULL)
        Error("unable to create colormap","memory allocation failed");
      /*
        Initialize linear color ramp.
      */
      p=colors;
      color.flags=DoRed | DoGreen | DoBlue;
      if (linear_colormap)
        for (i=0; i < number_colors; i++)
        {
          color.blue=(unsigned short) 0;
          if (map_info->blue_max > 0)
            color.blue=(unsigned short)
              (((i % map_info->green_mult)*65535)/map_info->blue_max);
          color.green=color.blue;
          color.red=color.blue;
          color.pixel=XStandardPixel(map_info,color,16);
          *p++=color;
        }
      else
        for (i=0; i < number_colors; i++)
        {
          color.red=(unsigned short) 0;
          if (map_info->red_max > 0)
            color.red=(unsigned short)
              (((i/map_info->red_mult)*65535)/map_info->red_max);
          color.green=(unsigned short) 0;
          if (map_info->green_max > 0)
            color.green=(unsigned short) ((((i/map_info->green_mult) %
              (map_info->green_max+1))*65535)/map_info->green_max);
          color.blue=(unsigned short) 0;
          if (map_info->blue_max > 0)
            color.blue=(unsigned short)
              (((i % map_info->green_mult)*65535)/map_info->blue_max);
          color.pixel=XStandardPixel(map_info,color,16);
          *p++=color;
        }
      if ((visual_info->class == DirectColor) &&
          (colormap != XDefaultColormap(display,visual_info->screen)))
        XStoreColors(display,colormap,colors,number_colors);
      else
        for (i=0; i < number_colors; i++)
          XAllocColor(display,colormap,&colors[i]);
      break;
    }
  }
  if ((visual_info->class != DirectColor) && (visual_info->class != TrueColor))
    {
      /*
        Set background/border/foreground/pen pixels.
      */
      status=XAllocColor(display,colormap,&pixel_info->background_color);
      if (status == 0)
        XBestPixel(colors,number_colors,&pixel_info->background_color);
      status=XAllocColor(display,colormap,&pixel_info->foreground_color);
      if (status == 0)
        XBestPixel(colors,number_colors,&pixel_info->foreground_color);
      status=XAllocColor(display,colormap,&pixel_info->border_color);
      if (status == 0)
        XBestPixel(colors,number_colors,&pixel_info->border_color);
      for (i=0; i < MaxNumberPens; i++)
      {
        status=XAllocColor(display,colormap,&pixel_info->pen_color[i]);
        if (status == 0)
          XBestPixel(colors,number_colors,&pixel_info->pen_color[i]);
        pixel_info->pixels[image->colors+i]=pixel_info->pen_color[i].pixel;
      }
      pixel_info->colors=image->colors+MaxNumberPens;
    }
  (void) free((char *) colors);
  if (resource_info->debug)
    {
      (void) fprintf(stderr,"Standard Colormap:\n");
      (void) fprintf(stderr,"  colormap id: 0x%lx\n",map_info->colormap);
      (void) fprintf(stderr,"  red, green, blue max: %lu %lu %lu\n",
        map_info->red_max,map_info->green_max,map_info->blue_max);
      (void) fprintf(stderr,"  red, green, blue mult: %lu %lu %lu\n",
        map_info->red_mult,map_info->green_mult,map_info->blue_mult);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X M a k e W i n d o w                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XMakeWindow creates an X11 window.
%
%  The format of the XMakeWindow routine is:
%
%      XMakeWindow(display,parent,argv,argc,class_hint,manager_hints,property,
%        window_info)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server; returned from
%      XOpenDisplay.
%
%    o parent: Specifies the parent window_info.
%
%    o argv: Specifies the application's argument list.
%
%    o argc: Specifies the number of arguments.
%
%    o class_hint: Specifies a pointer to a X11 XClassHint structure.
%
%    o manager_hints: Specifies a pointer to a X11 XWMHints structure.
%
%    o property: A property to define on the window_info.
%
%    o window_info: Specifies a pointer to a X11 XWindowInfo structure.
%
%
*/
void XMakeWindow(display,parent,argv,argc,class_hint,manager_hints,property,
  window_info)
Display
  *display;

Window
  parent;

char
  **argv;

int
  argc;

XClassHint
  *class_hint;

XWMHints
  *manager_hints;

Atom
  property;

XWindowInfo
  *window_info;
{
#define MinWindowSize  64

  int
    mask,
    status;

  XSizeHints
    *size_hints;

  XTextProperty
    icon_name,
    window_name;

  /*
    Set window_info hints.
  */
  size_hints=XAllocSizeHints();
  if (size_hints == (XSizeHints *) NULL)
    Error("unable to make window_info","memory allocation failed");
  size_hints->flags=window_info->flags;
  size_hints->x=window_info->x;
  size_hints->y=window_info->y;
  size_hints->width=window_info->width;
  size_hints->height=window_info->height;
  if (!window_info->immutable)
    {
      /*
        Window size can be changed.
      */
      size_hints->min_width=window_info->min_width;
      size_hints->min_height=window_info->min_height;
      size_hints->flags|=PMinSize;
    }
  else
    {
      /*
        Window size cannot be changed.
      */
      size_hints->min_width=window_info->width;
      size_hints->min_height=window_info->height;
      size_hints->max_width=window_info->width;
      size_hints->max_height=window_info->height;
      size_hints->flags|=PMinSize | PMaxSize;
    }
  size_hints->flags|=PResizeInc;
  size_hints->width_inc=window_info->width_inc;
  size_hints->height_inc=window_info->height_inc;
#ifndef PRE_R4_ICCCM
  size_hints->flags|=PBaseSize;
  size_hints->base_width=size_hints->min_width;
  size_hints->base_height=size_hints->min_height;
#endif
  if (window_info->geometry != (char *) NULL)
    {
      char
        default_geometry[2048];

      int
        flags,
        gravity;

      /*
        User specified geometry.
      */
      (void) sprintf(default_geometry,"%dx%d\0",size_hints->width,
        size_hints->height);
      flags=XWMGeometry(display,window_info->screen,window_info->geometry,
        default_geometry,window_info->border_width,size_hints,&size_hints->x,
        &size_hints->y,&size_hints->width,&size_hints->height,&gravity);
      window_info->x=size_hints->x;
      window_info->y=size_hints->y;
      if ((flags & WidthValue) && (flags & HeightValue))
        size_hints->flags|=USSize;
      if ((flags & XValue) && (flags & YValue))
        size_hints->flags|=USPosition;
#ifndef PRE_R4_ICCCM
      size_hints->win_gravity=gravity;
      size_hints->flags|=PWinGravity;
#endif
    }
  if (window_info->id == (Window) NULL)
    window_info->id=XCreateWindow(display,parent,window_info->x,window_info->y,
      window_info->width,window_info->height,window_info->border_width,
      window_info->depth,InputOutput,window_info->visual_info->visual,
      window_info->mask,&window_info->attributes);
  else
    {
      unsigned int
        mask;

      XEvent
        discard_event;

      XWindowChanges
        window_info_changes;

      /*
        Window already exists;  change relevant attributes.
      */
      XChangeWindowAttributes(display,window_info->id,window_info->mask,
        &window_info->attributes);
      XSync(display,False);
      while (XCheckTypedWindowEvent(display,window_info->id,ConfigureNotify,
        &discard_event));
      window_info_changes.x=window_info->x;
      window_info_changes.y=window_info->y;
      window_info_changes.width=window_info->width;
      window_info_changes.height=window_info->height;
      mask=CWWidth | CWHeight;
      if (window_info->flags & USPosition)
        mask|=CWX | CWY;
      XReconfigureWMWindow(display,window_info->id,window_info->screen,mask,
        &window_info_changes);
    }
  if (window_info->id == (Window) NULL)
    Error("unable to create window",window_info->name);
  status=XStringListToTextProperty(&window_info->name,1,&window_name);
  if (status == 0)
    Error("unable to create text property",window_info->name);
  if (window_info->icon_name == (char *) NULL)
    icon_name=window_name;
  else
    {
      status=XStringListToTextProperty(&window_info->icon_name,1,&icon_name);
      if (status == 0)
        Error("unable to create text property",window_info->icon_name);
    }
  if (window_info->icon_geometry != (char *) NULL)
    {
      int
        flags,
        gravity,
        height,
        width;

      /*
        User specified icon geometry.
      */
      size_hints->flags|=USPosition;
      flags=XWMGeometry(display,window_info->screen,window_info->icon_geometry,
        (char *) NULL,0,size_hints,&manager_hints->icon_x,
        &manager_hints->icon_y,&width,&height,&gravity);
      if ((flags & XValue) && (flags & YValue))
        manager_hints->flags|=IconPositionHint;
    }
  XSetWMProperties(display,window_info->id,&window_name,&icon_name,argv,argc,
    size_hints,manager_hints,class_hint);
  XSetWMProtocols(display,window_info->id,&property,1);
  XFree((void *) size_hints);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P o p U p A l e r t                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPopupAlert displays a popup window with an alert to the user.
%  The function returns when the user presses a button or key.
%
%  The format of the XPopupAlert routine is:
%
%    XPopupAlert(display,popup_window,message,qualifier)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o message: Specifies the message to display before terminating the
%      program.
%
%    o qualifier: Specifies any qualifier to the message.
%
%
*/
void XPopupAlert(display,popup_window,message,qualifier)
Display
  *display;

XWindowInfo
  *popup_window;

char
  *message,
  *qualifier;
{
  char
    text[2048];

  int
    i,
    state,
    x,
    y;

  unsigned int
    mask;

  Window
    window;

  XEvent
    event;

  /*
    Position and map popup window.
  */
  (void) sprintf(text,"%s\0",message);
  if (qualifier != (char *) NULL)
    {
      (void) strcat(text," (");
      (void) strcat(text,qualifier);
      (void) strcat(text,")");
    }
  popup_window->width=XTextWidth(popup_window->font_info,text,strlen(text))+
    4*popup_window->font_info->max_bounds.width;
  popup_window->height=
    popup_window->font_info->ascent+popup_window->font_info->descent+8;
  XQueryPointer(display,XRootWindow(display,popup_window->screen),&window,
    &window,&i,&i,&popup_window->x,&popup_window->y,&mask);
  x=Min(popup_window->x+2,XDisplayWidth(display,popup_window->screen)-
    popup_window->width);
  y=Min(popup_window->y+2,XDisplayHeight(display,popup_window->screen)-
    popup_window->height);
  XMoveResizeWindow(display,popup_window->id,x,y,popup_window->width,
    popup_window->height);
  XMapRaised(display,popup_window->id);
  XClearWindow(display,popup_window->id);
  /*
    Display message in popup window.
  */
  x=2*popup_window->font_info->max_bounds.width;
  y=popup_window->font_info->ascent+4;
  XDrawString(display,popup_window->id,popup_window->graphic_context,x,y,text,
    strlen(text));
  XBell(display,0);
  /*
    Wait for a key press.
  */
  state=DefaultState;
  do
  {
    /*
      Wait for next event.
    */
    XMaskEvent(display,ButtonPressMask | KeyPressMask | VisibilityChangeMask,
      &event);
    switch (event.type)
    {
      case ButtonPress:
      case KeyPress:
      {
        state|=ExitState;
        break;
      }
      case VisibilityNotify:
      {
        XMapRaised(display,popup_window->id);
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XWithdrawWindow(display,popup_window->id,popup_window->screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P o p U p M e n u                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPopupMenu maps a menu and returns the command pointed to by the
%  user when the button is released.
%
%  The format of the XPopupMenu routine is:
%
%    selection_number=XPopupMenu(display,popup_window,x,y,menu_title,
%      menu_selections,number_selections,item)
%
%  A description of each parameter follows:
%
%    o selection_number: Specifies the number of the selection that the
%      user choose.
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o x: Specifies an unsigned integer representing the root offset in the
%      x-direction.
%
%    o y: Specifies an unsigned integer representing the root offset in the
%      x-direction.
%
%    o menu_title: Specifies a character string that describes the menu
%      selections.
%
%    o menu_selections: Specifies a pointer to one or more strings that
%      make up the choices in the menu.
%
%    o number_selections: Specifies the number of choices in the menu.
%
%    o item: Specifies a character array.  The item selected from the menu
%      is returned here.
%
%
*/
unsigned int XPopupMenu(display,popup_window,x,y,menu_title,menu_selections,
  number_selections,item)
Display
  *display;

XWindowInfo
  *popup_window;

int
  x,
  y;

char
  *menu_title,
  **menu_selections;

unsigned int
  number_selections;

char
  *item;
{
  typedef struct _Selection
  {
    int
      id,
      x,
      y;

    unsigned int
      height;
  } Selection;

  int
    id,
    state;

  Selection
    selection;

  unsigned int
    height,
    title_height,
    width;

  XEvent
    event;

  /*
    Size and position menu window under current pointer location and map.
  */
  popup_window->width=
    XTextWidth(popup_window->font_info,menu_title,strlen(menu_title));
  for (selection.id=0; selection.id < number_selections; selection.id++)
  {
    width=XTextWidth(popup_window->font_info,menu_selections[selection.id],
      strlen(menu_selections[selection.id]));
    if (width > popup_window->width)
      popup_window->width=width;
  }
  popup_window->width+=4*popup_window->font_info->max_bounds.width;
  title_height=
    (popup_window->font_info->descent+popup_window->font_info->ascent)*2;
  popup_window->height=title_height+number_selections*
    (popup_window->font_info->ascent+popup_window->font_info->descent+4);
  width=popup_window->width+2*popup_window->border_width;
  popup_window->x=x-(int) width/2;
  if (popup_window->x < 0)
    popup_window->x=0;
  else
    if (popup_window->x > (XDisplayWidth(display,popup_window->screen)-width))
      popup_window->x=XDisplayWidth(display,popup_window->screen)-width;
  height=popup_window->height+2*popup_window->border_width;
  popup_window->y=y-(int) popup_window->border_width;
  if (popup_window->y < 0)
    popup_window->y=0;
  else
    if (popup_window->y > (XDisplayHeight(display,popup_window->screen)-height))
      popup_window->y=XDisplayHeight(display,popup_window->screen)-height;
  XMoveResizeWindow(display,popup_window->id,popup_window->x,popup_window->y,
    popup_window->width,popup_window->height);
  XMapRaised(display,popup_window->id);
  XClearWindow(display,popup_window->id);
  /*
    Draw title.
  */
  width=XTextWidth(popup_window->font_info,menu_title,strlen(menu_title));
  XDrawString(display,popup_window->id,popup_window->graphic_context,
    (int) (popup_window->width-width) >> 1,
    (int) (popup_window->font_info->ascent*3) >> 1,menu_title,
    strlen(menu_title));
  XDrawLine(display,popup_window->id,popup_window->graphic_context,0,
    title_height-2,popup_window->width,title_height-2);
  /*
    Draw menu selections.
  */
  selection.x=2*popup_window->font_info->max_bounds.width;
  selection.y=title_height+popup_window->font_info->ascent;
  selection.height=popup_window->font_info->ascent+
    popup_window->font_info->descent+4;
  for (selection.id=0; selection.id < number_selections; selection.id++)
  {
    XDrawString(display,popup_window->id,popup_window->graphic_context,
      selection.x,selection.y,menu_selections[selection.id],
      strlen(menu_selections[selection.id]));
    selection.y+=(int) selection.height;
  }
  /*
    Highlight menu as pointer moves;  return item on button release.
  */
  selection.id=(-1);
  state=DefaultState;
  do
  {
    /*
      Wait for next event.
    */
    XMaskEvent(display,ButtonPressMask | ButtonMotionMask | ButtonReleaseMask |
      EnterWindowMask | LeaveWindowMask | VisibilityChangeMask,&event);
    switch (event.type)
    {
      case ButtonPress:
        break;
      case ButtonRelease:
      {
        /*
          Exit menu.
        */
        *item='\0';
        state|=ExitState;
        break;
      }
      case EnterNotify:
      {
        if (event.xcrossing.window != popup_window->id)
          break;
        id=((event.xcrossing.y-title_height)/(int) selection.height);
        if ((id < 0) || (id >= number_selections))
          break;
        /*
          Highlight this selection.
        */
        selection.id=id;
        selection.y=title_height+
          popup_window->font_info->ascent+2+selection.id*selection.height;
        XFillRectangle(display,popup_window->id,popup_window->graphic_context,0,
          selection.y-popup_window->font_info->ascent-2,popup_window->width,
          selection.height);
        XDrawString(display,popup_window->id,popup_window->highlight_context,
          selection.x,selection.y,menu_selections[selection.id],
          strlen(menu_selections[selection.id]));
        break;
      }
      case LeaveNotify:
      {
        if (event.xcrossing.window != popup_window->id)
          break;
        if ((selection.id >= 0) && (selection.id < number_selections))
          {
            /*
              Unhighlight last selection.
            */
            XClearArea(display,popup_window->id,0,selection.y-
              popup_window->font_info->ascent-2,popup_window->width,
              selection.height,False);
            XDrawString(display,popup_window->id,popup_window->graphic_context,
              selection.x,selection.y,menu_selections[selection.id],
              strlen(menu_selections[selection.id]));
          }
        selection.id=(-1);
        break;
      }
      case MotionNotify:
      {
        if (event.xmotion.window != popup_window->id)
          break;
        /*
          Determine if pointer has moved to a new selection.
        */
        id=(event.xmotion.y-title_height)/(int) selection.height;
        if ((selection.id >= 0) && (selection.id < number_selections))
          {
            /*
              Unhighlight last selection.
            */
            if (id == selection.id)
              break;
            XClearArea(display,popup_window->id,0,selection.y-
              popup_window->font_info->ascent-2,popup_window->width,
              selection.height,False);
            XDrawString(display,popup_window->id,popup_window->graphic_context,
              selection.x,selection.y,menu_selections[selection.id],
              strlen(menu_selections[selection.id]));
          }
        selection.id=id;
        if ((id < 0) || (id >= number_selections))
          break;
        /*
          Highlight this selection.
        */
        selection.y=title_height+
          popup_window->font_info->ascent+2+selection.id*selection.height;
        XFillRectangle(display,popup_window->id,popup_window->graphic_context,0,
          selection.y-popup_window->font_info->ascent-2,popup_window->width,
          selection.height);
        XDrawString(display,popup_window->id,popup_window->highlight_context,
          selection.x,selection.y,menu_selections[selection.id],
          strlen(menu_selections[selection.id]));
        break;
      }
      case VisibilityNotify:
      {
        XMapRaised(display,popup_window->id);
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XWithdrawWindow(display,popup_window->id,popup_window->screen);
  if ((selection.id >= 0) && (selection.id < number_selections))
    (void) strcpy(item,menu_selections[selection.id]);
  return((unsigned int) selection.id);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X P o p U p Q u e r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XPopupQuery displays a popup window with a query to the user.  The
%  user keys their reply and presses return to exit.  The typed text is
%  returned as the reply function parameter.
%
%  The format of the XPopupQuery routine is:
%
%    XPopupQuery(display,popup_window,query,reply)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o popup_window: Specifies a pointer to a XWindowInfo structure.
%
%    o query: Specifies a pointer to the query to present to the user.
%
%    o reply: The response from the user is returned in this parameter.
%
%
*/
void XPopupQuery(display,popup_window,query,reply)
Display
  *display;

XWindowInfo
  *popup_window;

char
  *query,
  *reply;
{
  char
    *p,
    text[2048];

  GC
    graphic_context;

  int
    i,
    state,
    x,
    y;

  unsigned int
    height,
    mask;

  Window
    window;

  XEvent
    event;

  XFontStruct
    *font_info;

  /*
    Position and map popup window.
  */
  (void) sprintf(text,"%s %s\0",query,reply);
  popup_window->width=XTextWidth(popup_window->font_info,text,strlen(text))+
    22*popup_window->font_info->max_bounds.width;
  popup_window->height=
    popup_window->font_info->ascent+popup_window->font_info->descent+8;
  XQueryPointer(display,XRootWindow(display,popup_window->screen),&window,
    &window,&i,&i,&popup_window->x,&popup_window->y,&mask);
  x=Min(popup_window->x+2,XDisplayWidth(display,popup_window->screen)-
    popup_window->width);
  y=Min(popup_window->y+2,XDisplayHeight(display,popup_window->screen)-
    popup_window->height);
  XMoveResizeWindow(display,popup_window->id,x,y,popup_window->width,
    popup_window->height);
  XMapRaised(display,popup_window->id);
  XClearWindow(display,popup_window->id);
  /*
    Display query in popup window.
  */
  font_info=popup_window->font_info;
  graphic_context=popup_window->graphic_context;
  x=2*font_info->max_bounds.width;
  y=font_info->ascent+4;
  height=font_info->ascent+font_info->descent;
  XDrawString(display,popup_window->id,graphic_context,x,y,query,strlen(query));
  x+=XTextWidth(font_info,query,strlen(query))+font_info->max_bounds.width;
  /*
    Display reply in popup window.
  */
  XDrawString(display,popup_window->id,graphic_context,x,y,reply,strlen(reply));
  x+=XTextWidth(font_info,reply,strlen(reply));
  /*
    Begin editing the reply.
  */
  state=DefaultState;
  p=reply+strlen(reply);
  do
  {
    if ((x+font_info->max_bounds.width) >= popup_window->width)
      {
        /*
          Resize popup window.
        */
        (void) sprintf(text,"%s %s\0",query,reply);
        popup_window->width=
          XTextWidth(popup_window->font_info,text,strlen(text))+
          22*popup_window->font_info->max_bounds.width;
        XResizeWindow(display,popup_window->id,popup_window->width,
          popup_window->height);
        /*
          Display reply in popup window.
        */
        x=2*font_info->max_bounds.width;
        XDrawString(display,popup_window->id,graphic_context,x,y,query,
          strlen(query));
        x+=XTextWidth(font_info,query,strlen(query))+
          font_info->max_bounds.width;
        XDrawString(display,popup_window->id,graphic_context,x,y,reply,
          strlen(reply));
        x+=XTextWidth(font_info,reply,strlen(reply));
      }
    /*
      Display text cursor.
    */
    *p='\0';
    XDrawString(display,popup_window->id,graphic_context,x,y,"_",1);
    /*
      Wait for next event.
    */
    XMaskEvent(display,ButtonPressMask | KeyPressMask | VisibilityChangeMask,
      &event);
    /*
      Erase text cursor.
    */
    XClearArea(display,popup_window->id,x,y-font_info->ascent,
      (unsigned int) font_info->max_bounds.width,height,False);
    switch (event.type)
    {
      case ButtonPress:
      {
        Atom
          type;

        int
          format,
          status;

        unsigned char
          *data;

        unsigned long
          after,
          length;

        if ((event.xbutton.button == Button3) &&
            (event.xbutton.state & Mod1Mask))
          {
            /* 
              Convert Alt-Button3 to Button2.
            */
            event.xbutton.button=Button2;
            event.xbutton.state&=(~Mod1Mask);
          }
        if (event.xbutton.button != Button2)
          break;
        /*
          Obtain response from cut buffer.
        */
        status=XGetWindowProperty(display,XRootWindow(display,0),XA_CUT_BUFFER0,
          0L,2047L,False,XA_STRING,&type,&format,&length,&after,&data);
        if ((status != Success) || (type != XA_STRING) || (format == 32) ||
            (length == 0))
          break;
        /*
          Append cut buffer to reply.
        */
        (void) strncpy(p,(char *) data,(int) length);
        XFree((void *) data);
        XDrawString(display,popup_window->id,graphic_context,x,y,p,
          (int) length);
        x+=XTextWidth(font_info,p,(unsigned int) length);
        p+=length;
        *p='\0';
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
            if (p == reply)
              break;
            p--;
            x-=XTextWidth(font_info,p,1);
            XClearArea(display,popup_window->id,x,y-font_info->ascent,
              (unsigned int) font_info->max_bounds.width,height,False);
            break;
          }
          case XK_Escape:
          {
            /*
              Return null response.
            */
            *reply='\0';
            state|=ExitState;
          }
          case XK_Delete:
          {
            /*
              Erase the entire line of text.
            */
            while (p != reply)
            {
              p--;
              x-=XTextWidth(font_info,p,1);
              XClearArea(display,popup_window->id,x,y-font_info->ascent,
                (unsigned int) font_info->max_bounds.width,height,False);
            }
            break;
          }
          case XK_Return:
          {
            /*
              Commit to current response.
            */
            state|=ExitState;
            break;
          }
          default:
          {
            /*
              Draw a single character on the popup window.
            */
            if (*command == '\0')
              break;
            *p=(*command);
            XDrawString(display,popup_window->id,graphic_context,x,y,p,1);
            x+=XTextWidth(font_info,p,1);
            p++;
            break;
          }
        }
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
      case VisibilityNotify:
      {
        XMapRaised(display,popup_window->id);
        break;
      }
      default:
        break;
    }
  } while (!(state & ExitState));
  XWithdrawWindow(display,popup_window->id,popup_window->screen);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e a d C o l o r m a p                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XReadColormap returns the red, green, and blue colormap of a window.
%  Additionally, the number of colors in the colormap is returned.
%
%  The format of the XReadColormap function is:
%
%      number_colors=XReadColormap(display,window_attributes,colors)
%
%  A description of each parameter follows:
%
%    o number_colors: XReadColormap returns the number of colors in the
%      colormap.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o window_attributes: Specifies a pointer to the window attributes
%      structure;  returned from XGetWindowAttributes.
%
%    o colors: Specifies a an array of XColor structures.  The colormap
%      red, green, and blue are returned.
%
%
*/
static unsigned int XReadColormap(display,window_attributes,colors)
Display
  *display;

XWindowAttributes
  *window_attributes;

XColor
  **colors;
{
  unsigned int
    number_colors;

  register int
    i;

  if (window_attributes->colormap == (Colormap) NULL)
    return(0);
  number_colors=window_attributes->visual->map_entries;
  *colors=(XColor *) malloc(number_colors*sizeof(XColor));
  if (*colors == (XColor *) NULL)
    {
      Warning("unable to read colormap","memory allocation failed");
      return(0);
    }
  if ((window_attributes->visual->class != DirectColor) &&
      (window_attributes->visual->class != TrueColor))
    for (i=0; i < number_colors; i++)
    {
      (*colors)[i].pixel=i;
      (*colors)[i].pad=0;
    }
  else
    {
      unsigned long
        blue,
        blue_bit,
        green,
        green_bit,
        red,
        red_bit;

      /*
        DirectColor or TrueColor visual.
      */
      red=0;
      green=0;
      blue=0;
      red_bit=window_attributes->visual->red_mask &
        (~(window_attributes->visual->red_mask)+1);
      green_bit=window_attributes->visual->green_mask &
        (~(window_attributes->visual->green_mask)+1);
      blue_bit=window_attributes->visual->blue_mask &
        (~(window_attributes->visual->blue_mask)+1);
      for (i=0; i < number_colors; i++)
      {
        (*colors)[i].pixel=red | green | blue;
        (*colors)[i].pad=0;
        red+=red_bit;
        if (red > window_attributes->visual->red_mask)
          red=0;
        green+=green_bit;
        if (green > window_attributes->visual->green_mask)
          green=0;
        blue+=blue_bit;
        if (blue > window_attributes->visual->blue_mask)
          blue=0;
      }
    }
  XQueryColors(display,window_attributes->colormap,*colors,(int) number_colors);
  return(number_colors);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X R e f r e s h W i n d o w                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XRefreshWindow refreshes an image in a X window.
%
%  The format of the XRefreshWindow routine is:
%
%      XRefreshWindow(display,window,event)
%
%  A description of each parameter follows:
%
%    o display: Specifies a connection to an X server;  returned from
%      XOpenDisplay.
%
%    o window: Specifies a pointer to a XWindowInfo structure.
%
%    o event: Specifies a pointer to a XEvent structure.  If it is NULL,
%      the entire image is refreshed.
%
%
*/
void XRefreshWindow(display,window,event)
Display
  *display;

XWindowInfo
  *window;

XEvent
  *event;
{
  int
    x,
    y;

  unsigned int
    height,
    width;

  if (event != (XEvent *) NULL)
    {
      /*
        Determine geometry from expose event.
      */
      x=event->xexpose.x;
      y=event->xexpose.y;
      width=event->xexpose.width;
      height=event->xexpose.height;
    }
  else
    {
      XEvent
        discard_event;

      /*
        Refresh entire window; discard outstanding expose events.
      */
      x=0;
      y=0;
      width=window->width;
      height=window->height;
      XSync(display,False);
      while (XCheckTypedWindowEvent(display,window->id,Expose,&discard_event));
    }
  /*
    Check boundary conditions.
  */
  if ((window->ximage->width-(x+window->x)) < width)
    width=window->ximage->width-(x+window->x);
  if ((window->ximage->height-(y+window->y)) < height)
    height=window->ximage->height-(y+window->y);
  /*
    Refresh image.
  */
  if (window->pixmap != (Pixmap) NULL)
    {
      if (window->depth > 1)
        XCopyArea(display,window->pixmap,window->id,window->graphic_context,
          x+window->x,y+window->y,width,height,x,y);
      else
        XCopyPlane(display,window->pixmap,window->id,window->highlight_context,
          x+window->x,y+window->y,width,height,x,y,1L);
    }
  else
    XPutImage(display,window->id,window->graphic_context,window->ximage,
      x+window->x,y+window->y,x,y,width,height);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X S e l e c t W i n d o w                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XSelectWindow allows a user to select a window using the mouse.  If
%  the mouse moves, a clipping rectangle is drawn and the extents of the
%  rectangle is returned in the clip_info structure.
%
%  The format of the XSelectWindow function is:
%
%      target_window=XSelectWindow(display,clip_info)
%
%  A description of each parameter follows:
%
%    o window: XSelectWindow returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o clip_info: Specifies a pointer to a XRectangle structure.  It contains
%      the extents of any clipping rectangle.
%
%
*/
static Window XSelectWindow(display,clip_info)
Display
  *display;

XRectangle
  *clip_info;
{
#define ClipRectangle(clip_info)  \
{  \
  if ((clip_info->width*clip_info->height) >= MinimumClipArea)  \
    {  \
      XSetFunction(display,graphic_context,GXinvert);  \
      XDrawRectangle(display,root_window,graphic_context,clip_info->x,  \
        clip_info->y,clip_info->width-1,clip_info->height-1);  \
      XSetFunction(display,graphic_context,GXcopy);  \
    }  \
}
#define MinimumClipArea  (unsigned int) 9

  Cursor
    target_cursor;

  GC
    graphic_context;

  int
    presses,
    status,
    x_offset,
    y_offset;

  Window
    root_window,
    target_window;

  XEvent
    event;

  XGCValues
    graphic_context_value;

  /*
    Initialize graphic context.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  graphic_context_value.function=GXcopy;
  graphic_context_value.line_width=2;
  graphic_context_value.plane_mask=AllPlanes;
  graphic_context_value.subwindow_mode=IncludeInferiors;
  graphic_context=XCreateGC(display,root_window,GCFunction | GCLineWidth |
    GCPlaneMask | GCSubwindowMode,&graphic_context_value);
  if (graphic_context == (GC) NULL)
    return(False);
  /*
    Make the target cursor.
  */
  target_cursor=XCreateFontCursor(display,XC_tcross);
  /*
    Grab the pointer using target cursor.
  */
  status=XGrabPointer(display,root_window,False,(unsigned int)
    (ButtonPressMask | ButtonReleaseMask | ButtonMotionMask),GrabModeSync,
    GrabModeAsync,root_window,target_cursor,CurrentTime);
  if (status != GrabSuccess)
    Error("unable to grab the mouse",(char *) NULL);
  /*
    Select a window.
  */
  target_window=(Window) NULL;
  presses=0;
  clip_info->width=0;
  clip_info->height=0;
  do
  {
    if ((clip_info->width*clip_info->height) >= MinimumClipArea)
      ClipRectangle(clip_info);
    /*
      Allow another event.
    */
    XAllowEvents(display,SyncPointer,CurrentTime);
    XWindowEvent(display,root_window,ButtonPressMask | ButtonReleaseMask |
      ButtonMotionMask,&event);
    ClipRectangle(clip_info);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (target_window == (Window) NULL)
          {
            target_window=event.xbutton.subwindow;
            if (target_window == (Window) NULL)
              target_window=root_window;
          }
        x_offset=event.xbutton.x_root;
        y_offset=event.xbutton.y_root;
        clip_info->x=x_offset;
        clip_info->y=y_offset;
        clip_info->width=0;
        clip_info->height=0;
        presses++;
        break;
      }
      case ButtonRelease:
      {
        presses--;
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,ButtonMotionMask,&event));
        clip_info->x=event.xmotion.x;
        clip_info->y=event.xmotion.y;
        /*
          Check boundary conditions.
        */
        if (clip_info->x < x_offset)
          clip_info->width=(unsigned int) (x_offset-clip_info->x);
        else
          {
            clip_info->width=(unsigned int) (clip_info->x-x_offset);
            clip_info->x=x_offset;
          }
        if (clip_info->y < y_offset)
          clip_info->height=(unsigned int) (y_offset-clip_info->y);
        else
          {
            clip_info->height=(unsigned int) (clip_info->y-y_offset);
            clip_info->y=y_offset;
          }
      }
      default:
        break;
    }
  }
  while ((target_window == (Window) NULL) || (presses > 0));
  XUngrabPointer(display,CurrentTime);
  if ((clip_info->width*clip_info->height) < MinimumClipArea)
    {
      clip_info->width=0;
      clip_info->height=0;
    }
  XFreeCursor(display,target_cursor);
  XFreeGC(display,graphic_context);
  return(target_window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X V i s u a l C l a s s N a m e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XVisualClassName returns the visual class name as a character
%  string.
%
%  The format of the XVisualClassName routine is:
%
%      visual_type=XVisualClassName(visual_info)
%
%  A description of each parameter follows:
%
%    o visual_type: XVisualClassName returns the visual class as a character
%      string.
%
%    o visual_info: Specifies a pointer to a X11 XVisualInfo structure;
%      returned from XGetVisualInfo.
%
%
*/
char *XVisualClassName(visual_info)
XVisualInfo
  *visual_info;
{
  if (visual_info == (XVisualInfo *) NULL)
    return((char *) NULL);
  switch (visual_info->class)
  {
    case StaticGray: return("StaticGray");
    case GrayScale: return("GrayScale");
    case StaticColor: return("StaticColor");
    case PseudoColor: return("PseudoColor");
    case TrueColor: return("TrueColor");
    case DirectColor: return("DirectColor");
  }
  return("unknown visual class");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y I D                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByID locates a child window with a given ID.  If not window
%  with the given name is found, 0 is returned.   Only the window specified
%  and its subwindows are searched.
%
%  The format of the XWindowByID function is:
%
%      child=XWindowByID(display,window,id)
%
%  A description of each parameter follows:
%
%    o child: XWindowByID returns the window with the specified
%      id.  If no windows are found, XWindowByID returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o id: Specifies the id of the window to locate.
%
%
*/
Window XWindowByID(display,root_window,id)
Display
  *display;

Window
  root_window;

unsigned long
  id;
{
  register int
    i;

  unsigned int
    number_children;

  Window
    child,
    *children,
    window;

  if (root_window == id)
    return(id);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByID(display,children[i],id);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y N a m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByName locates a window with a given name on a display.
%  If no window with the given name is found, 0 is returned. If more than
%  one window has the given name, the first one is returned.  Only root and
%  its children are searched.
%
%  The format of the XWindowByName function is:
%
%      window=XWindowByName(display,root_window,name)
%
%  A description of each parameter follows:
%
%    o window: XWindowByName returns the window id.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o root_window: Specifies the id of the root window.
%
%    o name: Specifies the name of the window to locate.
%
%
*/
Window XWindowByName(display,root_window,name)
Display
  *display;

Window
  root_window;

char
  *name;
{
  register int
    i;

  unsigned int
    number_children;

  Window
    *children,
    child,
    window;

  XTextProperty
    window_name;

  if (XGetWMName(display,root_window,&window_name) != 0)
    if (strcmp((char *) window_name.value,name) == 0)
      return(root_window);
  if (!XQueryTree(display,root_window,&child,&child,&children,&number_children))
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByName(display,children[i],name);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(window);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   X W i n d o w B y P r o p e r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function XWindowByProperty locates a child window with a given property.
%  If no window with the given name is found, 0 is returned.  If more than
%  one window has the given property, the first one is returned.  Only the
%  window specified and its subwindows are searched.
%
%  The format of the XWindowByProperty function is:
%
%      child=XWindowByProperty(display,window,property)
%
%  A description of each parameter follows:
%
%    o child: XWindowByProperty returns the window id with the specified
%      property.  If no windows are found, XWindowByProperty returns 0.
%
%    o display: Specifies a pointer to the Display structure;  returned from
%      XOpenDisplay.
%
%    o property: Specifies the property of the window to locate.
%
%
*/
static Window XWindowByProperty(display,window,property)
Display
  *display;

Window
  window;

Atom
  property;
{
  Atom
    type;

  int
    format,
    status;

  unsigned char
    *data;

  unsigned int
    i,
    number_children;

  unsigned long
    after,
    number_items;

  Window
    *children,
    child,
    parent,
    root;

  if (XQueryTree(display,window,&root,&parent,&children,&number_children) == 0)
    return((Window) NULL);
  type=(Atom) NULL;
  child=(Window) NULL;
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
  {
    status=XGetWindowProperty(display,children[i],property,0L,0L,False,
      (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
    if ((status == Success) && (type != (Atom) NULL))
      child=children[i];
  }
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
    child=XWindowByProperty(display,children[i],property);
  if (children != (Window *) NULL)
    XFree((void *) children);
  return(child);
}
