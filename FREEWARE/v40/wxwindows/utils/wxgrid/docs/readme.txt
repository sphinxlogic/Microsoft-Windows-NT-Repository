
wxTableWindow 0.1
-----------------

This is a pre-prototype of a wxTableWindow class, for showing spreadsheets/
tables. I'm uploading it in the hope that someone has time to take it further
(I probably won't have time in the next month or so).

I think there's enough here to give a good start: it should start
being useful with a couple days' work, and 'releasable' with perhaps a
few more days' work. Thereafter I would hope it would evolve to get more
slick and functional.

What we have so far is a minimal wrapper for the free Motif table widget
library called Xbae (the .c files in the src directory), plus MS Windows
code which creates something vaguely resembling a spreadsheet. The MS
Windows code is actually a combination of wxWindows and native Windows
code, so could quite easily be modified to be generic, for platforms
that support drawing on a panel.

UNIX files:

  caption.c
  clip.c
  matrix.c
  caption.h
  captionp.h
  clip.h
  clipp.h
  matrix.h
  matrixp.h
  wxtablem.cc
  wxtablem.h

Windows files:

  wxtablew.cc
  wxtablew.h
  
Generic files:

  wxtable.h (includes wxtablew.h or wxtablem.h as necessary)
  test.h
  test.cc

  
Some details
============

Motif stuff
-----------

I've altered the Xbae files very slightly to be DOS-conformant
filenames, and taken them out of their original distribution for
development purposes. For a proper understanding of this widget (and
some examples and the manual), please upload the full Xbae
distribution, e.g. from AIAI's ftp site, in this directory:

  ftp.aiai.ed.ac.uk/pub/packages/wxwin/code_library/x
  
The Xbae matrix widget is very functional and optimized for
a Motif style of programming. It was very straightforward to get it
working in a subclass of wxPanel (see wxtablew.h/cc).
I haven't defined a generic wxTableWindow API, apart from the
constructor, a CreateTable call, and a GetCell(int row, int col)
call. So, you're free to invent one, perhaps using the Xbae manual as a
guide to the kind of functionality that one might need.

Windows stuff
-------------

The Windows code was derived from an example file in the Microsoft ftp
site. It uses a single EDIT (wxText) control for input, so even though
you click on a cell in the table, input is directed to the wxText
control, and output appears in both the wxText control and the cell as
you type. A sneaky solution for coding text input.

Because we're creating a widget from scratch here, there's more to be
done than for the Motif side. In particular, an efficient way of
refreshing the screen needs to be written, and a clipping region defined
to avoid drawing all over the scrollbars.

Compiling wxTableWindow
-----------------------

Don't forget to edit wx_setup.h and set USE_SCROLLBAR to 1 to enable
the wxScrollBar panel item in wxWindows (and recompile wxWindows,
obviously). The wxScrollBar class isn't necessary under Motif, only
Windows.

There are makefiles for UNIX, MS VC++ 1.5 and 16-bit Borland.

The files test.h/test.cc comprise the 'demo'.

Some random thoughts
--------------------

We need:

    - to be able to set row and column labels
    - to get/set column data
    - to change font and colour attributes
    - to have setttings for column alignment and maybe data checking
    - set column widths (in the Windows code there are hooks for this)
    - perhaps a wxTableData class, so we can read/write/handle the data
      without having to have a wxTableWindow. The wxTableWindow class
      has its own wxTableData member containing the data.
      
We could have a nice little demo where we use the recently uploaded
chart library to show graphs of the data in a wxTableWindow.

Good luck!

Julian Smart, August 28th 1995