----------------------------------------------------------------------------
The files listed below are available via an anonymous FTP from arizona.edu
and from the Icon BBS.


The files are the same for FTP and the BBS, although some are too large
for practical transfer over telephone lines -- it just makes it simpler
to have the same files and documentation both places.

----------------------------------------------------------------------------
IMPORTANT: It is important to use binary (image) file transfer for non-text
files.

----------------------------------------------------------------------------
Most of the Icon material here corresponds to Version 7 of the language.
Where Version 7 material is not available yet, Version 6 material is
provided.  See the notes that follow.

----------------------------------------------------------------------------
Files with the suffix .arc are in ``ARC'' format. There are dearchiving
programs for several systems as noted below.

Files with the suffix .sit are in "stuffit" format for the Macintosh;
see the notes below.

----------------------------------------------------------------------------
In some cases, "natural" file names have been modified to conform to
limitations of some operating systems.  You may wish to rename them
at your end.

----------------------------------------------------------------------------
Note:  in many cases, the files here are images of diskettes.  As such,
they preserve the structure of the physical distribution as referred
to in their documentation.  One consequence of this is that, for example,
there are ARC files within ARC files.

------------------------------------------------------------------------------
If you pick up a copy of Icon from this machine, please let us know
by mail to icon-project@arizona.edu and include a postal mailing
address.  We will add you to our Icon Newsletter mailing list if you are
not already on it. On request, we'll send you printed copies of the
documentation on installing and using Version 7 of Icon.

------------------------------------------------------------------------------
Note that the compressed files were produced with Version 4.0 of compress.
The Version 4.0 format is incompatible with earlier versions, notably 3.0, but
compress.bdl is the source for 4.0.  Thus, if your version of compress has
obvious difficulty with these compressed files, just pick up compress.bdl
and use it.

------------------------------------------------------------------------------
If you are not familiar with the ARC utility, get the executable binary and
enter

	arc h

To unpack all of the files in an archive, use

	arc x filename.arc

or

	pkxarc filename.arc


----------------------------------------------------------------------------
name		   size		date	   description
----------------------------------------------------------------------------

Utilities:
----------
arc.exe		  32429		10/17/86   MS-DOS archive utility.

pkxarc.com	  11482		 9/29/88   Fast MS-DOS dearchiving utility.

arc.prg		  35840		 7/30/87   Atari ST archive utility.

compress.bdl	  59090		 5/29/86   "Bundled" files for compress(1). Use

		   				sh compress.bdl

					   in a suitable directory to unload.

make.arc	  60136		 2/15/88   Public-domain UNIX-style make for
					   MS-DOS; executables and source.

unstuff.hqx	  57220		11/15/88   Macintosh unpacking utility
					   "Unstuffit" for Stuffit 1.5 (.sit)
					   files.  Rename to Unstuffit.hqx and
					   convert to Macintosh application
					   using BinHex 4.0.

os2arc.exe	  45098		10/21/88   OS/2 version of ARC utility.


os2util.arc	  68405		10/21/88   OS/2 archive containing file move
					   and comparison utilities.

-------------------------------------------------------------------------------

Documentation:
--------------

	Note:  .doc files are formatted for monospaced devices such as
	       terminal screens and line printers.  .ps files are PostScript
	       source suitable as input to PostScript printers.

overview.doc	  24797		 10/3/88   Overview of the Icon programming
					   language.
overview.ps	  65225		 10/8/88

version7.doc	  41737		 10/3/88   Description of the features of
					   Version 7 of Icon.
version7.ps	 101056		 10/8/88

unix_ins.doc	  63923		 10/3/88   Installation instructions for
					   Version 7 of Icon on UNIX systems.
unix_ins.ps	 116425		 10/8/88

unix_pi.doc	  15078		 10/3/88   Instructions for building personal-
					   ized interpreters for Icon on UNIX
unix_pi.ps	  41499		 10/8/88   systems.

unix_vt.doc	  73659		 10/3/88   Instructions for building variant
					   translators for Icon on UNIX systems.
unix_vt.ps	 137967		 10/8/88

port.doc	  43842		 10/3/88   Instructions for porting Icon to new
					   systems.
port.ps		 101736		 10/8/88

library6.doc	  53214		 10/3/88   Manual for Version 6 of the Icon
				 	   program library.
library6.ps	 125145		 10/8/88

-------------------------------------------------------------------------------

Icon Program Material:
----------------------

atari_e.arc	 253413		 5/23/88   Executables for Version 7 of Icon
					   for the Atari ST.

atari_s.arc	 373095		10/10/88   Source code for Version 7.2 of Icon
					   for the Atari ST.  Not tested here.
					   No test suites.

atari_s6.arc	 403687		  8/5/87   Source code for Version 6 of Icon
					   for the Atari ST.

bigints.cpi	 261632		10/15/88   Modifications to Version 7.0 Icon
					   source to support arbitrary-
					   precision integer arithmetic.
					   Note: compatible with 7.0 only.
					   Has some dependencies on Berkeley
					   UNIX.

bigints.tar	 286720		10/15/88   As above.

dos_e_1.arc	  61768		 10/3/88   Large-memory model executables for
dos_e_2.arc	 345917		 10/3/88   Version 7.5 of Icon for MS-DOS.

dos386_e.arc	 309433		 9/28/88   MS-DOS/386 Version 7 Icon executables

dos_e_s.arc	 194086		 2/3/87	   Small-memory model executables for
					   Version 6 of Icon for MS-DOS.

dos_ipl.arc	  73944		10/31/86   Icon program library for MS-DOS.

dos_s_1.arc	 333274		 9/29/88   Source code for the MS-DOS imple-
dos_s_2.arc	 355048		 9/29/88   mentation of Version 7.5 of Icon.

macmpw_e.hqx	 284130		11/14/88   Macintosh (MPW) Version 7.0 Icon
					   executables in Stuffit 1.5 format,
					   packaged in BinHex 4.0 format.

mac_s_1.hqx	 448527		11/15/88   Macintosh (MPW) Version 7.0 Icon
mac_s_2.hqx	 259779	       	11/15/88   source in Stuffit 1.5 format,
					   packaged in BinHex 4.0 format.

os2_e.arc	 227714		10/21/88   OS/2 Version 7.5 Icon LMM
					   executables.

port_f_1.arc	 280499		 3/20/88   Source code for Version 7 of Icon
port_f_2.arc	 288535		 3/20/88   for porting to new operating systems;
					   flat file system version.

unix.cpi	1710080		 2/27/88   Version 7 Icon for UNIX in cpio(1)
					   format. Unload using cpio -icd.

unix_cpi.Z	 824114		 2/27/88   As above, but compressed using
					   compress(1). Rename to unix.cpi.Z

bin.cpi		    512		 10/9/88   Version 7  of Icon for UNIX in
					   separate files (bin.cpi through
book.cpi	  62464		 10/9/88   vt.cpi) containing everything in
					   unix.cpi above.
config.cpi	 152576		 10/9/88

docs.cpi	 272384		 10/9/88

h.cpi		  43520		 10/9/88

icont.cpi	  16896		 10/9/88

iconx1.cpi	 246784		 10/9/88

iconx2.cpi	 228352		 10/9/88

ilink.cpi	  84480		 10/9/88

ipl.cpi		  89088		 10/9/88

itran.cpi	 208384		 10/9/88

pi.cpi		   3072		 10/9/88

samples.cpi	  13312		 10/9/88

tests.cpi	 253952		 10/9/88

top.cpi		   7680		 10/9/88

vt.cpi		  39936		 10/9/88

unix.tar	2519040		 2/27/88   Version 7.0 Icon hierarchy for UNIX
					   in tar(1) format.

unix_tar.Z	1005334		 2/27/88   As above, but compressed using
					   compress(1). Rename to unix.tar.Z

vms.bin		2793472		 2/20/88   Source and binaries for Version 7.0
					   of Icon for VAX/VMS.

vmsget.doc	   1752		 2/20/88   What to do on a VAX/VMS system
					   after retrieving vms.bin.

xenix_e.cpi      297984		 9/29/88   Executables for Version 7 of Icon
				 	   for XENIX in cpio format.

xnx386_e.cpi     223744		 9/29/88   Executables for Version 7 of Icon
					   for XENIX/386 in cpio format.

----------------------------------------------------------------------------

SNOBOL4 Program Material:
-------------------------

s4v311.sil	 245374		 10/9/88   Source code for Version 3.11 of
					   SNOBOL4.

s4_sil.Z	 104641		 10/9/88   Compresed form of s4v311.sil.
					   Rename as s4v311.sil.Z.

s4syntax.sil	   3551		 10/9/88   Syntax table description for
					   s4v311.sil.

s4mac360.sil	  56763		 10/9/88   IBM 360 macro definitions for
					   SNOBOL4.

s4sub360.sil	  59318		 10/9/88   IBM 360 subroutines for SNOBOL4.

vanilla.arc	 257851		 10/9/88   Public-domain subset of SNOBOL4+
					   for MS-DOS.

----------------------------------------------------------------------------

Odds and Ends:
--------------

group83.txt	  58147		 10/8/88   Electronic mail from icon-group by
					   year.
group84.txt	  27285		 10/8/88

group85.txt	  43454		 10/8/88

group86.txt	  21271		 10/8/88

group87.txt	 282418		 10/8/88

group88a.txt	 332664		 10/8/88

group88b.txt	 186082		 10/8/88
