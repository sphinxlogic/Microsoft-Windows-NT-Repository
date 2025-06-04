! SDLMSG.MSG
!
! COPYRIGHT (c) 1978-1992 BY
! DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
! ALL RIGHTS RESERVED.
!
! THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED
! ONLY  IN  ACCORDANCE  WITH  THE  TERMS  OF  SUCH LICENSE AND WITH THE
! INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER
! COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY
! OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY
! TRANSFERRED.
!
! THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE
! AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT
! CORPORATION.
!
! DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS
! SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.
!
!
! FACILITY:	VAX-11 SDL (Structure Definition Language)
!
! ABSTRACT:
!
!	This file defines the error messages used by SDL.
!	It is processed by the VMS MESSAGE utility to produce
!	an object file defining global values for all the message
!	numbers and an SDL text file (SDLMSG.SDL).  The SDL file is 
!	processed to produce a PL/I include file (.in) and a MACRO 
!	library file (.src) defining the message numbers.  
!	Currently, the PL/I include file is not used, but rather a 
!	manually maintained file, SDLMSGDEF.IN, defines the message
!	numbers as GLOBALREF VALUE's.  The global values are defined
!	by the object file, SDLMSG.OBJ, produced by the MESSAGE utility
!	from this file.
!	Someday a TPU procedure ought to be written so that the SDLMSG.IN
!	file can be automatically transformed into the format of the 
!	SDLMSGDEF.IN file.
!
! ENVIRONMENT:	VAX/VMS user mode
!
! AUTHOR:	?		CREATION DATE:	?
!
!			C H A N G E    L O G
!
!      Date	| Name	| Description
!_______________|_______|______________________________________________________
!   01-Apr-1992	|  jak	| EV1-3
!		|	| Copyright, abstract, etc. above and change log added.
!		|	| Added .ident, OFFSETEXPR, and INVEXPR.
!_______________|_______|______________________________________________________
!   20-May-1992	|  jak	| EV1-10
!		|	| Added REVCHECK.
!_______________|_______|______________________________________________________
!    8-Dec-1992	|  jak	| EV1-20
!		|	| Added INVLISTOPT.
!    7-Jan-1993 |	| Added UNDEFORG, UNDEFCON, UNDEFFIL, UNDEFUSER.
!_______________|_______|______________________________________________________
!   28-Jan-1993	|  jak	| EV1-21
!		|	| Added BASEALIGN.
!   12-Feb-1993	|	| Changed SYMNOTDEF to warning and reworded.
!		|	| Added LANGDUP and LANGMISS.  Reworded LANGMATCH.
!  18-Feb-93	|	| Changed wording of OFFSETEXPR.
!_______________|_______|______________________________________________________
!  12-Mar-1993	|  rs	| EV-22
!		|	| Added NEGORIGIN message
!_______________|_______|______________________________________________________
!  12-Dec-1996	|  aem	| EV-56
!		|	| Added INVDECL message
!_______________|_______|______________________________________________________

 
	.title		sdlmsg
	.ident		'EV1-56'
	.facility	/system	sdl,186

!***************************************************************************
! NOTE: 
!	Every new message requires an entry to be added manually to 
!	the PL/I include file SDLMSGDEF.IN.
!
!***************************************************************************

	.severity	fatal
 
symtabovr	<Symbol table overflow>
abort		<Fatal internal error. Unable to continue execution>
infilopn	<Unable to open input file !AS> /fao_count=1
bugcheck	<Internal consistency failure [Line !UL] - please submit a bug report> -
		/fao_count=1
revcheck	<Front-end / back-end version mismatch.  Check installation.> 
errexit		<Error exit>

	.severity	error
 
invshrimg	<Shareable language image not found !AS> -
		/fao_count=1
nullstruc	<Null structure !AS has no members [Line !UL]> -
		 /fao_count=2
dupconatt	<Item !AS has duplicate or conflicting attributes [Line !UL]> -
		 /fao_count=2
invfldsiz	<Item !AS has bitfield length or offset greater than 32 [Line !UL]> -
		/fao_count=2
undefsym	<Undefined local symbol !AS used in expression [Line !UL]> /fao_count=2
undefcon	<Undefined constant name !AS used in expression [Line !UL]> /fao_count=2
undefuser	<Undefined user type name !AS referenced [Line !UL]> /fao_count=2
invdecl		<Invalid DECLARE for type !AS [Line !UL]> /fao_count=2
undeffil	<Unable to open include file !AS [Line !UL]> /fao_count=2
undeforg	<Definition of ORIGIN name !AS not found in aggregate [Line !UL]> /fao_count=2
invout		<Invalid attributes for output language !AS [Line !UL]> /fao_count=2
tokovf		<Token exceeds maximum size of !AS [Line !UL]> /fao_count=2
outfilopn	<Unable to open output file !AS> /fao_count=1
syntaxerr	<Syntax error [Line !UL]> /fao_count=1
intovf		<Integer overflow in expression [Line !UL]> /fao_count=1
zerodiv		<Zero divide in expression [Line !UL]> /fao_count=1
bytsiz		<Aggregate !AS must be integral byte size [Line !UL]> -
		/fao_count=2
toomanyfields   <Structure !AS has too many fields [Line !UL]> /fao_count=2
invunklen	<Unknown length attribute valid only for parameter type [Line !UL]> /fao_count=1
sizeredef	<Size or type of item !AS redefined [Line !UL]> -
		 /fao_count=2
sizequal	<Item !AS, an aggregate, cannot be qualified by SIZEOF [Line !UL]> -
		 /fao_count=2
sizenest	<Illegal nesting of SIZEOF clauses (Item !AS) [Line !UL]> -
		 /fao_count=2
stringconst	<String constant !AS used in arithmetic expression [Line !UL]> -
		 /fao_count=2
invname		<Item name is invalid>
illforwref	<Illegal forward reference for output language !AS [Line !UL]> -
		/fao_count=2
invsymdef	<Invalid symbol !AS specified in /SYMBOLS qualifier> -
		/fao_count=1
symalrdef	<Symbol !AS was already defined in command line> -
		/fao_count=1
invalign	<Illegal value for /ALIGNMENT qualifier in command line>
infilsdi	<File format error reading intermediate file !AS.  Possible version mismatch> /fao_count=1
invbitfld	<Invalid bitfield !AS -- bitfields must be aggregate members [Line !UL]> -
		/fao_count=2
adrobjbas	<Address object !AS must have based storage class [Line !UL]> -
		/fao_count=2
incdefstruc	<Incompletely defined structure -- !AS [Line !UL]> -
		/fao_count=2
multdefsym	<Multiply defined symbol -- !AS [Line !UL]> -
		/fao_count=2
invparmtyp	<Invalid parameter type for language !AS [Line !UL]> -
		/fao_count=2
invexpr		<Invalid expression -- cannot be resolved to a constant as required, !AS [Line !UL]> -
		/fao_count=2
invlistopt      <Invalid use of LIST attribute -- LIST may only appear on the last parameter.  !AS [Line !UL]> -
		/fao_count=2
basealign       <Invalid expression with BASEALIGN option.  Value must be in range 0 to 124. !AS [Line !UL]> -
		/fao_count=2

	.severity	warning
 
lisfilopn	<Unable to open listing file !AS> /fao_count=1
nooutput	<No language output produced>
zerolen		<Item !AS has 0 or negative length [Line !UL]> -
		/fao_count=2
matchend	<End name does not match declaration name !AS [Line !UL]> -
		/fao_count=2
warnexit	<Warning exit>
typnam		<Aggregate type name not supported [Line !UL]> /fao_count=1
invreqparm	<Required parameter encountered after optional parameter !AS>/fao_count=1
namtrunc	<Generated name too long - truncated to 64 characters !AS> /fao_count = 1
typnotsup	<Output language does not support data type !AS [Line !UL]> -
		/fao_count=2
immgtr32	<Cannot pass values larger than 32 bits by immediate mechanism [Line !UL]> -
		/fao_count=1
identgtr31	<SDL-generated identifier longer than 31 characters exceeds capacity of !AS compiler [Line !UL]> -
		/fao_count=2
langdup		<Language name !AS appears more than once in list [Line !UL]> -
		/fao_count=2
langmatch	<Language !AS does not appear in list of matching IF statement [Line !UL]> -
		/fao_count=2
langmiss	<Language !AS in list of matching IF statement missing from END list [Line !UL]> -
		/fao_count=2
unaligned	<!AS does not align on its natural boundry [Line !UL]>-
		/fao_count=2
fillneglen	<Fill item !AS has a negative length [Line !UL]> -
		/fao_count=2
offsetexpr	<Offset or origin relative expression involves a forward or circular reference. !AS [Line !UL]> -
		/fao_count=2
symnotdef	<Symbol !AS was not defined in command line, value zero assumed [Line !UL]> -
		/fao_count=2

	.severity	informational

negorigin	<Aggregate !AS has a negative origin - negative offset elements will be ignored [Line !UL]> -
		/fao_count=2
fillzerolen	<Fill item !AS has a zero length [Line !UL]> /fao_count=2

	.end
