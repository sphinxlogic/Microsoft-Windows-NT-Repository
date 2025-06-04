	%TITLE "DECTALK-RESULTS.BAS"
 !
 !				RESULTS.BAS
 ! 
 !	NOTE:  This program was written with VAX BASIC V3.0.
 !
 !  The following file contains a version of the "modifiable"
 !  demonstration program that is included in the DECtalk Application
 !  Development Guide.  This version of the demonstration program
 !  shows how DECtalk can be used to report laboratory and radiology
 !  results.  The demonstration program typifies most telephone application
 !  programs written for DECtalk.  It is written in VAX BASIC and uses
 !  Version 4.5 or later of the VMS/MicroVMS Run-Time Library Support 
 !  provided for DECtalk.
 !
 !	Program description:  In the "MENU" subroutine there is a SELECT
 !		statement where the code determines which of 3 menu items
 !		are chosen.  The menu choices and menu message must be
 !		modified to reflect the menu items being implemented.
 !		The menu choices are:
 !
 !			A) Laboratory Results
 !			B) Radiology Results
 !			C) DECtalk Product Information
 !
 !		All error messages are logged only to the operator terminals
 !		that have specified that the "OPER11" type of message will
 !		be handled. These messages give the error message, and the
 !		terminal line connected to that process.  To have error
 !		messages logged to the console terminal, or any other terminal,
 !		at the DCL prompt ($), enter the REPLY/ENABLE command:
 !
 !			$ reply/enable=oper11
 !
 !		To enter this command, you must have the OPER user privelege.
 !

 %INCLUDE "DTKDEF"

 EXTERNAL LONG CONSTANT	STS$M_SUCCESS, &
			SS$_NORMAL,   &
			SS$_TIMEOUT
 !
 !  EXTERNAL Run-time Library Routines (DECtalk support)
 !
 EXTERNAL LONG FUNCTION &
			DTK$ANSWER_PHONE,	! Wait for phone to ring !&
			DTK$INITIALIZE, 	! Initialize DECtalk device !&
			DTK$TERMINATE, 		! End initialized DECtalk use!&
			initialize_dectalk
 !
 !  Prompts and demo text
 !
  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160, &
				dispatch_desk = 160

   msg_policyid$ = "Please enter your policy I.D. followed by the pound-key."&
		+ " You may enter any number as a test."		
   msg_security$ = "Please enter your security code followed by the pound-key."&
		+ " You may enter any number as a test."
   msg_invalid  = "Invalid entry.  Please try again. "
   msg_timeout  = "No key pressed in the timeout period specified."
   msg_bad_command  = "Invalid command.  Please try again. "
   msg_no_terminator= "Please remember to terminate your entry with the "&
		+ "pound-key.  Command accepted.  "
  customer_service = "In an actual application, DECtalk would now transfer "&
	+ "your call to a customer service representative. This demonstration "&
	+ "just returns to the selection menu. "

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND, &
					T30SECOND, T5MINUTE, T15MINUTE

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
        period_pause_duration, speech_on, new_mode, number_of_rings, keypad_mode

  COMMON (application_specific)	LONG		voice_id, &
						num_hold_keys, &
						num_input_keys, &
					BYTE	hold_key_buf(80) , &
						input_key_buf(80), &
					STRING	dictionary_file_name = 80, &
						terminal_line

  COMMON (results_reporting)    LONG		call_count

	MAX_ENTRY = 3
	T1SECOND = 1
	T2SECOND = 2
	T20SECOND = 20
	T30SECOND = 30
	T5MINUTE = 300
	T15MINUTE = 900
	TIME_45_SEC = "0 00:00:45"
	TIME_90_SEC = "0 00:01:30"
	TIME_6_MIN  = "0 00:06:00"
	TIME_16_MIN = "0 00:16:00"
	voice = DTK$K_VOICE_MALE
	speaking_rate = 180
	comma_pause_duration = 0
	period_pause_duration = 0
	speech_on = DTK$K_SPEAK
	new_mode = DTK$M_SQUARE
	keypad_mode = DTK$K_KEYPAD_AUTO
	call_count = 0%

   msg_welcome$  = "Welcome to Digital's [l'aebraxtoriy] Results Reporting "&
		 + "Demonstration.  "

 %page
 ! main

 DECLARE LONG	version, init_dtk, need_to_init
 init_dtk = 0%
 need_to_init = 0%
	!
	! Get the name of the physical device that the DECtalk module is
	! connected to and the name of the dictionary file (if specified)
	! that contains DECtalk's user dictionary words. If the call to 
	! DTK$INITIALIZE is unsuccessful, then exit.
	!
	CALL get_cli_params(terminal_line, dictionary_file_name)

	call set_timer(Time_45_sec)
	stat% = DTK$INITIALIZE(voice_id, terminal_line, version)
	call cancel_timer
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF

 WHILE_LOOP:
	WHILE 1% = 1%			! do always

	num_hold_keys = 0%
	num_input_keys = 0%

	 !  Check to see if the DECtalk needs to be initialized.
	 !  (init_dtk = need_to_init = 0).  If so, reinitialize the
 	 !  application specific parameters.  Then, set a watchdog timer 
	 !  for 16 minutes in the future in case the DECtalk module fails
	 !  and no response to the DTK$ANSWER_PHONE command is received
	 !  from DECtalk.  ** NOTE:  MAKE SURE THE WATCHDOG TIMER IS 
	 !  ALWAYS SET TO A VALUE LARGER THAN THE TIMEOUT PERIOD 
	 !  SPECIFIED IN THE DTK$ANSWER_PHONE COMMAND.  THE WATCHDOG TIMER
	 !  CAN BE LARGER THAN 16 MINUTES.  Next, wait
	 !  for the phone connected to the DECtalk to ring.  If the phone
	 !  does not ring in "timeout" seconds (or the watchdog timer 
	 !  times out), normally, the status of the DECtalk unit would be
	 !  checked.  This is not possible in the current version (V4.5)
	 !  of the VMS Run-Time Library Support for DECtalk.  Therefore,
	 !  instead of reinitializing DECtalk every time the unit power cycled,
	 !  it is reinitialized every time no phone calls are received in 
	 !  "timeout" seconds. Then, a greeting message is spoken to the user.
         !
	IF init_dtk = need_to_init
	THEN  stat% = initialize_dectalk		! Reinitialize DECtalk paramS
		IF (STAT% AND STS$M_SUCCESS) = 0%
		THEN
			!  Initialization failed.  It appears that
			!  the DECtalk module may be dead.  For now, 
			!  I will exit.

			GO TO out			! End the program
		END IF
	    init_dtk = NOT need_to_init			! Appl. init. complete
	END IF
	CALL set_timer(time_16_min)		! Set system (watchdog) timer
	!
	! The greeting message DECtalk speaks upon answering the telephone can
	! be modified by changing the text in the character string
	! "msg_welcome". The number of rings DECtalk waits to answer the
	! telephone ("number_of_rings"), can be changed but it is recommended
	! that the phone is always answered on the first ring.
	!
        stat% = DTK$ANSWER_PHONE(voice_id, number_of_rings, msg_welcome$, &
								T15MINUTE)
	CALL cancel_timer
	IF (stat% = SS$_NORMAL)
	THEN
		!
		!  The telephone has been answered.  
		!
		!  NOTE: DTK$ANSWER_PHONE automatically enables autostop keypad
		!	 mode, and wink detection on the telephone keypad.
		!
		CALL menu
		CALL end_call
	ELSE IF (stat% = SS$_TIMEOUT)			! Timeout period expired
	     THEN
		init_dtk = need_to_init			! Have to reinitialize
	     ELSE
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)		! Fatal error so exit
	     END IF
	END IF
    NEXT
 
out:
    !  some fatal error has occured... Terminate DECtalk and exit
    !
    CALL set_timer(Time_45_sec)
    stat% = DTK$TERMINATE(VOICE_ID)     ! End use of DECtalk device
    CALL cancel_timer
    IF (stat% AND STS$M_SUCCESS) = 0%
    THEN
		CALL error_log(stat%)
    END IF
 
 END

 !********************* END OF PROGRAM ***************************************
 !
 ! Gets the parameters from the command line using LIB$GET_FOREIGN.
 ! If a fatal error occurs, it is reported and the demo is stopped.
 ! Otherwise, the parameters specified are returned in terminal_line_string
 ! for the device name, and dictionary_name for the name of the dictionary file.
 !
 SUB  get_cli_params (STRING device_name, STRING dictionary_name)

	EXTERNAL LONG CONSTANT STS$M_SUCCESS
	EXTERNAL LONG FUNCTION LIB$GET_FOREIGN

	DECLARE STRING get_param_buf
	DECLARE LONG device_length, diction_name_length

	stat% = LIB$GET_FOREIGN(get_param_buf,,,)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)		! Fatal error so exit
	END IF
	device_length = POS(get_param_buf, " ", 1)
	diction_name_length = LEN(get_param_buf)

	IF device_length = 0%
	THEN
		device_name = SEG$(get_param_buf,1, diction_name_length)
	ELSE
		device_name = SEG$(get_param_buf, 1, device_length - 1)
		dictionary_name = SEG$(get_param_buf, device_length+1,&
						 diction_name_length)
	END IF
  END SUB


 !*************************************************************************
 !  Initializes the application specific parameters of DECtalk.
 !  First, left square bracket ('[') and right square bracket (']') 
 !  are enabled as phonemic delimiters. To specify other modes, the bit masks
 !  for the modes to be set should be OR'd together with the DTK$M_SQUARE bit
 !  mask and assigned to the varible "NEW_MODE".  Next, the default speaking 
 !  voice and speaking rate are selected for the application.  The comma
 !  pause and period pause are set to DECtalk defaults.  Other voices
 !  can selected for the default speaking voice by modifying the variable
 !  "voice".  Likewise, a different speaking rate can be specified
 !  by changing the static variable "speaking_rate".  Finally, the user
 !  dictionary is loaded (by invoking "load_dictionary").  Note, other
 !  application specific parameters should also be initialized in this
 !  subroutine if the need arises.
 !

 FUNCTION LONG initialize_dectalk()

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	DTK$SET_MODE, &
				DTK$SET_VOICE, &
				load_dictionary

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min

  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
		 period_pause_duration, speech_on, new_mode
  COMMON (application_specific)	LONG	voice_id

	CALL set_timer(Time_45_sec)
	stat% = DTK$SET_MODE(voice_id, new_mode, )
	CALL cancel_timer
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)		! Fatal error so exit
	END IF
	!
	!  Indicate the type of voice and the speaking rate for DECtalk.
	CALL set_timer(Time_45_sec)
	stat% = DTK$SET_VOICE(voice_id, voice, speaking_rate &
				,comma_pause_duration, period_pause_duration)
	CALL cancel_timer
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)		! Fatal error so exit
	END IF
	!
	! load user dictionary using "load_dictionary" function
	!
	initialize_dectalk = load_dictionary
	EXIT FUNCTION
 END FUNCTION

 !  -----------------------------------------------------------------------
 !  Gets a series of touch tone keys entered on the telephone keypad terminated
 !  by a pound sign (#) or an asterisk (*).  Returns SUCCESS if the series of
 !  touch tone keys was received successfully.  Otherwise, FAILURE is returned.

 FUNCTION LONG get_key_string(STRING prompt,LONG timeout,LONG terminator_code)

 %INCLUDE "DTKDEF"

  EXTERNAL LONG FUNCTION	DTK$READ_KEYSTROKE
  EXTERNAL LONG CONSTANT	SS$_TIMEOUT, &
				SS$_NORMAL, &
				DTK$_WINK

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND

   COMMON (application_specific)	LONG	voice_id, &
						num_hold_keys, &
						num_input_keys, &
					BYTE	hold_key_buf(80), &
						input_key_buf(80)

  DECLARE LONG  read_key_buf, hold_buf_ptr, ad_timeout, loc_timeout

	loc_timeout = timeout

	IF num_hold_keys > 0%
	THEN
	!
	!  There are Touch Tone keys in the typeahead buffer (hold_key_buf).
	!  The routine "read_advance_keys" is called here to read any new
	!  keys pressed.  This is used mainly to read and process winks.
	!  The "keys" stored in the typeahead buffer are moved into the input
	!  buffer "input_key_buf" up to any terminator character.  The
	!  terminator character is put in "terminator_value", and any remaining
	!  characters left in the typeahead buffer are copied to the front of 
	!  the buffer.
	!
		ad_timeout = 1%
		GOSUB read_advance_keys
		hold_key_buf(num_hold_keys) = 0%
		hold_buf_ptr = 0%
		WHILE hold_key_buf(hold_buf_ptr) <> DTK$K_TRM_NUMBER_SIGN AND &
		      hold_key_buf(hold_buf_ptr) <> DTK$K_TRM_ASTERISK

			IF hold_key_buf(hold_buf_ptr) = 0%
			THEN
				num_hold_keys = 0%
				GO TO read_key
			END IF
				input_key_buf(num_input_keys) = &
					hold_key_buf(hold_buf_ptr)

			num_input_keys = num_input_keys + 1%
			hold_buf_ptr = hold_buf_ptr + 1%
		NEXT
		terminator_code = hold_key_buf(hold_buf_ptr)
		hold_buf_ptr = hold_buf_ptr + 1%
		num_hold_keys = 0%
		!
		!  Recopy the remaining keys in the typeahead buffer to
		!  the beginning of the buffer.  If the terminating keycode was
		!  a "#" or "*", skip over it and start recopying with the next
		!  keycode.
	 
		UNTIL hold_key_buf(hold_buf_ptr) = 0%
			hold_key_buf(num_hold_keys)=hold_key_buf(hold_buf_ptr)
			num_hold_keys = num_hold_keys + 1%
			hold_buf_ptr  = hold_buf_ptr  + 1%
		NEXT
		GO TO process_entry
	END IF
	IF LEN(prompt) > 1%
	THEN
	!  this test assumes that no 1 character prompts will be sent to 
	!  dectalk.  Prompts of 1 char will be passed over here.
	!
		CALL speak_text(DTK$K_WAIT, prompt)
	END IF
 read_key:
	WHILE 1% = 1%
		!
		!  Read all keys entered on the touch tone keypad by the user
		!  and store them in the typeahead buffer "hold_key_buf". First
		!  a watchdog timer must be set before any keys can be read.
         
		CALL set_timer(Time_45_sec)
		stat% = DTK$READ_KEYSTROKE(voice_id, read_key_buf, , loc_timeout)
		CALL cancel_timer
		IF stat% = SS$_NORMAL
		THEN
			IF (read_key_buf = DTK$K_TRM_NUMBER_SIGN OR &
			    read_key_buf = DTK$K_TRM_ASTERISK)
			THEN
				!
				!  Received key string terminator ("#")
				!  so reception of keystring is completed. Use
				!  a short timeout to gather up any remaining
				!  touch tone keys entered by the user.

				terminator_code = read_key_buf
				ad_timeout = 2%
				GOSUB read_advance_keys
				GO TO process_entry
			ELSE
				input_key_buf(num_input_keys) = read_key_buf
				num_input_keys = num_input_keys + 1%
				loc_timeout = 10%
			END IF
		ELSE	IF stat% = SS$_TIMEOUT
			THEN
				EXIT read_key
			ELSE	IF stat% = DTK$_WINK
				THEN
				!
				!  DECtalk detected a wink which sometimes
				!  indicates that the user has hungup.  This
				!  demonstration program assumes that a WINK
				!  does indicate the a user has hungup. 
				!  Therefore, return so DECtalk can hangup its
				!  phone.  If for any reason it is noticed that
				!  spontaneous winks are occurring and the user
				!  at the other end of the telephone did not
				!  hangup, then DO NOT return. Just continue
				!  receiving touch tone keys.

				    CALL error_log(stat%)
				    get_key_string = 0%
 				    EXIT FUNCTION
				ELSE
				    CALL error_log(stat%)
				    CALL LIB$STOP(stat% BY VALUE)
				END IF
			END IF
		END IF
	NEXT
        !
        !  No keystring terminator ("#" or "*") entered so set the terminating
        !  keycode to DTK$K_TRM_TIMEOUT.  Otherwise, return all touch tone
	!  keys entered. Finally, recopy any remaining touch tone keys in the
	!  typeahead buffer (ttkeys) to the beginning of the buffer.
         
	terminator_code = DTK$K_TRM_TIMEOUT

 PROCESS_ENTRY:
	hold_key_buf(num_hold_keys) = 0%
	CALL restart
	get_key_string = 1%
	EXIT FUNCTION

 READ_ADVANCE_KEYS:
	!
	!  Read any keys from DECtalk into the typeahead buffer "hold_key_buf"
	!
	WHILE 1=1
		CALL set_timer(Time_45_sec)
		stat% = DTK$READ_KEYSTROKE(voice_id, read_key_buf, , ad_timeout)
		CALL cancel_timer
		IF stat% = SS$_NORMAL
		THEN
			hold_key_buf(num_hold_keys) = read_key_buf
			num_hold_keys = num_hold_keys + 1%
		ELSE	IF stat% = SS$_TIMEOUT
			THEN				! no key pressed: return
				RETURN
			ELSE	IF stat% = DTK$_WINK
				THEN			! Hung up?  exit
					CALL error_log(stat%)
					get_key_string = 0%
 					EXIT FUNCTION
				ELSE
					CALL error_log(stat%)
					CALL LIB$STOP(stat% BY VALUE)
				END IF
			END IF
		END IF
	NEXT

 END FUNCTION


 !***************************************************************************
 ! Verifies the access code received from the customer.  This routine always
 ! returns SUCCESS for the purpose of this demonstration.  In a real
 ! application, the code would be verified against access codes in the database.

 FUNCTION LONG access_verify ()

	access_verify = 1%
	EXIT FUNCTION  
 END FUNCTION

 !***************************************************************************
 ! Verifies the password received from the customer. This routine always
 ! returns SUCCESS for the purpose of this demonstration.  In a real
 ! application, the password would be verified against their password in the
 ! database.

 FUNCTION LONG password_verify ()

	password_verify = 1%
	EXIT FUNCTION
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
 SUB menu()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string, &
				laboratory, &
				radiology, &
				DTK$SET_VOICE

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

  main_menu$	= "For [l'aebraxtoriy] results, press 1, followed by "&
	+ "the pound-key.  For [reydiy'aolaxjhiy] results, press 2, "&
 	+ "followed by the pound-key.  For DECtalk product information, "&
	+ "press 3, followed by the pound-key.  To repeat this message, "&
	+ "press 0, followed by the pound-key.  To exit, press the star key.  "

  dtcfamily$	= "[+] The [`]DECtalk [`]Voice Response-System is the latest "&
	+ "addition to Digital's Voice solutions. It features the performance "&
	+ "of the MicroVAX 2, plus the flexibility of the foremost voice "&
	+ "synthesis product on the market today.  The [`]DECtalk [`]Voice "&
	+ "Response-System is sized to support from 2 to 32 DEctalk lines. "&
	+ "It is ideally suited to front end large data-bases. Packaged with "&
	+ "the [`]DECtalk [`]Voice Response-System is the DECtalk Application "&
	+ "Development-Guide.  This guide consists of a user's manual and "&
	+ "sample application-software. It is designed to significantly "&
	+ "reduce the time required to develop a voice response application "&
	+ "for any of the DEctalk products. DECtalk component products include"&
	+ " Dual-Line DECtalk, a two-line subsystem, and Multi-line DECtalk, "&
	+ "an eight-line subsystem, both of which are rack mountable. In "&
	+ "addition, the DECtalk board can be used to expand the number of "&
	+ "lines needed.  [+] Single line DECtalk is a table-top unit with a "&
	+ "built-in loud-speaker. It is best for [st'aend#axl`own] "&
	+ "applications, such as aids for the handicapped. It is also commonly"&
	+ " used in manufacturing, for [m'aonaxtaxrixnx] status on the factory"&
	+ " floor. [+] All DECtalk products appropriately pronounce commonly "&
	+ "occurring forms, such as numbers, money, abbreviations, dates, "&
	+ "times, and [paxnxkchuw'eyshaxn]. "

  
  DECLARE	LONG	terminator_value, illegal_entry_count
 	null_char$ = X'00'C
	terminator_value = 0%
	illegal_entry_count = 0%

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
		EXIT SUB
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		EXIT SUB
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
		EXIT SUB
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		EXIT SUB
	ELSE	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) <> 0%)
		THEN
			CALL speak_all_text(msg_no_terminator)
			GOSUB process_menu_entry	! Valid command so process it	

		ELSE 	IF (terminator_value = DTK$K_TRM_TIMEOUT AND &
				input_key_buf(0) = 0%)
			THEN
				CALL speak_all_text(msg_timeout)
				EXIT SUB
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
	EXIT SUB
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
			CALL speak_text(DTK$K_WAIT, main_menu$)
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_ONE			! lab results demo
			stat% =  laboratory
			if (stat% AND STS$M_SUCCESS) = 0%
			THEN
				EXIT SUB
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_TWO			! radiology demo
			stat% = radiology
			if (stat% AND STS$M_SUCCESS) = 0%
			THEN
				EXIT SUB
			END IF
			illegal_entry_count = 0%

		CASE = 	DTK$K_TRM_THREE			! dectalk product info
			CALL speak_text(DTK$K_WAIT, dtcfamily$)
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
		CALL speak_text(DTK$K_WAIT, main_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END SUB

 !***************************************************************************
 !  Load the user dictionary with the words and phonemic pronunciations
 !  stored in the sequential file specified in the foreign command line
 !  invoking the program.  Each line of this file contains the word to be
 !  defined in the user dictionary followed by a space, followed by the
 !  phonemic pronunciation of the word.  The entry is parsed to find the 
 !  start and finnish of the word and it's replacement.  The word and phonemic
 !  pronunciation are loaded into the user dictionary. If the load dictionary
 !  command fails, the user is notified and processing is terminated.  Returns
 !  SUCCESS if the dictionary is loaded successfully or if no user dictionary
 !  file name is specified.  Otherwise, FAILURE is returned.

 FUNCTION LONG load_dictionary()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG FUNCTION	DTK$LOAD_DICTIONARY
  EXTERNAL LONG CONSTANT	SS$_NORMAL, &
				DTK$_TOOLONG, DTK$_NOROOM
  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80), &
				STRING	dictionary_file_name = 80

	MAP (dictionary) STRING dictionary_entry = 256
	DECLARE STRING		diction_word,  phonetic
	DECLARE LONG		word_count, phonem_end

	sp$ = X'20'C
	tab$ = X'09'C
 	null_char$ = X'00'C
	ON ERROR GOTO ERROR_CHECK
	IF LEFT$(dictionary_file_name,1) = null_char$	! No dictionary filename
	THEN
	  !  No dictionary was specified in the command line.
	  !  Return to initialize_dectalk 
		load_dictionary = 1%
		exit function
	END IF

	OPEN dictionary_file_name AS FILE #1,	ORGANIZATION SEQUENTIAL, &
						ACCESS READ, MAP DICTIONARY
	!
	! Since there is no timeout associated with the DTK$LOAD_DICTIONARY 
	! command, it is possible for an application to hang waiting for a
	! response after issuing the DTK$LOAD_DICTIONARY command.  Therefore,
	! arm a watchdog timer (6 minutes in this case) to time the loading of
	! the entire user dictionary.  If all the entries are not loaded into 
	! the user dictionary before the timer expires, then assume that
	! something is wrong with the DECtalk module and exit.
	!
	CALL set_timer(time_6_min)		! Set system (watchdog) timer

     WHILE 1% = 1%
	GET #1
	! 
	!  Read in all of the words and substitutions from
	!  the sequential file specified in the command string.
	!
	word_count = POS(dictionary_entry, sp$, 1)
	IF word_count = 0%
	THEN		word_count = POS(dictionary_entry, tab$, 1)
	END IF
	phonem_end = POS(dictionary_entry, null_char$, 1)

	diction_word =	SEG$(dictionary_entry, 1, word_count-1)
	phonetic =	SEG$(dictionary_entry, word_count+1, phonem_end-1)

	stat% = DTK$LOAD_DICTIONARY(voice_id, DICTION_WORD, phonetic)
	IF (stat% = DTK$_TOOLONG OR stat% = DTK$_NOROOM)
	THEN
	     !
	     !  These are not generally fatal errors.  However, 
	     !  they will be treated as such.  In creating a demo
	     !  program, if words cannot be loaded into the user
	     !  dictionary, the programmer should be notified so that
	     !  the appropriate action can be taken.  By treating these
	     !  errors as fatal, the programmer will at least know the
	     !  point in the program where the error occurred.
	     !
		CLOSE #1
		CALL error_log(stat%)
		load_dictionary = 0%
		EXIT FUNCTION			! Fatal error so exit
	ELSE IF (stat% <> SS$_NORMAL) 	! Fatal failure
	     THEN
			CALL cancel_timer		! Cancel watchdog timer
			CLOSE #1
			CALL error_log(stat%)
			CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	     END IF
	END IF
     NEXT
 	!
	!  Entire dictionary is loaded so cancel system timer.
	!
 DONE:
	CALL cancel_timer			! Cancel system(watchdog) timer
	CLOSE #1				! Close dictionary file
	load_dictionary = 1%
 EXIT FUNCTION

 ERROR_CHECK:
	IF ERR = 11%		! IF End of File is found
	THEN
		RESUME DONE
	ELSE
	   CALL error_log(VMSSTATUS)
	   RESUME HERE
HERE:
	   load_dictionary = 0%
	   EXIT FUNCTION
	END IF
 END FUNCTION

 !***************************************************************************
 !  DECtalk stopped speaking because it was in autostop keypad mode when
 !  it received a Touch Tone Key from the user.  First, send DECtalk a 
 !  right square bracket "]" just in case speech was stopped while speaking
 !  phonemic text. Then, restart speech (using DTK$SET_SPEECH_MODE) and reset
 !  the speaking voice and rate (using DTK$SET_VOICE).
 !
 SUB restart()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	DTK$SET_SPEECH_MODE, &
				DTK$SET_VOICE

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min
  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
		 period_pause_duration, speech_on
  COMMON (application_specific)	LONG	voice_id

   rsbracket$    = "]"

  DECLARE LONG old_mode				! Current mode before reset

	CALL speak_text( DTK$K_WAIT, rsbracket$)
	!
	!  Set speakig on
	!
	CALL set_timer(Time_45_sec)
	stat% = DTK$SET_SPEECH_MODE(voice_id, speech_on, old_mode)
	CALL cancel_timer
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF
	!
	!  Reset the voice and speaking rate
	CALL set_timer(Time_45_sec)
	stat% = DTK$SET_VOICE(voice_id, voice, speaking_rate, &
				comma_pause_duration, period_pause_duration)
	CALL cancel_timer
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF

 END SUB

 !***************************************************************************
 !  End the current user session.  Since the DTK$HANGUP_PHONE command
 !  does not set a timeout, and it requests DECtalk to send a phone
 !  status, a watchdog timer is set to insure that the application does
 !  not hang (if DECtalk fails).  If a longer timeout period is needed,
 !  adjust the value of the parameter moved into SET_TIMER_STRING before
 !  performing "set_timer".  After the watchdog timer is set, speak a goodbye
 !  message to the caller and then hangup the phone.  The goodbye message
 !  spoken can be changed by modifying the text in "msg_goodbye". 
 !
 SUB end_call ()

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS, &
				SS$_TIMEOUT, &
				DTK$_ONHOOK

  EXTERNAL LONG FUNCTION 	DTK$HANGUP_PHONE, &
				DTK$READ_KEYSTROKE, &
				DTK$SPEAK_TEXT

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min
  COMMON (application_specific)	LONG	voice_id
  COMMON (results_reporting)    LONG	call_count

  msg_goodbye$  = "Thank you for calling Digital's [l'aebraxtoriy] Results "&
	+ "Reporting Demonstration.  For more information, please call "&
	+ "1, 800, 8 3 2, 6 2 7 7. "

	IF call_count = 0%
	THEN 
		call_count = 1%
	ELSE IF call_count = 1%
	    THEN 
		call_count = 0%
	    END IF
	END IF
	CALL set_timer(time_6_min)		! Set system (watchdog) timer
	!
	!  Say goodbye and hangup the phone
	!
	stat% = DTK$HANGUP_PHONE(voice_id, msg_goodbye$)
	CALL cancel_timer			! Cancel system timer
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF
	!
	! Get rid of any winks that may have been generated by the caller
	! hanging up in the middle of the goodbye message.  This works
	! around clears the internal WINK flag.
	!
        timeout% = 1%
	WHILE 1% = 1%
	    CALL set_timer(Time_45_sec)
	    stat% = DTK$READ_KEYSTROKE(voice_id, xx%, , timeout%)
	    CALL cancel_timer
	    IF (stat% = DTK$_ONHOOK OR stat% = SS$_TIMEOUT)
	    THEN
		!
	  	!  Clear remaining winks that may have been
		!  detected before DECtalk hung up the telephone.
		!
	        CALL set_timer(Time_45_sec)
	        stat% = DTK$READ_KEYSTROKE(voice_id, xx%, , timeout%)
	        CALL cancel_timer
		IF (stat% AND STS$M_SUCCESS) = 0%
	        THEN
			CALL error_log(stat%)
			CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	        END IF
	        EXIT SUB
	    ELSE IF (stat% AND STS$M_SUCCESS) = 0%
	         THEN
			CALL error_log(stat%)
			CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	         END IF
	    END IF
	NEXT
 END SUB

 !  ----------------------------------------------------------------------
 !  Speak all text -- disable autostop keypad, speak text in prompt using 
 !  "speak_text". If autostop was set, re-enable autostop.
 !
 SUB speak_all_text (STRING prompt)

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	DTK$SET_KEYPAD_MODE

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min
  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
        period_pause_duration, speech_on, new_mode, number_of_rings, keypad_mode
  COMMON (application_specific)	LONG	voice_id

  DECLARE LONG	keypad_on

  keypad_on = DTK$K_KEYPAD_ON
	!
	!  If autostop keypad mode is enabled (keymode = DTK$K_KEYPAD_AUTO), 
	!  then enable the keypad without autostop mode.
	!
	IF (keypad_mode = DTK$K_KEYPAD_AUTO)	! Keypad is in autostop mode	
	THEN
		CALL set_timer(Time_45_sec)	! Set the watchdog timer	
		stat% = DTK$SET_KEYPAD_MODE(voice_id, keypad_on)
		CALL cancel_timer		! Cancel the watchdog timer	
		IF (stat% AND STS$M_SUCCESS) = 0%
		THEN
			CALL error_log(stat%)
			CALL LIB$STOP(stat% BY VALUE)
		END IF
	END IF

	!  Send text to DECtalk to be spoken.  
	!  Specify the mode as DTK$K_WAIT so that text is completely
	!  spoken before the keypad is re-enabled in autostop mode.

	CALL speak_text(DTK$K_WAIT, prompt)

	!  If autostop keypad mode was enabled
	!  (keypad_mode = DTK$K_KEYPAD_AUTO), then re-enable it.
	!
	IF (keypad_mode = DTK$K_KEYPAD_AUTO) ! Keypad was in autostop mode	
	THEN
		CALL set_timer(Time_45_sec)	! Set watchdog timer		
		stat% = DTK$SET_KEYPAD_MODE(voice_id, keypad_mode)
		CALL cancel_timer		! Cancel the watchdog timer	
		IF (stat% AND STS$M_SUCCESS) = 0%
		THEN
			CALL error_log(stat%)
			CALL LIB$STOP(stat% BY VALUE)
		END IF
	END IF
 END SUB

 !***************************************************************************
 !  Sends the prompt (specified by prompt) to 
 !  the DECtalk to be spoken.  Returns SUCCESS if
 !  everything is spoken o.k.  Otherwise, FAILURE is returned.
 !  
 !  NOTE:  If DTK$SPEAK_TEXT is called with mode set to
 !	   DTK$K_WAIT or DTK$K_STATUS, a phone status request
 !	   is sent from DECtalk.  Since there is no timeout associated
 !	   with this request, a system timer should be set (to some
 !	   exteremely long value (16 minutes) to insure that the application
 !	   does not hang.
 !
 SUB speak_text (LONG return_status_mode, STRING prompt)

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT	STS$M_SUCCESS, &
				SS$_NORMAL
  EXTERNAL LONG FUNCTION	DTK$SPEAK_TEXT

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min

  COMMON (application_specific)	LONG	voice_id
	!
	!  Speak the text
	CALL set_timer(time_16_min)	! Set system (watchdog) timer
	stat% = DTK$SPEAK_TEXT(voice_id, prompt, return_status_mode)
	CALL cancel_timer		! Cancel system timer
	IF (stat% <> SS$_NORMAL)
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF
 END SUB

 ! **********************************************************************
 !	Log error to the operator console  "OPER11".
 !
 !  Take the error number, input to SYS$GETMSG to get the system message text.
 !  Format the message text with the specific terminal line for that application
 !  "process" (using SYS$FAO), and call SYS$SENOPR with the error structure.
 !
 !  NOTE:  the DTK$ errors that occur will have the %DTK- facility name,
 !	  but not the error message text, just the error number.  It is
 !	  anticipated that these messages will be included in future VMS/RTL
 !	  releases.
 !
 SUB error_log(LONG error_number)

 %INCLUDE "$OPCDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"

  EXTERNAL LONG CONSTANT	SS$_NORMAL
  EXTERNAL LONG FUNCTION	SYS$FAO, &
				SYS$SNDOPR, &
				SYS$GETMSG

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80) , &
					input_key_buf(80), &
				STRING	dictionary_file_name = 80, &
					terminal_line

  MAP	(error_struct) LONG opc$type_target, LONG OPC$L_MS_RQSTID, &
			STRING error_text = 120
  MAP	(error_struct) STRING error_msg = 128
  MAP   (temp)  STRING error_buf = 80

  DECLARE STRING CONSTANT filler = "  ON  "
  DECLARE STRING CONSTANT control_str = "!AD!AS!AD"
  DECLARE LONG buf_len

     temp% = SYS$GETMSG(error_number by value, buf_len, error_buf, 15% by value,)
 !
 ! Set message target to OPER11, and the message type to RQ_RQST.
 ! For more information on sending messages to an operators terminal
 ! see the System Services manual.
 !
    opc$type_target = (OPC$M_NM_OPER11 * 256%) OR OPC$_RQ_RQST

 	temp% = SYS$FAO(control_str, buf_len, error_text,		&
 				buf_len by value, error_buf by ref,	&
 				filler, 5% by value, terminal_line by ref)
 	WHILE MID$(error_text, buf_len + 1, 1) <> X"00"C
 		MID$(error_text, buf_len + 1, 1) = X"00"C
 		buf_len = buf_len + 1
 	NEXT
 	stat% = SYS$SNDOPR(error_msg, 0% by value)
  	IF (stat% <> SS$_NORMAL)		! Send to oper failed	
 	THEN
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
 	END IF 
 END SUB

 !***************************************************************************
 !  Sets the system (watchdog) timer to expire "sec" seconds in the future
 !  by invoking the "SYS$SETIMR" system service.  SYS$BINTIM is passed a 
 !  character string in the format "D HH:MM:SS", where D is Days, H is Hours
 !  M is minutes etc.  The output of BINTIM is the quadword delta_time ,
 !  which gets passed as input to SYS$SETIMR.  
 !
 SUB set_timer(STRING time_string)

	EXTERNAL LONG CONSTANT	STS$M_SUCCESS, &
				SS$_TIMEOUT
	EXTERNAL LONG FUNCTION	SYS$SETIMR, &
				SYS$BINTIM
	EXTERNAL LONG		LIB$SIGNAL 	! Indicate exception condition
	MAP (quadword) LONG delta_time, FILL

	stat% = SYS$BINTIM(time_string, delta_time)
	IF (stat% AND STS$M_SUCCESS)  = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF

	stat%= SYS$SETIMR( ,delta_time, LIB$SIGNAL BY VALUE, SS$_TIMEOUT BY VALUE)
	IF (stat% AND STS$M_SUCCESS)  = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF
 END SUB

 !  ----------------------------------------------------------------------
 !  Cancel the system watchdog timer
 !
 SUB cancel_timer()

 EXTERNAL LONG CONSTANT	STS$M_SUCCESS
 EXTERNAL LONG FUNCTION	SYS$CANTIM		! Cancel SYS$SETIMR request

	stat% = SYS$CANTIM(SS$_TIMEOUT BY VALUE, 0% BY VALUE)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF
 END SUB
