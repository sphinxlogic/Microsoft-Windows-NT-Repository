/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                     X   X  TTTTT   OOO   PPPP   SSSSS                       %
%                      X X     T    O   O  P   P  S                           %
%                       X      T    O   O  PPPP    SSS                        %
%                      X X     T    O   O  P          S                       %
%                     X   X    T     OOO   P      SSSSS                       %
%                                                                             %
%                                                                             %
%                  Import X11 image to a Postscript format.                   %
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
%  XtoPS is an X Window System window dumping utility.  XtoPS reads an
%  image from any visible window on an X server and outputs it to an
%  encapsulated Postscript file.  You can view this file with any
%  Postscript compatible viewer or printer.  The image is displayed in
%  color on viewers or printers that support color Postscript, otherwise
%  it is displayed as grayscale.  The target window can be specified by id
%  or name or be selected by clicking the mouse in the desired window.
%  The keyboard bell is rung once at the beginning of the dump and twice
%  when the dump is completed.
%
%  The XtoPS program command syntax is:
%
%  Usage: XtoPS [options ...] file
%
%  Where options include:
%    -border             include image borders in the output image
%    -delay seconds      pause before selecting target window
%    -display server     X server to contact
%    -frame              include window manager frame
%    -geometry geometry  preferred size and location of the image
%    -monochrome         transform image to black and white
%    -rotate degrees     apply Paeth rotation to the image
%    -scale geometry     preferred size factors of the image
%    -screen             select image from root window
%    -verbose            print detailed information about the image
%    -window id          select window with this id or name
%
%  Change '-' to '+' in any option above to reverse its effect.
%  For example, +frame means do not window manager frame.
%
%  Specify 'file' as '-' for standard input or output.
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
      "-border             include image borders in the output image",
      "-delay seconds      pause before selecting target window",
      "-display server     X server to contact",
      "-frame              include window manager frame",
      "-geometry geometry  preferred size and location of the image",
      "-monochrome         transform image to black and white",
      "-rotate degrees     apply Paeth rotation to the image",
      "-scale geometry     preferred size factors of the image",
      "-screen             select image from root window",
      "-verbose            print detailed information about the image",
      "-window id          select window with this id or name",
      (char *) NULL
    };
  (void) fprintf(stderr,"Usage: %s [options ...] file\n",client_name);
  (void) fprintf(stderr,"\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) fprintf(stderr,"  %s\n",*p);
  (void) fprintf(stderr,
    "\nChange '-' to '+' in any option above to reverse its effect.\n");
  (void) fprintf(stderr,
    "For example, +frame means do not include window manager frame.\n");
  (void) fprintf(stderr,"\nSpecify 'file' as '-' for standard output.\n");
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
  *argv[];
{
  char
    *filename,
    *option,
    *resource_value,
    *scale_geometry,
    *server_name,
    *target_window;

  Display
    *display;

  Image
    *image;

  int
    degrees,
    i,
    x;

  time_t
    start_time;

  unsigned int
    borders,
    frame,
    screen,
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
    Usage();
  /*
    Connect to X server.
  */
  server_name=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if (((int) strlen(option) > 1) && ((*option == '-') || (*option == '+')))
      if (strncmp("dis",option+1,3) == 0)
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
  resource_value=XGetResource(resource_database,client_name,"borders","False");
  borders=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"frame","False");
  frame=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"rotate","False");
  scale_geometry=
    XGetResource(resource_database,client_name,"scaleGeometry",(char *) NULL);
  degrees=atoi(resource_value);
  resource_value=XGetResource(resource_database,client_name,"screen","False");
  screen=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"verbose","False");
  verbose=IsTrue(resource_value);
  /*
    Check command syntax.
  */
  filename=(char *) NULL;
  target_window=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if (((int) strlen(option) < 2) || ((*option != '-') && (*option != '+')))
      filename=argv[i];
    else
      switch(*(option+1))
      {
        case 'b':
        {
          borders=(*option == '-');
          break;
        }
        case 'd':
        {
          if (strncmp("delay",option+1,2) == 0)
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
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'g':
        {
          /*
            User specified server name.
          */
          i++;
          if (i == argc)
            Error("missing geometry on -geometry",(char *) NULL);
          resource_info.image_geometry=argv[i];
          break;
        }
        case 'h':
        {
          Usage();
          break;
        }
        case 'f':
        {
          frame=(*option == '-');
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
        case 's':
        {
          if (strncmp("scale",option+1,4) == 0)
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
          if (strncmp("screen",option+1,4) == 0)
            {
              screen=(*option == '-');
              break;
            }
          Error("unrecognized option",option);
          break;
        }
        case 'v':
        {
          verbose=(*option == '-');
          break;
        }
        case 'w':
        {
          i++;
          if (i == argc)
            Error("missing id, name, or 'root' on -window",(char *) NULL);
          target_window=argv[i];
          break;
        }
        default:
        {
          Error("unrecognized option",option);
          break;
        }
      }
  }
  if (filename == (char *) NULL)
    Error("missing an image file name",(char *) NULL);
  /*
    Read image from X server.
  */
  if (resource_info.delay != 0)
    (void) sleep(resource_info.delay);
  start_time=time((time_t *) 0);
  image=ReadXImage(target_window,server_name,frame,screen,borders);
  if (image == (Image *) NULL)
    exit(1);
  if (resource_info.image_geometry || scale_geometry)
    TransformImage(&image,(char *) NULL,resource_info.image_geometry,
      scale_geometry);
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
  if (resource_info.monochrome)
    QuantizeImage(image,2,8,False,GRAYColorspace,True);
  (void) strcpy(image->filename,filename);
  (void) PrintImage(image,resource_info.image_geometry);
  if (verbose)
    {
      /*
        Display detailed info about the image.
      */
      if (image->class == DirectClass)
        image->colors=NumberColors(image,(FILE *) NULL);
      (void) fprintf(stderr,"[%u] %s %ux%u",image->scene,image->filename,
        image->columns,image->rows);
      if (image->class == DirectClass)
        (void) fprintf(stderr," DirectClass ");
      else
        (void) fprintf(stderr," PseudoClass ");
      (void) fprintf(stderr,"%uc %s %lds\n",image->colors,image->magick,
        time((time_t *) 0)-start_time+1);
    }
  DestroyImage(image);
  XCloseDisplay(display);
  return(False);
}
