
Greetings,

Here's a PS to Text filter that works pretty well.  It is pretty
simple, as it does not interpret PS, but merely parses it based on previous
knowledge about the relevant PS operators.  

This program is not currently a part of any product.  Enjoy.

David


------------------------------------------------------------
Features:

- filters PS text back into text
- keeps page, paragraph and line breaks
- traps kerning in some PS implementations
- can handle included PS files, provided the file is by a supported 
	creator
- supports following PS creators:
  + VAX Document (Latin, and Japanese)
  + DECwrite (Latin, and Japanese)
  + dtroff
  + troff
  + Rags
- creators are registered in a run-time accessed dat file
  + it has not been too difficult to add new creators
- runs on U*x and VMS and Dos 


Usage:
- on VMS, copy executable, to any directory
  + invoke with a symbol
  + Make sure that dat file resides in your top-level directory
- on U*X, compile and link.  Please call the executable 'ps2text'
  + place dat file in top level shell
  + invoke by typing 'ps2text'
- on DOS, copy the executable ps2text.dos
- the first parameter is the input file
- the second parameter is the output file.  If missing, output goes to the
  terminal
- a modicum of help is available via -h or '?'


Notes:
- some error messages, but not ideal
- code is pretty thoroughly commented
- no Mac PS yet.  Feel free to try and get it working and send it to me
- more graphics creators are needed
- Please feel free to send flame-free comments and criticisms, bug fixes,
  new creators, and whatnot to VAXUUM::PARMENTER.  This is the first U*X work
  I've ever done, so I'm especially interested in that side of things.
- Output is usually quite wide.  It typically looks better with your terminal
  set to 132 width mode
- Code has no global variables.  If you decide to extend it, please do so
  without introducing any global variables


The posting contains:
- ps2text.c: the main converter
- psdat.c: reads the dat file
- ps2text.dat: dat file of registered PS creators
- ps2text.h: include file for the 'c' source files
- ps2text.exe: VMS executable
- ps2text.opt: VMS options file

To build:
- cc ps2text, psdat
- link ps2text/opt

To run:
- copy pstext.dat into your home directory
- define logical: $ps2text to the directory containing ps2text.dat
- define symbol:  ps2text :== $disk:[directory]ps2text.exe
- ps2text foo.ps [output file] ! output file is optional


All files are located in 
  via::$user3:[parmenter.public.ps]

Enjoy,

David
