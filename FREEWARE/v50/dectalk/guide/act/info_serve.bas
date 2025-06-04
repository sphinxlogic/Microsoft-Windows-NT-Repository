 FUNCTION LONG info_services()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

  info_help$    = "To hear the current Boston weather forecast, Press 1. "&
		+ "To hear transportation information, Press 2. "&
		+ "To hear mortgage rate information, Press 3. "&
		+ "To hear the current ski conditions, Press 4. "&
		+ "To repeat this message, Press 0. Terminate your "&
		+ "entry with the pound-key.  To exit, Press the star-key. "

  msg_weather$  = "Welcome to the Boston Area Weather Service. April 1st. "&
		+ 'Today will be a day ["]more like early May.  Current '&
		+ "[d'awn*t`awn] Boston temperature is 58 degrees fahrenheit, "&
		+ '14 Celsius. It will be a ["]beautiful sunny day, breezy and'&
		+ " mild, with a high of 76 degrees Fahrenheit. Humidity will "&
		+ "be 76%.  [baeraxm'ehtrixk] pressure is currently 32.5.  "&
		+ "Tomorrow will be cooler, with a high of fifty on the coast "&
		+ "and sixty inland.  Fair weather is expected to continue "&
		+ "throughout the week. "

  msg_MBTA$     = "Welcome to the ['ehm*b`iy*t`iy*'ey] Information Line.  All "&
		+ "['ehm*b`iy*t`iy*'ey] lines are on time.  The B and M line "&
		+ "from the North Shore is running a limited service, with "&
		+ "trains running from ['ihpswihch] and Rockport, at 8 "&
		+ "[`ey*'ehm] and 9 [p`iy*'ehm] only.  There will be no red "&
		+ "line service between Park Street and Harvard, Monday from "&
		+ "9 [p`iy*'ehm] to 1 [`ey*'ehm].  Substitute bus "&
		+ "transportation will be provided. "

  msg_mortgage$ = "Welcome to [hx'aemdaxn] National Bank's Mortgage Line.  "&
		+ "All of the following rates are subject to change.  The "&
		+ "application fee is $250. 30 year rate with 10% [']down are "&
		+ "9.9% with 3 [aen ax] half points, 10.2% with 3 points, "&
		+ "10.5% with 2 [aen ax] half points, 10.8% with no points.  "&
		+ "15 year rates with 10% [']down are 9.9% with 2 [aen ax] "&
		+ "half points, 10.1% with 2 points, and 10.5% with no "&
		+ "points. Adjustable rates are at 8.5% fixed for 3 years "&
		+ "with 2% a year and 6% lifetime caps thereafter."

  msg_ski$      = "Welcome to the New England Ski Report. January 31st. Skiing"&
		+ " is excellent in Vermont because of [y'ehstixdxeyz] snowfall"&
		+ "of 10 inches. All ski trails are open at Stowe, Sugar-"&
		+ "bush, Mt. Snow, Stratton, Jay Peak, and [sm'ahglrrz] Notch,"&
		+ " with packed powder and full snow-making in operation. All"&
		+ " cross country ski trails are also open, with many groom"&
		+ "ed trails.  Skiing in Maine and [nuw] Hampshire is good, "&
		+ "with cold temperatures allowing for constant snow-making. "&
		+ "Gun-stock has 15 [aatax] 25 trails open, Mt. Cranmore has"&
		+ " 15 [aatax] 20 trails open, ['aetixtaesh] and Wildcat have "&
		+ "all trails open.  Sugar-loaf and Sunday River also have all"&
		+ " trails open, with packed powder and loose [gr'aenyxaxlrr] "&
		+ "conditions. Expected snowfall in Maine and [nuw] Hampshire "&
		+ "should improve weekend ski conditions."

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
		info_services = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		info_services = 1%
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
				CALL speak_text(DTK$K_WAIT, info_help$)
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
		info_services = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		info_services = 1%
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
				info_services = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   info_services = 0%
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
			CALL speak_text(DTK$K_WAIT, info_help$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_ONE			! Boston weather forecast	
			CALL speak_text(DTK$K_WAIT, msg_weather$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_TWO			! transportation info	
			CALL speak_text(DTK$K_WAIT, msg_MBTA$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_THREE			! mortgage rate info	
			CALL speak_text(DTK$K_WAIT, msg_mortgage$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_FOUR			! ski condition info	
			CALL speak_text(DTK$K_WAIT, msg_ski$)
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
		CALL speak_text(DTK$K_WAIT, info_help$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION

