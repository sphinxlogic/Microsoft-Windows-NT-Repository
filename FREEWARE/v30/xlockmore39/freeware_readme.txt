XLOCKMORE39, MOTIF_TOYS, X Window Screen Locker with many graphic demos



  Under X, run xlock.  The screen will clear, and some pretty animated
  picture (exactly which depends on which module is active) will appear
  on the screen.  If you hit a key, then the screen will clear, and
  (unless you've changed the application defaults file that I packaged
  with this) you'll get a black screen with some graphics in the top
  center.  These graphics consist of a reduced size image of the module
  you were viewing, the name of the user who executed xlock, and
  password prompt field, and some short instructions.
 
  At this point, you can either click on the graphic to return to xlock,
  or you can type a password.  If the password is verifiable as the
  root password, or the password of the user listed above, then xlock
  will terminate.  THIS IS THE ONLY WAY TO STOP XLOCK WITHOUT SHUTTING
  DOWN THE X SERVER.  That's what makes it a lock.

 
Resources: (Also taken from Darren Senn's xlock)
------------------------------------------------
 
  There are two sets of resources for XLock.  The first set are (what I
  call) global XLock resources, while the second set consists of
  module-specific resources.  I'll get more into modules a little further
  below.
 
  The global resources are:
        XLock.mode: This sets the module.  More about this later.
        XLock.font: This is the font used on the password entry screen.
        XLock.background: The background color for the password entry screen.
        XLock.foreground: The foreground color for the password entry screen.
        XLock.username: The label for the field indicating the user name.
        XLock.password: The label for the password prompt.
        XLock.info: The "short instructions" to print.
        XLock.validate: A message to display while checking the password
        XLock.invalid: A message to display if the password is incorrect
        XLock.nice: How much XLock should nice itself.
        XLock.timeout: How long to wait idle at the password prompt.
        XLock.timeelapsed: Message to see how long lock running (yes or no)
        XLock.mono: Monochrome mode (yes or no)
        XLock.nolock: disable the lock mechanism (yes or no)
        XLock.remote: allow remote locking (meaningless under linux)
        XLock.allowroot: allow the root password to unlock (yes or no)
        XLock.enablesaver: allow the system screensaver to work (yes or no)
        XLock.allowaccess: allow other clients to connect while active
        XLock.echokeys: Echo "?" for each password keypress (yes or no)
        XLock.usefirst: Ignore the first character typed (yes or no)
        XLock.verbose: Verbose mode. (yes or no)
        XLock.inwindow: allow the xlock to run in a window (yes or no)
        XLock.inroot: allow the xlock to run in the root window (yes or no)
        XLock.grabmouse: Grab the keyboard and mouse (yes or no)
 
  XLock has a number of modules which it can display.  (See the man page
  for a complete list).  It turns out that each module is characterized
  by a number of initializations, separated by a number of "draws".
  Each module has the following resources defined:

        XLock.<module>.delay: How long to wait between draws (usec)
        XLock.<module>.batchcount: May mean various things (see man page).
        XLock.<module>.cycles: Controls the timeout of screen (see man page).
        XLock.<module>.saturation: Saturation (as in HSV) of colors to use.


Acknowledgments:
----------------

  I did not write the original algorithms in any of the lock screens
  (except wator, bug, life1d, demon, and ant), although I did
  convert many of the new ones to run with xlock.  I tried to follow
  the original style of Patrick Naughton.  Updates are made at
  ftp.x.org in directory /contrib/applications.  Many of the additions
  were "borrowed" from xscreensaver (Jamie Zawinski <jwz@netscape.com>).
  Some of the others were "borrowed" from old demos from Sun.

  I will consider putting new ones in if (1) they are more or less public
  domain (i.e. BSD, sorry no GNU copyrights), (2) they are neat (I am
  biased towards mathematically based programs (especially the Computer
  Recreations of Scientific American)), and (3) I have the time.

  Also many thanks to the people that helped me with the main program
  itself mentioned in "Revisions", especially Heath A. Kehoe.


Primary site: ftp.x.org in /contrib/applications
Maintainer: David A. Bagley <bagleyd@hertz.njit.edu>
Adapted from Patrick J. Naughton's original xlock.

