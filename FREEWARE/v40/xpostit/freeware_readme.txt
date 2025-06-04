XPOSTIT, MOTIF_TOYS, X11 Post-it Notes program

This is a port of XPostIt obtained from comp.source.x from Athena widget set
to VMS DECwindows environment using the DECtoolkit widget set.

Some further changes have been made in the area of sending/receiving notes.
XPostit now puts the name of the sender on the title line of recieved notes,
and you can get received notes to use different colours. The resources are:

XPostit.PostItNote*background:	Yellow
XPostit.PostItNote*foreground:	Black

for normal notes,

XPostit.ReceiveNote*background:	Red
XPostit.ReceiveNote*foreground:	White

for received notes,

XPostit*NoteMenu*background:	Yellow
XPostit*NoteMenu*foreground:	Black

for the popup menu in normal notes,

XPostit*ReceiveMenu*background:	Red
XPostit*ReceiveMenu*foreground:	White

For the popup menu in received notes

There is also an MMS script for building Xpostit, in XPOSTIT.MMS

Simon Graham, 19-Feb-1991
-------------------------------------------------------------------------------

To learn about programming on multiple displays, I tried fooling around with
XPostIt and here are the results.

I have added functionality of sending and receiving note from other people
running XPostIt. I have removed a lot of Athena widget set code and conditional
compiles for VMS. Now the code contains references to XUI widgets only (plus
the Plaid widget). There isn't much VMS specific stuff left. Basically the
exchange of notes is implemented by receiver keeping tight reign on as the
selection owner of an atom XPOSTIT. When a sender takes away ownership of the
selection, the receiver gets the value of the new selection (a new note)
and then declares itself as the owner of the selection again.

I would like to add some kind of notification when one receives a note
(a reverse icon). But do not have time to do so. I will provide help
to some one inclined to do so.

This version uses one popup menu for all the notes (Instead of one per each
note) and it uses gadgets whereever possible.

You can control whether you want to receive notes and the list of people
in the Send dialog box by following resources:

XPostit*receiveNotes: false
XPostit*sendList: JOHN,DOE,NODE::0

If you want to use names like JOHN, you must define logical JOHN to be
JOHNS'NODE::0 before running postit.

Known bugs:
   1. Lower windows options doesn't work.

Jatin Desai (VIA::DESAI)
Core Applications Group
Digital Equipment Corporation
110 Spitbrook Road, Nashua, NH
-------------------------------------------------------------------------------

Following are the notes from the previous port. (It is no longer possible
to look for "ifdef VMS" and "ifndef VMS" to see the differences between the
original code and new code.)

-------------------------------------------------------------------------------
This is a port of XPostIt obtained from comp.source.x from Athena widget set
to VMS DECwindows environment using the DECtoolkit widget set.

Search for "ifdef VMS" and "ifndef VMS" through the source files to see the
changes I made. These are sometimes extensive chnages.

In a nutshell, I used following transformation:
    Form widget -> attached dialog box widget
    Text Widget -> SText widget
    command widget -> pushbutton widget
    
I also used popupmenu widget instead of form widget in one place

Known Bugs:
   1. Lower windows options doesn't work.

Differences:
   1. Main window: MB2 and MB3 usage is swapped. 
	(MB3 == lower window doesn't work)

   2. Postit notes window: Save, Destroy and Erase buttons are replaced by
      a popup menu (MB2)

   3. You can use the logical POSTIT$HOME to define where the notes are
      saved and read from. If not defined, current directory is used.

Jatin Desai (VIA::DESAI)
Core Applications Group
Digital Equipment Corporation
110 Spitbrook Road, Nashua, NH
