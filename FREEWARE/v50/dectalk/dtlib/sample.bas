10	EXTEND
20	!*							&
	!* DECtalk function library and a sample application.	&
	!* The function library generally duplicates the C	&
	!* library, with some minor simplifications.		&
	!*							&
	!* The sample program reads a string of numbers from	&
	!* the keypad and speaks them as a number, and as a	&
	!* string of digits.  The '*' key functions as a dollar	&
	!* sign, and the '#' key functions as a decimal point.	&
	!* The program also illustrates how an application	&
	!* might manage keypad timeouts.			&
	!*							&

100	!*							&
	!* Defaults						&
	!*							&
	DEF.kb$ = "KB2:"		! DECtalk device	&
	\ DEF.log$ = "yes"		! Assume log?		&

1000	!*							&
	!* Main program						&
	!*							&
	! Initialize DECtalk and start the DEMO			&
	! Channel 1	Console keyboard for parameters		&
	! Channel 2	Log file				&

1010	open "kb:" for input as file 1%				&
	\ kb$ = FNprompt$("Dectalk terminal", DEF.kb$)		&
	\ DT.log% = (FNyesno%("Enable logging", DEF.log$))	&
	\ debug% = FNyesno%("Enable debug printouts", "yes")	&
	\ retries%, ncalls% = 0%	! Clear counters	&
	\ error.count% = 0%		! No errors yet		&
	\ if (debug% or DT.log%) then	! Need a log file.	&
	    logfile$ = FNprompt$("Debug log file", "kb:")	&
	    \ open logfile$ for output as file 2%		&

1100	while (FNinit%(kb$))		! Initialize DECtalk	&
	    \ q% = FNlog%("Initialization")			&
	    \ retries% = retries% + 1%	! Count initializations	&
	    \ while (FNanswer%)		! Answer the phone	&
		\ if (FNprocess%) then	! Do this call		&
		    ncalls% = ncalls% + 1%	! Got a call	&
		    \ retries% = 0%		! Clear retry	&

1200		goto 1800 if (debug% and error.count% > 0%)	&
		\ if (retries% > 2%) then	! Trouble?	&
		    q% = FNlog%("Too many retries")		&
		    \ goto 1800			! Fatal.	&

1300	    next			! For all calls		&
	\ next				! For all restarts	&

1800	q% = FNlog%("finished after " + num1$(ncalls%))		&
	\ close 2% if DT.log%					&

1900	goto 32767			! All done		&

2000	def* FNprocess%						&
	!							&
	!	F N p r o c e s s %				&
	!							&
	! User process.  Read a number from the keypad and	&
	! speak it out.  Return when phone is to be hung up.	&
	! Return TRUE% if ok, FALSE% on error.			&
	!							&

2010	FNprocess% = FALSE%		! Assume failure	&
	\ nkeys% = 0%			! Count button presses	&
	\ q% = FNlog%("answered")				&
	\ q% = FNspeak%("[:np :ra 180] Welcome to DECtalk.")	&
	\ q% = FNspeak%("It is now " + time$(0%)		&
		+ " on " + date$(0%) + ".")			&
	\ q% = FNspeak%("Enter a number, the star key means")	&
	\ q% = FNspeak%("dollar sign, while the number-sign")	&
	\ q% = FNspeak%("key means decimal point.")		&
	\ if (not FNphone%("20")) then	! turn the keypad on	&
	    q% = FNlog%("error enabling keypad")		&
	    \ goto 2080			! Error exit		&

2020	if (not FNptest%(R3.PH.OFFHOOK%)) then			&
	    q% = FNlog%("enable keypad, state: " + num1$(R3%))	&
	    \ goto 2080			! Error exit		&

2030	while TRUE%			! For all numbers	&
	    \ timer% = 10%		! For first character	&
	    \ work$ = ""		! Input buffer		&
	    \ while TRUE%		! Get the number	&
		\ c% = FNkey%(timer%)	! Read a character	&
		\ c$ = chr$(c%)		! Get both flavors	&
		\ goto 2080 if c$ = 'H'	! Hangup		&
		\ goto 2080 if c$ = 'E'	! Error from RSTS/E	&
		\ goto 2080 if c$ = 'X'	! Escape sequence error	&
		\ goto 2050 if c$ = 'T'	! Timeout		&
		\ c$ = '$'  if c$ = '*'	! Fix funny		&
		\ c$ = '.'  if c$ = '#'	! buttons		&
		\ work$ = work$ + c$	! Stuff it		&
		\ timer% = 2%		! Short prompt now	&
	    \ next			! Read a number loop	&

2050	    goto 2060 if (work$ = "")	! Did we read anything?	&
	    \ q% = FNspeak%("You entered " + work$ + ",")	&
	    \ q% = FNspeak%("that is" + FNexpand$(work$) + ".")	&
	\ next				! Read all numbers	&

2060	FNprocess% = TRUE%		! Normal completion	&

2080	q% = FNphone%("21")		! Turn off keypad	&
	\ q% = FNhangup%		! And hang up the phone	&
	\ q% = FNlog%("process exit after " + num1$(nkeys%))	&

2090	fnend							&

3000	!*							&
	!*		F N e x p a n d $ ( t e x t $ )		&
	!*							&
	!* Expand a number string into its component bytes.	&
	!* Note that this would be useful in a "bank by phone"	&
	!* application to speak a number, digit by digit, so	&
	!* the caller could copy it down.  If the input is	&
	!* "12.3", the output will be " 1 2 point 3".  Note	&
	!* the leading blank.					&
	!*							&
	def* FNexpand$(text$)					&
	\ q$ = ""				! Output work	&
	\ for q% = 1% to len(text$)		! For each byte	&
	    \ q1$ = mid(text$, q%, 1%)		! Locate it	&
	    \ q1$ = "point" if q1$ = '.'	! Fix the	&
	    \ q1$ = "minus" if q1$ = '-'	!   special	&
	    \ q1$ = "dollar sign" if q1$ = '$'	!      cases	&
	    \ q$ = q$ + " " + q1$		! and stuff it	&
	\ next q%				! Do 'em all	&
	\ FNexpand$ = q$			! That's it	&

3090	fnend							&

10000	!*							&
	!* Basic-Plus Support functions for DECtalk		&
	!* Note that the code is not particularly fast and some	&
	!* of the error conditions that are handled by the C	&
	!* version of the Escape Sequence parser are ignored.	&
	!*							&
	! Note: the following channels are used:		&
	!	 8	DECtalk input				&
	!	 9	DECtalk	output				&
	!	 2	Log file 				&
	!		If DT.log% is TRUE, a log file is open	&
	!		on channel 2				&
	!							&
	!*							&
	!* Application programs call the following routines	&
	!*							&
	! FNinit%(kb$)		Initialize DECtalk on kb:	&
	! FNanswer%		Finish last call, answer next	&
	! FNhangup%		Hangup the call			&
	! FNkey%(timeout%)	Read a character with timeout	&
	!			Returns the character, or	&
	!			  E	Error (from RSTS)	&
	!			  H	Phone hung up		&
	!			  T	Timeout			&
	!			  X	Bad Escape sequence	&
	! FNtimeout%(sec%)	Set specified timeout, 0 = none	&
	! FNtest%(R2%, R3%)	Test current reply, true if ok	&
	!			  R3% is -1 to ignore it.	&
	!			  FNtest%() checks character,	&
	!			  intermediates, and finals.	&
	! FNptest%(R3%)		Test phone reply (R2% checked)	&
	! FNsend%(text$)	Send text to DECtalk.		&
	! FNspeak%(text$)	Send text followed by <CR><LF>	&
	! FNlog%(text$)		Log text message 		&
	! FNvisible$(char%)	Make character printable for	&
	!			  logging and debugging msgs.	&
	! FNmessage%(text$, R2%, R3%)				&
	!			Send DCS seq. test reply.	&
	!			  text is "P2;P3...",		&
	!			  return TRUE if ok.		&
	!			  Note: FNmessage%() ignores	&
	!			  R3.PH.TIMEOUT replies.	&
	! FNphone%(text$)	Send DCS sequence, test reply	&
	!			  text is "P3;P4..."		&
	!			  R2% must be R2.PHONE%		&
	!			  R3% not tested		&
	!			  FNfunny% called if error	&
	!			  returns as FNmessage%()	&
	! FNfunny%(text$)	Print bad sequence on the log	&
	! FNdump%(text$)	Dump the current reply		&

10010	!*							&
	!* The application program generally doesn't call the	&
	!* following routines.					&
	!*							&
	! FNsave%(char)		Save type-ahead character,	&
	!			  return TRUE if saved.		&
	! FNdcs%(text$)		Send DECtalk DCS message.	&
	!			  text is "P2;P3...",		&
	! FNcsi%(text$)		Send DECtalk CSI message.	&
	!			  text has parm, inter, final.	&
	! FNfromdectalk%(time%)	Read key or escape sequence.	&
	! FNgetseq%(time%)	Read key or escape sequence.	&
	! FNget%(timeout%)	Read one character.		&
	!			  parity is stripped.		&
	!			  <NUL> and <DEL> are ignored.	&
	!			  Return 0% on timeout.		&
	!			  Other errors are fatal.	&
	!			NOTE: do not use fnget%() to	&
	!			read from the telephone keypad.	&
	! FNread%(timeout%)	Read a record from DECtalk	&
	!							&
	!*							&
	!* Globals:						&
	!*							&
	! R1%, R2%, R3%	current reply parameters		&
	!		set by FNgetsequence%()			&
	! DT.timeout%	TRUE if keypad timeouts are		&
	!		currently enabled.			&
	! error.count%	Incremented on serious errors		&
	! ESC%		ESC character (parity bit cleared)	&
	! CAN%		CTRL/U character (cancel sequence)	&
	! SUB%		CTRL/Z character			&
	! CSI%		CSI character				&
	! DCS%		DCS character				&
	! ST%		ST character				&
	! ESC$		An escape to send chr$(155%)		&
	! CRLF$		Carriage-return, Line-feed		&
	! VT$		Vertical Tab (DECtalk flush)		&
	! R2.PHONE%	  R2% phone reply			&
	! R3.PH.ONHOOK%	  R3% (phone hung up)			&
	! R3.PH.OFFHOOK%  R3% (phone is alive)			&
	! R3.PH.TIMEOUT%  R3% (keypad timeout)			&
	!							&
	! DT.anything	reserved for local buffers		&
	! SEQ.anything	reserved for sequence parser		&
	! q[anything]	general temporaries			&
	!							&

10100	def* FNinit%(kb$)					&
	!							&
	! F N i n i t % ( k b $ )				&
	!							&
	! Initialize the DECtalk device				&
	! Return TRUE% if ok, FALSE% if error			&
	!							&

10110	!*							&
	!* Open the terminal in "binary" mode.			&
	!* Then initialize all constants.			&
	!*							&
	open kb$ for input as file 8%, mode 32%+16%+4%+1%	&
	\ open kb$ for input as file 9%, mode 32%++16%+4%+1%	&
	\ DT.incount%, DT.inend% = 0%	! Clear input buffer	&
	\ SEQ.state% = 0%		! Clear input state	&
	\ DIM DT.p%(3), SEQ.p%(3)	! 3 parameters		&
	\ DIM q%(256)			! For debugging		&
	\ TRUE% = (1% = 1%)		! TRUE			&
	\ FALSE% = not TRUE%		! FALSE			&
	\ ESC% = 27%			! Escape		&
	\ ESC$ = chr$(ESC% + 128%)	! Define escape char	&
	\ VT$ = chr$(ascii('K') - 64%)	! DECtalk flush char	&
	\ CRLF$ = chr$(13%) + chr$(10%)	! <CR><LF> string	&
	\ CAN% = ascii('U') - 64%	! CANcel (CTRL/U)	&
	\ SUB% = ascii('Z') - 64%	! SUBstitute (CTRL/Z)	&
	\ CSI% = ascii('[') - 64% + 128%	! Define 	&
	\ DCS% = ascii('P') - 64% + 128%	! C1 control	&
	\ ST%  = ascii('\') - 64% + 128%	! characters	&
	\ R2.PHONE% = 70%					&
	\ R3.PH.ONHOOK% = 0%					&
	\ R3.PH.OFFHOOK% = 1%					&
	\ R3.PH.TIMEOUT% = 2%					&

10120	q% = FNsend%(chr$(ascii('Q') - 64% + 128%)) ! CTRL/Q	&
	\ q% = FNget%(2%) while (q% > 0%) ! Drain text		&
	\ q% = FNdcs%("82")		! No local->host	&
	\ q% = FNcsi%("c")		! "Who are you"		&
	\ q% = FNfromdectalk%(5%)	! Read escape sequence	&
	\ if (DT.char%    <> CSI%	! Check			&
	   or DT.final$   <> 'c'	!   for			&
	   or DT.private$ <> '?'	!     DECtalk		&
	   or R1%         <> 19%) then	!	reply		&
	    q% = FNfunny%("initialization")			&
	    \ FNinit% = FALSE%		! Return failure	&
	    \ goto 10190		! from FNinit%()	&

10130	q% = FNsend%(ESC$ + "!p")	! Soft Terminal Reset	&
	\ q% = FNdcs%("80;1")		! Set MODE SQUARE	&
	\ DT.timeout% = 0%		! No timeouts now	&
	\ FNinit% = TRUE%		! Return TRUE		&

10190	fnend							&

10200	def* FNanswer%						&
	!							&
	!	F N a n s w e r %				&
	!							&
	! Finish off any current call (hanging up the phone)	&
	! Then setup and answer the next call.			&
	! Return TRUE% if the call was answered.		&
	! Return FALSE% if there's serious problems.		&
	!							&

10210	FNanswer% = FALSE%			! Assume error	&
	\ q% = FNget%(2%) while (q% > 0%)	! Drain text	&
	\ goto 10290 if (not FNphone%(""))	! poll status	&
	\ if (R3% = R3.PH.OFFHOOK%) then	! if alive,	&
	    goto 10290 if (not FNhangup%)	! hangup phone	&

10220	if (R3% <> R3.PH.ONHOOK%) then		! still alive?	&
	    q% = FNfunny%("hangup/poll")	! Urk.		&
	    \ goto 10290			! exit this	&

10230	goto 10290 if (not FNphone%("10;1"))	! answer 1 ring	&
	\ if (R3% <> R3.PH.ONHOOK%) then	! ok?		&
	    q% = FNfunny%("enable answer")	! Urk.		&
	    \ goto 10290			! exit this	&

10240	q% = FNfromdectalk%(0%)			! wait for ring	&
	\ if (q% <> DCS%) then			! ok?		&
	    q% = FNfunny%("waiting for ring")	! oops.		&
	    \ goto 10290			! exit this	&

10250	if (not FNptest%(R3.PH.OFFHOOK%)) then			&
	    q% = FNfunny%("expecting offhook")			&
	    \ goto 10290					&

10260	DT.timeout% = 0%		! No timeouts now	&
	\ DT.pending$ = ""		! Nothing pending now	&
	\ FNanswer% = TRUE%		! ok.			&

10290	fnend							&

10300	def* FNtimeout%(seconds%)				&
	!							&
	!	F N t i m e o u t % ( s e c o n d s % )		&
	!							&
	! Enable or disable keypad timeout.  Note that		&
	! FNtimeout%(non-zero%) will examine the state of the	&
	! type-ahead buffer before actually enabling timeouts	&
	!							&

10310	if (seconds% > 0%) then					&
	    seconds% = 0% if (len(DT.pending$) > 0%)		&
	    \ if (seconds% > 0%) then				&
		q% = FNsync%		! make sure all heard	&
		\ seconds% = 0% if (len(DT.pending$) > 0%)	&
	    ! If the program requests that timeouts be turned	&
	    ! on, perform some special checks that the user	&
	    ! hasn't already entered any text (which would be	&
	    ! stored in one of the type-ahead buffers.  If	&
	    ! something is pending, turn timeouts off.  This is	&
	    ! needed because RSTS allows a program to run even	&
	    ! if all output has not been sent to the device.	&

10320	goto 10390 if (seconds% = DT.timeout%)	! Don't resend	&
	\ print #2%, "timeouts set "; seconds% if (DT.log%)	&
	\ q% = FNphone%("30;" + num1$(seconds%))		&

10330	DT.timeout% = seconds%		! save timeout state	&
	\ if (not FNptest%(R3.PH.OFFHOOK%)) then		&
	    q% = FNfunny%("timeout")				&

10390	fnend							&

10400	def* FNsync%						&
	!							&
	!	F N s y n c %					&
	!							&
	! Synchronize with DECtalk.  This function returns	&
	! when all text sent to DECtalk has been spoken.	&
	! Warning: if you have sent much text to DECtalk and	&
	! the moon is in the wrong phase, there is a very	&
	! slight chance that this code could get an operating	&
	! system timeout, even though there are no errors.	&
	!							&

10410	q% = FNsend%(VT$)		! Flush speech		&
	\ q% = FNdcs%("11")		! Send sync		&
	\ if (not FNmessage%("22", 32, -1)) then		&
	    q% = FNfunny%("sync")				&

10490	fnend							&

10500	def* FNhangup%						&
	!							&
	!	F N h a n g u p %				&
	!							&
	! Hangup the telephone.  Returns when the phone is	&
	! properly on-hook (TRUE%) or an error is detected.	&
	!							&

10510	FNhangup% = FALSE%		! Assume problems	&
	\ goto 10590 if (not FNphone%("11"))	! send hangup	&
	\ while (R3% = R3.PH.OFFHOOK%)	! wait until		&
	    \ sleep 5%			! it's hung up		&
	    \ goto 10590 if (not FNphone%(""))			&
	\ next				! loop forever		&
	\ FNhangup% = TRUE%		! OK now.		&

10590	fnend							&

10600	def* FNphone%(text$)					&
	!							&
	!	F N p h o n e % ( t e x t $ )			&
	!							&
	! Send a phone message, return the FNmessage% code.	&
	! You should then call FNtest% to see just what the	&
	! phone state actually is.				&
	!							&

10610	if (text$ <> "")		! If extra parameters	&
	    then text$ = "60;" + text$	! tack them on, else	&
	    else text$ = "60"		! just do status report	&

10620	FNphone% = FNmessage%(text$, R2.PHONE%, -1%)		&

10690	fnend							&

10700	def* FNsave%(char%)					&
	!							&
	!	F N s a v e % ( c h a r % )			&
	!							&
	! If the char% came from a user data entry, save it in	&
	! the DT.pending$ buffer and return TRUE%, otherwise,	&
	! return FALSE%.  Note that FNsave%() watches for	&
	! asynchronous keypad timeouts.				&
	!							&
	! Note that unreasonable amounts of type-ahead may	&
	! cause the program to overflow memory.			&
	!							&

10710	FNsave% = TRUE%						&
	\ if FNptest%(R3.PH.TIMEOUT%) then	! Timeout?	&
	    goto 10790 if (DT.timeout% = 0%)	! Disabled?	&
	    \ DT.timeout% = 0%			! None now	&
	    \ goto 10790 if (len(DT.pending$) > 0%)		&
	    \ char% = ascii('T')		! Save 'T'	&

10720	if (instr(0%, "0123456789*#ABCDT", chr$(char%)) = 0%)	&
	    then FNsave% = FALSE%				&
	    else DT.pending$ = DT.pending$ + chr$(char%)	&

10790	fnend							&

10800	def* FNkey%(timeout%)					&
	!							&
	!	F N k e y % ( t i m e o u t % )			&
	!							&
	! Read a keypad character (in there is one in the	&
	! type-ahead buffer, or read a character or escape	&
	! sequence from DECtalk.  The timeout% parameter is	&
	! non-zero to enable timeouts.				&
	!							&
	! Note that the timeout parameter, if non-zero, will be	&
	! extended to compensate for RSTS/E output buffering.	&
	!							&
	! FNkey% ignores user timeout if timeout was disabled.	&
	!							&

10810	q% = FNtimeout%(timeout%)	! Set/clear timeouts	&
	\ if (len(DT.pending$) > 0%) then			&
	    FNkey% = ascii(DT.pending$)				&
	    \ DT.pending$ = right(DT.pending$, 2%)		&
	    \ goto 10890					&

10820	timeout% = (timeout% * 4%) + 60% if timeout% > 0%	&
	\ q% = FNfromdectalk%(timeout%)				&
	\ q% = ascii('T')	if FNptest%(R3.PH.TIMEOUT%)	&
	\ DT.timeout% = 0%	if (q% = ascii('T'))		&
	\ q% = ascii('H')	if FNptest%(R3.PH.ONHOOK%)	&
	\ q% = ascii('E')	if (q% <= 0%)	! O.S. error	&
	\ FNkey% = q%						&

10890	fnend							&

12000	def* FNmessage%(text$, t2%, t3%)			&
	!							&
	! F N m e s s a g e $ ( t e x t $ ,   t 2 % ,   t 3 %)	&
	!							&
	! Send a DECtalk DCS sequence to DECtalk and wait 	&
	! for a reply.  Make sure the reply matches the t2% 	&
	! and t3% parameters.  Return TRUE% if ok, else FALSE%.	&
	!							&
	! A keypad timeout (escape sequence) may be read when	&
	! we are expecting some other reply.  In this case,	&
	! the timeout is ignored, the timeout status  flag is	&
	! set FALSE and we read another sequence.		&
	!							&

12010	q% = FNdcs%(text$)		! Send the sequence	&
	\ FNmessage% = TRUE%		! Assume success	&

12020	q% = FNfromdectalk%(60%)	! get something		&
	\ goto 12020 if (q% = ST%)	! ignore string term.	&
	\ goto 12020 if FNsave%(q%)	! save type-ahead	&
	\ if not (FNtest%(t2%, t3%)) then	! Check seq.	&
	    q% = FNfunny%("message test error")			&
	    \ FNmessage% = FALSE%				&

12090	fnend							&

12100	def* FNfromdectalk%(timeout%)				&
	!							&
	!	F N f r o m d e c t a l k % ( t i m e o u t % )	&
	!							&
	! Read an escape sequence or keypad character.  Dump	&
	! junk between DCS final and string terminator.		&
	!							&

12110	if (SEQ.state% <> 0% and SEQ.state% <> ST%) then	&
	    gosub 12200			! Grab the sequence	&
	    \ goto 12180		! And return char value	&

12120	SEQ.state% = 0%			! Nothing pending now	&
	\ q% = FNgetsequence%(timeout%)	! Get something		&
	\ gosub 12200			! Make it current	&
	\ q% = FNtoss% if (q% = DCS%)	! Toss junk until ST	&

12180	FNfromdectalk% = DT.char%	! Return character 	&

12190	fnend							&

12200	!							&
	! Subroutine called from FNfromdectalk% to copy the	&
	! last escape sequence read into the "current sequence"	&
	! buffer.  This is needed to skip over junk between	&
	! the DCS final and the string terminator.		&
	!							&

12210	DT.char% = SEQ.char%		! Sequence type		&
	\ DT.final$ = SEQ.final$	! Sequence terminator	&
	\ DT.private$ = SEQ.private$	! private characters	&
	\ DT.inter$ = SEQ.inter$	! Intermediates		&
	\ DT.parm% = SEQ.parm%		! Parameter count	&
	\ R1%, DT.p%(1%) = SEQ.p%(1%)	! Param's		&
	\ R2%, DT.p%(2%) = SEQ.p%(2%)	!			&
	\ R3%, DT.p%(3%) = SEQ.p%(3%)	!			&
	! \ print #2%,  dt.parm%; "parms: "; R1%; R2%; R3%	&
	
12290	return

12300	def* FNtoss%						&
	!							&
	!	F N t o s s %					&
	!							&
	! Called after reading a DCS, this function reads	&
	! text to the terminating string terminator.		&
	!							&

12310	SEQ.state% = 0%						&

12320	q% = FNgetsequence%(5%)					&
	\ if (q% <= 0% or (q% >= 128% and q% <= 159%))		&
	    then FNtoss% = q%					&
	    else goto 12320					&

12390	fnend							&

13000	def* FNgetsequence%(timeout%)				&
	!							&
	!	F N g e t s e q u e n c e % ( t i m e o u t % )	&
	!							&
	! Read the next character or the next ANSI standard	&
	! Escape Sequence.					&
	!							&
	! Initialize by setting SEQ.state% to zero.  Returns:	&
	! SEQ.state%		sequence final character	&
	!							&
	! Note the following goto's:				&
	! goto 13010 to read the next character in a sequence.	&
	! goto 13020 to continue processing (needed when escape	&
	!		followed by a second character turns	&
	!		into a C1 control character).		&
	! goto 13120 to exit an ESC sequence			&
	! goto 13130 to exit after reading a DCS/CSI sequence.	&
	! goto 13140 to exit a C0 control within a sequence.	&
	!							&
	! The following is set by this module:			&
	! SEQ.char%	the character or sequence type		&
	! SEQ.final$	the sequence final for CSI/DCS/ESC	&
	! SEQ.state%	zero when sequence ends.		&
	! SEQ.parm%	number of parameters			&
	! SEQ.p%[]	each parameter as read			&
	! SEQ.inter$	intermediates				&
	! SEQ.private$	private introducer, 'X' if error seen	&
	!							&

13010	DT.c% = fnget%(timeout%)	! Get a character	&

13020	if (DT.c% = ESC%		! If the character	&
	 or DT.c% = CSI%		! introduces a new	&
	 or DT.c% = DCS%) then		! sequence, initialize	&
	    SEQ.state% = DT.c%		! all work areas.	&
	    ! \ print #2%,  "seq start: "; fnvisible$(dt.c%)	&
	    \ SEQ.inter$ = ""					&
	    \ SEQ.private$ = ""					&
	    \ SEQ.parm% = 0%					&
	    \ SEQ.p%(1%), SEQ.p%(2%), SEQ.p%(3%) = 0%		&
	    \ goto 13010		! go read another byte	&

13030	goto 13140 if (SEQ.state% = 0%) ! done if no sequence	&
	!							&
	! Continue processing the current sequence		&
	!							&
	\ if ((DT.c% >= 128% and DT.c% < 160%)	! C1 control	&
	   or (DT.c% = CAN%)		! or CTRL/U		&
	   or (DT.c% = SUB%)) then	! or CTRL/Z		&
	    SEQ.state% = 0%		! force sequence exit	&
	    ! \ print #2%, "c0 control: "; fnvisible$(dt.c%)	&
	    \ goto 13140		! and return C0 control	&

13040	goto 13140 if (DT.c% < 32%)	! Exit if C0 control	&
	!							&
	! Process C1 introducers, intermediates, parameters,	&
	! sequence terminators and other strange stuff		&
	!							&
	\ if (DT.c% < 48%) then		! Intermediate		&
	    SEQ.inter$ = SEQ.inter$ + chr$(DT.c%)		&
	    ! \ print #2%, "intermediate: "; fnvisible$(dt.c%)	&
	    \ goto 13010		! Go get another	&

13050	if (SEQ.state% = ESC%) then	! <ESC> -> C1 control?	&
	    q% = DT.c% and 63%		! Mask out lower 6 bits	&
	    \ goto 13130 if (SEQ.inter$ <> "" or q% >= 32%)	&
	    \ DT.c% = q% + 128%		! Make it a C1 control	&
	    ! \ print #2%, "c0 -> c1: "; fnvisible$(dt.c%)	&
	    \ goto 13020		! Process C1 control	&

13060	goto 13120 if (DT.c% >= 64%)	! Sequence terminator	&
	! \ print #2%, "not terminator "; fnvisible$(dt.c%)	&
	\ goto 13080 if (DT.c% < 60%)	! private introducer?	&
	! \ print #2%, "private introducer "; fnvisible$(dt.c%)	&
	\ if (SEQ.parm% > 0%)		! maybe, but illegal	&
	    then SEQ.private$ = "X"	! after first param.	&
	    else SEQ.private$ = chr$(DT.c%)			&
		\ SEQ.parm% = 1%	! Mark "param"		&

13070	    goto 13010			! Read another char.	&

13080	!							&
	! We know the character is in the range '0'..'9' or	&
	! ';' (separator) or ':' (illegal separator)		&
	!							&
	SEQ.parm% = 1% if (SEQ.parm% = 0%)			&
	! \ print #2%, "param or sep: "; fnvisible$(dt.c%)	&
	\ if (SEQ.inter$ <> "") then	! No param's after	&
	    SEQ.inter$ = ""		! intermediates.	&
	    \ SEQ.private$ = "X"	! Mark it invalid.	&
	    ! \ print #2%, "parm or separator after inter"	&

13090	if (DT.c% <= ascii('9')) then	! Parameter digit	&
	    SEQ.p%(SEQ.parm%) =		! Make it a number	&
	      (SEQ.p%(SEQ.parm%) * 10%) + (DT.c% - ascii('0'))	&
	    ! \ print #2%, "digit, param :="; seq.p%(SEQ.parm%)	&
	    \ goto 13010		! Go read another byte	&

13100	if (DT.c% = ascii(';')) then	! parameter separator	&
	    SEQ.parm% = SEQ.parm% + 1%				&
	    \ goto 13010		! and read another byte	&

13110	SEQ.private$ = "X"		! ':' isn't a separator	&
	! \ print #2%, "bad separator "; fnvisible$(dt.c%)	&
	\ goto 13010			! read another byte	&

13120	!							&
	! Character is a sequence terminator.  If no parameters	&
	! were read, return a single zero-valued parameter.	&
	!							&
	SEQ.parm% = 1% if (SEQ.parm% = 0%)			&
	! \ print #2%, "terminator: "; fnvisible$(dt.c%)	&

13130	!							&
	! Jump here at the end of the sequence.			&
	!							&
	SEQ.final$ = chr$(DT.c%)	! Set the final		&
	\ DT.c% = SEQ.state%		! Get return value	&
	\ SEQ.state% = 0%		! Not in a sequence	&

13140	SEQ.char% = DT.c%		! Character code	&

13180	FNgetsequence% = SEQ.char%	! Return value		&

13190	fnend				! That's all, folks	&

14100	def* FNsend%(text$)					&
	!							&
	!	F N s e n d % ( t e x t $ )			&
	!							&
	! Send a string of text to DECtalk.  Note, the text	&
	! length must be less than the DECtalk terminal buffer	&
	! size.							&
	!							&

14110	field #9%, len(text$) as q$				&
	\ lset q$ = text$					&
	\ put #9%, record 4096%, count len(text$)		&
	\ if (debug% and DT.log%) then				&
	    print #2%, using 'sent: ### "', len(text$); 	&
	    \ change text$ to q%				&
	    \ print #2%, FNvisible$(q%(q%));			&
		for q% = 1% to q%(0%)				&
	    \ print #2%, '"'					&

14190	fnend							&

14200	def* FNcsi%(text$) = FNsend%(ESC$ + "[" + text$)	&
	!							&
	!	F N c s i % ( t e x t $ )			&
	!							&
	! Send a Control Sequence to DECtalk.			&
	!							&
	
14300	def* FNdcs%(text$) =					&
	    FNsend%(ESC$ + "P0;" + text$ + "z" + ESC$ + "\")	&
	!							&
	!	F N d c s % ( t e x t $ )			&
	!							&
	! Send a DECtalk Device Control Sequence.		&
	! Note that the DECtalk P1 parameter, final, and	&
	! string terminator are automatically included.		&
	!							&

14400	def* FNspeak%(text$) = FNsend%(text$ + CRLF$)		&
	!							&
	!	F N l i n e % ( t e x t $ )			&
	!							&
	! Send a line of text to DECtalk, followed by <CR><LF>	&
	!							&

15000	def* FNget%(timeout%)					&
	!							&
	!	F N g e t % ( t i m e o u t % )			&
	!							&
	! Read the next character from DECtalk.			&
	! timeout% =    0%	means none			&
	! timeout% >	0%	wait timeout% seconds		&
	! timeout% =   -1%	return immediately if none	&
	! return 0% on timeout, fatal exit on other errors	&
	! The character is forced into the range 000 to 127	&
	! and <NUL> (000) and <DEL> (127) are ignored		&
	!							&

15010	while (DT.incount% >= DT.inend%)	! None saved?	&
	    \ goto 15080 if (not FNread%(timeout%))		&

15020	next							&

15030	field #8%, DT.incount% as q$, 1% as q$	! get char	&
	\ DT.incount% = DT.incount% + 1%	! step index	&
	\ q% = ascii(q$) and 127%		! drop parity	&
	\ goto 15010 if (q% = 0% or q% = 127%)	! ignore nulls	&
	\ FNget% = q%				! return char	&
	\ goto 15090				! exit		&

15080	FNget% = 0%				! got timeout	&

15090	fnend							&

15100	def* FNread%(timeout%)					&
	!							&
	!	F N r e a d % ( t i m e o u t % )		&
	!							&
	! Read a record from DECtalk.				&
	! timeout% =    0%	means none			&
	! timeout% >	0%	wait timeout% seconds		&
	! timeout% =   -1%	return immediately if none	&
	! return FALSE% on timeout, fatal exit on other errors	&
	! return TRUE% on success.				&
	!							&

15110	goto 15120 if DT.incount% < DT.inend%	! Still stuff	&
	\ on error goto 15150			! grab error	&
	\ q$ = sys(chr$(3%) + chr$(9%))		! no echo	&
	    + sys(chr$(4%) + chr$(9%))		! odt mode	&
	\ wait timeout% if timeout% > 0%	! timeout	&
	\ get #8% if timeout% >= 0%		! read buffer	&
	\ get #8%, record 8192% if timeout% < 0%		&
	\ DT.inend% = recount			! got it	&
	\ wait 0%				! no timeout	&
	\ DT.incount% = 0%			! clear index	&
	\ on error goto 19000			! common exit	&
	\ if (debug% and DT.log%) then				&
	    print #2%, using "read: ### '", DT.inend%;		&
	    \ field #8%, DT.inend% as q$			&
	    \ change q$ to q%					&
	    \ print #2%, FNvisible$(q%(q%));			&
		for q% = 1% to q%(0%)				&
	    \ print #2%, "'"					&

15120	FNread% = TRUE%						&
	\ goto 15190						&


15150	resume 15180						&
	  if ((err = 15% and timeout% > 0%)			&
	   or (err = 13% and timeout% < 0%))			&
	\ goto 19000						&

15180	FNread% = FALSE%					&

15190	fnend							&

16000	def* FNtest%(t2%, t3%) =				&
		(DT.char% = DCS%)	! Make 			&
	    and (DT.final$ = 'z')	!   sure		&
	    and (len(DT.inter$) = 0%)	!     it's		&
	    and (len(DT.private$) = 0%)	!	from		&
	    and (R1% = 0%)		!	  DECtalk	&
	    and (t2% = R2%)		! Check R2%		&
	    and (t3% = R3% or t3% = -1%) ! maybe check R3%	&
	!							&
	!	F N t e s t % ( t 2 % ,  t 3 % )		&
	!							&
	! Return TRUE% if the current reply is a properly-	&
	! formed DECtalk reply sequence whose R2% and R3%	&
	! parameters match T2% and T3%.  T3% is ignored if	&
	! it is -1%.						&
	!							&

16100	def* FNptest%(t3%) =		! Test phone reply	&
	    FNtest%(R2.PHONE%, t3%)				&
	!							&
	!	F N p t e s t % ( t 3 % )			&
	!							&
	! Return TRUE% if the current reply R2% parameter	&
	! is R2.PHONE% and the T3% matches R3%			&
	!							&

17000	def* FNfunny%(text$)					&
	!							&
	!	F N f u n n y % ( t e x t $ )			&
	!							&
	! Log an error message and dump the current reply.	&
	!							&

17010	error.count% = error.count% + 1%			&
	\ if (DT.log%) then					&
	    print #2% if (ccpos(2%) <> 0%)			&
	    \ print #2%, "Illegal reply at "; text$; "."	&
	    \ FNfunny% = FNdump%("")				&

17090	fnend							&

17100	def* FNdump%(text$)					&
	!							&
	!	F N d u m p % ( t e x t $ )			&
	!							&
	! Dump the current reply.				&
	!							&

17110	if (DT.log%) then					&
	    print #2%, "Last sequence read";			&
	    \ print #2%, " at "; text$; if (text$ <> "")	&
	    \ print #2%, ": ";					&
	    \ if (DT.char% = 0%)				&
		then print #2%, "<TIMEOUT>"			&
		else print #2%, FNvisible$(DT.char%);		&
		    \ print #2%, DT.private$; DT.inter$;	&
		    \ for q% = 1% to DT.parm%			&
			\ print #2%, num1$(DT.p%(q%));		&
			    if (DT.p%(q%) <> 0%)		&
			\ print #2%, ";";			&
			    if ((q% + 1%) < DT.parm%)		&
		    \ next q%					&
		    \ print #2%, DT.final$;			&
		    \ print #2%, "<ST>"; if (DT.char% = DCS%)	&
		    \ print #2%					&

17190	fnend							&

17200	def* FNvisible$(c%)					&
	!							&
	!	F N v i s i b l e $ ( c % )			&
	!							&
	! Return "datascope" version of c%			&
	!							&

17210	if (c% = ESC%)		then FNvisible$ = "<ESC>"	&
	else if (c% = DCS%)	then FNvisible$ = "<DCS>"	&
	else if (c% = CSI%)	then FNvisible$ = "<CSI>"	&
	else if (c% = ST%)	then FNvisible$ = "<ST>"	&
	else if (c% = 10%)	then FNvisible$ = CRLF$		&
	else if (c% = 11%)	then FNvisible$ = "<VT>"	&
	else if (c% = 13%)	then FNvisible$ = ""		&
	else q.vis% = (c% >= 127% or c% < 32%)			&
	    \ q$ = ""						&
	    \ q$ = "<~" if (c% >= 128%)				&
	    \ q$ = "<"  if (c% <   32%)				&
	    \ c% = c% and 127%					&
	    \ q$ = q$ + "^" + chr$(c% + 64%) if (c% < 32%)	&
	    \ q$ = q$ + chr$(c%) if (c% >= 32%)			&
	    \ q$ = q$ + ">" if q.vis%				&
	    \ FNvisible$ = q$					&

17290	fnend							&

17300	def* FNlog%(text$)					&
	!							&
	! F N l o g % ( t e x t $ )				&
	!							&
	! Log a text message					&
	!							&

17310	if (DT.log%) then					&
	    print #2%, date$(0%); " "; time$(0%); " "; text$	&

17390	fnend							&

17400	def* FNyesno%(prompt$, default$)			&
	!							&
	! F N y e s n o % ( p r o m p t $ , d e f a u l t $ )	&
	!							&
	! Prompt and get a yes/no answer			&
	!							&

17410	q% = 0%							&
	\ until (q% = 1% or q% = 5%)				&
	    \ q$ = FNprompt$(prompt$ + "(Yes/No)", default$)	&
	    \ q% = instr(1%, "YES NO", cvt$$(q$, -1%))		&
	\ next							&
	\ fnyesno% = (q% = 1%)					&

17490	fnend							&

17500	def* FNprompt$(prompt$, default$)			&
	!							&
	! F N p r o m p t % ( p r o m p t $ , d e f a u l t $ )	&
	!							&
	! Prompt and get a response				&
	!							&

17510	print #1%, prompt$; " <"; default$; ">? ";		&
	\ input line #1%, q$					&
	\ FNprompt$, q$ = cvt$$(q$, 397%)			&
	\ FNprompt$ = cvt$$(default$, 397%) if len(q$) = 0%	&

17590	fnend							&

19000	!							&
	!	F a t a l   E r r o r   T r a p			&
	!							&

19010	error% = err			! save error number	&
	\ error.line% = erl		! and error line	&
	\ resume 19100			! and take fatal exit	&

19100	print				! force new line	&
	\ print 'Fatal Error "';	! print error message	&
	    cvt$$(right(sys(chr$(6%) + chr$(9%)			&
		+ chr$(error%)), 3%), 4%);			&
	    '" at line'; error.line%	! and line number	&

19180	stop							&

19190	goto 32767

32767	end
