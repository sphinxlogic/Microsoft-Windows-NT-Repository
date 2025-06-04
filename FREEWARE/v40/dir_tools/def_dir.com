$!-----
$!
$!  Programmer:     Brian Vande Merwe
$!		    (801)226-3355
$!
$!  Group:          Development
$!
$!  Description:    Define a logical to point to the current directory.
$!
$
$   !
$   ! Make sure that P1..P7 (NOTE: NO P8) are parameters and all qualifiers are
$   ! in the variable (QUALIFIERS)
$   !
$   Gosub PARSER
$
$   !
$   ! If /SYS is on the command line, then define add the logical to the SYSTEM
$   ! table.
$   !
$   If F$locate("/SYS", qualifiers) .Ne. F$length(qualifiers)
$   Then
$	def_qual = "/SYSTEM"
$   Else
$	def_qual = ""
$   Endif
$
$   !
$   ! Make sure we have a logical to define
$   !
$   If p1 .Eqs. "" Then Inquire p1 "Logical to define"
$
$   !
$   ! Do it to it.
$   !
$   Define'def_qual' 'p1' 'F$environment("default")'
$
$   !
$   ! Show what happend
$   !
$   Show log 'p1'
$EXIT

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
$ Return

$SHIFT_PARAMS:
$   p1 = p2
$   p2 = p3
$   p3 = p4
$   p4 = p5
$   p5 = p6
$   p6 = p7
$   p7 = p8
$   p8 = ""
$ Return
