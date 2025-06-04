$	old_verify = 'f$verify(f$trnlnm("SHADOW$CONFIG_verify"))'
$!-----------------------------------------------------------------------------
$!
$! SHADOW$CONFIG.COM
$!
$! Determine the existence, membership, and status of all shadowsets on the
$! system, and their member devices.
$!
$! Results are placed either into logical names in a specified logical name
$! table (default is the /PROCESS table), or defined as DCL global symbols
$! in the context of the executing process.
$!
$! The P1 parameter determines whether the results are placed into logical
$! names or DCL global symbols.  If P1 is blank or contains "GLOBAL_SYMBOLS",
$! the results are placed into DCL symbols.  If P1 contains "LOGICAL_NAMES",
$! the results are placed into logical names.  By default, the logical names
$! are defined in the Process table, but if P2 contains "GROUP" or "SYSTEM",
$! the logical names are placed instead into the Group or System tables.
$!
$! The following information is returned by the procedure:
$!
$!  SHADOW$CONFIG_set_count			Number of shadowsets
$!  SHADOW$CONFIG_SET_n_LABEL			Volume label for each shadowset
$!!!!  SHADOW$CONFIG_SET_n_STATE			shadowset state (normal, mergin, copying, etc.)
$!  SHADOW$CONFIG_SET_n_DEVICE_NAME		DSAn: device name for shadowset
$!  SHADOW$CONFIG_SET_n_MEMBER_COUNT		shadowset size (# of members)
$!!!  SHADOW$CONFIG_SET_n_VALID_MEMBER_COUNT	# of members with fully valid data
$!  SHADOW$CONFIG_SET_n_MEMBER_m_DEVICE_NAME	Member device names
$!  SHADOW$CONFIG_SET_n_MEMBER_m_STATE	Member state:
$!   "ShadowMember", "FullCopying", or "MergeCopying"
$!
$! There is also a timestamp logical name defined if the results are placed
$! into logical names:
$!
$!  SHADOW$CONFIG_TIMESTAMP	Time of data-gathering
$!						        V0.0-000, 7/96
$!	Based on the example RAID$CONFIG.COM from DEC's StorageWorks RAID
$!	Software for OpenVMS, V2.2 (which I also wrote)
$!
$! Author: Keith Parris  parris@encompasserve.org or keithparris@yahoo.com
$! http://www.geocities.com/keithparris/ or http://encompasserve.org/~parris/
$!-----------------------------------------------------------------------------
$! The contents of the P1 parameter determine whether to place results in
$! logical names or in DCL global symbols.
$	USE_LOGICAL_NAMES := FALSE
$!	IF P1 .EQS. "" .OR. P1 .EQS. "GLOBAL_SYMBOLS" THEN -
$!!	    USE_LOGICAL_NAMES := FALSE
$	IF P1 .EQS. "LOGICAL_NAMES" THEN USE_LOGICAL_NAMES := TRUE
$! Determine if results are to be placed into a logical name table or into
$! global symbols.
$	IF USE_LOGICAL_NAMES
$	THEN   !Define logical names
$! The P2 parameter can be optionally included to specify the logical name
$! table into which the results are to be placed:
$		TABLE = "PROCESS"
$  		IF P2 .eqs. "SYSTEM" then TABLE = "SYSTEM"
$		IF P2 .eqs. "GROUP" then TABLE = "GROUP"
$!!		IF P2 .eqs. "PROCESS" then TABLE = "PROCESS"
$		DEFINE_IT := DEFINE/nolog/'TABLE' !Place in the specified table
$		EQUALS = ""
$		SHOW_IT = "!"   !Disable output
$!!!!		SHOW_IT := SHOW LOGICAL/GROUP !Uncomment for display output
$		TIMESTAMP = F$TIME()  !Record the time
$	ELSE   !Define global symbols
$		DEFINE_IT = ""
$		EQUALS = "=="
$		SHOW_IT = "!"   !Disable output
$!!!!		SHOW_IT := SHOW SYMBOL/GLOBAL !Uncomment for display output
$	ENDIF
$!-----------------------------------------------------------------------------
$!
$! First, get information about the shadowsets from F$GETDVI
$!
$!-----------------------------------------------------------------------------
$	pid = f$getjpi("","PID")
$	dev = f$device("OPA0:",,,1) !Reset search context
$	set_count = 0
$ shadowset_loop: !Look for all host-based volume shadowing shadow sets
$! Get shadow set virtual unit name (_DSAn:)
$	dev = f$device("_DSA*","DISK",,1)
$	if dev .eqs. "" then goto shadowset_loop_end
$	if f$extract(0,1,dev) .eqs. "_" then -
		dev = dev - "_"	!Remove leading underscore
$	set_count = set_count + 1
$	'DEFINE_IT' -
		SHADOW$CONFIG_SET_'set_count'_DEVICE_NAME -
		'EQUALS' "''dev'"	!DSAn: device name for shadowset
$	'SHOW_IT' SHADOW$CONFIG_SET_'set_count'_DEVICE_NAME	!!!
$	lbl = f$getdvi(dev,"VOLNAM")
$	'DEFINE_IT' -
		SHADOW$CONFIG_SET_'set_count'_LABEL -
                'EQUALS' "''lbl'"       !Volume label
$!!!!!  SHADOW$CONFIG_SET_n_STATE			shadowset state (normal, mergin, copying, etc.)
$	devname = dev
$	k = 0
$!!!	valid_mbrs = 0	!Count of regular normal members or merge-copying members
$ LOOP_SHAD_MEM: !Get the device name of the next shadow set member
$		devname = f$getdvi("_''devname'","SHDW_NEXT_MBR_NAME")
$		if devname .eqs. "" then goto END_SHAD_MEM
$		if f$extract(0,1,devname) .eqs. "_" then -
			devname = devname - "_"	!Remove leading underscore
$		k = k + 1
$		'DEFINE_IT' -
			SHADOW$CONFIG_SET_'set_count'_MEMBER_'k'_DEVICE_NAME -
			'EQUALS' "''devname'"
$		'SHOW_IT' SHADOW$CONFIG_SET_'set_count'_MEMBER_'k'_DEVICE_NAME !!!
$		if f$getdvi(devname,"SHDW_MEMBER")
$		then
$			shdw_mem_state = "ShadowMember"
$!!!			valid_mbrs = valid_mbrs + 1
$		endif
$!!!! Note: merge-copying volumes are indicated by F$GETDVI as ShadowMembers
$!!!! above, and thus counted in the valid_mbrs total.
$! Below, we change the member state for merge-copying members to differentiate
$! them from members which are not in a merge-copy state.
$		if f$getdvi(devname,"SHDW_MERGE_COPYING") then -
			shdw_mem_state = "MergeCopying"
$!!!! Note: Full-copy targets are also counted above in the valid_mbrs total,
$!!!! so here we remove then from the count.
$!!!! To protect against disk failures, we need at least 2 members in the set
$!!!! which are not full-copy targets.
$		if f$getdvi(devname,"SHDW_CATCHUP_COPYING")
$		then
$			shdw_mem_state = "FullCopying"
$!!!			valid_mbrs = valid_mbrs - 1
$		endif
$		'DEFINE_IT' -
			SHADOW$CONFIG_SET_'set_count'_MEMBER_'k'_STATE -
			'EQUALS' "''shdw_mem_state'"
$		'SHOW_IT' SHADOW$CONFIG_SET_'set_count'_MEMBER_'k'_STATE !!!
$		goto LOOP_SHAD_MEM
$ END_SHAD_MEM:
$	'DEFINE_IT' -
		SHADOW$CONFIG_SET_'set_count'_MEMBER_COUNT 'EQUALS' 'k'
$	'SHOW_IT' SHADOW$CONFIG_SET_'set_count'_MEMBER_COUNT  !!!
$!!!	'DEFINE_IT' -
$!!!!		SHADOW$CONFIG_SET_'set_count'_VALID_MEMBER_COUNT 'EQUALS' 'valid_mbrs'
$!!!	'SHOW_IT' SHADOW$CONFIG_SET_'set_count'_VALID_MEMBER_COUNT  !!!
$	goto shadowset_loop
$ shadowset_loop_end:
$	'DEFINE_IT' SHADOW$CONFIG_SET_COUNT 'EQUALS' 'set_count'
$	'SHOW_IT' SHADOW$CONFIG_SET_COUNT  !!!
$!!!	show symbol/global shadow$*	!!!!
$	EXIT 1+0*f$verify(old_verify)
