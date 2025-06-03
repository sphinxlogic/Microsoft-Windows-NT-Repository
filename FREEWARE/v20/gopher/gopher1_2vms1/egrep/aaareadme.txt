GNU grep v1.6 (June 11, 1992)

This version was taken from prep.ai.mit.edu and modified to run under
VMS.  It compiles under VMS using GNU C or VAX C.  It was tested with
GNU C v1.39.1 and VAX C v3.2 under VMS v5.5.

Thanks to Henrik Tougaard, Datani A/S, Copenhagen, Denmark <ht@datani.dk>
for the VAX C changes.

As supplied, VMSMAKE.COM also compiles SHELL_MUNG.C, which will allow grep to
handle wildcarded file specs.

To install:

	$ @VMSMAKE.COM			!or just $ MMS
	$ GREP :== $dev:[dir]GREP.EXE
	$ EGREP :== $dev:[dir]EGREP.EXE
	$ SET DEFAULT [.TESTS]
	$ @VMSREGRESS.COM		!To test it out
	$ LIBRARY/HELP/INSERT library GREP

New to this distribution:

	VMSMAKE.COM			DCL command procedure to build grep
	DESCRIP.MMS			MMS file to build grep
	GREP.CLD			Command definition file for e?grep
	GREP.EXAMPLES			grep commands equiv. to SEARCH commands
	GREP.RNH			RUNOFF source for on-line VMS help
	SHELL_MUNG.C			Emulate UNIX shell
	GREP.C				Modified for VMS
	DFA.C				Modified for VMS
	DFA.H				Modified for VMS
	REGEX.C				Modified for VMS
	[.TESTS]VMSSCRIPTGEN.AWK	GNU awk program to create testing .COM
	[.TESTS]VMSREGRESS.COM		DCL command procedure to test egrep

This version of grep also includes a few VMS-specific (bracketed by #ifdef VMS)
changes.  Specifically:

    1.  Added call to shell_mung
    2.  References to sys_errlist[errno] were causing grep to stack dump.
        I modified it to call strerror instead.
    3.  The exit statuses were changed to better fit VMS:
            Meaning                  UNIX       VMS
            -------                  ----       ---
            matches found               0         1
            no matches found            1         3
            error                       2        44 (SS$_ABORT)
    4.  Modified print_line to use puts when printing lines instead of
        putchar.  Much faster output on VMS.

I also modified SHELL_MUNG.C to allow it to be compiled by GNU C.

Any suggestions/comments/etc. can be sent to the address below.

Hunter Goatley, VAX Systems Programmer      E-mail: goathunter@WKUVX1.BITNET
Academic Computing, STH 226                 Voice:  (502) 745-5251
Western Kentucky University
Bowling Green, KY 42101
