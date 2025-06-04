	%TITLE "DECTALK-DEMO.BAS  --  BASIC APPLICATION GUIDELINE PROGRAM"
 !
 !			  BASIC_DTDEMO.BAS
 ! 
 !	NOTE:  This program was written with VAX BASIC V3.0.
 !
 !  The following file contains a "modifiable" demonstration program 
 !  to be included in the DECtalk Application Development Guide.  This
 !  demonstration program typifies most telephone application programs
 !  written for DECtalk.  It is written in VAX BASIC and uses Version 4.5
 !  or later of the VMS/MicroVMS Run-Time Library Support provided for 
 !  DECtalk.
 !
 ! Description:	This program provides a framework to develop other
 !		demonstration programs.  Currently, it provides an
 !		information services demonstration giving the Boston
 !		weather forecast, transportation information, ski
 !		conditions, and mortgage rate information in a single
 !		menu system.  The program provides dial-in access only.
 !		When a user calls in, DECtalk answers the phone and speaks a
 !		short greeting message.  Then, the customer is asked to enter
 !		his access code and password.  The customer is given 
 !		three attempts to enter his access code and three 
 !		attempts to enter his password.  Once access to the 
 !		system has been gained, DECtalk prompts the user to
 !		enter a command.  The six valid commands are given below:
 !
 !		   Key 1 for the Boston weather forecast.
 !		   Key 2 for transportation information.
 !		   Key 3 for mortgage rate information.
 !		   Key 4 for the current ski conditions.
 !		   Key 0 for help.
 !		   Key * to exit.
 ! 
 !	NOTE:	the user's access code, password and all commands
 !		except exit must be terminated by the pound sign key,
 !		although the application will accept input without the
 !		pound sign key after a timeout.  When a valid command is 
 !		entered, DECtalk speaks the appropriate message to the user.
 !		Once the exit key is entered, a wink is detected, or a user
 !		fails to enter a command in the specified time period, the 
 !		user session is ended.  DECtalk speaks a goodbye message to
 !		the user and hangs up the telephone.  Then, DECtalk is
 !		re-enabled for autoanswer and waits for a new telephone call.
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
 !  VMS VERSION 4.5 RUN-TIME LIBRARY RESTRICTIONS FOR DECtalk SUPPORT:
 !
 !	There is a problem with the DTKDEF module in STARLET.OLB.
 !	The definitions of the touch tone key constants DTK$K_TRM_ZERO thru
 !	DTK$K_TRM_NINE should be set to the ASCII valies of the digits.
 !	The correct definitions are currently found in "dtkdef.bas".  Note,
 !	it is anticipated that this will be fixed in the Version 4.6 release
 !	of VMS.  Consult the release notes for Version 4.6.
 !
 !	The current version of the RTL does not provide a command to check
 !	the status of DECtalk.  With this command, an application program 
 !	can determine whether or not the DECtalk module has power cycled 
 !	since the last time its status has been observed.  If it is detected
 !	that the DECtalk module has power cycled, the application specific 
 !	parameters (speaking voice, speaking rate, words loaded into the user 
 !	loadable dictionary) should be reinitialized.  By periodically checking 
 !	the status of the DECtalk module, and reinitializing if the unit
 !	has power cycled, an application may not have to be terminated
 !	to replace failed DECtalk modules.  An alternative solution (used in
 !	this demonstration program) re-initializes application specific 
 !	parameters every time a phone call has not been received in 15 minutes.
 !
 !	Most of the DTK$ RTL functions that read and return a status condition
 !	from the DECtalk do not have a timeout specified on their read from 
 !	DECtalk.  These DTK$ RTL functions include DTK$HANGUP_PHONE,
 !	DTK$LOAD_DICTIONARY, DTK$RETURN_LAST_INDEX, DTK$SET_KEYPAD_MODE, 
 !	DTK$SPEAK_FILE, DTK$SPEAK_PHONEMIC_TEXT, DTK$SPEAK_TEXT.  Without a
 !	timeout, it is possible for the application program to hang if the 
 !	DECtalk module fails, the power cord is disconnected or the RS232 cable
 !	is disconnected. To prevent the application program from hanging
 !	without notifying the operator of the problem, a system timer 
 !	(using SYS$SETIMR) is set before ALL calls made to the DTK$ facility
 !	of the Run-Time Library.  This is done by performing the routine
 !	SET_TIMER.  If a response is received from the DECtalk within the time
 !	period specified, the system timer is canceled (using SYS$CANTIM) by
 !	performing the routine CANCEL_TIMER. Otherwise, if the timer expires,
 !	the timeout value 556 will be returned. To correct any error that may
 !	occur in the communication between the DECtalk module and the physical
 !	device, terminate the current job, correct the error, and then restart
 !	the job.  Note, the application program will hang until the problem
 !	has been corrected.
 !
 !	The current version of DTK$READ_STRING does not work correctly.  If
 !	a series of touch tone keys is entered on the touch tone keypad and
 !	a terminating character (number sign key or asterisk) is not entered,
 !	the series of touch tone keys entered is returned after the timeout
 !	period specified along with a terminator code of DTK$K_TRM_TIMEOUT.
 !	On the next call to DTK$READ_STRING, when the series of touch tone
 !	keys is returned, it always includes the last touch tone key returned
 !	in the previous call to DTK$READ_STRING as the first touch tone key in
 !	the series of keys returned.  The routine "GET_KEY_STRING"
 !	in this demonstration program can be used to read a series of touch
 !	tone keys terminated by the number sign key or the asterisk key.  It
 !	is similar in functionality to the DTK$READ_STRING routine.
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
			initialize_dectalk, &
			verify_user
 !
 !  Prompts and demo text
 !
  COMMON (MESSAGE_TEXT) STRING  rsbracket = 1, &
				msg_welcome  = 60,&
				msg_access   = 100, &
				msg_password = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_noaccess = 100, &
				msg_goodbye  = 100, &
				msg_timeout  = 50, &
				menu_prompt  = 100, &
				msg_help     = 320, &
				msg_weather  = 475, &
				msg_MBTA     = 365, &
				msg_mortgage = 500, &
				msg_ski      = 770

   rsbracket    = "]"
   msg_welcome  = "Welcome to the DECtalk information services demonstration."
   msg_access   = "Please enter your access code followed by the pound-key." &
		+ " You may enter any number as a test."
   msg_password = "Please enter your password followed by the pound-key."&
		+ " You may enter any number as a test."
   msg_invalid  = "Invalid entry.  Please try again. "
   msg_noaccess = "Access denied.  Please check your access code and"&
		+ " password and try again."
   msg_goodbye  = "Thank you for calling the DECtalk application"&
		+ " demonstration program.  Have a nice day."
   msg_timeout  = "No key pressed in the timeout period specified."
   msg_bad_command  = "Invalid command.  Please try again. "
   msg_no_terminator= "Please remember to terminate your entry with the"&
		+ "pound-key.  Command accepted.  "

   menu_prompt  = "Please enter a command.  For help, press 0 followed by "&
		+ "the pound-key. To exit, press the star-key."

   msg_help     = "To hear the current Boston weather forecast, Press 1. "&
		+ "To hear transportation information, Press 2. "&
		+ "To hear mortgage rate information, Press 3. "&
		+ "To hear the current ski conditions, Press 4. "&
		+ "To repeat this message, Press 0. Terminate your "&
		+ "entry with the pound-key.  To exit, Press the star-key. "

   msg_weather  = "Welcome to the Boston Area Weather Service. April 1st. "&
		+ 'Today will be a day ["]more like early May.  Current'&
		+ " downtown Boston temperature is 58 degrees fahrenheit, 14 "&
		+ 'Celsius. It will be a ["]beautiful sunny day, breezy and '&
		+ "mild, with a high of 76 degrees Fahrenheit. Humidity will "&
		+ "be 76%.  Barometric pressure is currently 32.5.  Tomorrow "&
		+ "will be cooler, with a high of fifty on the coast and "&
		+ "sixty inland.  Fair weather is expected to continue "&
		+ "throughout the week. "

   msg_MBTA     = "Welcome to the MBTA Information Line.  All MBTA lines are"&
		+ " on time.  The B and M line from the North Shore is "&
		+ "running a limited service, with trains running from "&
		+ "Ipswich and Rockport, at 8 A.M. and 9 A.M. only.  There "&
		+ "will be no red line service between Park Street and "&
		+ "Harvard, Monday from 9 P.M. to 1 A.M..  Substitute "&
		+ "bus transportation will be provided. "

   msg_mortgage = "Welcome to Hamden National Bank's Mortgage Line.  All of "&
		+ "the following rates are subject to change.  The application"&
		+ " fee is $250. 30 year rate with 10% [']down are 9.9% "&
		+ "with 3 [aen ax] half points, 10.2% with 3 points, 10.5%"&
		+ " with 2 [aen ax] half points, 10.8% with no points.  15 "&
		+ "year rates with 10% [']down are 9.9% with 2 [aen ax] "&
		+ "half points, 10.1% with 2 points, and 10.5% with no "&
		+ "points. Adjustable rates are at 8.5% fixed for 3 years "&
		+ "with 2% a year and 6% lifetime caps thereafter."

   msg_ski      = "Welcome to the New England Ski Report. January 31st. Skiing"&
		+ " is excellent in Vermont because of yesterday's snowfall "&
		+ "of 10 inches. All ski trails are open at Stowe, Sugar-"&
		+ "bush, Mt. Snow, Stratton, Jay Peak, and Smuggler's Notch, "&
		+ "with packed powder and full snow-making in operation. All"&
		+ " cross country ski trails are also open, with many groom"&
		+ "ed trails.  Skiing in Maine and [nuw] Hampshire is good, "&
		+ "with cold temperatures allowing for constant snow-making. "&
		+ "Gun-stock has 15 [aatax] 25 trails open, Mt. Cranmore has"&
		+ " 15 [aatax] 20 trails open, Attitash and Wildcat have all"&
		+ " trails open.  Sugar-loaf and Sunday River also have all "&
		+ "trails open, with packed powder and loose granular. "&
		+ "Expected snowfall in Maine and [nuw] Hampshire should "&
		+ "improve weekend ski conditions."

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
        stat% = DTK$ANSWER_PHONE(voice_id, number_of_rings, msg_welcome, &
								T15MINUTE)
	CALL cancel_timer
	IF (stat% = SS$_NORMAL)
	THEN
		!
		!  The telephone has been answered. Then verify that the caller
		!  is a valid user of the system.  If the caller fails to enter
		!  a valid access code and password in three attempts, access to
		!  system is denied.  If the user is successful, prompt him for
		!  a command.
		!
		!  NOTE: DTK$ANSWER_PHONE automatically enables autostop keypad
		!	 mode, and wink detection on the telephone keypad.
		!
		IF (stat% = verify_user) = 0%	! Deny access and hangup
		THEN
		   CALL speak_text(DTK$K_WAIT, msg_noaccess)
		   CALL end_call	  
		ELSE			
			!
			! The user has successfully gained access to the system.
			! Start processing commands from the user.  Note, the 
			! menu prompt spoken prior to receiving commands from
			! the user, can be modified by changing the text in the
			! character string "menu_prompt"
			!
			CALL menu
			CALL end_call
		END IF
	ELSE IF (stat% = SS$_TIMEOUT)			! Timeout period expired
	     THEN
		init_dtk = need_to_init			! Have to reinitialize
	     ELSE
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
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
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
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
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
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
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF
	!
	! load user dictionary using "load_dictionary" function
	!
	initialize_dectalk = load_dictionary
	EXIT FUNCTION
 END FUNCTION

 !  -----------------------------------------------------------------------
 !
 !  Verifies that the caller is a valid user of the system.
 !  The caller is given three attempts to enter a valid access code and
 !  three attempts to enter his password.  If the caller fails to enter
 !  a valid access code or a valid password in the timeout period specified,
 !  FALSE is returned.  Otherwise, TRUE is returned.
 !
 !  NOTE: In this demonstration, almost all access codes and passwords
 !	 are detected as valid.  The only ways an access code or a 
 !	 password are rejected are if the user fails to enter an access
 !	 code or a password in the time period specified, or if the user
 !	 terminates his access code or password with the star key ("*")
 !	 rather than the pound key ("#").  All valid access codes and passwords
 !	 (terminated with the pound key) are verified by performing the dummy
 !	 verification routines "access_verify" and "password_verify".
 !	 Both of these routines always return TRUE.

 FUNCTION LONG verify_user()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS

  COMMON (MESSAGE_TEXT) STRING  rsbracket = 1, &
				msg_welcome  = 60,&
				msg_access   = 100, &
				msg_password = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_noaccess = 100

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG   MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND, T30SECOND

  EXTERNAL LONG FUNCTION	get_key_string, &
				access_verify, &
				password_verify

  DECLARE LONG illegal_entry,			! Number of illegal entries! &
		term_code			! Key string terminator code

	illegal_entry = 0%			! No bad attempts yet
 check_user_loop:
    WHILE illegal_entry < MAX_ENTRY
	num_input_keys = 0%
	!
	! Gets the user's access code entered on the touch tone keypad.  The
	! entered access code is returned in the character buffer "access_code".
	! Currently, the maximum size of the buffer is 80 characters. The user
	! is prompted for his access code by the text specified in "msg_access".
	! To have a different prompt spoken, the character string "msg_access"
	! should be modified. Currently, the application waits 30 seconds for
	! a touch tone key to be entered.  If a longer or shorter timeout
	! period is desired, the new timeout value (in seconds) should be moved
	! into timeout before the call to "get_key_string". The parameter
	! "terminator_value" will contain the character used to terminate the
	! key string or a timeout upon return from the "get_key_string"
	! routine.     

	stat% = get_key_string( msg_access, T30SECOND,term_code)
	IF (stat% AND STS$M_SUCCESS) <> 0%
	THEN
	    stat% = access_verify
	    IF (term_code=DTK$K_TRM_ASTERISK OR term_code=DTK$K_TRM_TIMEOUT &
			                  OR (stat% AND STS$M_SUCCESS) = 0%)
	    THEN
		!		
		!  Either an invalid key string termination character was
		!  entered, the key string was not entered in the timeout
	  	!  period specified or an invalid access code was entered.

		illegal_entry = illegal_entry + 1%
		IF illegal_entry >= MAX_ENTRY
		THEN
			verify_user = 0%		! failure
			EXIT FUNCTION			! 3 strikes you're out
		ELSE
		    CALL speak_text(DTK$K_WAIT, msg_invalid)
		END IF
	    ELSE
		EXIT check_user_loop		! Access code is valid
	ELSE
		verify_user = 0%		! failure
		EXIT FUNCTION			! No key string received
	END IF
    NEXT
	! Gets the user's password entered on the touch tone keypad. The entered
	! password is returned in the character buffer "password_code".
	! Currently, the maximum size of the buffer is 80 characters.  The user
	! is prompted for his password by the text specified in "msg_password".
	! To have a different prompt spoken, the character string 
	! "msg_password" should be modified. Currently, the application waits
	! 30 seconds for a touch tone key to be entered.  If a longer or
	! shorter timeout period is desired, the new timeout value (in seconds)
	! should be moved into TIMEOUT before "get_key_string". The parameter,
	! "terminator_value" will contain the character used to terminate the
	! key string or timeout upon return from the "get_key_string" routine.
	!
	illegal_entry = 0%			! No bad attempts yet
 check_password:
    WHILE illegal_entry < MAX_ENTRY
	num_input_keys = 0%
	stat% = get_key_string(msg_password,T30SECOND,term_code)
	IF (stat% AND STS$M_SUCCESS) <> 0%
	THEN
	    stat% = password_verify
	    IF (term_code = DTK$K_TRM_ASTERISK OR term_code= DTK$K_TRM_TIMEOUT &
			                  OR (stat% AND STS$M_SUCCESS) = 0%)
	    THEN
		!
		! If an invalid password code was entered, increment the count
		! of invalid entry attempts.  If this count is greater than the
		! maximum allowed (specified by MAX_ENTRY), return FALSE. 
		! Otherwise, notify the user and prompt him again. If a valid
		! password is entered, then return TRUE.
		!
		illegal_entry = illegal_entry + 1%
		IF illegal_entry >= MAX_ENTRY
		THEN
			verify_user = 0%
			EXIT FUNCTION		! three strikes you're out
		ELSE
		    CALL speak_text(DTK$K_WAIT, msg_invalid)
		END IF
  	    ELSE 
		EXIT check_password		! PASSWORD IS VALID
	ELSE
		verify_user = 0%
		EXIT FUNCTION				! No key string received
	END IF
    NEXT
    verify_user = 1%					! success
    EXIT FUNCTION
 END FUNCTION

 ! -----------------------------------------------------------------------
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
				    CALL LIB$STOP(stat% BY VALUE)  ! stop
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
					CALL LIB$STOP(stat% BY VALUE)	! Stop
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
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (MESSAGE_TEXT) STRING  rsbracket = 1, &
				msg_welcome  = 60,&
				msg_access   = 100, &
				msg_password = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_noaccess = 100, &
				msg_goodbye  = 100, &
				msg_timeout  = 50, &
				menu_prompt  = 100, &
				msg_help     = 320, &
				msg_weather  = 475, &
				msg_MBTA     = 365, &
				msg_mortgage = 500, &
				msg_ski      = 770

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  DECLARE	LONG	terminator_value, illegal_entry_count
 	null_char$ = X'00'C
	terminator_value = 0%
	illegal_entry_count = 0%
	num_input_keys = 0%

 GET_INPUT:
	WHILE (input_key_buf(0) <> 0% OR  &
				terminator_value <> DTK$K_TRM_TIMEOUT)
	!
	! Keep getting commands from the user until the exit key "*",
	! is entered, a wink is detected, or the timeout period expires.
	! First, check if there are any keys in the typeahead buffer (done by 
	! "get_key_string" with a 1 second timeout).  If so, process them. 
	! Otherwise, prompt the user for a command and wait for his response.
	!
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
			GO TO  get_input
		ELSE IF ((terminator_value <> DTK$K_TRM_TIMEOUT) OR	&
	        	 (terminator_value = DTK$K_TRM_TIMEOUT AND	&
				input_key_buf(0) = 0%))
			!
			!  No keys in typeahead buffer so prompt for a command.
			!
		     THEN
				CALL speak_text(DTK$K_WAIT, menu_prompt)
		     END IF
	         END IF
	     END IF
	!
	!  Any new keys received should be processed after the keys read
	!  from the typeahead buffer.  Start storing touch tone keys into
	!  into the buffer following the keys read from the typeahead buffer.
	!
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
			CALL speak_text(DTK$K_WAIT, msg_help)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_ONE			! Boston weather forecast	
			CALL speak_text(DTK$K_WAIT, msg_weather)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_TWO			! transportation info	
			CALL speak_text(DTK$K_WAIT, msg_MBTA)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_THREE			! mortgage rate info	
			CALL speak_text(DTK$K_WAIT, msg_mortgage)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_FOUR			! ski condition info	
			CALL speak_text(DTK$K_WAIT, msg_ski)
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
		CALL speak_text(DTK$K_WAIT, msg_help)
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

     WHILE 1 = 1
	GET #1
	! 
	!  Read in all of the words and substitutions from
	!  the sequential file specified in the command string.
	!
 !	print "dictionary_entry = ", dictionary_entry
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
 !		print "ERROR -- Loading user dictionary.  "
 !		print "WORD: ", diction_word
 !		print "PHONETIC SUBSTITUTION: ", phonetic
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

  COMMON (MESSAGE_TEXT) STRING  rsbracket = 1
  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min
  COMMON (dt_features) LONG voice, speaking_rate, comma_pause_duration, &
		 period_pause_duration, speech_on
  COMMON (application_specific)	LONG	voice_id

  DECLARE LONG old_mode				! Current mode before reset

	CALL speak_text( DTK$K_WAIT, rsbracket)
	!
	!  Set speaking on
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
				DTK$READ_KEYSTROKE

  COMMON (MESSAGE_TEXT) STRING  rsbracket = 1, &
				msg_welcome  = 60,&
				msg_access   = 100, &
				msg_password = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_noaccess = 100, &
				msg_goodbye  = 100

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min
  COMMON (application_specific)	LONG	voice_id

	CALL set_timer(time_6_min)		! Set system (watchdog) timer
	!
	!  Say goodbye and hangup the phone
	!
	stat% = DTK$HANGUP_PHONE(voice_id, msg_goodbye)
	CALL cancel_timer			! Cancel system timer
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		CALL error_log(stat%)
		CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
	END IF
	!
	!  This section of code has been added as a workaround for processing
	!  WINKS at the end of a phone session.  These will be taken care of
	!  by the RTL in later releases.
	!
	timeout% = 1%
	WHILE 1% = 1%
	    CALL set_timer(time_45_sec)		! Set system (watchdog) timer
	    stat% = DTK$READ_KEYSTROKE(voice_id, xx%, , timeout%)
	    CALL cancel_timer			! Cancel system timer

	    IF (stat% = DTK$_ONHOOK) OR (stat% = SS$_TIMEOUT)
	    THEN
		CALL set_timer(time_45_sec)		! Set system (watchdog) timer
		stat% = DTK$READ_KEYSTROKE(voice_id, xx%, , timeout%)
		CALL cancel_timer			! Cancel system timer

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
			CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
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
			CALL LIB$STOP(stat% BY VALUE)	! Fatal error so exit
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
	IF (stat% <> SS$_NORMAL)			! Send to oper failed	
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
