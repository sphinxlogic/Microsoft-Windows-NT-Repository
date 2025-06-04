$ ! Procedure:	SYS_SYMBOL_TO_LOGICAL.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$ disk_checker__ = "dbslibrary:sys_check_sys$disk.com"
$ if (f$search(disk_checker__) .nes. "") then @'disk_checker__'
$	set noon
$	say = "write sys$output"
$	pattern = f$edit(P1, "collapse, upcase")
$	if (pattern .eqs. "") then goto no_symbol
$	to_be_removed = f$edit(P2, "collapse, upcase")
$	prefix = f$edit(P3, "collapse, upcase")
$	symbol_list = "''f$getjpi("","pid")'_SYMBOLS.LIST"
$	definee/nolog sys$error nl:
$	definee/nolog sys$output 'symbol_list'
$	show symbol/global 'pattern'
$	deassign sys$output
$	deassign sys$error
$	open/read/error=end_read_loop symbols 'symbol_list'
$read_loop:
$	read/end_of_file=end_read_loop symbols symbol_text
$	symbol_text = f$edit(symbol_text, "collapse, upcase")
$	if (f$extract(0, 1, symbol_text) .eqs. "$") then goto read_loop
$	if (f$extract(0, 1, symbol_text) .eqs. "%") then goto read_loop
$	delay_symbol = f$element(0, "=", symbol_text) - "*"
$	delay_value = &delay_symbol
$	delay_logical = delay_symbol - "''to_be_removed'"
$	if (delay_value .nes. "") then -
$	    if (.not. f$trnlnm("''prefix'''delay_logical'",,,,,"no_alias")) -
		then define 'prefix''delay_logical' "''delay_value'"
$ goto read_loop
$end_read_loop:
$	if (f$trnlnm("symbols") .nes. "") then close/nolog symbols
$	if (f$search(symbol_list) .nes. "") then deletee/nolog 'symbol_list';*
$ goto bail_out
$no_symbol:
$	say "%SYM2LOG-E-NOSYMPAT, symbol pattern is missing"
$ goto bail_out
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_SYMBOL_TO_LOGICAL.COM
$ !
$ ! Purpose:	This procedure will take symbols as described by the
$ !		parameters and define logicals based on the symbol names and
$ !		symbol values.  To define the logicals in a particular table
$ !		make sure you have a symbol 'define' which will do the right
$ !		thing, since the table name is not passed.
$ !
$ ! Parameters:
$ !	 P1	This the what we use to get the symbols;
$ !			SHOW SYMBOL/GLOBAL 'P1'
$ !	[P2]	This is removed from the symbol name to create the logical
$ !		name.  Given that the symbol name will usually have a common
$ !		prefix e.g. cfg_, then this will usually be that prefix
$ !		(unless you want the prefix in the logical name).
$ !	[P3]	This can be used to specify a prefix to the logical names
$ !		created e.g. DISK$.
$ !
$ ! Example:
$ !			@sys_sy..ical cfg_disk_*1 cfg_disk_ disk$
$ !		This will use all the configuration symbols for the disks and
$ !		define logicals beginning with DISK$ e.g. DISK$SYS1.
$ !
$ ! History:
$ !		29-Jun-1989, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		12-Sep-1989, DBS; Version V1-002
$ !	002 -	Changed the name of the temporary list file.
$ !		29-Sep-1989, DBS; Version V1-003
$ !	003 -	Changed to check for empty strings on the define.
$ !		30-Jul-1990, DBS; Version V1-004
$ !	004 -	Added call to the disk check procedure.
$ !		09-Sep-1990, DBS; Version V1-005
$ !	005 -	Fixed to check for no_alias before defining logical.
$ !-==========================================================================
