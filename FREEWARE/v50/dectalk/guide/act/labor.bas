 ! ------------------------- LABOR.BAS ----------------------
 ! This subroutine contains the time reporting section of the ACT demo.
 ! The menu choices are:
 !	1) LABOR ENTRY
 !	2) REVIEW DATA ENTERED
 !	3) CHARGE NUMBER VALIDATION
 !	4) LABOR REPORTS
 !
 FUNCTION LONG labor_reporting()

  %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS, &
				SS$_NORMAL,   &
				SS$_TIMEOUT
  EXTERNAL LONG FUNCTION	DTK$SET_VOICE, &
				get_key_string, &
				labor_entry
 !
 !  Prompts and demo text
 !
  COMMON (MESSAGE_TEXT) STRING  msg_access	= 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50
 !
 !  labor demo specific variables and initialization.
 !
  COMMON (labor)		STRING	last_name = 80, &
					badge_num = 80, &
					project_num(30), &
					hours(30)  = 80, &
				LONG	tot_proj_count

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG  MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND, T30SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80) , &
					input_key_buf(80)

  labor_welcome$ = "Welcome to the DECtalk time reporting demonstration. "

  top_level_menu$ = "For labor entry, Press 1. "&
		+ "For review of data entered, Press 2. "&
		+ "For charge number validation, Press 3. "&
		+ "For labor reports, Press 4. "&
		+ "To exit, Press the star-key. "&
		+ "To repeat this message, Press 0. Terminate your"&
		+ "entry with the pound-key. "

  badge_no$	= "Please enter your badge-number followed by the pound-key. "&
		+ "You may enter any number as a test. "

  illegal_term$ = "Illegal terminator. Please terminate your entry with the "&
		+ "pound-key. "

  project_prompt$ = "Please enter the project number followed by a pound-key. "&
		+ "You may enter any number as a test. "

  review_msg$	= "You have entered the following data.  Badge-number. "
  proj_num_msg$  = "Project number. "
  hours_msg$	= "hours charged. "
  no_hours_msg$	= "No hours charged. "

  no_data$	= "There has been no data entered yet. "

  charge_number$ = "If this had been a real application, this choice would "&
		+ "provide information on valid charge numbers. "

  labor_report$	= "If this had been a real application, a supervisor or"&
		+ " manager would be able to ['aeksehs] labor reports. "

  verify_charge$ = "In a real application DECtalk would report on the "&
		+ "validity of charge number "

  DECLARE LONG	illegal_entry_count,		! Number of illegal entries! &
		terminator_used			! Key string terminator code

	sp$ = " "
 	null_char$ = X'00'C
	proj_count% = 0%
	tot_proj_count = 0%

	CALL speak_text(DTK$K_WAIT, labor_welcome$)

	! The user is prompted for his badge number by the text specified in
	! "msg_badge_no". Currently, the application waits
	! 30 seconds for a touch tone key to be entered.  If a longer or
	! shorter timeout period is desired, the new timeout value (in seconds)
	! should be passed to "get_key_string". The parameter,
	! "terminator_used" will contain the character used to terminate the
	! key string or timeout upon return from the "get_key_string" routine.
	!

	illegal_entry_count = 0%			! No bad attempts yet

 check_badge_no:

    WHILE illegal_entry_count < MAX_ENTRY
	num_input_keys = 0%

	stat% = get_key_string(badge_no$,T30SECOND,terminator_used)

	IF (stat% AND STS$M_SUCCESS) <> 0%
	THEN
	    IF (terminator_used = DTK$K_TRM_ASTERISK)
	    THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		GO TO check_badge_no
	    ELSE
		IF (terminator_used=DTK$K_TRM_TIMEOUT)
		THEN
		!
		! If an invalid password code was entered, increment the count
		! of invalid entry attempts.  If this count is greater than the
		! maximum allowed (specified by MAX_ENTRY), exit sub.
		! Otherwise, notify the user and prompt him again. If a valid
		! password is entered, then continue.
		!
			illegal_entry_count = illegal_entry_count + 1%
			IF illegal_entry_count >= MAX_ENTRY
			THEN
				labor_reporting = 1%
				EXIT FUNCTION	! three strikes you're out
			ELSE
				CALL speak_text(DTK$K_WAIT, msg_invalid)
			END IF
		ELSE 
			EXIT check_badge_no		! PASSWORD IS VALID
		END IF
	ELSE
		labor_reporting = 0%
		EXIT FUNCTION			! No key string received
	END IF
    NEXT

    ! store badge number away in "badge_num"

	j% = 1%
	FOR i% = 0% TO (num_input_keys - 1%)
		MID$(badge_num, j%, 1) = CHR$(input_key_buf(i%))
		j% = j% + 1%
		MID$(badge_num, j%, 1) = sp$
		j% = j% + 1%
	NEXT i%

	MID$(badge_num, (j%-1%), 1) = "."

	FOR i% = j% TO 80%
		MID$(badge_num, i%, 1%) = " "
	NEXT i%

 !  Prompts the user for a command and receives the command (touch tone
 !  key) from the user.  Note, the command will be accepted whether or not
 !  it is terminated with the pound key "#".  However, if the command
 !  is not followed by the pound key, the command will not be processed
 !  until the timeout period has expired and a warning message has been spoken.
 !  When commands are received, the routine "process_menu_entry" is performed
 !  to implement the menu choice.
 !
      terminator_used = 0%
      illegal_entry_count = 0%
      WHILE (input_key_buf(0) <> 0% OR  &
				terminator_used <> DTK$K_TRM_TIMEOUT)
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
        stat% = get_key_string(null_char$, T1SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_reporting = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		labor_reporting = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_used = DTK$K_TRM_NUMBER_SIGN)
		THEN
			GOSUB process_menu_entry	! Valid command so process it	
			IF (input_key_buf(0) = DTK$K_TRM_ZERO)
			THEN
				GO TO get_long_read
			ELSE
				GO TO get_short_read
			END IF
		ELSE IF ((terminator_used <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_used = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
			!
			!  No keys in typeahead buffer so prompt for a command.
			!
		     THEN
				CALL speak_text(DTK$K_WAIT, top_level_menu$)
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
        stat% = get_key_string(null_char$, T20SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_reporting = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		labor_reporting = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
			GOSUB process_menu_entry	! Valid command so process it	

		ELSE 	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				labor_reporting = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   labor_reporting = 0%
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
		CASE = 	DTK$K_TRM_ZERO
			CALL speak_text(DTK$K_WAIT, top_level_menu$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE
			stat% = labor_entry
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				labor_reporting = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO
			   CALL speak_text(DTK$K_IMMED, review_msg$)
			   CALL speak_text(DTK$K_IMMED, badge_num)

			   IF tot_proj_count = 0%
			   THEN
				CALL speak_text(DTK$K_WAIT, no_data$)
			   ELSE
			      FOR i% = 0% TO (tot_proj_count - 1%)
				CALL speak_text(DTK$K_IMMED, proj_num_msg$)
				CALL speak_text(DTK$K_WAIT, project_num(i%))

				IF ((LEFT$(hours(i%), 1) <> " ") AND &
				    (LEFT$(hours(i%), 1) <> null_char$))
				THEN
				    CALL speak_text(DTK$K_IMMED, hours_msg$)
				    CALL speak_text(DTK$K_WAIT, hours(i%))
				ELSE
				    CALL speak_text(DTK$K_WAIT, no_hours_msg$)
				END IF
			      NEXT i%
			   END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_THREE
			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)
			CALL speak_text(DTK$K_WAIT, charge_number$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_FOUR
			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)
			CALL speak_text(DTK$K_WAIT, labor_report$)
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
	IF illegal_entry_count >= 3
	THEN
		CALL speak_text(DTK$K_WAIT, top_level_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION

 !***************************************************************************
 !  Prompts the user for a command and receives the command (touch tone
 !  key) from the user.  Note, the command will be accepted whether or not
 !  it is terminated with the pound key "#".  However, if the command
 !  is not followed by the pound key, the command will not be processed
 !  until the timeout period has expired and a warning message has been spoken.
 !  When commands are received, the routine "process_menu_entry" is performed
 !  to implement the menu choice.
 !
 FUNCTION LONG labor_entry()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (MESSAGE_TEXT) STRING  msg_access	= 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_noaccess = 100, &
				msg_timeout  = 50

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (labor)		STRING	last_name = 80, &
					badge_num = 80, &
					project_num(30), &
					hours(30)  = 80, &
				LONG	tot_proj_count

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  project_prompt$ = "Please enter the project number followed by a pound-key. "&
		+ "You may enter any number as a test. "

  illegal_term$ = "Illegal terminator. Please terminate your entry with the "&
		+ "pound-key. "

  invalid$	= "Invalid entry. "
  hours_prompt$   = "Please enter hours and tenths of hours charged to project "
  hours_prompt_2$ = "Enter the hours with no decimal point, followed by a "&
		+  "pound-key. " 

  exit_menu$	= "To enter further jobs, press 1. "&
		+ "To exit, press 0. Terminate your entry with the pound-key. "

  exit_prompt$	= "To exit saving data, press 1. "&
		+ "To exit without entering data, press 2. Terminate your " &
		+ "entry with the pound-key. "

  DECLARE	LONG	terminator_used, illegal_entry_count

	sp$ = " "
 	null_char$ = X'00'C
	terminator_used = 0%
	illegal_entry_count = 0%
	proj_count% = tot_proj_count

 LABOR:
	!
	! Keep getting commands from the user until the exit key "*",
	! is entered, a wink is detected, or the timeout period expires.
	! First, check if there are any keys in the typeahead buffer (done by 
	! "get_key_string" with a 1 second timeout).  If so, process them. 
	! Otherwise, prompt the user for a command and wait for his response.
	!
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_WAIT, project_prompt$)

	ELSE	IF (terminator_used = DTK$K_TRM_NUMBER_SIGN)
		THEN
			GO TO  store_proj_num
		ELSE IF ((terminator_used <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_used = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
			!
			!  No keys in typeahead buffer so prompt for a command.
			!
		     THEN
				CALL speak_text(DTK$K_WAIT, project_prompt$)
		     END IF
	         END IF
	     END IF
	!
	!  Any new keys received should be processed after the keys read
	!  from the typeahead buffer.  Start storing touch tone keys into
	!  into the buffer following the keys read from the typeahead buffer.
	!
 read_proj_num:
	num_input_keys = 0%
        stat% = get_key_string(null_char$, T20SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_WAIT, project_prompt$)
		GO TO read_proj_num
	ELSE	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)

		ELSE 	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN			! timeout.  end of call
				CALL speak_all_text(msg_timeout)
				labor_entry = 0%
				EXIT FUNCTION
			END IF
		END IF
	END IF

 store_proj_num:
	!
	!  stores project number in "project_num"
	!

	j% = 1%
	FOR i% = 0% TO (num_input_keys - 1%)
		MID$(project_num(proj_count%), j%, 1) = CHR$(input_key_buf(i%))
		j% = j% + 1
		MID$(project_num(proj_count%), j%, 1) = sp$
		j% = j% + 1
	NEXT i%

	MID$(project_num(proj_count%), j%, 10) = "          "

	num_input_keys   = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_used)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_IMMED, hours_prompt$)
		CALL speak_text(DTK$K_IMMED, project_num(proj_count%))
		CALL speak_text(DTK$K_WAIT, hours_prompt_2$)

	ELSE	IF (terminator_used = DTK$K_TRM_NUMBER_SIGN)
		THEN
			GO TO store_hours

		ELSE IF ((terminator_used <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_used = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
			!
			!  No keys in typeahead buffer so prompt for a command.
			!
		     THEN
				CALL speak_text(DTK$K_IMMED, hours_prompt$)
				CALL speak_text(DTK$K_IMMED, &
						project_num(proj_count%))
				CALL speak_text(DTK$K_WAIT, hours_prompt_2$)
		     END IF
	         END IF
	     END IF
	!
	!  Any new keys received should be processed after the keys read
	!  from the typeahead buffer.  Start storing touch tone keys into
	!  into the buffer following the keys read from the typeahead buffer.
	!
 read_hrs:
	num_input_keys = 0%
        stat% = get_key_string(null_char$, T20SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_WAIT, hours_prompt_2$)
		GO TO read_hrs

	ELSE	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
		ELSE 	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				labor_entry = 0%
				EXIT FUNCTION
			END IF
		END IF
	END IF

 ! %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 store_hours:

	!
	!  store hours  --  the time will be stored in "hours" and "tenths"
	!
	IF (num_input_keys = 1)
	THEN
	    hours(proj_count%) = CHR$(input_key_buf(0))
	ELSE	IF (num_input_keys = 2)
	    THEN	
		hours(proj_count%) = CHR$(input_key_buf(0)) + &
				     CHR$(input_key_buf(1))
	    ELSE
		IF (num_input_keys = 3)
		THEN
		    hours(proj_count%) = CHR$(input_key_buf(0)) + &
					 CHR$(input_key_buf(1)) + &
					" point " + CHR$(input_key_buf(2))
		ELSE
			CALL speak_text(DTK$K_WAIT, invalid$)
			CALL speak_text(DTK$K_IMMED, hours_prompt$)
			CALL speak_text(DTK$K_IMMED, project_num(proj_count%))
			CALL speak_text(DTK$K_WAIT, hours_prompt_2$)
			GO TO read_hrs
		END IF
	    END IF
	END IF

exit_:	num_input_keys   = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_WAIT, exit_menu$)
	ELSE	IF (terminator_used = DTK$K_TRM_NUMBER_SIGN)
		THEN
			GO TO check_exit
		ELSE IF ((terminator_used <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_used = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
			!
			!  No keys in typeahead buffer so prompt for a command.
			!
		     THEN
				CALL speak_text(DTK$K_WAIT, exit_menu$)
		     END IF
	         END IF
	     END IF
	!
	!  Any new keys received should be processed after the keys read
	!  from the typeahead buffer.  Start storing touch tone keys into
	!  into the buffer following the keys read from the typeahead buffer.
	!
 read_exit:
	num_input_keys = 0%
	stat% = get_key_string(null_char$, T20SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_WAIT, exit_menu$)
		GO TO read_exit

	ELSE	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
			GO TO check_exit

		ELSE 	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				labor_entry = 0%
				EXIT FUNCTION
			END IF
		END IF
	END IF
 
 ! %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 check_exit:
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
		CASE = 	DTK$K_TRM_ZERO
			tot_proj_count = proj_count% + 1%
			GO TO exit_verify

		CASE = 	DTK$K_TRM_ONE
			proj_count% = proj_count% + 1%
			GO TO labor

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
		CALL speak_text(DTK$K_WAIT, exit_menu$)
		illegal_entry_count = 0%
	END IF
	GO TO exit_

 !  %%%%%%%%%
 exit_verify:
	num_input_keys   = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_WAIT, exit_prompt$)

	ELSE	IF (terminator_used = DTK$K_TRM_NUMBER_SIGN)
		THEN
			GO TO exit_choice
		ELSE IF ((terminator_used <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_used = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
			!
			!  No keys in typeahead buffer so prompt for a command.
			!
		     THEN
				CALL speak_text(DTK$K_WAIT, exit_prompt$)
		     END IF
	         END IF
	     END IF
	!
	!  Any new keys received should be processed after the keys read
	!  from the typeahead buffer.  Start storing touch tone keys into
	!  into the buffer following the keys read from the typeahead buffer.
	!
 get_exit:
	num_input_keys = 0%
        stat% = get_key_string(null_char$, T20SECOND, terminator_used)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		labor_entry = 0%
		EXIT FUNCTION
	END IF
	IF terminator_used = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, illegal_term$)
		CALL speak_text(DTK$K_WAIT, exit_prompt$)
		GO TO get_exit
	ELSE	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
			GO TO exit_choice

		ELSE 	IF (terminator_used = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				labor_entry = 0%
				EXIT FUNCTION
			END IF
		END IF
	END IF
 exit_choice:
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
		CASE = 	DTK$K_TRM_ONE
			labor_entry = 1%
			EXIT FUNCTION

		CASE = 	DTK$K_TRM_TWO
			FOR i% = 0% TO (tot_proj_count - 1%)
				hours(i%) = "            "
			NEXT i%
			labor_entry = 1%
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
		CALL speak_text(DTK$K_WAIT, exit_prompt$)
		illegal_entry_count = 0%
	END IF
	GO TO exit_verify

 END FUNCTION
