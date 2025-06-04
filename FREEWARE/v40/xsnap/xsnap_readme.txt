
   xsnap is a program that allows one to interactively take a 
"snapshot" of a region of the screen.  This snapshot is then 
saved to a window. I've found it useful for such things as 
comparing a window's "before and after" pictures when debugging 
graphics code, and quicker than using xwd and xwud when there's 
no other reason to save a window to a file.  However, it does eat
up server memory, since it creates a pixmap for each snapshot.

   It also illustrates how one can have a user interactively 
specify a region of the screen or of a window using rubberbanding.  
The code for doing so is pretty self-contained and, if one is interested, 
it should be pretty easy to rip it out and use it in something else.
 
   As with all free code: no warranty, any shape, any color.

                           -- Clauss Strauch
                   internet:  cbs@cad.cs.cmu.edu
		       uucp:  ...!harvard!cs.cmu.edu!cbs
                     BITnet:  cbs@cad.cs.cmu.edu%CMCCVMA


