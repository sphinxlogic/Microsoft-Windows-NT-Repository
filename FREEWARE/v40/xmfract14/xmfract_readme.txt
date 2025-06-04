
###############################################################################

        xmfract version 1.4 - a freeware fractal generation program.
          Copyright (C) 1995 Darryl House and the Stone Soup Group.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Some source code for this program is covered by additional copyrights.
    See the copyright notices in the source files for more information.

    You should have received a machine-readable and/or hard copy of the
    GNU General Public License along with this program; if not, write to the
    Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

###############################################################################

  Please carefully read the "COMPILATION AND INSTALLATION" section below
  for important information about the configuration options for your site.

  This is xmfract 1.4, an X11R[5/6] Motif program based upon a
  popular DOS freeware program named Fractint. It is a true graphic
  user interface quite different from that program.

  The program will calculate, display and manipulate over 90 discrete
  fractal types with very fast algorithms. Other features include zooming,
  animation (color cycling), color editing, spherical projection, and an
  amazing wealth of parameters. There are too many features to list here.

  Also included are dozens of ancillary files such as color map files,
  parameter files, formula files, and an extensive on-line help system.

  System requirements:
    The program is known run to under the following operating systems:

      OSF/1 (with Motif 1.2 or later)
      Solaris 2.x (with Motif 1.2 or later)
      Linux 1.94 or later (with XFree 3.1 and Motif 1.2 or later)
      HPUX (with Motif 1.2 or later)
      AIX (with Motif 1.2 or later)

    Motif 1.2 or later is required for this program. Motif 1.1 will
      not work with this program.

    The program has been tested under the following window managers:
      mwm olwm olvwm twm tvtwm vuewm

    Required utilities:
      The GNU utility "gunzip" and the Unix or GNU utility "tar" are
        required to unpack the archive.

    Recommended utilities:
      The GNU C compiler "gcc", while not required, is recommended.

  Disk requirements:
    2MB of space is required to download the source archive.
    The compiled program (stripped and optimized) requires 1MB of space.
    The ancillary files (see below) require 1.5MB of space.

    The amount of space required to compile the program will vary,
    depending on the compiler used, the compilation flags that
    are set, and whether the ancillary files are stored in the
    same file system as the source (the default configuration).
    About 20MB of space is recommended for the most convenient
    method of compilation and storage.

  Bugs fixed and enhancements since version 1.1:

  - Added GNU "autoconf" capability with configuration files and scripts.
  - All dialogs have been extensively generalized. There are now no
      specific widget positioning or sizing parameters, allowing the
      use of any font or window manager.
  - Added platform-dependent conditions for some system library functions
      and definitions.
  - Deleted the call to MrmInitialize() and deleted the reference to
      the Mrm library in the Makefiles.
  - Conditionally compile and/or include the Xmu library and calls
      to register the event handlers for "editres".
  - The zoom box problems reported in the previous version have been fixed.
  - The two pop-up menus have been replaced by option menus.
  - Fixed various widget heirarchy and memory allocation errors.
  - Function prototypes have been generalized.
  - varargs vs. stdarg handling has been generalized.
  - Vastly enhanced the color editor.
  - Added signal handling for Linux platforms.
  - Added a Fractint-like 'z' key function for parameters.
  - Added an Image Refresh Rate option.
  - Added a parameter forwarding option.
  - Added the Fractint zoombox pan capability.
  - Added a Fractint-like color editor freestyle capability.
  - Added palette randomization functions and an associated dialog.
  - Enhanced the handling of formula, ifs, and lsystem fractal types.
  - Enhanced the initialization file and environment variable handling.
  - Updated this file and the on-line help files.
  - Added more preset window sizes.
  - Fixed all reported bugs.
  - Added an icon.

  Table of Contents:

  1: COMPILATION AND INSTALLATION
  2: USER INFORMATION
  3: COMMAND LINE OPTIONS
  4: GETTING STARTED
  5: DEVELOPER INFORMATION
  6: BUGS
  7: CORRESPONDENCE
  8: TO-DO LIST
  9: GNU GENERAL PUBLIC LICENSE

  ===========================================================================
  1: COMPILATION AND INSTALLATION

  Unpack the archive with gunzip and tar:

    prompt% gunzip xmfract.beta.tar.gz
    prompt% tar xvf xmfract.beta.tar
    prompt% rm xmfract.beta.tar

  This will leave a directory named xmfract_1.4 that contains the
  source archive and configuration files.

  Also included are dozens of ancillary files used at runtime
  (*.par, *.frm, *.map, etc.), and a directory containing the on-line
  help files.

  Type "man editres" for more information about editres. If it is
  not installed on your system, it is of no concern to this program.

  Editres is a resource editor that is useful for debugging and
  figuring out the names of specific widgets, which is also handy
  for customizing the X resource file.

  All you need to know about your system is where the Motif include
  and library directories are, and whether you have (and wish to use)
  the Xmu library for editres. Autoconf will try to figure out the rest.

  By default (configurable -- these are just defaults):
    The program will be installed in /usr/local/bin
    The ancillary files will be installed in /usr/local/xmfract
    The help files will be installed in /usr/local/xmfract/help

  TO CONFIGURE THE PROGRAM FOR YOUR SITE:

  The pertinent options for "configure" are:
    (type ./configure -help for syntax information and more options)

    --prefix (or make install prefix=<path>)
        This changes the /usr/local prefix for everything.

    --exec-prefix (or make install exec_prefix=<path>)
        This changes just the /usr/local prefix for the xmfract executable.

       "make install bindir=<path>" has the same effect -- the program
          will be installed as <path>/xmfract

       "make install libdir=<path>" changes the installation path for the
          ancillary files. They will be installed in <path> and the help
          files will be installed in <path>/help

       "make install helpdir=<path>" will change the path to the help files.

    --with-editres
        Enables the editres protocol and includes the Xmu libraries.

    --with-motif="motif_includes=DIR motif_libraries=DIR"
        Tells the Makefiles where it can find the Motif directories.

        For example, on a system where Motif is installed in the
        directory /usr/lib/motif_1.2, then motif_includes=DIR becomes
        motif_includes=/usr/lib/motif_1.2/include

     Please note that there are a puzzling combination of dashes,
     underscores and quotation marks in the arguments -- take care
     to get it correct, and use full path names, not relative paths.

  Type "./configure [options]"

  Type "make install [options]"

  If you are using a BSD compiler, you will probably get a
    gazillion warnings about "& before array or function" that
    will be safely ignored.

  Initialization:

  1. - Xmfract may obtain initialization information from many sources,
       including the command line, the environment, and two different
       names for initialization files. Choose whichever of these options
       is most convenient for you:

         a: If you use this method, no modification of any initialization
            file is necessary (yet not disallowed). Set an environment
            variable XMFRACTDIR or FRACTDIR that points to wherever you
            have placed the ancillary files (*.map, *.par, etc.).
            Then rename the file "xmfract.ini" to ".xmfractrc" (note the
            leading "dot") and place it in your home directory.

         -- or --

         b: Add a line to the top of the "xmfract.ini" that points to
            wherever you have placed the ancillary files. This must be
            a full-pathed name of a directory (relative paths or variables
            are not (yet) supported). i.e. fractdir=/usr/local/xmfract
            Note that there are no spaces before or after the "=", nor
            is the path surrounded by quotation marks. Place this file
            in your home directory or the directory from which you
            intend to run xmfract. If it is in your home directory,
            you may rename the file as .xmfractrc, or leave it named
            xmfract.ini -- both forms are supported.

       In either case, if you have placed the help files in any directory
       other than <fractdir>/help, then add a line to whichever file you
       are using for initialization (xmfract.ini or .xmfractrc) that
       points to wherever you have placed the help files.

  2. - The program contains internal default fallback X resources, so an
       app-defaults file is not necessary, but it may be useful for
       customizing the program's appearance and behaviour. If desired,
       rename the X resource file from Xmfract.ad to Xmfract and place it
       where your system's resource files are stored, or your home directory.
       Review the comments at the top of the resource file for more
       information about customizing resources.

       For Linux systems, this will probably be required because XFree has
       a problem dealing with tear-off menus in many versions of Motif.
       Set the tearOffModel resource to TEAR_OFF_DISABLED for Linux systems,
       if you experience problems with tear-off menus. Xmfract does not need
       to be re-compiled when any resource is changed.


  2: USER INFORMATION

  This documentation assumes that the user has read "Fractal Creations
  Second Edition", by Tim Wegner and Bert Tyler, published (1993) by
  Wait Group Press, 200 Tamal Plaza, Corte Madera, CA 94925. If you
  have not read this most excellent book, please do so. The book will
  give you many insights into the creation and inner workings of fractals
  in addition to the user's guide to Fractint, the original DOS program
  that this version of xmfract is based upon.

  Ken Shirriff has ported Fractint to UNIX, using an interface almost
  identical to the original Fractint. That version of the program is
  named Xfract and has several advantages over this version, and may
  be preferable for many users. Those differences include the ability to
  use the disk video mode, and that the interface has the same look-and-feel
  of the original Fractint, among many others.

  Xmfract differs from Fractint and Xfract in many ways. The first and
  most obvious of these is the graphic user interface. Many of the same
  keystrokes will invoke familiar menus, and some other keystrokes are
  changed or non-existent. Please browse each of the menus on the menu
  bar to become familiar with their contents.

  The letter to the right of the menu item indicates the letter which may
  be typed to invoke that menu item. This is called a menu item accelerator.
  The menu does not need to be displayed to invoke the item with the
  accelerator letter.

  Xmfract supports tear-off menus, if the user has not specifically
  prevented it by an entry in the resource file. Menus may be "torn off",
  or posted in windows of their own (separate from the main window), by
  selecting the dashed line at the top of the menu. To dismiss any torn-off
  menu, press the 'Esc' key while the pointer is in the menu window.

  An extensive on-line help system is installed. Browse the help files
  from the main menu's "Help" button or press the "Help" button in
  any of the various dialogs that will be displayed during the
  course of the program. A good place to start with the on-line help
  system is the main index. Select "About the Main Menu Bar" to help
  you get started.

  For a detailed description of a specific fractal formula, select the
  topic "Summary of Fractal Types" from the help index, or highlight a
  fractal type from the fractal type selection dialog, then press the help
  button in the dialog to get more information about that type.

  In general, pressing the "Done" button in a dialog will accept the
  values displayed unless an error is detected. Pressing the "Reset"
  button will reset the values to those that were in effect when the
  dialog was invoked. Pressing the "Default" button will set the values
  to preset default values. Pressing the "Cancel" button will cause the
  dialog to disappear without changing any values. Pressing "Apply" will
  accept the values displayed unless an error is detected, and not make
  the dialog disappear.

  Other specialized buttons, and menus are available in some dialogs.
  Use the on-line help system to guide you through the maze of fractal
  parameters.

  As of this writing, the zoom box functions available are move, pan,
  and stretch.

  If the user has drawn a zoom box on the screen and presses the "d"
  key (or selects "Create image" from the "Image" menu), the program
  assumes that the user wishes to zoom in on the selected area.

  Selecting Pan from the Zoom menu will draw a zoom box the full size
  of the screen. Moving this zoom box around on the screen will cause
  the program to select the area inside the zoom box and fill the
  screen with it. This effectively "pans" the image on the screen. The
  portion of the visible image that has been calculated will not be
  recalculated, just re-displayed. Any portion of the image that was
  not visible before the pan will be calculated and displayed.

  Moving the zoom box means moving it around on the screen without
  changing its size or shape. Draw a zoom box, then press the left
  mouse button down when the pointer is inside the zoom box. The outline
  of the zoom box will change to a dotted line and the cursor will change
  to a cross-arrow. While holding the mouse button down, move the cursor
  around the screen and the outline of the zoom box will follow the mouse
  pointer. When the zoom box is positioned where you want it, release the
  mouse button and the outline of the zoom box will revert to a solid line.
  All done. Note that if you press the mouse button down outside the zoom
  box, the program assumes that you want to draw a different zoom box, and
  cancels the current zoom box, and any move or stretch operation that is in
  effect. Also note that if you draw a zoom box that is too small (the
  minimum size is 25 x 25 pixels), the box will not be drawn. This also is
  a short cut for canceling a zoom box. Simply click the left mouse button
  when the pointer is outside the zoom box and the zoom box and any pending
  functions will be canceled. If you move the zoom box so that the outside
  edge of the box is outside the visible window, the area outside of
  the visible window will not be included in the zoomed in or out image.

  Stretching the zoom box means changing its shape. The zoom box may be
  stretched vertically, horizontally, or diagonally. After drawing a
  zoom box on the screen some additional lines will appear on the zoom
  box. The boxes that are created by these lines are zoom box "handles".
  Press the left mouse button down in one of the handles and the cursor
  will change to the appropriate arrow (either up, down, left, right, or
  toward one of the four corners). Hold the mouse button down and move the
  cursor and the zoom box will grow and/or shrink in the desired direction.
  Experiment with changing the shape in several directions to become
  familiar with the stretching operation.

  Options may be specified on the command line or in a parameter file
  (<filename>.par) or in a startup command file named "$HOME/.xmfractrc"
  or "./xmfract.ini".

  I have used dozens of ".par" files in this program and have found none
  that will confuse xmfract. In addition, all of the "*.map" (color map),
  "*.l" (L-System definition), "*.frm" (formula definition), and ".ifs"
  (IFS definition) files that I have used have performed as expected.

  Arguments that are not understood by xmfract are displayed in
  a dialog, and the rest of the arguments are processed normally.
  Dismiss the dialog and generate the image as usual, and you
  will probably find that the image appears as expected. If not,
  edit the offending line from the parameter file or command line
  and try again.

  Some new options may be of note - the author credits screen that pops
  up when the program lifts off may enabled or disabled with the
  command-line or startup-file option "creditscreen=". An xmfract.ini
  entry of "creditscreen=no" will prevent the credits screen from
  popping up when the program starts. Of course, the credits may be
  viewed at any time by selecting "Display authors window" from the
  "Authors" menu (or by hitting the "w" key) regardless of the
  creditscreen option. The copyright and legal disclaimer may also
  be disabled with a command-line or startup-file option named
  "disclaimer=". The disclaimer may popped up again by invoking the
  "Version information" item in the "Authors" menu.

  Experienced xmfract users may want to set the "verbose=" option. This
  option (sometimes in concert with the "overwrite=" option) when set
  set to "verbose=yes" will prevent some of the more obvious dialogs
  from being presented. For example, the program usually asks the user
  to confirm creation of image or parameter files, or whether to resize
  a window to fit an image to be read. These messages and some other
  dialogs will not appear when this option is set to "yes".

  Setting the "overwrite=" option to "overwrite=yes" will cause the
  program to automatically overwrite existing files (such as parameter
  files or images), without asking the user -- if the verbose option
  is not set to "yes".

  I recommend that you become familiar with the dialogs that appear
  periodically during the course of this program before setting the
  verbose option to "no" or the overwrite option to "yes". That will
  prevent you from becoming confused by unexpected behavior from xmfract.

  A new menu item has been added to allow the user to explicitly set
  the size of the main window. The default size is 640 x 480 pixels,
  and the maximum size is 2048 x 2048. Only the size of the menu bar is
  taken into account when calculating the maximum size for a fully
  visible image (the window manager decorations, such as borders,
  etc., are not considered). That means that if you resize the window
  to the maximum size for a fully visible image, the borders may be off
  the screen, but the image canvas will be fully visible. In addition,
  if the user elects to load an image that was created in a size that
  is different from the current window size, the user is given the option
  to resize the window to fit the new image.

  Press the left mouse button when the pointer is in the option menu and
  a list will appear that allows the user to select from several
  preconfigured window sizes, including the maximum size for a fully
  visible image. Pressing the "Default" pushbutton will set the window
  to the pre-selected default size of 640 x 480 pixels. The normal
  aspect ratio for the program (the ratio of width to height) is 4:3,
  and the program figures out the aspect ratio of the root window on
  your display and configures several preset values in that aspect
  ratio as well. To resize the window to any of these values, select
  the desired menu item.

  A note about color - if you are using a display that allows only one
  hardware color map (this is by far the most common type of color display)
  xmfract will, by default, attempt to allocate as many color cells as
  possible that results in a total number of colors which is a power of two.
  This results in a color map that will contain 256, 128, 64, 32, 16, 8, 4,
  or 2 colors, depending on how many colors are already allocated by the
  window manager and other clients running on this display. To find out how
  many colors xmfract has currently allocated, use the <tab> display ("Info
  about image" under the "Misc" menu). The "Video:" line in this dialog will
  display the current width and height of the main window and the number of
  colors that xmfract has currently allocated.  For example, the line
  "Video: 640x480x128 xmfract mode" means that the main window is 640 by 480
  pixels and that xmfract has allocated 128 colors. To use all 256 colors
  in this type of display, use the -private command line option. This will
  cause color flashing (rather rude, eh?), but it is the only method for
  displays that allow only one color map.

  Starfields may be generated only on displays where xmfract has allocated
  all 256 colors, and the images that are generated with the *.par files
  that are supplied with the program have all been generated with 256 color
  displays. To display an image that appears as the artist has visualized it,
  use a 256-color mode when invoking the related parameter file or loading
  the image file.

  I have implemented a different color palette editor -- experienced
  Fractint users will find it quite different, yet the same in some
  respects. The active vs. inactive color registers have completely
  different meanings than those in Fractint.

  To use the editor, select "Edit current color palette" from the "File"
  menu, or press the "e" key. Two color registers are displayed at the
  top of the dialog, and the color palette will be displayed below.
  Only the color in the active register may be edited. Colors in the
  inactive register will follow the mouse (until a range is selected,
  see help). The inactive register will display the color that is under
  the mouse pointer if the pointer is in the editor dialog or in the image
  window. You may select a color to edit by clicking the left mouse button
  in the desired color in either window. If you click in the image window,
  a small circle is drawn on the image to remind you where you clicked.
  A white dashed line is drawn around the selected color box in the
  editor palette, and the selected color plus it's red, green, and blue
  component values are displayed in the active register scale widgets.
  Moving the bars on the scales will change the color.

  For range selection, the palette rectangles are numbered from left
  to right, and top to bottom. That is, the upper left corner is the
  lowest numbered rectangle and the bottom right is the highest.
  To select a range of colors to operate upon, select the start of the
  range with the left mouse button. Again, a white dashed line is drawn
  around the selected color box in the editor palette. Select the end of
  the range by pressing the center mouse button down in the desired
  color. That box will now be surrounded by a solid white line,
  and the range operation menu items will be enabled (not greyed
  out). The inactive register will no longer follow the mouse. Select the
  desired range operation from the menu. You may get the inactive register
  color to follow the mouse again by canceling the range selection from
  the range sub-menu. When selecting a range to copy, it does not matter
  whether the range is selected with the active register as the low end
  of the range or whether the inactive register is the low end. In either
  case, the saved range will be loaded into memory from low to high.
  When pasting the range, the order of the selected ranged becomes
  important. The range will be pasted in a forward direction if the
  active register is the low end of the range, and will be pasted in a
  reverse order if the inactive register is the low end of the range.

  See the help topic "Color Editor Commands" for an example range
  editing session.

  Experiment with the palette editor to become familiar with these
  operations. Hands-on experimentation will make these operation much
  more clear because you can see the colors change, rather than just
  numbers in the crude boxes used as an example in the help files...

  In addition, the color cycling operations are available. Start or stop
  the color cycling by pressing the "c" key. Colors may be cycled if you
  have completed creation of an image and/or if the palette editor is
  displayed.

  You may change the speed of the color cycling by pressing the keypad
  up and down arrow keys or the cursor up and down arrow keys, whichever
  is appropriate for your keyboard. The keypad "+" and "-" keys and the
  alphabetic "+" and "-" keys will cause the palette cycle to change
  direction. Pressing the "+" and "-" key when not color cycling will
  cause the palette to rotate one step in the indicated direction. The
  keypad "Enter" key will randomize portions of the palette while the
  palette is cycling.

  A command-line or initialization file parameter named rate=nnn is
  also available. This value controls the rate at which the screen
  will update during image calculation. Valid values are 1 -> 100.
  Lower numbers will cause the screen to update less frequently,
  thereby (hopefully) reducing network overhead in situations
  where net traffic is critical. This value may also be changed when
  the program is running by selecting "Refresh Rate..." from the
  "Image" menu.

  Orbit windows and Julia Inverse method displays are toggled on and off
  by pressing the space bar (for Julia Inverse) or "o" key (for an orbits
  window) after an image is complete. The operation of these functions is
  similar to the same functions in Fractint. When the orbits window is
  displayed, the line and/or circle modes may be toggled with upper-case
  "L" or "C", respectively. The upper case is required to resolve a
  conflict where the the lower-case "l" and "c" are menu item accelerator
  letters (load color map and cycle color palette).

  There are a maximum of 10 parameters sets that are automatically logged
  by the program. You may re-create a previously created image by using
  the "Restore" menu.

  Some of the commands that were necessary in Xfract are no longer
  necessary in xmfract. For example, the -slowdisplay, -simple, and -fast
  options are not required and therefore not implemented. The program
  will silently ignore command-line options or parameter-file entries
  that are valid for Fractint or Xfract that are included in xmfract
  for backward compatibility purposes only.

  The user may reset the color, clear the screen, or reset the fractal
  parameters from a menu selection. One may also choose to reset all of
  the above from the same menu.


  3: COMMAND-LINE OPTIONS

  Some of these are detailed in the previous section, and are listed here
  as well. All of these options, except those that are preceded with a
  dash (-) may be entered either on the command line, or in an
  initialization file, or in a parameter file. The most common or
  recommended locations for these options are listed here and in the
  on-line help files (see "Command Line Parameters" in the on-line help).

  Xmfract accepts command-line parameters that allow you to start it with a
  particular mode, fractal type, starting coordinates, and just about
  every other parameter and option.

  These parameters can also be specified in a ~/.xmfractrc file, to set them
  every time you run xmfract.

  They can also be specified as named groups in a <name>.par (parameter) file
  which you can then call up while running xmfract by using the <@> command.

  In all three cases (command line, ~/.xmfractrc, and parameter file) the
  parameters use the same syntax, usually a series of keyword=value commands
  like overwrite=no. In each case below, values between "<" and ">" are
  required arguments for the specified command, and values between "[" and "]"
  are optional arguments for the specified command. Do not type any of these
  characters in the command unless otherwise directed.

  The command-line only parameters that may be set are:

    -display <hostname>:<screen>.<display>
       Specifies the X11 display to use.

    -share
       Shares the current display's colormap.

    -private
       Allocates the entire colormap (i.e. more colors).

    -geometry <WxH[{+-X}{+-Y}]>
       Changes the geometry of the initial image window.

    makemig=<x/y>
       Create a multi-image gif file "x" images wide by "y" images high.
       See "Creating Multi-image GIF files" below for more information.


  These commands and options may be specified on the command line or in
  an initialization file named "~/.xmfractrc":

    fractdir=<directory>
       Specify where the ancillary files are stored.
       This should be the first line in the initialization file.

    helpdir=<directory>
       Specify where the ancillary files are stored.

    exitnoask=<yes|no>
       Exit with or without asking for confirmation.

    creditscreen=<yes|no>
       Display the authors screen when the program starts.

    disclaimer=<yes|no>
       Display the disclaimer message when the program starts.

    verbose=<yes|no>
       Set the level of verbosity in the program.

    overwrite=<yes|no>
       Automatically overwrite existing files without confirmation.

    map=<mapfilename>
       Specify a color map file at startup.

    rate=<number>
       Specify the screen refresh rate as a percentage. A lower value
       is slower and results in less overhead on networked machines,
       and usually results in faster fractal calculations, due to the
       reduced number of graphics function calls. The valid range for
       this value is 1 -> 100.

    undo=<number>
       Specify the number of color editor commands that may undone.
       A zero means that no undos will available. This is useful on
       machines that have a limited amount of memory available.
       The valid range for this value is 0 -> 32767. The default is 8192.


  These options are most commonly specified in a parameter file:

    filename=<filename>
      Process commands from a file.

    map=<@mapfilename>
      Use 'filename' as the current color map.

    reset[=<version>]
      Causes xmfract to reset all calculation related parameters
      to their default values.

    colors=<@mapname|colorspec>
      Reads in a replacement color map from 'filename'. The 'colorspec'
      is rather long (768 characters for 256 color modes), and its syntax
      is not documented here. This form of the 'colors=' command is not
      intended for manual use - it exists for use by the 'b' command when
      saving the description of a nice image.

    type=<fractal>
      Selects the fractal type to calculate. The default is type "mandel".

    inside=<nnn|bof60|bof61|zmag|attractor|epscross|startrail|period>
      Set the color of the interior; for example, "inside=0" makes the M-set
      "lake" a stylish basic black. A setting of -1 makes inside=maxiter.
      Three more options reveal hidden structure inside the lake. Inside=bof60
      and inside=bof61, are named after the figures on pages 60 and 61 of
      "Beauty of Fractals".

    fillcolor=<color>
      Sets a block fill color for use with boundary tracing and tesseral
      calculation mode options.

    finattract=<yes|no>
      Look for finite attractor in julia types.

    function=<func[,func,func,func]>
      Allows specification of transcendental functions with fractal types
      which use variable functions. Valid values are:
        sin, cos, tan, cotan, sinh, cosh, tanh, cotanh,
        exp, log, sqr, recip (1/z), ident (identity),
        conj, flip, zero, and cosxx (cos with bug).

    outside=<nnn|iter|real|imag|mult|summ>
      Fractal exterior color options.

    maxiter=<iterations>
      Maximum number of iterations {0-32767} (default = 127).

    passes=<1|2|g|b|t>
      Select single-pass, dual-pass, solid-guessing, boundary-tracing,
      or the tesseral drawing algorithms.

    cyclerange=<min/max>
      Range of colors to cycle (default 1/255).

    ranges=<ranges=nn/nn/nn/...>
      Ranges of iteration values to map to colors.

    potential=<potential=nn[/nn[/nn[/16bit]]]>
      Continuous potential.

    params=params=<xxx[/xxx[/...]]>
      Begin with these extra parameter values.

    miim=<[depth|breadth|walk]/[left|right]/[xxx/yyy[/zzz]]>
      Params for MIIM julias. xxx/yyy = julia constant, zzz = max hits.
      Eg. miim=depth/left/-.74543/.11301/3

    initorbit=<nnn/nnn>
      Sets the value used to initialize Mandelbrot orbits to the given
      complex number (real and imag parts).

    orbitname=<type>
      Sets the orbit type for Julias.

    3dmode=<monocular|left|right|red-blue>
      Sets the 3D mode used with Julibrot.

    julibrot3d=<nn[/nn[/nn[/nn[/nn[/nn]]]]]>
      Sets Julibrot 3D parameters zdots, origin, depth, height, width,
      and distance.

    julibroteyes=<nn>
      Distance between the virtual eyes for Julibrot.

    julibrotfromto=<julibrotfromto=nn/nn[/nn/nn]>
      "From-to" parameters used for Julibrot.

    corners=<xmin/xmax/ymin/ymax[/x3rd/y3rd]>
      Begin with these coordinates. (Eg.: corners=-0.739/-0.736/0.288/0.291)

    viewwindows=<xx[/xx[/yes|no[/nn[/nn]]]]>
      Set the reduction factor, final media aspect ratio,
      crop starting coordinates (y/n), explicit x size,
      and explicit y size

    center-mag=[Xctr/Yctr/Mag]
      This is an alternative way to enter corners as a center point and
      a magnification that is popular with some fractal programs and
      publications.

    invert=<nn/nn/nn>
      Turns on inversion - turns images 'inside out'

    biomorph=<nnn>
      Biomorph coloring.

    bailout=<nnnn>
      Use this as the iteration bailout value instead of the default.
      (4.0 for most fractal types)

    symmetry=<xxxx>
      Force symmetry to None, Xaxis, Yaxis, XYaxis, Origin, or Pi symmetry.

    periodicity=<no|show|nnn>
      Controls periodicity checking. 'no' turns checking off; entering a
      number nnn controls the tightness of checking (default 1, higher is
      more stringent). 'show' or a neg value colors 'caught' points white.

    logmap=<yes|old|nn>
      Yes maps logarithm of iteration to color. Old uses pre vsn 14 logic.
      >1 compresses, <-1 for quadratic.

    rseed=<nnnnn>
      Random number seed, for reproducable plasma clouds.

    orbitdelay=<nn>
      Slows up the display of orbits (by nn/10000 sec).

    showdot=<nn>
      Colors the current dot being calculated color nn.

    decomp=<nn>
      'Decomposition' toggle, value 2 to 256.

    distest=<nnn/nnn>
      Distance estimator method.

    formulafile=<filename>
      File for type=formula.

    formulaname=<name>
      Specifies the formula name for type=formula fractals.

    lfile=<filename>
      File for type=lsystem.

    lname=<name>
      Lsystem name for 'type=lsystem' fractals.

    ifsfile=<filename>
      File for type=ifs.

    ifs=<name>
      IFS name for 'type=ifs' fractals.

    stereo=<nnn>
      Sets Stereo (R/B 3D) option:  0 = none, 1 = alternate,
      2 = superimpose, 3 = photo.

    rotation=<nn[/nn[/nn]]>
      Rotation about x, y, and z axes.

    perspective=<nn>
      Perspective viewer distance (100 is at the edge).

    xyshift=<nn/nn>
      Shift image in x & y directions (alters viewpoint).

    interocular=<nnn>
      Sets 3D Interocular distance default value.

    converge=<nnn>
      Sets 3D Convergence default value.

    crop=<nnn/nnn/nnn/nnn>
      Sets 3D red-left, red-right, blue-left, and blue-right cropping values.

    bright=<nnn/nnn>
      Sets 3D red and blue brightness values.

    xyadjust=<nnn/nnn>
      Sets 3D X and Y adjustment values.

    3d=<yes|overlay>
      Resets 3D to defaults, starts 3D mode. If overlay specified,
      does not clear existing graphics screen.

    sphere=<yes>
      Turns on 3D sphere mode.

    scalexyz=<nn/nn/nn>
      3d x, y, and z scale factors.

    roughness=<nn>
      Same as z scale factor.

    waterline=<nn>
      Colors this number and below will be 'inside' color.

    filltype=<nn>
      3D filltype.

    lightsource=<nn/nn/nn>
      The coordinates of the light source vector.

    smoothing=<nn>
      Smooths rough images in light source mode.

    latitude=<nn/nn>
      Latitude minimum and maximum.

    longitude=<nn/nn>
      Longitude minumim and maximum.

    radius=<nn>
      Radius scale factor.

    transparent=<mm/nn>
      Sets colors 'mm' to 'nn as transparent.

    preview=<yes>
      Turns on 3D 'preview' mode.

    showbox=<yes>
      Turns on 3D 'showbox' mode.

    coarse=<nnn>
      Sets preview 'coarseness' value.

    randomize=<nnn>
      Smoothes 3d color transitions between elevations.

    ambient=<nnn>
      Sets depth of shadows and contrast when using light source fill types.

    haze=<nnn>
      Sets haze for distant objects if fullcolor=1.

    fullcolor=<yes>
      Allows creation of full color .tga image with light source fill types.

    lightname=<filename>
      Targa FullColor output file name.

    ray=nnn
      Selects raytrace output file format.

    brief=yes
      Selects brief or verbose file for DKB output.


  Creating Multi-image GIF files:

    To create a multiple-image GIF file from images created with xmfract,
    the input images must all have the same width and height. Select the
    images to be merged, then save or rename the selected gif files in
    this format: "frmig_xy.gif" where x is the horizontal offset (from 0)
    in the resulting mig and y is the vertical offset (from 0) in the
    resulting mig... for example, if you have nine gif files to stitch
    together and you want them to be in a 3-gif by 3-gif mig, then the
    names will be:

    frmig_00.gif will become the upper left image
    frmig_10.gif will become the upper middle image
    frmig_20.gif will become the upper right image

    frmig_01.gif will become the middle left image
    frmig_11.gif will become the middle middle image
    frmig_21.gif will become the middle right image

    frmig_02.gif will become the lower left image
    frmig_12.gif will become the lower middle image
    frmig_22.gif will become the lower right image

    Because the command-line parameters are processed by xmfract before
    the initialization file or environment variables, the mig maker
    will not understand "fractdir" if it is set in the initialization
    file or the environment. To overcome this limitation, supply the
    "fractdir=" command-line argument in addition to the "makemig="
    argument on the command line when creating a multi-image GIF from
    files that reside in <fractdir>. If the files that are to be
    stitched together reside in the current directory (from where you
    invoke xmfract) then this additional argument is not necessary.

    In the above example, the command-line argument would be:
      xmfract fractdir=/my/images makemig=3/3

    The output multiple-image GIF file will be named "fractmig.gif"

  4: GETTING STARTED

  To generate a fractal, select the fractal type by invoking the
  "Parameters" menu (or hitting the "t" key). By default the basic
  Mandelbrot type is selected. Parameters for the selected type will
  be displayed. Change these if desired, then select "Create Image"
  from the "Image" menu (or hit the "d" key).

  Other parameter selection is found under the "Options" menu. I have
  attempted to make these selections appear (as much as possible) as they
  appear in Fractint. Again, please read the above-mentioned book for
  a MUCH more detailed explanation of these and other options.

  To load a pre-selected set of parameters, select "Load saved parameter
  set" from the "File" menu. Once a parameter file is selected, and an
  entry from that file is selected, the parameters for that image are
  loaded into the calculation routines. Select "Create Image" from the
  "Image" menu (or hit the "d" key) to begin creating an image.

  I have attempted to update the help files for xmfract, but there may be
  inconsistencies that I have overlooked. If you spot any, please bring
  them to my attention.

  5: DEVELOPER INFORMATION

  An immense debt of gratitude is owed to those who have provided
  assistance and suggestions with this version of xmfract:

  -> Thomas Winder <tom@vlsivie.tuwien.ac.at> is single-handedly responsible
       for all of the portability features of this project, in addition to
       countless bug fixes and enhancements. Tom is an excellent programmer
       and has devised fixes for many bugs that I could not solve, and has
       been very successful at getting xmfract to run on Linux platforms.
       Please direct any Linux questions to Tom at the address above.

  -> Noel Giffin <noel@erich.triumf.ca> has tirelessly experimented with
       various porting efforts and has graciously hosted an ftp home
       for the program (spanky.triumf.ca -> pub.fractals.programs.unix)
       Noel has also been a constant source of ideas and suggestions
       for making this program more flexible, more like Fractint, and
       easier to use. Almost all of the new features of the color editor,
       and many of the other user interface features of this program as
       well, are due to Noel's excellent ideas.

  Several others have volunteered time and expertise, and put up with
    a considerable amount of frustration and haggling on my part to
    get early versions of this program in runnable shape:

    Rob Laddish <robl@rugrat.sr.hp.com>
    Craig Humphrey <usagi@sans.vuw.ac.nz>
    Roger Hekansson <hson@ludd.luth.se>

  Many thanks to all of you, and to all of the others whose contributions
    were no less important. If you don't see your name here, shame on me!
    Write me, and I'll fix it immediately.

  And, of course, a huge round of applause for the Fractint authors and
    others of the Stone Soup Group, without whom there would be a great
    hole in the universe where Fractint should be.

  A little history... when I began this project, I had just received the
  Motif libraries from Sun and was itching to create just about anything.
  I decided to use Fractint to create my first Motif program, intending
  to create a simple Mandelbrot generator with a zoom box for exploration
  similar to others that I had seen in demo programs for OpenLook, etc.

  I did complete that program, and in the meanwhile had become completely
  captivated by Fractint. I kept that original xmfract as a Motif demo
  and training program for my other developers, and began this monster.

  The Fractint authors will recognize little of their original code. I
  suppose I should apologize for hacking away mercilessly at this stuff,
  but I did have to get it into a condition where I could port it. There
  are massive style changes in the code, as well as relocations of many
  of the functions. The philosophy behind this is simply a matter of
  convenience and religion so I can do nothing except say that I did it
  my way. To make it easier for me to read and manipulate, I converted
  the code to a style with which I am most familiar. That style means
  that there are no tabs in any of these files. Indentation is (usually)
  two spaces and braces are (I hope) used consistently. I read somewhere
  a great truth that states "The placement of braces is perhaps the most
  hotly contested issue facing developers of style guides today". I do
  not intend to force my vision of the world upon you, but we will have to
  live with the changes that I have made in your code :-)

  The relocations of the functions have come about for convenience. I
  implemented the different fractals and calculation types in a manner
  that made sense when I began coding xmfract. Some of the function
  names and many variable names have changed for clarity and (more
  importantly) to avoid conflicts with UNIX System V Release 4 and
  Motif system-wide variable and function names.

  This program is loosely based upon Fractint version 18.2

  Most of the original documentation for the source code is still as
  valid for xmfract as it is for Fractint. That text is in a file
  named fractsrc.doc in the original distribution of Fractint. I will
  not attempt to duplicate or update that text in this file. The
  documentation for Fractint is quite thorough and if you attempt to
  add fractal types or fix any bugs that are not GUI-related, please
  refer to that documentation before proceeding.

  We use RCS, a revision control system distributed under license by the
  Free Software Foundation, Inc., to store and track the revisions of this
  code. You will find all of the revision history notes at the bottom of
  each source code file, vs. the top where I have seen them in most other
  projects.

  I have placed all of the global variables in a file called "globals.c",
  and reference them through "globals.h". This cleans up the headers on
  the source files and generally speeds things up.

  All of the macros have been moved to "macros.h".

  All of the functions that are effectively stubs are placed in a file
  named "stubs.c". These include the per_image and per_pixel routines
  listed in fractalp.c for integer-math fractals that have a floating-
  point equivalent. Rather than removing these entries from the
  fractalspecificstuff structure, which would make a lot of other code
  changes necessary, I just left them as stubs in this file. They
  will never be invoked.

  Keyboard key definitions are in "keydefs.h".

  When the program starts, the top level shell, canvas (drawing area),
  and menus are created, then event handlers are registered to handle
  the zoom box mouse tracking and any special keyboard events (such as
  invoking the color cycler or orbits windows). The default translations
  for mouse button events in the canvas are replaced with a function
  that handles the zoom box rubber-banding and zoom box "handle" creation.

  A graphics context is created with an XOR function so that when
  a zoom box is drawn on the screen it is drawn in reverse-video.

  A callback is then registered for the canvas resize events.

  The init_data() function is called to initialize all of the variables
  and allocate any memory that is required, and process command-line
  arguments.

  Some window size preset values are initialized. These are used in the
  option menu associated with the "Set window size" dialog.

  If the user has not explicitly prevented it, the credits screen pops up.

  After that, XtAppMainLoop() is called and the program waits for events
  generated by the user.

  The GUI code takes several different forms. When I was creating this
  program, I experimented with various methods for invoking the widgets
  that are displayed on the screen. This is why you may see some widgets
  created with XtSetArg called repeatedly, and others created with
  the XtVaCreate... method, which allows a null-terminated list of args.

  I did not use a GUI-builder to create this program. I have not (yet)
  found one of those that meets my programming needs. They do not support
  a dialog that contains a variable number of widgets, for example, and
  the widget naming and callback invocation conventions in the builders
  that I have used leave a lot to be desired.

  The color cycler is invoked using a timeout. See an XLib or Xt
  programmer's manual for an explanation of timeouts if you are not
  familiar with them. As the time expires, the function that is specified
  in the XtAppAddTimeout call will be invoked and the timeout removes
  itself. To get a function to repeat periodically, the called function
  must install another timeout.

  The window resizer also uses a timeout. A recursive function is called
  to ensure that the canvas has reached the desired size. The window will
  not be the same size as the canvas because of the menu bar. In smaller
  windows, the menu bar will become thicker because the text will be
  displayed on several rows. This is taken into consideration when the
  window is resized. If the desired size cannot be reached, an error
  message is displayed and the program exits.

  When the user elects to read in an image that is not the same size
  as the current window, the resize functions are conditionally called.
  When the window is resized, the X server may not have completed the
  event processing, resulting in a distorted image. I experimented with
  several different methods for ensuring that the reconfiguration was
  complete before the image was read. All of the standard recommendations
  for this type of function failed miserably with different symptoms on
  different servers. The only one that has performed reliably is an ugly
  little method that again involves our friend the timeout. When the
  resize is invoked, the program basically loops through events during
  the timeout before displaying the image file. This ensures that the
  server has had time to reconfigure the window and notify the program
  that the configuration is complete. Even the example in the O'Reilly
  books for dealing with ConfigureNotify stuff doesn't work well in this
  situation but this one, while not exactly pretty, performs excellently.

  The zoom box code is handled in zoom.c and rubber_band.c. Basically, the
  mouse button is trapped, tracked, and released. The default translations
  for the canvas are replaced early in the program. When the left mouse
  button is pressed down in an image, the cursor is changed to a cross-hair
  and the start_rubber_band function is called. If the mouse is held down
  and moved, the track_rubber_band function is called. When the button is
  released, the end_rubber_band function is called. The "rubber_band" name
  comes from the action of the rectangles that are drawn during the above
  process. Hold the left mouse button down in an image and move the pointer
  around in a circle about the point where you pressed the button. See that
  the resulting box will stretch and shrink like a rubber band as it
  follows the mouse.

  I implemented a different color palette editor because it was easier
  than attempting to implement the existing color editor, where the DOS-style
  keyboard events and mouse tracking are tough problems. I did implement
  Fractint's color editor in one version of this program, and the results
  were less than ideal. See the operation of the Julia Inverse code for
  an example of how this was done, and what the results are (press the
  space bar after completing an image). I would like to get that (jiim)
  code updated sometime so that the inverse Julia and orbits images are
  in windows other than the main window.

  The help system is a simple, single-threaded flat file operation. The
  help files are regular non-compressed text files that are named after
  the indexes in helpdefs.h (help.x[x[x]]). This makes it very simple to
  update or add help. It does cost slightly more in disk space, however.

  If you send me bug fixes or reports, enhancements, etc., please make the
  code conform to System V Release 4 and ANSI specifications. Of course,
  all of these additions or fixes will be documented with the author's name
  and other vital statistics, in keeping with the phenomenal philosophy of
  the Stone Soup Group.

  Program size and performance has varied very little under different
  compilers in my environment. You will see, of course, a vast reduction
  in fractal generation time if you run the program on a server instead
  of a workstation.

  6: BUGS

  Probably infinite.

  - The ones I know about:

      -> On machines where the size of the 'float' data type does not
         equal the size of the 'long' data type, the log palette functions
         will fail causing alignment errors and, eventually, segmentation
         faults. A workaround (sort of) -- don't use a log palette setting
         other than zero.

      -> The 3D tranformation function cannot be interrupted with the
         ESC or CTRL-C keys. This is due to the recursive nature of
         the plasma fractal type, which also cannot be interrupted.

      -> Tear-off menus do not work on many Linux systems. If you
         attempt to tear off a menu and the program halts, you
         may disable the tear-off menus by changing the tear-off
         resource in xmfract's resource file. Change the line
         that reads: Xmfract**tearOffModel: TEAR_OFF_ENABLED
         to: Xmfract**tearOffModel: TEAR_OFF_DISABLED
         xmfract does not have to be re-compiled when any
         resource is changed. Simply stop and restart the program.

      -> The Restore menu and saved image indexes are confusing.

      -> Generating a set of thumbnail GIF images is tedious.

  7: CORRESPONDENCE

  Bug reports, complaints, comments of any sort, and any news whatsoever
  concerning this program are *all* welcome, any time. I enjoy hearing
  from anyone who has used this program. Thank you!

  Send correspondence by e-mail, phone, surface mail or fax:

    Darryl House
    Business Process Systems and Services (BPSS)
    Electronic Data Systems, Inc.
    450 W. East Avenue
    Chico, CA 95926

    e-mail: darryl@chc.bpss.eds.com

    ph:(916) 896-7076 fax:(916) 896-7193 icbm:39.48N 121.51W

  8: TO-DO LIST

  Things that will be (or maybe will be) accomplished in the next
  major release of xmfract (release 2.0). These are in addition
  to all of the features in this release, and all of the new
  features of Fractint 19.2 (newer than Fractint 18.2). These
  are not listed in any particular order.

  -- Enhance the zoom box, adding rotation and skew.

  -- Put the inverse julia and orbits displays in windows other
     than the main window.

  -- Display the contents of parameter file entries as they
     are selected.

  -- Revise the history and image restoration functions, adding
     a browsing dialog that may display thumbnail images created
     from the saved parameters.

  -- Enhance the reset options, adding modes where the options
     and/or parameters may be reset independently. Also take
     into consideration the preview mode and window resize mode.

  -- Add image manipulation routines which change the size of the
     image according to a preset range of sizes.

  -- Add 90-degree (and continuously-variable?) image rotation.

  -- Add HSV controls to the color editor.

  -- Add (up to) 24-bit color support.

  -- Add support for more image formats (TIFF, MPEG, PNG).

  -- Add (maybe various flavors of) image animation.

  -- Add a Fractint-like "Enter command string" function.

  -- Automate and enhance the MIG generator (with support
     for other image formats?)

  -- Enhance the formula parser, adding binary if/else.

  -- Add image manipulation routines for merging images.

  -- Add ray-trace (and rendering?) capabilities.

  -- Add fractal morphing capability.

  -- Revise the on-line help, adding contex-sensitive help and hyper-links.

  -- A whole host of other stuff...


  9: GNU GENERAL PUBLIC LICENSE

                    GNU GENERAL PUBLIC LICENSE
                       Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.
                          675 Mass Ave, Cambridge, MA 02139, USA
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Library General Public License instead.)  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute copies of the software, or if you modify it.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must give the recipients all the rights that
you have.  You must make sure that they, too, receive or can get the
source code.  And you must show them these terms so they know their
rights.

  We protect your rights with two steps: (1) copyright the software, and
(2) offer you this license which gives you legal permission to copy,
distribute and/or modify the software.

  Also, for each author's protection and ours, we want to make certain
that everyone understands that there is no warranty for this free
software.  If the software is modified by someone else and passed on, we
want its recipients to know that what they have is not the original, so
that any problems introduced by others will not reflect on the original
authors' reputations.

  Finally, any free program is threatened constantly by software
patents.  We wish to avoid the danger that redistributors of a free
program will individually obtain patent licenses, in effect making the
program proprietary.  To prevent this, we have made it clear that any
patent must be licensed for everyone's free use or not licensed at all.

  The precise terms and conditions for copying, distribution and
modification follow.

                    GNU GENERAL PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. This License applies to any program or other work which contains
a notice placed by the copyright holder saying it may be distributed
under the terms of this General Public License.  The "Program", below,
refers to any such program or work, and a "work based on the Program"
means either the Program or any derivative work under copyright law:
that is to say, a work containing the Program or a portion of it,
either verbatim or with modifications and/or translated into another
language.  (Hereinafter, translation is included without limitation in
the term "modification".)  Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not
covered by this License; they are outside its scope.  The act of
running the Program is not restricted, and the output from the Program
is covered only if its contents constitute a work based on the
Program (independent of having been made by running the Program).
Whether that is true depends on what the Program does.

  1. You may copy and distribute verbatim copies of the Program's
source code as you receive it, in any medium, provided that you
conspicuously and appropriately publish on each copy an appropriate
copyright notice and disclaimer of warranty; keep intact all the
notices that refer to this License and to the absence of any warranty;
and give any other recipients of the Program a copy of this License
along with the Program.

You may charge a fee for the physical act of transferring a copy, and
you may at your option offer warranty protection in exchange for a fee.

  2. You may modify your copy or copies of the Program or any portion
of it, thus forming a work based on the Program, and copy and
distribute such modifications or work under the terms of Section 1
above, provided that you also meet all of these conditions:

    a) You must cause the modified files to carry prominent notices
    stating that you changed the files and the date of any change.

    b) You must cause any work that you distribute or publish, that in
    whole or in part contains or is derived from the Program or any
    part thereof, to be licensed as a whole at no charge to all third
    parties under the terms of this License.

    c) If the modified program normally reads commands interactively
    when run, you must cause it, when started running for such
    interactive use in the most ordinary way, to print or display an
    announcement including an appropriate copyright notice and a
    notice that there is no warranty (or else, saying that you provide
    a warranty) and that users may redistribute the program under
    these conditions, and telling the user how to view a copy of this
    License.  (Exception: if the Program itself is interactive but
    does not normally print such an announcement, your work based on
    the Program is not required to print an announcement.)

These requirements apply to the modified work as a whole.  If
identifiable sections of that work are not derived from the Program,
and can be reasonably considered independent and separate works in
themselves, then this License, and its terms, do not apply to those
sections when you distribute them as separate works.  But when you
distribute the same sections as part of a whole which is a work based
on the Program, the distribution of the whole must be on the terms of
this License, whose permissions for other licensees extend to the
entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest
your rights to work written entirely by you; rather, the intent is to
exercise the right to control the distribution of derivative or
collective works based on the Program.

In addition, mere aggregation of another work not based on the Program
with the Program (or with a work based on the Program) on a volume of
a storage or distribution medium does not bring the other work under
the scope of this License.

  3. You may copy and distribute the Program (or a work based on it,
under Section 2) in object code or executable form under the terms of
Sections 1 and 2 above provided that you also do one of the following:

    a) Accompany it with the complete corresponding machine-readable
    source code, which must be distributed under the terms of Sections
    1 and 2 above on a medium customarily used for software interchange; or,

    b) Accompany it with a written offer, valid for at least three
    years, to give any third party, for a charge no more than your
    cost of physically performing source distribution, a complete
    machine-readable copy of the corresponding source code, to be
    distributed under the terms of Sections 1 and 2 above on a medium
    customarily used for software interchange; or,

    c) Accompany it with the information you received as to the offer
    to distribute corresponding source code.  (This alternative is
    allowed only for noncommercial distribution and only if you
    received the program in object code or executable form with such
    an offer, in accord with Subsection b above.)

The source code for a work means the preferred form of the work for
making modifications to it.  For an executable work, complete source
code means all the source code for all modules it contains, plus any
associated interface definition files, plus the scripts used to
control compilation and installation of the executable.  However, as a
special exception, the source code distributed need not include
anything that is normally distributed (in either source or binary
form) with the major components (compiler, kernel, and so on) of the
operating system on which the executable runs, unless that component
itself accompanies the executable.

If distribution of executable or object code is made by offering
access to copy from a designated place, then offering equivalent
access to copy the source code from the same place counts as
distribution of the source code, even though third parties are not
compelled to copy the source along with the object code.

  4. You may not copy, modify, sublicense, or distribute the Program
except as expressly provided under this License.  Any attempt
otherwise to copy, modify, sublicense or distribute the Program is
void, and will automatically terminate your rights under this License.
However, parties who have received copies, or rights, from you under
this License will not have their licenses terminated so long as such
parties remain in full compliance.

  5. You are not required to accept this License, since you have not
signed it.  However, nothing else grants you permission to modify or
distribute the Program or its derivative works.  These actions are
prohibited by law if you do not accept this License.  Therefore, by
modifying or distributing the Program (or any work based on the
Program), you indicate your acceptance of this License to do so, and
all its terms and conditions for copying, distributing or modifying
the Program or works based on it.

  6. Each time you redistribute the Program (or any work based on the
Program), the recipient automatically receives a license from the
original licensor to copy, distribute or modify the Program subject to
these terms and conditions.  You may not impose any further
restrictions on the recipients' exercise of the rights granted herein.
You are not responsible for enforcing compliance by third parties to
this License.

  7. If, as a consequence of a court judgment or allegation of patent
infringement or for any other reason (not limited to patent issues),
conditions are imposed on you (whether by court order, agreement or
otherwise) that contradict the conditions of this License, they do not
excuse you from the conditions of this License.  If you cannot
distribute so as to satisfy simultaneously your obligations under this
License and any other pertinent obligations, then as a consequence you
may not distribute the Program at all.  For example, if a patent
license would not permit royalty-free redistribution of the Program by
all those who receive copies directly or indirectly through you, then
the only way you could satisfy both it and this License would be to
refrain entirely from distribution of the Program.

If any portion of this section is held invalid or unenforceable under
any particular circumstance, the balance of the section is intended to
apply and the section as a whole is intended to apply in other
circumstances.

It is not the purpose of this section to induce you to infringe any
patents or other property right claims or to contest validity of any
such claims; this section has the sole purpose of protecting the
integrity of the free software distribution system, which is
implemented by public license practices.  Many people have made
generous contributions to the wide range of software distributed
through that system in reliance on consistent application of that
system; it is up to the author/donor to decide if he or she is willing
to distribute software through any other system and a licensee cannot
impose that choice.

This section is intended to make thoroughly clear what is believed to
be a consequence of the rest of this License.

  8. If the distribution and/or use of the Program is restricted in
certain countries either by patents or by copyrighted interfaces, the
original copyright holder who places the Program under this License
may add an explicit geographical distribution limitation excluding
those countries, so that distribution is permitted only in or among
countries not thus excluded.  In such case, this License incorporates
the limitation as if written in the body of this License.

  9. The Free Software Foundation may publish revised and/or new versions
of the General Public License from time to time.  Such new versions will
be similar in spirit to the present version, but may differ in detail to
address new problems or concerns.

Each version is given a distinguishing version number.  If the Program
specifies a version number of this License which applies to it and "any
later version", you have the option of following the terms and conditions
either of that version or of any later version published by the Free
Software Foundation.  If the Program does not specify a version number of
this License, you may choose any version ever published by the Free Software
Foundation.

  10. If you wish to incorporate parts of the Program into other free
programs whose distribution conditions are different, write to the author
to ask for permission.  For software which is copyrighted by the Free
Software Foundation, write to the Free Software Foundation; we sometimes
make exceptions for this.  Our decision will be guided by the two goals
of preserving the free status of all derivatives of our free software and
of promoting the sharing and reuse of software generally.

                            NO WARRANTY

  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

                     END OF TERMS AND CONDITIONS

        Appendix: How to Apply These Terms to Your New Programs

  If you develop a new program, and you want it to be of the greatest
possible use to the public, the best way to achieve this is to make it
free software which everyone can redistribute and change under these terms.

  To do so, attach the following notices to the program.  It is safest
to attach them to the start of each source file to most effectively
convey the exclusion of warranty; and each file should have at least
the "copyright" line and a pointer to where the full notice is found.

    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 19yy  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Also add information on how to contact you by electronic and paper mail.

If the program is interactive, make it output a short notice like this
when it starts in an interactive mode:

    Gnomovision version 69, Copyright (C) 19yy name of author
    Gnomovision comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
    This is free software, and you are welcome to redistribute it
    under certain conditions; type `show c' for details.

The hypothetical commands `show w' and `show c' should show the appropriate
parts of the General Public License.  Of course, the commands you use may
be called something other than `show w' and `show c'; they could even be
mouse-clicks or menu items--whatever suits your program.

You should also get your employer (if you work as a programmer) or your
school, if any, to sign a "copyright disclaimer" for the program, if
necessary.  Here is a sample; alter the names:

  Yoyodyne, Inc., hereby disclaims all copyright interest in the program
  `Gnomovision' (which makes passes at compilers) written by James Hacker.

  <signature of Ty Coon>, 1 April 1989
  Ty Coon, President of Vice

This General Public License does not permit incorporating your program into
proprietary programs.  If your program is a subroutine library, you may
consider it more useful to permit linking proprietary applications with the
library.  If this is what you want to do, use the GNU Library General
Public License instead of this License.

