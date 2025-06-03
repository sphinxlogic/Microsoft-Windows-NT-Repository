/* options.c:
 *
 * finds which option in an array an argument matches
 *
 * jim frost 10.03.89
 *
 * Copyright 1989 Jim Frost.
 * See included file "copyright.h" for complete copyright information.
 */

#include "copyright.h"
#include <stdio.h>
#include "options.h"

/* options array and definitions.  If you add something to this you also
 * need to add its OptionId in options.h.
 */

static OptionArray Options[] = {

  /* global options
   */

  { "border",     BORDER,     "color", "\
Set the color used for the border around centered or placed images.", },
  { "debug",      DBUG,       NULL, "\
Turn on synchronous mode for debugging.", },
  { "default",    DEFAULT,    NULL, "\
Set the root background to the default pattern and colors.", },
  { "delay",      DELAY,      "seconds", "\
Set the automatic advance delay for all images.", },
  { "display",    DISPLAY,    NULL, "\
Indicate the X display you would like to use.", },
  { "fit",        FIT,        NULL, "\
Force the image(s) to use the default colormap.", },
  { "fork",       FORK,       NULL, "\
Background automatically.  Turns on -quiet.", },
  { "fullscreen", FULLSCREEN, NULL, "\
Use the whole screen for displaying an image.  If -onroot is also specified,\n\
the image will be zoomed to fit the size of the screen.", },
  { "geometry",   GEOMETRY,   "window_geometry", "\
Specify the size of the display window.  Ignored if -fullscreen is given.\n\
If used in conjunction with -onroot, this defines the size of the base image.", },
  { "help",       HELP,       "[option ...]", "\
Give help on a particular option or series of options.  If no option is\n\
supplied, a list of available options is given.", },
  { "identify",   IDENTIFY,   NULL, "\
Identify images rather than displaying them.", },
  { "install",    INSTALL,    NULL, "\
Force colormap installation.  This option is useful for naive window managers\n\
which do not know how to handle colormap installation, but should be avoided\n\
unless necessary.", },
  { "list",       LIST,       NULL, "\
List the images along the image path.  Use `xloadimage -path' to see the\n\
current image path.", },
  { "onroot",     ONROOT,     NULL, "\
Place the image on the root window.  If used in conjunction with -fullscreen,\n\
the image will be zoomed to fit.  -border, -at, and -center also affect the\n\
results.", },
  { "path",       PATH,       NULL, "\
Display the image path and default extensions that are loaded from the\n\
.xloadimagerc file.", },
  { "pixmap",     PIXMAP,     NULL, "\
Force the use of a pixmap as backing store.  This may improve performance but\n\
may not work on memory-limited servers.", },
  { "private",    PRIVATE,    NULL, "\
Force the use of a private colormap.  This happens automatically if a visual\n\
other than the default is used.  This is the opposite of -fit.", },
  { "quiet",      QUIET,      NULL, "\
Turn off verbose mode.  This is the default if using -onroot or -windowid.", },
  { "supported",  SUPPORTED,  NULL, "\
Give a list of the supported image types.", },
  { "verbose",    VERBOSE,    NULL, "\
Turn on verbose mode.  This is the default if using -view.", },
  { "version",    VER_NUM,    NULL, "\
Show the version number of this version of xloadimage.", },
  { "view",       VIEW,       NULL, "\
View an image in a window.  This is the default for all but xsetbg.", },
  { "visual",     VISUAL,     NULL, "\
Force the use of a particular visual to display an image.  Normally xloadimage\n\
will attempt to pick a visual which is reasonable for the supplied image.", },
  { "windowid",   WINDOWID,   "window_id", "\
Set the background of a particular window.  This is similar to -onroot and\n\
is useful for servers which use an untagged virtual root.  The window ID\n\
should be supplied as a hexadecimal number, eg 0x40003.", },

  /* image options
   */

  { "at",         AT,         NULL, "\
Load the image onto the base image (if using -merge) or the root window (if\n\
using -onroot) at a specific location.  If used in conjunction with -onroot\n\
this forces the -fullscreen flag on.", },
  { "background", BACKGROUND, "color", "\
Set the background pixel color for a monochrome image.  See -foreground and\n\
-invert.", },
  { "brighten",   BRIGHT,     "percentage", "\
Brighten or darken the image by a percentage.  Values greater than 100 will\n\
brighten the image, values smaller than 100 will darken it.", },
  { "center",     CENTER,     NULL, "\
Center the image on the base image (if using -merge) or the root window (if\n\
using -onroot).  If used in conjunction with -onroot this forces the\n\
-fullscreen flag on.", },
  { "clip",       CLIP,       "X,Y,W,H", "\
Clip out the rectangle specified by X,Y,W,H and use that as the image.", },
  { "colors",     COLORS,     "number_of_colors", "\
Specify the maximum number of colors to be used in displaying the image.\n\
Values of 1-32768 are acceptable although low values will not look good.\n\
This is done automatically if the server cannot support the depth of the\n\
image.", },
  { "dither",     DITHER,     NULL, "\
Dither the image into monochrome.  This happens automatically if sent to\n\
a monochrome display.", },
  { "foreground", FOREGROUND, "color", "\
Set the foreground pixel color for a monochrome image.  See -background and\n\
-invert.", },
  { "gamma",      GAMMA,      "value", "\
Adjust the image's colors by a gamma value.  1.0 is the default, which does\n\
nothing.  Values under 1.0 darken the image, values higher brighten it.\n\
Typically a dark image needs a value of 2.0 to 2.5.  This option is similar\n\
to -brighten but often works better.", },
  { "goto",       GOTO,       "image_name", "\
Start displaying a particlar image.  This is useful for creating image display\n\
loops.  If two images with the target name exist, the first in the argument list\n\
will be used.", },
  { "gray",       GRAY,       NULL, "\
Convert a color image to grayscale.  Also called -grey.", },
  { "grey",       GRAY,       NULL, "\
See -gray.", },
  { "halftone",   HALFTONE,   NULL, "\
Dither the image into monochrome using a halftone dither.  This preserves\n\
image detail but blows the image up by sixteen times.", },
  { "idelay",     IDELAY,     NULL, "\
Set the automatic advance delay for this image.  This overrides -delay\n\
temporarily.", },
  { "invert",     INVERT,     NULL, "\
Invert a monochrome image.  This is the same as specifying `-foreground black'\n\
and `-background white'.", },
  { "merge",      MERGE,      NULL, "\
Merge this image onto the previous image.  When used in conjunction with\n\
-at, -center, and -clip you can generate collages.", },
  { "name",       NAME,       NULL, "\
Specify that the next argument is to be the name of an image.  This is\n\
useful for loading images whose names look to be options.", },
  { "newoptions", NEWOPTIONS, NULL, "\
Clear the options which propagate to all following images.  This is useful\n\
for turning off image processing options which were specified for previous\n\
images.", },
  { "normalize",  NORMALIZE,  NULL, "\
Normalize the image.  This expands color coverage to fit the colormap as\n\
closely as possible.  It may have good effects on an image which is too\n\
bright or too dark.", },
  { "rotate",     ROTATE,     "degrees", "\
Rotate the image by 90, 180, or 270 degrees.", },
  { "smooth",     SMOOTH,     NULL, "\
Perform a smoothing convolution on the image.  This is useful for making\n\
a zoomed image look less blocky.  Multiple -smooth arguments will run\n\
the smoother multiple times.  This option can be quite slow on large images.", },
  { "xzoom",      XZOOM,      "percentage", "\
Zoom the image along the X axis by a percentage.  See -zoom.", },
  { "yzoom",      YZOOM,      "percentage", "\
Zoom the image along the X axis by a percentage.  See -zoom.", },
  { "zoom",       ZOOM,       NULL, "\
Zoom the image along both axes. Values smaller than 100 will reduce the\n\
size of the image, values greater than 100 will enlarge it.  See also\n\
-xzoom and -yzoom.", },
  { NULL,         0,          NULL, NULL }
};

OptionId optionNumber(arg)
     char *arg;
{ int a, b;

  if ((*arg) != '-')
    return(OPT_NOTOPT);
  for (a= 0; Options[a].name; a++) {
    if (!strncmp(arg + 1, Options[a].name, strlen(arg) - 1)) {
      for (b= a + 1; Options[b].name; b++)
	if (!strncmp(arg + 1, Options[b].name, strlen(arg) - 1))
	  return(OPT_SHORTOPT);
      return(Options[a].option_id);
    }
  }
  return(OPT_BADOPT);
}

static void listOptions()
{ int a, width;

  printf("\nThe options are:\n\n");

  width= 0;
  for (a= 0; Options[a].name; a++) {
    width += strlen(Options[a].name) + 2;
    if (width > 78) {
      printf("\n");
      width= strlen(Options[a].name) + 2;
    }
    printf("%s%s", Options[a].name, (Options[a + 1].name ? ", " : "\n\n"));
  }
}

static int helpOnOption(option)
     char *option;
{ int a, foundone;

  if (*option == '-')
    option++;
  foundone= 0;
  for (a= 0; Options[a].name; a++)
    if (!strncmp(Options[a].name, option, strlen(option))) {
      printf("Option: %s\nUsage: xloadimage -%s %s\nDescription:\n%s\n\n",
	     Options[a].name, Options[a].name,
	     (Options[a].args ? Options[a].args : ""),
	     Options[a].description);
      foundone= 1;
    }
  if (!foundone) {
    printf("No option `%s'.\n", option);
    listOptions();
  }
  return(foundone);
}

static void literalMindedUser(s)
     char *s;
{
  printf("The quotes around %s are unnecessary.  You don't have to be so\n\
literal-minded!\n", s);
}

void help(option)
     char *option;
{ char buf[BUFSIZ];

  /* batch help facility
   */

  if (option) {
    if (!helpOnOption(option))
      printf("\
Type `xloadimage -help [option ...]' to get help on a particular option or\n\
`xloadimage -help' to enter the interactive help facility.\n\n");
    return;
  }

  /* interactive help facility
   */

  printf("\nXloadimage Interactive Help Facility\n\n");
  printf("\
Type `?' for a list of options, or `.' or `quit' to leave the interactive\n\
help facility.\n");
  for (;;) {
    printf("help> ");
    buf[BUFSIZ - 1]= '\0';
    fgets(buf, BUFSIZ - 1, stdin);
    while(buf[strlen(buf) - 1] == '\n')
      buf[strlen(buf) - 1]= '\0';

    /* help keywords
     */

    if (!strcmp(buf, "")) {
      printf("Type `?' for a list of options\n");
      continue;
    }
    if (!strcmp(buf, "?"))
      ;
    else if (!strcmp(buf, "quit") || !strcmp(buf, "."))
      exit(0);
    else if (!strcmp(buf, "`?'"))
      literalMindedUser("the question mark");
    else if (!strcmp(buf, "`quit'")) {
      literalMindedUser("quit");
      exit(0);
    }
    else if (!strcmp(buf, "`.'")) {
      literalMindedUser("the period");
      exit(0);
    }
    else if (helpOnOption(buf))
      continue;
    listOptions();
    printf("\
You may get this list again by typing `?' at the `help>' prompt, or leave\n\
the interactive help facility with `.' or `quit'.\n");
  }
}

