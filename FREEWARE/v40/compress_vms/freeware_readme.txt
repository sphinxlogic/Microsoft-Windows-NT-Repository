COMPRESS_VMS, UTILITIES, A better VMS compress/uncompress tool


The following text comes directly from the distribution.


VMS COMPRESS

This is the UNIX compress/uncompress program, ported to VMS 5.2 by Doug Tody
of the National Optical Astronomy Observatories.

To build and use VMS compress:

    o	Type @make in the source directory to build the executable,
	compress.exe.

    o	Define the following symbols, replacing the sample executable
	pathname shown by the actual pathname on your system.

	compress	:== "$disk:[dir]compress.exe compress"
	uncompress	:== "$disk:[dir]compress.exe uncompress"

	Note that the compress.exe executable contains both the compress
	and uncompress programs.

Usage:

	compress foo.h		# compress file "foo.h"

	uncompress foo.h	# uncompress file "foo.h_z"
or
	uncompress foo.h_z

Since VMS does not like multiple "." fields in filenames and VMS filenames
are case insensitive, the usual UNIX compress convention of adding a ".Z"
suffix to the compressed file does not work, and a "_z" suffix is used
instead.

Since the compress program is C code and uses the VAX C stdio library to
open the output file, all files created by the program are of type Stream_LF.
This is fine for text files, but will probably be wrong when compressing
and later uncompressing a binary file.  As a workaround to avoid this
problem, the "-c" switch has been modified in the VMS version to allow
concatenation to an existing disk file (rather than writing to the standard
output as in the UNIX version).

For example,

	uncompress -c outfile file_z

will uncompress file "file_z", writing the uncompressed output to the
existing file "outfile".   This should be a zero length file of the
desired VMS/RMS file type.  For example, to uncompress to a 512 byte fixed
format file (the usual binary file), you could create the zero length
file as follows:

	$ create/fdl = sys$input outfile
	record
		carriage_control none
		size 512
		format fixed
	<ctrl/z>
	$

and then uncompress to the file as in the example above.
	

Caveats:
	A Stream_LF file is created in a normal uncompress operation.
	Ideally the program should preserve the file type in a compress-
	uncompress sequence of operations.

	The file modes and times are not preserved as they are in the UNIX
	version.  This feature could be added but I didn't need this feature
	so didn't bother.




Old README from UNIX version.
-------------------
Compress version 4.0 improvements:
	o compress() speedup (10-50%) by changing division hash to xor
	o decompress() speedup (5-10%)
	o Memory requirements reduced (3-30%)
	o Stack requirements reduced to less than 4kb
	o Removed 'Big+Fast' compress code (FBITS) because of compress speedup
    	o Portability mods for Z8000 and PC/XT (but not zeus 3.2)
	o Default to 'quiet' mode
	o Unification of 'force' flags
	o Manual page overhaul
	o Portability enhancement for M_XENIX
	o Removed text on #else and #endif
	o Added "-V" switch to print version and options
	o Added #defines for SIGNED_COMPARE_SLOW
	o Added Makefile and "usermem" program
	o Removed all floating point computations
	o New programs:
		compressdir - compress all files on a directory
		uncompressdir - uncompress all files on a directory
		zcmp - cmp compressed files
		zdiff - diff compressed files
	  The following are with thanks to philabs!per:
		btoa - convert binary to ascii for mailing
		atob - convert ascii to binary with checksum
		tarmail - tar, compress, btoa, and mail files
		untarmail - restore "tarmail" files

		WARNING: These last few programs are not compatible 
		with the original ones from the net.  The encoding
		has changed.  See btoa.c for more info.

The "usermem" script attempts to determine the maximum process size.  Some
editing of the script may be necessary (see the comments).  If you can't get
it to work at all, just create file "USERMEM" containing the maximum process
size in decimal.

The following preprocessor symbols control the compilation of "compress.c":

	o USERMEM		Maximum process memory on the system
	o SACREDMEM		Amount to reserve for other proceses
	o SIGNED_COMPARE_SLOW	Unsigned compare instructions are faster
	o NO_UCHAR		Don't use "unsigned char" types
	o BITS			Overrules default set by USERMEM-SACREDMEM
	o vax			Generate inline assembler
	o interdata		Defines SIGNED_COMPARE_SLOW
	o M_XENIX		Makes arrays < 65536 bytes each
	o pdp11			BITS=12, NO_UCHAR
	o z8000			BITS=12
	o pcxt			BITS=12
	o BSD4_2		Allow long filenames ( > 14 characters) &
				Call setlinebuf(stderr)

The difference "usermem-sacredmem" determines the maximum BITS that can be
specified with the "-b" flag.

memory: at least		BITS
------  -- -----                ----
     433,484			 16
     229,600			 15
     127,536			 14
      73,464			 13
           0			 12

The default is BITS=16.

The maximum bits can be overrulled by specifying "-DBITS=bits" at
compilation time.

WARNING: files compressed on a large machine with more bits than allowed by 
a version of compress on a smaller machine cannot be decompressed!  Use the
"-b12" flag to generate a file on a large machine that can be uncompressed 
on a 16-bit machine.

The output of compress 4.0 is fully compatible with that of compress 3.0.
In other words, the output of compress 4.0 may be fed into uncompress 3.0 or
the output of compress 3.0 may be fed into uncompress 4.0.

The output of compress 4.0 not compatable with that of
compress 2.0.  However, compress 4.0 still accepts the output of
compress 2.0.  To generate output that is compatable with compress
2.0, use the undocumented "-C" flag.

Check the Makefile, then "make".

README3.0 from UNIX version
----------------------------
Enclosed is compress version 3.0 with the following changes:

1.	"Block" compression is performed.  After the BITS run out, the
	compression ratio is checked every so often.  If it is decreasing,
	the table is cleared and a new set of substrings are generated.

	This makes the output of compress 3.0 not compatable with that of
	compress 2.0.  However, compress 3.0 still accepts the output of
	compress 2.0.  To generate output that is compatable with compress
	2.0, use the undocumented "-C" flag.

2.	A quiet "-q" flag has been added for use by the news system.

3.	The character chaining has been deleted and the program now uses
	hashing.  This improves the speed of the program, especially
	during decompression.  Other speed improvements have been made,
	such as using putc() instead of fwrite().

4.	A large table is used on large machines when a relatively small
	number of bits is specified.  This saves much time when compressing
	for a 16-bit machine on a 32-bit virtual machine.  Note that the
	speed improvement only occurs when the input file is > 30000
	characters, and the -b BITS is less than or equal to the cutoff
	described below.

Most of these changes were made by James A. Woods (ames!jaw).  Thank you
James!

Version 3.0 has been beta tested on many machines.

To compile compress:

	cc -O -DUSERMEM=usermem -o compress compress.c

Where "usermem" is the amount of physical user memory available (in bytes).  
If any physical memory is to be reserved for other processes, put in 
"-DSACREDMEM sacredmem", where "sacredmem" is the amount to be reserved.

The difference "usermem-sacredmem" determines the maximum BITS that can be
specified, and the cutoff bits where the large+fast table is used.

memory: at least		BITS		cutoff
------  -- -----                ----            ------
   4,718,592 			 16		  13
   2,621,440 			 16		  12
   1,572,864			 16		  11
   1,048,576			 16		  10
     631,808			 16               --
     329,728			 15               --
     178,176			 14		  --
      99,328			 13		  --
           0			 12		  --

The default memory size is 750,000 which gives a maximum BITS=16 and no
large+fast table.

The maximum bits can be overrulled by specifying "-DBITS=bits" at
compilation time.

If your machine doesn't support unsigned characters, define "NO_UCHAR" 
when compiling.

If your machine has "int" as 16-bits, define "SHORT_INT" when compiling.

After compilation, move "compress" to a standard executable location, such 
as /usr/local.  Then:
	cd /usr/local
	ln compress uncompress
	ln compress zcat

On machines that have a fixed stack size (such as Perkin-Elmer), set the
stack to at least 12kb.  ("setstack compress 12" on Perkin-Elmer).

Next, install the manual (compress.l).
	cp compress.l /usr/man/manl
	cd /usr/man/manl
	ln compress.l uncompress.l
	ln compress.l zcat.l

		- or -

	cp compress.l /usr/man/man1/compress.1
	cd /usr/man/man1
	ln compress.1 uncompress.1
	ln compress.1 zcat.1

The zmore shell script and manual page are for use on systems that have a
"more(1)" program.  Install the shell script and the manual page in a "bin"
and "man" directory, respectively.  If your system doesn't have the
"more(1)" program, just skip "zmore".

					regards,
					petsd!joe

Here is the README file from the previous version of compress (2.0):

>Enclosed is compress.c version 2.0 with the following bugs fixed:
>
>1.	The packed files produced by compress are different on different
>	machines and dependent on the vax sysgen option.
>		The bug was in the different byte/bit ordering on the
>		various machines.  This has been fixed.
>
>		This version is NOT compatible with the original vax posting
>		unless the '-DCOMPATIBLE' option is specified to the C
>		compiler.  The original posting has a bug which I fixed, 
>		causing incompatible files.  I recommend you NOT to use this
>		option unless you already have a lot of packed files from
>		the original posting by thomas.
>2.	The exit status is not well defined (on some machines) causing the
>	scripts to fail.
>		The exit status is now 0,1 or 2 and is documented in
>		compress.l.
>3.	The function getopt() is not available in all C libraries.
>		The function getopt() is no longer referenced by the
>		program.
>4.	Error status is not being checked on the fwrite() and fflush() calls.
>		Fixed.
>
>The following enhancements have been made:
>
>1.	Added facilities of "compact" into the compress program.  "Pack",
>	"Unpack", and "Pcat" are no longer required (no longer supplied).
>2.	Installed work around for C compiler bug with "-O".
>3.	Added a magic number header (\037\235).  Put the bits specified
>	in the file.
>4.	Added "-f" flag to force overwrite of output file.
>5.	Added "-c" flag and "zcat" program.  'ln compress zcat' after you
>	compile.
>6.	The 'uncompress' script has been deleted; simply 
>	'ln compress uncompress' after you compile and it will work.
>7.	Removed extra bit masking for machines that support unsigned
>	characters.  If your machine doesn't support unsigned characters,
>	define "NO_UCHAR" when compiling.
>
>Compile "compress.c" with "-O -o compress" flags.  Move "compress" to a
>standard executable location, such as /usr/local.  Then:
>	cd /usr/local
>	ln compress uncompress
>	ln compress zcat
>
>On machines that have a fixed stack size (such as Perkin-Elmer), set the
>stack to at least 12kb.  ("setstack compress 12" on Perkin-Elmer).
>
>Next, install the manual (compress.l).
>	cp compress.l /usr/man/manl		- or -
>	cp compress.l /usr/man/man1/compress.1
>
>Here is the README that I sent with my first posting:
>
>>Enclosed is a modified version of compress.c, along with scripts to make it
>>run identically to pack(1), unpack(1), an pcat(1).  Here is what I
>>(petsd!joe) and a colleague (petsd!peora!srd) did:
>>
>>1. Removed VAX dependencies.
>>2. Changed the struct to separate arrays; saves mucho memory.
>>3. Did comparisons in unsigned, where possible.  (Faster on Perkin-Elmer.)
>>4. Sorted the character next chain and changed the search to stop
>>prematurely.  This saves a lot on the execution time when compressing.
>>
>>This version is totally compatible with the original version.  Even though
>>lint(1) -p has no complaints about compress.c, it won't run on a 16-bit
>>machine, due to the size of the arrays.
>>
>>Here is the README file from the original author:
>> 
>>>Well, with all this discussion about file compression (for news batching
>>>in particular) going around, I decided to implement the text compression
>>>algorithm described in the June Computer magazine.  The author claimed
>>>blinding speed and good compression ratios.  It's certainly faster than
>>>compact (but, then, what wouldn't be), but it's also the same speed as
>>>pack, and gets better compression than both of them.  On 350K bytes of
>>>unix-wizards, compact took about 8 minutes of CPU, pack took about 80
>>>seconds, and compress (herein) also took 80 seconds.  But, compact and
>>>pack got about 30% compression, whereas compress got over 50%.  So, I
>>>decided I had something, and that others might be interested, too.
>>>
>>>As is probably true of compact and pack (although I haven't checked),
>>>the byte order within a word is probably relevant here, but as long as
>>>you stay on a single machine type, you should be ok.  (Can anybody
>>>elucidate on this?)  There are a couple of asm's in the code (extv and
>>>insv instructions), so anyone porting it to another machine will have to
>>>deal with this anyway (and could probably make it compatible with Vax
>>>byte order at the same time).  Anyway, I've linted the code (both with
>>>and without -p), so it should run elsewhere.  Note the longs in the
>>>code, you can take these out if you reduce BITS to <= 15.
>>>
>>>Have fun, and as always, if you make good enhancements, or bug fixes,
>>>I'd like to see them.
>>>
>>>=Spencer (thomas@utah-20, {harpo,hplabs,arizona}!utah-cs!thomas)
>>
>>					regards,
>>					joe
>>
>>--
>>Full-Name:  Joseph M. Orost
>>UUCP:       ..!{decvax,ucbvax,ihnp4}!vax135!petsd!joe
>>US Mail:    MS 313; Perkin-Elmer; 106 Apple St; Tinton Falls, NJ 07724
>>Phone:      (201) 870-5844
