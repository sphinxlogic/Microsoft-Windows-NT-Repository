 ! ---------------------------- financial.bas ------------------------
 !
 !	This subroutine contains all the code for the financial information
 !	section of the ACT DECtalk demo program.
 !	The menu choices contain:
 !		1) MORTGAGE RATE INFORMATION
 !		2) ATM LOCATIONS
 !		3) CREDIT CARD INFORMATION
 !		9) TRANSFER TO CUSTOMER SERVICE REP.
 !
 FUNCTION LONG financial_info()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	DTK$SET_VOICE, &
				get_key_string, &
				atm, &
				credit_card

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160

  financial_info$ = "[+]Welcome to the financial information demonstration. "

  main_menu$	= "Please enter a command. For help, press 0 followed by"&
		+ " the pound-key. To exit, press the star-key."

  financial_help$ = "For Mortgage-rate information, press 1.  "&
		+ "For ['eyt`iy'ehm] locations, press 2. "&
		+ "For credit-card information, press 3. "&
		+ "To speak to a customer-service representative, press 9. "&
		+ "To repeat this message, press 0. "&
		+ "Terminate your entry with the pound-key. "&
		+ "To exit this section, press the star-key. "

  mortgage$  = "Welcome to [hx'aemdaxn] National Bank's Mortgage-Line. All of "&
	+ "the following rates are subject to change. The application fee is "&
	+ "$250. 30 year rates with 10% [']down are 8.9% with 3 [aen ax] half "&
	+ "[p`oynts], 9.2% with 3 [p`oynts], 9.5% with 2 [aen ax] half "&
	+ "[p`oynts], 9.8% with no [p`oynts]. 15 year rates with 10% [']down "&
	+ "are 8.9% with 2 [aen ax] half [p`oynts], 9.1% with 2 [p`oynts], and"&
	+ " 9.5% with no [p`oynts]. Adjustable rates are at 8.5% fixed for 3 "&
	+ "years, with 2% a year and 6% lifetime caps thereafter. "

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
	terminator_value = 0%
	illegal_entry_count = 0%

	CALL speak_text(DTK$K_WAIT, financial_info$)

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
		financial_info = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		financial_info = 1%
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
				CALL speak_text(DTK$K_WAIT, main_menu$)
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
		financial_info = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		financial_info = 1%
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
				financial_info = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   financial_info = 0%
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
			CALL speak_text(DTK$K_WAIT, financial_help$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE			! mortgage rate info
			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)

			CALL speak_text(DTK$K_WAIT, mortgage$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO			! atm locations
			stat% = atm
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				financial_info = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_THREE			! credit card info
			stat% = credit_card
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				financial_info = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_NINE			! customer service
			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)
			CALL speak_text(DTK$K_WAIT, customer_service)
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
		CALL speak_text(DTK$K_WAIT, financial_help$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION

 ! -----------------  ATM locations -------------------------- 
 !
 !  This section will give the ATM locations in the greater maynard area.
 !  The demo asks for a zip code, then speaks a canned message containing
 !  atm locations.   To tailor this section to a different region, the string
 !  "atm_locations$" can be modified.
 !
 FUNCTION LONG atm()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  DECLARE LONG term_code

   zip_code$ = "Enter your zip code followed by the pound-key.  You may "&
	+ "enter any number as a test. "

   atm_locations$ = "There are ['ey*t`iy'ehm] machines in your local area, "&
	+ "located at 25 [n'eysxn] [str`iyt], Maynard.  1 18 Great Road, Stow."&
	+ " 86 [p'awdrrm`ihl] Road, Maynard. "

   input_key_buf(0) = 0%
   num_input_keys = 0%

   stat% = get_key_string(zip_code$, T20SECOND, term_code)
   IF (stat% AND STS$M_SUCCESS) = 0%
   THEN
	financial_info = 0%
	EXIT FUNCTION
   END IF

   CALL speak_text(DTK$K_WAIT, atm_locations$)

   atm = 1%
 END FUNCTION		! end atm

 ! --------------------  credit card menu ------------------------
 !
 !  This section contains the credit card information demo.  
 !  The menu choices are:
 !
 !	1) BILLING INFORMATION
 !	2) AVAILABLE CREDIT LINE
 !	3) LOST CARD INFORMATION
 !
 FUNCTION LONG credit_card()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

   credit_menu$ =  "For billing-information, press 1. "&
		+ "For available credit-line, press 2. "&
		+ "For lost card-information, press 3. "&
		+ "To exit, press the star-key.  Terminate your entry with "&
		+ "the pound-key. To repeat this message, press 0. "

   billing$ = "Your minimum payment is $40.  Total balance is $1000.  "&
	+ "Date of last payment is January 23, 1987.  Total payment "&
	+ "made was $65. "

   cred_line$ = "You have $500 available on your credit-line of $1500. "

   lost_card$ = "Please call immediately. 1, 800, 2 3 5,  6000, to report "&
	+ "your lost card. "

   account_no$ = "enter your account number followed by the pound-key.  You "&
	+ "may enter any number as a test. "

  DECLARE	LONG	terminator_value, illegal_entry_count
 	null_char$ = X'00'C
	illegal_entry_count = 0%
	num_input_keys = 0%
	input_key_buf(0) = 0%

        stat% = get_key_string(account_no$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		financial_info = 0%
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
		financial_info = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		financial_info = 1%
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
				CALL speak_text(DTK$K_WAIT, credit_menu$)
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
		financial_info = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		financial_info = 1%
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
				financial_info = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   financial_info = 0%
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
			CALL speak_text(DTK$K_WAIT, credit_menu$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_ONE
			CALL speak_text(DTK$K_WAIT, billing$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_TWO
			CALL speak_text(DTK$K_WAIT, cred_line$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_THREE
			CALL speak_text(DTK$K_WAIT, lost_card$)
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
		CALL speak_text(DTK$K_WAIT, credit_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION

