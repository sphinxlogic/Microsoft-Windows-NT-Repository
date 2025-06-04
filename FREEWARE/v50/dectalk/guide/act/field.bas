 !  --------------------------  FIELD.BAS  ---------------------------
 !
 !  This subroutine contains the field service section of the ACT demo program.
 !  The menu choices included are:
 !
 !	1) PHONE MESSAGES
 !	2) SERVICE CALLS
 !	9) TRANSFER TO DISPATCH DESK
 !	
 FUNCTION LONG field_service()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				message, &
				DTK$SET_VOICE

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160, &
				dispatch_desk = 160

     field_welcome$ = "[+] Welcome to the DECtalk field service demonstration. "

     service_menu$ = "To hear your [f'own#m`ehsixjhixz], press 1. "&
	      + "To hear your [s'rrvixs#k`aolz], press 2. "&
	      + "To speak to the dispatch-desk, press 9. "&
	      + "To exit the field-service demonstration, press the star-key. "&
	      + "Terminate your entry with the pound-key. "&
	      + "To repeat this message, press 0. "

     phone$ = "You have 2 [f'own#m`ehsixjhixz]. "
     service$ = "You have 1 [s'rrvixs#k`aol]. "

     badge_no$ = "Enter your badge number followed by the pound-key. You may "&
	+ "enter any number as a test. "

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  DECLARE	LONG	terminator_value, illegal_entry_count

 	null_char$ = X'00'C
	illegal_entry_count = 0%

	CALL speak_text(DTK$K_WAIT, field_welcome$)

	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(badge_no$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		field_service = 0%
		EXIT FUNCTION
	END IF

	input_key_buf(0) = 0%
	num_input_keys = 0%
        stat% = get_key_string(msg_access, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		field_service = 0%
		EXIT FUNCTION
	END IF

	terminator_value = 0%
	WHILE (input_key_buf(0) <> 0% OR  &
				terminator_value <> DTK$K_TRM_TIMEOUT)
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
		field_service = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		field_service = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_value = DTK$K_TRM_NUMBER_SIGN)
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
				CALL speak_text(DTK$K_WAIT, service_menu$)
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
		field_service = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		field_service = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
			GOSUB process_menu_entry	! Valid command so process it	

		ELSE 	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				field_service = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   field_service = 0%
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
	    SELECT	input_key_buf(0)
		CASE = 	DTK$K_TRM_ZERO			! Speak menu help msg
			CALL speak_text(DTK$K_WAIT, service_menu$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE
			CALL speak_text(DTK$K_WAIT, phone$)
			stat% = message(2%)		! phone messages
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				field_service = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO
			CALL speak_text(DTK$K_WAIT, service$)
			stat% = message(1%)		! service calls
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				field_service = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_NINE
			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)
			CALL speak_text(DTK$K_WAIT, dispatch_desk)
			illegal_entry_count = 0%

		CASE ELSE
			CALL speak_text(DTK$K_WAIT, msg_invalid)
			illegal_entry_count = illegal_entry_count + 1%
	   END SELECT
	END IF
	num_input_keys = 0%
	!
	! If user enters 3 illegal/incorrect commands, speak help message
	!
	IF illegal_entry_count >= 3%
	THEN
		CALL speak_text(DTK$K_WAIT, service_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION

 ! -------------------------- message -----------------------------
 !
 !  This section of code will process and speak the service and phone
 !  messages.   The menu choices are:
 !
 !	1) HEAR MESSAGE REPEATED
 !	2) HEAR NEXT MESSAGE
 !	9) TRANSFER TO DISPATCH DESK
 !
 FUNCTION LONG message( LONG msg_type)

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				DTK$SET_VOICE

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160, &
				dispatch_desk = 160

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80), &
				STRING	phone_msg(2) = 200, &
					service_msg(1) = 300

	phone_msg(1) = "Message 1 is from your manager.  It reads, Training "&
	+ "for new hardware installation is scheduled for next Wednesday. "

	phone_msg(2) = "Message 2 is from the warehouse. The subject is Parts."&
	+ " It reads: the parts you ordered last [fr'aydxey] for the "&
	+ "air-conditioning system have arrived. "

	service_msg(1) = "[s'rrvixs#k`aol] 1. The customer is Carter Labs. "&
	+ "The system type is [axl'ehvixn thrrdxiy*f'or], Option is "&
	+ "['aar*p`iy*`ow*s'ihks]. Contact is Warren Foster. "&
	+ "His [f'own#n`ahmbrr] is 4 1 3 -- 5 5 5,   1 2 1 2.  "&
	+ "The problem is the [dr'ayv<150>] won't spin up. "&
	+ "Please call customer. "

	message_menu$ = "To hear the message repeated, press 1. "&
	+ "To hear the next message, press 2. "&
	+ "To speak to the dispatch-desk, press 9. "&
	+ "To exit, press the star-key. To repeat this message, press 0. "&
	+ "Terminate your entry with the pound-key.  "&
	+ "[:nh +] In a real system, the caller could file or delete messages. "


  DECLARE	LONG	terminator_value, illegal_entry_count
 	null_char$ = X'00'C
	illegal_entry_count = 0%
	message_num% = 1%

top:	IF msg_type = 2%
	THEN
		CALL speak_text(DTK$K_WAIT, phone_msg(message_num%))
	ELSE
		CALL speak_text(DTK$K_WAIT, service_msg(message_num%))
	END IF

	terminator_value = 0%
	WHILE (input_key_buf(0) <> 0% OR  &
				terminator_value <> DTK$K_TRM_TIMEOUT)
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
		message = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		message = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_value = DTK$K_TRM_NUMBER_SIGN)
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
				CALL speak_text(DTK$K_WAIT, message_menu$)
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
		message = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		message = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
			GOSUB process_menu_entry	! Valid command so process it	

		ELSE 	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				message = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   message = 0%
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
	    SELECT	input_key_buf(0)
		CASE = 	DTK$K_TRM_ZERO			! Speak menu help msg
			CALL speak_text(DTK$K_WAIT, message_menu$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE
			illegal_entry_count = 0%
			GO TO top

		CASE = 	DTK$K_TRM_TWO
			message_num% = message_num% + 1%
			IF message_num% = msg_type + 1
			THEN
				message_num% = 1%
			END IF
			illegal_entry_count = 0%
			GO TO top

		CASE = 	DTK$K_TRM_NINE
			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)

			CALL speak_text(DTK$K_WAIT, dispatch_desk)
			message = 1%
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
		CALL speak_text(DTK$K_WAIT, message_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION
