	.TITLE	DTK$MSGDEF	DTK$ facility messages
	.IDENT	'1-004'		! File: DTKMSG.MSG  Edit: TS1004
!
!****************************************************************************
!*									    *
!*  COPYRIGHT (c) 1978, 1980, 1982, 1984, 1985, 1986, 1987, 1988 BY	    *
!*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    *
!*  ALL RIGHTS RESERVED.						    *
!* 									    *
!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
!*  TRANSFERRED.							    *
!* 									    *
!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
!*  CORPORATION.							    *
!* 									    *
!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    *
!* 									    *
!*									    *
!****************************************************************************
!
!
! 1-004 Add DSR status messages.  TS 29-Aug-1986
! 1-003 Add /MACRO=$DTKMSG for STARLETSD.  TS 30-Jul-1986
! 1-002 Add DTK$_TLKINUSE.  TS 10-Apr-1986
! 1-001 Original.  TS 6-Aug-1985
!-

!+
! Symbols are defined following the standard for global names:
!
!	DTK$_abcmnoxyz
!
! In addition, the library standards specify that the letters "abc", "mno",
! and "xyz" are the first three letters of the first three words of the error
! message, not counting articles and prepositions.
!
! The names of all procedures using each error condition value should appear
! in the comments included with each error definition.
!

! MACRO-32 Programming:
!
! The macros call:
!
!	$DTKDEF
!
! will cause all symbols to be made available to the module.
! The Run-time Library declares these symbols as EXTERNAL;
! the macros are provided only for the convenience of the user.
!
!-

! Template for documenting messages.
! Each explanation follows the message it applies to and
! is followed by a blank line.
! Be sure not to use the sequence "++" after a "!" in regular comments
! as this sequence begins a documentation sequence and can be
! searched for if and when the process of extracting the
! documentation becomes automated.

	!++
	! FACILITY:	DTK
	! EXPLANATION:	x
	! USER ACTION:	y
	!--

!			; Set LH to 419 (decimal).
	.FACILITY	DTK,419 /SYSTEM/MACRO=$DTKMSG
	.SEVERITY	SUCCESS

	.BASE		0

	NORMAL		<normal successful completion>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	Normal successful completion.
	! USER ACTION:	none
	!--

	.BASE		2

	VOIALREXI	<voice_id already exists for this device>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A user tried to create a new voice_id for a DECtalk 
	!		device, when one already exists.  This is normal in
	!		a modular environment.  The existing voice id
	!		is returned.
	! USER ACTION:	None. Make sure you don't delete a voice_id
	!		that you actually didn't create.
	!--

	NOT_A_TRM	<device is not a terminal>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	This is an informational message telling you that
	!		the output device is not a terminal.
	! USER ACTION:	None.
	!--

	!+
	! NOTE::	Do NOT add any additional SUCCESS messages here.
	!		Add them below after the remaining .SUCCESS messages.
	!-

	.SEVERITY	INFORMATIONAL

	BUSY		<a busy signal was detected on the phone line>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The routine DTK$DIAL_PHONE was called to dial a phone
	!		number and the line was busy.
	! USER ACTION:	Try calling again.
	!--

	NOANSWER	<no answer was detected on the phone line>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The routine DTK$DIAL_PHONE was called to dial a phone
	!		number and no one answered the call.
	! USER ACTION:	Try calling again.
	!--

	NODIATONE	<no dial tone was detected on the phone line>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The routine DTK$DIAL_PHONE was called to dial a phone
	!		number and no dial tone was detected on the
	!		phone line.  
	! USER ACTION:	Correct the telephone line problem and call again.
	!--

	OFFHOOK		<telephone is offhook (active)>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	This is an informational message telling you that
	!		the phone is offhook (a call is active).
	! USER ACTION:	None.
	!--

	ONHOOK		<telephone is onhook (inactive)>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	This is an informational message telling you that
	!		the phone is onhook (no call is active).
	! USER ACTION:	None.
	!--

	TIMEOUT		<a timeout has occurred>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	This is an informational message telling you that
	!		the output device has timed out.
	! USER ACTION:	None.
	!--

	WINK		<a wink was detected on the phone line>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A wink (possible timeout) was detected on the
	!		phone line.  The caller may have hung up the phone.
	! USER ACTION:	None.
	!--

	COMFAIL		<a communication failure was detected>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A communications malfunction has occurred since the 
	!		DECtalk unit was turned on.
	! USER ACTION:	Check the DECtalk hardware.
	!--

	INPBUFOVR	<input buffer overflow was detected>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	An input buffer overflow malfunction has occurred since 
	!		the DECtalk unit was turned on.
	! USER ACTION:	Check the DECtalk hardware.
	!--

	NVROPRFAI	<the last NVR operation has failed>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A NVR operation failure has occurred since 
	!		the DECtalk unit was turned on.
	! USER ACTION:	Check the DECtalk hardware.
	!--

	ERRPHOTRA	<phonemic transcription error was detected>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A phonemic transcription malfunction has occurred since 
	!		the DECtalk unit was turned on.
	! USER ACTION:	Check the DECtalk hardware.
	!--

	CONSEQERR	<a control sequence malfunction was detected>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A DECtalk private control sequence malfunction has 
	!		occurred since the DECtalk unit was turned on.
	! USER ACTION:	Check the DECtalk hardware.
	!--

	DECTSTFAI	<a local self-test has failed>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A local self-test malfunction has occurred since 
	!		the DECtalk unit was turned on.
	! USER ACTION:	Check the DECtalk hardware.
	!--

	.SEVERITY	FATAL
	.BASE		64

	FATERRLIB	<fatal error in library>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	An internal error has occured within DTK.
	! USER ACTION:	Submit an SPR describing the problem.
	!--

	FILTOOLON	<file-specification is too long (over 255 characters)>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The length of the specified string
	!		was too long.
	! USER ACTION:	Correct your program so that it requests a file
	!		name of 255 characters or less.
	!--

	INVARG		<invalid argument>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The value of some argument is not in the correct range.
	! USER ACTION:	Examine each argument. Determine which one is
	!		out of range and correct your program.
	!--

	INVMODE		<invalid mode parameter>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The value of the mode argument is not in the 
	!		correct range.
	! USER ACTION:	Examine the argument. Determine what is
	!		out of range and correct your program.
	!--

	INVVOI_ID	<invalid voice-id>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	An invalid voice-id was specified.
	! USER ACTION:	Check that you are passing the voice id by reference
	!		not by value. Ensure that the voice id was
	!		previously set up by a call to DTK$INITIALIZE.
	!--

	NOROOM		<no room in dictionary for definition>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	There is no room left in the dictionary for this
	!		definition.
	! USER ACTION:	Correct your program so that it requests fewer 
	!		dictionary entries or so that each entry is smaller.
	!--

	NOTIMP		<feature not implemented in this version of DTK$>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	You tried to use a feature that is not implemented
	!		in this version of DTK$.
	! USER ACTION:	Get the appropriate version of DTK$.
	!--

	PROTOOLON	<prompt-string is too long>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The length of a prompt string exceeded 512 characters.
	! USER ACTION:	Shorten the prompt to 512 characters or less.
	!--

	STRTERESC	<illegal escape sequence embedded in string>
	!++
	! FACILITY:	DTK
	! EXPLANATION:  An DTK output routine was passed an escape sequence
	!		embedded in the output string.  
	! USER ACTION:	Check your output string to determine what escape
	!		sequence was being passed.  If you need the action
	!		provided by this escape sequence, there is most
	!		likely a DTK routine that provides that functionality.
	!--

	TOOLONG		<specified parameter is too long (over 255 characters)>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The length of the specified string was too long.
	! USER ACTION:	Correct your program so that it requests a phone number
	!		or dictionary substitution of 255 characters or less.
	!--

	UNKESCSEQ	<unknown escape sequence received from DECtalk>
	!++
	! FACILITY:	DTK
	! EXPLANATION:  A DTK input routine was passed an escape sequence
	!		that it was unable to understand.
	! USER ACTION:	????
	!--

	UNKREPLY	<unknown reply recieved from DECtalk>
	!++
	! FACILITY:	DTK
	! EXPLANATION:  A DTK input routine was passed an escape sequence
	!		that it could not parse correctly.
	! USER ACTION:	????
	!--

	WRONUMARG	<wrong number of arguments>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A DTK routine was called with too few arguments
	!		(less than the number of required arguments)
	!		or too many arguments.
	! USER ACTION:	Correct the code to call the routine with
	!		the proper number of arguments.
	!--

	TLKINUSE	<DECTalk is in use>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	A DTK routine was called from an AST routine
	!		that interrupted a DTK routine with the same
	!		VOICE_ID.
	! USER ACTION:	Correct the code to call the routine with
	!		the proper synchronization.
	!--

	NOTLCLTRM	<device is not a local terminal>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	The output device is not a terminal on the
	!		local VMS node.
	! USER ACTION:	Specify a local terminal device.
	!--

	.SEVERITY	SUCCESS
	.BASE		256

	NOMALFUN1	<no malfunctions detected since power up>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	This is an informational message telling you that
	!		no malfunctions have occurred since the DECtalk
	!		unit was turned on.  This is the first reply.
	! USER ACTION:	None.
	!--

	NOMALFUN2	<no malfunctions detected since power up>
	!++
	! FACILITY:	DTK
	! EXPLANATION:	This is an informational message telling you that
	!		no malfunctions have occurred since the DECtalk
	!		unit was turned on.  This is the second or later reply.
	! USER ACTION:	None.
	!--

	.END

!	End of file DTKMSG.MSG
