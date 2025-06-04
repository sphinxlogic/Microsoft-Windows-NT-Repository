xlockmore, utilities, X Windows Session Lock

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

  Please see XLOCKMORE.README for details, attributions, pointers, 
  build instructions, etc.

