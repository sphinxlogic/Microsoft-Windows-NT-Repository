 ! ---------------------------- laboratory.bas --------------------------------
 !
 !  	This file contains all of the functions for the laboratory 
 ! 	results reporting demonstration and the radiology results 
 !	reporting demonstration.  These functions correspond to the
 !	two selections in the health care demo menu: laboratory results
 !	and radiology results.  
 !
 FUNCTION LONG laboratory()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				get_physician_code, &
				get_patient_id, &
				speak_lab_results, &
				speak_radiology_results, &
				DTK$SET_VOICE

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160

   contract1$ = "These [l'aebraxtoriy] results are for Mary Smith.  "&
	+ "If this is correct, press 1, followed by the pound-key.  "&
	+ "If this is incorrect, press 2, followed by the pound-key.  "&
	+ "To exit, press the star-key.  "

   contract2$ = "These [l'aebraxtoriy] results are for Betty Blood.  "&
	+ "If this is correct, press 1, followed by the pound-key.  "&
	+ "If this is incorrect, press 2, followed by the pound-key.  "&
	+ "To exit, press the star-key.  "

   contract3$ = "These [l'aebraxtoriy] results are for John Doe.  "&
	+ "If this is correct, press 1, followed by the pound-key.  "&
	+ "If this is incorrect, press 2, followed by the pound-key.  "&
	+ "To exit, press the star-key.  "

   lab_exit$ = "For [reydiy'aolaxjhiy] results for this patient, press 1, "&
	+ "followed by the pound-key.  For [l'aebraxtoriy] results for "&
	+ "another patient, press 2, followed by the pound-key.  To exit, "&
	+ "press the star-key.  "

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  COMMON (results_reporting)    LONG    call_count

  DECLARE	LONG	terminator_value, illegal_entry_count

	!
	!  Get the caller's physician identification code and
	!  the id number of the patient whose lab results are
	!  desired and then speak the results to the caller.
	!
	stat% = get_physician_code
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		laboratory = 0%
		EXIT FUNCTION
	END IF
new_patient:

	stat% = get_patient_id
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		laboratory = 0%
		EXIT FUNCTION
	END IF
	WHILE (input_key_buf(0) <> 0% OR  &
				terminator_value <> DTK$K_TRM_TIMEOUT)
	    !
	    ! Tell the physician the patient's name for the patient id
	    ! number entered and allow him to verify it or to reenter.
            ! Keep getting commands from the user until the exit key "*",
	    ! is entered, a wink is detected, or the timeout period expires.
            ! First, prompt the user for a command and wait for his response.
	    !
	    SELECT call_count
		CASE = 0%
		    CALL speak_text(DTK$K_IMMED, contract1$)		
		CASE = 1%
		    CALL speak_text(DTK$K_IMMED, contract2$)
		CASE ELSE
		    CALL speak_text(DTK$K_IMMED, contract3$)
	    END SELECT	   
	    input_key_buf(0) = 0%
	    num_input_keys = 0%
	    stat% = get_key_string(null_char$, T20SECOND, terminator_value)
	    IF (stat% AND STS$M_SUCCESS) = 0%
	    THEN
		laboratory = 0%
		EXIT FUNCTION
	    END IF
	    IF terminator_value = DTK$K_TRM_ASTERISK
	    THEN
		laboratory = 1%
		EXIT FUNCTION
	    ELSE    IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
		    				input_key_buf(0) = 0%)
		    THEN
			CALL speak_all_text(msg_timeout)
			laboratory = 0%
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
		    CASE = DTK$K_TRM_ZERO		! Give user help
			illegal_entry_count = 0%

		    CASE = DTK$K_TRM_ONE		! Correct contract
			GO TO speak_results

		    CASE = DTK$K_TRM_TWO		! Incorrect patient
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
		SELECT call_count
		    CASE = 0%
			CALL speak_text(DTK$K_IMMED, contract1$)		
		    CASE = 1%
			CALL speak_text(DTK$K_IMMED, contract2$)
		    CASE ELSE
			CALL speak_text(DTK$K_IMMED, contract3$)
		END SELECT	   
		illegal_entry_count = 0%
	    END IF
	NEXT
	laboratory = 1%
	EXIT FUNCTION

speak_results:
	stat% = speak_lab_results(call_count)
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN	
		laboratory = 0%
		EXIT FUNCTION
	END IF
	last_result% = call_count
	!
	!  Now check if the caller wants to repeat these results, hear
	!  lab results for this patient greater then 24 hours old, hear
	!  radiology results for this patient, or hear lab results for
	!  another patient.
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
		laboratory = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		laboratory = 1%
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
				CALL speak_text(DTK$K_WAIT, lab_exit$)
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
		laboratory = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		laboratory = 1%
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
				laboratory = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   laboratory = 0%
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
		    CALL speak_text(DTK$K_WAIT, lab_exit$)
		    illegal_entry_count = 0%

		CASE =  DTK$K_TRM_ONE			! speak radiology results
		    stat% = speak_radiology_results(call_count)
		    IF (stat% AND STS$M_SUCCESS) = 0%
		    THEN
			laboratory = 0%
		        EXIT FUNCTION
		    END IF
		    illegal_entry_count = 0%

		CASE = DTK$K_TRM_TWO			! New patient id
		    IF call_count = 0%
		    THEN 
			call_count = 1%
		    ELSE IF call_count = 1%
			THEN 
			    call_count = 2%
			ELSE IF call_count = 2%
			    THEN 
				call_count = 0%
			    END IF
			END IF
		    END IF
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
		CALL speak_text(DTK$K_WAIT, lab_exit$)
		illegal_entry_count = 0%
	END IF
	RETURN
 END FUNCTION

 ! ----------------------- get_physician_code -------------------------

 FUNCTION LONG get_physician_code()

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

   physician_code$ = "Please enter your physician code, followed by the "&
	+ "pound-key.  You may enter any number as a test.  "

   DECLARE LONG term_code

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string(physician_code$, T20SECOND, term_code)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		get_physician_code = 0%
		EXIT FUNCTION
	END IF
	get_physician_code = 1%
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

   patient_id$ = "Please enter the patient's identification number, "&
               + "followed by the pound-key.  You may enter any number "&
	       + "as a test.  "

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


 ! ------------------------- speak_lab_results --------------------------
 !
 !  This section speaks the laboratory results to the caller specified
 !  by patient_no and returns.
 !
 FUNCTION LONG speak_lab_results(LONG patient_no)

 %INCLUDE "DTKDEF"

   lab1$ = "Electrolyte chemistry results for Mary Smith, drawn on "
   lab1_result$ = " at 8 am.  Accession number 2, 3, 4.  Chloride, 51, "&
	+ "low.  [s'iy `ow t'uw], 139, high.  Potassium, 4.4.  B.U.N, 58, "&
	+ "high.  Glucose, 198, high.  "

   lab2$ = "Blood-test results for Betty Blood, drawn on "
   lab2_result$ = "[hx'iymowglowbixn], 11.3.  [hxiym'aetaxkrixt], 43.8.  "&
	+ "Red blood-cell count, 4.95.  White blood-cell count, 29220. "&
	+ "[gr'aenyxaxlowsayts], 24%.  Lymph, 59%.  [pl'eytlixt] count, 354.  "&

   lab3$ = "Urinalysis results for John Doe, drawn on "
   lab3_result$ = " at 9:40 AM.  Accession number 5, 4, 6.  Color, "&
	+ "Yellow.  Clarity, cloudy.  Specific gravity, 1.027.  P.H, 5.5.  "&
	+ "Protein, 1+.  Glucose, 2+.  [k'iytownz], trace.  "&
	+ "Blood, negative.  [ehpixth'iyliyaxl] cells, negative.  "&
	+ "[d'ahbelyx`uw b`iy s'iyz prr 'eych p`iy 'ehf], negative.  "&
	+ "['aar b`iy s'iyz prr 'eych p`iy 'ehf], negative.  Casts, "&
	+ "negative.  Crystals, negative.  "

   no_lab_results$ = "There are no [l'aebraxtoriy] results for this patient.  "
    
     SELECT patient_no
	CASE = 0%
	    CALL speak_text(DTK$K_WAIT, lab1$)
	    CALL speak_text(DTK$K_WAIT, DATE$(0))
	    CALL speak_text(DTK$K_WAIT, lab1_result$)

	CASE = 1%
	    CALL speak_text(DTK$K_WAIT, lab2$)
	    CALL speak_text(DTK$K_WAIT, DATE$(0))
	    CALL speak_text(DTK$K_WAIT, lab2_result$)

	CASE = 2%
	    CALL speak_text(DTK$K_WAIT, lab3$)
	    CALL speak_text(DTK$K_WAIT, DATE$(0))
	    CALL speak_text(DTK$K_WAIT, lab3_result$)

	CASE = 3%
	    CALL speak_text(DTK$K_WAIT, no_lab_results$)

    END SELECT
    speak_lab_results = 1%
 END FUNCTION

! ----------------------------- radiology ------------------------------------

 FUNCTION LONG radiology()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				get_physician_code, &
				get_patient_id, &
				speak_lab_results, &
				speak_radiology_results, &
				DTK$SET_VOICE

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160


   contract1$ = "These [reydiy'aolaxjhiy] results are for Mary Smith.  "&
	+ "If this is correct, press 1, followed by the pound-key.  "&
	+ "If this is incorrect, press 2, followed by the pound-key.  "&
	+ "To exit, press the star-key.  "

   contract2$ = "These [reydiy'aolaxjhiy] results are for Betty Blood.  "&
	+ "If this is correct, press 1, followed by the pound-key.  "&
	+ "If this is incorrect, press 2, followed by the pound-key.  "&
	+ "To exit, press the star-key.  "

   contract3$ = "There are no [reydiy'aolaxjhiy] results for John Doe.  "

   lab_exit$ = "For  results for this patient, press 1, "&

   rad_exit$ = "For [l'aebraxtoriy] results for this patient, press 1, "&
	+ "followed by the pound-key.  For [reydiy'aolaxjhiy] results for "&
	+ "another patient, press 2, followed by the pound-key.  To exit, "&
	+ "press the star-key.  "

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
				 period_pause_duration

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  COMMON (results_reporting)    LONG    call_count

  DECLARE	LONG	terminator_value, illegal_entry_count

	!
	!  Get the caller's physician identification code and
	!  the id number of the patient whose lab results are
	!  desired and then speak the results to the caller.
	!
	stat% = get_physician_code
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		radiology = 0%
		EXIT FUNCTION
	END IF
new_patient:
	stat% = get_patient_id
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN
		radiology = 0%
		EXIT FUNCTION
	END IF
	WHILE (input_key_buf(0) <> 0% OR  &
				terminator_value <> DTK$K_TRM_TIMEOUT)

	    !
	    !  Tell the physician the patient's name for the patient id
	    !  number entered and allow him to verify it or to reenter.
            ! Keep getting commands from the user until the exit key "*",
	    ! is entered, a wink is detected, or the timeout period expires.
            ! First, prompt the user for a command and wait for his response.
	    !
	    SELECT call_count
		CASE = 0%
		    CALL speak_text(DTK$K_IMMED, contract1$)		
		CASE = 1%
		    CALL speak_text(DTK$K_IMMED, contract2$)
		CASE ELSE
		    CALL speak_text(DTK$K_IMMED, contract3$)
		    GO TO get_cmd
	    END SELECT	   
	    input_key_buf(0) = 0%
	    num_input_keys = 0%
	    stat% = get_key_string(null_char$, T20SECOND, terminator_value)
	    IF (stat% AND STS$M_SUCCESS) = 0%
	    THEN
		radiology = 0%
		EXIT FUNCTION
	    END IF
	    IF terminator_value = DTK$K_TRM_ASTERISK
	    THEN
		radiology = 1%
		EXIT FUNCTION
	    ELSE    IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
		    				input_key_buf(0) = 0%)
		    THEN
			CALL speak_all_text(msg_timeout)
			radiology = 0%
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
		    CASE = DTK$K_TRM_ZERO		! Give user help
			illegal_entry_count = 0%

		    CASE = DTK$K_TRM_ONE		! Correct contract
			GO TO speak_results

		    CASE = DTK$K_TRM_TWO		! Incorrect patient
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
		SELECT call_count
		    CASE = 0%
			CALL speak_text(DTK$K_IMMED, contract1$)		
		    CASE = 1%
			CALL speak_text(DTK$K_IMMED, contract2$)
		    CASE ELSE
			CALL speak_text(DTK$K_IMMED, contract3$)
		END SELECT	   
		illegal_entry_count = 0%
	    END IF
	NEXT
	radiology = 1%
	EXIT FUNCTION

speak_results:
	stat% = speak_radiology_results(call_count)
	if (stat% AND STS$M_SUCCESS) = 0%
	THEN	
		radiology = 0%
		EXIT FUNCTION
	END IF
get_cmd:
	!
	!  Now check if the caller wants to repeat these results, hear
	!  lab results for this patient greater then 24 hours old, hear
	!  radiology results for this patient, or hear lab results for
	!  another patient.
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
		radiology = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		radiology = 1%
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
				CALL speak_text(DTK$K_WAIT, rad_exit$)
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
		radiology = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		radiology = 1%
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
				radiology = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   radiology = 0%
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
		    CALL speak_text(DTK$K_WAIT, rad_exit$)
		    illegal_entry_count = 0%

		CASE =  DTK$K_TRM_ONE			! Speak lab results
		    stat% = speak_lab_results(call_count)
		    IF (stat% AND STS$M_SUCCESS) = 0%
		    THEN
			radiology = 0%
		        EXIT FUNCTION
		    END IF
		    illegal_entry_count = 0%

		CASE =  DTK$K_TRM_TWO			! New patient id
		    IF call_count = 0%
		    THEN 
			call_count = 1%
		    ELSE IF call_count = 1%
			THEN 
			    call_count = 2%
			ELSE IF call_count = 2%
			    THEN 
				call_count = 0%
			    END IF
			END IF
		    END IF
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
		CALL speak_text(DTK$K_WAIT, rad_exit$)
		illegal_entry_count = 0%
	END IF
	RETURN
 END FUNCTION



! -------------------- speak_radiology_results ----------------------------

 FUNCTION LONG speak_radiology_results (LONG patient_no)

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

   patient1$ = "There are three [reydiy'aolaxjhiy] results for Mary "&
	+ "Smith.  For P.A and lateral chest X-ray results, press 1, "&
	+ "followed by the pound-key.  For the cervical spine x-ray "&
	+ "results, press 2, followed by the pound-key.  For the "&
	+ "[ehsaofaxg'aoskaxpiy] results, press 3, followed by the "&
	+ "pound-key.  To exit, press the star-key.  "

   patient1_test1$ = "P.A and lateral chest X-ray results for "&
	+ "Mary Smith.  Aorta is somewhat [t'orchuwaxs].  Otherwise normal.  "

   patient1_test2$ = "Cervical spine x-ray results for Mary Smith.  "&
	+ "Degenerative changes in C3, C4, C5, C6, and C7, and T 1. "

   patient1_test3$ = "[ehsaofaxg'aoskaxpiy] results for Mary "&
	+ "Smith.  Mary ['ahndrrwehnt] [']upper [jh'iy 'ay] "&
	+ "[ehnd'aoskaxpiy] without complications.  [priymehdixk'eyshaxn] "&
	+ "was Fentanyl 0.15 mg. ['ay v'iy].  She had small, straight, "&
	+ "blue [v'erixsiyz] extending through the [d'ihstaxl] 20 "&
	+ "centimeters of her esophagus.  No ulcerations or evidence of "&
	+ "bleeding were apparent.  This represents a substantial reduction "&
	+ "in [v'erixsiyel] size since her previous [ehnd'aoskaxpiy] and "&
	+ "sclerosis.  In view of the patient's age and diabetes, with "&
	+ "chronic [kaxnjh'ehstixv] heart-failure, she will not undergo "&
	+ "further sclerosis unless bleeding [riyaxk'rrz].  "

   patient2$ = "There are three [reydiy'aolaxjhiy] results for Betty "&
	+ "Blood.  For CAT-Scan Head results, press 1, followed by "&
	+ "the pound-key.  For foot X-ray results, press 2, followed by "&
	+ "the pound-key.  For P.A and Lateral Chest X-ray results, press "&
	+ "3, followed by the pound-key.  To exit, press the star-key.  "

   patient2_test1$ = "CAT-Scan Head results, [']with and [']without "&
	+ "contrast, for Betty Blood.  Normal "&
	+ "[vehntr'ihkyxuwlrr] system, mid-line third ventricle and normal "&
	+ "abion cistern.  Good gray white differentiation.  No evidence of "&
	+ "any areas of increased or decreased attenuation.  No "&
	+ "['ihntraxsrr'iybraxl] or ['ehkstraxsrr'iybraxl] collections.  "&
	+ "Conclusion: Normal Cat-scan Head.  "

   patient2_test2$ = "Foot X-ray results for Betty Blood.  "&
	+ "Left foot series with two obliques, shows slight "&
	+ "hallux valgus deformity, with no other abnormalities noted.  "&
	+ "A bipartite [s'ehsaxmoyd] is visible.  This is unchanged from "&
	+ 'the film taken three ["]years ago.  Conclusion: slight hallux '&
	+ "valgus deformity, left foot.  "

   patient2_test3$ = "P.A and Lateral Chest X-ray results for Betty Blood.  "&
	+ "Impression: 1,  Minimal to moderate degenerative "&
	+ "changes involving the thoracic spine.  2, Calcified granuloma "&
	+ "indicative of previous infection of either "&
	+ "[hx'ihstowplaezm'owsixs] or [taxbrrkyxaxl'owsixs].  No evidence "&
	+ "of active disease at this time.  3, linear [aetaxl'ehktaxsixs] "&
	+ "left base.  4, left lower lobe ['ihnfixltreyt] versus "&
	+ "subsegmental [aetaxl'ehktaxsixs].  "

   no_patient$ = "There are no [reydiy'aolaxjhiy] results for John Doe.  "


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
		speak_radiology_results = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		speak_radiology_results = 1%
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
			    SELECT patient_no
				CASE = 0%
				    CALL speak_text(DTK$K_WAIT, patient1$)
				CASE = 1%
    				    CALL speak_text(DTK$K_WAIT, patient2$)
				CASE ELSE
				    CALL speak_text(DTK$K_WAIT, no_patient$)
				    speak_radiology_results = 1%
				    EXIT FUNCTION
			    END SELECT
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
		speak_radiology_results = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		speak_radiology_results = 1%
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
		                speak_radiology_results = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   speak_radiology_results = 0%
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
		    SELECT patient_no
			CASE = 0%
			    CALL speak_text(DTK$K_WAIT, patient1$)
			CASE = 1%
			    CALL speak_text(DTK$K_WAIT, patient2$)
			CASE ELSE
			    CALL speak_text(DTK$K_WAIT, no_patient$)
		    END SELECT
		    illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE			! Give test result
		    SELECT patient_no
			CASE = 0%
			    CALL speak_text(DTK$K_WAIT, patient1_test1$)
			CASE = 1%
			    CALL speak_text(DTK$K_WAIT, patient2_test1$)
			CASE ELSE
			    CALL speak_text(DTK$K_WAIT, no_patient$)
		    END SELECT
		    illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO			! Give test result
		    SELECT patient_no
			CASE = 0%
			    CALL speak_text(DTK$K_WAIT, patient1_test2$)
			CASE = 1%
			    CALL speak_text(DTK$K_WAIT, patient2_test2$)
			CASE ELSE
			    CALL speak_text(DTK$K_WAIT, no_patient$)
		    END SELECT
		    illegal_entry_count = 0%


		CASE = DTK$K_TRM_THREE			! Give test result
		    SELECT patient_no
			CASE = 0%
			    CALL speak_text(DTK$K_WAIT, patient1_test3$)
			CASE = 1%
			    CALL speak_text(DTK$K_WAIT, patient2_test3$)
			CASE ELSE
			    CALL speak_text(DTK$K_WAIT, no_patient$)
		    END SELECT
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
	    SELECT patient_no
		CASE = 0%
		    CALL speak_text(DTK$K_WAIT, patient1$)
		CASE = 1%
		    CALL speak_text(DTK$K_WAIT, patient2$)
		CASE ELSE
		    CALL speak_text(DTK$K_WAIT, no_patient$)
	    END SELECT
	    illegal_entry_count = 0%
	END IF
	RETURN
 END FUNCTION
