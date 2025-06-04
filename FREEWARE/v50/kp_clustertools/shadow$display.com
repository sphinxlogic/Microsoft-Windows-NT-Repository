$!----------------------------------------------------------------------------
$! SHADOW$DISPLAY.COM
$!
$! Display, in semi-graphical format, all the host-based volume shadowing
$! shadow sets, highlighting any anomalies (such as lack of redundancy
$! or full copies in progress) with the judicious use of color.
$!
$! If it is desired that only particular shadow sets be displayed, P1 should
$! contain a list of shadow set device names or volume labels, separated by
$! commas.
$!
$! If a continuous display (updating in place) is desired, specify "CONTINUOUS"
$! as the P2 parameter.
$!								V0.0-000, 7/96
$!	Based on the example RAID$DISPLAY.COM from DEC's StorageWorks RAID
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
$!
$ ED_ALL = CSI + "2J"	!Erase display, all of it
$ ED_EOS = CSI + "J"	!Erase display, from cursor to end of screen
$ EL_EOL = CSI + "K"	!Erase from cursor position to end of line
$ HOME = CSI + "H"
$ LINE_DRAWING_ON = ESC + "(0"
$ LD_ON = LINE_DRAWING_ON
$ LINE_DRAWING_OFF = ESC + "(B"
$ LD_OFF = LINE_DRAWING_OFF
$ SE_CORNER = "''LD_ON'j''LD_OFF'"
$ NE_CORNER = "''LD_ON'k''LD_OFF'"
$ NW_CORNER = "''LD_ON'l''LD_OFF'"
$ SW_CORNER = "''LD_ON'm''LD_OFF'"
$ CROSS = "''LD_ON'n''LD_OFF'"
$ SCAN1 = "''LD_ON'o''LD_OFF'"
$ SCAN3 = "''LD_ON'p''LD_OFF'"
$ SCAN5 = "''LD_ON'q''LD_OFF'"
$ HORIZ_LINE = SCAN5
$ SCAN7 = "''LD_ON'r''LD_OFF'"
$ SCAN9 = "''LD_ON's''LD_OFF'"
$ S1 = "o"
$ S3 = "p"
$ S5 = "q"
$ S7 = "r"
$ S9 = "s"
$ W_TEE = "''LD_ON't''LD_OFF'"
$ E_TEE = "''LD_ON'u''LD_OFF'"
$ S_TEE = "''LD_ON'v''LD_OFF'"
$ N_TEE = "''LD_ON'w''LD_OFF'"
$ VL = "x"
$ VERT_LINE = LD_ON + VL + LD_OFF
$ SPACE = "''LD_ON'_''LD_OFF'"
$ SP = " "
$ DOT = "''LD_ON'~''LD_OFF'"
$!-----------------------
$ wo :== write sys$output
$ clr = "''wo' ED_ALL"
$! clr
$! Create strings of repeated characters so we can create patterns of
$! arbitrary length using them.
$	call REPEAT S1
$	call REPEAT S3
$	call REPEAT S5
$	call REPEAT S7
$	call REPEAT S9
$	call REPEAT SP
$! P1 is the parameter to create a repeated string from
$ REPEAT: SUBROUTINE
$ 'P1's='P1'+'P1'+'P1'+'P1'+'P1'+'P1'+'P1'+'P1'		!8
$ 'P1's='P1's+'P1's+'P1's+'P1's+'P1's+'P1's+'P1's+'P1's	!64
$ 'P1's=='P1's+'P1's+'P1's+'P1's				!256
$ ENDSUBROUTINE
$! End of escape-sequence setup work.
$!
$! P1 specifies a list of shadow set device names or volume labels to be
$! displayed.
$!
$! If P1 is non-blank, to avoid mismatches when one shadowset name is a substring
$! of another shadowset name, add commas surrounding P1
$	if P1 .nes. "" then P1 = "," + P1 + ","	!Add leading & trailing commas
$	P1_length = f$length(P1)
$!
$! Set various parameter values which remain unchanged for the duration
$!
$	md_depth = 5	!Depth of an ordinary member disk
$	md_width = 16	!Width of an ordinary member disk
$! Save the old terminal width
$	old_width = f$getdvi("sys$output","DEVBUFSIZ")
$ old_verify = 'f$verify(f$trnlnm("SHADOW$DISPLAY_verify"))'	!!!!
$ proc = f$element(0,";",f$environment("PROCEDURE"))
$ proc_dir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$!
$ WAIT_FOR_SHADOWSETS:	!Come back here if we ever run out of shadowsets to display
$!
$! First, find out the maximum number of members in any shadowset
$! so we can set an appropriate display width
$!!!!	@SYS$EXAMPLES:SHADOW$CONFIG GLOBAL_SYMBOLS	!Gather initial shadowset info
$	@'proc_dir'SHADOW$CONFIG GLOBAL_SYMBOLS	!Gather initial shadowset info
$! See if there's anything to display at the moment
$	if SHADOW$CONFIG_SET_COUNT .EQ. 0
$	then
$		if f$extract(0,4,P2) .nes. "CONT" then goto EXIT	!or CLEANUP
$		wait 00:00:02
$		goto WAIT_FOR_SHADOWSETS	!Nothing to show yet
$	endif
$! At this point in time (1996), host-based volume shadowing supports a
$! maximum of 3 members in a shadow set.  This code was left generic enough
$! that it could support shadowsets of more than 3 memebers in the future.
$	call FIND_MAX_MEMBERS	!Determine the number of members in the largest
$!				!shadowset
$!
$! Determine the number of members in the largest shadowset
$!
$ FIND_MAX_MEMBERS: SUBROUTINE
$	max_members == 0
$! First, size the shadowsets
$	i = 0		!Shadowset index
$	if SHADOW$CONFIG_SET_COUNT .GT. 0
$	then
$ COUNT_SHADOWSET_LOOP:
$	    i = i + 1	!Look at the next shadowset out there
$	    if P1_length .gt. 0
$	    then	!Skip it if we aren't displaying it
		if f$locate(","+SHADOW$CONFIG_SET_'i'_LABEL+",",P1) -
			.eq. P1_length -
	     .and. f$locate(","+SHADOW$CONFIG_SET_'i'_DEVICE_NAME+",",P1) -
			.eq. P1_length then -
		    goto COUNT_SHADOWSET_NEXT
$	    endif
$	    if SHADOW$CONFIG_SET_'i'_MEMBER_COUNT .gt. max_members then -
		max_members == SHADOW$CONFIG_SET_'i'_MEMBER_COUNT
$ COUNT_SHADOWSET_NEXT:
$	    if i .lt. SHADOW$CONFIG_SET_COUNT then goto COUNT_SHADOWSET_LOOP
$	endif
$ ENDSUBROUTINE
$!
$! Use the info to set an appropriate screen width (80 or 132 columns)
$	if max_members .gt. 4
$	then
$		new_width = 132
$	else
$		new_width = 80
$	endif
$	on control_y then goto CLEANUP	!Reset screen width on CtlY starting now
$	set term/wid='new_width'
$! Note that this also clears the screen for us initially.
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
$	wo HOME,EL_EOL		!Note that because of the CR/LF at the end, this
$!			!actually leaves the cursor at the start of the 2nd row
$!
$ SHDWSET_LOOP:
$	i = 0		!Shadowset index
$ DISPLAY_LOOP:
$	i = i + 1	!Look at the next shadowset out there
$	if i .gt. SHADOW$CONFIG_SET_COUNT then goto DISPLAY_END    !No more -->
$	lbl = SHADOW$CONFIG_SET_'i'_LABEL	!Shadowset volume label
$	dev = SHADOW$CONFIG_SET_'i'_DEVICE_NAME	!Shadowset device name
$	if P1_length .gt. 0
$	then 	!Skip this one if we aren't displaying it
	    if f$locate(","+lbl+",",P1) .eq. P1_length -
	 .and. f$locate(","+dev+",",P1) .eq. P1_length then -
		goto DISPLAY_LOOP
$	endif
$	memcnt = SHADOW$CONFIG_SET_'i'_MEMBER_COUNT	!Number of members
$! If the shadowset is empty, and it's not mounted, don't display it
$	if memcnt .le. 0 .and. .not. f$getdvi(dev,"MNT") then goto DISPLAY_LOOP
$! At this point in time (1996), host-based volume shadowing supports a
$! maximum of 3 members in a shadow set.  This code was left generic enough
$! that it could support shadowsets of more than 3 memebers in the future.
$!
$! If a shadowset were to have too many members, we couldn't display it due to
$! DCL's internal limits on symbol length.  Print a message in these cases.
$	if memcnt .gt. 5
$	then
$	    wo "Shadowset ''dev', volume label ''lbl', has ''memcnt' members,"
$	    wo "which is too many for this procedure to display."
$	    wo ""
$	    goto DISPLAY_LOOP
$	endif
$!
$! Calculate the width of the enclosing disk (the DSA virtual disk device)
$	md_wid = md_width
$	mc = memcnt
$	if mc .le. 0 then mc = 1  !Display empty shadowsets (if we ever were to
$!				  !encounter one) with the width of 1 disk
$	va_wid = 3+(mc*(md_wid+1))		!Virtual array display width
$!
$! Calculate the height of the member disks.
$	md_dep = md_depth
$	j = memcnt
$	valid_members = 0
$	eventual_valid_members = 0
$	merge_in_progress = "FALSE"
$ VALID_DEPTH_LOOP:
$		state = SHADOW$CONFIG_SET_'i'_MEMBER_'j'_STATE
$		if state .eqs. "ShadowMember" .or. -	!Count members which
		   state .eqs. "MergeCopying" then -	!are considered to
		    valid_members = valid_members + 1	!contain valid data now
$		if state .eqs. "ShadowMember" .or. -	!Count members which
		   state .eqs. "MergeCopying" -		!will in eventually
	      .or. state .eqs. "FullCopying" then -	!contain valid data
		    eventual_valid_members = eventual_valid_members + 1
$		if state .eqs. "MergeCopying" then -
		    merge_in_progress = "TRUE"	!Merge is in progress
$		j = j - 1
$		if j .gt. 0 then goto VALID_DEPTH_LOOP
$! Remember these values for later use, when we're choosing colors for the
$! shadow-set virtual units
$!!!!	valid_members_'i' = valid_members
$!!!!	eventual_valid_members_'i' = eventual_valid_members
$! It will take 'md_dep' lines to display the member [virtual] disk(s).
$! Find out how many "interior" lines will be needed
$	md_int = md_dep - 4	!Total depth minus top 3 and bottom 1
$!
$! Choose a color for the enclosing disk (virtual unit)
$!!!!	state = SHADOW$CONFIG_SET_'i'_STATE
$!	label = "Shadowset ''lbl'"
$!	label = "Shadowset ''dev', volume ''lbl'"
$!	label = "''dev' ''lbl'"
$!	label = "Shadowset ''dev'; volume ''lbl'"
$!	label = "Shadowset ''dev', volume ''lbl'"
$	label = "''dev', ''lbl'"
$!!!!	if state .eqs. "NORMAL" .and. min_valid_depth .ge. 2
$	if valid_members .ge. 2
$	then	!Have redundancy; safe
$!!!@@@@!!! Also check for safety across site failures for BRS configurations
$	    VA_COLOR = SGR_BG_Green + SGR_FG_Blue	!All OK
$	    if merge_in_progress then VA_COLOR = SGR_BG_Green + SGR_FG_White
$	else
$	    if eventual_valid_members .ge. 2
$	    then	!Full-copy is in progress to restore redundancy
$		VA_COLOR = SGR_BG_Yellow + SGR_FG_Red	!Some danger, but fix underway
$	    else
$		VA_COLOR = SGR_BG_Red + SGR_FG_Yellow	!Danger!!!
$	    endif
$	endif
$	call container -
			'va_wid' -	!Width of the disks
			5 -		!Height of each disk (3 top, 1 bottom, + "n" rows of dead space inside)
			1 -		!Number of disks in a vertical row
			"''label'" -	!Label for virtual disk
			VA_COLOR	!Color for virtual disk
$! Back edge of top of enclosing disk
$	va_back_top = CONT_LINE_1 + SGR_RESET
$! Center of top of enclosing disk
$	va_center_top = CONT_LINE_2 + SGR_RESET
$! Front edge of top of enclosing disk
$	va_front_top = CONT_LINE_3 + SGR_RESET
$! Dead space inside enclosing disk
$	va_dead_space = CONT_LINE_4 + SGR_RESET
$! Bottom of enclosing disk
$	va_bottom = CONT_LINE_5 + SGR_RESET
$!
$! Start building the pieces of the enclosed disks' pictures
$!
$! Back edges of enclosed disks
$	md_back_top = VA_COLOR + VERT_LINE + SP
$! Centers of tops of enclosed disks
$	md_center_top = VA_COLOR + VERT_LINE + SP
$! Front edges of enclosed disks
$	md_front_top = VA_COLOR + VERT_LINE + SP
$! Empty space inside enclosed disks
$!!	md_dead_space = VA_COLOR + VERT_LINE + SP
$! Interior portions of the enclosed disks
$	n = 0
$ INIT_INTERIOR_LOOP:
$	n = n + 1
$	md_interior_'n' = VA_COLOR + VERT_LINE + SP
$	if n .lt. md_int then goto INIT_INTERIOR_LOOP
$! Bottoms of enclosed disks
$	md_bottom = VA_COLOR + VERT_LINE + SP
$!
$! Put in the inner portions for each of the enclosed disks
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
	    MD_COLOR = SGR_BG_Green + SGR_FG_White
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
$	    devnam = "EMPTY"	!Handle the case of a missing member
$	endif
$	call container -
			'md_wid' -	!Width of member disk (adjusted for 0+1)
			5 -		!Height of each disk (3 top, 1 bottom, + "n" rows of dead space inside)
			1 -		!Number of disks in a vertical row
			"''devnam'" -	!Label for virtual disk
			MD_COLOR	!Color for virtual disk
$!
$! Back edge of shadowset member disk
$	md_back_top = md_back_top + CONT_LINE_1 + VA_COLOR + SP
$! Center of top of shadowset member disk
$	md_center_top = md_center_top + CONT_LINE_2 + VA_COLOR + SP
$! Front edge of shadowset member disk
$	md_front_top = md_front_top + CONT_LINE_3 + VA_COLOR + SP
$! Middles will be different for RAID 0+1 arrays as opposed to RAID 5 and 0...
$! Dead space inside RAID array member disk
$!!	md_dead_space = md_dead_space + CONT_LINE_4 + VA_COLOR + SP
$! Bottom of RAID array member disk
$	md_bottom = md_bottom + CONT_LINE_5 + VA_COLOR + SP
$!
$! Empty space inside enclosed disk
$!!!	md_dead_space = md_dead_space + CONT_LINE_4 + VA_COLOR + SP
$	n = 0	!Interior not yet filled; fill 1 line below
$!
$! Fill up any leftover space with "emptiness" (blank lines inside container)
$ FILL_INTERIOR_LOOP:
$	n = n + 1
$	md_interior_'n' = md_interior_'n' + -
		MD_COLOR + LD_ON + VL + -
		f$extract(0,md_wid-2,SPs) + -
		VL + LD_OFF + VA_COLOR + SP
$	if n .lt. md_int then goto FILL_INTERIOR_LOOP
$!
$	if j .lt. SHADOW$CONFIG_SET_'i'_MEMBER_COUNT then goto DRAW_MEMBER_LOOP
$!
$! Finish off the enclosed-disk display lines
$!
$! Back edges of enclosed disks
$	md_back_top = md_back_top + VERT_LINE + SGR_RESET
$! Centers of tops of enclosed disks
$	md_center_top = md_center_top + VERT_LINE + SGR_RESET
$! Front edges of enclosed disks
$	md_front_top = md_front_top + VERT_LINE + SGR_RESET
$! Empty space inside enclosed disks
$!!!	md_dead_space = md_dead_space + VERT_LINE + SGR_RESET
$	n = 0
$ DISPLAY_INTERIOR_LOOP:
$	n = n + 1
$	md_interior_'n' = md_interior_'n' + VERT_LINE + SGR_RESET
$	if n .lt. md_int then goto DISPLAY_INTERIOR_LOOP
$! Bottoms of enclosed disks
$	md_bottom = md_bottom + VERT_LINE + SGR_RESET
$!
$! Now we're ready to draw
$!
$! Display the top of the virtual unit
$	wo va_back_top,EL_EOL
$	wo va_center_top,EL_EOL
$	wo va_front_top,EL_EOL
$	wo va_dead_space,EL_EOL
$! Display the completed shadowset members' picture
$ wo md_back_top,EL_EOL
$ wo md_center_top,EL_EOL
$ wo md_front_top,EL_EOL
$	n = 0
$ DISPLAY_INTERIOR_LOOP:
$	n = n + 1
$ wo md_interior_'n',EL_EOL
$	if n .lt. md_int then goto DISPLAY_INTERIOR_LOOP
$ wo md_bottom,EL_EOL
$!
$!! wo md_dead_space,EL_EOL
$!! wo md_dead_space,EL_EOL
$ wo va_dead_space,EL_EOL
$ wo va_bottom,EL_EOL
$!
$ wo EL_EOL	!Visual separation between arrays
$!
$	goto DISPLAY_LOOP
$!
$ DISPLAY_END:
$! We've displayed all the shadowsets
$ wo ED_EOS	!Erase any leftovers below this point
$	if f$extract(0,4,P2) .nes. "CONT" then goto EXIT	!or CLEANUP
$	wait 00:00:02
$! See if there's anything to display at the moment
$	if SHADOW$CONFIG_SET_COUNT .EQ. 0
$	then
$		goto WAIT_FOR_SHADOWSETS	!Nothing to show yet
$	else
$		goto MAIN_LOOP
$	endif
$ CLEANUP:
$	wo SGR_RESET
$	if old_width .ne. new_width then set term/width='old_width'
$ EXIT:
$	exit
$!-----------------------------------------------------------------------------
$!
$! Create the code for display of a container
$!
$! Inputs: (parameters)
$!
$!	width of the disks
$!		P1 (integer; minimum width 13)
$!	height of each disk (3 top, 1 bottom, + "n" rows of dead space inside)
$!		P2 (single integer indicating number of rows; minimum of 4)
$!	number of disks to place in a vertical row
$!		P3 (single integer indicating number of disks in the stack)
$!	label(s) for disk(s)
$!		P4 (list of text values, separated by backslashes)
$!	color(s) for disk(s)
$!		P5 (list of text values, separated by backslashes)
$!	number of lines which separate the disks vertically
$!		P6 (integer; minimum 0)
$!
$! Outputs: (global symbols)
$!
$!	Number of lines of output
$!		CONT_LINES
$!	Actual lines of disk output, each in a global symbol of their own
$!		CONT_LINE_n (where "n" varies from 1 to 'CONT_LINES')
$! Note: at the end of each line, the graphics rendition (color) is left
$! in the color of the disk; if material is to be concatenated onto the
$! end, the color must be specified first; or, if the material is to be
$! at the end of a line, the graphics rendition (color) should be reset.
$!
$ CONTAINER:	SUBROUTINE
$	wid = P1				!Width
$	if f$integer(P1) .lt. 13 then wid = 13
$	hgt = P2				!Height
$	if f$integer(P2) .lt. 4 then hgt = 4
$	dskcnt = P3				!Number of disks
$	if f$integer(P3) .lt. 1 then dskcnt = 1
$	labels = P4				!Label text strings
$	colors = P5				!Color specifier strings
$	sep = f$integer(P6)
$	if f$integer(P6) .lt. 0 then sep = 0	!Lines of separation
$!
$! Calculate the total number of lines of output we'll return
$	CONT_LINES == dskcnt*hgt + (dskcnt-1)*sep
$!
$! Calculate the width of the various portions of the elliptical curves for
$! the tops and bottoms of the disk displays
$	sa_wid = 1+(wid/64)	!Width of pieces of ends of ellipse
$	sb_wid = 1+(wid/32)
$	sc_wid = 1+(wid/16)
$	sd_wid = 1+(wid/8)
$	se_wid = wid - 2 - 2*sa_wid - 2*sb_wid - 2*sc_wid - 2*sd_wid !Middle
$! Back edge of top of disk
$	back_top = SP + LD_ON + -
		f$ext(0,sa_wid*f$len(S9),S9s) + -
		f$ext(0,sb_wid*f$len(S7),S7s) + -
		f$ext(0,sc_wid*f$len(S5),S5s) + -
		f$ext(0,sd_wid*f$len(S3),S3s) + -
		f$ext(0,se_wid*f$len(S1),S1s) + -
		f$ext(0,sd_wid*f$len(S3),S3s) + -
		f$ext(0,sc_wid*f$len(S5),S5s) + -
		f$ext(0,sb_wid*f$len(S7),S7s) + -
		f$ext(0,sa_wid*f$len(S9),S9s) + -
		LD_OFF + SP
$! (Center of top of disk is done differently for each disk because the labels
$! vary)
$! Front edge of top of disk
$	front_top = LD_ON + VL + -
		f$ext(0,sa_wid*f$len(S1),S1s) + -
		f$ext(0,sb_wid*f$len(S3),S3s) + -
		f$ext(0,sc_wid*f$len(S5),S5s) + -
		f$ext(0,sd_wid*f$len(S7),S7s) + -
		f$ext(0,se_wid*f$len(S9),S9s) + -
		f$ext(0,sd_wid*f$len(S7),S7s) + -
		f$ext(0,sc_wid*f$len(S5),S5s) + -
		f$ext(0,sb_wid*f$len(S3),S3s) + -
		f$ext(0,sa_wid*f$len(S1),S1s) + -
		VL + LD_OFF
$! Dead space inside disk
$	dead_space = LD_ON + VL + -
		f$ext(0,wid-2,SPs) + VL + LD_OFF
$! Bottom of disk
$	bottom = SP + LD_ON + -
		f$ext(0,sa_wid*f$len(S1),S1s) + -
		f$ext(0,sb_wid*f$len(S3),S3s) + -
		f$ext(0,sc_wid*f$len(S5),S5s) + -
		f$ext(0,sd_wid*f$len(S7),S7s) + -
		f$ext(0,se_wid*f$len(S9),S9s) + -
		f$ext(0,sd_wid*f$len(S7),S7s) + -
		f$ext(0,sc_wid*f$len(S5),S5s) + -
		f$ext(0,sb_wid*f$len(S3),S3s) + -
		f$ext(0,sa_wid*f$len(S1),S1s) + -
		LD_OFF + SP
$!
$! Create the lines for the disks
$	i = 0	!Index for disks
$	j = 0	!index for lines of output
$ DSK_LOOP:	!Loop once for each disk we need to display
$	color = f$element(i,"\",colors)
$! Back edge of top of disk
$	j = j + 1
$	CONT_LINE_'j' == 'color' + back_top
$! Center of top of disk
$! Put the label centered in the top of the disk
$	ct = "(" + f$ext(0,wid-2,SPs) + ")"
$	label = f$extract(0,wid-2,f$element(i,"\",labels))
$	ct[(wid-f$len(label)+1)/2,f$len(label)] := "''label'"
$	center_top = ct
$	j = j + 1
$	CONT_LINE_'j' == 'color' + center_top
$! Front edge of top of disk
$	j = j + 1
$	CONT_LINE_'j' == 'color' + front_top
$! Dead space inside disk
$	k = 4
$ HGT_LOOP:
$	if k .lt. hgt
$	then	!Add another blank line to build the height of the container
$	    j = j + 1
$	    CONT_LINE_'j' == 'color' + dead_space
$	    k = k + 1
$	    goto HGT_LOOP
$	endif
$! Bottom of disk
$	j = j + 1
$	CONT_LINE_'j' == 'color' + bottom
$!
$	k = 0
$ SEP_LOOP:
$	if k .lt. sep
$	then	!Add another line between disks to separate them vertically
$	    j = j + 1
$	    CONT_LINE_'j' == f$extract(0,wid,SPs)
$	    goto SEP_LOOP
$	endif
$!
$	i = i + 1	!Count another disk
$	if i .lt. dskcnt then goto DSK_LOOP	!More to do -->
$!
$	ENDSUBROUTINE
