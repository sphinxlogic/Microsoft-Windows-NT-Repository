%TITLE "PROCLIST"
%SBTTL "List all processes owned by this username"
%IDENT "V1.0"
PROGRAM PROCLIST

!+
! FACILITY:
! 
!   Miscelaneous Utilities
! 
! PROGRAM DESCRIPTION:
! 
!   List processes of this username and their subprocesses.
!
!   This is done so that the subprocess of a parent is listed immediately
!   after it's parent.
! 
! AUTHORS:
! 
!   David L. Kachelmyer
! 
! CREATION DATE: April 7, 1991
! 
! DESIGN ISSUES:
! 
!   None.  The subroutine LIST_PROCESSES does all of the work.
! 
! MODIFICATION HISTORY:
! 
! V1.0	    07-Apr-1991	    Dave Kachelmyer
!   Original program.
!
!-
    OPTION TYPE = EXPLICIT, &
	   SIZE = INTEGER LONG, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR, &
	   ACTIVE = INTEGER OVERFLOW
    %PAGE
    %SBTTL "Declarations"

    !+
    ! External Constants
    !-
    %INCLUDE "$jpidef" %FROM %LIBRARY
    EXTERNAL LONG CONSTANT ss$_normal

    !+
    ! External Routines
    !-
    EXTERNAL SUB list_processes (LONG, LONG, STRING)
    EXTERNAL LONG FUNCTION sys$getjpiw
    EXTERNAL LONG FUNCTION chksts

    !+
    ! RECORD Declarations
    !-
    RECORD item_list_3
    	GROUP item (1 TO 16)
	    VARIANT
	    	 CASE
		     WORD	buf_len
		     WORD	code
		     LONG	buf_address
		     LONG	len_address
		 CASE
		     LONG	terminator
	    END VARIANT
	END GROUP item
    END RECORD item_list_3

    RECORD io_status_block
    	WORD	iosb_field(1 TO 4)
    END RECORD io_status_block

    !+
    ! COMMON Declarations
    !-
    COMMON (alpha) LONG	jpi_flags, &
		   STRING 	state$(1 TO 14) = 5
    ! For one-time initialized values.
    
    !+
    ! MAP Declarations
    !-
    MAP (local1) LONG	stat%, &
			jpi_username_len%, &
		 item_list_3	jpi_list, &
		 io_status_block	iosb, &
		 STRING	jpi_username$ = 12

    %PAGE
    %SBTTL "Initialization"

    jpi_flags = jpi$m_ignore_target_status OR jpi$m_no_target_inswap

    state$(1) = "COLPG"
    state$(2) = "MWAIT"
    state$(3) = "CEF"
    state$(4) = "PFW"
    state$(5) = "LEF"
    state$(6) = "LEFO"
    state$(7) = "HIB"
    state$(8) = "HIBO"
    state$(9) = "SUSP"
    state$(10) = "SUSPO"
    state$(11) = "FPG"
    state$(12) = "COM"
    state$(13) = "COMO"
    state$(14) = "CUR"

    jpi_list::item(1)::code = jpi$_username
    jpi_list::item(1)::buf_len = LEN (jpi_username$)
    jpi_list::item(1)::buf_address = LOC (jpi_username$)
    jpi_list::item(1)::len_address = jpi_username_len%
    jpi_list::item(2)::terminator = 0

    stat% = sys$getjpiw (,,, &
	    	 jpi_list BY REF, &
	    	 iosb BY REF,,)
    SELECT stat%
    	CASE = ss$_normal
	    stat% = INTEGER (iosb::iosb_field(1), LONG)
    	    SELECT stat%
	    	 CASE = ss$_normal
		     ! Continue.
	    	 CASE ELSE
		    stat% = chksts(stat%)
	    END SELECT
    	CASE ELSE
	    stat% = chksts(stat%)
    END SELECT
    ! Get the username for this process.

    OPEN "SYS$OUTPUT:" AS FILE #1

    %PAGE
    %SBTTL "Main Routine"

    PRINT #1, " Username      Node  Process name       PID    State  # Terminal   Image"
    ! Display column titles.

    CALL list_processes (x"00000000"l, x"00000000"l, jpi_username$)
    ! List all parent processes on all cluster nodes under this username.

END PROGRAM
%TITLE "LIST_PROCESSES"
%SBTTL "List all processes owned by specified PID"
%IDENT "V1.0"
SUB list_processes (LONG owner_pid%, LONG csid%, STRING user_name$)

!+
! FUNCTIONAL DESCRIPTION:
! 
!   List subprocesses owned by a specified PID.
! 
! FORMAL PARAMETERS:
! 
!   	 OWNER_PID%:
!            Owner PID to perform processes selection with.
!	     Zero = All processes with this username that have zero Owner
!	     PID.
!         
!        CSID%:
!            Cluster ID of node to restrict search to.
!	     Zero = Search all nodes in cluster.
!	  
!        USERNAME$:
!            Username to perform process selection with.
!    
! 
! IMPLICIT INPUT PARAMETERS:
! 
!   None
! 
! IMPLICIT OUTPUT PARAMETERS:
! 
!   None
! 
! SIDE EFFECTS:
! 
!   None
! 
! DESIGN:
! 
!    This routine is recursive so that the subprocesses of a currently
!    displayed process may be conveniently listed beneath that process in
!    the listing.
!
!    When called with OWNER_PID% = 0, the USERNAME$ parameter is used to
!    restrict the process search.  When called with OWNER_PID% <> 0, the
!    owner_pid restricts the processes searched and USERNAME$ is ignored.
!
!    When CSID% is zero, the match condition is PSCAN$M_NEQ, which causes
!    all nodes in the cluster to be searched for matching processes.  When
!    CSID% is non-zero, the match condition is PSCAN$M_EQL, which
!    restricts the search to the specified node.
!
!
!    PRECONDITIONS:
!
!    	 None
!	 
!    POSTCONDITIONS:
!
!    	 Anything in statically allocated memory (i.e., the MAP) is lost.
!	 
!    INVARIANTS:
!
!    	 The context variable for $PROCESS_SCAN - PIDCTX%
!	 
!    CLIENT OBLIGATIONS:
!     
!    	 None
!     
! [other_tags]...
!-
    OPTION TYPE = EXPLICIT, &
	   SIZE = INTEGER LONG, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR, &
	   ACTIVE = INTEGER OVERFLOW
    %PAGE
    %SBTTL "Declarations"

    !+
    ! Internal Constants
    !-
    DECLARE INTEGER CONSTANT false = 0, true = 1

    !+
    ! External Constants
    !-
    %INCLUDE "$fscndef" %FROM %LIBRARY
    %INCLUDE "$jpidef" %FROM %LIBRARY
    %INCLUDE "$pscandef" %FROM %LIBRARY
    %INCLUDE "$ssdef" %FROM %LIBRARY
    
    !+
    ! External Routines
    !-
    EXTERNAL LONG FUNCTION chksts (LONG)
    EXTERNAL LONG FUNCTION lib$analyze_sdesc
    EXTERNAL LONG FUNCTION lib$put_output
    EXTERNAL LONG FUNCTION lib$sys_fao
    EXTERNAL LONG FUNCTION sys$filescan
    EXTERNAL LONG FUNCTION sys$getjpiw
    EXTERNAL LONG FUNCTION sys$process_scan

    !+
    ! RECORD Declarations
    !-
    RECORD item_list_3
    	GROUP item (1 TO 16)
	    VARIANT
	    	 CASE
		     WORD	buf_len
		     WORD	code
		     LONG	buf_address
		     LONG	len_address
		 CASE
		     LONG	terminator
	    END VARIANT
	END GROUP item
    END RECORD item_list_3

    RECORD item_list_2
        GROUP item (1 TO 16)
            VARIANT
                CASE
                    WORD	comp_len
        	    WORD	code
        	    LONG	comp_addr
                CASE
                    LONG	terminator
            END VARIANT
        END GROUP item
    END RECORD item_list_2

    RECORD io_status_block
    	WORD	iosb_field(1 TO 4)
    END RECORD io_status_block
    
    !+
    ! COMMON Declarations
    !-
    COMMON (alpha) LONG	jpi_flags, &
		   STRING 	state$(1 TO 14) = 5

    !+
    ! MAP Declarations
    !-
    MAP (local2) LONG	imagname_len%, &
			mode%, &
			nodename_len%, &
			node_csid%, &
			owner%, &
			pid%, &
			prccnt%, &
			prcnam_len%, &
			state%, &
			stat%, &
			terminal_len%, &
		item_list_3	pscan_list, &
				jpi_list, &
		item_list_2	fscn_list, &
		io_status_block iosb, &
		STRING	imagname$ = 256, &
			nodename$ = 6, &
			prcnam$ = 15, &
			terminal$ = 8, &
			username$ = 12
    
    !+
    ! Local Variables
    !-
    DECLARE LONG	pidctx%, &
	    STRING	output_line$, &
			prccnt$, &
			image_name$
    %PAGE
    %SBTTL "Initialization"
    !+
    ! Subroutine Initialization
    !-
    pidctx% = 0

    fscn_list::item(1)::code = fscn$_name
    fscn_list::item(2)::terminator = 0

    pscan_list::item(1)::code = pscan$_getjpi_buffer_size
    pscan_list::item(1)::buf_len = 0
    pscan_list::item(1)::buf_address = 200*15
    pscan_list::item(1)::len_address = 0
    pscan_list::item(2)::code = pscan$_owner			! Owner PID
    pscan_list::item(2)::buf_len = 0
    pscan_list::item(2)::buf_address = owner_pid%
    pscan_list::item(2)::len_address = pscan$m_eql

    pscan_list::item(3)::code = pscan$_node_csid		! Cluster ID
    pscan_list::item(3)::buf_len = 0
    pscan_list::item(3)::buf_address = csid%
    IF csid% = 0
    THEN
	pscan_list::item(3)::len_address = pscan$m_neq
    ELSE
	pscan_list::item(3)::len_address = pscan$m_eql
    END IF
    ! If the Cluster ID is zero, then search all nodes in the cluster.
    ! If non-zero, then search on that node only.

    IF owner_pid% = x"00000000"l
    THEN
	pscan_list::item(4)::code = pscan$_username		! User name
	CALL lib$analyze_sdesc ( &
	     	  user_name$, &
	     	  pscan_list::item(4)::buf_len, &
	     	  pscan_list::item(4)::buf_address )
	pscan_list::item(4)::len_address = pscan$m_eql
	pscan_list::item(5)::terminator = 0
    ELSE
	pscan_list::item(4)::terminator = 0
    END IF
    ! If searching for processes with owner PID = 0, then restrict
    ! search to a given username.  Otherwise, the search is for
    ! subprocesses of a particular owner with that username and we
    ! don't need to (redundantly) specify PSCAN$_USERNAME.

    jpi_list::item(1)::code = jpi$_getjpi_control_flags
    jpi_list::item(1)::buf_len = 4
    jpi_list::item(1)::buf_address = LOC (jpi_flags)
    jpi_list::item(1)::len_address = 0
    jpi_list::item(2)::code = jpi$_imagname
    jpi_list::item(2)::buf_len = LEN (imagname$)
    jpi_list::item(2)::buf_address = LOC (imagname$)
    jpi_list::item(2)::len_address = LOC (imagname_len%)
    jpi_list::item(3)::code = jpi$_mode
    jpi_list::item(3)::buf_len = 4
    jpi_list::item(3)::buf_address = LOC (mode%)
    jpi_list::item(3)::len_address = 0
    jpi_list::item(4)::code = jpi$_nodename
    jpi_list::item(4)::buf_len = LEN (nodename$)
    jpi_list::item(4)::buf_address = LOC (nodename$)
    jpi_list::item(4)::len_address = LOC (nodename_len%)
    jpi_list::item(5)::code = jpi$_node_csid
    jpi_list::item(5)::buf_len = 4
    jpi_list::item(5)::buf_address = LOC (node_csid%)
    jpi_list::item(5)::len_address = 0
    jpi_list::item(6)::code = jpi$_owner
    jpi_list::item(6)::buf_len = 4
    jpi_list::item(6)::buf_address = LOC (owner%)
    jpi_list::item(6)::len_address = 0
    jpi_list::item(7)::code = jpi$_pid
    jpi_list::item(7)::buf_len = 4
    jpi_list::item(7)::buf_address = LOC (pid%)
    jpi_list::item(7)::len_address = 0
    jpi_list::item(8)::code = jpi$_prccnt
    jpi_list::item(8)::buf_len = 4
    jpi_list::item(8)::buf_address = LOC (prccnt%)
    jpi_list::item(8)::len_address = 0
    jpi_list::item(9)::code = jpi$_prcnam
    jpi_list::item(9)::buf_len = LEN (prcnam$)
    jpi_list::item(9)::buf_address = LOC (prcnam$)
    jpi_list::item(9)::len_address = LOC (prcnam_len%)
    jpi_list::item(10)::code = jpi$_state
    jpi_list::item(10)::buf_len = 4
    jpi_list::item(10)::buf_address = LOC (state%)
    jpi_list::item(10)::len_address = 0
    jpi_list::item(11)::code = jpi$_terminal
    jpi_list::item(11)::buf_len = LEN (terminal$)
    jpi_list::item(11)::buf_address = LOC (terminal$)
    jpi_list::item(11)::len_address = LOC (terminal_len%)
    jpi_list::item(12)::code = jpi$_username
    jpi_list::item(12)::buf_len = LEN (username$)
    jpi_list::item(12)::buf_address = LOC (username$)
    jpi_list::item(12)::len_address = 0
    jpi_list::item(13)::terminator = 0

    %PAGE
    %SBTTL "Process Loop"
    !+
    !  Process Loop
    !-
    stat% = chksts (sys$process_scan ( &
			pidctx% BY REF, &
			pscan_list BY REF))
Process_loop:
    WHILE true
	stat% = sys$getjpiw (, &
		    pidctx% BY REF,, &
		    jpi_list BY REF, &
		    iosb BY REF,,)
	SELECT stat%			! Check return status
	    CASE = ss$_normal
		stat% = INTEGER (iosb::iosb_field(1), LONG)
		SELECT stat%		! Check IOSB status value
		    CASE = ss$_normal
			! Continue.
		    CASE = ss$_nonexpr, = ss$_nopriv, = ss$_nosuchnode, &
			 = ss$_remrsrc, = ss$_suspended, = ss$_unreachable
			ITERATE process_loop
		    CASE = ss$_nomoreproc
			EXIT process_loop
		    CASE ELSE
		    	  stat% = chksts(stat%)
		END SELECT
	    CASE = ss$_nomoreproc
		EXIT process_loop
	    CASE = ss$_nonexpr, = ss$_nopriv, = ss$_nosuchnode, &
		 = ss$_remrsrc, = ss$_suspended, = ss$_unreachable
		ITERATE process_loop
	    CASE ELSE
		stat% = chksts(stat%)
	END SELECT
	! Get a process

	IF prccnt% = 0
	THEN
	     prccnt$ = ""
	ELSE
	    stat% = chksts (lib$sys_fao ( &
				"!2UL",, &
				prccnt$, &
				prccnt% BY VALUE))
	END IF
	! Zero-suppress the process_count.

	IF imagname_len% > 0%
	THEN
		stat% = chksts (sys$filescan ( &
				    imagname$, &
				    fscn_list, ))
		image_name$ = MID$ (imagname$, &
		    (fscn_list::item(1)::comp_addr - LOC (imagname$))+ 1, &
			    fscn_list::item(1)::comp_len)
	ELSE
		image_name$ = " "
	END IF
	! Extract the file name from the image specification.

	IF owner% = x"00000000"l
	THEN
	    SELECT mode%
	    	 CASE = jpi$k_other
	    	     terminal$ = "Other"
		     terminal_len% = 5
	    	 CASE jpi$k_network
		     terminal$ = "Network"
		     terminal_len% = 7
		 CASE jpi$k_batch
		     terminal$ = "Batch"
		     terminal_len% = 5
		 CASE jpi$k_interactive
	    	 CASE ELSE
	    END SELECT
	    ! Show process mode if no terminal name to show.
	    stat% = chksts (lib$sys_fao( &
			        " !12AS !6AS !16AS !8XL !5AS !2AS !10AS !AS",, &
			        output_line$, &
			        username$, &
			        LEFT$ (nodename$, nodename_len%), &
			        LEFT$ (prcnam$, prcnam_len%), &
			        pid% BY VALUE, &
			        state$(state%), &
			        prccnt$, &
				LEFT$ (terminal$, terminal_len%), &
			        image_name$))
	    ! Display for top-level processes.
	ELSE
	    stat% = chksts (lib$sys_fao( &
			        " !12AS !6AS !16AS !8XL !5AS !2AS (!8XL) !AS",, &
			        output_line$, &
			        " ", &
			        LEFT$ (nodename$, nodename_len%), &
			        LEFT$ (prcnam$, prcnam_len%), &
			        pid% BY VALUE, &
			        state$(state%), &
			        prccnt$, &
			        owner% BY VALUE, &
			        image_name$))
	    ! Display for subprocesses.
	END IF
	PRINT #1, output_line$
	! Display process information.

	IF prccnt% > 0
	THEN
	    CALL list_processes (pid%, node_csid%, username$)
	END IF
	! Display subprocesses of this process, if any.
    NEXT

END SUB	! End of SUB list_processes
FUNCTION LONG chksts (LONG stat%)

    OPTION TYPE = EXPLICIT, &
	   SIZE = INTEGER LONG, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = SEVERE

    EXTERNAL SUB lib$signal

    IF (stat% AND x"00000001"l) = 0
    THEN
	CALL lib$signal (stat% BY VALUE)
    END IF

END FUNCTION stat%  ! End of FUNCTION chksts
