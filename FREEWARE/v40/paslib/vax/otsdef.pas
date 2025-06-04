{************************************************************************
*									*
*  OTSDEF								*
*									*
*  Declarations for OTS$ RTL routines.					*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION OTS$CNVOUT (
	D_G_Or_H_Float_Pt_Input_Val : DOUBLE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Digits_In_Fraction : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CNVOUT_G (
	D_G_Or_H_Float_Pt_Input_Val : DOUBLE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Digits_In_Fraction : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CNVOUT_H (
	D_G_Or_H_Float_Pt_Input_Val : QUADRUPLE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Digits_In_Fraction : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_L_TB (
	Varying_Input_Value :
		[UNSAFE] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Number_Of_Digits : INTEGER := %IMMED 0 ;
	%IMMED Input_Value_Size : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_L_TI (
	Varying_Input_Value :
		[UNSAFE] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Number_Of_Digits : INTEGER := %IMMED 0 ;
	%IMMED Input_Value_Size : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_L_TL (
	Longword_Integer_Value : INTEGER ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_L_TO (
	Varying_Input_Value :
		[UNSAFE] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Number_Of_Digits : INTEGER := %IMMED 0 ;
	%IMMED Input_Value_Size : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_L_TU (
	Varying_Input_Value :
		[UNSAFE] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Number_Of_Digits : INTEGER := %IMMED 0 ;
	%IMMED Input_Value_Size : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_L_TZ (
	Varying_Input_Value :
		[UNSAFE] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	%STDESCR Fixed_Length_Resultant_String :
		PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%IMMED Number_Of_Digits : INTEGER := %IMMED 0 ;
	%IMMED Input_Value_Size : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_TB_L (
	Input_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Varying_Output_Value :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%IMMED Output_Value_Size : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_TI_L (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Varying_Output_Value :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%IMMED Output_Value_Size : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_TL_L (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Varying_Output_Value :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%IMMED Output_Value_Size : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_TO_L (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Varying_Output_Value :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%IMMED Output_Value_Size : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_TU_L (
	Fixed_Length_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Varying_Output_Value :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%IMMED Output_Value_Size : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_TZ_L (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Varying_Output_Value :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%IMMED Output_Value_Size : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_T_D (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Floating_Point_Value : [VOLATILE] DOUBLE ;
	%IMMED Digits_In_Fraction : UNSIGNED := %IMMED 0 ;
	%IMMED Scale_Factor : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 ;
	VAR Extension_Bits : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_T_F (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Floating_Point_Value : [VOLATILE] REAL ;
	%IMMED Digits_In_Fraction : UNSIGNED := %IMMED 0 ;
	%IMMED Scale_Factor : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 ;
	VAR Extension_Bits : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_T_G (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Floating_Point_Value : [VOLATILE] DOUBLE ;
	%IMMED Digits_In_Fraction : UNSIGNED := %IMMED 0 ;
	%IMMED Scale_Factor : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 ;
	VAR Extension_Bits : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$CVT_T_H (
	Fixed_Or_Dynamic_Input_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Floating_Point_Value : [VOLATILE] QUADRUPLE ;
	%IMMED Digits_In_Fraction : UNSIGNED := %IMMED 0 ;
	%IMMED Scale_Factor : INTEGER := %IMMED 0 ;
	%IMMED Flags_Value: Mask_Longword := %IMMED 0 ;
	VAR Extension_Bits : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

{****************************************************************************
 ****									 ****
 ****  The following RTL functions which implement Complex Arithmetic    ****
 ****  are NOT supported in these declarations.				 ****

 OTS$DIVC
 OTS$DIVCD_R3
 OTS$DIVCG_R3

*****************************************************************************}

[ASYNCHRONOUS] FUNCTION OTS$DIV_PK_LONG (
	Packed_Decimal_Dividend : [UNSAFE] UNSIGNED ;
	Packed_Decimal_Divisor : [UNSAFE] UNSIGNED ;
	%IMMED Divisor_Precision : $WORD ;
	VAR Packed_Decimal_Quotient : [VOLATILE,UNSAFE] UNSIGNED ;
	%IMMED Quotient_Precision : $WORD ;
	%IMMED Precision_Data : $WORD ;
	%IMMED Scale_Data : $WORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$DIV_PK_SHORT (
	Packed_Decimal_Dividend : [UNSAFE] UNSIGNED ;
	Packed_Decimal_Divisor : [UNSAFE] UNSIGNED ;
	%IMMED Divisor_Precision : $WORD ;
	VAR Packed_Decimal_Quotient : [VOLATILE,UNSAFE] UNSIGNED ;
	%IMMED Quotient_Precision : $WORD ;
	%IMMED Precision_Data : $WORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE OTS$MOVE3 (
	%IMMED Length_Value : INTEGER ;
	%REF Source_Array : PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%REF Destination_Array :
		PACKED ARRAY [$L3..$U3:INTEGER] OF $UBYTE ) ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE OTS$MOVE5 (
	%IMMED Longword_Int_Source_Length : INTEGER ;
	%REF Source_Array : PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%IMMED Fill_Value : [UNSAFE] $UBYTE ;
	%IMMED Longword_Int_Dest_Length : INTEGER ;
	%REF Destination_Array :
		PACKED ARRAY [$L5..$U5:INTEGER] OF $UBYTE ) ; EXTERNAL ;

{****************************************************************************
 ****									 ****
 ****  The following RTL functions which implement Complex Arithmetic    ****
 ****  are NOT supported in these declarations.				 ****

 OTS$MULCD_R3
 OTS$MULCG_R3
 OTS$POWCC
 OTS$POWCDCD_R3
 OTS$POWCGCG_R3
 OTS$POWCJ
 OTS$POWCDJ_R3
 OTS$POWCGJ_R3

*****************************************************************************}

{****************************************************************************
 ****									 ****
 ****  The following RTL functions are all called indirectly in Pascal   ****
 ****  when using the Arithmetic Exponentiation Operator on Operands     ****
 ****  of appropriate types.  Hence, no declaration is required.	 ****
 ****  Simply use normal exponentiation (e.g. 91643542357487.43 ** -7)   ****

 OTS$POWDD
 OTS$POWDJ
 OTS$POWDR
 OTS$POWGG
 OTS$POWGJ
 OTS$POWHH_R3
 OTS$POWHJ_R3
 OTS$POWII
 OTS$POWJJ
 OTS$POWLULU
 OTS$POWRD
 OTS$POWRR
 OTS$POWRJ
 OTS$POWRLU
 OTS$POWDLU
 OTS$POWGLU
 OTS$POWHLU_R3

*****************************************************************************}

[ASYNCHRONOUS] FUNCTION OTS$SCOPY_DXDX (
	Source_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Destination_String : VARYING [$L2] OF CHAR )
		: $UWORD ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION OTS$SCOPY_R_DX (
	%IMMED Word_Int_Source_Length_Val : $UWORD ;
	%REF Source_String_Address : PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%DESCR Destination_String : VARYING [$L3] OF CHAR )
		: $UWORD ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE OTS$SFREE1_DD (
	VAR Dynamic_Descriptor : [VOLATILE] $UQUAD) ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE OTS$SFREEN_DD (
	%IMMED Descriptor_Count_Value : UNSIGNED ;
	VAR First_Descriptor : [VOLATILE] $UQUAD) ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE OTS$SGET1_DD (
	%IMMED Word_Integer_Length_Value : $UWORD ;
	VAR Dynamic_Descriptor : [VOLATILE] $UQUAD) ; EXTERNAL ;

