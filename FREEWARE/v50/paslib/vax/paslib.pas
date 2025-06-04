{************************************************************************
*									*
*			PASCAL  USER  ENVIRONMENT.			*
*			=========================			*
*									*
*  The Pascal User Environment contains declarations for all VAX/VMS	*
*  Run-Time Library and Utility routines (with some minor exceptions	*
*  as detailed below), and useful Constant and Type definitions.	*
*									*
*  Routines not implemented:  A few routines have not been implemented	*
*  in Paslib.  They are some MTH$ routines which return Complex		*
*  numbers, and any routines which have an equivalent in-built Pascal	*
*  function (or procedure).  Omissions are documented in each section.	*
*									*
*.......................................................................*
*									*
*  Written in VAX-11 Pascal for VAX/VMS machines.			*
*									*
*  Author:  Jonathan Ridler,						*
*	    University Computing Services,				*
*	    University of Melbourne.					*
*									*
*  Created: November/December, 1987.					*
*									*
*.......................................................................*
*									*
*  Modifications:							*
*	20-Feb-1989	JER	Revised for VMS Version 5		*
*	20-Mar-1990	JER	Revised for Pascal V4.0			*
************************************************************************}

[ ENVIRONMENT	('PASLIB'),
  INHERIT	('SYS$LIBRARY:STARLET',
		 'SYS$LIBRARY:OSIT',
		 'SYS$LIBRARY:PASCAL$DTK_ROUTINES',
		 'SYS$LIBRARY:PASCAL$D_FLOAT',
		 'SYS$LIBRARY:PASCAL$G_FLOAT',
		 'SYS$LIBRARY:PASCAL$LIB_ROUTINES',
		 'SYS$LIBRARY:PASCAL$MTH_ROUTINES',
		 'SYS$LIBRARY:PASCAL$NCS_ROUTINES',
		 'SYS$LIBRARY:PASCAL$OTS_ROUTINES',
		 'SYS$LIBRARY:PASCAL$PPL_ROUTINES',
		 'SYS$LIBRARY:PASCAL$SMG_ROUTINES',
		 'SYS$LIBRARY:PASCAL$SOR_ROUTINES',
		 'SYS$LIBRARY:PASCAL$STR_ROUTINES') ]


MODULE Paslib ;		{ Pascal User Environment }


TYPE		{ **** Pre-declared data types (some from Starlet) **** }

$BYTE = [BYTE]   -128..127 ;
$WORD = [WORD] -32768..32767 ;

$QUAD = [QUAD,UNSAFE] RECORD
			 L0 : UNSIGNED ;
			 L1 : INTEGER ;
		      END ;

$OCTA = [OCTA,UNSAFE] RECORD
			L0 , L1 , L2 : UNSIGNED ;
			L3 : INTEGER ;
		      END ;

$UBYTE = [BYTE] 0..255 ;
$UWORD = [WORD] 0..65535 ;

$UQUAD = [QUAD,UNSAFE] RECORD
			L0 , L1 : UNSIGNED ;
		       END ;

$UOCTA = [OCTA,UNSAFE] RECORD
			L0 , L1 , L2 , L3 : UNSIGNED ;
		       END ;

$PACKED_DEC = [BIT(4),UNSAFE] 0..15 ;

$DEFTYP = [UNSAFE] INTEGER ;
$DEFPTR = [UNSAFE] ^$DEFTYP ;

$BOOL  = [BIT(1), UNSAFE] BOOLEAN ;
$BIT   = [BIT(1), UNSAFE] BOOLEAN ;
$BIT2  = [BIT(2), UNSAFE] 0..3 ;
$BIT3  = [BIT(3), UNSAFE] 0..7 ;
$BIT4  = [BIT(4), UNSAFE] 0..15 ;
$BIT5  = [BIT(5), UNSAFE] 0..31 ;
$BIT6  = [BIT(6), UNSAFE] 0..63 ;
$BIT7  = [BIT(7), UNSAFE] 0..127 ;
$BIT8  = [BIT(8), UNSAFE] 0..255 ;
$BIT9  = [BIT(9), UNSAFE] 0..511 ;
$BIT10 = [BIT(10),UNSAFE] 0..1023 ;
$BIT11 = [BIT(11),UNSAFE] 0..2047 ;
$BIT12 = [BIT(12),UNSAFE] 0..4095 ;
$BIT13 = [BIT(13),UNSAFE] 0..8191 ;
$BIT14 = [BIT(14),UNSAFE] 0..16383 ;
$BIT15 = [BIT(15),UNSAFE] 0..32767 ;
$BIT16 = [BIT(16),UNSAFE] 0..65535 ;
$BIT17 = [BIT(17),UNSAFE] 0..131071 ;
$BIT18 = [BIT(18),UNSAFE] 0..262143 ;
$BIT19 = [BIT(19),UNSAFE] 0..524287 ;
$BIT20 = [BIT(20),UNSAFE] 0..1048575 ;
$BIT21 = [BIT(21),UNSAFE] 0..2097151 ;
$BIT22 = [BIT(22),UNSAFE] 0..4194303 ;
$BIT23 = [BIT(23),UNSAFE] 0..8388607 ;
$BIT24 = [BIT(24),UNSAFE] 0..16777215 ;
$BIT25 = [BIT(25),UNSAFE] 0..33554431 ;
$BIT26 = [BIT(26),UNSAFE] 0..67108863 ;
$BIT27 = [BIT(27),UNSAFE] 0..134217727 ;
$BIT28 = [BIT(28),UNSAFE] 0..268435455 ;
$BIT29 = [BIT(29),UNSAFE] 0..536870911 ;
$BIT30 = [BIT(30),UNSAFE] 0..1073741823 ;
$BIT31 = [BIT(31),UNSAFE] 0..2147483647 ;
$BIT32 = [BIT(32),UNSAFE] UNSIGNED ;

Status_Block_Type   = [UNSAFE] PACKED ARRAY [1..4] OF $UWORD ;

Mask_Byte     = [BYTE, UNSAFE] PACKED ARRAY [1..8]  OF BOOLEAN ;
Mask_Word     = [WORD, UNSAFE] PACKED ARRAY [1..16] OF BOOLEAN ;
Mask_Longword = [LONG, UNSAFE] PACKED ARRAY [1..32] OF BOOLEAN ;

F_Complex_Number = [LONG(2),UNSAFE] PACKED ARRAY [1..2] OF UNSIGNED ;
D_Complex_Number = [QUAD(2),UNSAFE] PACKED ARRAY [1..4] OF UNSIGNED ;
G_Complex_Number = D_Complex_Number ;

Item_List_Cell = RECORD
		   CASE INTEGER OF
		     1: (			{ Normal Cell }
			 Buffer_Length : [WORD] 0..65535 ;
			 Item_Code     : [WORD] 0..65535 ;
			 Buffer_Addr   : UNSIGNED ;
			 Return_Addr   : UNSIGNED
			) ;
		     2: (			{ Terminator }
			 Terminator    : UNSIGNED
			) ;
		 END ;

Item_List_Template (Count:INTEGER) = ARRAY [1..Count] OF Item_List_Cell ;


CONST		{ Constant definitions for Pascal itself }

%INCLUDE 'SYS$LIBRARY:PASDEF.PAS /NOLIST'	{ Supplied by Digital }

%INCLUDE 'SYS$LIBRARY:PASSTATUS.PAS /NOLIST'	{ Supplied by Digital }


{ Here follow definitions and declarations for Run-Time Library and Utility
  facilities. }


%INCLUDE 'SYS_LIBRARY:UMLIB_PAS.PAS'

%INCLUDE 'ACLDEF.PAS'

%INCLUDE 'ARBDEF.PAS /NOLIST'		{ From SYS$LIBRARY:LIB.MLB }

%INCLUDE 'CLIDEF.PAS'

%INCLUDE 'CONVDEF.PAS'

%INCLUDE 'DCXDEF.PAS'

%INCLUDE 'EDTDEF.PAS'

%INCLUDE 'FDLDEF.PAS'

%INCLUDE 'FILDEF.PAS /NOLIST'		{ From SYS$LIBRARY:LIB.MLB }

%INCLUDE 'LBRDEF.PAS'

%INCLUDE 'NMADEF.PAS /NOLIST'		{ From SYS$LIBRARY:LIB.MLB }

%INCLUDE 'PASDEF.PAS'

%INCLUDE 'PSMDEF.PAS'

%INCLUDE 'SMBDEF.PAS'

%INCLUDE 'SMGDEFDEF.PAS /NOLIST'	{ From SYS$LIBRARY:STARLETSD }

%INCLUDE 'STATEDEF.PAS /NOLIST'		{ From SYS$LIBRARY:LIB.MLB }

%INCLUDE 'SYSDEF.PAS'

%INCLUDE 'TPUDEF.PAS'

%INCLUDE 'UAFDEF.PAS'

END.	{ of Module Paslib }
