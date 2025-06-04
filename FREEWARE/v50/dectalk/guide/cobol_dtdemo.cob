IDENTIFICATION DIVISION.
PROGRAM-ID.	DECTALK-DEMO.

AUTHOR.

*
*			 COBOL_DTDEMO.COB
* 
*  The following file contains a "modifiable" demonstration program 
*  to be included in the DECtalk Voice Application Development Guide.  This
*  demonstration program typifies most telephone application programs
*  written for DECtalk.  It is written in VAX-COBOL and uses Version 4.5
*  or later of the VMS/MicroVMS Run-Time Library Support provided for 
*  DECtalk.
*
*  Description:	This program provides a framework to develop other
*		demonstration programs.  Currently, it provides an
*		information services demonstration giving the Boston
*		weather forecast, transportation information, ski
*		conditions, and mortgage rate information in a single
*		menu system.  The program provides dial-in access only.
*		When a user calls in, DECtalk answers the phone and speaks a
*		short greeting message.  Then, the customer is asked to enter
*		his access code and password.  The customer is given 
*		three attempts to enter his access code and three 
*		attempts to enter his password.  Once access to the 
*		system has been gained, DECtalk prompts the user to
*		enter a command.  The six valid commands are given below:
*
*		   Key 1 for the Boston weather forecast.
*		   Key 2 for transportation information.
*		   Key 3 for mortgage rate information.
*		   Key 4 for the current ski conditions.
*		   Key 0 for help.
*		   Key * to exit.
* 
*	NOTE:	the user's access code, password and all commands
*		except exit must be terminated by the pound sign key,
*		although the application will accept input without the
*		pound sign key after a timeout.  When a valid command is 
*		entered, DECtalk speaks the appropriate message to the user.
*		Once the exit key is entered, a wink is detected, or a user
*		fails to enter a command in the specified time period, the 
*		user session is ended.  DECtalk speaks a goodbye message to
*		the user and hangs up the telephone.  Then, DECtalk is
*		re-enabled for autoanswer and waits for a new telephone call.
*
*		All error messages are logged only to the operator terminals
*		that have specified that the "OPER11" type of message will
*		be handled. These messages give the error message, and the
*		terminal line connected to that process.  To have error
*		messages logged to the console terminal, or any other terminal,
*		at the DCL prompt ($), enter the REPLY/ENABLE command:
*
*			$ reply/enable=oper11
*
*		To enter this command, you must have the OPER user privelege.
*
*
*  VAX COBOL LANGUAGE RESTRICTIONS:
*
*	If running a COBOL program on a MicroVAX you should be careful
*	to use only the COMPUTATIONAL data type for integer variables.
*	This is because the MicroVAX only emulates the COBOL commands
*	that deal with packed decimal variables, causeing the application
*	to run VERY slow.  All integer variables being passed to the RTL
*	routines must also be type COMP.  
*
*  VMS VERSION 4.5 RUN-TIME LIBRARY RESTRICTIONS FOR DECtalk SUPPORT:
*
*	There is a problem with the DTKDEF module in STARLET.OLB.
*	The definitions of the touch tone key constants DTK$K_TRM_ZERO thru
*	DTK$K_TRM_NINE should be set to the ASCII valies of the digits.
*	The correct definitions are currently found in "dtkdef.lib".  Note,
*	it is anticipated that this will be fixed in the Version 4.6 release
*	of VMS.  Consult the release notes for Version 4.6.
*
*	The current version of the RTL does not provide a command to check
*	the status of DECtalk.  With this command, an application program 
*	can determine whether or not the DECtalk module has power cycled 
*	since the last time its status has been observed.  If it is detected
*	that the DECtalk module has power cycled, the application specific 
*	parameters (speaking voice, speaking rate, words loaded into the user 
*	loadable dictionary) should be reinitialized.  By periodically checking 
*	the status of the DECtalk module, and reinitializing if the unit
*	has power cycled, an application may not have to be terminated
*	to replace failed DECtalk modules.  An alternative solution (used in
*	this demonstration program) re-initializes application specific 
*	parameters every time a phone call has not been received in 15 minutes.
*
*	Most of the DTK$ RTL functions that read and return a status condition
*	from the DECtalk do not have a timeout specified on their read from 
*	DECtalk.  These DTK$ RTL functions include DTK$HANGUP_PHONE,
*	DTK$LOAD_DICTIONARY, DTK$RETURN_LAST_INDEX, DTK$SET_KEYPAD_MODE, 
*	DTK$SPEAK_FILE, DTK$SPEAK_PHONEMIC_TEXT, DTK$SPEAK_TEXT.  Without a
*	timeout, it is possible for the application program to hang if the 
*	DECtalk module fails, the power cord is disconnected or the RS232 cable
*	is disconnected. To prevent the application program from hanging
*	without notifying the operator of the problem, a system timer 
*	(using SYS$SETIMR) is set before ALL calls made to the DTK$ facility
*	of the Run-Time Library.  This is done by performing the routine
*	SET_TIMER.  If a response is received from the DECtalk within the time
*	period specified, the system timer is canceled (using SYS$CANTIM) by
*	performing the routine CANCEL_TIMER. Otherwise, if the timer expires,
*	the timeout value 556 will be returned. To correct any error that may
*	occur in the communication between the DECtalk module and the physical
*	device, terminate the current job, correct the error, and then restart
*	the job.  Note, the application program will hang until the problem
*	has been corrected.
*
*	The current version of DTK$READ_STRING does not work correctly.  If
*	a series of touch tone keys is entered on the touch tone keypad and
*	a terminating character (number sign key or asterisk) is not entered,
*	the series of touch tone keys entered is returned after the timeout
*	period specified along with a terminator code of DTK$K_TRM_TIMEOUT.
*	On the next call to DTK$READ_STRING, when the series of touch tone
*	keys is returned, it always includes the last touch tone key returned
*	in the previous call to DTK$READ_STRING as the first touch tone key in
*	the series of keys returned.  The routine "GET_KEY_STRING"
*	in this demonstration program can be used to read a series of touch
*	tone keys terminated by the number sign key or the asterisk key.  It
*	is similar in functionality to the DTK$READ_STRING routine.
*

DATE-WRITTEN.	OCT-86.
DATE-COMPILED.

ENVIRONMENT DIVISION.
CONFIGURATION SECTION.
SOURCE-COMPUTER.	VAX.
OBJECT-COMPUTER.	VAX.

INPUT-OUTPUT SECTION.
FILE-CONTROL.
	SELECT OPTIONAL USER-DICTIONARY ASSIGN TO "DICTIONARY.USER" 
				FILE STATUS IS file_stat.

DATA DIVISION.
FILE SECTION.
FD	USER-DICTIONARY
	VALUE OF ID IS DICTIONARY_NAME.
01	ENTRY	PIC X(257).

WORKING-STORAGE SECTION.
COPY DTKDEF.
01	SS$_NORMAL	PIC S9(9) COMP VALUE EXTERNAL SS$_NORMAL.
01	SS$_TIMEOUT	PIC S9(9) COMP VALUE EXTERNAL SS$_TIMEOUT.
01	STS$M_SUCCESS	PIC S9(9) COMP VALUE EXTERNAL STS$M_SUCCESS.
01	LIB$SIGNAL	USAGE IS POINTER VALUE EXTERNAL	LIB$SIGNAL.

01 RSBRACKET	PIC X VALUE "]".

01 MSG_WELCOME	PIC X(80)  VALUE 
	"Welcome to the Dectalk information services demonstration.".

01 MSG_ACCESS	PIC X(120)  VALUE "Please enter your Access Code followed
-    " by the Pound-key. You may enter any number as a test. ".

01 MSG_PASSWORD	PIC X(120)  VALUE "Please enter your Password followed
-    " by the Pound-key. You may enter any number as a test. ".

01 MSG_NOACCESS	PIC X(80)  VALUE 
     "Access denied. Please check your access code and password and try again. ".

01 MSG_GOODBYE	PIC X(100)  VALUE "Thank you for calling the Dectalk 
-    " application demonstration program. Have a nice day. ".

01 MSG_TIMEOUT	PIC X(80)  VALUE 
	"No key pressed in the timeout period specified. ".

01 MSG_BAD_COMMAND  PIC X(80) VALUE "Invalid command. Please try again. ". 

01 MSG_NO_TERMINATOR  PIC X(100)  VALUE "Please remember to terminate 
-    " your entry with the pound-key.  Command accepted.  ".

01 MENU_PROMPT	PIC X(150)  VALUE 
	"Please enter a command. For help, press 0 followed by the pound
-    "-key. To exit, press the star-key. ".

01 MSG_HELP.
   03 MSG_HELP_1	PIC X(256) VALUE 
	" To hear the current Boston weather forecast, press 1. 
-	" To hear transportation information, press 2. 
-	" To hear mortgage rate information, press 3.
-	" To hear the current ski conditions, press 4.".

   03 MSG_HELP_2	PIC X(256) VALUE
	" To repeat this message, press 0.
-	" Terminate your entry with the pound-key.
-	" To exit, press the star-key.".

01 MSG_WEATHER.
   03 MSG_WEATHER_1	PIC X(256) VALUE 
     " Welcome to the Boston area weather service. April 1st.  Today will be
-    " a day [\""]more like early May. Current downtown Boston temperature is
-    " 58 degrees Fahrenheit, 14 Celsius. It will be a [\""]beautiful sunny day,
-    " breezy and mild, ". 
   03 MSG_WEATHER_2	PIC X(256) VALUE 
     " with a high of 76 degrees Fahrenheit.  Humidity will be 76%.
-    " Barometric pressure is currently 32.5.  Tomorrow will be cooler, with a
-    " high of fifty on the coast and sixty inland.  Fair weather is expected
-    " to continue throughout the week." .

01 MSG_MBTA.
   03 MSG_MBTA_1	PIC X(256) VALUE
     " Welcome to the MBTA information line.  All MBTA lines are on time. The
-    " B and M line from the north shore is running a limited service, with
-    " trains running from Ipswich and Rockport at 8 A.M. and 9 A.M. only.".
   03 MSG_MBTA_2	PIC X(256)  VALUE 
     " There will be no red line service between Park Street and Harvard,
-    " Monday from 9 P.M. to 1 A.M..  Substitute bus transportation will be
-    " provided.".

01 MSG_MORTGAGE.
   03 MSG_MORTGAGE_1	PIC X(256) VALUE 
     " Welcome to Hamden National Bank's mortgage line. All of the following
-    " rates are subject to change. The application fee is $250. 30 year rate
-    " with 10% [']down are 9.9% with 3 [aen ax] half points, 10.2% with 3
-    " points, 10.5% with 2 [aen ax] half points,".
   03 MSG_MORTGAGE_2	PIC X(256) VALUE 
     " 10.8% with no points. 15 year rates with 10% [']down are 9.9% with 2
-    " [aen ax] half points, 10.1% with 2 points, and 10.5% with no points.
-    " Adjustable rates are at 8.5% fixed for 3 years with 2% a year and 6%
-    " lifetime caps thereafter.".

01 MSG_SKI.
  03 MSG_SKI_1		PIC X(256) VALUE 
     " Welcome to the New England ski report. January 31st. skiing is excellent
-    " in Vermont because of yesterday's snowfall of 10 inches.  All ski trails
-    " are open at Stowe, Sugar-Bush, Mt. Snow, Stratton, Jay peak, and
-    " Smuggler's Notch, ".
  03 MSG_SKI_2		PIC X(256) VALUE 
     " with packed powder and full snow-making in operation.  All cross country
-    " ski trails are also open, with many groomed trails.  Skiing in Maine and
-    " [nuw] Hampshire is good, with cold temperatures allowing for constant
-    " snow-making. ".
  03 MSG_SKI_3		PIC X(256) VALUE 
     " Gun-stock has 15 [aatax] 25 trails open, Mt. Cranmore has 15 [aatax] 20
-    " trails open, Attitash and Wildcat have all trails open.  Sugar-loaf and
-    " Sunday River also have all trails open, with packed powder and loose
-    " granular. ".
  03 MSG_SKI_4		PIC X(90) VALUE
     " Expected snowfall in Maine and [nuw] Hampshire should improve weekend
-    " ski conditions. ".

*  constants
01	MAX_ENTRY	PIC 9(9) VALUE 3	USAGE IS COMP.
01	T15MINUTE	PIC 9(9) VALUE 900	USAGE IS COMP.
01	TS45SECOND	PIC X(10) VALUE "0 00:00:45".
01	TS1MINUTE	PIC X(10) VALUE "0 00:01:00".
01	TS2MINUTES	PIC X(10) VALUE "0 00:02:00".
01	TS6MINUTES	PIC X(10) VALUE "0 00:06:00".
01	TS11MINUTES	PIC X(10) VALUE "0 00:11:00".
01	TS16MINUTES	PIC X(10) VALUE "0 00:16:00".
*
*   DECtalk specific variables
*
01	voice_id	PIC 9(9)  USAGE IS COMP.
01	return_status_mode PIC 9(9) USAGE IS COMP.
01	terminator_value PIC 9(9) USAGE IS COMP.
01	keypad_mode	PIC 9(9) USAGE IS COMP.
01	keypad_on	PIC 9(9) USAGE IS COMP.
01	number_of_rings	PIC 9(9) USAGE IS COMP VALUE 0.
01	voice		PIC 9(9) USAGE IS COMP.
01	speaking_rate	PIC 9(9) USAGE IS COMP VALUE 180.
01	comma_pause_duration	PIC 9(9) USAGE IS COMP VALUE 0.
01	period_pause_duration	PIC 9(9) USAGE IS COMP VALUE 0.
01	speech_on	PIC 9(9) USAGE IS COMP.
01	new_mode	PIC 9(9) USAGE IS COMP.
*
*   Return status variables
*
01	return_status	PIC S9(9) USAGE IS COMP.
01	ret_stat	PIC S9(9) USAGE IS COMP.
*
*   Timer variables
*
01	set_timer_string PIC X(10).
01	delta_time	PIC S9(18) USAGE COMP.
*
*   Dictionary file related variables
*
01	word-cnt		PIC 9(9) USAGE IS COMP.
01	phonem-cnt		PIC 9(9) USAGE IS COMP.
01	more-data-remains-flag	PIC XXX VALUE 'YES'.
   88	more-data-remains		VALUE 'YES'.
   88	no-more-data-remains		VALUE 'NO'.
01	dictionary_name		PIC X(20).
01	file_stat		PIC XX.
*
*   General user buffers and variables
*
01	menu_choice		pic 9(9) usage is comp.
01	keypad_input_buffer	pic X(80).
01	read_key_buffer		PIC 9(9) usage is comp.
01	read_key_string REDEFINES read_key_buffer PIC XXXX.
01	temp_char		PIC X.
01	temp_int	REDEFINES temp_char PIC 9(9) USAGE IS COMP.
01	access_code		pic X(80).
01	password_code		pic X(80).
01	speak_text_buffer	pic X(860).
01	xx			pic 9(9).
01	num_input_keys		pic 9(9) usage is comp value 0.
01	num_hold_keys		pic 9(9) usage is comp value 0.
01	buf_pointer		pic 9(9) usage is comp value 0.
01	hold_key_buf		pic X(80).
01	d_name_length		pic 9(9) usage is comp value 0.
01	device_length		pic 9(9) usage is comp value 0.
01	get_param_buf		pic X(80).
01	abort			PIC X(5) VALUE "FALSE".
*
*   Application specific variables
*
01	timeout			PIC 9(9)  USAGE IS COMP.
01	advance_timeout		PIC 9(9)  USAGE IS COMP.
01	version			PIC S9(9) USAGE IS COMP.
01	terminal_line_string	PIC X(16) GLOBAL.
01	illegal_entry_count	PIC 9(9) USAGE IS COMP VALUE 0.
01	initialize_flag		PIC XXX VALUE 'YES'.
   88	need_to_initialize		VALUE 'YES'.
   88	already_initialized		VALUE 'NO'.
*
*   Error processing variables
*
01	error_msg	PIC X(130).
01	error_structure REDEFINES error_msg.
   03	OPC$type_target	PIC 9(9) COMP.
   03	OPC$L_MS_RQSTID	PIC 9(9) COMP.
   03	error_text	PIC X(120).
01	error_number	PIC 9(9) COMP.
01	error_buf	PIC X(80).
01	on_text		PIC X(6) VALUE "  ON  ".
01	control_str	PIC X(9) VALUE "!AD!AS!AD".
01	buf_len		PIC 9(9) COMP.

*%page

PROCEDURE DIVISION.
MAIN-PROGRAM-SPACE.

*	initialization of the DECtalk parameters.
*
	MOVE DTK$K_VOICE_MALE TO voice.
	MOVE DTK$K_KEYPAD_AUTO TO keypad_mode.
	MOVE DTK$K_KEYPAD_ON TO keypad_on.
	MOVE DTK$K_SPEAK TO speech_on.
	MOVE DTK$M_SQUARE TO new_mode.
	MOVE DTK$K_WAIT TO return_status_mode.
*
*	Get the name of the physical device that the DECtalk module is
*	connected to and the name of the dictionary file (if specified)
*	that contains DECtalk's user dictionary words. If the call to 
*	DTK$INITIALIZE is unsuccessful, then exit.
*
	PERFORM GET_CLI_PARAMS.
	move TS45SECOND to set_timer_string.
	perform set_timer.
	CALL "DTK$INITIALIZE" USING	BY REFERENCE	VOICE_ID,
					BY DESCRIPTOR	TERMINAL_LINE_STRING,
					BY REFERENCE	VERSION
			      GIVING RETURN_STATUS.
	perform cancel_timer.
	IF RETURN_STATUS IS failure
		MOVE return_status to error_number
		PERFORM error_log
		STOP RUN.

	PERFORM
		NORMAL-OPERATION-PARAGRAPH
			UNTIL ABORT IS = "TRUE".
ABORT_PROGRAM.
*
*	some fatal error has occured... Terminate DECtalk and exit
*
	move TS45SECOND to set_timer_string.
	perform set_timer.
	CALL "DTK$TERMINATE" USING  VOICE_ID
			     GIVING RETURN_STATUS.
	perform cancel_timer.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log.
	STOP RUN.

NORMAL-OPERATION-PARAGRAPH.
*
*  Main loop of the demonstration program.
*  First, check to see if the application specific parameters of
*  the DECtalk need to be initialized (initialize_flag=need_to_initialize).
*  If so, reinitialize them by calling INITIALIZE_DECTALK.  Next, set 
*  a watchdog timer for the DTK$ANSWER_PHONE command. If the
*  phone does not ring in the timeout period specified, (or the 
*  watchdog timer expires), reinitialize the application specific 
*  parameters of DECtalk.  Normally, these parameters would not be 
*  reinitialized everytime a phone call has not been received, but
*  ONLY when the DECtalk module had power cycled.  However, in the
*  current version of the VMS Run-Time Library Support for DECtalk
*  (Version 4.5), it is not possible to check the status of DECtalk
*  (detect if it has power cycled).  If a phone call is received 
*  within the timeout period, the phone is answered, autostop keypad
*  mode and wink detection are enabled (the default), and a greeting
*  message is spoken to the user.  Next, the user must be verified as a
*  valid user of the system.  As soon as the user has successfully gained
*  access to the system, he can start entering main menu commands.
*  The code in this loop is continuely executed until a fatal error
*  occurs or the process is stopped.
*
	IF need_to_initialize
		PERFORM initialize_dectalk
		IF return_status IS NOT SUCCESS
*
*			Initialization failed.  It appears that
*			the DECtalk module may be dead so exit
*
			GO TO abort_program.
		MOVE 'NO' TO initialize_flag.
	MOVE spaces TO hold_key_buf.
*  
*	The greeting message DECtalk speaks upon answering the telephone can
*	be modified by changing the text in the character string "msg_welcome".
*	The number of rings DECtalk waits to answer the telephone
*	("number_of_rings"), can be changed but it is recommended that the
*	phone is always answered on the first ring.
*
	MOVE 0 TO num_hold_keys.
	MOVE 0 TO num_input_keys.
	MOVE TS16MINUTES TO set_timer_string.
	PERFORM set_timer.
	CALL "DTK$ANSWER_PHONE" USING BY REFERENCE	voice_id
				      			number_of_rings
				      BY DESCRIPTOR	msg_welcome
				      BY REFERENCE	T15MINUTE
				GIVING return_status.
	PERFORM cancel_timer.
	IF return_status IS EQUAL TO SS$_NORMAL

*	The telephone has been answered. Next verify that the caller is a
*	valid user of the system.  If the caller fails to enter a valid 
*	access code and password in three attempts, access to the system
*	is denied.  A warning message is spoken to the user and the current
*	phone call is ended by invoking "end_call".
*
		PERFORM verify_user THRU exit_verify_user
		IF return_status IS NOT SUCCESS
			MOVE msg_noaccess TO speak_text_buffer
			PERFORM speak_text
			PERFORM end_call
		ELSE
*
*			The user has successfully gained access to the system.
*			Start processing commands from the user.  Note, the 
*			menu prompt spoken prior to receiving commands from the
*			user, can be modified by changing the text in the
*			character string "menu_prompt"
*

			PERFORM menu THRU exit_menu
			PERFORM end_call THRU exit_end_call
	ELSE IF return_status IS EQUAL TO SS$_TIMEOUT
		MOVE 'YES' TO initialize_flag
	ELSE 
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.
*  --------------------  END OF MAIN PROGRAM  ----------------------------
GET_CLI_PARAMS.
*
*  Gets the parameters from the command line using LIB$GET_FOREIGN.
*  If a fatal error occurs, it is reported and the demo is stopped.
*  Otherwise, the parameters specified are returned in terminal_line_string
*  for the device name, and dictionary_name for the name of the dictionary file.
*

	INSPECT get_param_buf REPLACING CHARACTERS BY X"00" BEFORE X"00".

	CALL "LIB$GET_FOREIGN" USING BY DESCRIPTOR get_param_buf,
						   OMITTED,
						   OMITTED,
						   OMITTED
			     GIVING return_status.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.

	INSPECT get_param_buf TALLYING device_length FOR CHARACTERS BEFORE " ",
			d_name_length FOR CHARACTERS AFTER " " BEFORE X"00".
	
	MOVE get_param_buf(1:device_length) TO terminal_line_string.
	MOVE get_param_buf(device_length + 2: d_name_length) TO dictionary_name.

*  **********************************************************************
INITIALIZE_DECTALK.
*
*  Initializes the application specific parameters of DECtalk.
*  First, left square bracket ('[') and right square bracket (']') 
*  are enabled as phonemic delimiters. To specify other modes, the bit masks
*  for the modes to be set should be OR'd together with the DTK$M_SQUARE bit
*  mask and assigned to the varible "NEW_MODE".  Next, the default speaking 
*  voice and speaking rate are selected for the application.  The comma
*  pause and period pause are set to DECtalk defaults.  Other voices
*  can selected for the default speaking voice by modifying the variable
*  "voice".  Likewise, a different speaking rate can be specified
*  by changing the static variable "speaking_rate".  Finally, the user
*  dictionary is loaded (by invoking "load_dictionary").  Note, other
*  application specific parameters should also be initialized in this
*  subroutine if the need arises.
*
	MOVE TS45SECOND TO set_timer_string.
	PERFORM set_timer.
	CALL "DTK$SET_MODE" USING  BY REFERENCE voice_id,
				   BY REFERENCE new_mode
			    GIVING return_status.
	PERFORM cancel_timer.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.
	PERFORM set_timer.
	CALL "DTK$SET_VOICE" USING  BY REFERENCE voice_id,
						 voice,
						 speaking_rate,
						 comma_pause_duration,
						 period_pause_duration
			     GIVING return_status.
	PERFORM cancel_timer.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.

	PERFORM load_dictionary THRU exit_load_dictionary.
*  ----------------------------------------------------------------------
VERIFY_USER.
*
*  Verifies that the caller is a valid user of the system.
*  The caller is given three attempts to enter a valid access code and
*  three attempts to enter his password.  If the caller fails to enter
*  a valid access code or a valid password in the timeout period specified,
*  FALSE is returned.  Otherwise, TRUE is returned.
*
*  NOTE: In this demonstration, almost all access codes and passwords
*	 are detected as valid.  The only ways an access code or a 
*	 password are rejected are if the user fails to enter an access
*	 code or a password in the time period specified, or if the user
*	 terminates his access code or password with the star key ("*")
*	 rather than the pound key ("#").  All valid access codes and passwords
*	 (terminated with the pound key) are verified by performing the dummy
*	 verification routines "access_verify" and "password_verify".
*	 Both of these routines always return TRUE.
*
	MOVE 0 TO illegal_entry_count.
	PERFORM
		get-access-code
	UNTIL illegal_entry_count =  max_entry.
exit_verify_user.

GET-ACCESS-CODE.
*
*  Gets the user's access code entered on the touch tone keypad.  The entered
*  access code is returned in the character buffer "access_code".  Currently,
*  the maximum size of the buffer is 80 characters. The user is prompted for
*  his access code by the text specified in "msg_access". To have a different
*  prompt spoken, the character string "msg_access" should be modified.
*  Currently, the application waits 30 seconds for a touch tone key to be
*  entered.  If a longer or shorter timeout period is desired, the new timeout
*  value (in seconds) should be moved into timeout before the call to
*  "get_key_string". The parameter "terminator_value" will contain the
*  character used to terminate the key string or a timeout upon return from
*  the "get_key_string" routine.     
*
	MOVE 0 TO num_input_keys.
	MOVE 30 TO timeout.
	MOVE msg_access TO speak_text_buffer.
	PERFORM get_key_string THRU exit_get_key_string.
	MOVE keypad_input_buffer TO access_code.

*
*	If an invalid code was entered, increment the count of invalid access
*	code entry attempts.  If this count is greater than the maximum allowed
*	(specified by MAX_ENTRY), return FALSE.  Otherwise, notify the user
*	and prompt him again.  If a valid access code is entered, then prompt
*	the user for his password.
*
	IF return_status IS SUCCESS
		PERFORM access_verify
		IF terminator_value IS EQUAL TO DTK$K_TRM_ASTERISK OR
		   terminator_value IS EQUAL TO DTK$K_TRM_TIMEOUT  OR
		   return_status IS FAILURE
			ADD 1 TO illegal_entry_count
			IF illegal_entry_count = max_entry
				SET return_status TO FAILURE
				GO TO exit_verify_user
			ELSE
			     MOVE msg_bad_command TO speak_text_buffer
			     PERFORM speak_text
			     MOVE spaces TO keypad_input_buffer
		ELSE
			MOVE spaces TO keypad_input_buffer
			MOVE 0 TO illegal_entry_count
			PERFORM get-password-code
				UNTIL illegal_entry_count = max_entry
	ELSE
		SET return_status TO FAILURE
		GO TO exit_verify_user.

GET-PASSWORD-CODE.
*
*  Gets the user's password entered on the touch tone keypad. The entered
*  password is returned in the character buffer "password_code".  Currently,
*  the maximum size of the buffer is 80 characters.  The user is prompted for
*  his password by the text specified in "msg_password".  To have a different
*  prompt spoken, the character string "msg_password" should be modified.
*  Currently, the application waits 30 seconds for a touch tone key to be
*  entered.  If a longer or shorter timeout period is desired, the new timeout
*  value (in seconds) should be moved into TIMEOUT before "get_key_string". The
*  parameter, "terminator_value" will contain the character used to terminate
*  the key string or timeout upon return from the "get_key_string" routine.     
*
	MOVE 0 TO num_input_keys.
	MOVE 30 TO timeout.
	MOVE msg_password TO speak_text_buffer.
	PERFORM get_key_string THRU exit_get_key_string.
	MOVE keypad_input_buffer TO password_code.
*
*	If an invalid password code was entered, increment the count of invalid
*	entry attempts.  If this count is greater than the maximum allowed
*	(specified by MAX_ENTRY), return FALSE.  Otherwise, notify the user
*	and prompt him again. If a valid password is entered, then return TRUE.
*
	IF return_status IS SUCCESS
		PERFORM password_verify
		IF terminator_value IS EQUAL TO DTK$K_TRM_ASTERISK OR
		   terminator_value IS EQUAL TO DTK$K_TRM_TIMEOUT  OR
		   return_status IS FAILURE
			ADD 1 TO illegal_entry_count
			IF illegal_entry_count = max_entry
				SET return_status TO FAILURE
				GO TO exit_verify_user
			ELSE
				MOVE msg_bad_command TO speak_text_buffer
				PERFORM speak_text
				MOVE spaces TO keypad_input_buffer
		ELSE
			SET return_status TO SUCCESS
			GO TO exit_verify_user
	ELSE
		SET return_status TO FAILURE
		GO TO exit_verify_user.

*  ----------------------------------------------------------------------
GET_KEY_STRING.
* 
*  Gets a string of touch tone keys entered on the telephone keypad terminated
*  by the pound key "#" or the star key "*".  Returns TRUE if the string of
*  touch tone keys was received successfully. Otherwise, FALSE is returned.
*
	IF num_hold_keys IS GREATER THAN 0
*
*		read any characters waiting in typeahead buffer.  We already
*		have "keys" in "hold_key_buf" (num_hold_keys > 0), but the
*		user could have hung up the phone with several commands pending.
*		This call to "read_advance_keys" is mainly used to read any
*		winks that are detected by DECtalk.  If a wink is detected,
*		the program should hangup the phone, and reset for a new caller.
*
		MOVE 1 TO advance_timeout
		PERFORM read_advance_keys
		   TEST AFTER UNTIL return_status IS EQUAL TO SS$_TIMEOUT
		MOVE 1 TO buf_pointer
		PERFORM hold_buf_to_key_buf
		UNTIL (hold_key_buf(buf_pointer:1) IS EQUAL TO "#" OR
		       hold_key_buf(buf_pointer:1) IS EQUAL TO "*")
*
*	The terminator code needs to be moved to "terminator_value" which is a
*	longword.  To get the terminator character into the longword, it
*	is moved into the character variable "temp_char" which has been
*	redefined as an integer "temp_int".  "temp_int" can then be moved
*	correctly into terminator_value.
*
		MOVE hold_key_buf(buf_pointer:1) TO temp_char
		MOVE temp_int TO terminator_value
		MOVE 0 TO num_hold_keys
		ADD 1 TO buf_pointer
		PERFORM copy_to_start_of_holdbuf
			UNTIL hold_key_buf(buf_pointer:1) = X"20"
		MOVE space TO hold_key_buf(num_hold_keys + 1:1)
		GO TO process_entry.

	IF speak_text_buffer(1:1) IS NOT EQUAL TO X"00"
		MOVE DTK$K_IMMED TO return_status_mode
		PERFORM speak_text.
		MOVE DTK$K_WAIT TO return_status_mode

	PERFORM read_keys
			UNTIL return_status IS EQUAL TO SS$_TIMEOUT.
read_keys.
*
*  Read keys from DECtalk.
*
	MOVE TS45SECOND TO set_timer_string.
	PERFORM set_timer.
	CALL "DTK$READ_KEYSTROKE" USING BY REFERENCE voice_id,
					BY REFERENCE read_key_buffer,
						     OMITTED,
					BY REFERENCE timeout
			       GIVING return_status.
	PERFORM cancel_timer.
	EVALUATE TRUE
*
*	It is important that the check for WINK comes before the check
*	for SUCCESS, because WINK is a successful status. The test for 
*	SUCCESS only tests the bottom 3 bits, not the entire longword.
*
	WHEN return_status IS EQUAL TO DTK$_WINK
		MOVE return_status TO error_number
		PERFORM error_log
		MOVE 0 TO return_status
		GO TO exit_get_key_string

	WHEN return_status IS SUCCESS
*
*		SUCCESS:  if a terminator is read, set the timeout to 2 seconds
*			  and read any advance keys pressed.  Otherwise copy
*			  the key to input buffer.  The timeout is shortened to
*			  10 seconds to read any further keys.
*
		IF (read_key_buffer IS EQUAL TO DTK$K_TRM_ASTERISK OR
		    read_key_buffer IS EQUAL TO DTK$K_TRM_NUMBER_SIGN)
			MOVE read_key_buffer TO terminator_value
			MOVE 2 TO advance_timeout
			PERFORM read_advance_keys 
			  TEST AFTER UNTIL return_status IS EQUAL TO SS$_TIMEOUT
			GO TO process_entry
		ELSE
			MOVE read_key_string(1:1) TO
				keypad_input_buffer(num_input_keys + 1:1)
			ADD 1 TO num_input_keys
			MOVE 10 TO timeout
		END-IF

	WHEN return_status IS EQUAL TO SS$_TIMEOUT
		MOVE DTK$K_TRM_TIMEOUT TO terminator_value
		GO TO process_entry 	

	WHEN OTHER
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN
	END-EVALUATE.
		
PROCESS_entry.	
	PERFORM restart.

EXIT_GET_KEY_STRING.

read_advance_keys.
*
*	read any advance keys pressed in "timeout" seconds and put into
*	"hold_key_buf".  Return when done.
*
	MOVE TS45SECOND TO set_timer_string.
	PERFORM set_timer.
	CALL "DTK$READ_KEYSTROKE" USING BY REFERENCE voice_id,
					BY REFERENCE read_key_buffer,
						     OMITTED,
					BY REFERENCE advance_timeout
			       GIVING return_status.
	PERFORM cancel_timer.
	EVALUATE TRUE
*
*		wink must be checked first because COBOL will interpret it
*		as a SUCCESS status.
*
	WHEN return_status IS EQUAL TO DTK$_WINK
		MOVE return_status TO error_number
		PERFORM error_log
		MOVE 0 TO return_status
		GO TO exit_get_key_string

	WHEN return_status IS SUCCESS
		MOVE read_key_string(1:1) TO hold_key_buf(num_hold_keys + 1:1)
		ADD 1 TO num_hold_keys

	WHEN return_status IS EQUAL TO SS$_TIMEOUT
		MOVE space TO hold_key_buf(num_hold_keys + 1:1)
*
*		Move space to end of hold key string.  This space marks the end
*		of advance keys in "hold_key_buf".
*
	WHEN OTHER
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN
	END-EVALUATE.

hold_buf_to_key_buf.
*
*	Copy keys from "hold_key_buf" to "input_key_buf" up to a terminator
*	("*", "#", or space).
*
	IF hold_key_buf(buf_pointer:1) IS EQUAL TO X"20"
		MOVE 0 TO num_hold_keys
		MOVE spaces TO hold_key_buf
		GO TO read_keys
	ELSE
		MOVE hold_key_buf(buf_pointer:1) TO
				 keypad_input_buffer(num_input_keys + 1:1) 
		ADD 1 TO num_input_keys   buf_pointer.

copy_to_start_of_holdbuf.
*
*	Copy any remaining keys in "hold_key_buf" to the begining of
*	"hold_key_buf".
*
	MOVE hold_key_buf(buf_pointer:1) TO  hold_key_buf(num_hold_keys + 1:1).
	ADD 1 TO num_hold_keys buf_pointer.

*  ----------------------------------------------------------------------
ACCESS_VERIFY.
*
*  Verifies the access code received from the user.  This routine always
*  returns TRUE for the purpose of this demonstration.  In a real application,
*  the entry would be verified against access codes in the database.
*
	SET RETURN_STATUS TO SUCCESS.

*  **********************************************************************
PASSWORD_VERIFY.
*
*  Verifies the password received from the user. This routine always returns
*  TRUE for the purpose of this demonstration.  In a real application, the
*  entry would be verified against their password in the database.
*
	SET RETURN_STATUS TO SUCCESS.

*  **********************************************************************
MENU.
*
*  Prompts the user for a command and receives the command (touch tone
*  key) from the user.  Note, the command will be accepted whether or not
*  it is terminated with the pound key "#".  However, if the command
*  is not followed by the pound key, the command will not be processed
*  until the timeout period has expired and a warning message has been spoken.
*  When commands are received, the routine "process_menu_entry" is performed
*  to implement the menu choice.
*
	MOVE 0 TO num_input_keys.
	MOVE 0 TO illegal_entry_count.
	PERFORM
	   get_keypad_entry THRU exit_get_keypad_entry
		UNTIL (keypad_input_buffer(1:1) IS EQUAL TO X"20"  AND
			terminator_value IS EQUAL TO DTK$K_TRM_TIMEOUT)
	SET return_status TO SUCCESS.
EXIT_MENU.

*  ----------------------------------------------------------------------
GET_KEYPAD_ENTRY.
*
*  Keep getting commands from the user until the exit key "*",
*  is entered, a wink is detected, or the timeout period expires.
*  First, check if there are any keys in the typeahead buffer (done by 
*  "get_key_string" with a 1 second timeout).  If so, process them. 
*  Otherwise, prompt the user for a command and wait for his response.
*
	MOVE spaces TO keypad_input_buffer.
	MOVE 1 TO timeout.
	MOVE X"00" TO speak_text_buffer.	
	PERFORM get_key_string THRU exit_get_key_string.

	IF return_status IS FAILURE
		GO TO exit_menu.
	EVALUATE TRUE
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_NUMBER_SIGN
		PERFORM process_menu_entry
		GO TO exit_get_keypad_entry
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_ASTERISK
		SET return_status to SUCCESS
		GO TO exit_menu
	  WHEN (terminator_value IS NOT EQUAL TO DTK$K_TRM_TIMEOUT OR
		(terminator_value is equal to DTK$K_TRM_TIMEOUT AND
			keypad_input_buffer(1:1) IS EQUAL TO X"20"))
		MOVE menu_prompt TO speak_text_buffer
		PERFORM speak_text
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_TIMEOUT AND
			keypad_input_buffer(1:1) IS NOT EQUAL X"20"
*
*		Touch tone keys entered but no command terminator ("#")
*		entered.  Warn the user that commands must be terminated
*		by the pound key and then process the entered command.
*
		MOVE msg_no_terminator TO speak_text_buffer
		PERFORM speak_all_text
		PERFORM process_menu_entry
		GO TO exit_get_keypad_entry
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_ASTERISK
		SET return_status TO SUCCESS
		GO TO exit_menu

	END-EVALUATE.
*
*	Read any new keys from keypad, and take appropriate action.
*
	MOVE 20 TO timeout.
	MOVE X"00" TO speak_text_buffer.
	PERFORM get_key_string THRU exit_get_key_string.
	IF return_status IS FAILURE
		GO TO exit_menu.

	EVALUATE TRUE
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_NUMBER_SIGN
		PERFORM process_menu_entry
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_TIMEOUT AND
			keypad_input_buffer(1:1) IS NOT EQUAL X"20"
*
*		Touch tone keys entered but no command terminator ("#")
*		entered.  Warn the user that commands must be terminated
*		by the pound key and then process the entered command.
*
		MOVE msg_no_terminator TO speak_text_buffer
		PERFORM speak_all_text
		PERFORM process_menu_entry
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_TIMEOUT AND
			keypad_input_buffer(1:1) IS EQUAL X"20"
		MOVE msg_timeout TO speak_text_buffer
		PERFORM speak_all_text
		SET return_status TO SUCCESS
		GO TO exit_menu
	  WHEN terminator_value IS EQUAL TO DTK$K_TRM_ASTERISK
		SET return_status to SUCCESS
		GO TO exit_menu

	END-EVALUATE.
EXIT_GET_KEYPAD_ENTRY.
*  ----------------------------------------------------------------------
PROCESS_MENU_ENTRY.
*
*  Processes the touch tone key string received from the user.
*
	IF num_input_keys IS EQUAL TO 1
		MOVE keypad_input_buffer(1:1) TO menu_choice
*
*		-- Make the menu choice an ascii value to match dtk$k codes
*
		ADD 48 TO menu_choice.

	EVALUATE TRUE
	   WHEN num_input_keys IS NOT EQUAL TO 1
*
*		Only single key commands are valid in this menu.  Inform
*		the user that an invalid command was entered and return.
*
		MOVE msg_bad_command TO speak_text_buffer
		PERFORM speak_all_text
		ADD 1 TO illegal_entry_count

	   WHEN menu_choice IS EQUAL TO DTK$K_TRM_ZERO
		MOVE MSG_HELP TO speak_text_buffer
		PERFORM speak_text
		MOVE 0 TO illegal_entry_count

	   WHEN menu_choice IS EQUAL TO DTK$K_TRM_ONE
		MOVE MSG_WEATHER TO speak_text_buffer
		PERFORM speak_text
		MOVE 0 TO illegal_entry_count

	   WHEN menu_choice IS EQUAL TO DTK$K_TRM_TWO
		MOVE MSG_MBTA TO speak_text_buffer
		PERFORM speak_text
		MOVE 0 TO illegal_entry_count

	   WHEN menu_choice IS EQUAL TO DTK$K_TRM_THREE
		MOVE MSG_MORTGAGE TO speak_text_buffer
		PERFORM speak_text
		MOVE 0 TO illegal_entry_count

	   WHEN menu_choice IS EQUAL TO DTK$K_TRM_FOUR
		MOVE MSG_SKI TO SPEAK_TEXT_BUFFER
		PERFORM speak_text
		MOVE 0 TO illegal_entry_count

	   WHEN OTHER
		MOVE msg_bad_command TO speak_text_buffer
		PERFORM speak_text
		ADD 1 TO illegal_entry_count
	END-EVALUATE.

	MOVE 0 TO num_input_keys.
*
*	If user enters 3 illegal/incorrect commands, speak help message
*
	IF illegal_entry_count IS GREATER THAN 2
		MOVE MSG_HELP TO speak_text_buffer
		PERFORM speak_text
		MOVE 0 TO illegal_entry_count.
EXIT_PROCESS_MENU_ENTRY.
*  ----------------------------------------------------------------------
LOAD_DICTIONARY.
*
*  Load the user dictionary with the words and phonemic pronunciations
*  stored in the sequential file specified in the foreign command line
*  invoking the program.  Each line of this file contains the word to be
*  defined in the user dictionary followed by a space, followed by the
*  phonemic pronunciation of the word.  The entry is parsed to find the 
*  start and finnish of the word and it's replacement.  The word and phonemic
*  pronunciation are loaded into the user dictionary. If the load dictionary
*  command fails, the user is notified and processing is terminated.  Returns
*  TRUE if the dictionary is loaded successfully or if no user dictionary file
*  name is specified.  Otherwise, FALSE is returned.
*
		IF dictionary_name(1:1) IS EQUAL TO X"20"
			SET return_status TO SUCCESS
			GO TO exit_load_dictionary
		END-IF.
	OPEN INPUT user-dictionary.
	EVALUATE TRUE
		WHEN file_stat IS EQUAL TO "05"
		WHEN file_stat IS EQUAL TO "97"
*
*			This is not the best way to do this, but these two
*			conditions indicate "FILE NOT OPENED CORRECTLY".
*			98962 is the error number for the RMS - 
*			File Not Found message.  This is used because we
*			need to pass ERROR_LOG the VMS error number.
*
			MOVE 98962 TO error_number
			PERFORM error_log
			STOP RUN
	END-EVALUATE.
*
*	Since there is no timeout associated with the DTK$LOAD_DICTIONARY 
*	command, it is possible for an application to hang waiting for a
*	response after issuing the DTK$LOAD_DICTIONARY command.  Therefore,
*	arm a watchdog timer (6 minutes in this case) to time the loading of
*	the entire user dictionary.  If all the entries are not loaded into 
*	the user dictionary before the timer expires, then assume that
*	something is wrong with the DECtalk module and exit.
*
	MOVE TS6MINUTES TO set_timer_string.
	PERFORM set_timer.
	PERFORM  load-dictionary-entry
		UNTIL no-more-data-remains.
*
*	Entire dictionary has loaded successfully.
*	Cancel the watchdog timer, close the dictionary file and return.
*
	PERFORM cancel_timer.
	CLOSE user-dictionary.
	SET return_status TO SUCCESS.
EXIT_LOAD_DICTIONARY.

*  ----------------------------------------------------------------------
LOAD-DICTIONARY-ENTRY.
	INSPECT entry REPLACING CHARACTERS BY X"00" BEFORE X"00".
	MOVE ZERO TO word-cnt.
	MOVE ZERO TO phonem-cnt.
* 
*	Read in all of the words and substitutions from the sequential
*	file specified in the command string.  The positions of each word
*	are located with the INSPECT command, and then these fields are
*	defined using REFERENCE MODIFICATION in the DTK$LOAD_DICTIONARY routine.
*
	READ user-dictionary
		AT END MOVE 'NO' TO more-data-remains-flag.
	IF more-data-remains
*
*	   This inspect isn't necessary if the words and substitution are
*	   separated by spaces only.
*
	   INSPECT entry REPLACING ALL X"09" BY " " BEFORE X"00"
	   INSPECT entry TALLYING word-cnt FOR CHARACTERS BEFORE " ",
			       phonem-cnt FOR CHARACTERS AFTER " " BEFORE X"00"

	   CALL "DTK$LOAD_DICTIONARY" USING BY REFERENCE  voice_id,
					    BY DESCRIPTOR entry (1:word-cnt),
				  BY DESCRIPTOR entry (word-cnt + 2:phonem-cnt)
				GIVING return_status
	   IF return_status IS EQUAL TO DTK$_TOOLONG OR
	      return_status IS EQUAL TO DTK$_NOROOM
*
*		Dictionary entry too long or no room in user dictionary.
*		These are not generally fatal errors.  However, in this
*		demonstration program, they are treated as such.  In creating
*		a demo program, if words cannot be loaded into the user
*		dictionary, the programmer should be notified so that the
*		appropriate corrections can be made.
*
		CLOSE user-dictionary
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN
	   ELSE IF return_status IS NOT SUCCESS
		CLOSE user-dictionary
		GO TO exit_load_dictionary.

*  ----------------------------------------------------------------------
RESTART.
*
*  DECtalk stopped speaking because it was in autostop keypad mode when
*  it received a Touch Tone Key from the user.  First, send DECtalk a 
*  right square bracket "]" just in case speech was stopped while speaking
*  phonemic text. Then, restart speech (using DTK$SET_SPEECH_MODE) and reset
*  the speaking voice and rate (using DTK$SET_VOICE).
*
	MOVE rsbracket TO speak_text_buffer
	PERFORM speak_text.
	MOVE TS45SECOND TO set_timer_string.
	PERFORM set_timer.
	CALL "DTK$SET_SPEECH_MODE" USING voice_id
					 speech_on 
				   GIVING return_status.
	PERFORM cancel_timer.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.
	PERFORM set_timer.
	CALL "DTK$SET_VOICE" USING  BY REFERENCE voice_id
						 voice
						 speaking_rate
						 comma_pause_duration
						 period_pause_duration
			     GIVING return_status.
	PERFORM cancel_timer.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.
	SET return_status TO SUCCESS.
*  ----------------------------------------------------------------------
END_CALL.
*
*  End the current user session.  Since the DTK$HANGUP_PHONE command
*  does not set a timeout, and it requests DECtalk to send a phone
*  status, a watchdog timer is set to insure that the application does
*  not hang (if DECtalk fails).  If a longer timeout period is needed,
*  adjust the value of the parameter moved into SET_TIMER_STRING before
*  performing "set_timer".  After the watchdog timer is set, speak a goodbye
*  message to the caller and then hangup the phone.  The goodbye message
*  spoken can be changed by modifying the text in "msg_goodbye". 
*
	MOVE TS6MINUTES TO set_timer_string.
	PERFORM set_timer.
	CALL "DTK$HANGUP_PHONE" USING BY REFERENCE voice_id
				      BY DESCRIPTOR msg_goodbye
				GIVING return_status.
	PERFORM cancel_timer.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.

	MOVE 1 TO timeout.
	MOVE TS45SECOND TO set_timer_string.

*  PERFORM FOREVER.
	PERFORM CLEAR_WINK UNTIL ABORT IS EQUAL TO "TRUE".

exit_end_call.

* ----------------- clear winks routine --------------------
CLEAR_WINK.
*
*  This section of code has been added as a workaround for processing
*  WINKS at the end of a phone session.  These will be taken care of
*  by the RTL in later releases.
*
	PERFORM set_timer.
	CALL "DTK$READ_KEYSTROKE" USING BY REFERENCE voice_id,
					BY REFERENCE xx,
						     OMITTED,
					BY REFERENCE timeout
			       GIVING return_status.
	PERFORM cancel_timer.
	IF return_status IS EQUAL TO DTK$_ONHOOK OR
	   return_status IS EQUAL TO SS$_TIMEOUT
		PERFORM set_timer
		CALL "DTK$READ_KEYSTROKE" USING BY REFERENCE voice_id,
						BY REFERENCE xx,
							     OMITTED,
						BY REFERENCE timeout
				       GIVING return_status
		PERFORM cancel_timer
		IF return_status IS NOT SUCCESS
			MOVE return_status TO error_number
			PERFORM error_log
			STOP RUN
		END-IF
		GO TO exit_end_call

	ELSE IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.


*  ----------------------------------------------------------------------
SPEAK_ALL_TEXT.
*
*  Sends the prompt contained in "speak_text_buffer" to the DECtalk to
*  be spoken.  However, before the prompt is sent to the DECtalk, 
*  autostop keypad mode is disabled (if it is enabled) so that it is
*  guarenteed that the user hears the entire prompt.  Once the prompt
*  has been spoken, autostop keypad mode is re-enabled.  
*
	IF keypad_mode IS EQUAL TO DTK$K_KEYPAD_AUTO
		MOVE TS45SECOND TO set_timer_string
		PERFORM set_timer
		CALL "DTK$SET_KEYPAD_MODE" USING BY REFERENCE voice_id
							 keypad_on
			      GIVING return_status
		PERFORM cancel_timer
		IF return_status IS NOT SUCCESS
			MOVE return_status TO error_number
			PERFORM error_log
			STOP RUN.
	PERFORM speak_text.
	IF keypad_mode IS EQUAL TO DTK$K_KEYPAD_AUTO
*
*	If autostop keypad mode was enabled (keymode = DTK$K_KEYPAD_AUTO),
*	then re-enable it.
*
		PERFORM set_timer
		CALL "DTK$SET_KEYPAD_MODE" USING BY REFERENCE voice_id
							keypad_mode
			      GIVING return_status
		PERFORM cancel_timer
		IF return_status IS NOT SUCCESS
			MOVE return_status TO error_number
			PERFORM error_log
			STOP RUN.
	SET return_status TO SUCCESS.

*  ----------------------------------------------------------------------
SPEAK_TEXT.
*
*  Sends the prompt contained in "speak_text_buffer" to the DECtalk to be
*  spoken.  If an error occurs, a warning message is displayed on the
*  console terminal.
*  NOTE: The watchdog timer (set by performing "set_timer") is set to
*	 6 minutes because the longest message in this demo is about 900
*	 characters.  This value should be modified based on the longest
*	 message thats to be spoken.  The timeout should be long enough to
*	 allow ample time to completly finnish speaking all text.
*
	MOVE TS6MINUTES TO set_timer_string
	PERFORM set_timer
	CALL "DTK$SPEAK_TEXT" USING BY REFERENCE voice_id
				    BY DESCRIPTOR speak_text_buffer
				    BY REFERENCE return_status_mode
			      GIVING return_status
	PERFORM cancel_timer.
	IF return_status IS NOT SUCCESS
		MOVE return_status TO error_number
		PERFORM error_log
		STOP RUN.
	SET return_status TO SUCCESS.

*  ----------------------------------------------------------------------
ERROR_LOG.
*
*  Take the error number, input to SYS$GETMSG to get the system message text.
*  Format the message text with the specific terminal line for that application
*  "process" (using SYS$FAO), and call SYS$SENOPR with the error structure.
*
*  NOTE:  the DTK$ errors that occur will have the %DTK- facility name,
*	  but not the error message text, just the error number.  It is
*	  anticipated that these messages will be included in future VMS/RTL
*	  releases.
*
	CALL "SYS$GETMSG" USING BY VALUE	error_number,
				BY REFERENCE	buf_len,
				BY DESCRIPTOR	error_buf,
				BY VALUE	15,
						OMITTED.
*
*	Set message target to OPER11, and the message type to RQ_RQST.
*	For more information on sending messages to an operators terminal
*	see the System Services manual.
*
	MOVE 1073741827 TO opc$type_target

	CALL "SYS$FAO" USING BY DESCRIPTOR	control_str, 
				BY REFERENCE	buf_len,
				BY DESCRIPTOR	error_text,
				BY VALUE	buf_len,
				BY REFERENCE	error_buf,
				BY DESCRIPTOR	on_text,
				BY VALUE	5,
				BY REFERENCE	terminal_line_string.

	CALL "SYS$SNDOPR" USING BY DESCRIPTOR error_msg,
					      OMITTED
			GIVING ret_stat.
	IF ret_stat IS NOT SUCCESS
		STOP RUN.
*  ----------------------------------------------------------------------
SET_TIMER.
*
*  Sets the system (watchdog) timer to expire "sec" seconds in the future
*  by invoking the "SYS$SETIMR" system service.  SYS$BINTIM is passed a 
*  character string in the format "D HH:MM:SS", where D is Days, H is Hours
*  M is minutes etc.  The output of BINTIM is the quadword delta_time ,
*  which gets passed as input to SYS$SETIMR.  
*

	CALL "SYS$BINTIM" USING	BY DESCRIPTOR	set_timer_string,
				BY REFERENCE	delta_time
			GIVING ret_stat.
	IF ret_stat IS NOT SUCCESS
		MOVE ret_stat TO error_number
		PERFORM error_log
		STOP RUN.
	CALL "SYS$SETIMR" USING 	     OMITTED,
				BY REFERENCE delta_time,
				BY VALUE     LIB$SIGNAL,
				BY VALUE     SS$_TIMEOUT
			  GIVING ret_stat.
	IF ret_stat IS NOT SUCCESS
		MOVE ret_stat TO error_number
		PERFORM error_log
		STOP RUN.
*  ----------------------------------------------------------------------
CANCEL_TIMER.
*
*  Cancel the system (watchdog) timer
*

	CALL "SYS$CANTIM" USING BY VALUE SS$_TIMEOUT,
					 OMITTED
			  GIVING ret_stat.
	IF ret_stat IS NOT SUCCESS
		MOVE ret_stat TO error_number
		PERFORM error_log
		STOP RUN.

END PROGRAM DECTALK-DEMO.
