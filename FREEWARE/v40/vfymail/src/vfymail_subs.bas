%TITLE "MAILFILE_FOLDER_ROUTINE"
%SBTTL "Folder action routine for BUILD_INTERNAL_NAMELIST"
%IDENT "V05.01"

FUNCTION LONG mailfile_folder_routine ( &
		       LONG user_data, &
		       STRING folder_name, &
		       LONG dummy_1, &
		       LONG dummy_2)

![function-header]
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    %INCLUDE "$maildef" %FROM %LIBRARY
    %INCLUDE "$mailmsgdef"

    EXTERNAL LONG CONSTANT lib$_normal, lib$_keynotfou, ss$_normal

    %INCLUDE "mail$routines"
    %INCLUDE "str$routines" %FROM %LIBRARY
    EXTERNAL LONG FUNCTION allocate_node, &
			   chksts, &
			   compare_node, &
			   compare_node1

    EXTERNAL LONG FUNCTION lib$insert_tree ( &
			       LONG BY REF, &
			       ANY, &
			       LONG BY REF, &
			       ANY BY VALUE, &
			       ANY BY VALUE, &
			       LONG BY REF, &
			       ANY), &
			   lib$lookup_tree ( &
			       LONG BY REF, &
			       ANY, &
			       ANY BY VALUE, &
			       LONG BY REF), &
			   lib$traverse_tree ( &
			       LONG BY REF, &
			       ANY BY VALUE, &
			       ANY)

    MAP (folder_local) INTEGER	cnt, &
		       WORD	message_flags, &
		       LONG	stat, &
				new_node_loc, &
				message_current_id, &
				select_count, &
		       STRING	external_id = 21, &
				null_string = 0

    %INCLUDE "vfycommon"
    
    EXIT FUNCTION ss$_normal  IF folder_name = lost_folder_id OR &
				 folder_name = ""
    ! Skip processing if this is the lost message folder or if this
    ! is the last invocation of the folder routine with null parameter.

    WHEN ERROR IN

	input_list(1)::itmcod = mail$_message_file_ctx
	input_list(1)::buflen = 4
	input_list(1)::bufadr = LOC (mailfile_context)
	input_list(1)::lenadr = 0
	input_list(2)::terminator = 0

	stat = mail$message_begin ( &
		    message_context, &
		    input_list() BY REF, &
		    null_list)
	! Initialize processing of messages.

	input_list(1)::itmcod = mail$_message_folder
	CALL str$analyze_sdesc ( &
	         folder_name, &
	         input_list(1)::buflen, &
	         input_list(1)::bufadr)
	input_list(1)::lenadr = 0
	input_list(2)::terminator = 0

	output_list(1)::itmcod = mail$_message_selected
	output_list(1)::buflen = 4
	output_list(1)::bufadr = LOC (select_count)
	output_list(1)::lenadr = 0
	output_list(2)::terminator = 0

	stat = mail$message_select ( &
		    message_context, &
		    input_list() BY REF, &
		    output_list() BY REF)
	! Select desired folder.
	
Message_loop:

	FOR cnt = 1 TO select_count

	    input_list(1)::itmcod = mail$_message_next
	    input_list(1)::buflen = 0
	    input_list(1)::bufadr = 0
	    input_list(1)::lenadr = 0
	    input_list(2)::terminator = 0

	    output_list(1)::itmcod = mail$_message_extid
	    CALL str$analyze_sdesc ( &
		     external_id, &
		     output_list(1)::buflen, &
		     output_list(1)::bufadr)
	    output_list(1)::lenadr = 0
	    output_list(2)::itmcod = mail$_message_return_flags
	    output_list(2)::buflen = 2
	    output_list(2)::bufadr = LOC (message_flags)
	    output_list(2)::lenadr = 0
	    output_list(3)::itmcod = mail$_message_current_id
	    output_list(3)::buflen = 4
	    output_list(3)::bufadr = LOC (message_current_id)
	    output_list(3)::lenadr = 0
	    output_list(4)::terminator = 0

	    stat = mail$message_info ( &
			message_context, &
			input_list() BY REF, &
			output_list() BY REF)
	    ! Look-up the next message in this folder and return
	    !  external-id, and message flags.

	    ITERATE message_loop  IF (message_flags AND mail$m_extmsg) = 0
	    ! Skip this message if no external message file.

	    stat = chksts (lib$insert_tree ( &
			       internal_name_tree, &
			       external_id BY DESC, &
			       1, &
			       LOC (compare_node), &
			       LOC (allocate_node), &
			       new_node_loc,))
	    ! Insert external-id into list for use in the missing
	    !  message check.

	    stat = lib$lookup_tree ( &
		       external_file_tree, &
		       external_id, &
		       LOC (compare_node1), &
		       new_node_loc)
	    SELECT stat
		 
		CASE = lib$_normal

		CASE = lib$_keynotfou
		    input_list(1)::itmcod = mail$_message_id
		    input_list(1)::buflen = 4
		    input_list(1)::bufadr = LOC (message_current_id)
		    input_list(1)::lenadr = 0
		    input_list(2)::terminator = 0

		    output_list(1)::itmcod = mail$_message_date
		    output_list(1)::buflen = LEN (message_date)
		    output_list(1)::bufadr = LOC (message_date)
		    output_list(1)::lenadr = 0
		    output_list(2)::itmcod = mail$_message_sender
		    output_list(2)::buflen = LEN (message_sender)
		    output_list(2)::bufadr = LOC (message_sender)
		    output_list(2)::lenadr = LOC (message_sender_len)
		    output_list(3)::itmcod = mail$_message_subject
		    output_list(3)::buflen = LEN (message_subject)
		    output_list(3)::bufadr = LOC (message_subject)
		    output_list(3)::lenadr = LOC (message_subject_len)
		    output_list(4)::terminator = 0

		    stat = mail$message_info ( &
				message_context, &
				input_list() BY REF, &
				output_list() BY REF)
		    ! If the external file isn't found, then lookup
		    !  the the rest of the information needed to
		    !  identify the message to the user.

		    PRINT #5, "%VFYMAIL-I-FILNOTFOU, file "; &
				external_id; ".MAI not found"
		    PRINT #5, TAB (9); &
			    "folder "; folder_name; &
			    " message number "; message_current_id
		    PRINT #5, TAB (9); "from: "; &
			    LEFT$ (message_sender, message_sender_len); " "; &
			    message_date
		    PRINT #5, TAB (9); "subj: "; LEFT$ (message_subject, &
							  message_subject_len)
		    external_error_count = external_error_count + 1
		    
		    IF (option_flags AND vfy_m_repair) <> 0
		    THEN
			input_list(1)::itmcod = mail$_nosignal
			input_list(1)::buflen = 0
			input_list(1)::bufadr = 0
			input_list(1)::lenadr = 0
			input_list(2)::itmcod = mail$_message_id
			input_list(2)::buflen = 4
			input_list(2)::bufadr = LOC (message_current_id)
			input_list(2)::lenadr = 0
			input_list(3)::itmcod = mail$_message_folder
			input_list(3)::buflen = LEN (lost_folder_id)
			input_list(3)::bufadr = LOC (lost_folder_name)
			input_list(3)::lenadr = 0
			input_list(4)::itmcod = mail$_message_delete
			input_list(4)::buflen = 0
			input_list(4)::bufadr = 0
			input_list(4)::lenadr = 0
			input_list(5)::itmcod = mail$_message_default_name
			CALL str$analyze_sdesc ( &
				 null_string, &
				 input_list(5)::buflen, &
				 input_list(5)::bufadr)
			input_list(5)::lenadr = 0
			input_list(6)::terminator = 0

			stat = mail$message_copy ( &
				    message_context, &
				    input_list() BY REF, &
				    null_list)
			SELECT stat

			    CASE = ss$_normal, = mail$_openin
			    
			    CASE ELSE
				stat = chksts (stat)

			END SELECT
		    END IF
		 
		CASE ELSE
		    PRINT "Bad Lookup_tree status on external file lookup"
		    stat = chksts (stat)
	    END SELECT
	    ! Perform a lookup on this external-id in the external file list.
	    ! If it's not found, then issue an informational and identify the
	    !  message as best we can.
	    ! Additionally, if /REPAIR is in effect, then move the mail
	    !  message to the lost-message folder.
	    ! Note that the null MESSAGE_DEFAULT_NAME is provided to work
	    !  around some behavior in the routine MAIL$MESSAGE_COPY, which
	    !  causes the output mailfile to default to SYS$LOGIN:MAIL.MAI,
	    ! if the item is unspecified/allowed to default.

	    internal_count = internal_count + 1
	    ! Increment the count of messages with external-id's.

	NEXT cnt
	! End of message_loop.

	stat = mail$message_end ( &
		    message_context, &
		    null_list, &
		    null_list)
	! Finish processing on this folder.

    USE
	EXIT HANDLER
    END WHEN

END FUNCTION ss$_normal  ! End of FUNCTION mailfile_folder_routine

%TITLE "SET_DELPROT"
%SBTTL "Set Delete File Protection"
%IDENT "V05.00"

FUNCTION LONG set_delprot (STRING file_spec BY DESC)

![function-header]

    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    %PAGE
    %SBTTL "Declarations"

    %INCLUDE "$rmsdef" %FROM %LIBRARY
    %INCLUDE "$fabdef" %FROM %LIBRARY
    %INCLUDE "$xabdef" %FROM %LIBRARY

    %PAGE
    %SBTTL "Declarations"

    EXTERNAL LONG CONSTANT ss$_normal

    DECLARE LONG CONSTANT xab_v_sys = 0, &
			  xab_v_own = 4, &
			  xab_v_grp = 8, &
			  xab_v_wld = 12
    
    EXTERNAL LONG FUNCTION chksts ( &
			       LONG)
    EXTERNAL LONG FUNCTION lib$bbcci ( &
			       ANY BY REF, &
			       LONG BY REF)
    EXTERNAL SUB str$analyze_sdesc ( &
		     STRING, &
		     WORD, &
		     LONG)
    EXTERNAL LONG FUNCTION sys$open ( &
			       ANY BY REF, &
			       OPTIONAL &
			       LONG, &
			       LONG)
    EXTERNAL LONG FUNCTION sys$close ( &
			       ANY BY REF, &
			       OPTIONAL &
			       LONG, &
			       LONG)

    DECLARE WORD	fns, &
	    LONG	stat, &
			stat1, &
	    fabdef	fab, &
	    xabdef	xab

    %PAGE
    %SBTTL "Main routine"

    WHEN ERROR IN
	fab::fab$b_bid = fab$c_bid
	fab::fab$b_bln = fab$c_bln
	fab::fab$b_fac = fab$m_upd
	CALL str$analyze_sdesc ( &
		 file_spec, &
		 fns, &
		 fab::fab$l_fna)
	fab::fab$b_fns = INTEGER (fns, BYTE)
	fab::fab$l_xab = LOC (xab)

	xab::xab$b_cod = xab$c_pro
	xab::xab$b_bln = xab$c_prolen
	! Initialize and set up the FAB and XAB blocks

	stat = chksts (sys$open ( &
			   fab,,))

	xab::xab$w_pro = xab::xab$w_pro AND NOT ((2^xab_v_own)*xab$m_nodel)

	stat = chksts (sys$close ( &
			   fab,,))
	! Open the file to set the XAB fields,
	! Clear the XAB nodelete bit,
	! Close the file to write the new protection.

	EXIT FUNCTION ss$_normal

Exit_error:
	EXIT FUNCTION stat1
    USE
	stat1 = VMSSTATUS
	CONTINUE exit_error
    END WHEN

END FUNCTION ss$_normal	 ! End of FUNCTION set_delprot
