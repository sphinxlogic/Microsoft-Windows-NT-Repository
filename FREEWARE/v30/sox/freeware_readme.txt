SOX, UTILITIES, Souns Manipulation Tools

		ST: Sound Tools

ST translates sound samples between different file formats,
and performs various sound effects.

This release understands "raw" files in various binary formats,
raw textual data,
Sound Blaster .VOC files, IRCAM SoundFile files, SUN Sparcstation 
.au files, mutant DEC .au files, Apple/SGI AIFF files, 
CD-R (music CD format), Macintosh HCOM files, Sounder files, 
NeXT .snd files, SUN ADPCM (compressed) .au files,
and Soundtool (DOS) files.

The sound effects include changing the sample rate, adding echo 
delay lines, applying low-, high, and band-pass filtering, 
reversing a sample in order to search for Satanic messages,
and the infamous Fender Vibro effect.

History:
This is the tenth release of the Sound Tools.  Patchlevel 11.
All previous releases went out on alt.sources.  This one should
go out on comp.sources.misc, if all goes well.

There is a nice graphical front-end for the NeXT
called GISO.  Check the comp.sys.next group
or poke around the NeXT binary sites.  Also
someone has done an OS/2 port.

SOX can now decompress SUN ADPCM compressed sound files.
Internet Talk Radio is now distributed in this format,
I believe.  If not, it soon will be.

Caveats:
SOX is intended as the Swiss Army knife of sound
processing tools.  It doesn't do anything very well,
but sooner or later it comes in very handy.
SOX is really only useable day-to-day if you
hide the wacky options with one-line shell scripts.

Channel averaging doesn't work.  The software architecture
of stereo & quad is bogus.

Installing:
Use the DOS, Unix, Amiga, OS2, or OS9 Makefile as appropriate.
The Makefile needs one option set: -DSYSV if you're on a
System V machine, or -DBSD if you're on a BSD-ish machine.
See the INSTALL file for more detailed instructions.
There are three DOS Makefiles: Makefile.bor for pre-3.0 
Borland C, Makefile.b30 for Borland 3.0, and Makefile.c70
for Microsoft C 7.0.  OS2 needs the two OS2 Makefile-type
programs.  OS9 needs Makefile.os9 for Microware Ultra C.

After compiling, run 'tests.sh'.  It should print nothing.  This
indicates that data is copied correctly.  By reading the tests
you may see how to make a sound sample file which you can play.
'monkey.au' and 'monkey.voc' are a short lo-fi monkey screech
in two supported file formats, to help you ensure that Sound 
Tools works.  Note: 'tests.sh' works only under the Unix sh(1) 
shell.  Use 'tests.com' under VMS.

Then, run 'testall.sh'.  This copies monkey.voc into all other
supported file formats, making files in /tmp.  Then, it
translates those formats back into .voc format.  This
ensures (slightly) that all of the readers & writers don't 
core-dump.

Now, read TIPS and CHEAT.  These give a background on how
SOX deals with sound files and how to convert this format
to that format, with examples for the most popular formats.

SOX uses file suffices to determine the nature of a sound sample file.
If it finds the suffix in its list, it uses the appropriate read
or write handler to deal with that file.  You may override the suffix
by giving a different type via the '-t type' argument.  See the manual
page for more information.  The 'tests.sh' script illustrates various
sox usages.

SOX has an auto-detect feature that attempts to figure out
the nature of an unmarked sound sample.  It works very well.
This is the 'auto' file format.

I hope to inspire the creation of a common base of sound processing
tools for computer multimedia work, similar to the PBM toolkit for 
image manipulation.

Sound Tools may be used for any purpose.  Source
distributions must include the copyright notices.  Binary
distributions must include acknowledgements to the creators.
The files I wrote are copyright Lance Norskog.
The contributed files are copyright by their respective authors.


        Creator & Maintainer:
                Lance Norskog           thinman@netcom.com
 
==============================

SOX was distributed on an old DECUS tape. I made some mods to compile with DEC
C on Alpha.

Patrick Moreau

pmoreau@cena.dgac.fr
moreau_p@decus.fr
