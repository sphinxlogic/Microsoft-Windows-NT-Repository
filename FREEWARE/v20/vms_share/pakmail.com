$! Purpose:
$!	To mail out a series of files produced by VMS_SHARE to a series
$!	of recipients.
$!
$! Parameters:
$!
$!	P1 = name (or list ) of recipient(s). Can be logical or distribution
$!           list.
$!
$!		If sending to a distribution list, it is necessary to define
$!		a logical pointing to the file and then give that logical as
$!		the recipient name.  This avoids problems with DCL's handling
$!		of quotes and '@' symbols.
$!
$!
$!	P2 = file spec of package - "[directory]file" without VMS_SHARE suffix!
$!
$!
$!	P3 = no of parts of the package
$!
$! 		This procedure adds the VMS_SHARE suffix and sends files
$!		called  "[directory]file''nn'-OF-''P3'"   nn from 1-> 'P3'
$!
$!	P4 = Optional list of parts to be sent (default: all parts)
$!
$!	P5 = Comment to add to mail subject line
$!
$!
$! Privileges:
$!	Whatever is necessary to access e-mail and the relevant network.
$!
$!
$! Environment:
$!	Nothing special.  VMS_SHARE must exist of course in order to
$!	produce the software package.
$!
$! Revision History:
$!	1.0	Andy Harper	6-DEC-1988	Original version
$!	1.1	Andy Harper	19-DEC-1988	Allow P4 to be mail subject
$!	1.2	Andy Harper	5-JAN-1989	Return exit status
$!	1.3	Andy Harper	16-JUN-1989	Remove "OF_''mm'" extension
$!	1.4	Andy Harper	3-AUG-1989	use READ, check EOF status
$!	1.5	Andy Harper	3-SEP-1992	Update part format
$!	2.0	Andy Harper	8-JAN-1993	Recode for V5; add part list
$!	2.1	Andy harper	18-FEB-1993	Add missing 'exit' label
$!
$!
$
$
$! SET UP STANDARD EXIT CODES
$ ss$_normal= 1
$ ss$_abort = 44
$
$! SET UP USEFUL SYMBOLS
$ em="write sys$error ""%" +f$parse(f$environment("PROCEDURE"),,,"NAME")+""","
$
$
$! MAKE SURE WE HAVE ALL THE REQUIRED PARAMETERS
$
$! P1: E-mail address of recipient(s)
$get_recipient:
$ P1 = f$edit(P1,"TRIM,UPCASE")
$ if P1 .eqs. ""
$   then
$      read/prompt="_mail address of recipient "/end=exit sys$command P1
$      goto get_recipient
$ endif
$
$
$! P2: Name of package to be sent
$package_location:
$ P2 = f$edit(P2,"TRIM,UPCASE")
$ if P2 .eqs. ""
$   then
$     read/prompt="_Enter package directory and base filename "/end=exit sys$command P2
$     goto package_location
$ endif
$ P2 = f$parse(P2,,,,"SYNTAX_ONLY") - f$parse(P2,,,"VERSION")
$ if f$parse(P2,,,"TYPE") .nes. "." then $ P2 = P2 + "_"
$
$
$! P3:  Total number of parts in the package
$get_parts:
$ P3 = f$edit(P3,"TRIM")
$ if P3 .eqs. ""
$   then
$     read/prompt="_Enter number of parts "/end=exit sys$command P3
$     goto get_parts
$ endif
$
$ if f$type(P3) .nes. "INTEGER"
$   then
$     em "-E-NONNUMTOT, Non-numeric total part count - ", P3
$     exit ss$_abort
$ endif
$
$
$! P4: (Optional) list of parts to be sent, defaults to all parts
$! P5: (Optional) comment to add to mail subject line
$
$
$! SET UP USEFUL SYMBOLS
$ package = f$parse(P2,,,"NAME")
$ l = f$length(P3)
$
$
$! IS THERE A SET OF FILES THAT MATCHES THE SPECIFICATION??
$ x = f$fao("!AS*-OF-!ZL",P2,f$integer(P3))
$ if f$search("''x'") .eqs. ""
$   then
$     em "-E-PACKAGE, Cannot find any parts for ", Package
$     exit ss$_abort
$ endif
$
$
$! DO A DUMMY RUN TO MAKE SURE ALL REQUIRED FILES ARE THERE!!!!!
$ Really_Post_It = "FALSE"
$ gosub PostParts
$
$! WERE ALL THE PARTS FOUND? IF NOT, NOW'S THE TIME TO FIND OUT!
$ if .not. OK
$   then
$     em "-E-PARTERRS, Error with one or more parts. Abandoning send"
$     exit ss$_abort
$ endif
$
$! NOW DO A REAL RUN AND POST THE PARTS
$ Really_Post_It = "TRUE"
$ gosub PostParts
$exit:
$ exit
$
$
$! -------------------------------------------------------------------------
$! Routine to post all the required parts; Really_Post_It must be "TRUE"
$! -------------------------------------------------------------------------
$PostParts:
$ n = 0
$ OK = "TRUE"
$NextPart:
$ n = n+1
$ part =n
$
$! If a part list was specified, use these; otherwise do all parts
$ if P4 .nes. ""
$   then
$
$ ! Extract the next required part number
$     part = f$element(n-1,",",P4)
$     if part .eqs. "," then $ return	! no more parts, give up here
$
$ ! Make sure they've not given us a dodo ....
$     if f$type(part) .nes. "INTEGER"
$       then
$         em "-E-NONNUMPRT, Non-numeric part specified - ", part
$         OK = "FALSE"
$     endif
$
$ ! Make sure this part is within the range for this package
$     if part .gt. P3
$       then
$         em "-E-BIGPRTNUM, Part number greater than maximum - ", part
$         OK = "FALSE"
$     endif
$     part = f$integer(part)
$ endif
$
$! OK, go action this part
$ if part .le. P3
$   then
$     part = f$fao("!#ZL",l,part)
$     file = P2 + part + "-OF-" + P3
$     if Really_Post_It
$       then
$         em "-I-SENDPART, Sending part ", part, " of ", package
$         subj = f$parse(file,,,"NAME") + f$parse(file,,,"TYPE") + "  " + P5
$         mail/noedit/noself/subject=&subj 'file' 'P1'
$         if .not. $status then $ return ! Return status from mail if it failed
$       else
$         if f$search(file) .eqs. ""
$           then
$             em "-E-NOPART, Part ''part' of ''package' is missing!"
$             OK = "FALSE"
$         endif
$     endif
$     goto NextPart
$ endif
$ return
