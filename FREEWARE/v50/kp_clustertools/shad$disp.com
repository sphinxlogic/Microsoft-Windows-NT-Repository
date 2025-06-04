$	old_verify = 'f$verify(f$trnlnm("SHAD$DISP_verify")+0)'
$!----------------------------------------------------------------------------
$! SHAD$DISP.COM, abbreviated version of SHADOW$DISPLAY.COM, taking less
$! on-screen real estate
$!
$! Display, in color-coded style, all the host-based volume shadowing
$! shadow sets, highlighting any anomalies (such as lack of redundancy
$! or full copies in progress) with the judicious use of color.
$!
$! If it is desired that only particular shadow sets be displayed, P1 should
$! contain a list of shadow set device names or volume labels, separated by
$! commas.  If it is desired that all the shadow sets _except_ a given set be
$! displayed, P1 should contain the exclusion list, preceded by a dash "-".
$!
$! If a continuous display (updating in place) is desired, specify "CONTINUOUS"
$! as the P2 parameter.
$!
$	multi_site = "FALSE"
$! This version optionally supports a multi-site cluster with disk allocation
$! classes of $1$ at one site and $2$ at the other site.  Shadowsets without
$! cross-site shadowing protection are then flagged as unprotected, even if
$! they have two members within a single site.  To enable this behavior,
$! to support a disaster-tolerant cluster, uncomment this line:
$!	multi_site = "TRUE"
$!
$!						V1.0-001, 7/96 & 7/97
$!	Based on my example RAID$DISPLAY.COM from DEC's StorageWorks RAID
$!	Software for OpenVMS, V2.2 (which I also wrote)
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$!----------------------------------------------------------------------------
$!
$! Start by defining symbols for the escape sequences we'll need to display
$! things.
$!
$ ESC[0,8]=27
$ CSI[0,8]=155
$ DCS[0,8]=144
$! Reset graphics rendition
$ SGR_RESET = CSI + "0m"
$! Bold, Blink, Underline, Reverse Video
$ SGR_BOLD = CSI + "1m"
$ SGR_UNDERLINE = CSI + "4m"
$ SGR_BLINK = CSI + "5m"
$ SGR_REVERSE = CSI + "7m"
$ SGR_BOLD_OFF = CSI + "22m"
$ SGR_UNDERLINE_OFF = CSI + "24m"
$ SGR_BLINK_OFF = CSI + "25m"
$ SGR_REVERSE_OFF = CSI + "27m"
$! Foreground color
$ SGR_FG_Black = CSI + "30m"
$ SGR_FG_Red = CSI + "31m"
$ SGR_FG_Green = CSI + "32m"
$ SGR_FG_Yellow = CSI + "33m"
$ SGR_FG_Blue = CSI + "34m"
$ SGR_FG_Magenta = CSI + "35m"
$ SGR_FG_Cyan = CSI + "36m"
$ SGR_FG_White = CSI + "37m"
$ SGR_FG_DEFAULT = CSI + "39m"
$! Background color
$ SGR_BG_Black = CSI + "40m"
$ SGR_BG_Red = CSI + "41m"
$ SGR_BG_Green = CSI + "42m"
$ SGR_BG_Yellow = CSI + "43m"
$ SGR_BG_Blue = CSI + "44m"
$ SGR_BG_Magenta = CSI + "45m"
$ SGR_BG_Cyan = CSI + "46m"
$ SGR_BG_White = CSI + "47m"
$ SGR_BG_DEFAULT = CSI + "49m"
$ SGR_RESET_COLOR = SGR_FG_DEFAULT + SGR_BG_DEFAULT
$!
$ ED_ALL = CSI + "2J"	!Erase display, all of it
$ ED_EOS = CSI + "J"	!Erase display, from cursor to end of screen
$ EL_EOL = CSI + "K"	!Erase from cursor position to end of line
$ HOME = CSI + "H"
$ LD_ON = ESC + "(0"	!LINE_DRAWING_ON
$ LD_OFF = ESC + "(B"	!LINE_DRAWING_OFF
$! SE_CORNER = "''LD_ON'j''LD_OFF'"
$! NE_CORNER = "''LD_ON'k''LD_OFF'"
$! NW_CORNER = "''LD_ON'l''LD_OFF'"
$! SW_CORNER = "''LD_ON'm''LD_OFF'"
$! CROSS = "''LD_ON'n''LD_OFF'"
$! SCAN1 = "''LD_ON'o''LD_OFF'"
$! SCAN3 = "''LD_ON'p''LD_OFF'"
$! SCAN5 = "''LD_ON'q''LD_OFF'"
$! HORIZ_LINE = SCAN5
$! SCAN7 = "''LD_ON'r''LD_OFF'"
$! SCAN9 = "''LD_ON's''LD_OFF'"
$! S1 = "o"
$! S3 = "p"
$! S5 = "q"
$! S7 = "r"
$! S9 = "s"
$! W_TEE = "''LD_ON't''LD_OFF'"
$! E_TEE = "''LD_ON'u''LD_OFF'"
$! S_TEE = "''LD_ON'v''LD_OFF'"
$! N_TEE = "''LD_ON'w''LD_OFF'"
$ VL = "x"
$ VERT_LINE = LD_ON + VL + LD_OFF
$! SPACE = "''LD_ON'_''LD_OFF'"
$ SP = " "
$! DOT = "''LD_ON'~''LD_OFF'"
$!-----------------------
$ wo :== write sys$output
$ clr = "''wo' ED_ALL"
$! clr
$! Create strings of repeated characters so we can create patterns of
$! arbitrary length using them.
$	call REPEAT SP
$! P1 is the parameter to create a repeated string from
$ REPEAT: SUBROUTINE
$ 'P1's='P1'+'P1'+'P1'+'P1'+'P1'+'P1'+'P1'+'P1'		!8
$ 'P1's='P1's+'P1's+'P1's+'P1's+'P1's+'P1's+'P1's+'P1's	!64
$ 'P1's=='P1's+'P1's+'P1's+'P1's				!256
$ ENDSUBROUTINE
$! End of escape-sequence setup work.
$!
$!----------------------------------------------------------------------------
$!
$! P1 specifies a list of shadow set device names or volume labels to be
$! displayed (or excluded, if the list is preceded by a dash)
$	inclusion_list = ""
$	exclusion_list = ""
$	if f$extract(0,1,P1) .eqs. "-"
$	then
$		P1 = P1 - "-"
$		if P1 .nes. "" then exclusion_list = "," + P1 + ","
$ show symbol exclusion_list	!!!
$	else
$		if P1 .nes. "" then inclusion_list = "," + P1 + ","
$ show symbol inclusion_list	!!!
$	endif
$	i_len = f$length(inclusion_list)
$	e_len = f$length(exclusion_list)
$ proc = f$element(0,";",f$environment("PROCEDURE"))
$ proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$!
$ WAIT_FOR_SHADOWSETS:	!Come back here if we ever run out of shadowsets to display
$!!!!	@SYS$EXAMPLES:SHADOW$CONFIG GLOBAL_SYMBOLS	!Gather initial shadowset info
$	@'proc_dir'SHADOW$CONFIG GLOBAL_SYMBOLS	!Gather initial shadowset info
$! See if there's anything to display at the moment
$	if SHADOW$CONFIG_SET_COUNT .EQ. 0
$	then
$		if f$extract(0,4,P2) .nes. "CONT" then goto EXIT	!or CLEANUP
$!		wait 00:00:02
$		wait 00:00:15
$		goto WAIT_FOR_SHADOWSETS	!Nothing to show yet
$	endif
$	on control_y then goto CLEANUP	!Reset screen width on CtlY starting now
$!!!	set term/wid='new_width'
$!!!! Note that this also clears the screen for us initially.
$! ***
$!     We may need to also count the number of shadowsets to be displayed and
$!     set an appropriate _page length_ as well...
$! ***
$	goto jump_start	!Skip a redundant call to @SHADOW$CONFIG
$!-----------------------------------------------------------------------------
$!
$! Main program loop.  Come back here each time we refresh the display.
$!
$!-----------------------------------------------------------------------------
$ MAIN_LOOP:
$!
$!***
$! Eventually, we'll want to do this:
$!
$!	@SYS$EXAMPLES:SHADOW$CONFIG LOGICAL_NAMES
$!				   !Is done in another process, with logical
$!				   !names defined in the GROUP table...
$! Move logical-name values from the group table into global symbols.  If the
$! data has not changed (as reflected by the timestamp), leave the existing
$! values alone.
$!
$! Somehow try to get the data in a cohesive chunk (using a file lock as
$! the update/read lock?).....
$!
$! For now, just use global symbols in the context of a single process, instead
$! of trying to use logical names in the group or system logical name table
$! and using separate data-gathering and display procedures.
$!***
$!
$!!!!	@sys$examples:SHADOW$CONFIG GLOBAL_SYMBOLS
$	@'proc_dir'SHADOW$CONFIG GLOBAL_SYMBOLS
$!
$ JUMP_START:	!Enter here on the first entry to the main loop
$!!	wo HOME,EL_EOL		!Note that because of the CR/LF at the end, this
$!!!			!actually leaves the cursor at the start of the 2nd row
$	wo ""
$	wo ""
$	wo ""
$	wo ""
$	show time
$!
$ SHDWSET_LOOP:
$	i = 0		!Shadowset index
$ DISPLAY_LOOP:
$	i = i + 1	!Look at the next shadowset out there
$	if i .gt. SHADOW$CONFIG_SET_COUNT then goto DISPLAY_END    !No more -->
$	lbl = SHADOW$CONFIG_SET_'i'_LABEL	!Shadowset volume label
$	dev = SHADOW$CONFIG_SET_'i'_DEVICE_NAME	!Shadowset device name
$	if i_len .gt. 0
$	then 	!Skip this one if we aren't including it
	    if f$locate(","+lbl+",",inclusion_list) .eq. i_len -
	 .and. f$locate(","+dev+",",inclusion_list) .eq. i_len then -
		goto DISPLAY_LOOP
$	endif
$	if e_len .gt. 0
$	then 	!Skip this one if we are excluding it
	    if f$locate(","+lbl+",",exclusion_list) .ne. e_len -
	  .or. f$locate(","+dev+",",exclusion_list) .ne. e_len then -
		goto DISPLAY_LOOP
$	endif
$	memcnt = SHADOW$CONFIG_SET_'i'_MEMBER_COUNT	!Number of members
$!! Skip empty shadow sets
$	if memcnt .le. 0 then goto display_loop
$	mc = memcnt
$!!	if mc .le. 0 then mc = 1  !Display empty shadowsets (if we ever were to
$!!!				  !encounter one) with the width of 1 disk
$	j = memcnt
$	valid_members = 0
$	valid_members_site1 = 0
$	valid_members_site2 = 0
$	eventual_valid_members = 0
$	eventual_valid_members_site1 = 0
$	eventual_valid_members_site2 = 0
$	merge_in_progress = "FALSE"
$ VALID_DEPTH_LOOP:
$		state = SHADOW$CONFIG_SET_'i'_MEMBER_'j'_STATE
$		devnam = SHADOW$CONFIG_SET_'i'_MEMBER_'j'_DEVICE_NAME
$		if state .eqs. "ShadowMember" .or. -	!Count members which
		   state .eqs. "MergeCopying"		!are considered to
$		then					!contain valid data now	
$		    valid_members = valid_members + 1
$! For now, use allocation class to indicate which site a disk is located at.
$! Eventually, when we go back to a single allocation class, we'll have to use
$! some other algorithm.
$		    if f$locate("$1$",devnam) .ne. f$length(devnam) then -
			valid_members_site1 = valid_members_site1 + 1
$		    if f$locate("$2$",devnam) .ne. f$length(devnam) then -
			valid_members_site2 = valid_members_site2 + 1
$		endif
$		if state .eqs. "ShadowMember" .or. -	!Count members which
		   state .eqs. "MergeCopying" -		!will eventually
	      .or. state .eqs. "FullCopying"		!contain valid data
$		then
		    eventual_valid_members = eventual_valid_members + 1
$		    if f$locate("$1$",devnam) .ne. f$length(devnam) then -
			eventual_valid_members_site1 = eventual_valid_members_site1 + 1
$		    if f$locate("$2$",devnam) .ne. f$length(devnam) then -
			eventual_valid_members_site2 = eventual_valid_members_site2 + 1
$		endif
$		if state .eqs. "MergeCopying" then -
		    merge_in_progress = "TRUE"	!Merge is in progress
$		j = j - 1
$		if j .gt. 0 then goto VALID_DEPTH_LOOP
$! Remember these values for later use, when we're choosing colors for the
$! shadow-set virtual units
$!
$! Choose a color for the virtual unit
$!	label = "Shadowset ''lbl'"
$!	label = "Shadowset ''dev', volume ''lbl'"
$!	label = "''dev' ''lbl'"
$!	label = "Shadowset ''dev'; volume ''lbl'"
$!	label = "Shadowset ''dev', volume ''lbl'"
$!	label = f$extract(0,23,"''dev' (''lbl')"+SPs)
$	label = f$extract(0,8,dev+SPS) + " " + f$extract(0,14,("("+lbl+")"+SPs))
$! Check for redundancy and also for safety across site failures for BRS
$! configurations (at least 1 copy at each site)
$	if (valid_members_site1 .ge. 1 .and. -
	    valid_members_site2 .ge. 1) .or. -
		((.not. multi_site) .and. (valid_members .ge. 2))
$	then	!Have redundancy (2 members) and site failure protection; safe
$	    VA_COLOR = SGR_BG_Green + SGR_FG_Blue	!All OK
$	    if merge_in_progress then VA_COLOR = SGR_BG_Green + SGR_FG_Black	!White
$	else
$	if (eventual_valid_members_site1 .ge. 1 .and. -
	    eventual_valid_members_site2 .ge. 1) .or. -
		((.not. multi_site) .and. (eventual_valid_members .ge. 2))
$	    then	!Full-copy is in progress to restore redundancy and
$!			!site failure protection
$		VA_COLOR = SGR_BG_Yellow + SGR_FG_Black	!Red	!Some danger, but fix underway
$	    else
$		VA_COLOR = SGR_BG_Red + SGR_FG_Yellow + SGR_BLINK	!Danger!!!
$	    endif
$	endif
$! Start building the output line
$	line = VA_COLOR + SGR_UNDERLINE + label + SGR_RESET_COLOR + SGR_BLINK_OFF
$!
$	j = 0
$ DRAW_MEMBER_LOOP:
$	j = j + 1
$! Choose the color for this member disk
$	if j .le. memcnt
$	then
$	    state = SHADOW$CONFIG_SET_'i'_MEMBER_'j'_STATE
$	else
$	    state = "MISSING"	!Handle the case of a missing member
$	endif
$	if state .eqs. "ShadowMember" then -
	    MD_COLOR = SGR_BG_Green + SGR_FG_Blue
$	if state .eqs. "MergeCopying" then -
	    MD_COLOR = SGR_BG_Green + SGR_FG_Black	!White
$	if state .eqs. "FullCopying" then -
	    MD_COLOR = SGR_BG_Yellow + SGR_FG_Red
$	if state .eqs. "MISSING" then -
	    MD_COLOR = SGR_BG_Black + SGR_FG_Yellow
$!
$	if j .le. memcnt
$	then
$	    devnam = f$ext(0,15,SHADOW$CONFIG_SET_'i'_MEMBER_'j'_DEVICE_NAME)
$	else
$!!!!@@@@@!!!!! Get the device name of the missing member from logical names
$!!!!@@@@@!!!!! or from recorded data a la SHADOW$TRACK.COM
$	    devnam = "EMPTY"	!Handle the case of a missing member
$	endif
$	line = line + " " + -
    MD_COLOR + "[" + f$extract(0,11,devnam+SPs) + "]" + SGR_RESET_COLOR
$	if state .nes. "ShadowMember"
$	then
$		state_abbr = state - "ing"
$		if f$locate("Merge",state) .ne. f$length(state) then -
			state_abbr = state_abbr - "Copy"
$		line = line + "<" + state_abbr + ">"
$	endif
$!
$	if j .lt. SHADOW$CONFIG_SET_'i'_MEMBER_COUNT then goto DRAW_MEMBER_LOOP
$!	line = line + SGR_RESET + EL_EOL
$!	line = line + EL_EOL
$! Now we're ready to draw
$!	wo line
$	wo line,SGR_UNDERLINE_OFF,EL_EOL
$!
$!!! wo EL_EOL	!Visual separation between arrays
$!
$	goto DISPLAY_LOOP
$!
$ DISPLAY_END:
$! We've displayed all the shadowsets
$ wo ED_EOS	!Erase any leftovers below this point
$	if f$extract(0,4,P2) .nes. "CONT" then goto EXIT	!or CLEANUP
$!	wait 00:00:02
$	wait 00:01:00
$! See if there's anything to display at the moment
$	if SHADOW$CONFIG_SET_COUNT .EQ. 0
$	then
$		goto WAIT_FOR_SHADOWSETS	!Nothing to show yet
$	else
$		goto MAIN_LOOP
$	endif
$ CLEANUP:
$	wo SGR_RESET
$ EXIT:
$	exit 1+0*f$verify(old_verify)
