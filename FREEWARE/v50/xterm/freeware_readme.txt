XTERM, Utilities, X Windows terminal emulator

The xterm program is a terminal emulator for the X Windows system. This
terminal emulator provides DIGITAL VT102 and Tektronix 4014 compatible 
terminal emulations for programs that cannot use the X Windows system 
directly.

This is an OpenVMS port of xterm version 1.22, originally from:

  http://www.clark.net/pub/dickey/xterm/xterm.html

This 1.22 version is rather more recent than the previously available and
rather old OpenVMS port of xterm -- though that version of xterm had the 
confusingly higher version 2.01.

You can also acquire a current full distribution, along with compatible 
XMU and XAW3D library files from:

  http://seqaxp.bio.caltech.edu/pub/SOFTWARE/

This distribution includes a binary for OpenVMS 7.2-1 with DECwindows V1.2-5.

The following features all seem to work (at least "mostly"):

  VT emulation 80 chars and 132 chars
  Tek emulation
  PRINT screen
  cut/paste

There is currently no command logging enabled and the XMU and XAW3D bits
are linked into the image statically.  It should be possible to get the 
X11KIT to work if you can figure out why the _XA_ bits aren't showing up 
in the shared library.

Copy the XTERM.DAT file for your SYS$SYSROOT:[DECW$DEFAULTS.USER] directory
to pick up reasonable fonts.

Caution:  if you turn on num_lock it blocks the application keypad!

For additional information, please see the xterm FAQ 

