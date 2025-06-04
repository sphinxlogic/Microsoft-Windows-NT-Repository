	.Title		CMem_Message, VMS Messages for C Heap Debugger
	.Ident		"V1.1"


	.Facility	CMEM, 161/Prefix=CMEM__


	.Severity	Success

Normal		<normal successful completion>
!
!	Everything worked.  Don't worry about it!
!


	.Severity	Informational

Init		<malloc/free monitoring has begun>
!
!	This informational message is simply meant to let the user know that
!	the debugging code was linked with the image and that at least one
!	piece of code has invoked our routines.
!

Timestamp	<event occurred at !%D>/FAO=1
!
!	Sometimes it is handy to have a timestamp with the message.
!

StackEna	<record call stack when segment allocated>
StackDis	<do not record call stack when segment allocated>
TimeEna		<allocation timestamp enabled>
TimeDis		<allocation timestamp disabled>
BndryPrtEna	<boundary check for newly allocated memory enabled>
BndryPrtDis	<boundary check for newly allocated memory disabled>
RelMemPrt	<released memory blocks will be protected>
RelMemReuse	<released memory blocks will be reused>
!
!	The user can change how the software operates on the fly.  These
!	messages simply alert the user to the state change.
!


	.Severity	Warning

mallocFail	<malloc request cannot be fulfilled; $EXPREG failure>
!
!	As a result of the user calling the 'malloc' routine, we tried to
!	expand the virtual address space and failed.  We will return a NULL
!	pointer to the calling routine.
!

FreeUnallo	<attempted 'free' of unallocated memory at !8XL>
!
!	The calling routine attempted to release memory that was never
!	allocated.  The address specified by the caller is displayed.
!

FreeWrtFail	<access out of bounds detected>
!
!	The memory being released shows signs that someone modified memory
!	before that location in memory.
!

NoBndyPrt	<boundary protection not enabled; freed memory will be reused>
!
!	The user may request that freed memory be protected.  That ability
!	is only available for blocks which have been allocated using
!	boundary protection.  This warning message is displayed because we
!	are currently not in that mode (although we could have some previously
!	allocated blocks in the protected memory zone).  
!

UnExpResp	<unexpected response from debugger; expected !SL line!%S, got !SL>/FAO=2
!
!	We check the number of lines returned from the debugger subprocess in
!	response to some CMEM requests.  This message simply means that we
!	didn't get the expected response.  The program will display the text
!	which was returned.
!


	.Severity Error

ProtChgFail	<attempt to change page protection ($SETPRT) failed>
!
!	A call to $SETPRT has failed.  The exact status from that call should
!	be displayed following this message.
!

DeltVAFail	<call to $DELTVA has failed>
!
!	We had some sort of weird error trying to release memory.
!

BndryRdVio	<boundary read violation; VA=!8XL; allocated segment info:>/FAO=1
BndryWrtVio	<boundary write violation; VA=!8XL; allocated segment info:>/FAO=1
!
!	The process has incurred an access violation.  The attempted read or
!	write was into a protected page surrounding one of segments of
!	allocated memory.  We display any info we have about the segment.
!


	.Severity Fatal

InfoExpFail	<unable to allocate memory for a new CLUSTER_INFO_STRUCT>
!
!	We had run out of slots for holding information about 'malloc'
!	requests.  A call to LIB$GET_VM has failed.
!

UnexpErr	<unexpected error from !AZ>/FAO=1
!
!	An error occurred which really should never happen.  Refer to the
!	associated error text to see what failed.
!

GetVMFail	<memory allocation failed at !AZ>/FAO=1
!
!	A call to LIB$GET_VM failed.  This usually means we're out of virutal
!	address space or else there is some form of memory corruption.
!

ExpRegFail	<unable to allocate virtual address space>
!
!	An error was returned from $EXPREG
!

NoFreeBlk	<no free info blocks when there should be !UL>/FAO=1
!
!	There were no free blocks within the cluster when the count of free
!	blocks was greater than zero.  This represents a programming error.
!

OldSubPrc	<old subprocess will not exit>
!
!	We did a $FORCEX on an old sub-process but it did not die.
!

SubCreFail	<unable to create subprocess>
!
!	LIB$SPAWN failed
!

ArrTooSml	<array is too small to accept debugger response>
!
!	The debugger returned more lines than was expected.
!

UnDbgExit	<unexpected exit of debugger subprocess>
!
!	The subprocess exited unexpectedly.  The completion status is also
!	reported.  I bet it is a quota problem...
!

	.End
