$ sav_verify = 'f$verify(0)
$ set = "set"
$ set symbol/scope=noglobal
$ delete /symbol/local set
$!
$ set noon
$!
$! define VDE$LIBRARY for this root
$!
$
$ @vde$system:plmenu$$set_vde_library 'mnemonic'
$ if .not. $status then GOTO Error_Exit
$ root = f$trnlnm("PLMENU$CMSROOT")
$ mnemonic = f$trnlnm("PLMENU$MNEMONIC")
$!
$ if p2 .eqs. ""
$ then
$   write sys$output - 
    "%COMPSTRM-F-NOSTREAM, No stream name specified for compare"
$   GOTO Error_Exit
$ else
$   stream = p2
$ endif
$!
$ if p3 .eqs. ""
$ then
$   write sys$output - 
    "%COMPSTRM-F-NOFACS, No facilities specified for compare"
$   GOTO Error_Exit
$ else
$   facility_spec = p3
$ endif
$!
$ if p4 .eqs. "" .or.  p4 .eqs. "LOG"
$ then
$   output_qual = ""
$ else
$   output_qual = "/OUTPUT=''p4'"
$ endif
$!
$ if p5 .eqs. ""
$ then
$   set default sys$login
$ else
$   set default 'p5'
$ endif
$!
$! define the compare Vde to CMS command
$!
$  set command VDE$SYSTEM:PLMENU$$faccomp
$!
$ faccomp/log'output_qual' 'facility_spec' 'stream' 
$!
$Main_Exit:
$ EXIT 1+0*'f$verify(sav_verify)
$!
$Error_Exit:
$ EXIT 0+0*'f$verify(sav_verify)
$!
$!+-----------------------------------------------------------------------------
$!
$! COMPSTRM.COM
$!
$! Description:
$!
$! This procedure runs the compare_vde2cms image which produces a report of
$! differences between the VDE and CMS representations of a stream.
$!
$! INPUTS:
$!
$!  P1-> Product mnemonic
$!	    The mnemonic for the VDE library to operate on; this is used
$!          to derive the CMS root on which this procedure operates on.
$!
$!  P2-> Stream name
$!	    The stream for which to compare VDE and CMS.
$!
$!  P3-> Facility(ies) specification
$!	    A file specification of the format 'root':['p3'.CMS].  P3 may
$!	    contain wildcard characters.
$!
$!  P4-> Output file
$!	    A file name for the differences report.  If the file name is
$!	    "LOG" the differences are listed in the logfile.
$!
$!  P5-> Work directory
$!	    Default directory for report file.
$!
$!  AUTHOR:
$!	David Sweeney	20-MAR-1992
$!
$!  MODIFICATION HISTORY
$!
$!	DJS	X-1	Original Version - process non-archived facs only
$!
$!+-----------------------------------------------------------------------------
