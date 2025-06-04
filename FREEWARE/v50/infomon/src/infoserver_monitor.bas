%TITLE "InfoServer Monitor"
%IDENT "V2.2"			! Also change variable SOFTWARE_VERSION below

PROGRAM infoserver_monitor

!+
! FACILITY:
! 
!   InfoServer Monitor
! 
! PROGRAM DESCRIPTION:
! 
!   The InfoServer Monitor monitors all mounted InfoServer disks and
!   dismounts and remounts them if they are offline due to a previous write
!   access to the disk.
! 
! AUTHORS:
! 
!   Paul Anderson
! 
! CREATION DATE: February 19, 1991
! 
! MODIFICATION HISTORY:
! 
! 02/19/91  V1.0    PRA	    Original version
! 03/02/91  V1.1    PRA	    Enhancements:
!				Dismount disks without unloading
!				Remove ESS$LADCP commands
!				Do not delete logical name DAD$service_name
! 03/07/91  V1.2    PRA	    Enhancements:
!				Checks all DAD devices, mounted or not
!				Remounts disks that have changed volume labels
! 05/21/91  V1.3    PRA	    Enhancements:
!				Don't mount writable devices
!				Change BYTE variables to LONG
! 04/01/92  V1.3A   PRA	    Change to KITINSTAL.COM to not send mail
! 08/05/92  V1.3B   PRA	    Bug fix:
!				Correctly translate INFO_INTERVAL logical name
! 10/04/94  V2.0    PRA	    Changes to KITINSTAL.COM for OpenVMS Alpha
! 06/21/95  V2.1    PRA	    Bug fixes:
!				Wait after DISMOUNT to allow it to complete
!				Wait after stopping old process to allow new one
!			    Enhancement:
!				Display process ID in log file
! 11/03/96  V2.2    PRA	    Bug fixes:
!				Change device length for MOUNT96 in OpenVMS V7.1
!			    Enhancement:
!				Display mount and dismount error messages
!-

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"

    RECORD zero
        VARIANT
	    CASE
	        basic$quadword	context
	    CASE
	        LONG		zero_long (2)
        END VARIANT
    END RECORD zero

    DIMENSION	STRING		writable_device (100)

    DECLARE	LONG   CONSTANT wrong_volume	    = 7471372
    DECLARE	STRING CONSTANT software_version    = "V2.2"

    DECLARE	LONG		device_allocated,			    &
				device_information,			    &
				device_mounted,				    &
				i,					    &
				number_of_writable_devices,		    &
		STRING		device_name,				    &
				volume_label,				    &
		zero		z

    FOR i = 1 TO 2
	z::zero_long (i) = 0
    NEXT i

    PRINT "InfoServer Monitor "; software_version
    PRINT
    CALL print_current_time
    PRINT "started in process "; get_process_id
    PRINT
    WHILE 1
	CALL initialize (no_more_devices, z::context)
	no_more_devices = get_device_name (device_name, z::context)
	no_more_devices = get_device_name (device_name, z::context)
	UNTIL no_more_devices
	    device_writable = device_check (device_name,		    &
					    writable_device (),		    &
					    number_of_writable_devices)
	    IF NOT device_writable
	       THEN CALL get_device_mount_status (device_name,		    &
						  device_allocated,	    &
						  device_mounted)
		    IF NOT device_allocated
		       THEN IF device_mounted
			       THEN device_offline =			    &
					get_device_online_status (device_name)
				    IF device_offline
				       THEN mount_status = remount_device   &
						(device_name,		    &
						 volume_label,		    &
						 system,		    &
						 writable_device (),	    &
						 number_of_writable_devices)
					    IF mount_status = wrong_volume
					       THEN remount_status =	    &
						    reset_device	    &
						    (device_name,	    &
						     volume_label,	    &
						     writable_device (),    &
						     number_of_writable_devices)
					    END IF
				    END IF
			       ELSE remount_status = reset_device	    &
						    (device_name,	    &
						     volume_label,	    &
						     writable_device (),    &
						     number_of_writable_devices)
			    END IF
		    END IF
	    END IF
	    no_more_devices = get_device_name (device_name, z::context)
	NEXT
	CALL go_to_sleep
    NEXT

END PROGRAM


FUNCTION LONG device_check (STRING  device_name,			    &
				    writable_device (),			    &
			    LONG    number_of_writable_devices)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"

    DECLARE LONG    i

    device_writable = false
    FOR i = 1 TO number_of_writable_devices
	IF device_name = writable_device (i)
	   THEN device_writable = true
        END IF
    NEXT i

END FUNCTION device_writable


FUNCTION LONG dismount_device (STRING device_name)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$dmtdef" %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$ssdef"  %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE LONG dismount_flags

    dismount_flags = dmt$m_abort      OR				    &
		     dmt$m_ovr_checks OR				    &
		     dmt$m_nounload
    cond_value = sys$dismou (device_name, dismount_flags)
    IF cond_value <> ss$_normal
       THEN CALL print_current_time
	    PRINT "Error dismounting "; device_name
	    CALL print_error_message (cond_value)
    END IF
    SLEEP 10

END FUNCTION cond_value


FUNCTION LONG get_device_name (STRING		device_name,		    &
			       basic$quadword	context)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$ssdef"  %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE STRING	device_name_search
    
    device_name_search = "_DAD*"
    cond_value = sys$device_scan (c::device_name_fixed,			    &
				  device_name_length,			    &
				  device_name_search,			    &
				  ,					    &
				  context)
    SELECT cond_value
	CASE ss$_normal
	    device_name = EDIT$ (c::device_name_fixed, 6)
	    no_more_devices = false
	CASE ss$_nomoredev,						    &
	     ss$_nosuchdev
	    no_more_devices = true
	CASE ELSE
	    CALL stop_program (cond_value)
    END SELECT

END FUNCTION no_more_devices


FUNCTION LONG get_device_online_status (STRING device_name)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"

    device_offline = false
    WHEN ERROR IN
	OPEN device_name + "[000000]000000.dir" FOR INPUT AS 1,		    &
	    ORGANIZATION VIRTUAL,					    &
	    ACCESS READ
	GET #1
    USE
	SELECT ERR
	    CASE 1			! bad directory for device
		device_offline = true
	    CASE 162			! Cannot open file
		device_offline = true
	    CASE 252			! file ACP failure
		device_offline = true
	END SELECT
    END WHEN
    CLOSE 1

END FUNCTION device_offline


FUNCTION LONG get_device_write_status (STRING device_name)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"

    DECLARE LONG    file_opened,					    &
	    STRING  file_name

    file_name = device_name + "[000000]infoserver_monitor.tmp"
    device_writable = true
    file_opened	    = true
    WHEN ERROR IN
	OPEN file_name FOR OUTPUT AS 1,					    &
	    ACCESS WRITE
	GET #1
    USE
	file_opened = false
	IF ERR = 14		! Device hung or write-locked
	   THEN device_writable = false
	END IF
    END WHEN
    IF file_opened
       THEN KILL file_name
	    CLOSE 1
    END IF

END FUNCTION device_writable


FUNCTION STRING get_process_id

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$jpidef"	    %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$ssdef"	    %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE STRING process_id

    cond_value = lib$getjpi (jpi$_pid,					    &
			     ,						    &
			     ,						    &
			     ,						    &
			     process_id)

END FUNCTION process_id


FUNCTION LONG get_volume_label (STRING	device_name,			    &
					volume_label)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$dvidef"	    %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$ssdef"	    %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE STRING device_information

    cond_value = lib$getdvi (dvi$_volnam,				    &
			     ,						    &
			     device_name,				    &
			     ,						    &
			     device_information,			    &
			     )
    SELECT cond_value
	CASE ss$_normal
	    volume_label = device_information
        CASE ELSE
	    CALL print_current_time
	    PRINT "Error getting volume label for "; device_name
	    CALL print_error_message (cond_value)
    END SELECT

END FUNCTION cond_value


FUNCTION LONG mount_device (STRING  device_name,			    &
				    volume_label,			    &
			    LONG    mount_type,				    &
			    STRING  writable_device (),			    &
			    LONG    number_of_writable_devices)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$dmtdef" %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$mntdef" %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$ssdef"  %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE item_list	item_list_mount (3),				    &
	    LONG	mount_flags

    item_list_mount(0)::itmcod = mnt$_devnam
    item_list_mount(0)::buflen = LEN (c::device_name_fixed)
    item_list_mount(0)::bufadr = LOC (c::device_name_fixed)
    item_list_mount(0)::lenadr = 0
    item_list_mount(1)::itmcod = mnt$_flags
    item_list_mount(1)::buflen = 4
    item_list_mount(1)::bufadr = LOC (mount_flags)
    item_list_mount(1)::lenadr = 0
    c::device_name_fixed = device_name
    SELECT mount_type
	CASE process
	    mount_flags = mnt$m_noassist  OR				    &
			  mnt$m_ovr_ident OR				    &
			  mnt$m_nomntver  OR				    &
			  mnt$m_nowrite
	    item_list_mount(2)::trmntr = 0
	CASE system
	    mount_flags = mnt$m_noassist OR				    &
			  mnt$m_nomntver OR				    &
			  mnt$m_system   OR				    &
			  mnt$m_nowrite
	    c::volume_label_fixed = volume_label
	    item_list_mount(2)::itmcod = mnt$_volnam
	    item_list_mount(2)::buflen = LEN (c::volume_label_fixed)
	    item_list_mount(2)::bufadr = LOC (c::volume_label_fixed)
	    item_list_mount(2)::lenadr = 0
	    item_list_mount(3)::trmntr = 0
    END SELECT
    cond_value = sys$mount (item_list_mount (0))
    IF cond_value = ss$_normal
       THEN device_writable = get_device_write_status (device_name)
	    IF device_writable
	       THEN dismount_status = dismount_device (device_name)
		    number_of_writable_devices = number_of_writable_devices + 1
		    writable_device (number_of_writable_devices) = device_name
	    END IF
       ELSE CALL print_current_time
	    PRINT "Error mounting "; device_name
	    CALL print_error_message (cond_value)
    END IF

END FUNCTION cond_value


FUNCTION LONG remount_device (STRING	device_name,			    &
					volume_label,			    &
			      LONG	mount_type,			    &
			      STRING	writable_device (),		    &
			      LONG	number_of_writable_devices)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$ssdef" %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE LONG volume_status

    remount_status = ss$_normal
    volume_status = get_volume_label (device_name, volume_label)
    SELECT volume_status
       CASE ss$_normal
	    dismount_status = dismount_device (device_name)
	    SELECT dismount_status
		CASE ss$_normal
		    IF mount_type = system
		       THEN CALL print_current_time
			    PRINT TRM$ (volume_label);			    &
				  " dismounted from "; device_name
		    END IF
		    mount_status =					    &
			mount_device (device_name,			    &
				      volume_label,			    &
				      system,				    &
				      writable_device (),		    &
				      number_of_writable_devices)
		    SELECT mount_status
			CASE ss$_normal
			    CALL print_current_time
			    PRINT TRM$ (volume_label); " mounted on "; &
				  device_name
			CASE ELSE
			    remount_status = mount_status
		    END SELECT
		CASE ELSE
		    remount_status = dismount_status
	    END SELECT
	CASE ELSE
	    remount_status = volume_status
    END SELECT

END FUNCTION remount_status


FUNCTION LONG reset_device (STRING  device_name,			    &
				    volume_label,			    &
				    writable_device (),			    &
			    LONG    number_of_writable_devices)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$ssdef" %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE LONG reset_status

    reset_status = ss$_normal
    mount_status = mount_device (device_name,				    &
				 ,					    &
				 process,				    &
				 writable_device (),			    &
				 number_of_writable_devices)
    IF mount_status = ss$_normal
       THEN remount_status = remount_device (device_name,		    &
					     volume_label,		    &
					     process,			    &
					     writable_device (),	    &
					     number_of_writable_devices)
	    IF remount_status <> ss$_normal
	       THEN reset_status = remount_status
	    END IF
    END IF

END FUNCTION reset_status


SUB get_device_mount_status (STRING device_name,			    &
			     LONG   device_allocated,			    &
				    device_mounted)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$devdef"	    %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$dvidef"	    %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$ssdef"	    %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE LONG device_information

    c::device_name_fixed = device_name
    cond_value = lib$getdvi (dvi$_devchar,				    &
			     ,						    &
			     c::device_name_fixed,			    &
			     device_information,			    &
			     ,						    &
			     )
    IF cond_value <> ss$_normal
       THEN CALL stop_program (cond_value)
    END IF
    IF device_information AND dev$m_all
       THEN device_allocated = true
       ELSE device_allocated = false
    END IF
    IF device_information AND dev$m_mnt
       THEN device_mounted = true
       ELSE device_mounted = false
    END IF

END SUB


SUB go_to_sleep

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$lnmdef" %FROM %LIBRARY "sys$library:basic$starlet"
    %INCLUDE "$ssdef"  %FROM %LIBRARY "sys$library:basic$starlet"

    RECORD var_logical
	STRING	logical_name_value  = 255
    END RECORD

    DECLARE LONG   CONSTANT logical_name_length	    = 255,		    &
			    sleep_interval_default  =  10

    DECLARE STRING CONSTANT logical_name_table = "LNM$SYSTEM_TABLE"

    DECLARE item_list	    item_list_trnlnm (1),			    &
	    LONG	    logical_name_value_length,			    &
			    sleep_interval,				    &
	    STRING	    logical_name,				    &
	    var_logical	    l

    logical_name = "INFO_INTERVAL"
    item_list_trnlnm(0)::buflen = logical_name_length
    item_list_trnlnm(0)::itmcod = lnm$_string
    item_list_trnlnm(0)::bufadr = LOC (l::logical_name_value)
    item_list_trnlnm(0)::lenadr = LOC (logical_name_value_length)
    item_list_trnlnm(1)::trmntr = 0
    cond_value = sys$trnlnm (,						    &
			     logical_name_table,			    &
			     logical_name,				    &
			     ,						    &
			     item_list_trnlnm (0))
    SELECT cond_value
	CASE ss$_normal
	    WHEN ERROR IN
		sleep_interval =					    &
			INTEGER (EDIT$ (l::logical_name_value, 6), LONG)
	    USE
		CALL print_current_time
		PRINT "Error in time interval of ";			    &
		      TRM$ (logical_name); "; using ";			    &      
		      STR$ (sleep_interval_default); " minutes"
		sleep_interval = sleep_interval_default
	    END WHEN
	CASE ss$_nolognam
	    sleep_interval = sleep_interval_default
	CASE ELSE
	    CALL stop_program (cond_value)
    END SELECT
    WHEN ERROR IN
	SLEEP sleep_interval * 60
    USE
	SLEEP sleep_interval_default * 60
    END WHEN

END SUB


SUB initialize (LONG		n,					    &
		basic$quadword	k)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"

    RECORD zero_rec
        VARIANT
	    CASE
	        basic$quadword	context
	    CASE
	        LONG		zero_long (2)
        END VARIANT
    END RECORD zero_rec

    DECLARE LONG	i,						    &
	    zero_rec	z

    n = false
    FOR i = 1 TO 2
	z::zero_long (i) = 0
    NEXT i
    k = z::context

END SUB


SUB print_current_time

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"
    %INCLUDE "$ssdef"  %FROM %LIBRARY "sys$library:basic$starlet"

    DECLARE LONG CONSTANT current_time_length = 20

    RECORD var_time
	STRING current_time = current_time_length
    END RECORD

    DECLARE var_time t

    cond_value = sys$asctim (current_time_length,			    &
			     t::current_time,				    &
			     ,						    &
			     )
    SELECT cond_value
	CASE ss$_normal,						    &
	     ss$_bufferovf
	    PRINT t::current_time; SPACE$ (2);
	CASE ELSE
	    CALL stop_program (cond_value)
    END SELECT

END SUB


SUB print_error_message (LONG error_number)

    OPTION TYPE = EXPLICIT

    %INCLUDE "infoserver_monitor.include"

    DECLARE STRING  error_message

    CALL lib$sys_getmsg (error_number,, error_message)
    PRINT TAB (22); error_message

END SUB


SUB stop_program (LONG cond_value)

    OPTION TYPE = EXPLICIT

    CALL lib$signal (cond_value BY VALUE)
    STOP

END SUB
