$	saved_verify = f$verify(0)
$!+
$! Put all the variables customizing this procedure here:
$!
$! P1 = Target device for kit build (defaults to KCUR$:)
$!-
$	facility = "SCAN"
$	version = "01"
$	update	= "2"
$	target = "DYA0:"
$	file_directory = "SCANFILES.DAT"
$	log_qual = "/NOLOG"
$	set noon
$	deassign scanexe
$	deassign scanmsg
$!
$!-
$	on control_y then goto bad_kit
$	on warning then goto bad_kit
$	ask := inquire/nopunctuation
$	say := write sys$output
$	first_kit = "Y"
$	remount = 0
$	say "''f$fao("!/!/!_VAX SCAN Kit Build Procedure!/")"
$
$	p = "CMKRNL,LOG_IO,SYSNAM,VOLPRO"
$	saved_privs = f$setprv(p)
$	if .not. f$priv(p) then say "You need the following privileges: ",p
$	if .not. f$priv(p) then goto bad_kit
$
$	!+
$	!  Ask additional questions
$	!-
$
$	!+
$	! Determine the point release number 
$	!- 
$	IF P2 .NES. "" THEN GOTO Cont2
$Get_Update: 
$	 WRITE SYS$OUTPUT ""
$	 INQUIRE Update "Point Release number (0-9)" 
$	 Update = F$EDIT (Update,"UPCASE,COLLAPSE") 
$	 IF (Update .GES. "0") .AND. -
	    (Update .LES. "9") .AND. - 
	    (F$LENGTH(Update) .EQ. 1) THEN GOTO Cont2 
$	 WRITE SYS$ERROR "Please specify a number from 0 to 9" 
$	 GOTO Get_Update 
$cont2: 
$
$	!+
$	! Determine the device if not specified
$	!- 
$	IF P1 .NES. "" THEN GOTO Cont3
$	WRITE SYS$OUTPUT ""
$	INQUIRE P1 "Device Name <DYA0:>"
$cont3: 
$
$
$	!+
$	! Determine the device type
$	!- 
$Get_kit_type:
$	P3 = F$EDIT(P3,"UPCASE,COLLAPSE")
$	IF (P3 .EQS. "Y") .OR. (P3 .EQS. "N") THEN GOTO Cont4
$	WRITE SYS$OUTPUT ""
$	INQUIRE P3 "TK50 Kit (Y/N)"	
$	GOTO Get_kit_type
$cont4: 
$
$	product = "''facility'''version'''update'"
$
$	dest = p1
$	if dest .eqs. "" then dest = target
$	if p1 .eqs. "" then say "Using ''dest' to build kits on."
$	if f$loc(":",dest) .eq. f$len(dest) then dest = dest + ":"
$	dest = f$parse(dest,,,"DEVICE") + f$parse(dest,"[0,0]",,"DIRECTORY")
$	if f$loc("CSA",dest) .ne. f$len(dest) then goto m7
$	if f$getdvi(dest,"EXISTS") then -
		if f$getdvi(dest,"DEVCLASS") .eq. 1 .or. -
		   f$getdvi(dest,"DEVCLASS") .eq. 2 then goto m7
$	say "Device must be a disk or tape."
$	goto bad_kit
$m7:	device = f$parse(dest,,,"DEVICE")
$
$	remount = f$loc("CSA",device) .ne. f$len(device)
$	if .not. remount then goto m9
$	s := $sysgen
$	s connect console
$	if f$getdvi(device,"MNT") then dismount 'device
$	allocate 'device
$m9:
$
$	if f$getdvi(device,"DEVCLASS") .eq. 1 .and. -
	   f$getdvi(device,"MNT") .and. -
	   .not. f$getdvi(device,"FOR") then goto m15
$	if f$getdvi(device,"MNT") then dismount/nounload 'device
$	say "Please ready the first distribution volume on ''device."
$m10:		ask w "Are you ready? "
$		if .not. w then goto m10
$	mount/foreign/noassist 'device
$	remount = 1
$	
$m15:	if f$getdvi(device,"DEVCLASS") .eq. 2 then goto m_TAPE
$	if f$getdvi(device,"MAXBLOCK") .gt. 8*512 then goto m_DISK
$
$m_CONSOLE:
$	backup_quals := /init/block=9000/group=25
$	labels = "''facility'01,''facility'02,''facility'03,''facility'04,''facility'05,"
$	labels = labels + "''facility'06,''facility'07,''facility'08,''facility'09,''facility'10"
$	goto m19
$m_DISK:
$	kitinstal_loc = ""
$	backup_quals := /block=9000/group=25
$	labels = facility
$	goto m19
$m_TAPE:
$	backup_quals := /rewi/dens=1600
$	ask w "Are there already savesets on this tape? "
$	if w then backup_quals := /norewi/dens=1600
$	if f$getdvi(device,"DEVTYPE") .eq. 10 then -
	backup_quals = backup_quals - "/dens=1600" + "/buffers=5"
$	labels = facility
$	goto m19
$m19:
$!+
$! Save away the kit's BACKUP qualifiers.
$!-
$	saved_backup_quals := 'backup_quals
$!+
$! Set protection in all files.
$!-
$	open/read data_file 'file_directory
$	prot := (s:rwed,o:rwed,g:rwed,w:re)
$p10:
$	read/end=done_set_prot data_file file_info
$	file_info := 'file_info
$	if file_info .eqs. "" then goto p10
$	if f$loc("SAVE SET", file_info) .ne. f$len(file_info) then goto p10
$	if f$loc("NEW MEDIA", file_info) .ne. f$len(file_info) then goto p10
$	if file_info .eqs. "KITINSTAL.COM;" then file_info := 'kitinstal_loc''file_info'
$	set protection='prot''log_qual 'file_info
$	goto p10
$done_set_prot:
$	close data_file
$next_kit:
$	open/read data_file 'file_directory
$s5:	read data_file junk	!Throw away first line of "SAVE SET A"
$	junk := 'junk
$	if junk .eqs. "" then goto s5
$
$	backup_quals := 'saved_backup_quals
$	letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
$
$s10:
$
$	f = ""
$s11:	read/end=s19 data_file file_info
$	file_info := 'file_info
$	if file_info .eqs. "" then goto s11
$	if f$loc("SAVE SET", file_info) .ne. f$len(file_info) then goto s12
$	if f$loc("NEW MEDIA", file_info) .ne. f$len(file_info) .and. -
	( f$getdvi(device,"DEVCLASS") .ne. 2 .and. -
	   f$getdvi(device,"MAXBLOCK") .le. 8*512) then goto s13
$	if f$loc("NEW MEDIA", file_info) .ne. f$len(file_info) .and. -
	( f$getdvi(device,"DEVCLASS") .eq. 2 .or. -
	   f$getdvi(device,"MAXBLOCK") .gt. 8*512) then goto s11
$	if f .eqs. "KITINSTAL.COM;" then f := 'kitinstal_loc''f'
$	if f .nes. "" then f := 'f'","
$	f := 'f''file_info'
$	goto s11
$	
$	
$s12:	write sys$output "Placing ",f," in save set ''f$extract(0,1,letters)'."
$	backup/comment="VAX/VMS SPKITBLD Procedure"/inter/log/verify -
		'f -
		'dest''product.'f$ext(0,1,letters)/label=('labels)/save'backup_quals
$	letters = f$ext(1,999,letters)
$	backup_quals := 'backup_quals/norewi/noinit
$	goto s10
$s13:	if f .nes. "" then -
		write sys$output "NEW MEDIA must be first line of a save set."
$	if f .nes. "" then goto bad_kit
$	dismount/nounload 'device
$	mount/foreign/noassist/nomessage 'device
$	i = f$loc(f$getdvi(device,"VOLNAM"),labels)
$	labels = f$ext(i+f$len(facility)+3,999,labels)
$	backup_quals := 'backup_quals/init
$	dismount 'device
$	say "Please mount the next distribution volume on ''device."
$s15:		ask w "Are you ready? "
$		if .not. w then goto s15
$	mount/foreign/noassist 'device
$	goto s10
$s19:
$
$	say "''f$fao("!/!_Kit Build Completed Successfully!/!/")"
$	close data_file
$	ask r "Do you want to make another kit? <Y/N> [N] "
$	if r then goto next_kit
$
$done:
$	if f$logical( "data_file" ) .nes. "" then close data_file
$	on warning then exit
$	if .not. remount then goto d9
$	if f$getdvi(device,"MNT") then dismount/nounload 'device
$	if f$loc("CSA",device) .eq. f$len(device) then goto d9
$	say "Please remount the console volume."
$d5:		ask w "Are you ready? "
$		if .not. w then goto d5
$	mount/foreign/system/protection=s:rwlp/nowrite/noassist 'device console
$d9:	w = f$setpriv(saved_privs)
$	exit
$
$bad_kit:
$	type sys$input:

	+---------------------+
	|		      |
	|  THIS IS A BAD KIT  |
	|		      |
	+---------------------+

$	goto done

$!++
$! Facility:	SPKITBLD, Procedure to Build a VMS Software Product Kit
$!
$! Abstract:	This procedure is used to build an installation kit
$!		for a VAX/VMS software product.  It is intended to
$!		build kits which are installed by the "new" VMSINSTAL.
$!
$! Environment:	The document "Developer's Guide to the VAX/VMS VMSINSTAL
$!		Procedure" describes this procedure in detail.
$!
$! Parameters:	P1 - product to be built
$!  (all	P2 - device on which to build kit
$!  optional)	P3 - files to place in first saveset
$!
$! Author:	Paul C. Anagnostopoulos, Creation Date: 30 June 1982
$!		STAR::Greek
$!--
