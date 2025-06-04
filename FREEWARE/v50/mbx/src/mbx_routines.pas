[ 
IDENT ( 'MBX V1.2' ) , 
INHERIT ( 
    'sys$library:pascal$cli_routines' ,
    'sys$library:pascal$lib_routines' ,
    'sys$library:starlet' ,
    'int:mbx_declarations' ,
    'int:mbx_messages' )
]
MODULE mbx_routines ;

{ © Marc Van Dyck, 06-MAY-1999 }


[ HIDDEN ] CONST

    {+}
    { Basic stuff, internal use only
    {-}

    word_unsigned_length    = 5 ;				{ max length of a word written in decimal (65535) }
    log_name_length	    = 255 ;				{ max length of a logical name }
    equ_name_length	    = 1024 ;				{ max length of the equivalence of a logical name }
    suffix		    = '_CHANNEL' ;			{ added to mbx name to form logical storing channel # }
    max_deltatime_length    = LENGTH ('9999-23:59:59.99') ;	{ max length of a deltatime string }


[ HIDDEN ] TYPE

    {+}
    { Basic stuff, internal use only
    {-}

    quad_unsigned   = [ BYTE (8) ] PACKED RECORD L:UNSIGNED ; H:UNSIGNED ; END ;
    word_unsigned   = [ BYTE (2) ] 0..65535 ;
    log_name_type   = VARYING [log_name_length] OF CHAR ;
    equ_name_type   = VARYING [equ_name_length] OF CHAR ;
    word_in_text    = VARYING [word_unsigned_length] OF CHAR ;

    item_list_type  = RECORD
			length	: word_unsigned ;
			item	: word_unsigned ;
			buffer	: UNSIGNED ;
			retlen	: UNSIGNED ;
			endmark	: UNSIGNED VALUE 0 ;		{ Indicates end of list. Must be zero at all times }
		      END;

    iosb_type	    = RECORD
			status	: word_unsigned ;
			count	: word_unsigned ;
			info	: UNSIGNED ;
		      END ;


[ HIDDEN ] VAR

    {+}
    { We need some static storage to store the data that we will pass to the [ UNBOUND ] routines.
    { All system services are declared [ UNBOUND ] in starlet.pas/pen .
    {-}

    item_list	    : [ VOLATILE ] item_list_type ;
    iostblk	    : [ VOLATILE ] iosb_type ;
    table	    : [ VOLATILE ] log_name_type ;
    mbx_log_name    : [ VOLATILE ] mbx_name_type ;
    mbx_permanent   : [ VOLATILE ] perm_flag_type ;
    mbx_msg_size    : [ VOLATILE ] msg_size_type ;
    mbx_msg_count   : [ VOLATILE ] msg_count_type ;
    mbx_protection  : [ VOLATILE ] protection_type ;
    mbx_dev_name    : [ VOLATILE ] dev_name_type ;
    mbx_dev_name2   : [ VOLATILE ] dev_name_type ;
    channel	    : [ VOLATILE ] mbx_chan_type ;
    channel_log	    : [ VOLATILE ] log_name_type ;
    mbx_message	    : [ VOLATILE ] message_type ;
    buffer_size	    : [ VOLATILE ] UNSIGNED ;
    device_class    : [ VOLATILE ] UNSIGNED ;
    io_function	    : [ VOLATILE ] UNSIGNED ;
    time_out_text   : [ VOLATILE ] VARYING [max_deltatime_length] OF CHAR ;
    time_out_bin    : [ VOLATILE ] quad_unsigned ;
    time_out_req    : [ VOLATILE ] BOOLEAN ;			
    equ_name	    : [ VOLATILE ] equ_name_type ;


[ HIDDEN , ASYNCHRONOUS , UNBOUND ] PROCEDURE cancel_io ;

    BEGIN

	$CANCEL ( chan := channel )

    END ;


[ HIDDEN ] FUNCTION convert_time ( seconds : time_out_type ; VAR time_machine : [ VOLATILE ] quad_unsigned ) : UNSIGNED ;

    VAR

	days	: UNSIGNED ;
	hours	: UNSIGNED ;
	minutes	: UNSIGNED ;

    BEGIN

    {+}
    { Anything to convert ? }
    {-}

	IF seconds = 0

    {+}
    { Well, not really ...
    {-}

	THEN BEGIN

	    time_machine := ZERO ;
	    convert_time := SS$_NORMAL

	END

    {+}
    { Sure !
    {-}

	ELSE BEGIN { of if-else branch } 

    {+}
    { We have a time expressed in seconds. Let's compute how many minutes and seconds it makes
    {-}

	    IF seconds < 60

	    THEN BEGIN
		minutes := 0 ;
		hours := 0 ;
		days := 0
	    END

	    ELSE BEGIN { of if-else branch }

		minutes := seconds DIV 60 ;
		seconds := seconds MOD 60 ;  { seconds is now < 60 }

    {+}
    { We now have minutes and seconds. Let's compute how many hours.
    {-}

		IF minutes < 60

		THEN BEGIN
		    hours := 0 ;
		    days := 0 ;
		END

		ELSE BEGIN { of if-else branch }

		    hours := minutes DIV 60 ;
		    minutes := minutes MOD 60 ; { minutes is now < 60 }

    {+}
    { We now have hours, minutes, and seconds. Let's compute how many days
    {-}

		    IF hours < 24
		    THEN days := 0
		    ELSE BEGIN { of if-else branch }

			days := hours DIV 24 ;
			hours := hours MOD 24   { hours is now < 24 }

    {+}
    { That's enough for a delta time. We don't needs months, years, ...
    {-}

		    END { of if-else branch }

		END { of if-else branch }

	    END { of if-else branch } ;

    {+}
    { Build a delta time string out of all the fields. If it fails, build an obviously wrong delta time string.
    {-}

	    WRITEV ( time_out_text , DEC ( days , 4 , 4 ) , ' ' ,
				     DEC ( hours , 2 , 2 ) , ':' ,
				     DEC ( minutes , 2 , 2 ) , ':' ,
				     DEC ( seconds , 2 , 2 ) , '.00' , ERROR := CONTINUE ) ;

	    IF STATUSV <> 0 THEN time_out_text := PAD ( '' , '*' , max_deltatime_length ) ;

    {+}
    { And convert this delta time to binary
    {-}

	    convert_time := $BINTIM ( timbuf := time_out_text , timadr := time_out_bin )

	END { of if-else branch }

    END { of function convert_time } ;


[ HIDDEN ] FUNCTION translate_logical (	logical_name	    : [ VOLATILE ] log_name_type ;
					VAR equival_name    : [ VOLATILE ] VARYING [eqbuflen] OF CHAR ;
					lntable_name	    : [ VOLATILE ] log_name_type ;
					access_mode	    : [ VOLATILE ] UNSIGNED := PSL$C_USER ;
					attributes	    : [ VOLATILE ] UNSIGNED := 0 ) 
				: UNSIGNED ;

    {+}
    { A generic function to translate a logical name
    {-}

    BEGIN { of function translate_logical }

        WITH item_list DO
        BEGIN
            length  := eqbuflen ;
            item    := LNM$_STRING ;
            buffer  := IADDRESS ( equival_name.BODY ) ;
            retlen  := IADDRESS ( equival_name.LENGTH )
        END ;

	translate_logical := $TRNLNM (  tabnam := lntable_name ,
					attr   := attributes ,
					lognam := logical_name ,
					acmode := access_mode ,
					itmlst := item_list )



    END { of function translate_logical } ;


[ HIDDEN ] FUNCTION define_kernel_logical (	logical_name	    : [ VOLATILE ] log_name_type ;
						VAR equival_name    : [ VOLATILE ] VARYING [eqbuflen] OF CHAR ;
						lntable_name	    : [ VOLATILE ] log_name_type )
					: UNSIGNED ;

    {+}
    { A generic function to define a logical name in kernel mode
    {-}

    VAR

	local_logical_name : [ STATIC , VOLATILE ] log_name_type ;
	local_lntable_name : [ STATIC , VOLATILE ] log_name_type ;

    {+}
    { This is the function that will be executed in kernel mode
    {-}

    [ ASYNCHRONOUS , UNBOUND ] FUNCTION define_logical : UNSIGNED ;

	BEGIN {of function define_logical }

            define_logical := $CRELNM ( tabnam := local_lntable_name ,
					attr   := LNM$M_CONFINE + LNM$M_NO_ALIAS ,
				        lognam := local_logical_name ,
				        acmode := PSL$C_KERNEL ,
				        itmlst := item_list )

        END { of function define_logical } ;

    BEGIN { of function define_kernel_logical } 

	local_logical_name := logical_name ;
	local_lntable_name := lntable_name ;

        WITH item_list DO
        BEGIN
	    length	:= equival_name.LENGTH ;
	    item	:= LNM$_STRING ;
	    buffer	:= IADDRESS ( equival_name.BODY ) ;
	    retlen	:= 0
	END ;
        
        define_kernel_logical := $CMKRNL ( routin := %IMMED define_logical , arglst := 0 )

    END { of function define_kernel_logical } ;


[ HIDDEN ] FUNCTION delete_kernel_logical (	logical_name	: [ VOLATILE ] log_name_type ;
						lntable_name	: [ VOLATILE ] log_name_type )
				    : UNSIGNED ;

    {+}
    { A generic function to delete a logical name in kernel mode
    {-}

    VAR

	local_logical_name : [ STATIC , VOLATILE ] log_name_type ;
	local_lntable_name : [ STATIC , VOLATILE ] log_name_type ;

    {+}
    { This is the function that will be executed in kernel mode
    {-}

    [ ASYNCHRONOUS , UNBOUND ] FUNCTION delete_logical : UNSIGNED ;

	BEGIN { of function delete_logical }

	    delete_logical := $DELLNM ( tabnam := local_lntable_name ,
				        lognam := local_logical_name ,
				        acmode := PSL$C_KERNEL )    

        END { of function delete_logical } ;

    BEGIN { of function delete_kernel_logical }

	local_logical_name := logical_name ;
	local_lntable_name := lntable_name ;

	delete_kernel_logical := $CMKRNL ( routin := %IMMED delete_logical , arglst := 0 )

    END { of function delete_kernel_logical } ;


[ HIDDEN ] FUNCTION get_mailbox_name (	logical_name	    : [ VOLATILE ] log_name_type ;
					VAR physical_name   : [ VOLATILE ] dev_name_type )
				: UNSIGNED ;

    {+}
    { A generic function that returns the physical name of a mailbox
    {-}

    VAR
	status			: UNSIGNED ;
	local_logical_name	: [ STATIC , VOLATILE ] log_name_type ;
	local_physical_name	: [ STATIC , VOLATILE ] dev_name_type ;

    BEGIN { of function test_mailbox }

    {+}
    { Initialize
    {-}

	local_logical_name := logical_name ;
	local_physical_name := '' ;
	physical_name := '' ;

    {+}
    { Prepare data for call to system service
    {-}

	WITH item_list DO
	BEGIN
	    length	:= dev_name_length ;
	    item	:= DVI$_DEVNAM ;
	    buffer	:= IADDRESS ( local_physical_name.BODY ) ;
	    retlen	:= IADDRESS ( local_physical_name.LENGTH )
	END ;

    {+}
    { Get the name now
    {-}

	status := $GETDVIW (	itmlst := item_list ,
				efn    := 0 ,
				devnam := local_logical_name ,
				iosb   := iostblk ) ;

	IF status = SS$_NORMAL THEN status := iostblk.status ;

    {+}
    { Evaluate results
    {-}

	IF status <> SS$_NORMAL

	THEN get_mailbox_name := status

	ELSE BEGIN

	    physical_name := local_physical_name ;
	    get_mailbox_name := SS$_NORMAL

	END

    END { of function get_mailbox_name } ;


[ HIDDEN ] FUNCTION test_mailbox (  mailbox_name    : [ VOLATILE ] log_name_type ;
				    VAR device_name : [ VOLATILE ] dev_name_type )
			: UNSIGNED ;

    {+}
    { A generic function that tests a logical name for the following things :
    { > The logical name exists
    { > It translates to an existing device
    { > The device is a mailbox
    { If all the above is true, it returns SS$_NORMAL and gives back the physical device name of the mailbox.
    {-}


    VAR
	status		    : UNSIGNED ;
	local_mailbox_name  : [ STATIC , VOLATILE ] log_name_type ;
	local_equiv_name    : [ STATIC , VOLATILE ] equ_name_type ;
	local_device_name   : [ STATIC , VOLATILE ] dev_name_type ;


    BEGIN { of function test_mailbox }

    {+}
    { Initialize
    {-}

	device_name := '' ;
	local_mailbox_name := mailbox_name ;
	local_equiv_name := '' ;
	local_device_name := '' ;

    {+}
    { Translate the logical name and see if it exists }
    {-}

        status := translate_logical (   logical_name := local_mailbox_name ,
					equival_name := local_equiv_name ,
					lntable_name := table ,
					attributes := LNM$M_CASE_BLIND ) ;

	IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

	THEN test_mailbox := status

    {+}
    { We got a translation
    {-}

	ELSE BEGIN { of if-else branch }

    {+}
    { Does that device exist ?
    {-}

	    device_class := 0 ;
	    WITH item_list DO
	    BEGIN
		length	:= 4 ;
		item	:= DVI$_DEVCLASS ;
		buffer	:= IADDRESS ( device_class ) ;
		retlen	:= 0
	    END ;

	    status := $GETDVIW (    itmlst := item_list ,
				    efn    := 0 ,
				    devnam := local_mailbox_name ,
				    iosb   := iostblk ) ;

	    IF status = SS$_NORMAL THEN status := iostblk.status ;	

	    IF status <> SS$_NORMAL

    {+}
    { No, or the call to the system service failed.
    {-}

	    THEN test_mailbox := status

    {+}
    { Yes; but is it a mailbox ?
    {-}

	    ELSE BEGIN { of if-else branch }

		IF device_class <> DC$_MAILBOX

    {+}
    { No. Report it to the caller
    {-}

		THEN test_mailbox := MBX_DEVNOTMBX
    
    {+}
    { Yes, it is a mailbox. Get the device physical name
    {-}

		ELSE BEGIN { of if-else branch }

		    status := get_mailbox_name (    logical_name := local_mailbox_name , 
						    physical_name := local_device_name ) ;
		    IF status <> SS$_NORMAL

    {+}
    { Request failed.
    {-}

		    THEN test_mailbox := status

    {+}
    { OK. Return success and exit.
    {-}

		    ELSE BEGIN
			device_name := local_device_name ;
			test_mailbox := SS$_NORMAL
		    END

		END { of if-else branch }

	    END { of if-else branch }

	END { of if-else branch }

    END { of function test_mailbox } ;


[ HIDDEN ] FUNCTION get_channel (   mailbox_name	: [ VOLATILE ] mbx_name_type ;
				    VAR channel_number	: [ VOLATILE ] mbx_chan_type ;
				    VAR channel_name	: [ VOLATILE ] log_name_type )
			: UNSIGNED ;

    {+}
    { A generic function that returns the channel number associated with a mailbox and the logical name that stores it
    {-}

    VAR
	status			: UNSIGNED ;
	local_channel_number	: [ STATIC ] mbx_chan_type ;
	local_channel_name	: [ STATIC , VOLATILE ] log_name_type ;
	local_channel_text	: [ STATIC , VOLATILE ] word_in_text ;

    BEGIN { of function get_channel }

    {+}
    { Initialize
    {-}

	channel_name := '' ;
	channel_number := 0 ;

    {+}
    { Construct and translate the logical name
    {-}

	local_channel_name := mailbox_name + suffix ;
	status := translate_logical (	logical_name    := local_channel_name ,
					equival_name    := local_channel_text ,
					lntable_name    := table ,
					attributes	:= LNM$M_CASE_BLIND ,
					access_mode	:= PSL$C_KERNEL ) ;

	CASE status OF

    {+}
    { We got a translation
    {-}

	    SS$_NORMAL	    : BEGIN { of first case branch }

    {+}
    { Transform ASCII into numeric
    {-}

		READV ( local_channel_text , local_channel_number ) ;
		IF STATUSV <> 0 

    {+}
    { Can't translate - logical name contains garbage
    {-}

		THEN get_channel := MBX_MBXINTERR

    {+}
    { Translation OK - all done, report good status
    {-}

		ELSE BEGIN

		    channel_name := local_channel_name ;
		    channel_number := local_channel_number ;
		    get_channel := SS$_NORMAL

		END

	    END { of first case branch } ;

    {+}
    { No translation found
    {-}

	    SS$_NOLOGNAM	: BEGIN { of second case branch }

    {+}
    { Return the name but no value
    {-}

		channel_name := local_channel_name ;
		get_channel := SS$_NOLOGNAM ;	    	    

	    END { of second case branch } ;

    {+}
    { Call to system service failed
    {-}

	    OTHERWISE get_channel := status

	END { of case statement }

    END { of function get_channel } ;


[ HIDDEN ] FUNCTION get_mbx_bufsize (	mailbox_name	: [ VOLATILE ] log_name_type ;
					VAR size	: [ VOLATILE ] UNSIGNED ) 
				: UNSIGNED ;

    {+}
    { A function to get the maximum buffer size of a mailbox.
    {-}

    VAR
	status		    : UNSIGNED ;
	local_mailbox_name  : [ STATIC , VOLATILE ] log_name_type ;
	device_size	    : [ STATIC , VOLATILE ] UNSIGNED ;

    BEGIN { of function get_mbx_bufsize }

	local_mailbox_name := mailbox_name ;
	device_size := 0 ;

	WITH item_list DO
	BEGIN
	    length	:= 4 ;
	    item	:= DVI$_DEVBUFSIZ ;
	    buffer	:= IADDRESS ( device_size ) ;
	    retlen	:= 0
	END ;
    
	status := $GETDVIW (	itmlst	:= item_list ,
				efn	:= 0 ,
				devnam	:= local_mailbox_name ,
				iosb	:= iostblk ) ;

	IF status = SS$_NORMAL THEN status := iostblk.status ;

	IF status = SS$_NORMAL THEN size := device_size ;

	get_mbx_bufsize := status

    END { of function get_mbx_bufsize } ;


[ GLOBAL , ASYNCHRONOUS ] FUNCTION mbx_create (    name		: mbx_name_type ;
						   permanent	: perm_flag_type ;
						   msg_size	: msg_size_type ;
						   msg_count	: msg_count_type ;
						   protection	: protection_type ;
						   VAR iochan	: [ VOLATILE ] mbx_chan_type ;
						   VAR device	: [ VOLATILE ] dev_name_type ) 
				: UNSIGNED ;

    {+}
    { Code for MBX/CREATE
    {-}

    VAR
	status		    : UNSIGNED ;
	local_channel_text  : [ STATIC , VOLATILE ] word_in_text ;
	local_equ_name	    : [ STATIC , VOLATILE ] equ_name_type ;

    [ ASYNCHRONOUS , UNBOUND ] FUNCTION create_mailbox : UNSIGNED ;

    {+}
    { Create mailbox in privileged mode, so that the I/O channel remains available after image exit.
    { We will do this in EXEC mode, altough SUPER would have been sufficient - but there is no $CMSUPER service ...
    {-}

        BEGIN {of function create_mailbox }

            create_mailbox := $CREMBX ( chan   := channel ,
					prmflg := mbx_permanent ,
				        maxmsg := mbx_msg_size ,
					bufquo := buffer_size ,
				        promsk := mbx_protection ,
				        acmode := PSL$C_SUPER ,
				        lognam := mbx_log_name )

	END { of function create_mailbox } ;

    BEGIN { of function mbx_create }

    {+}
    { Take local copies in static storage for all arguments to be passed to [ UNBOUND ] routines.
    { Those five arguments should not be touched anymore after this point.
    {-}

        mbx_log_name	:= name ;
        mbx_permanent   := UAND ( permanent , 1 )  ;	{ We look only at the last bit }
        mbx_msg_size    := msg_size ;
        mbx_msg_count   := msg_count ;
        mbx_protection  := protection ;

    {+}
    { Initialize the variables to be returned to our caller to something sensible,
    { in case we would not proceed to a successful end
    {-}

	iochan := 0 ;
	device := '' ;
	local_equ_name := '' ;

    {+}
    { Initialize our temporary storage
    {-}

	channel := 0 ;    
	mbx_dev_name := '' ;

    {+}
    { Compute the mailbox size
    {-}

        buffer_size := mbx_msg_size * mbx_msg_count ;

    {+}
    { We do not want to create a mailbox if the logical name that the user has specified is already defined.
    { So, let's try to translate that logical name ... Of course we look in the table where the logical name
    { would go, i.e. LNM$TEMPORARY_MAILBOX for temporary mailboxes and LNM$PERMANENT_MAILBOX for the others.
    {-}

        IF mbx_permanent = 1 THEN table := 'LNM$PERMANENT_MAILBOX' ELSE table := 'LNM$TEMPORARY_MAILBOX' ;

        status := translate_logical (   logical_name := mbx_log_name ,
					equival_name := local_equ_name ,
					lntable_name := table ,
					attributes := LNM$M_CASE_BLIND ) ;

        CASE status OF 

    {+}
    { Logical is already there : we take no risk and abort.
    {-}

	    SS$_NORMAL  : mbx_create := MBX_NAMALRXTS ;

    {+}
    { OK, the logical does not exist; go ahead
    {-}

	    SS$_NOLOGNAM : BEGIN {of second case branch }

    {+}
    { Create the mailbox in executive mode
    {-}

		status := $CMEXEC ( routin := %IMMED create_mailbox , arglst := 0 ) ;
		IF status <> SS$_NORMAL

    {+}
    { The system service call failed.
    { We don't continue any further and return to our caller with the status that we received.
    {-}

		THEN mbx_create := status

    {+}
    { The service call was successful
    {-}

		ELSE BEGIN { of if-else branch }

    {+}
    { Define a process logical name to contain the channel number
    { so that we can use it later with MBX/READ and /WRITE commands.
    { We use a kernel mode logical so that we are -nearly- sure that nobody will mess it up.
    {-}

		    channel_log := mbx_log_name + suffix ;
		    local_channel_text := UDEC ( channel , word_unsigned_length , word_unsigned_length ) ;
		    table := 'LNM$PROCESS_TABLE' ;
		    status := define_kernel_logical (   logical_name := channel_log ,
							equival_name := local_channel_text ,
							lntable_name := table ) ;
		    IF ( status = SS$_NORMAL ) OR ( status = SS$_SUPERSEDE )

    {+}
    { The service call was successful.
    {-}

		    THEN BEGIN { of if-then branch }

    {+}
    { Obtain the mailbox physical name, which will be returned to our caller.
    {-}

			status := get_mailbox_name ( logical_name := mbx_log_name , physical_name := mbx_dev_name ) ;
			IF STATUS <> SS$_NORMAL

    {+}
    { The request failed.
    { We don't continue any further and return to our caller with the status that we received.
    {-}

			THEN mbx_create := status

    {+}
    { The service call was successful
    {-}

			ELSE BEGIN { of if-else branch }

    {+}
    { Return the channel number and device name to our caller, and give a success completion status
    {-}

			    iochan := channel ;
			    device := mbx_dev_name ;
			    mbx_create := MBX_MBXCRE

			END { of if-else branch }

		    END { of if-then branch }

    {+}
    { The system service call failed.
    { We don't continue any further and return to our caller with the status that we received.
    {-}

		    ELSE mbx_create := status

		END { of if-else branch }

	    END { of second case branch } ;

    {+}
    { Call to system service failed
    {-}

	    OTHERWISE mbx_create := status

	END { of case statement }

    END { of function mbx_create } ;


[ GLOBAL , ASYNCHRONOUS ] FUNCTION mbx_delete (    name		: mbx_name_type ;
						   VAR iochan	: [ VOLATILE ] mbx_chan_type ;
						   VAR device	: [ VOLATILE ] dev_name_type )
				    : UNSIGNED ;

    {+}
    { Code for MBX/DELETE
    {-}

    VAR status : UNSIGNED ;

    [ ASYNCHRONOUS , UNBOUND ] FUNCTION delete_mailbox : UNSIGNED ;

    {+}
    { Delete the mailbox. We will do that in executive mode, like we did for the creation.
    { As per the VMS documentation, we must also deassign the channels to the mailbox before the deletion takes place.
    { Actually, supervisor mode would have been enough, but there is no $CMSUPER system service.
    {-}

        VAR intstat : UNSIGNED ;

        BEGIN { of function delete_mailbox }

            intstat := $DELMBX ( chan := channel ) ;
	    IF intstat = SS$_NORMAL 
	    THEN delete_mailbox := $DASSGN ( chan := channel )
	    ELSE delete_mailbox := intstat

        END { of function delete_mailbox } ;

    BEGIN { of function mbx_delete }

    {+}
    { Take local copies in static storage for all arguments to be passed to [ UNBOUND ] routines.
    { This argument should not be touched anymore after this point.
    {-}

        mbx_log_name	:= name ;

    {+}
    { Initialize the variables to be returned to our caller to something sensible,
    { in case we would not proceed to a successful end
    {-}

	iochan := 0 ;
	device := '' ;

    {+}
    { Initialize our temporary storage
    {-}

	channel := 0 ;    
	mbx_dev_name := '' ;

    {+}
    { Test the mailbox name that we have received
    {-}

	table := 'LNM$FILE_DEV' ;
	status := test_mailbox ( mailbox_name := mbx_log_name , device_name := mbx_dev_name ) ;

	IF STATUS <> SS$_NORMAL

    {+}
    { Request failed
    {-}

	THEN mbx_delete := status

    {+}
    { Request successful
    {-}

	ELSE BEGIN { of if-else branch }

    {+}
    { Get the mailbox's channel number
    {-}

	    table := 'LNM$PROCESS_TABLE' ;
	    status := get_channel ( mailbox_name := mbx_log_name , 
				    channel_number := channel ,
				    channel_name := channel_log ) ;
	    CASE status OF

    {+}
    { Request successful
    {-}

		SS$_NORMAL	: BEGIN { of first case branch }

    {+}
    { Delete the process logical name where the channel number was retained.
    { We will close that channel, so we don't need that info anymore ...
    {-}

		    status := delete_kernel_logical ( logical_name := channel_log , lntable_name := table ) ;
		    IF status <> SS$_NORMAL 

    {+}
    { Call to system service failed
    {-}

		    THEN mbx_delete := status

    {+}
    { Execution of system service successful
    {-}

		    ELSE BEGIN {of if-else branch }


    {+}
    { Delete the mailbox now.
    {-}

			status := $CMEXEC ( routin := %IMMED delete_mailbox , arglst := 0 ) ;
			IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}
			THEN mbx_delete := status

    {+}
    { Call to system service successful
    {-}

			ELSE BEGIN { of if-else branch }

    {+}
    { Is the mailbox still there ? remember, permanent mailboxes are deleted only after the last channel
    { has been closed. So it might just be "marked for deletion" ...
    { To know that, we just re-try to get some info about this device and see if it still exists.
    {-}

			    device_class := 0 ;
			    WITH item_list DO
			    BEGIN
				length	:= 4 ;
				item	:= DVI$_DEVCLASS ;
				buffer	:= IADDRESS ( device_class ) ;
				retlen	:= 0
			    END ;

			    status := $GETDVIW (    itmlst := item_list ,
						    efn    := 0 ,
						    devnam := mbx_dev_name ,
						    iosb   := iostblk ) ;

			    IF status = SS$_NORMAL THEN status := iostblk.status ;	

			    CASE status OF

    {+}
    { Call to system service successful : our mailbox is still there, marked for deletion
    {-}

				SS$_NORMAL	: BEGIN { of first case branch }

    {+}
    { Return the (to be released) channel number and device name, and report condition
    {-}

				    iochan := channel ;
				    device := mbx_dev_name ;
				    mbx_delete := MBX_MBXMKDEL

				END { of first case branch } ;

    {+}
    { System service reports no logical name match : mailbox has been successfully deleted
    {-}

				SS$_NOSUCHDEV   : BEGIN { of second case branch }

    {+}
    { Return the (now released) channel number and device name, and report full success
    {-}

				    iochan := channel ;
				    device := mbx_dev_name ;
				    mbx_delete := MBX_MBXDEL

				END {of second case branch } ;

    {+}
    { Call to system service reported an error
    {-}

				OTHERWISE mbx_delete := status

			    END { of case statement }

			END { of if-else branch }

		    END { of if-else branch }

		END { of first case branch } ;

    {+}
    { No logical name found : we have no channel with that mailbox
    {-}

		SS$_NOLOGNAM	: mbx_delete := MBX_NOCHANATT ;

    {+}
    { Request failed
    {-}

		OTHERWISE mbx_delete := status

	    END { of case statement }

	END { of if-else branch }

    END { of function mbx_delete } ;


[ GLOBAL , ASYNCHRONOUS ] FUNCTION mbx_attach (    name		: mbx_name_type ;
						   VAR iochan	: [ VOLATILE ] mbx_chan_type ;
						   VAR device	: [ VOLATILE ] dev_name_type )
			    : UNSIGNED ;

    {+}
    { Code for MBX/ATTACH
    {-}

    VAR
	status		    : UNSIGNED ;
	local_channel_text  : [ STATIC , VOLATILE ] word_in_text ;

    [ ASYNCHRONOUS , UNBOUND ] FUNCTION assign_channel : UNSIGNED ;

    {+}
    { Assign channel to mailbox. We will do that in executive mode, so that the I/O channel can survive the image run down.
    { Actually, supervisor mode would have been enough, but there is no $CMSUPER system service.
    {-}

	BEGIN { of function assign_channel }

	    assign_channel := $ASSIGN ( devnam := mbx_log_name ,
					chan   := channel ,
					acmode := PSL$C_SUPER )

        END { of function assign_channel } ;


    BEGIN { of function mbx_attach }

    {+}
    { Take local copies in static storage for all arguments to be passed to [ UNBOUND ] routines.
    { This argument should not be touched anymore after this point.
    {-}

        mbx_log_name	:= name ;

    {+}
    { Initialize the variables to be returned to our caller to something sensible,
    { in case we would not proceed to a successful end
    {-}

	iochan := 0 ;
	device := '' ;

    {+}
    { Initialize our temporary storage
    {-}

	channel := 0 ;    
	mbx_dev_name := '' ;

    {+}
    { Test the mailbox name that we have received
    {-}

	table := 'LNM$FILE_DEV' ;
	status := test_mailbox ( mailbox_name := mbx_log_name , device_name := mbx_dev_name ) ;

	IF STATUS <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

	THEN mbx_attach := status

    {+}
    { Call to system service successful
    {-}

	ELSE BEGIN { of if-else branch }

    {+}
    { Get the mailbox's channel number
    {-}

	    table := 'LNM$PROCESS_TABLE' ;
	    status := get_channel ( mailbox_name := mbx_log_name , 
				    channel_number := channel ,
				    channel_name := channel_log ) ;
	    CASE status OF

    {+}
    { Logical name already exists, in the right table (process), at the right mode (kernel), and fits in a
    { 5 positions character string. So most probably there is already a channel between our process and that
    { mailbox. As we don't want two, we exit with a WARNING status.
    {-}

		SS$_NORMAL : BEGIN { of first case branch }

    {+}
    { Return device name and already existing channel number, and report the condition
    {-}

		    device := mbx_dev_name ;
		    iochan := channel ;
		    mbx_attach := MBX_MBXALRATT

		END { of first case branch } ;
		
    {+}
    { At this point, we are sure we have not attached that mailbox already.
    {-}
		SS$_NOLOGNAM : BEGIN { of second case branch }

    {+}
    { Get a supervisor-mode channel to the specified mailbox.
    {-}

		    status := $CMEXEC ( routin := %IMMED assign_channel , arglst := 0 ) ;
		    IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

		    THEN mbx_attach := status

    {+}
    { Call to system service successful
    {-}

		    ELSE BEGIN { of if-else branch }

    {+}
    { Define a process logical name to contain the channel number
    { so that we can use it later with MBX/READ and /WRITE commands.
    { The logical is kernel so that we are -nearly- sure nobody will mess it up.
    {-}

			local_channel_text := UDEC ( channel , word_unsigned_length , word_unsigned_length ) ;
			status := define_kernel_logical (   logical_name := channel_log ,
							    equival_name := local_channel_text ,
							    lntable_name := table ) ;

			IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

			THEN mbx_attach := status

    {+}
    { Call to system service successful
    {-}

			ELSE BEGIN { of if-else branch }

    {+}
    { Report device name and channel number to caller, and exit with success condition
    {-}

			    device := mbx_dev_name ;
			    iochan := channel ;
			    mbx_attach := MBX_MBXATT

			END { of if-else branch }

		    END { of if-else branch }

		END { of second case branch } ;

    {+}
    { Call to system service reported an error
    {-}

		OTHERWISE mbx_attach := status

	    END { of case statement }

	END { of if-else branch }

    END { of function mbx_attach } ;


[ GLOBAL , ASYNCHRONOUS ] FUNCTION mbx_detach (    name		: mbx_name_type ;
						   VAR iochan	: [ VOLATILE ] mbx_chan_type ;
						   VAR device	: [ VOLATILE ] dev_name_type )
			    : UNSIGNED ;

    {+}
    { Code for MBX/DETACH
    {-}

    VAR status : UNSIGNED ;

    [ ASYNCHRONOUS , UNBOUND ] FUNCTION deassign_channel : UNSIGNED ;

    {+}
    { Deassign the mailbox channel. We will do that in executive mode, because the channel was a supervisor one.
    { Actually, supervisor mode would have been enough, but there is no $CMSUPER system service.
    {-}

	BEGIN { of function deassign_channel }

	    deassign_channel := $DASSGN ( chan := channel )

	END { of function deassign_channel } ;

    BEGIN { of function mbx_detach }

    {+}
    { Take local copies in static storage for all arguments to be passed to [ UNBOUND ] routines.
    { This argument should not be touched anymore after this point.
    {-}

        mbx_log_name := name ;

    {+}
    { Initialize the variables to be returned to our caller to something sensible,
    { in case we would not proceed to a successful end
    {-}

	iochan := 0 ;
	device := '' ;

    {+}
    { Initialize our temporary storage
    {-}

	channel := 0 ;    
	mbx_dev_name := '' ;

    {+}
    { Test the mailbox name that we have received
    {-}

	table := 'LNM$FILE_DEV' ;
	status := test_mailbox ( mailbox_name := mbx_log_name , device_name := mbx_dev_name ) ;

	IF STATUS <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

	THEN mbx_detach := status

    {+}
    { Call to system service successful
    {-}

	ELSE BEGIN { of if-else branch }

    {+}
    { Get the mailbox's channel number
    {-}

	    table := 'LNM$PROCESS_TABLE' ;
	    status := get_channel ( mailbox_name := mbx_log_name , 
				    channel_number := channel ,
				    channel_name := channel_log ) ;
	    CASE status OF

    {+}
    { Logical name defined - let's proceed
    {-}

		SS$_NORMAL	: BEGIN { of first case branch }

    {+}
    { Delete the process logical name where the channel number was retained.
    { We will close that channel, so we don't need that info anymore ...
    {-}

		    status := delete_kernel_logical ( logical_name := channel_log , lntable_name := table ) ;
		    IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

		    THEN mbx_detach := status

    {+}
    { Call to system service successful
    {-}

		    ELSE BEGIN { of if-else branch }

    {+}
    { Close that channel
    {-}

			status := $CMEXEC ( routin := %IMMED deassign_channel , arglst := 0 ) ;
			IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

			THEN mbx_detach := status

    {+}
    { Call to system service successful
    {-}

			ELSE BEGIN { of if-else branch }

    {+}
    { rerurn device and channel number to caller, and return success status
    {-}

			    device := mbx_dev_name ;
			    iochan := channel ;
			    mbx_detach := MBX_MBXDET

			END { of if-else branch }

		    END { of if-else branch }

		END { of second case branch } ;

    {+}
    { Logical name not defined - the mailbox exists but is not attached
    {-}

		SS$_NOLOGNAM    : mbx_detach := MBX_NOCHANATT ;

    {+}
    { Call to system service returned an error
    {-}

		OTHERWISE mbx_detach := status

	    END { of case statement }

	END { of if-else branch }

    END { of function mbx_detach } ;


[ GLOBAL , ASYNCHRONOUS ] FUNCTION mbx_send (	name	    : mbx_name_type ;
						send_eof    : sendeof_flag := FALSE ; 
						synchronize : synch_flag := FALSE ;
						time_out    : time_out_type := 0 ;
						message	    : message_type ;
						VAR sendlen : [ VOLATILE ] string_length ;
						VAR device  : [ VOLATILE ] dev_name_type ;
						VAR pid	    : [ VOLATILE ] pid_type ) 
		: UNSIGNED ;

    {+}
    { Code for MBX/SEND
    {-}

    VAR
	status		    : UNSIGNED ;
	local_sendeof_flag  : [ STATIC , VOLATILE ] BOOLEAN ;
	local_synchro_flag  : [ STATIC , VOLATILE ] BOOLEAN ;

    [ ASYNCHRONOUS , UNBOUND ] FUNCTION send_msg : UNSIGNED ;

    {+}
    { As our channel was defined in supervisor mode, we have to perform our I/O at least at that
    { level too. As there is no $CMSUPER system service, we'll do that in executive ...
    {-}

        VAR intstat : UNSIGNED ;

        BEGIN { of function send_msg }

    {+}
    { Adjust the io function to execute, depending on the synchronize and send_eof requests
    {-}
	    
	    IF local_sendeof_flag THEN io_function := IO$_WRITEOF ELSE io_function := IO$_WRITEVBLK ;
	    IF NOT local_synchro_flag THEN io_function := UOR ( io_function , IO$M_NOW ) ;
	    io_function := UOR ( io_function , IO$M_NORSWAIT ) ;

    {+}
    { If requested, set a timer that will fire an AST at completion time
    {-}

	    intstat := SS$_NORMAL ;

	    IF time_out_req THEN intstat := $SETIMR (	daytim := time_out_bin ,
							astadr := cancel_io ,
							reqidt := 2 ) ;

	    IF intstat <> SS$_NORMAL

	    THEN send_msg := intstat

	    ELSE BEGIN { of if-else branch }

    {+}
    { Now, send the message
    {-}

		IF local_sendeof_flag

		THEN intstat := $QIOW (	chan    := channel ,
					efn	:= 0 ,
					func    := io_function ,
					iosb    := iostblk )

		ELSE intstat := $QIOW (	chan    := channel ,
					efn	:= 0 ,
					func    := io_function ,
					iosb    := iostblk ,
					p1	:= mbx_message.BODY ,
					p2	:= mbx_message.LENGTH ) ;

    {+}
    { Check status of I/O in both return status and IOSB
    {-}

		IF intstat = SS$_NORMAL THEN intstat := iostblk.status ;

		IF intstat <> SS$_NORMAL

		THEN send_msg := intstat

    {+}
    { Cancel timer request now, as our I/O has completed anyway
    {-}

		ELSE send_msg := $CANTIM ( reqidt := 2 , acmode := PSL$C_EXEC )

	    END { of if-else branch }
				      
	END { of function send_msg } ;


    BEGIN { of function mbx_send }

    {+}
    { Take local copies in static storage for all arguments to be passed to [ UNBOUND ] routines.
    { Those two arguments should not be touched anymore after this point.
    {-}

        mbx_log_name := name ;
	IF message.LENGTH <= max_message_length
	THEN mbx_message := message
	ELSE mbx_message := SUBSTR ( message , 1 , max_message_length ) ;
	local_sendeof_flag := send_eof ;
	local_synchro_flag := synchronize ;

    {+}
    { Initialize the variables to be returned to our caller to something sensible,
    { in case we would not proceed to a successful end.
    {-}

	sendlen := 0 ;
	device := '' ;
	pid := 0 ;

    {+}
    { Initialize our temporary storage
    {-}

	channel := 0 ;    

    {+}
    { Process optional time_out request
    {-}

	time_out_req := time_out <> 0 ;
	status := convert_time ( seconds := time_out , time_machine := time_out_bin ) ;
	IF status <> SS$_NORMAL

    {+}
    { The time conversion failed, for whatever reason. Report the failure, and don't continue any further
    {-}

	THEN mbx_send := status

    {+}
    { Either there was no time to convert, or the conversion was successful. Let's continue the game.
    {-}

	ELSE BEGIN { of if-else branch }

    {+}
    { Get the mailbox physical name - we will need it later on
    {-}

    {+}
    { Test the mailbox name that we have received
    {-}

	    table := 'LNM$FILE_DEV' ;
	    status := test_mailbox ( mailbox_name := mbx_log_name , device_name := mbx_dev_name ) ;

	    IF STATUS <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

	    THEN mbx_send := status

    {+}
    { Call to system service successful
    {-}

	    ELSE BEGIN { of if-else branch }

    {+}
    { Get the mailbox's channel number
    {-}

		table := 'LNM$PROCESS_TABLE' ;
		status := get_channel ( mailbox_name := mbx_log_name , 
					channel_number := channel ,
					channel_name := channel_log ) ;

		CASE status OF

    {+}
    { We got a channel. Good, let's proceed further
    {-}

		    SS$_NORMAL	    : BEGIN { of first case branch }

    {+}
    { Get the maximim buffer size of the mailbox. If it is smaller than our message, truncate the message
    {-}

			status := get_mbx_bufsize ( mailbox_name := mbx_log_name , size := buffer_size ) ;
			IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

			THEN mbx_send := status 

    {+}
    { Everything's fine, let's continue
    {-}

			ELSE BEGIN { of if-else branch }

			    IF mbx_message.LENGTH > buffer_size 
						THEN mbx_message := SUBSTR ( mbx_message , 1 , buffer_size ) ;

    {+}
    { Execute requested operation
    {-}

			    status := $CMEXEC ( routin := %IMMED send_msg , arglst := 0 ) ;
			    CASE status OF

    {+}
    { Call to system service successful
    {-}

				SS$_NORMAL		    : BEGIN { of first case branch }

    {+}
    { Return the reader's pid and device name
    {-}

				    device := mbx_dev_name ;
				    pid := iostblk.info ;

    {+}
    { What did we sent ?
    {-}

				    IF send_eof

    {+}
    { EOF marker
    {-}

				    THEN BEGIN
					mbx_send := MBX_EOFSNT ;
					sendlen := 0
				    END

    {+}
    { Message
    {-}

				    ELSE BEGIN
					sendlen := iostblk.count ;

    {+}
    { Could send everything ? If yes, report success, if not, report truncation
    {-}

					IF sendlen < message.LENGTH 
					THEN mbx_send := MBX_TRCMSGSNT
					ELSE mbx_send := MBX_MSGSNT
				    END

				END { of first case branch } ;

    {+}
    { Timer expired
    {-}

				SS$_CANCEL, SS$_ABORT   : BEGIN { of second case branch }

				    pid := 0 ;
				    sendlen := 0 ;
				    device := mbx_dev_name ;
				    mbx_send := MBX_TIMMSGSNT

				END { of first case branch } ;

    {+}
    { Call to system service failed
    {-}

				OTHERWISE mbx_send := status

			    END { of case statement }

			END { of if-else branch }

		    END { of first case branch } ;

    {+}
    { Logical name not defined - means mailbox is not attached
    {-}

		    SS$_NOLOGNAM    : mbx_send := MBX_NOCHANATT ;

    {+}
    { Call to system service failed
    {-}

		    OTHERWISE mbx_send := status

		END { of case statement }

	    END { of if-else branch }

	END { of if-else branch }

    END { of function mbx_send } ;


[ GLOBAL , ASYNCHRONOUS ] FUNCTION mbx_receive (   name		: mbx_name_type ;
						   synchronize	: synch_flag := FALSE ;
						   time_out	: time_out_type := 0 ;
						   VAR message	: [ VOLATILE ] message_type ;
						   VAR reclen	: [ VOLATILE ] string_length ;
						   VAR device	: [ VOLATILE ] dev_name_type ;
						   VAR pid	: [ VOLATILE ] pid_type )
				    : UNSIGNED ;

    {+}
    { Code for MBX/RECEIVE
    {-}

    VAR status : UNSIGNED ;


    [ ASYNCHRONOUS , UNBOUND ] FUNCTION receive_msg : UNSIGNED ;

    {+}
    { As our channel was defined in supervisor mode, we have to perform our I/O at least at that
    { level too. As there is no $CMSUPER system service, we'll do that in executive ...
    {-}

        VAR intstat : UNSIGNED ;

	BEGIN { of function receive_msg }

	    intstat := SS$_NORMAL ;

    {+}
    { If requested, set a timer that will fire an AST at completion time
    {-}

	    IF time_out_req THEN intstat := $SETIMR (	daytim := time_out_bin ,
							astadr := cancel_io ,
							reqidt := 1 ) ;

	    IF intstat <> SS$_NORMAL

	    THEN receive_msg := intstat

	    ELSE BEGIN { of if-else branch }

    {+}
    { Now, read the message
    {-}

		intstat := $QIOW (  chan    := channel ,
				    efn	    := 0 ,
				    func    := io_function ,
				    iosb    := iostblk ,
				    p1	    := mbx_message.BODY ,
				    p2	    := mbx_message.LENGTH ) ;

    {+}
    { Check status of I/O in both return status and IOSB
    {-}

		IF intstat = SS$_NORMAL THEN intstat := iostblk.status ;

		IF intstat <> SS$_NORMAL

		THEN receive_msg := intstat 

    {+}
    { Cancel timer request now, as our I/O has completed anyway
    {-}

		ELSE receive_msg := $CANTIM ( reqidt := 1, acmode := PSL$C_EXEC )

	    END { of if-else branch }
				      
	END { of function receive_msg } ;


    BEGIN { of function mbx_receive } 

    {+}
    { Take local copies in static storage for all arguments to be passed to [ UNBOUND ] routines.
    { This argument should not be touched anymore after this point.
    {-}

        mbx_log_name := name ;

    {+}
    { Initialize the variables to be returned to our caller to something sensible,
    { in case we would not proceed to a successful end
    {-}

	reclen := 0 ;
	message	:= '' ;
	device := '' ;
	pid := 0 ;

    {+}
    { Initialize our temporary storage
    {-}

	channel := 0 ;    
	mbx_message := '' ;

    {+}
    { Did we get a request to synchronize with the sender process ?
    {-}
	    
	io_function := IO$_READVBLK ;
	IF NOT synchronize THEN io_function := UOR ( io_function , IO$M_NOW ) ;

    {+}
    { Process optional time out request
    {-}

	time_out_req := time_out <> 0 ;
	status := convert_time ( seconds := time_out , time_machine := time_out_bin ) ;
	IF status <> SS$_NORMAL

    {+}
    { The time conversion failed, for whatever reason. Report the failure, and don't continue any further
    {-}

	THEN mbx_receive := status

    {+}
    { Either there was no time to convert, or the conversion was successful. Let's continue the game.
    {-}

	ELSE BEGIN { of if-else branch }

    {+}
    { Test the mailbox name that we have received
    {-}

	    table := 'LNM$FILE_DEV' ;
	    status := test_mailbox ( mailbox_name := mbx_log_name , device_name := mbx_dev_name ) ;
	    IF STATUS <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

	    THEN mbx_receive := status

    {+}
    { Call to system service successful
    {-}

	    ELSE BEGIN { of if-else branch }

    {+}
    { Get the mailbox's channel number
    {-}

		table := 'LNM$PROCESS_TABLE' ;
		status := get_channel ( mailbox_name := mbx_log_name , 
					channel_number := channel ,
					channel_name := channel_log ) ;

		CASE status OF

    {+}
    { We got a channel. Good, let's proceed further
    {-}

		    SS$_NORMAL	: BEGIN { of first case branch }

    {+}
    { Get the max message size of the mailbox, so that we can initialize the output buffer.
    { We need to do that because if the output buffer is declared larger than the mailbox message
    { size, (parameter P2 of the $QIOW) the I/O completes with a 'mailbox too small for operation' error.
    {-}

			buffer_size := 0 ;
			status := get_mbx_bufsize ( mailbox_name := mbx_log_name , size := buffer_size ) ;
			IF status <> SS$_NORMAL

    {+}
    { Call to system service failed
    {-}

			THEN mbx_receive := status

    {+}
    { Call to system service successful
    {-}

			ELSE BEGIN { of if-else branch }

    {+}
    { Initialize receive buffer
    {-}

			    mbx_message := PAD ( ' ' , ' ' , buffer_size ) ;

    {+}
    { Get the message from the mailbox
    {-}

			    status := $CMEXEC ( routin := %IMMED receive_msg , arglst := 0 ) ;
			    CASE status OF

    {+}
    { Got a message
    {-}

				SS$_NORMAL : BEGIN { of first case branch }
				    mbx_message.LENGTH := iostblk.count ;
				    reclen := iostblk.count ;
				    device := mbx_dev_name ;
				    pid := iostblk.info ;

    {+}
    { Have enough space to store it ?
    {-}

				    IF mbx_message.LENGTH <= max_message_length

    {+}
    { Yes - copy to result area, and return success code
    {-}

				    THEN BEGIN
					message := mbx_message ;
					mbx_receive := MBX_MSGRCV
				    END

    {+}
    { No - truncate output to size of buffer provided by caller
    {-}

				    ELSE BEGIN
					message := SUBSTR ( mbx_message , 1 , max_message_length ) ;
					mbx_receive := MBX_TRCMSGRCV
				    END

				END { of first case branch } ;

    {+}
    { Got an EOF mark
    {-}

				SS$_ENDOFFILE		: BEGIN
				    pid := iostblk.info ;
				    reclen := 0 ;
				    device := mbx_dev_name ;
				    message := '' ;
				    mbx_receive := MBX_EOFRCV
				END ;

    {+}
    { Timer expired
    {-}

				SS$_CANCEL, SS$_ABORT	: BEGIN
				    pid := 0 ;
				    reclen := 0 ;
				    device := mbx_dev_name ;
				    message := '' ;
				    mbx_receive := MBX_TIMMSGRCV
				END ;

    {+}
    { Got an error
    {-}

				OTHERWISE mbx_receive := status

			    END { of case statement }

			END { of if-else branch }

		    END { of first case branch } ;

    {+}
    { Logical name not defined - means mailbox is not attached
    {-}

		    SS$_NOLOGNAM    : mbx_receive := MBX_NOCHANATT ;

    {+}
    { Request failed
    {-}

		    OTHERWISE mbx_receive := status

		END { of case statement }

	    END { of if-else branch }

	END { of if-else branch }

    END { of function mbx_receive } ;

END { of module mbx_routines } .
