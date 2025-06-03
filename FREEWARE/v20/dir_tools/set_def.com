$!-----
$!
$!  Programmer:     Brian Vande Merwe
$!		    (801)226-3355
$!
$!  Group:          Development
$!
$!  Description:    Set default and change prompt.
$!
$! Valid qualifiers:
$!	/(L)ist		! To list the saved directories
$!	/(S)et=<num>	! To set the number of saved directories
$!	/(T)op		! Set def to the TOP of the tree.
$!
$!  Examples:
$!	    $ sdf sys$system:
$!		Will change your default to sys$system: and change your prompt
$!		to reflect the changes.
$!
$!	    $ sdf/set=20
$!		Will set the maximum number of directories saved to 20.
$!
$!	    $ sdf/list
$!		Will list all the saved directories.
$!
$!-----
$!'F$verify(0)'
$   debug = "Neg-a-tory"
$   If p2 .Eqs. "DEBUG" Then debug = "Yup"
$
$   Gosub PARSER
$
$   !
$   ! Check for the TOP
$   !
$   if f$locate("/T", qualifiers) .nes. f$length(qualifiers)
$   then
$	cur_dir = f$environment("default")
$	top_level = f$element(0, ".", cur_dir) - "]" + "]"
$	p1 = top_level
$   endif
$
$   !
$   ! Get the number of symbols to save.
$   !
$   If F$locate("/S", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	start_inter = F$locate("/S", qualifiers)
$	inter_qual = F$element(1, "/", -
	    F$extract(start_inter, F$length(qualifiers), qualifiers))
$	total = F$element(1, "=", inter_qual)
$	last_total :== 'total'
$	Exit
$   Endif
$
$   !
$   ! List the saved symbols.
$   !
$   If F$locate("/L", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	Call SHOW_LAST
$	exit
$   Endif
$
$   If p1 .Eqs. "" Then Goto OOPS
$
$CHANGE_CURLY_LEFT:
$   If F$locate("{", p1) .Nes. F$length(p1)
$   Then
$	curly = F$locate("{", p1)
$	temp1 = F$extract(0, curly, p1)
$	temp2 = F$extract(curly + 1, F$length(p1), p1)
$	p1 = temp1 + "[" + temp2
$	Goto CHANGE_CURLY_LEFT
$   Endif
$
$CHANGE_CURLY_RIGHT:
$   If F$locate("}", p1) .Nes. F$length(p1)
$   Then
$	curly = F$locate("}", p1)
$	temp1 = F$extract(0, curly, p1)
$	temp2 = F$extract(curly + 1, F$length(p1), p1)
$	p1 = temp1 + "]" + temp2
$	Goto CHANGE_CURLY_RIGHT
$   Endif
$
$   If F$parse(p1) .Eqs. "" Then Gosub DIR_NOT_THERE
$
$   !
$   ! Move the "last" list around.
$   !
$   Call MOVE_LAST
$   last :== 'sdf' ' F$environment("default")'
$   Set default 'p1'
$
$   current = F$environment("default") - "000000."	! Current directory
$   nodename = F$getsyi("nodename")			! Current NODE Name
$   If nodename .Eqs. "" Then nodename = "?"
$
$   Set default 'current'				! Make sure we have a
$							! directory without the
$							! "000000."
$
$   node = "0"
$
$   node = F$extract(0, 3, nodename)
$
$   device = F$trnlnm(F$parse(current,,, "device", "syntax_only") - ":")
$   If device .Eqs. "" Then -
	device = F$parse(current,,, "device", "syntax_only")
$
$   disk = "D" + F$string(F$getdvi(device, "unit"))
$   list = F$parse(current,,, "directory", "syntax_only") - "[" - "]"
$
$   If debug
$   Then
$	Write Sys$output "current     = ''current'"
$	Write Sys$output "node        = ''node'"
$	Write Sys$output "device      = ''device'"
$	Write Sys$output "disk        = ''disk'"
$	Write Sys$output "list        = ''list'"
$   Endif
$
$TEST_LEN:
$!   environment = node + "." + disk + ":" + list
$   environment = node + ":" + list
$
$   If debug Then Write Sys$output "environment = ''environment'"
$
$   If F$length(environment) .Gt. 19
$   Then
$	list = "-" + F$extract(F$locate(".", list) + 1, F$length(list), list)
$	Goto TEST_LEN
$   Endif
$
$   environment = F$edit(environment, "lowercase")
$   a1 = F$edit(F$element(0, ":", environment), "upcase")
$   a2 = F$element(1, ":", environment)
$   environment = a1 + ":" + a2
$   Set prompt = "[1m''environment'> [m"
$EXIT

$OOPS:
$   Write Sys$output "OOPS! You must specify a path."
$EXIT

$DIR_NOT_THERE:
$   Write Sys$output "[31;47m''p1'[34;47m does not exist! [m"
$!   Read/prompt="Set Default anyway? " SYS$command yes_no
$!   If .Not. yes_no Then EXIT
$Return

$MOVE_LAST: Subroutine
$   If F$type(last_total) .Eqs. "" Then Goto END_MOVE_LAST_LOOP
$
$   kntr1 = F$integer('last_total')
$MOVE_LAST_LOOP:
$   kntr2 = kntr1 + 1
$   If F$type(last'kntr1') .Nes. ""
$   Then
$	temp = f$string(last'kntr1')
$	last'kntr2' :== 'temp'
$   Endif
$
$   kntr1 = kntr1 - 1
$   If kntr1 .Gt. 0 Then Goto MOVE_LAST_LOOP
$
$   If F$type(last) .Nes. "" Then last1 :== 'last'
$
$END_MOVE_LAST_LOOP:
$Endsubroutine

$SHOW_LAST: Subroutine
$   If F$type(last_total) .Eqs. ""
$   Then
$	Write sys$output "You are not updating any symbols:"
$	If F$type(last) .Nes. "" Then show sym last
$	Goto EXIT_SHOW_LAST
$   Else
$	kntr1 = F$integer('last_total')
$	Write sys$output "You are updating ''kntr1' symbols:"
$   Endif
$
$   If F$type(last) .Nes. "" Then show sym last
$
$   kntr2 = 1
$SHOW_LAST_LOOP:
$   If F$type(last'kntr2') .Nes. "" Then show sym last'kntr2'
$   kntr2 = kntr2 + 1
$   If kntr2 .Le. kntr1 Then Goto SHOW_LAST_LOOP
$
$EXIT_SHOW_LAST:
$Endsubroutine

$!
$PARSER:
$   !-----
$   ! Shift the parameters
$   !-----
$   Gosub shift_params
$
$   !-----
$   ! Check for qualifiers in every parameter
$   !-----
$   q1 = F$extract(F$locate("/", p1), F$length(p1), p1)
$   q2 = F$extract(F$locate("/", p2), F$length(p2), p2)
$   q3 = F$extract(F$locate("/", p3), F$length(p3), p3)
$   q4 = F$extract(F$locate("/", p4), F$length(p4), p4)
$   q5 = F$extract(F$locate("/", p5), F$length(p5), p5)
$   q6 = F$extract(F$locate("/", p6), F$length(p6), p6)
$   q7 = F$extract(F$locate("/", p7), F$length(p7), p7)
$   qualifiers = q1 + q2 + q3 + q4 + q5 + q6 + q7
$
$   !-----
$   ! Remove the qualifiers from the parameters
$   !-----
$   p1 = p1 - q1
$   p2 = p2 - q2
$   p3 = p3 - q3
$   p4 = p4 - q4
$   p5 = p5 - q5
$   p6 = p6 - q6
$   p7 = p7 - q7
$
$   !-----
$   ! Remove all spaces from the parameters
$   !-----
$   p1 = F$edit(p1, "collapse")
$   p2 = F$edit(p2, "collapse")
$   p3 = F$edit(p3, "collapse")
$   p4 = F$edit(p4, "collapse")
$   p5 = F$edit(p5, "collapse")
$   p6 = F$edit(p6, "collapse")
$   p7 = F$edit(p7, "collapse")
$
$   !-----
$   ! Shift the parameters if necessary
$   !-----
$   If p1 .Eqs. "" Then Gosub shift_params
$Return

$SHIFT_PARAMS:
$   p1 = p2
$   p2 = p3
$   p3 = p4
$   p4 = p5
$   p5 = p6
$   p6 = p7
$   p7 = p8
$   p8 = ""
$Return
