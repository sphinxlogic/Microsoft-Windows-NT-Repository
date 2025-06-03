/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                 IIIII  M   M  PPPP    OOO   RRRR    TTTTT                   %
%                   I    MM MM  P   P  O   O  R   R     T                     %
%                   I    M M M  PPPP   O   O  RRRR      T                     %
%                   I    M   M  P      O   O  R R       T                     %
%                 IIIII  M   M  P       OOO   R  R      T                     %
%                                                                             %
%                                                                             %
%             Import X11 image to a machine independent format.               %
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
%  Import is an X Window System window dumping utility.  Import allows X
%  users to store window images in a specially formatted dump file.  This
%  file can then be read by the Display utility for redisplay, printing,
%  editing, formatting, archiving, image processing, etc.  The target
%  window can be specified by id or name or be selected by clicking the
%  mouse in the desired window.  The keyboard bell is rung once at the
%  beginning of the dump and twice when the dump is completed.
%
%  The import program command syntax is:
%
%  Usage: import [options ...] file
%
%  Where options include:
%    -border          include image borders in the output image
%    -compress type   RunlengthEncoded or QEncoded
%    -delay seconds   pause before selecting target window
%    -display server  X server to contact
%    -frame           include window manager frame
%    -monochrome      transform image to black and white
%    -scene value     image scene number
%    -screen          select image from root window
%    -verbose         print detailed information about the image
%    -window id       select window with this id or name
%
%  Change '-' to '+' in any option above to reverse its effect.
%  For example, +frame means do not window manager frame.
%
%  By default, 'file' is written in the MIFF image format.  To specify
%  a particular image format, precede the filename with an image format
%  name and a colon (i.e. mtv:image) or specify the image type as the
%  filename suffix (i.e. image.mtv).  Specify 'file' as '-' for standard
%  input or output.
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
      "-border          include image borders in the output image",
      "-compress type   RunlengthEncoded or QEncoded",
      "-delay seconds   pause before selecting target window",
      "-display server  X server to contact",
      "-frame           include window manager frame",
      "-monochrome      transform image to black and white",
      "-scene value     image scene number",
      "-screen          select image from root window",
      "-verbose         print detailed information about the image",
      "-window id       select window with this id or name",
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
  (void) fprintf(stderr,
  "\nBy default, 'file' is written in the MIFF image format.  To specify a\n");
  (void) fprintf(stderr,
    "particular image format, precede the filename with an image format\n");
  (void) fprintf(stderr,
    "name and a colon (i.e. mtv:image) or specify the image type as the\n");
  (void) fprintf(stderr,
    "filename suffix (i.e. image.mtv).  Specify 'file' as '-' for standard\n");
  (void) fprintf(stderr,"input or output.\n");
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
    *server_name,
    *target_window;

  Display
    *display;

  Image
    *image;

  int
    i,
    x;

  time_t
    start_time;

  unsigned int
    borders,
    compression,
    frame,
    scene,
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
  resource_value=
    XGetResource(resource_database,client_name,"compression","Runlength");
  if (Latin1Compare("qencoded",resource_value) == 0)
    compression=QEncodedCompression;
  else
    compression=RunlengthEncodedCompression;
  resource_value=XGetResource(resource_database,client_name,"frame","False");
  frame=IsTrue(resource_value);
  resource_value=XGetResource(resource_database,client_name,"scene","0");
  scene=atoi(resource_value);
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
        case 'c':
        {
          compression=NoCompression;
          if (*option == '-')
            {
              i++;
              if (i == argc)
                Error("missing type on -compress",(char *) NULL);
              if ((*argv[i] == 'R') || (*argv[i] == 'r'))
                compression=RunlengthEncodedCompression;
              else
                if ((*argv[i] == 'Q') || (*argv[i] == 'q'))
                  compression=QEncodedCompression;
                else
                  Error("invalid compression type on -compress",(char *) NULL);
            }
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
        case 's':
        {
          if (strncmp("scene",option+1,4) == 0)
            {
              i++;
              if ((i == argc) || !sscanf(argv[i],"%d",&x))
                Error("missing scene on -scene",(char *) NULL);
              scene=atoi(argv[i]);
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
        case 'w':
        {
          i++;
          if (i == argc)
            Error("missing id, name, or 'root' on -window",(char *) NULL);
          target_window=argv[i];
          break;
        }
        case 'v':
        {
          verbose=(*option == '-');
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
  image->scene=scene;
  if (resource_info.monochrome)
    QuantizeImage(image,2,8,False,GRAYColorspace,True);
  if (compression != UndefinedCompression)
    image->compression=compression;
  (void) strcpy(image->filename,filename);
  (void) WriteAlienImage(image);
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
      (void) fprintf(stderr,"%dc %s %lds\n",image->colors,image->magick,
        time((time_t *) 0)-start_time+1);
    }
  DestroyImage(image);
  XCloseDisplay(display);
  return(False);
}
