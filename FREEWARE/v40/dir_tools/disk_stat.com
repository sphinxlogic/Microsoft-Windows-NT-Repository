$!-----
$!
$!  Programmer:     Brian Vande Merwe
$!		    (801)226-3355
$!
$!  Group:          Development
$!
$!  Description:    Utility to display a "map" of the current disk drives in a
$!		    cluster.
$!
$!  Parameters:	    Disk(s) to display.  If no parameters are entered,
$!			all disks are display.  If the letter "a" is entered,
$!			then all disks STARTING with the letter "a" will be
$!			displayed.
$!		    >x -- Reverse video disks with free space greater than (x)
$!		    <x -- Reverse video disks with free space less than than (x)
$!
$!  Qualifier:	    /[A]ll       -- Display all devices even if the device type
$!				    does not translate.
$!
$!		    /NOC[hange]  -- Do not display the number of blocks changed
$!				    since the last disk_stat was run.
$!
$!		    /NOSO[rt]    -- Do not sort the output.
$!
$!		    /NOT[ime]    -- Do not display the timestamp information.
$!
$!		    /I[nterval=] -- Number of seconds to up date the display.
$!
$!		    /P[age]      -- Display the disk map until CTRL-Z is
$!				    entered.
$!
$!		    /SO[rt=]     -- Sort disk in low to high order on
$!				    specified fields.
$!				    (fields:	[v]olume, [%]full, [ma]xblocks,
$!						[f]reeblocks, [me]gs
$!
$!		    /SY[stem]    -- Define the attribute logicals in the system
$!				    logical name table.
$!
$!		    /T[ime]      -- Specify the escape sequence for timestamp
$!				    information.
$!
$!		    /80          -- Specify the escape sequence for 80 percent
$!				    files
$!
$!		    /95          -- Specify the escape sequence for 95 percent
$!				    files
$!
$!		    /98          -- Specify the escape sequence for 98 percent
$!				    files
$!
$!
$!  Example:	    First, define a symbol (I use DS) to run the procedure.
$!		    Example: $ DS:==@DISK_STAT PAR=
$!		    Notice the "PAR=" at the end of the symbol definition.
$!		    This is to allow for "CLI" parsing.  This means that you
$!		    can now enter commands like: $ DS/PAGE without any spaces.
$!
$!		    $ ds/page t
$!			continueuously display all driver starting with "t"
$!
$!		    $ ds >20000
$!			display all disks and reverse video those with free
$!			space greater than 20000.
$!
$!		    $ ds/80=34;1/95=7/98=45;5/time=32;1
$ !			display all disks and change the colors of the output.
$!-----
$!'F$verify(0)'
$
$   Gosub PARSER
$
$   b_space = p1
$
$!--------------------------------------------------------------------------
$! Get cpu information
$!--------------------------------------------------------------------------
$   cpu = F$getsyi("node_hwtype")	    ! System CPU Type
$   node = F$getsyi("nodename")		    ! Current NODE Name
$
$!-----
$! Initialiaze variables
$!-----
$   ds$false = (0 .Eq. 1)
$   ds$true = .Not. ds$false
$
$   srt_cnt = 0
$
$   max_megs = 0
$   max_b_total = 0
$   fre_b_total = 0
$   usd_b_total = 0
$   dif_b_total = 0
$   mask1 = "!13AS !10AS %!4AS   !10AS !10AS  !10AS !5AS !4AS"
$   mask2 = "!13AS !10AS %!5AS !10SL  !10SL !10SL  !5SL !4AS"
$   mask3 = "!13AS !10AS %!5AS !10SL  !10SL !10SL  !5AS !4AS"
$
$   ds$cls = "Write Sys$output ""[2J[1;1H"""
$   ds$top = "Write Sys$output ""[1;1H"""
$
$   un = F$edit(F$getjpi("", "username"), "collapse")
$
$   at_noti = ";1"
$   at_warn = ";7"
$   at_seve = ";5"
$   at_time = "1m"
$
$   !
$   ! Add customized attributes here...
$   !
$   If un .Eqs. "BRIVAN"
$   Then
$	at_noti = ";34;1"
$	at_warn = ";43"
$	at_seve = ";41;5"
$	at_time = "32;1m"
$   Endif
$
$   !
$   ! Does the user want to define the logicals in the SYSTEM tables?
$   !
$   If F$locate("/SY", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	def_sys = "/system"
$   Else
$	def_sys = ""
$   Endif
$
$   !
$   ! Parse the notify (80 percent) code.
$   !
$   If F$locate("/80", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	start_q80 = F$locate("/80", qualifiers)
$	q80_qual = F$element(1, "/", -
	    F$extract(start_q80, F$length(qualifiers), qualifiers))
$	q80 = F$element(1, "=", q80_qual)
$	at_noti = q80
$	If F$extract(0, 1, at_noti) .Nes. ";" Then at_noti = ";" + at_noti
$	Define/nolog'def_sys' _disk_stat_80 "''at_noti'"
$   Else
$	q80 = F$trnlnm("_disk_stat_80")
$	If F$length(q80) .ne. 0 Then at_noti = q80
$   Endif
$
$   !
$   ! Parse the warning (95 percent) code.
$   !
$   If F$locate("/95", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	start_q95 = F$locate("/95", qualifiers)
$	q95_qual = F$element(1, "/", -
	    F$extract(start_q95, F$length(qualifiers), qualifiers))
$	q95 = F$element(1, "=", q95_qual)
$	at_warn = q95
$	If F$extract(0, 1, at_warn) .Nes. ";" Then at_warn = ";" + at_warn
$	Define/nolog'def_sys' _disk_stat_95 "''at_warn'"
$   Else
$	q95 = F$trnlnm("_disk_stat_95")
$	If F$length(q95) .ne. 0 Then at_warn = q95
$   Endif
$
$   !
$   ! Parse the severe (98 percent) code.
$   !
$   If F$locate("/98", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	start_q98 = F$locate("/98", qualifiers)
$	q98_qual = F$element(1, "/", -
	    F$extract(start_q98, F$length(qualifiers), qualifiers))
$	q98 = F$element(1, "=", q98_qual)
$	at_seve = q98
$	If F$extract(0, 1, at_seve) .Nes. ";" Then at_seve = ";" + at_seve
$	Define/nolog'def_sys' _disk_stat_98 "''at_seve'"
$   Else
$	q98 = F$trnlnm("_disk_stat_98")
$	If F$length(q98) .ne. 0 Then at_seve = q98
$   Endif
$
$   !
$   ! Parse the time (98 percent) code.
$   !
$   If F$locate("/T", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	start_qt = F$locate("/T", qualifiers)
$	qt_qual = F$element(1, "/", -
	    F$extract(start_qt, F$length(qualifiers), qualifiers))
$	qt = F$element(1, "=", qt_qual)
$	at_time = qt - "m" + "m"	! Make sure there is a "m" at the end
$					! of the sequence
$	Define/nolog'def_sys' _disk_stat_time "''at_time'"
$   Else
$	qt = F$trnlnm("_disk_stat_time")
$	If F$length(qt) .ne. 0 Then at_time = qt
$   Endif
$
$   !
$   ! Add the CSI code to the beginning of the at_time varible.
$   !
$   at_time = "[" + at_time
$
$   !
$   ! Are we in page mode?
$   !
$   If F$locate("/P", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	page_mode = ds$true
$   Else
$	page_mode = ds$false
$   Endif
$
$   !
$   ! Sort field
$   !
$   s = ""
$   n = "!"
$   sort_field = "%"
$
$   If F$locate("/SO", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	start_sort = F$locate("/SO", qualifiers)
$	sort_qual = F$element(1, "/", -
	    F$extract(start_sort, F$length(qualifiers), qualifiers))
$	sort_field = F$element(1, "=", sort_qual)
$
$	If F$extract(0, 2, sort_field) .eqs. "MA" Then sort_field = "MAX"
$	If F$extract(0, 2, sort_field) .eqs. "ME" Then sort_field = "MEG"
$	If F$extract(0, 1, sort_field) .eqs. "F" Then sort_field = "FRE"
$	If F$extract(0, 1, sort_field) .eqs. "V" Then sort_field = "VOL"
$	If F$extract(0, 1, sort_field) .eqs. "%" Then sort_field = "%"
$   Endif
$
$   If F$locate("/NOSO", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	s = "!"
$	n = ""
$   Endif
$
$   !
$   ! Get the interval for use in PAGE mode.
$   !
$   If F$locate("/I", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	start_inter = F$locate("/I", qualifiers)
$	inter_qual = F$element(1, "/", -
	    F$extract(start_inter, F$length(qualifiers), qualifiers))
$	interval = F$element(1, "=", inter_qual)
$   Else
$	interval = 10
$   Endif
$
$   If page_mode Then ds$cls
$TOP_SCREEN:
$   If page_mode Then ds$top
$
$   srt_cnt = 0
$
$   max_megs = 0
$   max_b_total = 0
$   fre_b_total = 0
$   usd_b_total = 0
$   dif_b_total = 0
$
$   !
$   ! Tell the user the node running on and the last time run.
$   !
$   last_run = F$trnlnm("ds_timestamp")
$   If F$length(last_run) .eq. 0 Then last_run = "N/A"
$!   Write Sys$output "Status for ''node' (''cpu')"
$
$   !
$   ! Display the timestamp info.
$   !
$   If F$locate("/NOT", qualifiers) .Nes. F$length(qualifiers)
$   Then
$   Else
$	da_time = F$time()
$	Write Sys$output "Current time: ''at_time'''da_time'[m, Last run: ''at_time'''last_run'[m"
$	Define/proc/nolog ds_timestamp "''da_time'"
$   Endif
$
$   !
$   ! Display the number of blocks changed.
$   !
$   If F$locate("/NOC", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	Write Sys$output F$fao(mask1, "Volume", "Label", "Full", "MAXBLOCKS", -
	    "USEDBLOCKS", "FREBLOCKS", "MEG's", "Type")
$   Else
$	Write Sys$output F$fao(mask1, "Volume", "Label", "Full", "MAXBLOCKS", -
	    "FREBLOCKS", "BLKS CHNGD", "MEG's", "Type")
$   Endif
$   Write Sys$output "[K"
$
$   !
$   ! Define the number and names of the disk drives for the current system
$   !
$
$   max_disks = 0
$FIND_DISKS_LOOP:
$   disk = F$device("*", "disk") - "_"
$   If disk .Eqs. "" Then Goto END_FIND_DISKS_LOOP
$   If F$locate("/A", qualifiers) .Eqs. F$length(qualifiers) then -
	If F$getdvi(disk, "media_name") .eqs. "" then goto FIND_DISKS_LOOP
$   max_disks = max_disks + 1
$   disk'max_disks' = disk
$   Goto FIND_DISKS_LOOP
$
$END_FIND_DISKS_LOOP:
$   cntr = 0
$LOOP:
$   cntr = cntr + 1
$   If cntr .Gt. max_disks Then Goto SHOW_TOTALS
$   disk_name = disk'cntr'
$
$   !
$   ! Make sure there is a disk to report on.
$   !
$   If .Not. F$getdvi(disk_name, "EXISTS")
$   Then
$	Write Sys$output "No duch disk: ''disk_name'"
$	Goto LOOP
$   Endif
$
$   label = F$getdvi(disk_name, "VOLNAM")
$
$   !
$   ! Display only the disk's that the user has requested.  (If the user
$   ! requested specific disks)
$   !
$   If F$extract(0, 1, b_space) .Nes. "<" .And. F$extract(0, 1, b_space) .Nes. ">"
$   Then
$	If F$extract(0, F$length(b_space), disk_name) .Nes. b_space Then Goto LOOP
$   Endif
$
$   Gosub SHOW_INFO
$   Goto LOOP

$SHOW_TOTALS:
$   Gosub SHOW_TOTAL
$
$   If page_mode
$   Then
$	Read/prompt=""/end_of_file=HASTA_LA_VISTA_BABY/time='interval' -
	    /error=TOP_SCREEN SYS$command cmd
$	Goto TOP_SCREEN
$   Endif
$
$HASTA_LA_VISTA_BABY:
$   EXIT

$SHOW_INFO:
$   !
$   ! Get the media name of the disk device
$   !
$   devtype_name = F$getdvi("''disk_name'", "media_name")
$
$   !
$   ! Get the name of the device
$   !
$!   volnam = F$getdvi("''disk_name'", "logvolnam")
$   If F$getdvi("''disk_name'", "shdw_member")
$   Then
$	If .Not. F$getdvi("''disk_name'", "shdw_master") Then Return
$   Else
$	If .Not. F$getdvi("''disk_name'", "mnt") Then Return
$   Endif
$
$   volnam = F$edit(disk_name, "upcase")
$
$   !
$   ! Get the maximum and free blocks on the disk
$   !
$   max_b = F$getdvi("''disk_name'", "maxblock")
$   fre_b = F$getdvi("''disk_name'", "freeblocks")
$
$   !
$   ! Get the number of blocks used and percent full for the disk
$   !
$   usd_b = max_b - fre_b
$   percent = (usd_b * 100) / max_b
$   deci = ((usd_b * 1000) / max_b) - (percent * 10)
$   show_percent = "''percent'.''deci'"
$
$   !
$   ! Adjust the total's count for the last report line
$   !
$   max_b_total = max_b_total + max_b
$   fre_b_total = fre_b_total + fre_b
$   usd_b_total = usd_b_total + usd_b
$
$   !
$   ! Get the number of megs for the disk
$   !
$   megs = ((max_b / 100) * 512) / 10000
$   max_megs = max_megs + megs
$
$   !
$   ! Display information on the disk
$   !
$   attribute = "["
$
$   If percent .Ge. 80 Then attribute = attribute + at_noti
$   If percent .Ge. 95 Then attribute = attribute + at_warn
$   If percent .Ge. 98 Then attribute = attribute + at_seve - at_warn
$
$   If b_space .Nes. ""
$   Then
$	If F$extract(0, 1, b_space) .Eqs. ">"
$	Then
$	    attribute = "["
$	    b_space1 = F$element(1, ">", b_space)
$	    If F$integer(fre_b) .Ge. b_space1 Then attribute = attribute + ";33;40"
$	Endif
$
$	If F$extract(0, 1, b_space) .Eqs. "<"
$	Then
$	    attribute = "["
$	    b_space1 = F$element(1, "<", b_space)
$	    If F$integer(fre_b) .Le. b_space1 Then attribute = attribute + ";36;40"
$	Endif
$   Endif
$
$   attribute = attribute + "m" - ";"
$
$   If megs .Gt. 0
$   Then
$	srt_cnt = srt_cnt + 1		! update the sort counter.
$
$	If F$locate("/NOC", qualifiers) .Nes. F$length(qualifiers)
$	Then
$	    sort_'srt_cnt' = attribute + F$fao(mask2, volnam, label, show_percent, -
		max_b, usd_b, fre_b, megs, devtype_name) + "[m"
$	Else
$	    last_fre = F$trnlnm("ds_''volnam'")
$	    If last_fre .Eqs. ""
$	    Then
$		diff = 0
$	    Else
$		diff = fre_b - F$integer(last_fre)
$	    Endif
$
$	    dif_b_total = dif_b_total + diff
$
$	    sort_'srt_cnt' = attribute + F$fao(mask2, volnam, label, show_percent, -
		max_b, fre_b, diff, megs, devtype_name) + "[m"
$	Endif
$
$'n'	Write Sys$output sort_'srt_cnt'
$   Else
$	blks = (max_b * 512) / 1000
$	megs = ".''blks'"
$	megs_len = F$length(megs)
$	fill_len = 5 - megs_len
$	If megs_len .Lt. 5 Then megs = F$fao("!''fill_len'AS!AS", "     ", megs)
$
$	If F$locate("/NOC", qualifiers) .Nes. F$length(qualifiers)
$	Then
$	    Write Sys$output attribute, F$fao(mask3, volnam, label, show_percent, -
		max_b, usd_b, fre_b, megs, devtype_name), "[m"
$	Else
$	    last_fre = F$trnlnm("ds_''volnam'")
$	    If last_fre .Eqs. ""
$	    Then
$		diff = 0
$	    Else
$		diff = fre_b - F$integer(last_fre)
$	    Endif
$
$	    dif_b_total = dif_b_total + diff
$
$	    Write Sys$output attribute, F$fao(mask3, volnam, label, show_percent, -
		max_b, fre_b, diff, megs, devtype_name), "[m"
$	Endif
$   Endif
$
$   Define/proc/nolog ds_'volnam' 'fre_b'
$Return

$SHOW_TOTAL:
$'s'	Gosub SORT_THE_LIST
$
$   !
$   ! reduce the size of the total blocks...  They could get quit large.
$   !
$   percent = (usd_b_total * 100) / max_b_total
$!   deci = ((usd_b_total * 1000) / max_b_total) - (percent * 10)
$!   show_percent = "''percent'.''deci'"
$   show_percent = "''percent'"
$
$   volnam = "Total"
$   label = ""
$   Write Sys$output "[K"
$   If F$locate("/NOC", qualifiers) .Nes. F$length(qualifiers)
$   Then
$	Write Sys$output F$fao(mask2, volnam, label, show_percent, max_b_total, usd_b_total, -
	    fre_b_total, max_megs, "")
$   Else
$	Write Sys$output F$fao(mask2, volnam, label, show_percent, max_b_total, fre_b_total, -
	    dif_b_total, max_megs, "")
$   Endif
$Return

$SORT_THE_LIST:
$   element = 2
$
$   If sort_field .eqs. "MAX" then element = 3
$   If sort_field .eqs. "MEG" then element = 6
$   If sort_field .eqs. "FRE" then element = 4
$   If sort_field .eqs. "VOL" then element = 1
$   If sort_field .eqs. "%" then element = 2
$
$	if srt_cnt .eq. 2
$	then
$	    d = 1
$	    goto GO_ANYWAY
$	endif
$
$	d = 1
$15:	d = d * 2
$	If d .Lt. srt_cnt Then Goto 15
$17:	d = (d - 1) / 2
$	If d .Eq. 0 Then Goto 1000
$GO_ANYWAY:
$	it = srt_cnt - d
$	i = 1
$DO_LOOP:
$	    j = i
$16:	    l = j + d
$	    if sort_field .eqs. "VOL"
$	    then
$		test1 = sort_'l' - F$element(element - 1, "m", sort_'l')
$		test2 = sort_'j' - F$element(element - 1, "m", sort_'j')
$
$		test = ".Lts."
$	    else
$		test1 = F$integer(F$element(element, " ", F$edit(sort_'l', "compress")) - "%" - ".")
$		test2 = F$integer(F$element(element, " ", F$edit(sort_'j', "compress")) - "%" - ".")
$
$		if test1 .eq. 1000 then test1 = 9999
$		if test2 .eq. 1000 then test2 = 9999
$
$		test = ".Lt."
$	    endif
$
$	If .Not. (test1 'test' test2) Then Goto END_IF
$		te = sort_'l'
$		sort_'l' = sort_'j'
$		sort_'j' = te
$		j = j - d
$		If j .Gt. 0 Then Goto 16
$	    END_IF:
$	If i .Eq. it Then Goto 17
$
$	i = i + 1
$	Goto DO_LOOP
$
$	Goto 17
$
$1000:
$   cntr = 0
$LIST_LOOP1:
$   If cntr .Eq.srt_cnt Then Goto EXIT_LIST_LOOP1
$
$   cntr = cntr + 1
$   Write Sys$output sort_'cntr'
$!   show sym sort_'cntr'
$
$   Goto LIST_LOOP1
$
$EXIT_LIST_LOOP1:
$Return

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
