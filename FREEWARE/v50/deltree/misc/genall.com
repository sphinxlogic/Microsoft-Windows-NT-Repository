$ echo := write sys$output
$ if f$getsyi("NODE_HWTYPE").EQS."ALPH"
$ then	ARCHI = "AXP"
$ else	ARCHI = "VAX"
$ endif
$
$ src  = "SYS$COMMON:[SYSUPD]DELETE.CLD"
$ trgt = "DELETE_''ARCHI'.CLD"
$
$ if f$search(src).eqs.""
$ then	echo "This script requires ''src' to be present"
$ 	echo "If you have it installed, you may use the VERB utility to extract"
$ 	echo "the DELETE CLI to ''trgt' and try again."
$ endif
$
$ if f$search(trgt).eqs.""
$ then	ON ERROR THEN GOTO KABOUM
$ 	COPY 'src []'trgt
$ endif
$
$ EDIT/TPU/NODISPLAY /COMMAND=[]GENCLD.TPU []'trgt'
$
$ if f$search("DELETE.CLD").eqs.""
$ then	echo "Failed to merge DELTREE to the DELETE command"
$ 	exit
$ else
$ 	DELETE/NOLOG/NOCONFIRM 'trgt';*
$ 	library/extract=delete/out=delete.hlp sys$help:HELPLIB.HLB
$ 	OPEN/APPEND filout DELETE.HLP
$ 	WRITE FILOUT "2 /TREE"
$ 	WRITE FILOUT " See DELTREE command for more information on allowed qualifiers"
$ 	WRITE FILOUT ""
$ 	CLOSE FILOUT
$ 	COPY DELETE.HLP,[-]DELTREE.HLP []DELETE.HLP
$	PURGE/NOLOG DELETE.HLP
$
$	type sys$input:

 -----------------------------------------------------------------------
 DELETE.CLD successfully generated.
 It assumes that DELTREE.EXE will be placed in VMSAPPS: If this does not
 reflect your local installation, edit the CLD file and replace VMSAPPS
 by the proper site specific location. In case your plan is to install
 the excutable in SYS$COMMON:[SYSEXE], just remove the VMSAPPS path.

 Now, you will need to issue the following command
    $ SET COMMAND DELETE.CLD /REPLACE -
	/TABLE=SYS$COMMON:[SYSLIB]DCLTABLES.EXE -
	/OUTPUT=SYS$COMMON:[SYSLIB]DCLTABLES.EXE

 In addition, the following needs to be excuted on ALL node within your
 cluster.
    $ INSTALL REPLACE SYS$COMMON:[SYSLIB]DCLTABLES.EXE /HEADER/OPEN/SHARE

 A new DELETE.HLP has also been created. You can
    $ LIBRARY/REPLACE SYS$COMMON:[SYSHLP]HELPLIB.HLB DELETE.HLP

 -----------------------------------------------------------------------

$ 	INQUIRE ANS "Would you like to proceed "
$ 	if ANS
$ 	then	! Install as specified above
$ 		SET COMMAND DELETE.CLD /REPLACE  -
		 /TABLE=SYS$COMMON:[SYSLIB]DCLTABLES.EXE -
		 /OUTPUT=SYS$COMMON:[SYSLIB]DCLTABLES.EXE
$		INSTALL REPLACE SYS$COMMON:[SYSLIB]DCLTABLES.EXE /HEADER/OPEN/SHARE
$ 		LIBRARY/REPLACE SYS$COMMON:[SYSHLP]HELPLIB.HLB DELETE.HLP
$ 		echo "All done. Do not forget to INSTALL REPLACE on satellite nodes."
$ 	else	! Else
$ 		echo "Bye"
$ 	endif
$
$ endif
$exit

$KABOUM:
$ echo "You do not have sufficient privileges to copy DELETE.CLD from SYS$UPDATE:"
$ echo "You can use VERB DELETE /OUTPUT=''trgt' if VERB is installed on your"
$ echo "system and start again. Local installation only will be possible."
$exit
