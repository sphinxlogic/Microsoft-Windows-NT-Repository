$ verify = 'f$verify(0)'
$!------------------------------------------------------------------------------
$! REX.COM  Count records in file(s).  Can use wildcards & lists in filespec.
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!------------------------------------------------------------------------------
$
$ ask := read SYS$COMMAND /error=END /end=END /prompt=
$
$START:
$ if p1 .eqs. ""  then  ask "File(s)? " p1
$ if p1 .eqs. ""  then  goto  START
$
$ copy /log 'p1' NL:
$
$END:
$ EXIT 1 + 0 * f$verify (verify)
