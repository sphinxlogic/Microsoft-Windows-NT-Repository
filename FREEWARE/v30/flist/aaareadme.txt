========================================================================
Copyright © 1990--1994  by  MadGoat Software.  This code may be freely
distributed and modified for non-commercial purposes as long  as  this
copyright notice is retained.
========================================================================

FLIST  v2.2-1  is  a file and directory manager written in VAX TPU and
VAX C (DEC C).  The program was originally written in 1987  by  Hunter
Goatley.   FLIST  v2.0  includes  many  new  features, added by Hunter
Goatley and Peter Galbraith.  (Generic  EVE  and  DCL  code  by  Peter
Galbraith is provided to support a "kept" EVE subprocess. This code is
compatible with the code used by EVEplus.)

Please forgive me for the bad code---I wrote it quickly and have never
had the time to go back and really do  much  with  it.   It  isn't  as
pretty as I'd like, but it works.

To  build,  simply execute BUILD_FLIST.COM.  This version of FLIST has
been tested on VMS V5.3-1 through v5.5-1 and on  OpenVMS  AXP  (Alpha)
v1.0.   Earlier  versions  will  report  errors during startup because
FLIST uses new (since 5.2) TPU features to change default  directories
and provide support for DECwindows.

To use, you must define a logical and a symbol:

	$ define FLIST_TPU_SECTION device:[dir]FLIST.TPU$SECTION
	$ flist :== $device:[dir]flist.exe

You can then type

	$ FLIST [file-spec]

The optional file-spec can contain wildcards.  Once inside of FLIST, you
can type H for a one-line description of some of the commands.  Pressing
PF2 will show you a list of all key definitions.

All  of  the  FLIST  commands  consist  of  either single-key presses or
GOLD-key combinations; be careful which characters you type once  inside
FLIST.  (If  you  use  a  DECwindows  mouse  for  cut  and paste between
DECterms, be careful that you don't paste into a DECterm running  FLIST,
because each pasted character will be treated as an FLIST command.)

If  you  don't  want  to define the logical, simply change the following
line in FLIST.MAR to point to the section file FLIST.TPU$SECTION:

	SECTION:	.ASCII	/FLIST_TPU_SECTION/

Simply replace FLIST_TPU_SECTION with the full file specification.

If you find bugs, drop me a line and I'll see what I can do.  If you fix
it, send me the fix and I'll incorporate it in my version.

Known limitations:

  o  Long  file  names  get  truncated and can't be viewed; it's one of
     those little bugs I just haven't gotten to yet.

A mailing list has been established for FLIST discussions and
announcements.  To subscribe, send the following line in the body of a mail
message to LISTSERV@WKUVX1.BITNET

SUBSCRIBE FLIST-L "Your real name"

I welcome any comments.
Hunter
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Hunter Goatley, VAX Systems Programmer		E-mail:	GOATHUNTER@WKU.EDU
ACRS, Western Kentucky University		Voice:	502-745-5251
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

FLIST v2.1 Description

Authors:	Hunter Goatley, goathunter@wkuvx1.bitnet
		Peter Galbraith, galbrth@ac.dal.ca

A mostly-complete list of changes can be found at the beginning of the
FLIST.TPU source file.  A number of bugs were fixed.  The following
list presents the biggest changes/enhancements to FLIST v2.1:

  o  The RETURN key has been redefined so that it will VIEW a file in
     the main buffer and return to the directory if viewing a file.
     This change should be much more natural than the previous methods
     for viewing and returning.
  o  The wildcard spec for the search is displayed on the status line.
  o  If only one file matches the wildcard spec, you are automatically
     placed in the view buffer.  I frequently use FLIST just to browse
     through a file; since I usually know the file name, I just
     specify enough to make it unique and FLIST views it....
  o  Modified purge so it can rename the version of the remaining file
     to ;1 (resets it to ;1).  This is controlled by the variable
     flist$reset_on_purge (see FLIST.INIT for more information).  This
     is enabled by default.
  o  Modified "flist_find" to accept RETURN as "yes."
  o  Added support for configuration variable flist$default_editor,
     which can be used to change the default editor from spawned EVE
     to callable EDT.  See FLIST.INIT for more information.
  o  Fixed tagged-file bug that was causing files to be untagged even
     if an error occurred.
  o  Fixed copy-file bug.
  o  Modified the check for directories so that only files with the
     directory header bit set are displayed as directories.
  o  GOLD-R has been defined to re-scan the directory, using the current
     wildcard specification.  Useful when editing multiple files that
     don't get displayed when new versions are created.  Same as "W",
     except there is no prompt for the wildcarded file spec.
  o  Various tree bugs fixed.
  o  Added ability to sort files based on the file type instead of the
     name.  Controlled by flist$sort_by_types in FLIST.INIT.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
FLIST v2.0 Description

Authors:	Hunter Goatley, goathunter@wkuvx1.bitnet
		Peter Galbraith, galbrth@ac.dal.ca

FLIST  v2.0  contains an almost-completely-new interface.  There are now
two windows created when FLIST is started:  a window  of  subdirectories
and  a  window  of  all  non-.DIR  files in the specified directory.  In
addition, a "graphical" directory tree interface has been added, as well
as support for a DECwindows mouse.

Numerous  bugs have been fixed, ranging from errors trying to copy files
to purely-visual enhancements.

Some of the features include:

	o  Separate subdirectory and file windows (can be resized)
	o  Graphical tree interface (GOLD/T)
	o  CD directory changer which uses abbreviations
	o  Ability to connect to a "kept" editor in a subprocess
	o  Separate key map lists for the various buffers, including
	   user key maps
	o  Support for DECwindows mouse ("M" and GOLD/S)
	o  Support for an initialization file containing user
	   customizations (see the file FLIST.INIT)
	o  Option to repaint screens instead of scroll
	o  Ability to copy, rename, and purge tagged files
	o  Ability to view a full-screen MESSAGES buffer
	o  Ability to shift the windows left and right
	o  Ability to show all key definitions for a given buffer (PF2)

The  number  of  keys  defined  are  too  numerous  to  list  here.  The
documentation for FLIST  is  sparse,  but  the  product  is  (hopefully)
intuitive  enough  that you should be able to understand what it does by
simply playing with it.

In  addition  to the documentation below, the sample initialization file
FLIST.INIT  contains   extensive   documentation   about   user-settable
parameters.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Kept Editor: (see also KEPT_EDIT.README)
~~~~~~~~~~~~
  FLIST allows the use of a kept editor (spawned in a sub-process)
  although none is provided.  It should work with EVEPLUS without
  modification.

  Your kept editor should:
   o Have defined the logical name KEPT_EDIT in the JOB table to hold
     the the process name of the editor.  FLIST will translate it and
     attach to this process.  This is usually defined by the COM file
     which you call to spawn and create the editor.
   o When you attach to the kept editor process, it should translate
     the logical name EDIT_NEW_FILE to get the filename of the new
     file to edit.  FLIST will define this logical in the JOB table.
     Note that a simple TPU `file_parse' or `file_search' statement
     can translate the logical for you.
   o Optionally, it may translate the logical name EDIT_NEW_DEFAULT
     which FLIST sets as the current default directory.
     Note that a TPU `file_parse' statement can translate the logical.

  The following keys are defined in FLIST for the kept editor:

  K      -  Attaches to kept editor.
  GOLD\K -  Attaches to Kept editor to edit the file under cursor.
  M3     - (mouse button 3 when mouse is activated using "M")
            Pressing M3 down highlights the file under the mouse.
            Move the mouse off of the selected line and release to cancel
            or release with the mouse still on the selected line to attach
            to the kept editor with the selected file.

Tree
~~~~
  A directory tree is displayed by pressing GOLD\T while in the
  directory/file list.  Pressing GOLD\T will display the Tree and
  position the cursor on the current default directory.  The movement
  keys are then defined to let you move around as well as:

  <RETURN>    exits back to the directory/file list
   V or M2    select directory under cursor and redo directory/file list
              (The mouse works again as describe above, move and release
               to cancel the selection)
   L          position cursor on current default directory
   GOLD\T     redo the TREE (if newer directories were created)

  The Tree is only built the first time you invoke it.  Pressing GOLD\T
  in the directory/file list afterwards simply put you back into the
  existing Tree with the cursor on the current default directory.
  Pressing GOLD\T again once in the Tree re-builds it.

CD
~~
    CD replaces the DCL "SET DEFAULT" command with a shorthand that
    permits the use of abbreviations for each directory name. It also
    allows the use of UNIX style directory separators (/).

    It automatically pops-up the directory tree if it cannot not find
    the search list below the current default (see examples).

    Normal_Use

       CD F.SE       ==> SET DEF [F*.SE*] where F and SE are abbreviations
                         to directory names.
                         If the directory is not found below, then the search
                         starts above.  All directories are validated.
       CD F/SE       ==> UNIX style directory separators can be used.

    Features - supported prefixed tokens:

       CD \          ==> Goes to SYS$LOGIN.
       CD ~          ==> Goes to SYS$LOGIN (UNIX style)
       CD \SUBDIR    ==> Goes to SYS$LOGIN and then searches for SUBDIR*.DIR
       CD ..         ==> Pops-up one level (equivalent to SET DEF [-]).
       CD $DISK:     ==> Forces "SET DEF DISK:" without validation.
                         The use of logical names can be made preceded by "$".
     Examples

      o  You may put any number of prefix token before the abbreviated
         search list:

         CD ....     will pop twice.
         CD \..      will go to SYS$LOGIN and then pop.
         CD \..A.B   will go to SYS$LOGIN, pop and search for [.A*.B*]

      o   SYS$LOGIN              Asuming the default directory is
                  \              [.FORTRAN.PROJECT]
                FORTRAN
                /     \          $ CD T      will go to TEST below.
             CODE     PROJECT    $ CD C      will go to CODE because
                         \                   nothing that starts with
                         TEST                C is found below PROJECT
                                             so that it will move up
                                             to FORTRAN and look again
                                             and find CODE.
                                 $ CD \F.C   will again go to CODE

      o  Current default is:
         DISK4:[GALBRTH.UTILITY.COM_FILES.EVE.BETA.POSITION]

         CD T.K.C will search for:
      (*) DISK4:[GALBRTH.UTILITY.COM_FILES.EVE.BETA.POSITION.T*.K*.C*]
         will not find a directory, will search for:
          DISK4:[GALBRTH.UTILITY.COM_FILES.EVE.BETA.T*.K*.C*]
         will not find a directory, will search for:
          DISK4:[GALBRTH.UTILITY.COM_FILES.EVE.T*.K*.C*]
         will not find a directory, will search for:
          DISK4:[GALBRTH.UTILITY.COM_FILES.T*.K*.C*]
         will not find a directory, will search for:
          DISK4:[GALBRTH.UTILITY.T*.K*.C*]
         and will find:
          DISK4:[GALBRTH.UTILITY.TEX.KATIA.CV]

      (*) Note that CD will actually skip this level and pop because
          it knows the directory can only be 8 levels deep.
