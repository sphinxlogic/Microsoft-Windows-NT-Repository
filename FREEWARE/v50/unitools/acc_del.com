$ verify = 'f$verify(0)'
$!------------------------------------------------------------------------------
$!  ACC_DEL.COM
$!
$!  Split off OpenVMS accounting file records.
$!
$!  Parameters: [P1] = Accounting filename
$!		[P2] = Accounting qualifiers
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	12-Oct-1994	JER	Original version.
$!	18-Sep-1995	JER	Tiny mods to accounting verification.
$!	17-Apr-1996	JER	Eliminate version number conflicts.
$!	17-Dec-1997	JER	Allow P1 and P2 on command line.
$!------------------------------------------------------------------------------
$
$ on warning    then  goto  ESCAPE
$ on control_y  then  goto  ESCAPE
$
$ say := write SYS$OUTPUT
$ ask := read SYS$COMMAND /error=ESCAPE /end=ESCAPE /prompt=
$ Pid  = f$getjpi ("","PID")
$ out_fyl = f$parse ("ACC_DEL_''PID'.TMP_EXTRACT") - ";"
$ rej_fyl = f$parse ("ACC_DEL_''PID'.TMP_SAVE") - ";"
$
$GET_P1:
$ p1 = f$edit   (p1,"UPCASE,COMPRESS")
$ if p1 .eqs. ""
$ then
$   ask "Accounting filename? " p1
$   goto  GET_P1
$ endif
$ p1 = f$element (0,";",f$search (p1))
$ if p1 .eqs. ""
$ then
$   say "%ERROR, File does not exist!"
$   goto  GET_P1
$ endif
$
$GET_P2:
$ p2 = f$edit (p2,"UPCASE,COMPRESS")
$ if p2 .eqs. ""
$ then
$   ask "Accounting qualifiers? " p2
$   goto  GET_P2
$ endif
$
$GET_ANS0:
$ ask "Validate this selection? (Yes/No) " ans
$ ans = f$edit (ans,"UPCASE,COMPRESS")
$ if ans .eqs. ""  then  goto  GET_ANS0
$ if .not. ans  then  goto  GET_ANS1
$
$ say "%Running accounting to validate selection ..."
$ define /user sys$input sys$command
$ account /full 'p1' 'p2'
$ say "%Accounting run completed."
$
$GET_ANS1:
$ ask "Use this selection? (Yes/No) " ans
$ ans = f$edit (ans,"UPCASE,COMPRESS")
$ if ans .eqs. ""  then  goto  GET_ANS1
$ if .not. ans  then  goto  get_p1
$
$ say "%Running accounting to extract selection ..."
$ account /binary /reject='rej_fyl' /output='out_fyl' 'p1' 'p2'
$ say "%Accounting run completed."
$ say ""
$ say "%Retained  records are in file ''rej_fyl'"
$ say "%Extracted records are in file ''out_fyl'"
$ say ""
$
$GET_ANS2:
$ ask "Copy ''rej_fyl' to ''p1'? (Yes/No) " ans
$ ans = f$edit (ans,"UPCASE,COMPRESS")
$ if ans .eqs. ""  then  goto  GET_ANS2
$ if ans  then  copy /log 'rej_fyl' 'p1'
$
$GET_ANS3:
$ ask "Delete temporary files? (Yes/No) " ans
$ ans = f$edit (ans,"UPCASE,COMPRESS")
$ if ans .eqs. ""  then  goto  GET_ANS3
$ if ans  then  delete /log 'rej_fyl';*,'out_fyl';*
$
$GET_ANS4:
$ ask "Purge accounting file? (Yes/No) " ans
$ ans = f$edit (ans,"UPCASE,COMPRESS")
$ if ans .eqs. ""  then  goto  GET_ANS4
$ if ans  then  purge /log 'p1'
$
$ESCAPE:
$EXIT_DOOR:
$ EXIT 1 + 0 * f$verify (verify)
