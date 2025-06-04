
	.TITLE		PPL$MSGDEF
	.IDENT		'V53-002'	! File: PPLMSG.MSG Edit: CMF1004
!
!****************************************************************************
!*									    *
!*  COPYRIGHT (c) 1987 BY						    *
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

!++
!
! FACILITY:	VAX PPL
!
! ABSTRACT:
!
!	This file contains the definition for the PPL facility specific
!	error messages.  The facility code for PPL is 481.
!
! ENVIRONMENT:	VAX/VMS user mode
!
! AUTHOR: P. Gilbert, CREATION DATE: 30-Sep-1986
!
! MODIFIED BY:
!
!	E01-001		Original
!
!	E01-002		To add the facility code.	CMF	23-JAN-1987
!
!	E01-003		To correct the ident to match	CMF	26-JAN-1987
!			the cms generation number.
!
!	E01-004		Revamp the file to conform to	CMF	28-JAN-1987
!			RTL standards. ( patterned after
!			LIBMSG.MSG )
!
!	E01-005		To change the module name from  CMF	16-FEB-1987
!			PPL$MSG to PPL$MSGDEF to be
!			consistent with the other RTLs
!
!	E01-006		To add PPL$_ATTUSETWO to indicate CMF	16-FEB-1987
!			that an attempt has been made to
!			use two barriers thus resulting in
!			dead lock.
!
!	E01-007		Add msgs for barriers and events,	DLR 24-JUL-87
!			plus other misc updates.
!			Remove old dead ones by ! *** notation.
!
!	E01-008		Conform to documentation standards.	DLR 19-AUG-1987
!
!	V051-001	Added PPL$_NO_NEXT_PROC			WWS 09-Sep-1988
!
!	V53-001		Corrected event signal messages		WWS 20-Mar-1989
!
!	V53-002		Added the V5.3 messages			WWS 27-Mar-1989
!
!	V57-001		Add a space to noone_enabled text	PJC 22-Jan-1993
!	
!	V57-002		Change reference to ppl$initialize	PJC 30-Aug-1993
!--

!+
! Symbols are defined following the standard for global names:
!
!	PPL$_abcmnoxyz
!
! In addition, the ibrary standards specify that the letters "abc", "mno",
! and "xyz" are the first three letters of the first three words of the
! error message, not counting articles and prepsitions.
!
! The names of all procedures using each error condition value should appear
! in the comments included with each error definition.
!
!
!
! MACRO-32Programming:
!
! The macros call:
!
!	$PPLDEF
!
! Will cause all symbols to be made available to the module.
! The Run-time Library declares these symbols as EXTERNAL;
! the macros are provided only for the covenience of the user.
!-

!+
! Special note:
!
! The messages here should be resolved from STARLET.OLB.  Inorder
! for this to happen, several vms build files need to be edited.
! This will be done for sys 11.  Note that several of out build
! procedures may need to be edited.  PPLRTLBLD.COM to copy
! and delete appropriate file in appropriate places.
!
! Also, the file PPLDEF will need to be changed to be name PPLMSGDEF
! to follow the standards of the other routines.
!
! PPLRTLLNK.OPT may need to be edited to get rid of universal symbold
! so that there won't be extra bagage in the shareable image.
!
!-

!			; Set LH to 481 (decimal).
	.FACILITY	PPL,481 /SYSTEM/MACRO=$PPLDEF
	.SEVERITY	SUCCESS
	.BASE		0

	NORMAL		<normal successful completion>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  All is well.
	!  USER ACTION:  Enjoy your success.  All things must pass.
	!--

	.BASE		2

	CREATED		<requested item was created>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Item was created, an alternate success status
	!		 indicating the item was not just successfully found.
	!  USER ACTION:  Continue normally.
	!--

	NOONE_ENABLED	<no one presently enabled for event notification>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Event trigger is successfully queued, and no one is
	!		 presently enabled to receive it.  The first caller to
	!		 enable it will receive it.
	!  USER ACTION:  Continue normally.
	!--

	NOT_AVAILABLE	<requested resource not available now>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Alternate success status returned when user requests
	!		 immediate non-blocking access to a resource which
	!		 was not available.
	!  USER ACTION:  Continue normally.
	!--

	CREATED_SOME	<created some of those requested - partial success>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Some of the requested items were created, but not all.
	!  USER ACTION:  If processing can continue with fewer items than those
	!		 requested, then continue normally.
	!--

	NO_NEXT_PROC	<there is no next process to receive message>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!
	!  EXPLANATION:  This code is for internal use only and is returned
	!		 from the routine, PPL$$TELL, when it is invoked with
	!		 the TELL_K_NEXT_PROCESS flag and there is only one
	!		 process currently in the application.
	!		 
	!  USER ACTION:  None:  This code is for internal use only.
	!--

	DELETED		<the specified object was forcibly deleted>
	!++
	!  FACILITY:	 PPL, Run-Time Library
	!
	!  EXPLANATION:	 The user requested that the object be deleted
	!		 regardless of its state.  The deletion was successfully
	!		 performed.
	!
	!  USER ACTION	 Continue normally
	!--

	

!			; Note:  Error codes must not overlap success codes!!!
!			; Set sub-system specific bit for now.
!			; Set LH to 481 (decimal)

	.SEVERITY	FATAL
	.BASE		64

!***	EXCQUESIZ	<exceeded queue size, number of waiting processes at limit>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Too many callers required queuing.
	!  USER ACTION:  Recode to reduce number of callers accessing resource.
	!--

	INCOMPEXI	<incompatible element with the same name already exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  All PPL objects must have unique names.
	!  USER ACTION:  Recode the call to specify a different name.
	!--

	INSVIRMEM	<insufficient virtual memory available>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Caller does not have sufficient available space to
	!		 complete request.
	!  USER ACTION:  If you use extensive PPL services, try specifying a
	!		 larger value to ppl$initialize.  Otherwise, check
	!		 your system quotas to see if an increase is needed.
	!--

	INVDESC		<invalid descriptor>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Descriptor is invalid.
	!  USER ACTION:  Recode to correct the descriptor.
	!--

	INVELEID	<invalid element ID>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Specified element id is not a legitimate id.
	!  USER ACTION:  Examine program code - some modification of the
	!		 element id has occurred.
	!--

	INVELENAM	<invalid element name>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Illegal character string.
	!  USER ACTION:  Fix the character string.
	!--

	INVELETYP	<invalid element type for requested operation>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  User specified an element which cannot be operated
	!		 by the called routine.
	!  USER ACTION:  Recode to pass the proper element type.
	!--

	INVNUMCHI	<invalid number of children, must be positive>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  User requested creation of a number < 1.
	!  USER ACTION:  Recode request to use the proper value.
	!--

!***	INVQUESIZ	<invalid queue size, must be positive>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Invalid queue size.
	!  USER ACTION:  Specify a positive number.
	!--

	INVSEMINI	<invalid semaphore initial value, cannot exceed its maximum value>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  A semaphore initial value must be <= its maximum.
	!  USER ACTION:  Change the specified initial value.
	!--

	INVSEMMAX	<invalid semaphore maximum value, must be positive>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  A semaphore's maximum value must be positive.
	!  USER ACTION:  Change the specified maximum value.
	!--

	LOCNOTEST	<lock was not established>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  You cannot release a lock you have not been granted.
	!  USER ACTION:  Recode to assure that the lock is held by the
	!		 caller attempting to release it.
	!--

	NOSECEX		<no such section exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Section specified for flush or delete does not exist.
	!  USER ACTION:  Recode to assure that section to delete/flush has been
	!		 created.
	!--

!***	NOSHRM		<no shared memory exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  No shared memory exists.
	!  USER ACTION:  Ditto.
	!--

	NOSUCHELE	<no element with the specified ID exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  No element is specified by the input id.
	!  USER ACTION:  Recode to assure that the input id is for a
	!		 legitimately created element.
	!--

	NOSUCHLOC	<no lock with the specified ID exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  No lock element is specified by the input id.
	!  USER ACTION:  Recode to assure that the input id is for a
	!		 legitimately created lock element.
	!--

	NOSUCHNAM	<no element with the specified NAME exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Input name does not specify an existing element.
	!  USER ACTION:  Recode to assure that the name matches a created
	!		 element.
	!--

	WRONUMARG	<wrong number of arguments>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Too many or too few arguments specified in the call.
	!  USER ACTION:  Recode using proper number of arguments.
	!--

	ABNTERMIN	<process #!UL (PID=!XL) ended; exit status %X!XL>/FAO=3
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Abnormal termination.
	!  USER ACTION:  An application participant has ended abnormally.
	!		 Some application-specific recovery may be desired.
	!--

	EXHNEVCAL	<process exit handler was not called>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  An application participant terminated without
	!		 executing its PPL exit handler.
	!  USER ACTION:  Application has been abnormally terminated.  Some
	!		 application-specific recovery may be desired.
	!--

	INVARG		<invalid argument>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  A specified argument is invalid.
	!  USER ACTION:  Recode to pass the proper argument.
	!--

	NOINIT		<PPL has not been initialized>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  PPL$INITIALIZE must be called before this routine.
	!  USER ACTION:  Recode to assure that calls are issued in proper order.
	!		 PPL$INITIALIZE is performed automatically for most
	!		 calls, but not for those which require the prior
	!		 creation of an element.
	!--

	NONPIC		<cannot map section !AS to same addresses as others mapped it>/FAO=1
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  PPL cannot map requested address space to the same
	!		 addresses in all processes.
	!  USER ACTION:  If it is critical to the application that the requested
	!		 space appear at the same addresses in all processes,
	!		 the user must take steps to assure that enough space
	!		 is made available.  This may require increasing system
	!		 quotas or reserving the required addresses through
	!		 linktime operations.
	!--

	SEMININON	<semaphore initial value must be non-negative>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Improper initial value specified.
	!  USER ACTION:  Recode to assure that proper value is passed.
	!--

	NOTIMP		<not implemented>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  A call has been issued to an unimplemented service.
	!  USER ACTION:  Recode to eliminate this call.
	!--

	ATTUSETWO	<attempt to use two full barriers simultaneously - deadlock will occur.>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  User has attempted to wait on 2 application-wide
	!		 barriers at the same time.
	!  USER ACTION:  Recode to assure that deadlock is avoided by waiting
	!		 on only one application-wide barrier at a time.
	!--

	IN_BARRIER_WAIT	<barrier in wait state, quorum not modified>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  The barrier was already in effect at the time of
	!		 this call.
	!  USER ACTION:  Recode to assure that the barrier quorum is established
	!		 before anyone attempts to wait on the barrier.
	!--

	NO_SUCH_PARTY	<no such party in this application>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Specified participant does not exist in this
	!		 application.
	!  USER ACTION:  Recode to assure that proper participant is specified
	!		 in the call to this routine.
	!--

	APPALREXI	<the specified application already exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This process attempted specifically to FORM an
	!		 application with a particular name.  That application
	!		 already exists, and so cannot be formed by this
	!		 process.
	!  USER ACTION:  Since the user specifically requested to form an
	!		 application, this is application-dependent. 
	!--

	ELEINUSE 	<object is in use, specified operation cannot be performed>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This process attempted an operation which may only 
	!		 be performed on an object which is not in use.
	!		 The specified object is currently in use and therefore 
	!		 the operation may not be performed.
	!  USER ACTION:  Recode to free the object before attempting this 
	!		 operation, or, if applicable, specify the option to force 
	!		 this operation.
	!--

	INCOMPARG	<arguments are incompatible with existing application>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This process specified a size, protection, or flag on a
	!		 call to PPL$Initialize which is incompatible with the
	!		 existing application which this process is attempting
	!		 to join.
	!  USER ACTION:  Recode the call to omit the arguments, or specify
	!		 arguments which are compatible with the existing
	!		 application.
	!--

	INVAPPNAM	<Invalid application name or illegal character string>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Illegal character string.
	!  USER ACTION:  Fix the character string.
	!--

	NOSUCHAPP	<the specified application does not exist>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This process attempted specifically to JOIN an existing
	!		 application with a particular name.  That application
	!		 does not exist, and so cannot be joined by this
	!		 process.
	!  USER ACTION:  Since the user specifically requested to join an
	!		 application, this is application-dependent. 
	!--


	.SEVERITY	WARNING
	.BASE		512

	BUFOVFL		<buffer overflow for output filename string>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Buffer could not contain the complete output string.
	!  USER ACTION:  Recode to pass a larger buffer.
	!--

	ALTSIZE		<internal data structure size cannot be altered at this point>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  PPL$INITIALIZE has already been called.
	!  USER ACTION:  Recode to assure that calls are issued in proper order.
	!		 PPL$INITIALIZE is performed automatically for most
	!		 calls.  Altering the internal structure size must be
	!		 done before any other call to PPL.
	!--


	.SEVERITY	INFORMATIONAL
	.BASE		2048

	ELEALREXI	<an element of the same name already exists>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  An element by this name already exists, and its
	!		 id has been returned.
	!  USER ACTION:  Continue normally if the name is expected to exist.
	!--

	SEMALRMAX	<semaphore is already at its maximum value>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  Semaphore cannot be incremented further.
	!  USER ACTION:  Recode to assure that only the proper maximum number
	!		 of ppl$increment_semaphore requests are issued.
	!--

	INTDELPRC	<participant stopped by PPL$STOP>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  An application call to ppl$stop forced the temination
	!		 of this participant.
	!  USER ACTION:  Since the application itself terminated this party,
	!		 any actions are application-dependent.
	!--

	INTTERMIN	<participant stopped by PPL$TERMINATE>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  An application call to ppl$terminate forced the
	!		 temination of this participant.
	!  USER ACTION:  Since the application itself terminated this party,
	!		 any actions are application-dependent.
	!--

	EXITSTATUS	<is '!AS'>/FAO=1
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  An application participant exited with this status.
	!  USER ACTION:  Determine the cause of the termination by examining
	!		 the status code.
	!--

	EVENT_OCCURRED	<event occurred - param: !XL>/FAO=1
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This code indicates the occurrence of an event for
	!		 which notification has been requested by the user.
	!  USER ACTION:  Since the application requested this notification,
	!		 response is application-dependent.
	!--

	NORMAL_EXIT	<normal exit - !+!+ ppl index: !UL status: !XL>/FAO=4
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This code indicates the occurrence of the normal
	!		 participant termination event.
	!  USER ACTION:  Since the application requested this notification,
	!		 response is application-dependent.
	!--

	ABNORMAL_EXIT	<abnormal exit - !+!+ ppl index: !UL status: !XL>/FAO=4
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This code indicates the occurrence of the abnormal
	!		 participant termination event.
	!  USER ACTION:  Since the application requested this notification,
	!		 response is application-dependent.
	!--

	FORMEDAPP	<a new application was formed>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This process formed a new application.
	!  USER ACTION:  Continue normally if this was an expected result.
	!--

	JOINEDAPP	<an existing application was joined>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  This process joined an existing application.
	!  USER ACTION:  Continue normally if this was an expected result.
	!--

	NOMATCH		<no match for the specified item was found>
	!++
	!  FACILITY:     PPL, Run-Time Library
	!  EXPLANATION:  No match for the specified element was found.
	!  USER ACTION:  Continue normally if this was an expected result.
	!--


	.END
