 ! ---------------------------- insure.bas --------------------------------
 !
 !  	This file contains all of the functions for the insurance
 !	demonstration.  These functions correspond to the four selections
 !      in the insurance demo menu: insurance policy information, 
 ! 	participating physician location, health insurance eligibility
 !	verification, and auto insurance claims.
 !
 FUNCTION LONG policy_info()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				get_policy_no, &
				get_security_code, &
				DTK$SET_VOICE

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160


   policy$	 = "Policy number "
   policy_value$ = " is a ten thousand dollar term policy.  "&
	+ "Dividends accumulated are worth nine hundred dollars.  There "&
	+ "is a loan of eight hundred fifty dollars outstanding.  Last "&
	+ "premium of ninety five dollars was paid in January of 1987.  "&
	+ "Term expires in March 1990.  "

   policy_exit$ = "To speak to a customer-service representative, press 9, "&
	+ "followed by the pound-key.  To repeat this message, press 0, "&
	+ "followed by the pound-key.  To exit this section, press the "&
	+ "star-key.  "


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

	!
	!  Get the caller's policy number and security code, and
	!  then read him the policy value.
	!
	stat% = get_policy_no
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		policy_info = 0%
		EXIT FUNCTION
	END IF
	pnumber$ = " "
	FOR i% = 0% TO num_input_keys - 1
		pnumber$ = pnumber$ + CHR$(input_key_buf(i%))
		pnumber$ = pnumber$ + " "
	NEXT i%
	stat% = get_security_code
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		policy_info = 0%
		EXIT FUNCTION
	END IF
        CALL speak_text(DTK$K_IMMED, policy$)
	CALL speak_text(DTK$K_IMMED, pnumber$)
	CALL speak_text(DTK$K_IMMED, policy_value$)
	!
	!  Now check if the caller wants to talk to a customer service
	!  representative or if he wants to exit the policy information demo
	!
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
		policy_info = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		policy_info = 1%
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
				CALL speak_text(DTK$K_WAIT, policy_exit$)
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
		policy_info = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		policy_info = 1%
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
				policy_info = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   policy_info = 0%
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
			CALL speak_text(DTK$K_WAIT, policy_exit$)
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
		CALL speak_text(DTK$K_WAIT, policy_exit$)
		illegal_entry_count = 0%
	END IF
	RETURN
 END FUNCTION

 ! ----------------------- get_policy_no -------------------------

 FUNCTION LONG get_policy_no()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

   policy_no$ = "For policy information, please enter your policy "&
	+ "identification number followed by the pound-key.  You may "&
	+ "enter any number as a test.  "

   DECLARE LONG term_code

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string(policy_no$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_policy_no = 0%
		EXIT FUNCTION
	END IF
	get_policy_no = 1%
 END FUNCTION

 ! ----------------------- get_security_code -------------------------

 FUNCTION LONG get_security_code()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

   security_code$ = "Enter your security code followed by the pound-key.  "&
		+ "You may enter any number as a test.  "

   DECLARE LONG term_code

 	num_input_keys = 0%
	stat% = get_key_string(security_code$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_security_code = 0%
		EXIT FUNCTION

	END IF
	get_security_code = 1%
 END FUNCTION


 ! ----------------- Participating Physician Locations ------------------
 !
 !  This section will give the Participating Physician locations in the
 !  greater maynard area.  The demo asks for a zip code, then speaks a 
 !  canned message containing the physician locations.  To tailor this
 !  section to a different region, the string "doctor_location$" can be
 !  modified.
 !
 FUNCTION LONG physician_location()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  DECLARE LONG term_code

   zip_code$ = "Enter your zip code followed by the pound-key.  You may "&
	+ "enter any number as a test. "

   doctor_location$ = "There are two participating physicians in your local "&
        + "area.  Dr. Henry D. Child is located at 43 [n'eysxn] [str`iyt], "&
        + "Maynard.  His phone number is 8 9 7, 2000.  Dr. John B. Knight is "&
	+ "located at 321 Main Street, Maynard.  His phone number is "&
	+ "2 6 3, 1 8 7 5.  "

   input_key_buf(0) = 0%
   num_input_keys = 0%

   stat% = get_key_string(zip_code$, T20SECOND, term_code)
   IF (stat% AND STS$M_SUCCESS) = 0%
   THEN
	physician_location = 0%
	EXIT FUNCTION
   END IF
   CALL speak_text(DTK$K_WAIT, doctor_location$)
   physician_location = 1%
 END FUNCTION

 FUNCTION LONG insurance_verification()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				get_ssn, &
				DTK$SET_VOICE

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160


   employee$ = "Employee number "
   verify_info$ = " is employed by Digital Equipment "&
	+ "Corporation.  He is covered for hospital and medical benefits.  "&
	+ "A deductible of $56.00 is yet to be satisfied, and "&
	+ "co insurance is 80 percent.  If the service is the result of "&
	+ "an accident, benefits up to $500.00 will be paid in full.  "

   no_dependent$ = "There is no dependent health insurance coverage "&
	+ "for this employee.  "

   verify_menu$ = "To verify a claim for an employee, press 1, followed "&
	+ "by the pound-key.  To verify a claim for an employee's spouse, "&
	+ "press 2, followed by the pound-key.  To verify a claim for a "&
	+ "dependent child, press 3, followed by the pound-key.  To speak "&
	+ "to a customer-service representative, press 9, followed by the "&
 	+ "pound-key.  To repeat this message, press 0, followed by the "&
	+ "pound-key.  To exit this section, press the star-key.  "


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
	!
	!  Get the caller's social security number
	!
	stat% =  get_ssn
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		insurance_verification = 0%
		EXIT FUNCTION
	END IF
    	snumber$ = " "
    	FOR i% = 0% TO num_input_keys - 1
	    snumber$ = snumber$ + CHR$(input_key_buf(i%))
 	    snumber$ = snumber$ + " "
    	NEXT i%
	!
	!  Now check if the caller wants to verify insurance coverage
	!  for the employee, the employee's spouse or the employee's children.
	!
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
		insurance_verification = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		insurance_verification = 1%
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
				CALL speak_text(DTK$K_WAIT, verify_menu$)
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
		insurance_verification = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		insurance_verification = 1%
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
				insurance_verification = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   insurance_verification = 0%
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
			CALL speak_text(DTK$K_WAIT, verify_menu$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE			! Say employee coverage
        	    	CALL speak_text(DTK$K_IMMED, employee$)
		    	CALL speak_text(DTK$K_IMMED, snumber$)
			CALL speak_text(DTK$K_IMMED, verify_info$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO			! Say spouse's coverage
			CALL speak_text(DTK$K_WAIT, no_dependent$)
			illegal_entry_count = 0%


		CASE = 	DTK$K_TRM_THREE			! Say child's coverage
			CALL speak_text(DTK$K_WAIT, no_dependent$)
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
		CALL speak_text(DTK$K_WAIT, verify_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN
 END FUNCTION

 ! -------------------------- get_ssn ----------------------------

 FUNCTION LONG get_ssn()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

   ssn$ = "To verify health insurance coverage, enter your social "&
	+ "security number followed by the pound-key.  You may enter "&
	+ "any number as a test.  "

   DECLARE LONG term_code

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string(ssn$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_ssn = 0%
		EXIT FUNCTION
	END IF
	get_ssn = 1%
 END FUNCTION

 ! --------------------  auto insurance claims menu ------------------------
 !
 !  This section contains the auto insurance claims demo
 !  The menu choices are:
 !
 !	1) Adjuster location
 !	2) Approved auto body repair shops
 ! 	3) Speak to a customer-service representative
 !
 FUNCTION LONG claims()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				adjuster, &
				repair_shop, &
				DTK$SET_VOICE

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

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

 
   claims_menu$ = "For insurance adjuster locations, press 1, followed by "&
	+ "the pound-key.  For approved auto body repair shops, press 2, "&
	+ "followed by the pound key.  To speak to a customer-service "&
	+ "representative, press 9, followed by the pound-key.  To repeat "&
	+ "this message, press 0, followed by the pound-key.  To exit, "&
	+ "press the star-key."   

  DECLARE	LONG	terminator_value, illegal_entry_count

 	null_char$ = X'00'C
	illegal_entry_count = 0%
	num_input_keys = 0%
	input_key_buf(0) = 0%
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
		claims = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		claims = 1%
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
				CALL speak_text(DTK$K_WAIT, claims_menu$)
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
		claims = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		claims = 1%
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
				claims = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   claims = 0%
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
			CALL speak_text(DTK$K_WAIT, claims_menu$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE			! Give nearest adjuster
			stat% = adjuster
			if (stat% AND STS$M_SUCCESS) = 0%
			THEN
			    claims = 0%
			    EXIT FUNCTION
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO			! Give approved repair
			stat% = repair_shop
			if (stat% AND STS$M_SUCCESS) = 0%
			THEN
			    claims = 0%
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
		CALL speak_text(DTK$K_WAIT, claims_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN
 END FUNCTION


 ! --------------------- Insurance Adjuster Location --------------------
 !
 !  This section will give the drive-in insurance adjuster locatios in
 !  the greater Maynard area.  The demo asks for a zip code, then speaks
 !  a canned message containing the insurance adjuster locations.  To 
 !  tailor this section to a different region, the string "adjuster_location$"
 !  can be modified.
 !
 FUNCTION LONG adjuster()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  DECLARE LONG term_code

   zip_code$ = "Enter your zip code followed by the pound-key.  You may "&
	+ "enter any number as a test. "

   adjuster_location$ = "There is one drive-in insurance adjuster in your "&
	+ "local area.  West Suburban Adjustment is located at 596 Boston "&
	+ "Post Road, Marlboro.  Their phone number is 4 8 1, 6 7 9 8.  "

   input_key_buf(0) = 0%
   num_input_keys = 0%

   stat% = get_key_string(zip_code$, T20SECOND, term_code)
   IF (stat% AND STS$M_SUCCESS) = 0%
   THEN
	adjuster = 0%
	EXIT FUNCTION
   END IF
   CALL speak_text(DTK$K_WAIT, adjuster_location$)
   adjuster = 1%
 END FUNCTION

 ! ----------------- Approved auto body repair shops --------------------
 !
 !  This section will give the approved auto body repair shops in the
 !  greater maynard area.  The demo asks for a zip code, then speaks a 
 !  canned message containing the auto body repair shop locations.  
 !  To tailor this section to a different region, the string 
 !  "repair_location$" can be modified.
 !
 FUNCTION LONG repair_shop()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  DECLARE LONG term_code

   zip_code$ = "Enter your zip code followed by the pound-key.  You may "&
	+ "enter any number as a test. "

   repair_location$ = "There are two approved auto body repair shops in "&
	+ "your local area.  Walnut Street Auto Body is located at 12 "&
	+ "Walnut Street, Maynard.  Their phone number is 8 9 7, 1 2 3 4.  "&
	+ "Mass General Auto Body is located at 15 Howard Road, Maynard.  "&
	+ "Their phone number is 8 9 7, 9000.  "

   input_key_buf(0) = 0%
   num_input_keys = 0%
   stat% = get_key_string(zip_code$, T20SECOND, term_code)
   IF (stat% AND STS$M_SUCCESS) = 0%
   THEN
	repair_shop = 0%
	EXIT FUNCTION
   END IF
   CALL speak_text(DTK$K_WAIT, repair_location$)
   repair_shop = 1%
 END FUNCTION
