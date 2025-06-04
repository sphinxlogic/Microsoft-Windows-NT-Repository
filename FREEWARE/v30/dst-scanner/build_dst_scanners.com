$! BUILD_DST_SCANNERS.COM
$!
$ DEFINE DSTDIR 'F$ENV("DEFAULT")'
$ WRITE := WRITE
$!
$ SAY :== WRITE SYS$OUTPUT
$ CPU = F$EXTRACT(0,3,F$GETSYI("HW_NAME"))
$!
$!
$! I hope it can ONLY ever be VAX or DEC! :-(
$ IF (CPU .EQS. "VAX")   
$ THEN  ! Do VAX setup
$ ENDIF
$ IF (CPU .EQS. "DEC")
$ THEN  ! Do AXP setup
$ ENDIF
$ TYPE/PAGE SYS$INPUT
$ DECK

This procedure builds the DST Scanner Utility.  The utility consists of 
three separate executable images, as follows:


    Image		    Understands		Displays
-----------------------	    -----------		-----------------------------
VAX_DST_SCANNER		    VAX images		source filespecs only

VAX_DST_SCANNER_DETAILS	    VAX images		source filespecs and RMS info

AXP_DST_SCANNER		    AXP images		source filespecs only


The DST Scanner Utility package stops short of full parallelism between VAX 
and AXP architectures, in that it does NOT provide an image which "understands 
AXP images" _and_ includes RMS information in its output.  There is no tech-
nical obstacle to creating the "missing" image, but it never got done, for 
"historical reasons."

$ EOD
$!+
$!  Verify presence of all necessary files
$!-
$ FILE_LIST = "VAX_DST_SCANNER.MAR,VAX_DST_SCANNER_DETAILS.MAR," + -
    "AXP_DST_SCANNER.MAR,$EIHDDEF.MAR,$EIHSDEF.MAR,ALLOSTK.MAR," + -
    "CHKSTS.MAR,DESC.MAR,DSTDEF.MAR,DSTDEF_AXP.MAR,VAX_DST.CLD," + -
    "AXP_DST.CLD"
$ FILE_IDX = 0
$ MISSING_COUNT = 0
$ STAT = 1
$ FILE_LOOP:
$   FILE_SPEC = F$ELEMENT(FILE_IDX,",",FILE_LIST)
$   IF (FILE_SPEC .EQS. ",") THEN $ GOTO FILE_DONE
$   IF F$SEARCH("[]''FILE_SPEC'") .EQS. "" THEN $ GOSUB MISSING_FILE
$   FILE_IDX = FILE_IDX + 1
$   GOTO FILE_LOOP
$!
$ MISSING_FILE:
$   SAY "Missing file: ""''FILE_SPEC'"" 
$   STAT = 0
$   MISSING_COUNT = MISSING+1
$   RETURN
$!
$ FILE_DONE:
$   IF (.NOT. STAT)
$   THEN
$     IF (MISSING_COUNT .EQ. 1)
$     THEN
$       SAY "There was a missing file -- unable to build DST Scanner Utility!"
$     ELSE
$       SAY "There were ''MISSING_COUNT' missing files -- unable to build ", -
            "DST Scanner Utility!"
$     ENDIF
$     EXIT
$   ELSE
$     SAY "All necessary files appear to be present."
$   ENDIF
$   SAY ""
$!+
$! Build macro library -- if on Alpha, allow user to choose whether to use
$! the _supplied_ or _system_ copies/versions of $EIHDDEF and $EIHSDEF; 
$! on VAX just plow through with _supplied_.
$!
$ USE_SUPPLIED = "YES"   ! Default for VAX environment
$ IF (CPU .EQS. "DEC")
$ THEN
$   TYPE/PAGE SYS$INPUT
$   DECK

You are building this package on an OpenVMS AXP system, where all necessary 
assembly-time symbol definitions _should_ be available as part of the VMS 
environment.  However, since OpenVMS AXP symbols would NOT be available IF 
you were building this package on a VAX/VMS system, the package itself 
provides its OWN definitions for those symbols.  Thus, you may now use 
either YOUR VMS SYSTEM's definitions, OR the ones supplied WITH THIS 
PACKAGE.

It shouldn't really matter WHICH set of definitions you use, but if you 
find that the DST/AXP command does not seem to operate properly, try 
re-running this build procedure and answering the following question "the 
other way" -- i.e., differently than you did the FIRST time.

$   EOD
$   CH_LOOP_1:
$     INQUIRE/NOPUNC CH "Use (S)ystem or (P)ackage symbol-definitions? "
$     CH = F$EDIT(CH,"TRIM,UPCASE,COMPRESS")
$     IF (CH .NES. "S") .AND. (CH .NES. "P") 
$     THEN 
$       SAY "You must respond with either the letter ""P"" or the letter ""S""."
$       SAY "Please try again."
$       GOTO CH_LOOP_1
$     ENDIF
$   IF (CH .EQS. "S") THEN $ USE_SUPPLIED = "NO"
$   SAY "Thank you..."
$   SAY ""
$ ENDIF
$!
$ IF USE_SUPPLIED
$ THEN
$   SAY "Building DST Scanner package, using package-supplied definitions"
$ ELSE
$   SAY "Building DST Scanner package, using VMS environment definitions"
$ ENDIF
$ SAY "of OpenVMS Alpha image-header symbols ($EIHDDEF and $EIHSDEF)."
$!+
$! Build macro library...
$!-
$ LIBRARY/CREATE MYDSTLIB/MACRO ALLOSTK.MAR,CHKSTS.MAR,DESC.MAR, -
     DSTDEF.MAR,DSTDEF_AXP.MAR
$ IF (USE_SUPPLIED) THEN $ LIBRARY/INSERT MYDSTLIB/MACRO -
     $EIHDDEF.MAR, $EIHSDEF.MAR
$!+
$! Build executables 
$!-
$ MACRO_QUAL = ""
$ IF (CPU .EQS. "DEC") 
$ THEN 
$    MACRO_QUAL = "/MIGRAT/NODEBUG"
$    DEFINE SYS$ERROR _NLA0:
$    DEFINE SYS$OUTPUT _NLA0:
$ ENDIF
$ MACRO'MACRO_QUAL' VAX_DST_SCANNER
$ MACRO'MACRO_QUAL' VAX_DST_SCANNER_DETAILS
$ MACRO'MACRO_QUAL' AXP_DST_SCANNER
$ IF (CPU .EQS. "DEC") 
$ THEN 
$   DEASSIGN SYS$ERROR 
$   DEASSIGN SYS$OUTPUT
$ ENDIF
$!
$ LINK VAX_DST_SCANNER
$ LINK VAX_DST_SCANNER_DETAILS
$ LINK AXP_DST_SCANNER
$!
$ TYPE/PAGE _NLA0:
$ SAY "DST Scanner Utility built successfully!"
$ SAY ""
$ SAY "Setting up DST command, for the current process ONLY!"
$ SAY ""
$ SAY "Read the supplied AAAREADME.TXT file for full DST Scanner documentation."
$!
$ IF (CPU .EQS. "VAX")
$ THEN 
$   SET COMMAND VAX_DST.CLD
$ ELSE
$   SET COMMAND AXP_DST.CLD
$ ENDIF
$ EXIT
