$!-----
$!
$!  Programmer:     Brian Vande Merwe
$!		    (801)226-3355
$!
$!  Group:          Development
$!
$!  Description:    Select a sub-directory.
$
$   !
$   ! Possible combinations to the DOWN command...
$   !
$   ! $ down
$   ! $ down/{num}
$   ! $ down [.]{directory}
$   ! $ down/{num} [.]{directory}
$   !
$   ! {directory} and also be a partial-directory to force "DOWN" to perform a
$   ! search for the directory being looked for.
$   !
$
$   !
$   ! Make sure that P1..P7 (NOTE: NO P8) are parameters and all qualifiers are
$   ! in the variable (QUALIFIERS)
$   !
$   Gosub PARSER
$
$   !
$   ! Setup local variables
$   !
$   file_counter = 0
$   last_file = ""
$   num_of_sub = -1
$   search_spec = "*.dir"
$   the_status = "YEP"
$
$   !
$   ! See if there is two dots ".".  If there is, then bypass all checking and
$   ! go right to the SET_DEFAULT routine.
$   !
$   loc1 = F$locate(".", p1)
$   If loc1 .Lt. F$length(p1)
$   Then
$	new_text = F$extract(loc1 + 1, F$length(p1), p1)
$	loc2 = F$locate(".", F$extract(0, F$length(new_text), new_text))
$	If loc2 .Lt. F$length(new_text)
$	Then
$	    !
$	    ! This will make sure the left and right brackets "[]" are present.
$	    !
$	    last_file = p1 - "[" - "]"
$	    last_file = "[" + last_file + "]"
$	    Goto SET_DEFAULT_BYPASS
$	Endif
$   Endif
$
$   !
$   ! Do we have text in p1?
$   !
$   If p1 .Nes. "" Then search_spec = "''p1'*.dir"
$
$   !
$   ! Do we have any qualifiers?  If so, get the number of the subdirectory
$   ! that the user want's to go to.
$   !
$   If F$length(qualifiers) .Gt. 0
$   Then
$	num_of_sub = F$integer(F$element(1, "/", qualifiers))
$	Gosub SUB_NUMBER
$	Goto SET_DEFAULT_BYPASS
$   Endif
$
$   !
$   ! Count the number of directories.
$   !
$   Gosub DIR_COUNTER
$
$   !
$   ! If there are more there is more that one directory that meets the search
$   ! specification then list what they are and exit.
$   !
$   If file_counter .Gt. 1
$   Then
$	Gosub LIST_DIRS
$	Goto THE_BIG_EXIT
$   Else
$	the_status = "YEP"
$	Goto SET_DEFAULT_BYPASS
$   Endif
$
$SET_DEFAULT:
$   Gosub CHECK_FILE
$
$SET_DEFAULT_BYPASS:
$
$   !
$   ! Test the CHECK_FILE status...
$   !
$   If the_status
$   Then
$	SDF 'last_file'
$   Else
$	Write Sys$output "Could not find the indicated directory."
$   Endif
$
$   Goto THE_BIG_EXIT


$DIR_COUNTER:
$   
$   If F$extract(0, 1, search_spec) .Eqs. "." Then -
	search_spec = search_spec - "."
$
$DIR_LOOP:
$   file = F$search(search_spec)
$   If file .Eqs. "" Then Goto NO_MORE_DIRS
$   last_file = "[." + F$parse(file,,, "name",) + "]"
$   file_counter = file_counter + 1
$   Goto DIR_LOOP
$
$NO_MORE_DIRS:
$ Return


$LIST_DIRS:
$   Write Sys$output "You must specify one of the following:"
$   Write Sys$output ""
$   cntr = 1
$   If F$extract(0, 1, search_spec) .Eqs. "." Then -
	search_spec = search_spec - "."
$
$NEW_DIR_LOOP:
$   file = F$search(search_spec)
$   If file .Eqs. "" Then Goto LIST_NO_MORE_DIRS
$   Write Sys$output F$fao("!3SL = !AS", -
	cntr, "[." + F$parse(file,,, "name",) + "]")
$   cntr = cntr + 1
$   Goto NEW_DIR_LOOP
$
$LIST_NO_MORE_DIRS:
$ Return


$SUB_NUMBER:
$   the_status = "YEP"
$   cntr = 1
$   If F$extract(0, 1, search_spec) .Eqs. "." Then -
	search_spec = search_spec - "."
$
$SUB_NUMBER_LOOP:
$   file = F$search(search_spec)
$   If file .Eqs. "" Then Goto SUB_NO_MORE_DIRS
$   last_file = "[." + F$parse(file,,, "name",) + "]"
$   If cntr .Eq. num_of_sub Then Goto SUB_NO_MORE_DIRS
$   cntr = cntr + 1
$   Goto SUB_NUMBER_LOOP
$
$SUB_NO_MORE_DIRS:
$ Return


$CHECK_FILE:
$   tst_file = F$search ("''last_file'")
$
$   If tst_file .Eqs. ""
$   Then
$	If F$extract(0, 1, search_spec) .Eqs. "." Then -
	    search_spec = search_spec - "."
$
$	tst_file = F$search(search_spec)
$
$	If tst_file .Eqs. ""
$	Then
$	    the_status = "NOPE"
$	Else
$	    !
$	    ! Do a simple search to clear the search context for the current
$	    ! directory that you are looking for.
$	    !
$	    tst_file = F$search("lkskjdflk.lkwjerlk")
$
$	    If file_counter .Gt. 1
$	    Then
$		Gosub LIST_DIRS
$		Goto THE_BIG_EXIT
$	    Else
$		the_status = "YEP"
$	    Endif
$	Endif
$   Else
$	the_status = "YEP"
$	last_file = "[." + F$parse(tst_file,,, "name",) + "]"
$   Endif
$
$ Return


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


$THE_BIG_EXIT:
$EXIT
