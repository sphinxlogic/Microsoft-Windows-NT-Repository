			      02README.TXT
			      [01-Sep-88]

This is an interim release of version 2.10 of the TeX DVI driver family,
incorporating a few bug fixes since the 17-Nov-87 release.  For a list
of files changed since that date, see 00tdir.lst.  Newletters 16 and 17
(in the files 00mail.*) describe the changes.

Two additional modifications especially for the IBM PC environment
have been made especially for this interim release; they will likely
be incorporated in release 2.11 currently in preparation.  The first
is to reduce the size of the bitmaps in dvieps.c and dvitos.c to
allow them to run with the limited memory on the PC; this reduction
is automatic when OS_PCDOS is defined.  The second change is that the
stack sizes of dvieps.exe and dvitos.exe need modification; by
empirical experiment, I found that

	exemod dvieps.exe /stack B000

seems to work for simple test files; the default setting of 8000 in the
Makefile results in a run-time stack overflow termination.  This change
has been applied to the Makefiles as well.

