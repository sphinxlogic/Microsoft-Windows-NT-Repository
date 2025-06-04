 !  ----------------------------  DIST.BAS  ------------------------
 !  This subroutine contains the distribution services section of the ACT demo.
 !  The menu choices are:
 !
 !	1) ORDER ENTRY
 !	2) STOCK STATUS
 !	3) CHECK OUTSTANDING ORDERS
 !	9) CUSTOMER SERVICE REP.
 !
 FUNCTION LONG distribution()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	DTK$SET_VOICE, &
				get_key_string, &
				order_entry, &
				stock, &
				order_status

  COMMON (MESSAGE_TEXT) STRING  msg_access   = 100, &
				msg_invalid  = 40, &
				msg_bad_command   = 40, &
				msg_no_terminator  = 100, &
				msg_timeout  = 50, &
				customer_service = 160

  dist_welcome$ = "[+] Welcome to the DECtalk distribution demonstration. "

  cust_id$ = "Please enter your customer I.D. number followed by the "&
	+ "pound-key.  You may enter any number as a test. "

  dist_menu$ = "For order entry, press 1. "&
	+ "For stock status, press 2. "&
	+ "To check outstanding orders, press 3. "&
	+ "To speak to a customer service representative, press 9. "&
	+ "To exit the distribution demonstration, press the star-key. "&
	+ "Terminate your entry with the pound-key. To repeat this message "&
	+ "press 0. "

  smith$ = "Smith Manufacturing Company. "
 
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
	illegal_entry_count = 0%

	CALL speak_text(DTK$K_WAIT, dist_welcome$)

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string( cust_id$, T20SECOND, terminator_value)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		distribution = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		distribution = 1%
		EXIT FUNCTION
	END IF

	num_input_keys = 0%
	input_key_buf(0) = 0%
	stat% = get_key_string( msg_access, T20SECOND, terminator_value)
	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		distribution = 0%
		EXIT FUNCTION
	END IF

	CALL speak_text(DTK$K_WAIT, smith$)

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
		distribution = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		distribution = 1%
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
				CALL speak_text(DTK$K_WAIT, dist_menu$)
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
		distribution = 0%
		EXIT FUNCTION
	END IF
	IF terminator_value = DTK$K_TRM_ASTERISK
	THEN
		distribution = 1%
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
				distribution = 0%
				EXIT FUNCTION
			ELSE
				GOSUB  process_menu_entry
			END IF
		END IF
	END IF
   NEXT
   distribution = 0%
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
			CALL speak_text(DTK$K_WAIT, dist_menu$)
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_ONE			! order entry
			stat% = order_entry
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				distribution = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_TWO			! stock status
			stat% = stock
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				distribution = 0%
				EXIT FUNCTION
			END IF
			illegal_entry_count = 0%
		CASE = 	DTK$K_TRM_THREE			! order status
			stat% = order_status
			IF (stat% AND STS$M_SUCCESS) = 0%
			THEN
				distribution = 0%
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
		CALL speak_text(DTK$K_WAIT, dist_menu$)
		illegal_entry_count = 0%
	END IF
	RETURN

 END FUNCTION			! end distribution

 ! ----------------------------- order entry --------------------------
 !
 FUNCTION LONG order_entry()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

	part_no$ = "Please enter the part number, followed by the pound-key. "&
		+ "You may enter any number up to 6 digits. "
	rivets$ = "Part Number              steel rivets. "
	quantity$ = "Please enter the quantity, followed by the pound-key. "&
		+ "You may enter any number up to 6 digits. "

	canceled$ = "Order request canceled. "
	reserved$ = "             steel rivets have been reserved for the "&
		+ "Smith Manufacturing Company. "

	shipped$ = "Your order will be shipped today, FOB [nuw] York. "&
		+ "Standard terms will apply. [:nh +] If the item had been "&
		+ "out of stock or back-ordered, DECtalk [wuhdax<60>v] "&
		+ "informed the caller. "

	too_long$ = "entry too long. "

  DECLARE	LONG	terminator_value, illegal_entry_count
 	null_char$ = X'00'C
	illegal_entry_count = 0%

 get_part_no:
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(part_no$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		order_entry = 0%
		EXIT FUNCTION
	END IF

	IF num_input_keys > 6%
	THEN
		CALL speak_text(DTK$K_WAIT, too_long$)
		GO TO get_part_no
	END IF

	i% = 13%
	FOR j% = 0% TO (num_input_keys - 1)
		MID$(rivets$, i%, 1%) = CHR$(input_key_buf(j%))
		i% = i% + 2%
	NEXT j%
	MID$(rivets$, i%-1%, 1%) = ","

	CALL speak_text(DTK$K_WAIT, rivets$)

 get_quantity:
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(quantity$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		order_entry = 0%
		EXIT FUNCTION
	END IF

	IF num_input_keys > 6%
	THEN
		CALL speak_text(DTK$K_WAIT, too_long$)
		GO TO get_quantity
	END IF

	IF ((num_input_keys = 0%) OR (terminator_value = DTK$K_TRM_ASTERISK))
	THEN
		CALL speak_text(DTK$K_WAIT, canceled$)
	ELSE
		FOR i% = 0% TO (num_input_keys - 1)
			MID$(reserved$, i%+1, 1%) = CHR$(input_key_buf(i%))
		NEXT i%

		CALL speak_text(DTK$K_WAIT, reserved$)
		CALL speak_text(DTK$K_WAIT, shipped$)
	END IF

	order_entry = 1%
 END FUNCTION			! end order_entry

 ! ---------------------  stock status ----------------------

 FUNCTION LONG stock()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)


	part_no$ = "Please enter the part number, followed by the pound-key. "&
		+ "You may enter any number up to 6 digits. "
	back_order$ = "part number              is currently back-ordered. "

	new_ship$ = "A new shipment is expected in 7 days. [:nh +] If the "&
	+ "parts had been in stock, DECtalk [wuhdax<60>v] said how many were "&
	+ "in stock. "

	too_long$ = "entry too long. "

  DECLARE	LONG	terminator_value, illegal_entry_count
 	null_char$ = X'00'C
	illegal_entry_count = 0%

 get_part:
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(part_no$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		stock = 0%
		EXIT FUNCTION
	END IF

	IF num_input_keys > 6%
	THEN
		CALL speak_text(DTK$K_WAIT, too_long$)
		GO TO get_part
	END IF

	i% = 13%
	FOR j% = 0% TO num_input_keys - 1
		MID$(back_order$, i%, 1%) = CHR$(input_key_buf(j%))
		i% = i% + 2%
	NEXT j%

	CALL speak_text(DTK$K_WAIT, back_order$)
	CALL speak_text(DTK$K_WAIT, new_ship$)

	stock = 1%	
 END FUNCTION			! end stock

 ! ---------------------  order status ----------------------

 FUNCTION LONG order_status()

 %INCLUDE "DTKDEF"

  EXTERNAL LONG CONSTANT STS$M_SUCCESS
  EXTERNAL LONG FUNCTION	get_key_string

  COMMON (constants) STRING time_45_sec, time_90_sec, time_6_min, time_16_min, &
		     LONG	MAX_ENTRY, T1SECOND, T2SECOND, T20SECOND

  COMMON (application_specific)	LONG	voice_id, &
					num_hold_keys, &
					num_input_keys, &
				BYTE	hold_key_buf(80), &
					input_key_buf(80)

	order_no$ = "Please enter the order number, followed by the pound-key."&
	+ " You may enter any number up to 6 digits. "

	shipped$ = "Order number              was shipped "

	order_status$ = "[:nh +] If the order had not been shipped, DECtalk "&
	+ "[wuhdax<60>v] informed the caller of the expected shipping date, "&
	+ "or any problems with the order. "

	too_long$ = "entry too long. "

  DECLARE	LONG	terminator_value, illegal_entry_count
 	null_char$ = X'00'C
	illegal_entry_count = 0%

 get_order:
	num_input_keys = 0%
	input_key_buf(0) = 0%
        stat% = get_key_string(order_no$, T20SECOND, terminator_value)

	IF (stat% AND STS$M_SUCCESS) = 0%
	THEN
		order_status = 0%
		EXIT FUNCTION
	END IF

	IF num_input_keys > 6%
	THEN
		CALL speak_text(DTK$K_WAIT, too_long$)
		GO TO get_order
	END IF

	i% = 14%
	FOR j% = 0% TO num_input_keys - 1
		MID$(shipped$, i%, 1%) = CHR$(input_key_buf(j%))
		i% = i% + 2%
	NEXT j%

 ! 493-TALK used the date of 3 days ago, but i can't easily do that in basic.
 ! The date used is todays date.

	shipped$ = shipped$ + date$(0)

	CALL speak_text(DTK$K_WAIT, shipped$)
	CALL speak_text(DTK$K_WAIT, order_status$)

	order_status = 1%
 END FUNCTION
