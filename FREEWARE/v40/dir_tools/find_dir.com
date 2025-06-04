$!-----
$!
$!  Programmer:     Brian Vande Merwe
$!
$!  Group:          Development
$!
$!  Creation Date:  21-Oct-1992
$!
$!  Description:    Search the directory tree (starting with the "upper" most
$!		    level. EX: [BRIVAN.COMMON.MISC] --> will start at [BRIVAN])
$!		    Define a process logical pointing to the directory to make
$!		    the search faster the next time this program is run.
$!
$!-----
$   curr_dir = F$environment("default")
$
$   If p2 .Nes. "" Then curr_dir = p2
$
$   root_dir = F$element(0, ".", curr_dir) -"]" + "]"
$
$   comment = "!"
$
$   !
$   ! Make sure the user entered a directory.
$   !
$   If p1 .Eqs. ""
$   Then
$	Write Sys$output "OOPS! You must specify a directory"
$	EXIT
$   Endif
$
$   srch_dir = F$element(0, ".", curr_dir) - "]" + "...]''p1'.dir"
$
$   !
$   ! If the logicals pointing to that directory already exists, then go
$   ! directly to the directory without searching for it.
$   !
$!   If F$trnlnm(p1,,,, "CASE_BLIND",) .Nes. ""
$!   Then
$!	last_file = F$trnlnm(p1,,,, "CASE_BLIND",)
$!	Goto SET_DEFAULT
$!   Endif
$
$   file_counter = 0
$
$   DIR_LOOP:
$	file = F$search(srch_dir)
$	If file .Eqs. "" Then Goto NO_MORE_DIRS
$	last_file = file
$	file_counter = file_counter + 1
$	Goto DIR_LOOP
$   NO_MORE_DIRS:
$
$   If file_counter .Eq. 0
$   Then
$	Write Sys$output ""
$	Write Sys$output "Could not find [1m''p1'[m"
$	Write Sys$output ""
$	Goto THE_BIG_EXIT
$   Endif
$
$   If file_counter .Gt. 1 Then Goto LIST_DIRS
$
$ SET_DEFAULT:
$   Gosub DEFINE_IT
$
$   !
$   ! Set default to the found directory.
$   !
$   SDF 'file'
$   Goto THE_BIG_EXIT
$
$ LIST_DIRS:
$   Write Sys$output ""
$   Write Sys$output "Found more than one listing of [1m''p1'[m"
$   Read/prompt="Prompt before defining the SYMBOLS and LOGICALS? " -
	SYS$command yes_no1
$   Write Sys$output ""
$   cntr = 1
$
$   NEW_DIR_LOOP:
$	file = F$search(srch_dir)
$	If file .Eqs. "" Then Goto THE_BIG_EXIT
$
$	last_file = file
$	If yes_no1
$	Then
$	    Read/prompt="Define ''file'? " SYS$command yes_no2
$	    If yes_no2
$	    Then
$		comment = ""
$		Gosub DEFINE_IT
$	    Endif
$	Else
$	    comment = ""
$	    Gosub DEFINE_IT
$	Endif
$	cntr = cntr + 1
$   Goto NEW_DIR_LOOP
$
$ THE_BIG_EXIT:
$EXIT
$
$ DEFINE_IT:
$   file = last_file
$   file = file - F$parse(file,,, "type",) - F$parse(file,,, "version",)
$   file = F$element(0, "]", file) + "." + F$element(1, "]", file) + "]"
$
$   !
$   ! Make some refinements before we define the symbols pointing to the
$   ! requested directory.
$   !
$   ele = 0
$   the_ele = ele
$   ELEMENT_LOOP:
$	text = F$element(ele, ".", file)
$	If text .Eqs. "." Then Goto END_ELEMENT_LOOP
$
$	If text - "]" .Eqs. p1
$	Then
$	    the_ele = ele
$	Endif
$
$	ele = ele + 1
$	Goto ELEMENT_LOOP
$   END_ELEMENT_LOOP:
$
$   new_file = file - F$parse(file,,, "device",)
$   text = F$element(the_ele - 1, ".", new_file)
$   text = text - "[" - "]"
$   'comment'first_text = F$extract(0, 1, text)
$
$   !
$   ! Define a logical to speed the process up next time.
$   !
$   Define/nolog 'first_text''p1' 'file'
$   Write Sys$output "Defined logical: ''first_text'''p1'"
$
$   !
$   ! Define a symbol to help bypass this whole procedure after it has been run
$   ! once or twice.
$   !
$   'first_text''p1' :== 'SDF' 'file'
$   Write Sys$output " Defined symbol: ''first_text'''p1'"
$
$   Write Sys$output ""
$
$   Return
