$! DIRDIFF.COM							V1.3
$!
$! Function: To create a patch set to between the contents of two directories.
$!
$! Created: 27-Apr-1991  Author: Mark Pizzolato (mark@infocomm.com)
$!
$! Inputs:
$!	P1 = the newer directory
$!	P2 = the base directory
$!	P3 = Optional message to include in the output
$!	P4 = File Type(s) to process (Defaults to ".C,.H,.COM,.")
$!	P5 = Boolean, which when true, causes patches to use SUMSLP edit format
$!	     and when false, causes the use of Larry Wall's diff and patch
$!	     facilities. (Defaults to FALSE)
$!	P6 = Boolean, which when true (and P5 is false), specifies that
$!	     "context" differences will be generated in the patch set, and 
$!	     when false, causes a "normal" diff to be used (Defaults to TRUE)
$! Output:
$!	The set of commands that will "patch" the specified directory is
$!	written to SYS$OUTPUT.  This can be directed to a file by the 
$!	following means:
$!		$ @DIRDIFF/OUTPUT=patchfile.com [newdir] [olddir] 
$!
$! Note:
$!	When generating and/or processing diffs that use Larry Wall's diff and
$!	patch facilities, these facilities are referenced through the symbols
$!	CDIFF and CPATCH.  If these symbols are undefined, they default to:
$!		CDIFF = "$UUCP_BIN:DIFF"
$!		CPATCH = "$UUCP_BIN:PATCH"
$!
$! Modified:
$!	V1.1 14-Feb-1992	Mark Pizzolato (mark@infocomm.com)
$!		- Implemented optional SUMSLP vs. Larry Wall's diff/patch
$! 	V1.2 25-Feb-1992	Mark Pizzolato (mark@infocomm.com)
$!		- Optional Context diff selected by P6.
$! 	V1.3 26-Feb-1992	Mark Pizzolato (mark@infocomm.com)
$!		- Fixed default sense of the P6 (context diff) option
$!		
$!---
$	set noon
$	say = "write sys$output"
$	SAME = 0
$	NEW = 0
$	DELETED = 0
$	CHANGED = 0
$	SUMSLP = .not.(.not.P5)
$	if .not.SUMSLP
$	    then
$		if "''cdiff'".eqs."" then cdiff = "$UUCP_BIN:DIFF"
$		if "''cpatch'".eqs."" then cpatch = "$UUCP_BIN:PATCH"
$		cdiff = (cdiff - "-c" - "-C")
$		if P6.eqs."" then P6 = "TRUE"
$		if P6 then cdiff = cdiff + " -c"
$	endif
$	if P1.nes."" then goto GOTP1
$GETP1:	read/end=EXIT/Error=EXIT SYS$INPUT/Prompt="First Directory: " P1
$	if P1.eqs."" then goto GETP1
$GOTP1:	if P2.nes."" then goto GOTP2
$GETP2:	read/end=EXIT/Error=EXIT SYS$INPUT/Prompt="First Directory: " P2
$	if P2.eqs."" then goto GETP2
$GOTP2:	TYPELIST = P4
$	if TYPELIST.eqs."" then TYPELIST = ".C,.H,.COM,."
$	D1 = (f$parse(P1)-".;")
$	D2 = (f$parse(P2)-".;")
$	say "$! Directory Difference:			''f$time()'"
$	say "$!"
$	if P3.nes.""
$	    then
$		say "$!	''P3'"
$		say "$!"
$	endif
$	say "$! This procedure, if executed in:"
$	say "$!		''D2'
$	say "$! will make it's files identical to those in:"
$	say "$!		''D1'
$	say "$!"
$	say "$! processing will include all files with types: ''TYPELIST'"
$	say "$!"
$	if .not.SUMSLP
$	    then
$		say "$!  This procedure presumes the symbol CPATCH is defined 
$		say "$!  to invoke Larry Wall's patch facility as a foreign
$		say "$!  command."
$		say "$!"
$		say "$ if ""","'","'","CPATCH'""",".eqs.""",""" then cpatch := $UUCP_BIN:PATCH"
$	endif
$	EXT = -1
$NEXTTY:EXT = EXT + 1
$TYPES:	TYP = f$element(EXT,",",TYPELIST)
$	if TYP.eqs."," then goto EXIT
$NEXT2:	F1 = f$search("''D1'*''TYP';",1)
$	F2 = f$search("''D2'*''TYP';",2)
$CHECK:	if (F1.eqs."") then goto DONE1
$	if (F2.eqs."") then goto DONE2
$	NAME1 = f$parse(F1,,,"NAME")+f$parse(F1,,,"TYPE")
$	NAME2 = f$parse(F2,,,"NAME")+f$parse(F2,,,"TYPE")
$	if NAME1.lts.NAME2
$	    then
$		if (".DIR;1".nes.f$parse(F1,,,"TYPE")+f$parse(F1,,,"VERSION"))
$		    then
$			say "$ CREATE ''NAME1'"
$			say "$ DECK/DOLLARS=$END-OF-FILE$"
$			type 'F1'
$			say "$END-OF-FILE$"
$			NEW = NEW + 1
$		endif
$		F1 = f$search("''D1'*''TYP';",1)
$		goto CHECK
$	endif
$	if NAME1.gts.NAME2
$	    then
$		if (".DIR;1".nes.f$parse(F2,,,"TYPE")+f$parse(F2,,,"VERSION"))
$		    then
$			say "$ delete/log ''NAME2';*"
$			DELETED = DELETED + 1
$		endif
$		F2 = f$search("''D2'*''TYP';",2)
$		goto CHECK
$	endif
$	if (".DIR;1".eqs.f$parse(F2,,,"TYPE")+f$parse(F2,,,"VERSION"))
$	    then
$		goto NEXT2
$	endif
$	define/user sys$output nla0:
$	define/user sys$error nla0:
$	diff 'D1''NAME1' 'D2''NAME2'
$	if $severity .eqs. "1" 
$	    then
$		SAME = SAME + 1
$		goto NEXT2
$	endif
$	if SUMSLP
$	    then
$		NAME1_UPD = NAME1 - f$parse(NAME1,,,"TYPE")+".SLP_UDATE_FILE"
$		say "$ create ''NAME1_UPD'"
$	    else
$		say "$ cpatch ''NAME1'"
$	endif
$	say "$ DECK/DOLLARS=$END-OF-FILE$"
$	if SUMSLP
$	    then
$		diff/slp/output=sys$output 'D2''NAME2' 'D1''NAME1'
$	    else
$		cdiff 'D2''NAME2' 'D1''NAME1'
$	endif
$	say "$END-OF-FILE$"
$	if SUMSLP
$	    then
$		say "$ edit/sum ''NAME1'/update=''NAME1_UPD'"
$		say "$ delete ''NAME1_UPD';"
$	endif
$	CHANGED = CHANGED + 1
$	goto NEXT2
$DONE1: if F2.eqs."" then goto NEXTTY
$	NAME2 = f$parse(F2,,,"NAME")+f$parse(F2,,,"TYPE")
$	if (".DIR;1".nes.f$parse(F2,,,"TYPE")+f$parse(F2,,,"VERSION"))
$	    then
$		say "$ delete/log ''NAME2';*"
$		DELETED = DELETED + 1
$	endif
$	F2 = f$search("''D2'*''TYP';",2)
$	goto DONE1
$DONE2:	if F1.eqs."" then goto NEXTTY
$	NAME1 = f$parse(F1,,,"NAME")+f$parse(F1,,,"TYPE")
$	if (".DIR;1".nes.f$parse(F1,,,"TYPE")+f$parse(F1,,,"VERSION"))
$	    then
$		say "$ CREATE ''NAME1'"
$		say "$ DECK/DOLLARS=$END-OF-FILE$"
$		type 'F1'
$		say "$END-OF-FILE$"
$		NEW = NEW + 1
$	endif
$	F1 = f$search("''D1'*''TYP';",1)
$	goto DONE2
$EXIT:	say "$! Directory difference complete:"
$	say f$fao("$!!!_!UL file!%S the same", SAME)
$	say f$fao("$!!!_!UL file!%S patched", CHANGED)
$	say f$fao("$!!!_!UL file!%S created", NEW)
$	say f$fao("$!!!_!UL file!%S deleted", DELETED)
