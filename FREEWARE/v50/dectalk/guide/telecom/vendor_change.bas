!-------------------------- vendor_change.bas ------------------------------
!
!	This function allows the user to select his/her 
!	preferred long distance vendor.
!	The menu choices contain:
!
!		1) MCI
!		2) SPRINT
!		3) AT&T
!
FUNCTION LONG vendor_change()

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

    vendor_menu$ = "To select ['ehm s'iy 'ay] as your long distance vendor, "&
		 + "Press 1.  To select Sprint as your long distance vendor, "&
		 + "Press 2.  To select ['ey t'iy axn t'iy] as your "&
		 + "long distance vendor, Press 3.   To repeat this "&
		 + "message, press 0.  Terminate your entry with the "&
		 + "pound-key.  To exit this menu, press the star-key.  "

    mci$	 = "You have selected ['ehm s'iy 'ay] as your long distance "&
		 + "vendor.  "
    sprint$	 = "You have selected Sprint as your long distance vendor.  "
    att$	 = "You have selected ['ey t'iy axn t'iy] as your "&
		 + "long distance vendor.  "

    DECLARE	LONG	terminator_value, illegal_entry_count

    null_char$ = X'00'C
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
GET_SHORT_READ:
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_value)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
	    vendor_change = 0%
	    EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
	    vendor_change = 1%
	    EXIT FUNCTION
	ELSE IF (terminator_value = DTK$K_TRM_NUMBER_SIGN)
	    THEN
		GOSUB process_menu_entry	! Valid command so process it	
		IF (input_key_buf(0) = DTK$K_TRM_ZERO)
		THEN
		    GO TO get_long_read
		ELSE
		    GO TO get_short_read
		END IF
	    ELSE IF ((terminator_value <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_value = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
		!
		!  No keys in typeahead buffer so prompt for a command.
		!
	        THEN
		    CALL speak_text(DTK$K_WAIT, vendor_menu$)
		END IF
	    END IF
	END IF
	!
	!  Any new keys received should be processed after the keys read
	!  from the typeahead buffer.  Start storing touch tone keys into
	!  into the buffer following the keys read from the typeahead buffer.
	!
 GET_LONG_READ:
	num_input_keys = 0%
        stat% = get_key_string(null_char$, T20SECOND, terminator_value)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
	    vendor_change = 0%
	    EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
	    vendor_change = 1%
	    EXIT FUNCTION
	ELSE IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
					input_key_buf(0) <> 0%)
	    THEN
		CALL speak_all_text(msg_no_terminator)
		GOSUB process_menu_entry	! Valid command so process it	
	    ELSE IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
						input_key_buf(0) = 0%)
		THEN
		    CALL speak_all_text(msg_timeout)
		    vendor_change = 0%
		    EXIT FUNCTION
		ELSE
		    GOSUB  process_menu_entry
		END IF
	    END IF
	END IF
   NEXT
   vendor_change = 0%
   EXIT FUNCTION
PROCESS_MENU_ENTRY:
    !
    !  Processes the touch tone key string received from the user.
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
	    CASE = DTK$K_TRM_ZERO		    ! Speak help message
		CALL speak_text(DTK$K_WAIT, vendor_menu$)
		illegal_entry_count = 0%

	    CASE = DTK$K_TRM_ONE		    ! MCI selected
		illegal_entry_count = 0%
		CALL speak_text(DTK$K_WAIT, mci$)
		vendor_change = 1%
		EXIT FUNCTION

	    CASE = DTK$K_TRM_TWO		    ! Sprint selected
		illegal_entry_count = 0%
		CALL speak_text(DTK$K_WAIT, sprint$)
		vendor_change = 1%
		EXIT FUNCTION

	    CASE = DTK$K_TRM_THREE		    ! ATT selected
		illegal_entry_count = 0%
		CALL speak_text(DTK$K_WAIT, att$)
		vendor_change = 1%
		EXIT FUNCTION

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
	CALL speak_text(DTK$K_WAIT, vendor_menu$)
	illegal_entry_count = 0%
    END IF
    RETURN
END FUNCTION

