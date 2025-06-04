[ 
IDENT ( 'MBX V1.2' ) , 
INHERIT ( 
    'sys$library:pascal$cli_routines' ,
    'sys$library:pascal$lib_routines' ,
    'sys$library:starlet' ,
    'int:mbx_declarations' ,
    'int:mbx_messages' ) 
]
PROGRAM mbx_cli ( OUTPUT ) ;

{ © Marc Van Dyck, 19-MAY-1999 }


CONST

    long_unsigned_length    = 10 ;				{ length of string '4294967294', max unsigned longword }
    max_symbol_length	    = 255 ;				{ maximum length of a DCL symbol name }
    max_dcl_string_length   = 1024 ;				{ maximum length of a DCL symbol string value }

TYPE

    logging_modes   = ( full , mini , none ) ;
    action	    = ( create , delete , attach , detach , send , receive ) ;
    prot_text	    = VARYING [4] OF CHAR ;
    acc_zones	    = ( system , owner , group , world ) ;
    acc_types	    = ( read , write , physical , logical ) ;
    prot_mask_type  = RECORD CASE BOOLEAN OF
			   TRUE  : ( bulk  : protection_type ; ) ;
			   FALSE : ( table : [ BYTE (2) ] PACKED ARRAY [acc_zones , acc_types] OF [ BIT (1) ] BOOLEAN ; ) ;
		      END;

VAR

    {+}
    { Local stuff - types declared locally
    {-}

    action_code		    : action ;
    action_text		    : VARYING [7] OF CHAR ;
    status, status1	    : UNSIGNED ;
    logging		    : logging_modes ;
    filter		    : BOOLEAN ;
    msg_size_text	    : VARYING [long_unsigned_length] OF CHAR ;
    msg_count_text	    : VARYING [long_unsigned_length] OF CHAR ;
    timer_text		    : VARYING [long_unsigned_length] OF CHAR ;
    timer		    : UNSIGNED ;
    buffer_size		    : UNSIGNED ;
    prot_string		    : prot_text ;
    msg_symbol		    : VARYING [max_symbol_length] OF CHAR ;
    pid_symbol		    : VARYING [max_symbol_length] OF CHAR ;
    get_pid		    : BOOLEAN ;
    put_eof		    : BOOLEAN ;
    message_buffer	    : VARYING [max_dcl_string_length] OF CHAR ;

    {+}
    { Passed to the MBX API - types inherited from the MBX API environment
    {-}

    mbx_name		    : mbx_name_type ;
    perm_flag		    : perm_flag_type ;
    msg_size		    : msg_size_type ;
    msg_count		    : msg_count_type ;
    prot_mask		    : prot_mask_type ;
    channel		    : mbx_chan_type ;
    dev_name		    : dev_name_type ;
    synch		    : synch_flag ;
    bytes_received	    : string_length ;
    bytes_sent		    : string_length ;
    writer_id		    : pid_type ;
    reader_id		    : pid_type ;


PROCEDURE parse_create ;

    PROCEDURE eval_prot ( zone : acc_zones ; text : prot_text ; VAR mask : prot_mask_type ) ;

    {+}
    { Build a 16 bits protection mask for the mailbox.
    { Each procedure call will build 4 of the 16 bits, so it will be called 4 times.
    { Attention, as per the $CREMBX documentation, the access is allowed if the bit is 0.
    { Physical access is irrelevant, and logical access must always be granted.
    {-}

        VAR 
	    i : UNSIGNED ;
	    prot_chars    : SET OF CHAR ;

        BEGIN { of procedure eval_prot }

    {+}
    { Syntax check - field must be made of letters RWLP only, and use each letter only once.}
    {-}

	    prot_chars := [ 'R' , 'W' , 'P' , 'L' ] ;
	    FOR i := 1 TO text.LENGTH DO
	    BEGIN
		IF NOT (text [i] IN prot_chars) THEN 
		    LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , MBX_MBXINVPRT , 1 , %STDESCR text ) ;
		prot_chars := prot_chars - [ text [i] ]
	    END ;

    {+}
    { Build protection mask - We look only at R and W - P is ignored and L must be set if W or R is requested.
    { Remember, TRUE (1) means DISABLED and FALSE (0) means ENABLED. The guy in VMS engineering who invented this
    { must be sacked.
    {-}

	    IF text = '' THEN mask.table [zone , logical] := TRUE ELSE mask.table [zone , logical] := FALSE ;
	    mask.table [zone , physical] := TRUE ;
	    IF INDEX ( text , 'W' ) <> 0 THEN mask.table [zone , write] := FALSE ELSE mask.table [zone , write] := TRUE ;
	    IF INDEX ( text , 'R' ) <> 0 THEN mask.table [zone , read] := FALSE ELSE mask.table [zone , read] := TRUE

	END { of procedure eval_prot } ;

    BEGIN {of procedure parse_create }

    {+}
    { Get mailbox name
    {-}

	status := CLI$GET_VALUE ( 'MBX' , mbx_name.BODY , mbx_name.LENGTH ) ;
        IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MBXNOTCRE , 0 , status ) ;

    {+}
    { Temporary or permanent ? }
    {-}

        status := CLI$PRESENT ( 'PERMANENT' ) ;
        CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES			: perm_flag := 1 ;
	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : perm_flag := 0 ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
        END ;

    {+}
    { Maximum message size
    {-}

        status := CLI$GET_VALUE ( 'MESSAGE_SIZE' , msg_size_text.BODY , msg_size_text.LENGTH ) ;
        IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status ) ;
        READV ( msg_size_text , msg_size , ERROR := CONTINUE ) ;
        IF STATUSV <> 0 THEN
		LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , MBX_MSGINVSIZ , 1 , %STDESCR msg_size_text ) ;
        IF ( msg_size < 1 ) OR ( msg_size > max_message_length ) THEN
		LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , MBX_MSGINVSIZ , 1 , %STDESCR msg_size_text ) ;

    {+}
    { Maximum message count
    {-}

        status := CLI$GET_VALUE ( 'POSITIONS' , msg_count_text.BODY , msg_count_text.LENGTH ) ;
        IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status ) ;
        READV ( msg_count_text , msg_count , ERROR := CONTINUE ) ;
        IF STATUSV <> 0 THEN 
	    LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , MBX_MSGINVCNT , 1 , %STDESCR msg_count_text ) ;
        IF msg_count < 1 THEN
	    LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , MBX_MSGINVCNT , 1 , %STDESCR msg_count_text ) ;

    {+}
    { From the two parameters above, we compute the mailbox size
    {-}

        buffer_size := msg_size * msg_count ;
        IF buffer_size > 65535 THEN
	    LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , MBX_MBXTOOBIG , 3 , buffer_size , msg_count , msg_size ) ;

    {+}
    { UIC-based protection
    {-}

        status := CLI$PRESENT ( 'PROTECTION' ) ;
        CASE status of
	    CLI$_PRESENT , CLI$_LOCPRES : BEGIN

    {+}
    { - SYSTEM protection
    {-}

		prot_string := '' ;
		status := CLI$GET_VALUE ( 'SYSTEM' , prot_string.BODY , prot_string.LENGTH ) ;
		CASE status OF
		    SS$_NORMAL  : eval_prot ( system , prot_string , prot_mask ) ;
		    CLI$_ABSENT : eval_prot ( system , '' , prot_mask ) ;
		    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	        END ;

    {+}
    { - OWNER protection
    {-}

		prot_string := '' ;
		status := CLI$GET_VALUE ( 'OWNER' , prot_string.BODY , prot_string.LENGTH ) ;
		CASE status OF
		    SS$_NORMAL  : eval_prot ( owner , prot_string , prot_mask ) ;
		    CLI$_ABSENT : eval_prot ( owner , '' , prot_mask ) ;
		    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
		END ;

    {+}
    { - GROUP protection
    {-}

		prot_string := '' ;
		status := CLI$GET_VALUE ( 'GROUP' , prot_string.BODY , prot_string.LENGTH ) ;
		CASE status OF
		    SS$_NORMAL  : eval_prot ( group , prot_string , prot_mask ) ;
		    CLI$_ABSENT : eval_prot ( group , '' , prot_mask ) ;
		    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
		END ;

    {+}
    { - World protection
    {-}

		prot_string := '' ;
		status := CLI$GET_VALUE ( 'WORLD' , prot_string.BODY , prot_string.LENGTH ) ;
		CASE status OF
		    SS$_NORMAL  : eval_prot ( world , prot_string , prot_mask ) ;
		    CLI$_ABSENT : eval_prot ( world , '' , prot_mask ) ;
		    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
		END

	    END { of case branch, when status = cli$_present or cli$_locpres } ;

    {+}
    { - No protection specified, or error status returned by cli$present
    {-}

	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : prot_mask.bulk := 0 ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status) 

        END { of case statement } ;

    {+}
    { Logging and filtering
    {-}

	status := CLI$PRESENT ( 'FILTER' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES                 : filter := TRUE ;
	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : filter := FALSE ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

	status := CLI$PRESENT ( 'LOG' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES     : logging := full ;
	    CLI$_ABSENT			    : logging := mini ;
	    CLI$_NEGATED , CLI$_LOCNEG	    : logging := none ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

    {+}
    { We know everything we need - let's create the mailbox }
    {-}

	status := mbx_create ( 	name	    := mbx_name , 
				permanent   := perm_flag , 
				msg_size    := msg_size , 
				msg_count   := msg_count , 
				protection  := prot_mask.bulk , 
				iochan	    := channel ,
				device	    := dev_name ) ;

	IF filter THEN dev_name := "_MBAxxxx:" ;

    {+}
    { Logging and signalling 
    {-}

	IF status <> MBX_MBXCRE
	THEN LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status , 0 )
	ELSE IF logging = full THEN LIB$SIGNAL ( MBX_MBXCRE , 3 , %STDESCR dev_name , msg_count , msg_size )

    END { of procedure parse_create } ;


PROCEDURE parse_delete ;

    BEGIN { of procedure parse_delete }

    {+}
    { Get mailbox name
    {-} 

        status := CLI$GET_VALUE ( 'MBX' , mbx_name.BODY , mbx_name.LENGTH ) ;
        IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MBXNOTDEL , 0 , status ) ;

    {+}
    { Logging and filtering
    {-}

	status := CLI$PRESENT ( 'FILTER' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES                 : filter := TRUE ;
	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : filter := FALSE ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

	status := CLI$PRESENT ( 'LOG' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES     : logging := full ;
	    CLI$_ABSENT			    : logging := mini ;
	    CLI$_NEGATED , CLI$_LOCNEG	    : logging := none ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

    {+}
    { We know everyhing we need - delete the mailbox now
    {-}

	status := mbx_delete (	name	:= mbx_name ,
				iochan	:= channel ,
				device	:= dev_name ) ;

	IF filter
	THEN BEGIN
	    dev_name := "_MBAxxxx:" ;
	    channel := 0
	END ;

    {+}
    { Logging and signalling
    {-}

	CASE status OF

	    MBX_MBXDEL	    : IF logging = full THEN LIB$SIGNAL ( MBX_MBXDEL , 1 , %STDESCR dev_name ) ;

	    MBX_MBXMKDEL    : IF logging <> none THEN LIB$SIGNAL ( MBX_MBXMKDEL , 1 , %STDESCR dev_name ) ;

	    OTHERWISE	      LIB$SIGNAL ( MBX_MBXNOTDEL, 1 , %STDESCR mbx_name, status , 0 )

	END { of case statement }	    

    END { of procedure parse_delete } ;


PROCEDURE parse_attach ;

    BEGIN { of procedure parse_attach }

    {+}
    { Get mailbox name
    {-} 

	status := CLI$GET_VALUE ( 'MBX' , mbx_name.BODY , mbx_name.LENGTH ) ;
	IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MBXNOTATT , 0 , status ) ;

    {+}
    { Logging and filtering
    {-}

	status := CLI$PRESENT ( 'FILTER' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES                 : filter := TRUE ;
	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : filter := FALSE ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

	status := CLI$PRESENT ( 'LOG' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES     : logging := full ;
	    CLI$_ABSENT			    : logging := mini ;
	    CLI$_NEGATED , CLI$_LOCNEG	    : logging := none ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

    {+}
    { We know everything we need - attach the mailbox now
    {-}

	status := mbx_attach (  name    := mbx_name ,
				iochan  := channel ,
				device  := dev_name ) ;

	IF filter
	THEN BEGIN
	    dev_name := "_MBAxxxx:" ;
	    channel := 0
	END ;

    {+}
    { Logging and signalling
    {-}

	CASE status OF 
	    MBX_MBXATT	    : IF logging = full THEN LIB$SIGNAL ( MBX_MBXATT , 2 , channel , %STDESCR dev_name ) ;
	    MBX_MBXALRATT   : IF logging <> none THEN LIB$SIGNAL ( MBX_MBXALRATT , 2 , %STDESCR dev_name , channel ) ;
	    OTHERWISE	      LIB$SIGNAL (MBX_MBXNOTATT , 1 , %STDESCR mbx_name , status , 0 )
	END { of case statement }

    END { of procedure parse_attach } ;


PROCEDURE parse_detach ;

    BEGIN { of procedure parse_detach }

    {+}
    { Get mailbox name
    {-} 

	status := CLI$GET_VALUE ( 'MBX' , mbx_name.BODY , mbx_name.LENGTH ) ;
	IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MBXNOTDET , 0 , status ) ;

    {+}
    { Logging and filtering
    {-}

	status := CLI$PRESENT ( 'FILTER' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES                 : filter := TRUE ;
	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : filter := FALSE ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

	status := CLI$PRESENT ( 'LOG' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES     : logging := full ;
	    CLI$_ABSENT			    : logging := mini ;
	    CLI$_NEGATED , CLI$_LOCNEG	    : logging := none ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

    {+}
    { We know everything we need - detach the mailbox now
    {-}

	status := mbx_detach (	name	:= mbx_name ,
				iochan	:= channel ,
				device	:= dev_name ) ;    

	IF filter
	THEN BEGIN
	    dev_name := "_MBAxxxx:" ;
	    channel := 0
	END ;

    {+}
    { Logging and siganlling
    {-}

	IF status <> MBX_MBXDET
	THEN LIB$SIGNAL ( MBX_MBXNOTDET, 1 , %STDESCR mbx_name , status , 0 )
	ELSE IF logging = full THEN LIB$SIGNAL ( MBX_MBXDET , 2 , channel , %STDESCR dev_name )
				
    END { of procedure parse_detach } ;	
				

PROCEDURE parse_receive ;

    BEGIN

    {+}
    { Get mailbox name
    {-} 

	status := CLI$GET_VALUE ( 'MBX' , mbx_name.BODY , mbx_name.LENGTH ) ;
	IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTRCV , 0 , status ) ;

    {+}
    { Get name of symbol to receive message, and test its validity.
    {-}

	status := CLI$GET_VALUE ( 'SYM' , msg_symbol.BODY , msg_symbol.LENGTH ) ;
	IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , status ) ;
	message_buffer := '' ;
	status := LIB$SET_SYMBOL (  symbol := msg_symbol ,
				    value_string := message_buffer ,
				    table_type_indicator := LIB$K_CLI_LOCAL_SYM ) ;
	IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status , 0 ) ;

    {+}
    { Synchronize with sender ?
    {-}

	status := CLI$PRESENT ( 'WAIT' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES			: BEGIN { of case branch }

		synch := TRUE ;
		status := CLI$GET_VALUE ( 'WAIT' , timer_text.BODY , timer_text.LENGTH ) ;
		CASE status OF

		    SS$_NORMAL	    : BEGIN
			READV ( timer_text, timer, ERROR := CONTINUE ) ;
			IF STATUSV <> 0 THEN LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , 
									MBX_MBXINVTIM , 1 , %STDESCR timer_text )
		    END ;

		    CLI$_ABSENT    : timer := 0 ;
		    OTHERWISE	      LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , status )

		END { of case statement }

	    END { of case branch } ;
	
	    CLI$_ABSENT , CLI$_NEGATED, CLI$_LOCNEG	: BEGIN
		synch := FALSE ;
		timer := 0
	    END ;

	    OTHERWISE LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , status )

	END { of case statement } ;

    {+}
    { Ask for PID of sender ? If yes, test the validity of the symbol supplied.
    {-}

	status := CLI$GET_VALUE ( 'PID' , pid_symbol.BODY , pid_symbol.LENGTH ) ;
	CASE status OF

	    SS$_NORMAL	: BEGIN
		get_pid := TRUE ;
		status := LIB$SET_SYMBOL (  symbol := pid_symbol ,
					    value_string := '00000000' ,
					    table_type_indicator := LIB$K_CLI_LOCAL_SYM ) ;
		IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status , 0 )
	    END;

	    CLI$_ABSENT	: get_pid := FALSE ;

	    OTHERWISE	  LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , status )

	END ;

    {+}
    { Logging and filtering
    {-}

	status := CLI$PRESENT ( 'FILTER' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES                 : filter := TRUE ;
	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : filter := FALSE ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

	status := CLI$PRESENT ( 'LOG' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES     : logging := full ;
	    CLI$_ABSENT			    : logging := mini ;
	    CLI$_NEGATED , CLI$_LOCNEG	    : logging := none ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

    {+}
    { We know everything we need - get the message now
    {-}

	message_buffer := '' ;
	IF ( logging <> none ) AND synch THEN LIB$SIGNAL ( MBX_MSGWRWAIT , 1 , %STDESCR mbx_name ) ;
	status := mbx_receive (	name	    := mbx_name ,
				synchronize := synch ,
				time_out    := timer ,
				message	    := message_buffer ,
				reclen	    := bytes_received ,
				device	    := dev_name ,
				pid	    := writer_id ) ;

	IF filter THEN dev_name := "_MBAxxxx:" ;

    {+}
    { Logging and signalling
    {-}

        CASE status OF

    {+}
    { Got some sensible result
    {-}

	    MBX_TIMMSGRCV , MBX_EOFRCV , MBX_MSGRCV , MBX_TRCMSGRCV : BEGIN { of case branch }

    {+}
    { Return message received
    {-}

	        status1 := LIB$SET_SYMBOL (  symbol := msg_symbol , 
					    value_string := message_buffer , 
					    table_type_indicator := LIB$K_CLI_LOCAL_SYM ) ;
		IF status1 <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , status1 , 0 ) ;

    {+}
    { If requested, return writer pid
    {-}

		IF get_pid
		THEN BEGIN
		    status1 := LIB$SET_SYMBOL (	symbol := pid_symbol ,
						value_string := HEX ( writer_id , 8 , 8 ) ,
						table_type_indicator := LIB$K_CLI_LOCAL_SYM ) ;
		    IF status1 <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , status1 , 0 )
		END ;

    {+}
    { Log
    {-}

		CASE status OF

		    MBX_MSGRCV	    : 

			IF logging = full THEN LIB$SIGNAL ( MBX_MSGRCV , 2 , message_buffer.LENGTH , %STDESCR dev_name ) ;

		    MBX_EOFRCV	    : BEGIN

			LIB$SIGNAL ( MBX_EOFRCV , 1 , %STDESCR dev_name ) ;
			$EXIT ( UOR ( SS$_ENDOFFILE , 2 ** 28 ) )
			{ bit 28 suppresses output }

		    END ;

		    MBX_TRCMSGRCV   : BEGIN

			LIB$SIGNAL ( MBX_TRCMSGRCV , 3 , bytes_received , message_buffer.LENGTH , %STDESCR dev_name ) ;
			$EXIT ( UOR ( SS$_RESULTOVF - 2 , 2 ** 28 ) )
			{ SS$_RESULTOVF is a FATAL, we want just ERROR => -2 ; bit 28 suppresses output }

		    END ;

		    MBX_TIMMSGRCV   : BEGIN

			LIB$SIGNAL ( MBX_TIMMSGRCV , 1 , %STDESCR dev_name ) ;
			$EXIT ( UOR ( SS$_TIMEOUT - 2 , 2 ** 28 ) )
			{ SS$_TIMEOUT is a FATAL, we want just ERROR => -2 ; bit 28 suppresses output }

		    END

		END { of case statement } ;

	    END { of case branch } ;

    {+}
    { Damned ! it did not work !
    {-}

	    OTHERWISE LIB$SIGNAL ( MBX_MSGNOTRCV , 1 , %STDESCR mbx_name , status , 0 )

	END { of case statement }

    END { of procedure parse_receive } ;


PROCEDURE parse_send ;

    BEGIN { of procedure parse_send }

    {+}
    { Get mailbox name
    {-} 

	status := CLI$GET_VALUE ( 'MBX' , mbx_name.BODY , mbx_name.LENGTH ) ;
	IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 0 , status ) ;

    {+}
    { Send direct message, message from symbol,  or end of file marker ?
    {-}

        status := CLI$PRESENT ( 'EOF' ) ;
	CASE status OF

    {+}
    { => Send EOF marker
    {-}

	    CLI$_PRESENT , CLI$_LOCPRES			: BEGIN { of case branch }

		put_eof := TRUE ;
		message_buffer := ''

	    END { of case branch } ;

    {+}
    { Not an EOF marker => some sort of message
    {-}

	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG	: BEGIN { of case branch }

		put_eof := FALSE ;
		message_buffer  := PAD ( ' ' , ' ' , max_dcl_string_length ) ;

		status := CLI$PRESENT ( 'SYMBOL' ) ;
		CASE status OF

    {+}
    { Send a message from a symbol
    {-}

		    CLI$_PRESENT , CLI$_LOCPRES			: BEGIN { of case branch }

			status := CLI$GET_VALUE ( 'SYMBOL' , msg_symbol.BODY , msg_symbol.LENGTH ) ;
			IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status ) ;

			status := LIB$GET_SYMBOL (  symbol := msg_symbol ,
						    resultant_string := message_buffer.BODY ,
						    resultant_length := message_buffer.LENGTH ) ;
			IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status )

		    END { of case branch } ;

    {+}
    { Send a direct message - type of input is $QUOTED_STRING - DCL does not remove the surrounding quotes, so we do it.
    {-}

		    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG	: BEGIN { of case branch }

			status := CLI$GET_VALUE ( 'MSG' , message_buffer.BODY , message_buffer.LENGTH ) ;
			IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status ) ;
			IF ( message_buffer [1] = '"' ) AND ( message_buffer [message_buffer.LENGTH] = '"' )
			    THEN message_buffer := SUBSTR ( message_buffer , 2 , message_buffer.LENGTH - 2 ) ;

		    END { of case branch } ;

    {+}
    { Something went wrong
    {-}

		    OTHERWISE LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status )

		END { of case statement }

	    END { of case branch } ;

    {+}
    { Something went wrong
    {-}

	    OTHERWISE LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status ) 

	END { of case statement } ;

    {+}
    { Synchronize with receiver ?
    {-}

	status := CLI$PRESENT ( 'WAIT' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES			: BEGIN { of case branch }

		synch := TRUE ;
		status := CLI$GET_VALUE ( 'WAIT' , timer_text.BODY , timer_text.LENGTH ) ;
		CASE status OF

		    SS$_NORMAL	    : BEGIN
			READV ( timer_text, timer, ERROR := CONTINUE ) ;
			IF STATUSV <> 0 THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , 
									    MBX_MBXINVTIM , 1 , %STDESCR timer_text )
		    END ;

		    CLI$_ABSENT    : timer := 0 ;
		    OTHERWISE	      LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status )

		END { of case statement }

	    END { of case branch } ;
	
	    CLI$_ABSENT , CLI$_NEGATED, CLI$_LOCNEG	: BEGIN
		synch := FALSE ;
		timer := 0
	    END ;

	    OTHERWISE LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status )

	END { of case statement } ;

    {+}
    { Ask for PID of receiver ? If yes, test validity of the symbol supplied.
    {-}

	status := CLI$GET_VALUE ( 'PID' , pid_symbol.BODY , pid_symbol.LENGTH ) ;
	CASE status OF

	    SS$_NORMAL	: BEGIN
		get_pid := TRUE ;
		status := LIB$SET_SYMBOL (  symbol := pid_symbol ,
					    value_string := '00000000' ,
					    table_type_indicator := LIB$K_CLI_LOCAL_SYM ) ;
		IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status , 0 )
	    END ;
		
	    CLI$_ABSENT	: get_pid := FALSE ;

	    OTHERWISE	  LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status )

	END ;

    {+}
    { Logging and filtering
    {-}

	status := CLI$PRESENT ( 'FILTER' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES                 : filter := TRUE ;
	    CLI$_ABSENT , CLI$_NEGATED , CLI$_LOCNEG    : filter := FALSE ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

	status := CLI$PRESENT ( 'LOG' ) ;
	CASE status OF
	    CLI$_PRESENT , CLI$_LOCPRES     : logging := full ;
	    CLI$_ABSENT			    : logging := mini ;
	    CLI$_NEGATED , CLI$_LOCNEG	    : logging := none ;
	    OTHERWISE LIB$SIGNAL ( MBX_MBXNOTCRE , 1 , %STDESCR mbx_name , status )
	END ;

    {+}
    { Send a message to the mailbox
    {-}

	IF ( logging <> none ) AND synch THEN LIB$SIGNAL ( MBX_MSGRDWAIT , 1 , %STDESCR mbx_name ) ;
	status := mbx_send (	name	    := mbx_name ,
				send_eof    := put_eof ,
				synchronize := synch ,
				time_out    := timer ,
				message	    := message_buffer ,
				sendlen	    := bytes_sent ,
				device	    := dev_name ,
				pid	    := reader_id ) ;

	IF filter THEN dev_name := "_MBAxxxx:" ;

    {+}
    { Logging and signalling
    {-}

	CASE status OF

    {+}
    { Got some sensible result }
    {-}

	    MBX_EOFSNT , MBX_MSGSNT , MBX_TRCMSGSNT , MBX_TIMMSGSNT : BEGIN

    {+}
    { Return reader's pid }	
    {-}

		IF get_pid
		THEN BEGIN
		    status1 := LIB$SET_SYMBOL ( symbol := pid_symbol ,
						value_string := HEX ( reader_id , 8 , 8 ) ,
						table_type_indicator := LIB$K_CLI_LOCAL_SYM ) ;
		    IF status1 <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status1 , 0 )
		END ;

    {+}
    { Log
    {-}

		CASE status OF

		    MBX_MSGSNT	    : 

			IF logging = full THEN LIB$SIGNAL ( MBX_MSGSNT , 2 , message_buffer.LENGTH , %STDESCR dev_name ) ;

		    MBX_EOFSNT	    : 

			LIB$SIGNAL ( MBX_EOFSNT , 1 , %STDESCR dev_name ) ;

		    MBX_TRCMSGSNT   : BEGIN

			LIB$SIGNAL ( MBX_TRCMSGSNT , 3 , bytes_sent , message_buffer.LENGTH , %STDESCR dev_name ) ;
			$EXIT ( UOR ( SS$_RESULTOVF - 2 , 2 ** 28 ) ) 
			{ SS$_RESULTOVF is FATAL, we just want ERROR => -2 ; bit 28 suppresses output }

		    END ;

		    MBX_TIMMSGSNT   : BEGIN

			LIB$SIGNAL ( MBX_TIMMSGSNT , 1, %STDESCR dev_name ) ;
			$EXIT ( UOR ( SS$_TIMEOUT - 2 , 2 ** 28 ) )
			{ SS$_TIMEOUT is a FATAL, we want just ERROR => -2 ; bit 28 suppresses output }

		    END

		END { of case statement }

	    END { of case branch } ;

    {+}
    { Heavens ! An error status !...
    {-}

	    OTHERWISE LIB$SIGNAL ( MBX_MSGNOTSNT , 1 , %STDESCR mbx_name , status , 0 )

	END { of case statement }

    END { of procedure parse_send } ;


BEGIN { of main program }

    {+}
    { Initialize
    {-}

    REWRITE ( output ) ;

    {+}
    { Get the DCL syntax that has been activated
    {-}

    status := CLI$GET_VALUE ( 'ACTION' , action_text.BODY , action_text.LENGTH ) ;
    IF status <> SS$_NORMAL THEN LIB$SIGNAL ( MBX_MBXINTERR , 0 , status ) ;
    READV ( action_text, action_code, ERROR := CONTINUE ) ;
    IF STATUSV <> 0 THEN LIB$SIGNAL ( MBX_MBXINTERR ) ;    

    {+}
    { Dispatch to syntax-specific section
    {-}

    CASE action_code OF

    create  : parse_create ;
    delete  : parse_delete ;
    attach  : parse_attach ;
    detach  : parse_detach ;
    receive : parse_receive ;
    send    : parse_send

    END { of case statement }

END { of main program } .
