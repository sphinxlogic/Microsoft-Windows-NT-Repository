$!-----
$!
$!  Programmer:     Brian Vande Merwe
$!		    (801)226-3355
$!
$!  Group:          Development
$!
$!  Description:    Search for a directory to move to.  Use 'SDF' to change
$!		    the default.
$!
$! Valid qualifiers:
$!	/<n>		! Where n is the item number of the directory to select.
$!	/(D)elete	! Delete the specified record.
$!	/(E)xact	! Indicates that p1 is a exact directory.
$!	/(IG)nore	! Ignore item
$!	/(IN)it		! To initialize the holdingTANK file.
$!	/(L)log		! Log what's happening.
$!	/(NOL)og	! Do NOT log what's happening.
$!	/(O)verride	! Override the IGNORE flag.
$!	/(R)estore	! Restore an IGNORED item.
$!	/(V)er		! Verify entries in the holdingTANK file.
$!	/(?)		! Display the "LAST" directories.
$!
$! Valid parameters:
$!	<partial dir>	! Specify a partial directory.  Could be used with /part
$!	?		! Display the "LAST" directories.
$!
$!  Examples:
$!	    $ acr misc
$!		Will change your default to the MISC.DIR directory.  If there
$!		are more that one, you will be given a list.  Then use "$
$!		acr/4 misc" to select the 4th item in the list given.
$!
$!-----
$   v = f$verify('across_debug')
$
$   on control_y then goto HASTA_LA_VISTA_BABY
$
$   Gosub PARSER
$
$   csi_off = "[m"
$   csi_bwarn = "[31;44;1m"
$   csi_warn = "[31;1m"
$   csi_info = "[34;1m"
$   csi_text = "[33;1m"
$   csi_show = "[37;44;1m"
$
$   if f$type(sdf) .eqs. "" then sdf :== set default
$
$   un = f$edit(f$getjpi("", "username"), "collapse")
$
$   datfile := misc:across_tank.'un'
$
$   standard = f$integer(f$trnlnm("_across$standard"))
$   starting_search = "000000"
$
$   if f$locate("/?", qualifiers) .nes. f$length(qualifiers) .or. p1 .eqs. "?"
$   then
$	sdf/list
$	goto HASTA_LA_VISTA_BABY
$   endif
$	
$   if standard .eq. 1
$   then
$	csi_off = "[m"
$	csi_bwarn = "[31;44;1m"
$	csi_warn = "[31;1m"
$	csi_info = "[34;1m"
$	csi_text = "[33;1m"
$	csi_show = "[37;44;1m"
$
$	starting_search = ""
$	the_dir = ""
$	datfile := sys$login:todir.dat
$	if (p1 .eqs. "") .and. (the_dir .eqs. "") then goto BVM_START
$	if p1 .nes. ""
$	then
$	    if f$trnlnm("''p1'") .nes. "" then the_dir = p1
$	    if f$extract(0, 1, p1) .eqs. "." then the_dir = "[''p1']"
$	    if qualifiers .eqs. "" .and. f$extract(0, 1, p1) .eqs. "[" then the_dir = p1
$	endif
$	if the_dir .eqs. "" then goto BVM_START
$	sdf 'the_dir'
$	goto HASTA_LA_VISTA_BABY
$   endif
$
$BVM_START:
$   if f$locate("/V", qualifiers) .nes. f$length(qualifiers)
$   then
$	write sys$output "''csi_show'verifying items in ''datfile'''csi_off'"
$	write sys$output ""
$
$	open/read/write/share tank 'datfile'
$VER_READ_LOOP:
$	read/end_of_file=END_VER_READ_LOOP tank stuff
$	stuff = stuff - "*"
$	file_loc = f$edit(f$element(1, ";", stuff) + f$element(0, ";", stuff) + ".DIR", "collapse")
$	if f$locate("/L", qualifiers) .nes. f$length(qualifiers) then write sys$output "Verify: ''file_loc'"
$	if f$search(file_loc) .eqs. ""
$	then
$	    if f$locate("/D", qualifiers) .nes. f$length(qualifiers)
$	    then
$		write sys$output "''csi_bwarn'Removing ''csi_show'''file_loc'''csi_off'"
$		read/error=NO_DEL_RECORD/key='stuff'/delete tank chk_rec
$	    else
$		write sys$output "''csi_info'Missing: ''csi_text'''file_loc'''csi_off'"
$	    endif
$	endif
$
$	goto VER_READ_LOOP
$
$END_VER_READ_LOOP:
$	close tank
$
$	goto HASTA_LA_VISTA_BABY
$   endif
$	    
$   if f$locate("/IN", qualifiers) .eqs. f$length(qualifiers) then goto NO_INIT
$
$   if f$search("''datfile'") .eqs. ""
$   then
$	gosub CREATE_TANK
$   endif
$
$   if p1 .eqs. "" then p1 = f$element(0, ".", f$environment("default"))
$
$   search_spec = p1 - "..." - "]" + "...]*.dir"
$
$   write sys$output "''csi_info'searching ''csi_text'''search_spec'''csi_off'"
$   write sys$output ""
$
$   kntr = 0
$
$   open/write/read/share tank 'datfile'
$
$FILE_LOOP:
$   file = f$search(search_spec)
$   if file .eqs. "" then goto END_FILE_LOOP
$   filename = "]" + f$parse(file,,, "name")
$   file = file - filename - f$parse(file,,, "type") - f$parse(file,,, "version") - "000000." + "]"
$   out_text = filename + ";" + file - "]"
$
$   if f$locate("/LOG", qualifiers) .nes. f$length(qualifiers)
$   then
$	write sys$output "[1mFound ''file'''filename'[m" - "]"
$   else
$   endif
$
$   read/error=WRITE_ITEM/key='out_text' tank info
$   if out_text .eqs. f$edit(info, "collapse") - "*" then goto FILE_LOOP
$
$WRITE_ITEM:
$   write/error=WRITE_REC_ERROR tank f$fao("!132AS", out_text)
$
$   if f$locate("/NOL", qualifiers) .nes. f$length(qualifiers)
$   then
$   else
$	write sys$output "''csi_show'Added ''file'''filename'''csi_off'" - "]"
$   endif
$   kntr = kntr + 1
$
$   goto FILE_LOOP
$
$END_FILE_LOOP:
$   close tank
$
$   if f$locate("/L", qualifiers) .nes. f$length(qualifiers) then write sys$output ""
$   if f$locate("/L", qualifiers) .nes. f$length(qualifiers) then write sys$output "''csi_show'Added ''kntr' files.''csi_off'"
$
$   goto HASTA_LA_VISTA_BABY
$
$NO_INIT:
$   if f$search("''datfile'") .eqs. ""
$   then
$	write sys$output "''csi_warn'Can't find ''datfile'''csi_off'"
$	goto HASTA_LA_VISTA_BABY
$   endif
$
$   if p1 .eqs. ""
$   then
$	write sys$output "''csi_info'Nothing to do...''csi_off'"
$	goto HASTA_LA_VISTA_BABY
$   endif
$
$   if f$locate("/L", qualifiers) .nes. f$length(qualifiers) then write sys$output "''csi_info'Searching for request...''csi_off'"
$   kntr = 0
$   open/write/read/share tank 'datfile'
$   read/error=END_READ_NEXT_REC/key='p1' tank info
$READ_NEXT_REC:
$   if f$edit(info, "collapse") .eqs. f$edit(info - "*", "collapse") .or. f$locate("/O", qualifiers) .nes. f$length(qualifiers)
$   then
$	info = info - "*"
$	if f$locate("/E", qualifiers) .nes. f$length(qualifiers)
$	then
$	    if p1 .eqs. f$element(0, ";", info)
$	    then
$		kntr = kntr + 1
$		saved'kntr' = f$edit(f$element(1, ";", info) + f$element(0, ";", info), "collapse")
$	    endif
$	else
$	    kntr = kntr + 1
$	    saved'kntr' = f$edit(f$element(1, ";", info) + f$element(0, ";", info), "collapse")
$	endif
$   endif
$
$   read/end_of_file=END_READ_NEXT_REC tank new_info
$   if f$extract(0, f$length(p1), new_info) .nes. p1 then goto END_READ_NEXT_REC
$   info = new_info
$   goto READ_NEXT_REC
$
$END_READ_NEXT_REC:
$   close tank
$
$   !
$   ! Check for nothing to select.
$   !
$   if kntr .eq. 0
$   then
$	write sys$output "''csi_info'No directory found...''csi_off'"
$	read/prompt="''csi_show'Scan [''starting_search'...] (Y/[N])?''csi_off' " sys$command yes_no
$	if yes_no
$	then
$	   the_dir = f$search("[''starting_search'...]''p1'*.dir")
$	   if the_dir .nes. ""
$	   then
$	      filename = "]" + f$parse(the_dir,,, "name")
$	      the_dir = the_dir - f$parse(the_dir,,, "version") - f$parse(the_dir,,, "type") - filename + "." + filename - "]" + "]"
$	      sdf 'the_dir'
$	   endif
$	endif
$   endif
$
$   if kntr .eq. 1
$   then
$	the_dir = f$element(0, "]", saved1) + "." + f$element(1, "]", saved1) + "]
$	if f$locate("/D", qualifiers) .nes. f$length(qualifiers)
$	then
$	    val = 1
$	    gosub DELETE_RECORD
$	else
$	    if f$locate("/IG", qualifiers) .nes. f$length(qualifiers)
$	    then
$		gosub SET_IGNORE
$	    else
$		if f$locate("/R", qualifiers) .nes. f$length(qualifiers)
$		then
$		    gosub RESET_IGNORE
$		else
$		    sdf 'the_dir'
$		endif
$	    endif
$	endif
$   endif
$
$   if kntr .gt. 1
$   then
$	gosub GET_VAL
$
$	if val .eq. 0
$	then
$	    gosub SHOW_LIST
$
$	    if standard .eq. 1
$	    then
$		Read/prompt="''csi_show'Directory Number:''csi_off' "/end_of_file=DO_NUTTIN sys$command val
$		DO_NUTTIN:
$		if val .ne. 0
$		then
$		    the_dir = f$element(0, "]", saved'val') + "." + f$element(1, "]", saved'val') + "]
$		    sdf 'the_dir'
$		endif
$	    endif
$	else
$	    the_dir = f$element(0, "]", saved'val') + "." + f$element(1, "]", saved'val') + "]
$	    if f$locate("/D", qualifiers) .nes. f$length(qualifiers)
$	    then
$		gosub DELETE_RECORD
$	    else
$		if f$locate("/IG", qualifiers) .nes. f$length(qualifiers)
$		then
$		    gosub SET_IGNORE
$		else
$		    if f$locate("/R", qualifiers) .nes. f$length(qualifiers)
$		    then
$			gosub RESET_IGNORE
$		    else
$			sdf 'the_dir'
$		    endif
$		endif
$	    endif
$	endif
$   endif
$
$HASTA_LA_VISTA_BABY:
$   if f$trnlnm("tank") .nes. "" then close tank
$
$   exit

$!
$WRITE_REC_ERROR:
$   write sys$output ""
$   write sys$output "Error writing record..."
$   write sys$output out_text
$   write sys$output ""
$   goto HASTA_LA_VISTA_BABY

$!
$DELETE_RECORD:
$   if f$locate("/L", qualifiers) .nes. f$length(qualifiers) then write sys$output "''csi_warn'Removing requested record...''csi_off'"
$
$   open/read/write/share tank 'datfile'
$
$   file = saved'val'
$   filename = f$parse(file,,, "name")
$   file = file - filename
$   out_text = filename + ";" + file
$
$   write sys$output "''csi_bwarn'removing ''csi_show'''file'.''filename']''csi_off'" - "]"
$   read/error=NO_DEL_RECORD/key='out_text'/delete tank chk_rec
$   goto END_REM_READ
$
$NO_DEL_RECORD:
$   write sys$output "''csi_show'The requested record was not found...''csi_off'"
$
$END_REM_READ:
$   close tank
$
$   return

$!
$SET_IGNORE:
$   if f$locate("/L", qualifiers) .nes. f$length(qualifiers) then write sys$output "''csi_warn'Setting ignore on requested record...''csi_off'"
$
$   open/read/write/share tank 'datfile'
$
$   file = saved'val'
$   filename = f$parse(file,,, "name")
$   file = file - filename
$   out_text = filename + ";" + file
$
$   write sys$output "''csi_bwarn'ignoring ''csi_show'''file'.''filename']''csi_off'" - "]"
$   read/error=NO_READ_RECORD/key='out_text'/delete tank chk_rec
$   out_text = f$edit(out_text, "collapse") + "*"
$   write/error=WRITE_REC_ERROR tank f$fao("!132AS", out_text)
$
$   goto END_SET_IGNORE
$
$NO_READ_RECORD:
$   write sys$output "''csi_show'The requested record was not found...''csi_off'"
$
$END_SET_IGNORE:
$   close tank
$
$   return

$!
$RESET_IGNORE:
$   if f$locate("/L", qualifiers) .nes. f$length(qualifiers) then write sys$output "''csi_warn'Setting ignore on requested record...''csi_off'"
$
$   open/read/write/share tank 'datfile'
$
$   file = saved'val'
$   filename = f$parse(file,,, "name")
$   file = file - filename
$   out_text = filename + ";" + file
$
$   write sys$output "''csi_bwarn'resetting ''csi_show'''file'.''filename']''csi_off'" - "]"
$   read/error=NO_READ_RECORD/key='out_text'/delete tank chk_rec
$   out_text = f$edit(out_text, "collapse")
$   write/error=WRITE_REC_ERROR tank f$fao("!132AS", out_text)
$
$   close tank
$
$   return

$!
$GET_VAL:
$   nkntr = 0
$   val = 0
$CHECK_SELECT:
$   q = f$element(nkntr, "/", qualifiers)
$   nkntr = nkntr + 1
$   if q .eqs. "/" then goto END_CHECK_SELECT
$   val = f$integer(q)
$   if val .eq. 0 then goto CHECK_SELECT
$
$END_CHECK_SELECT:
$   return

$!
$SHOW_LIST:
$   show_kntr = 1
$SHOW_LIST_LOOP:
$   the_dir = f$element(0, "]", saved'show_kntr') + "." + f$element(1, "]", saved'show_kntr') + "]
$   write sys$output f$fao("!3SL -- !AS", show_kntr, the_dir)
$   show_kntr = show_kntr + 1
$   if show_kntr .gt. kntr then goto END_SHOW_LIST_LOOP
$   goto SHOW_LIST_LOOP
$END_SHOW_LIST_LOOP:
$   return

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
$   q1 = f$extract(f$locate("/", p1), f$length(p1), p1)
$   q2 = f$extract(f$locate("/", p2), f$length(p2), p2)
$   q3 = f$extract(f$locate("/", p3), f$length(p3), p3)
$   q4 = f$extract(f$locate("/", p4), f$length(p4), p4)
$   q5 = f$extract(f$locate("/", p5), f$length(p5), p5)
$   q6 = f$extract(f$locate("/", p6), f$length(p6), p6)
$   q7 = f$extract(f$locate("/", p7), f$length(p7), p7)
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
$   p1 = f$edit(p1, "collapse")
$   p2 = f$edit(p2, "collapse")
$   p3 = f$edit(p3, "collapse")
$   p4 = f$edit(p4, "collapse")
$   p5 = f$edit(p5, "collapse")
$   p6 = f$edit(p6, "collapse")
$   p7 = f$edit(p7, "collapse")
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

$!
$CREATE_TANK:
$	create/nolog/fdl=sys$input 'datfile'
IDENT	"20-JAN-1995 09:11:49	OpenVMS ANALYZE/RMS_FILE Utility"

SYSTEM
	SOURCE                  OpenVMS

FILE
	BEST_TRY_CONTIGUOUS     yes
	BUCKET_SIZE             3
	CLUSTER_SIZE            3
	CONTIGUOUS              no
	EXTENSION               3
	FILE_MONITORING         no
	GLOBAL_BUFFER_COUNT     0
	ORGANIZATION            indexed

RECORD
	BLOCK_SPAN              yes
	CARRIAGE_CONTROL        carriage_return
	FORMAT                  variable
	SIZE                    132

AREA 0
	ALLOCATION              18
	BEST_TRY_CONTIGUOUS     yes
	BUCKET_SIZE             3
	EXTENSION               3

AREA 1
	ALLOCATION              3
	BEST_TRY_CONTIGUOUS     yes
	BUCKET_SIZE             3
	EXTENSION               3

KEY 0
	CHANGES                 no
	DATA_KEY_COMPRESSION    no
	DATA_RECORD_COMPRESSION yes
	DATA_AREA               0
	DATA_FILL               100
	DUPLICATES              no
	INDEX_AREA              1
	INDEX_COMPRESSION       no
	INDEX_FILL              100
	LEVEL1_INDEX_AREA       1
	NAME                    "File"
	NULL_KEY                no
	PROLOG                  3
	SEG0_LENGTH             132
	SEG0_POSITION           0
	TYPE                    string
$   return
