 ! ---------------------------- banking.bas ------------------------
 !
 !	This subroutine contains all the code for the "BANKING" section of
 !	the ACT DECtalk demo program.
 !	The menu choices contain:
 !
 !		1) SAVINGS ACCOUNT BALANCE
 !		2) CHECKING ACCOUNT BALANCE
 !		3) TRANSFER BETWEEN ACCOUNTS
 !		9) TRANSFER TO A CUSTOMER SERVICE REP.
 !
 !	Some arbitrary account balances are used.
 !
 FUNCTION LONG banking()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	DTK$SET_VOICE, &
				get_key_string, &
				get_account_no, &
				transfer

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  COMMON (banking)		LONG	savings_bal, &
					checking_bal, &
					savings_flag, &
					checking_flag

   bank_welcome$ = "[+] Welcome to the DECtalk banking-demonstration. "

   banking_main_menu$ = "Please enter a command. For help, press 0 followed by"&
		+ " the pound-key. To exit, press the star-key."

   banking_help$ = "To check your [s'eyvixnxz#axk`awnt] balance, press 1. "&
		 + "For your [ch'ehkixnx#axk`awnt] balance, press 2. "&
		 + "To transfer money between accounts, press 3. "&
		 + "To speak to a customer-service representative, press 9. "&
		 + "To exit BANKING, press the star-key. "&
		 + "To repeat this message, press 0.  Terminate your entry "&
		 + "with the pound-key. "

   savings_bal$ = "As of           the balance for your savings account is "
   checking_bal$ = "As of           the balance for your checking  account is "

  DECLARE	LONG	terminator_value, illegal_entry_count

 	null_char$ = X'00'C
	illegal_entry_count = 0%
	savings_bal = 13248%
	checking_bal = 4492%
	savings_flag = 0%
	checking_flag = 0%

	CALL speak_text(DTK$K_WAIT, bank_welcome$)

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
		banking = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		banking = 1%
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
				CALL speak_text(DTK$K_WAIT, banking_help$)
 !-----------------------------------------------------------------------------
 !  take expert user code out of banking -- I think this is more clear
 !
 !				CALL speak_text(DTK$K_WAIT, banking_main_menu$)
 !-----------------------------------------------------------------------------
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
		banking = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		banking = 1%
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
				banking = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   banking = 0%
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
			CALL speak_text(DTK$K_WAIT, banking_help$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_ONE
			IF savings_flag = 0%
			THEN
				stat% = get_account_no(1%)
				IF (stat% AND STS$M_SUCCESS) = 0%
				THEN
					banking = 0%
					EXIT FUNCTION
				END IF
			END IF
			savings_flag = 1%

			amount$ = "$"
			xx% = savings_bal
			FOR i% = 0% TO 4%
			   digit% = xx% / 10**(4%-i%)
			   IF digit% <= 0
			   THEN    GO TO nxt
			   ELSE
				amount$ = amount$ + CHR$(digit% + 48)
				xx% = xx% - (digit% * 10**(4%-i%))
			   END IF
 nxt:			NEXT i%

			MID$(savings_bal$, 7%, 9%) = DATE$(0)
			MID$(savings_bal$, 13%, 3%) = "   "

			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)
			CALL speak_text(DTK$K_IMMED, savings_bal$)
			CALL speak_text(DTK$K_WAIT, amount$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO
			IF checking_flag = 0%
			THEN
				stat% = get_account_no(2%)
				IF (stat% AND STS$M_SUCCESS) = 0%
				THEN
					banking = 0%
					EXIT FUNCTION
				END IF
			END IF
			checking_flag = 1%

			amount$ = "$"
			xx% = checking_bal
			FOR i% = 0% TO 4%
			   digit% = xx% / 10**(4%-i%)
			   IF digit% <= 0
			   THEN    GO TO c_nxt
			   ELSE
				amount$ = amount$ + CHR$(digit% + 48)
				xx% = xx% - (digit% * 10**(4%-i%))
			   END IF
 c_nxt:			NEXT i%

			MID$(checking_bal$, 7%, 9%) = DATE$(0)
			MID$(checking_bal$, 13%, 3%) = "   "

			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)

			CALL speak_text(DTK$K_IMMED, checking_bal$)
			CALL speak_text(DTK$K_WAIT, amount$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_THREE	
			stat% = transfer
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				banking = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_NINE
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
		CALL speak_text(DTK$K_WAIT, banking_help$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION

 ! --------------------  transfer -----------------------
 !
 !  This section handles the transfer of money between savings and checking
 !  accounts.  The menu choices are:
 !
 !	1) TRANSFER FROM SAVINGS TO CHECKING
 !	2) TRANSFER FROM CHECKING TO SAVINGS
 !	9) SPEAK TO A CUSTOMER SERVICE REP.
 !
 FUNCTION LONG transfer()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	DTK$SET_VOICE, &
				get_key_string, &
				get_account_no, &
				transaction

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  COMMON (banking)		LONG	savings_bal, &
					checking_bal, &
					savings_flag, &
					checking_flag

   transfer_menu$ = "To transfer from savings to checking, press 1. "&
		+ "to transfer from checking to savings, press 2. "&
		+ "to speak to a customer-service representative, press 9. "&
		+ "to cancel the transfer, press the star-key. "&
		+ "Terminate your entry with the pound-key. "

   no_trans$ = "No account-transfer done. "

  DECLARE	LONG	terminator_value, illegal_entry_count

    null_char$ = X'00'C
    illegal_entry_count = 0%

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
	input_key_buf(0) = 0%
	num_input_keys = 0%
        stat% = get_key_string(null_char$, T1SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		transfer = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, no_trans$)
		transfer = 1%
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
				CALL speak_text(DTK$K_WAIT, transfer_menu$)
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
		transfer = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, no_trans$)
		transfer = 1%
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
				transfer = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   transfer = 0%
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
			CALL speak_text(DTK$K_WAIT, transfer_menu$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE
			choice% = 1%
			IF (savings_flag = 0%)
			THEN
				stat% = get_account_no(1%)
				IF (stat% AND STS$M_SUCCESS) = 0%
				THEN
					transfer = 0%
					EXIT FUNCTION
				END IF
				savings_flag = 1%
			END IF
			IF (checking_flag = 0%)
			THEN
				stat% = get_account_no(2%)
				IF (stat% AND STS$M_SUCCESS) = 0%
				THEN
					transfer = 0%
					EXIT FUNCTION
				END IF
				checking_flag = 1%
			END IF
			stat% = transaction(choice%)
				IF (stat% AND STS$M_SUCCESS) = 0%
				THEN
					transfer = 0%
					EXIT FUNCTION
				END IF
			transfer = 1%
			EXIT FUNCTION

		CASE = 	DTK$K_TRM_TWO
			choice% = 2%
			IF (savings_flag = 0%)
			THEN
				stat% = get_account_no(1%)
				IF (stat% AND STS$M_SUCCESS) = 0%
				THEN
					transfer = 0%
					EXIT FUNCTION
				END IF
				savings_flag = 1%
			END IF
			IF (checking_flag = 0%)
			THEN
				stat% = get_account_no(2%)
				IF (stat% AND STS$M_SUCCESS) = 0%
				THEN
					transfer = 0%
					EXIT FUNCTION
				END IF
				checking_flag = 1%
			END IF
			stat% = transaction(choice%)
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				transfer = 0%
				EXIT FUNCTION
			END IF
			transfer = 1%
			EXIT FUNCTION

		CASE = 	DTK$K_TRM_NINE
			stat% = DTK$SET_VOICE(voice_id, DTK$K_VOICE_DEEP_MALE,&
						speaking_rate, &
						comma_pause_duration,&
						period_pause_duration)

			CALL speak_text(DTK$K_WAIT, customer_service)
			transfer = 1%
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
		CALL speak_text(DTK$K_WAIT, transfer_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION

 ! ------------------------- TRANSACTION -----------------------

 FUNCTION LONG transaction(LONG choice)

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

  COMMON (banking)	LONG	savings_bal, &
				checking_bal, &
				savings_flag, &
				checking_flag

   DECLARE LONG terminator_value

	money_prompt$ = "Please enter a whole dollar amount followed by the "&
		+ "pound-key. "

	no_money_s$  = "There are insufficient funds in your savings account "&
		+ "to transfer "
	no_money_c$  = "There are insufficient funds in your checking account "&
		+ "to transfer "

	trans$ = "You have transferred "
	sav_to_check$ = "from your savings to your checking account. "
	check_to_sav$ = "from your checking to your savings account. "

	receipt$ = "You will receive a written confirmation by mail. "

	no_trans$ = "No account-transfer done. "

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string(money_prompt$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		transaction = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, no_trans$)
		transaction = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_value = DTK$K_TRM_NUMBER_SIGN)
		THEN
			GO TO  math
		ELSE IF ((terminator_value <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_value = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
			!
			!  No keys in typeahead buffer so prompt for a command.
			!
		     THEN
				CALL speak_text(DTK$K_WAIT, money_prompt$)
		     END IF
	         END IF
	     END IF
	!
	!  Any new keys received should be processed after the keys read
	!  from the typeahead buffer.  Start storing touch tone keys into
	!  into the buffer following the keys read from the typeahead buffer.
	!
	num_input_keys = 0%
        stat% = get_key_string(null_char$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		transaction = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		CALL speak_text(DTK$K_WAIT, no_trans$)
		transaction = 1%
		EXIT FUNCTION
	ELSE	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
			GO TO math

		ELSE 	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				transaction = 0%
				EXIT FUNCTION
			END IF
		END IF
	END IF

 math:
	amount$ = "$"

	FOR i% = 0% TO num_input_keys - 1
		amount$ = amount$ + CHR$(input_key_buf(i%))
	NEXT i%

	amount% = 0%
	FOR j% = 0% TO i%
		x% = (input_key_buf(j%)- 48%) * (10 ** (i% - j%))
		IF x% <> 0%
		THEN
			amount% = amount% + x%
		END IF
	NEXT j%

	IF choice = 1%
	THEN
		IF (savings_bal - amount%) < 0%
		THEN
			CALL speak_text(DTK$K_IMMED, no_money_s$)
			CALL speak_text(DTK$K_IMMED, amount$)
			CALL speak_text(DTK$K_IMMED, sav_to_check$)
		ELSE
			savings_bal = savings_bal - amount%
			checking_bal = checking_bal + amount%

			CALL speak_text(DTK$K_IMMED, trans$)
			CALL speak_text(DTK$K_IMMED, amount$)
			CALL speak_text(DTK$K_IMMED, sav_to_check$)

			CALL speak_text(DTK$K_WAIT, receipt$)
		END IF
	ELSE
		IF (checking_bal - amount%) < 0%
		THEN
			CALL speak_text(DTK$K_IMMED, no_money_c$)
			CALL speak_text(DTK$K_IMMED, amount$)
			CALL speak_text(DTK$K_IMMED, check_to_sav$)
		ELSE
			checking_bal = checking_bal - amount%
			savings_bal = savings_bal + amount%

			CALL speak_text(DTK$K_IMMED, trans$)
			CALL speak_text(DTK$K_IMMED, amount$)
			CALL speak_text(DTK$K_IMMED, check_to_sav$)

			CALL speak_text(DTK$K_WAIT, receipt$)
		END IF
	END IF

	transaction = 1%
 END FUNCTION

 ! ----------------------- get_account_no -------------------------

 FUNCTION LONG get_account_no(LONG account_type)

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

   s_account_no$ = "Enter your savings account number followed by the "&
		+ "pound-key.  You may enter any account number as a test.  "

   c_account_no$ = "Enter your checking account number followed by the "&
		+ "pound-key.  You may enter any account number as a test.  "

   access_code$ = "Enter your access code followed by the pound-key.  "&
		+ "You may enter any number as a test.  "

   DECLARE LONG term_code

	IF account_type = 1%
	THEN
		stat% = get_key_string( s_account_no$, T20SECOND, term_code)
	ELSE
		stat% = get_key_string( c_account_no$, T20SECOND, term_code)
	END IF

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		transaction = 0%
		EXIT FUNCTION
	END IF

	stat% = get_key_string( access_code$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		transaction = 0%
		EXIT FUNCTION

	END IF

	get_account_no = 1%
 END FUNCTION
