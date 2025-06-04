NT Notes Version 1.5c Release Notes

GENERAL
-------

Please read this entire readme.txt document before installing or using
NT Notes.

NT Notes is a PC-based DEC Notes client for Windows NT, Windows 2000,
Windows ME, or Windows 98.  It does not work on Windows 95.

This software is completely unsupported.  If it works for you, fine.  If
it doesn't, you have my sympathy. 

COMMAND LINE CAPABILITY and MULTIPLE NOTEBOOKS
----------------------------------------------

NT Notes takes one optional parameter on the command line - the pathname
of a notebook file that this session is to use.  If NT Notes is launched
without an explicit notebook, it first looks for an existing NT Notes
session that was launched without an explicit notebook.  If such a
session is found, control is transferred to it.  Otherwise, a new NT
Notes session is started using the notebook recorded via
"Tools/Settings".

When NT Notes is launched with an explicit notebook, it uses that
notebook, NT Notes installation sets up NT Notes as the default
application for files of type ".not", so you can double-click on a
notebook file to launch NT Notes or you can use the START command on a
notebook file to invoke NT Notes. When NT Notes is started in any of
these ways, it does not remember the notebook pathname between sessions.


SYSTEM REQUIREMENTS
-------------------

NT Notes runs on Windows NT 4.0 on Compaq Alpha hardware, or on Windows
NT 4.0, Windows 2000, Windows ME, or Windows 98 on Intel IA-32 hardware.
TCP/IP must be configured and running.  Your machine must be connected
to the TCP/IP network on which the server for notes conferences you
wish to access resides.

NT Notes does not run on Windows 95 because that operating system lacks
the CancelIo system call, which NT Notes needs to implement its STOP
button.  As the name of the application implies, NT Notes has been
developed on and for Windows NT.  While the author has done nothing
conscious to preclude its running on Windows 98 and Windows ME, and
indeed it seems to function well there, he has not made any effort to
avoid API calls not available on Windows 98/ME.


INSTALLATION
------------

1) Choose a directory on a hard drive to hold the NT Notes kit files.

2) If you are installing on a Conpaq Alpha system, copy file
   Notes-V15ca.zip to that directory.

   If you are installing on an Intel IA-32 system, copy file
   Notes-V15ci.zip to that directory.
   
3) Unzip the .zip file.

4) Launch setup.exe and follow the instructions.

The documentation for NT Notes is in online help in the product, and
also in file notes-help.txt. Select the "Notes Help File" item from the
Start menu to access this file.


CONFIGURATION
-------------

The first time that you run NT Notes, it will ask you for the location
of your NT Notes notebook.  If you are a NetNotes user, you can use
your NetNotes notebook as your NT Notes notebook; see the next
paragraph.  Otherwise, use the dialog box to select a directory and
filename for your NT Notes notebook and select OK.  NT Notes will create
an empty notebook with one class (MAIN).

Once you have a notebook, you are ready to configure your NT Notes
personal preferences.  From the "Tools" menu, select "Settings" and the
"Preferences" page.  The items you can set here are:

    Notebook Path:  This is the full pathname of your NT Notes notebook.
    You can change this by typing in a new pathname, or you can select
    "Browse..." to bring up a file open dialog box from which you can
    select a file.

    Startup Class:  The class in your notebook that will be opened when
    NT Notes starts.  You may leave this blank if you wish.

    Personal Name:  Enter the name or other text that you wish to appear
    on topics and replies that you write.

    Conference Open Action:  When you open a notes conference, NT Notes
    will either read the first unseen note, read the first topic note
    in the conference, or display a descending directory of topics in
    the conference.

    Printer Page Margin: The margin (in millimeters) to be left at the
    top, bottom, left, and right when NT Notes prints a page.  The
    default of 13 mm corresponds to 1/2 inch.

Now bring up the "Network and Mail" page.  The items you can set here
are:

    Default Notes Server Access:  This selects the default mechanism by
    which NT Notes accesses notes conferences.  NT Notes can either
    connect directly to the system that serves the conference, or it can
    route the connection through a notes server on some other machine.
    Route-through is the only option when the notes server where the
    conference lives does not support TCP/IP.  If you select route-
    through, enter the TCP/IP domain name of the route-through server
    in the space provided.

    Mail Transport:  If you wish to be able to forward notes and reply
    to the author of notes via electronic mail, change the default of
    "Disallow Mail" to "SMTP" (note that MAPI transports such as
    Microsoft Exchange and Outlook are not supported at this time).
    Enter the hostname of the SMTP server that NT Notes is to use to
    post messages in the "via host" area.  Enter your email address in
    the "From: name" field.  In "MAIL-11 Gateway", enter the domain name
    of a local SMTP server that supports routing mail via MAIL-11 on
    DECnet.  Contact your local network administrators to obtain this
    information.

Click on the OK button.

Now you're all set to use NT Notes.  It's recommended that you make a
backup copy of your NT Notes notebook every so often.


VMS NOTES FEATURES PRESENT IN NT NOTES
--------------------------------------

NT Notes is intended to be a full-function DEC NOTES client.  It
supports the following DEC NOTES features:

    - notebook classes and entries
    - TCP/IP access to notes conferences, including use of route-through
    - topics and replies:  enter/delete/show/modify
    - keywords:  add/delete/show/modify
    - markers:  add/delete/show/modify
    - directory: ascending/descending/by selected criteria
    - set notes seen
    - enable/disable moderator privileges
    - conference attribute show/modify
    - conference membership show/modify
    - save note to file/print note
    - notes keypad
    - directory of notes at a remote server
    - conference creation
    - forward note via email
    - reply to author of note via email


NT NOTES FEATURES NOT YET IMPLEMENTED
-------------------------------------

These features are planned for a future version but are not implemented
in Version 1:

    - expand/contract display of replies to a topic in a directory listing
    - operations on multiple notes
    - import VMS Notes notebook
    - MAPI (Exchange and Outlook) transport for email


VMS NOTES FEATURES ABSENT FROM NT NOTES
---------------------------------------

These features are not present in NT Notes and are not planned to be
implemented:

    - direct access to notes conferences (i.e., not using a server)
    - server access via DECnet
    - direct use of remote VMS Notes notebook
    - non-default text editor for note composition
    - keypad customization

[End of release notes]
