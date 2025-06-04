!-------------------------- custom_service.bas ------------------------------
!
!	This function allows the user to select custom calling services
!	The menu choices contain:
!
!		1) Call forwarding
!		2) Speed calling
!		3) Call waiting
!		4) Three-way calling
!
FUNCTION LONG custom_service()

%INCLUDE "DTKDEF"

    EXTERNAL LONG CONSTANT	  STS$M_SUCCESS
    EXTERNAL LONG FUNCTION	  get_key_string

    COMMON (MESSAGE_TEXT)	  STRING msg_invalid  = 40, &
					 msg_bad_command   = 40, &
				         msg_no_terminator  = 100, &
					 msg_timeout  = 50

    COMMON (constants)		  STRING time_45_sec, time_6_min, time_16_min, &
				  LONG	 MAX_ENTRY, T1SECOND, T20SECOND

    COMMON (dt_features)          LONG   voice, speaking_rate, &
					 comma_pause_duration, &
					 period_pause_duration

    COMMON (application_specific) LONG	 voice_id, &
					 num_hold_keys, &
					 num_input_keys, &
				  BYTE	 hold_key_buf(80), &
					 input_key_buf(80)

    custom_menu$ = "To order call forwarding, press 1.  To order "&
		 + "speed-calling, press 2.  To order call waiting, "&
		 + "press 3.  To order three-way calling, press 4.  "&
		 + "To repeat this message, press 0.  Terminate your "&
		 + "entry with the pound-key.  To exit this menu, "&
		 + "press the star-key.  "

    custom_hi$   = "Welcome to the Custom-calling services order system.  "

    forward$	 = "You have ordered call forwarding.  "
    waiting$	 = "You have ordered call waiting.  "
    call_3$	 = "You have ordered three-way calling.  "
    speed$	 = "You have ordered speed-calling.  "
    charge$	 = "Charges for the services ordered, will appear on your "&
		 + "monthly phone-bill.  "

    more_prmt$	 = "To order another custom-calling service, press 1,  "&
		 + "followed by the pound-key.  To exit the custom-calling "&
		 + "services menu, press the star-key.  "

    DECLARE	LONG	terminator_value, illegal_entry_count, order_no

    null_char$ = X'00'C
    order_no = 0%
order_it:
    illegal_entry_count = 0%
    terminator_value = 0%
    WHILE (input_key_buf(0) <> 0% OR terminator_value <> DTK$K_TRM_TIMEOUT)
	!
	! Keep getting commands from the user until the exit key "*",
	! is entered, a wink is detected, or the timeout period expires.
	! First, check if there are any keys in the typeahead buffer (done by 
	! "get_key_string" with a 1 second timeout).  If so, process them. 
	! Otherwise, prompt the user for a command and wait for his response.
	!
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_value)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
	    custom_service = 0%
	    EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
	    IF order_no = 1%
	    THEN
	        CALL speak_text(DTK$K_WAIT, charge$)
	    END IF
	    custom_service = 1%
	    EXIT FUNCTION
	ELSE IF ((terminator_value <> DTK$K_TRM_TIMEOUT) AND &
		 (terminator_value <> DTK$K_TRM_NUMBER_SIGN)) 
	    THEN
		custom_service = 0%
		EXIT FUNCTION
	    END IF
	END IF
	IF (terminator_value = DTK$K_TRM_TIMEOUT AND input_key_buf(0) = 0%)
	THEN
	    !
	    !  Any new keys received should be processed after the keys read 
	    !  from the typeahead buffer.  Start storing touch tone keys into
	    !  the buffer following the keys read from the typeahead buffer.
	    !
	    CALL speak_text(DTK$K_WAIT, custom_menu$)
	    stat% = get_key_string(null_char$, T20SECOND, terminator_value)
	    IF (stat% AND STS$M_SUCCESS) = 0%
	    THEN
	        custom_service = 0%
	        EXIT FUNCTION
	    END IF
	    IF terminator_value = DTK$K_TRM_ASTERISK
	    THEN
		IF order_no = 1%
		THEN
		    CALL speak_text(DTK$K_WAIT, charge$)
		END IF
	        custom_service = 1%
	        EXIT FUNCTION
	    END IF
	    IF (terminator_value = DTK$K_TRM_TIMEOUT AND input_key_buf(0) = 0%)
	    THEN
		CALL speak_all_text(msg_timeout)
		custom_service = 0%
		EXIT FUNCTION
	    ELSE IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
		    CALL speak_all_text(msg_no_terminator)
		END IF
	    END IF
	END IF
	IF ((terminator_value <> DTK$K_TRM_TIMEOUT) AND &
		 (terminator_value <> DTK$K_TRM_NUMBER_SIGN)) 
	THEN
	    custom_service = 0%
	    EXIT FUNCTION
	END IF
	!
	!  Valid touch tone key sequence entered so process it.
	!
	IF num_input_keys > 1%
	THEN
	    !
	    !  Only single key commands are valid in this menu.  Inform
	    !  the user that an invalid command was entered and return.
	    !
	    CALL speak_all_text(msg_bad_command)
	    illegal_entry_count = illegal_entry_count + 1%
	ELSE
	    SELECT input_key_buf(0)
		CASE =  DTK$K_TRM_ZERO		! Speak menu help msg
		    CALL speak_text(DTK$K_WAIT, custom_menu$)
		    illegal_entry_count = 0%

	        CASE = DTK$K_TRM_ONE		    ! Call forwarding ordered
		    illegal_entry_count = 0%
		    CALL speak_text(DTK$K_WAIT, forward$)
		    order_no = 1%
		    GO TO new_cmd

	        CASE = DTK$K_TRM_TWO		    ! Speed calling ordered
		    illegal_entry_count = 0%
		    CALL speak_text(DTK$K_WAIT, speed$)
		    order_no = 1%
		    GO TO new_cmd

	        CASE = DTK$K_TRM_THREE		    ! Call waiting ordered
		    illegal_entry_count = 0%
		    CALL speak_text(DTK$K_WAIT, waiting$)
		    order_no = 1%
		    GO TO new_cmd

	        CASE = DTK$K_TRM_FOUR		    ! Three way calling ordered
		    illegal_entry_count = 0%
		    CALL speak_text(DTK$K_WAIT, call_3$)
		    order_no = 1%
		    GO TO new_cmd

	        CASE ELSE
		    CALL speak_text(DTK$K_WAIT, msg_invalid)
		    illegal_entry_count = illegal_entry_count + 1%
	    END SELECT
	END IF
	num_input_keys = 0%
	!
	! If user enters 3 illegal/incorrect commands, speak help message
	!
	IF illegal_entry_count >= 3
	THEN
	    CALL speak_text(DTK$K_WAIT, custom_menu$)
	    illegal_entry_count = 0%
	END IF
    NEXT
    custom_service = 0%
    EXIT FUNCTION
new_cmd:
    WHILE (input_key_buf(0) <> 0% OR  &
				terminator_value <> DTK$K_TRM_TIMEOUT)
	!
	! Keep getting commands from the user until the exit key "*",
	! is entered, a wink is detected, or the timeout period expires.
	! First, check if there are any keys in the typeahead buffer (done by 
	! "get_key_string" with a 1 second timeout).  If so, process them. 
	! Otherwise, prompt the user for a command and wait for his response.
	!
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_value)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
	    custom_service = 0%
	    EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
	    IF order_no = 1%
	    THEN
	        CALL speak_text(DTK$K_WAIT, charge$)
	    END IF
	    custom_service = 1%
	    EXIT FUNCTION
	ELSE IF ((terminator_value <> DTK$K_TRM_TIMEOUT) AND &
		 (terminator_value <> DTK$K_TRM_NUMBER_SIGN)) 
	    THEN
		custom_service = 0%
		EXIT FUNCTION
	    END IF
	END IF
	IF (terminator_value = DTK$K_TRM_TIMEOUT AND input_key_buf(0) = 0%)
	THEN
	    !
	    !  Any new keys received should be processed after the keys read 
	    !  from the typeahead buffer.  Start storing touch tone keys into
	    !  the buffer following the keys read from the typeahead buffer.
	    !
	    CALL speak_text(DTK$K_WAIT, more_prmt$)
	    stat% = get_key_string(null_char$, T20SECOND, terminator_value)
	    IF (stat% AND STS$M_SUCCESS) = 0%
	    THEN
	        custom_service = 0%
	        EXIT FUNCTION
	    END IF
	    IF terminator_value = DTK$K_TRM_ASTERISK
	    THEN
		IF order_no = 1%
		THEN
		    CALL speak_text(DTK$K_WAIT, charge$)
		END IF
	        custom_service = 1%
	        EXIT FUNCTION
	    END IF
	    IF (terminator_value = DTK$K_TRM_TIMEOUT AND input_key_buf(0) = 0%)
	    THEN
		CALL speak_all_text(msg_timeout)
		custom_service = 0%
		EXIT FUNCTION
	    ELSE IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
		    CALL speak_all_text(msg_no_terminator)
		END IF
	    END IF
	END IF
	IF ((terminator_value <> DTK$K_TRM_TIMEOUT) AND &
		 (terminator_value <> DTK$K_TRM_NUMBER_SIGN)) 
	THEN
	    custom_service = 0%
	    EXIT FUNCTION
	END IF
	IF num_input_keys > 1%
	THEN
	    !
	    !  Only single key commands are valid in this menu.  Inform
	    !  the user that an invalid command was entered and return.
	    !
	    CALL speak_all_text(msg_bad_command)
	    illegal_entry_count = illegal_entry_count + 1%
	ELSE
	    SELECT input_key_buf(0)
		CASE =  DTK$K_TRM_ZERO		! Speak menu help msg
		    CALL speak_text(DTK$K_WAIT, more_prmt$)
		    illegal_entry_count = 0%

		CASE = DTK$K_TRM_ONE
		    illegal_entry_count = 0%
		    GO TO order_it

		CASE ELSE
		    CALL speak_text(DTK$K_WAIT, msg_invalid)
		    illegal_entry_count = illegal_entry_count + 1%
	    END SELECT
	END IF
	num_input_keys = 0%
	!
	! If user enters 3 illegal/incorrect commands, speak help message
	!
	IF illegal_entry_count >= 3
	THEN
	    CALL speak_text(DTK$K_WAIT, more_prmt$)
	    illegal_entry_count = 0%
	END IF
    NEXT
    custom_service = 0%
    EXIT FUNCTION
END FUNCTION
