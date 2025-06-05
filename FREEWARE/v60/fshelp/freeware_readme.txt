FSHELP, utilities, A full screen help utility with lots of extra's

  FSHELP is a screen-oriented help utility. It makes use of SMG routines to 
  display help on video terminals. It has the following features

  - Full screen display of normal .HLB files (scrolling in both directions).
    Fshelp can also display data from .TLB, .OLB and .MLB files including
    an analyze/object of a module in an .OLB file.
  - Selecting subtopics by just typing the name of the subtopic or enter.
  - Allows cross-referencing of help modules via extra info in help file.
  - Allows header lines to remain on screen when scrolling.
  - Allows you to look back at the last 20 topics.
  - Allows you to mark some help topics for easy return to this topic
  - Allows you to toggle 132/80 columns
  - On all screens help can be displayed via the HELP or PF2 key
  - FSHELP checks for all libraries via the hlp$library* logicals
     and lets you select one of them via the TAB key.
  - FSHELP also interfaces to the HELP/MESSAGE databases so you can
     define FSHELP as your normal help utility (HELP:=$'directory'FSHELP)
  - There is a sharable image that can be called from any other application
     to display help (lib$find_image_symbol), so you can display help
     in FSHELP format from any image.
  - FSHELP messages are in English, Nederlands(Dutch) or Frysk (A language
     spoken in the north of Holland). You can easily add you own language
     (see the .MSG file for help). FSHELP does not translate text in helpfiles,
     only the FSHELP messages are in the selected language.
  - You can search text within a help page ((PF1-)PF3), or on this page and all 
    deeper levels ((PF1-)FIND). If you are on the top page of the help library, 
    you can search through the whole library, or even in all loaded 
    libraries.
  
  Help files and libraries are in the same format as normal VMS help files. 
  The format is described in the 
  VMS LIBRARIAN UTILITY MANUAL, chapter 5 : HELP Libraries
  with the following additions. All these commands start with an ! so
  VMSHELP will ignore them.

  a. !# Signal the end of header lines in a help text. Th(is)(ese) lines
        (upto 4) will remain on screen, even when the rest of the text
        is scrolled.
  b. ![text on screen]@library topic topic ...
        FSHELP will save this information, and when you type the 
        F12 key, a list of topics will be displayed, from which you 
        can select one. @library is optional. This gives a possibility to
        make see-also entries.
  c. !@[text on screen]DCL command|dcl command...
	FSHELP will save this information, and when you type the
	PF1-E (execute) command, FSHELP will let you select one of the 
        execute-scripts.
        FSHELP will generate a batch-job in the queue FSHELP_QUEUE (default
        SYS$BATCH) containing the above lines (separated by |). In these
        commands you can specify symbols to be asked from you.
        These symbols are separated by '. FSHELP will check if there is
        a symbol with that name. If this symbol exists FSHELP will substitute
        the value of that symbol, otherwise FSHELP will ask you for
        a substitution.
        Example  !@[header]$compute 'test'/out=q.q|$print/delete q.q
        will try to find the symbol test, and if not found will ask you
        to specify a new value for 'test'. Then a batchjob will be 
        submitted in the queue in the symbol FSHELP_QUEUE (default SYS$BATCH)
        containing the lines:
         $compute new_value/out=q.q
         $print/delete q.q
        where new_value is the value found in the symbol TEST or the value
        given by you.

  VMS HELP will ignore all lines starting with ! (as will FSHELP), so these
  lines can be included without interfering with VMS HELP.

  You can enter help topics on the command line, just as VMS HELP.
  The format is

  fshelp [@libraryname] [topic [topic...]]

 FSHELP does not only display help files, it can also display data from
  .TLB, .OLB (object and shareable image files) and .MLB files
  via the qualifier(s) /text, /object, /image and /macro

 For further help look at the FSHELP help file, you can do that by typing
 fshelp=$'directory'fshelp_alpha  (or _vax)
 fshelp/ownhelp
  Where 'directory' is the directory where fshelp (and its help file )
  can be found

 When looking at a help page, you can display the deeper topics by
 typing a Return, a /,  or a letter. This will display a menu containing
 the deeper topics. The / or letter will jump to the first topic containing
 that letter (or starting with that letter if you specified /NOSEARCHWILD).
 Typing FIND will jump to the next topic that matches.
 By typing more letters you can make your search more exact.
 PF4 or F10 returns to the previous screen. For more help type PF2 or HELP.

Usage:

Define fshelp as a foreign command
$FSHE*LP:=$'diretory'FSHELP_ALPHA   or FSHELP_VAX

Start it as normal help

$FSHELP [topic [subtopic...]]]	!give normal help
$FSHELP/OWNHELP			!give help info about FSHELP
$FSHELP/TEXT [module] 		!Display [the module in] FORSYSDEF.TLB
$FSHELP/IMAGE  [module]		!display [the module in] IMAGELIB.OLB library
$FSHELP/OBJECT [module]         !Display [the module in] STARLET.OLB library
				! in the deeper help you can get analyze/object info
$FSHELP/MACRO  [module]		!display [the module in] LIB.MLB
$FSHELP/LIBR=*.HLB		!Display help about all .HLB files in SYS$HELP
$FSHELP/LIBR=*.HLB/LOAD		!Read all info from all .HLB files to memory for
				! fast access (this takes a while, but if you do
				! not leave FSHELP, access is fast).
				!  select libraries via the TAB key.
$FSHELP/MESSAGE Accvio          !give help/mess about the ACCVIO item

The libraries chosen for the /text, /objext, /macro ,/image can be changed.
  See the .MSG file for more info

There are a lot more qualifiers, see the FSHELP/OWNHELP for more info.

Note: Of course you can also define FSHELP as HELP.

Also included is a shareable image that can be called from any application
See the FSHELP_OUTPUT_HELP_EXAMPLE.FOR for more info. 
Contents:

This package contains the following files 

In the home directory
README.FIRST		This file
CREATE_FSHELP.COM	The command file to compile/link FSHELP and 
			the sharable image
FSHELP_ALPHA.EXE	The main executable
FSHELP_SHR_ALPHA.EXE    The sharable image
FSHELP_VAX.EXE      	The main executable
FSHELP_SHR_VAX.EXE      The sharable image
FSHELP.HLB		The help library

in the [.src] directory

a. The fortran sources

FSHELP.FOR		The main program
FSHELP_MAIN.FOR         The body for the sharable image
FSHELP_DISP_INFO.FOR	Display info about open libraries/modules
FSHELP_MSGHELP.FOR      The help/message routines
FSHELP_UTIL.FOR         Utility routines
FSHELP_SCR.FOR          SMG routines
FSHELP_FILES.FOR        FIle handling (lbr)
FSHELP_OBJECT.FOR       The module for analyze/object
FSHELP_OBJECT_VAX.FOR   The module for analyze/object of vax object
FSHELP_OBJECT_ALPHA.FOR The module for analyze/object of alpha object
FSHELP_OUTPUT_HELP_EXAMPLE.FOR Example how to call the sharable image
VM_LIBRARY.FOR		Interface to LIB$GET_VM

b. the include files

FSHELP_DEF.FOR		The definitions for the interface to FSHELP_SHR
FSHELP.INC		Internal definitions to fshelp
VM_RECORD.INC		Definitions for VM_LIBRARY

c. The .cld file

FSHELP_CLD.CLD

d. The message file (includes all screen messages in all languages)

FSHELP_MSG.MSG

e. The help file

FSHELP.HLP

f. The jump vector (for VAX) for the sharable image

FSHELP_SHR.MAR

in the [.vax] subdirectory
 All the vax-objects of the sources

in the [.alpha] subdirectory
 All the alpha objects of the sources

Instructions:

unpack the .BCK file

The executables are included in the kit, but if you want to rebuild
FSHELP, do the following:
  If you have a fortran compiler you can compile and link

  $@create_fshelp 

  Otherwise, just link

  $@create_fshelp LINK

This will create the FSHELP_'architecture' and the FSHELP_SHR_'architecture'
images and the FSHELP.HLB help file.

I included the objects so you can adapt messsage files even if you do not have
a fortran compiler.

Author : Fekko Stubbe

If you have suggestions or questions, plaese mail to te address below
Email  : fshelpdev (at) oooovms.dyndns.org
   or  : fekko.stubbe (at) xs4all.nl
