ETAPE, UTILITIES, Reads EBCDIC ASCII and GCOS MagTapes


ETAPE is a general-purpose program for handling EBCDIC, ASCII, and Honeywell
GCOS BCD tapes. It allows the user to move around at will within the tape,
send output to different files, specify different blocksizes and record
lengths for different files on the tape, and combine multiple tape files
into a single output file.
Special translation can be performed on tapes which have embedded packed
decimal, floating-point, or binary fields.
The capability of writing EBCDIC or ASCII tapes from VMS ASCII files is also
included.  EBCDIC tapes can be written in FB of VB mode, and may span
several reels if necessary.

ETAPE removes all trailing blanks from tape records after reading them.
On output, records will be blank padded and blocked to create fixed length
tape blocks. A switch can be set to turn off the EBCDIC translation and
allow the translation (or writing) of ASCII tapes.



			Installation
			============

ETAPE is comprised of the following files:

	AAAREADME.TXT		This file
	ETAPE.CLD		Command language for ETAPE
	ETAPE.FOR		Source program & subroutines
	ETAPE.HLP		Help file for the program
	ETAPE.OBJ		Object from ETAPE (for re-linking)
				(Not distributed via MAILSERV)
	ETAPE_HELP_ENTRY.RNH	UNH help file (not edited to be general)
				(This file is not up to date, but if anyone
				would like to fix it up, I'd love to have a
				copy -- Dale)
	ETAPE_INCLUDE.FOR	Include file for fortran code
	SAMPLE.TRANS		A sample directive file for special translation

Note: in binary distributions (i.e. NOT Mailserv), the following files are also
included for those folks that don't have a FORTRAN compiler.

	ETAPE.OBJ_VAX
	ETAPE.EXE_VAX
	ETAPE.OBJ_AXP
	ETAPE.EXE_AXP

The following procedure is used to build ETAPE:

	$ FORT ETAPE
	(VAX) $ LINK ETAPE
	(AXP) $ LINK/NONATIVE ETAPE
	$ COPY ETAPE.EXE SYS$COMMON:[SYSEXE]ETAPE.EXE
	$ LIB/HELP/REPL/LOG SYS$HELP:HELPLIB ETAPE
	$ SET COMMAND ETAPE

			Known Errors / Limitations
			==========================

To use ETAPE, a user must possess LOG_IO and PHY_IO privilege or ETAPE must be
installed with those privileges.

I'm sure there are bugs, but I don't know what they are.  If you
find one, please let me know.

Special thanks for finding (and fixing) bugs go to:
	Bill Costa - University of New Hampshire
	John W. Miller - Indiana University of Pennsylvania
	Rollo Ross - University of South Australia
	Rick Millhollin - University of Oregon
	Don Vickers

Please direct all comments, criticisms, and especially praise to:

	Dale Miller
	University of Arkansas at Little Rock
	Data Center NS204
	2801 S. University Ave.
	Little Rock, AR 72204-1099
	(501) 569-8714
	DOMILLER@UALR.EDU
