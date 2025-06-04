 ! ---------------------------- care_info.bas --------------------------------
 !
 !  	This file contains all of the functions for the health-care
 !	demonstration.  These functions correspond to the three selections
 !      in the health-care demo menu: health insurance eligibility 
 !	verification, claims status information, and participating physician
 !	location.
 !
 FUNCTION LONG claim_status()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				get_physician_id, &
				get_patient_id, &
				get_service_date, &
				get_total_charge, &
				DTK$SET_VOICE

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
 				customer_service = 160
    
   claim_welcome$ = "Welcome to the physician claim status reporting "&
	+ "system.  "

   contract1$ = "This contract is for patient Mary Smith.  "

   contract2$ = "This contract is for patient Mark Clancy.  "

   contract3$ = "This contract is for patient George Robert.  "

   contract_prompt$ = "If this contract is correct, press 1, followed by "&
	+ "the pound-key.  If this contract is not correct, press 2, followed "&
	+ "by the pound-key.  To exit, press the star key.  "

   claim1$ = "Mary Smith's claim is currently being processed.  Please "&
	+ "[riyk'aontaekt] us in two weeks, if a summary statement is not "&
	+ "received.  "

   claim2$ = "Mark Clancy's claim was processed on June 24th, 1987, and "&
	+ "postmarked June 25th, 1987.  The physician payment amount is "&
	+ "$80.00.  "

   claim3$ = "George Robert's claim was finalized on June 30th, 1987.  "&
	+ "There is no entitled payment for this claim.  "

   claim_exit$ = "To check another patient's claim status, press 1, "&
	+ "followed by the pound-key.  To speak to a customer-service "&
	+ "representative, press 9, followed by the pound-key.  To exit "&
	+ "this section, press the star-key.  "


  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  COMMON (health_care)		LONG	call_count  

  DECLARE	LONG	terminator_value, illegal_entry_count

    CALL speak_text(DTK$K_IMMED, claim_welcome$)
    !
    !  Get the caller's physician identification number
    !  and the patient's identification number.
    !
    stat% = get_physician_id
    if (stat% AND STS$M_SUCCESS) = 0%
    THEN
	claim_status = 0%
	EXIT FUNCTION
    END IF 

new_patient:

    stat% = get_patient_id
    if (stat% AND STS$M_SUCCESS) = 0%
    THEN
	claim_status = 0%
	EXIT FUNCTION
    END IF
    !
    !  Tell the physician the patient's name for the patient id
    !  number entered and allow him to verify it or to reenter.
    !
    SELECT call_count
        CASE = 0%
            CALL speak_text(DTK$K_IMMED, contract1$)		
        CASE = 1%
	    CALL speak_text(DTK$K_IMMED, contract2$)
	CASE ELSE
	    CALL speak_text(DTK$K_IMMED, contract3$)
    END SELECT	   
    WHILE (input_key_buf(0) <> 0% OR  &
				terminator_value <> DTK$K_TRM_TIMEOUT)
	!
        ! Keep getting commands from the user until the exit key "*",
	! is entered, a wink is detected, or the timeout period expires.
        ! First, prompt the user for a command and wait for his response.
	!
        input_key_buf(0) = 0%
        num_input_keys = 0%
        CALL speak_text(DTK$K_WAIT, contract_prompt$)
        stat% = get_key_string(null_char$, T20SECOND, terminator_value)
        IF (stat% AND STS$M_SUCCESS) = 0%
        THEN
	    claim_status = 0%
	    EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
	    claim_status = 1%
	    EXIT FUNCTION
	ELSE	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
	    THEN
		CALL speak_all_text(msg_timeout)
		claim_status = 0%
		EXIT FUNCTION
	    ELSE    IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
		    CALL speak_all_text(msg_no_terminator)
		END IF
	    END IF
	END IF
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
		CASE = 	DTK$K_TRM_ZERO			! Give user help
		    illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE			! Correct contract
		    GO TO get_date

		CASE = 	DTK$K_TRM_TWO			! Incorrect contract
		    SELECT call_count
			CASE = 0%
			    call_count = 1%
			CASE = 1%
			    call_count = 2%
			CASE ELSE
			    call_count = 0%
		    END SELECT	   
		    GO TO new_patient

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
	    CALL speak_text(DTK$K_WAIT, contract_prompt$)
	    illegal_entry_count = 0%
	END IF
   NEXT
   claim_status = 0%
   EXIT FUNCTION

get_date:
        !
	! Get date of service and total charge from the physician
	! and then read the patient's claim status to him/her
	!
	stat% = get_service_date
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		claim_status = 0%
		EXIT FUNCTION
	END IF 
	SELECT call_count
	    CASE = 0%
	        CALL speak_text(DTK$K_IMMED, claim1$)
		call_count = 1%
	    CASE = 1%
	        CALL speak_text(DTK$K_IMMED, claim2$)
	        call_count = 2%
	    CASE ELSE
    	        CALL speak_text(DTK$K_IMMED, claim3$)
		call_count = 0%
	END SELECT	   
	!
	!  Now check if the person wishes to speak to a customer
	!  service representative or just wishes to exit this menu.
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
		claim_status = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		claim_status = 1%
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
				CALL speak_text(DTK$K_WAIT, claim_exit$)
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
		claim_status = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		claim_status = 1%
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
				claim_status = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   claim_status = 0%
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
			illegal_entry_count = 0%
		    
		CASE =  DTK$K_TRM_ONE		        ! Another claim status
		        illegal_entry_count = 0%
			GO TO new_patient
    
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
		CALL speak_text(DTK$K_WAIT, claim_exit$)
		illegal_entry_count = 0%
	END IF

	RETURN
 END FUNCTION

 ! ----------------------- get_physician_id -------------------------

 FUNCTION LONG get_physician_id()

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

   physician_id$ = "Please enter your participating physician's "&
	+ "identification number, followed by the pound-key.  You may "&
	+ "enter any number as a test.  "

   DECLARE LONG term_code

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string(physician_id$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_physician_id = 0%
		EXIT FUNCTION
	END IF
	get_physician_id = 1%
 END FUNCTION

 ! ----------------------- get_patient_id -------------------------

 FUNCTION LONG get_patient_id()

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

   patient_id$ = "Please enter the patient's identification number, followed "&
	+ "by the pound-key.  You may enter any number as a test.  "


   DECLARE LONG term_code

 	num_input_keys = 0%
	stat% = get_key_string(patient_id$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_patient_id = 0%
		EXIT FUNCTION

	END IF
	get_patient_id = 1%
 END FUNCTION

 ! ----------------------- get_service_date -------------------------

 FUNCTION LONG get_service_date()

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

   service_date$ = "Please enter the date of service, followed by "&
	+ "the pound-key.  Use two digit codes for month, followed "&
	+ "by day, and year.  You may enter any number as a test.  "

   DECLARE LONG term_code

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string(service_date$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_service_date = 0%
		EXIT FUNCTION
	END IF
	get_service_date = 1%
 END FUNCTION

 ! ----------------------- get_total_charge -------------------------

 FUNCTION LONG get_total_charge()

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

   charge$ = "Please enter the total charge, followed by the pound-key.  "&
	+ "You may enter any amount, as a test.  "

   DECLARE LONG term_code

 	num_input_keys = 0%
	stat% = get_key_string(charge$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_total_charge = 0%
		EXIT FUNCTION

	END IF
	get_total_charge = 1%
 END FUNCTION

! ------------------------ insurance_verification -------------------------

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
        + "area.  Dr. Henry D. Child is located at 43 [n'eysaxn] [str`iyt], "&
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


