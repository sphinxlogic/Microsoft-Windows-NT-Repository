{************************************************************************
*									*
*  LIBDEF								*
*									*
*  Declarations for LIB$ RTL Routines.					*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION LIB$ADAWI (
	Add : $WORD ;
	VAR Sum    : [VOLATILE] $WORD ;
	VAR Result : [VOLATILE] $WORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$ADD_TIMES (
	Time1 : $QUAD ;
	Time2 : $QUAD ;
	%REF Resultant_Time : [VOLATILE] $QUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$ADDX (
	%REF Addend_Array : [UNSAFE] ARRAY [$L1..$U1:INTEGER] OF INTEGER ;
	%REF Augend_Array : [UNSAFE] ARRAY [$L2..$U2:INTEGER] OF INTEGER ;
	%REF Resultant_Array : [UNSAFE] ARRAY [$L3..$U3:INTEGER] OF INTEGER ;
	Array_Length : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$ANALYZE_SDESC (
	Input_Descriptor : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Data_Length  : [VOLATILE] $UWORD ;
	VAR Data_Address : [VOLATILE,UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$ASN_WTH_MBX (
	Device_Name : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Maximum_Message_Size : INTEGER ;
	Buffer_Quota : INTEGER ;
	VAR Device_Channel  : [VOLATILE] $WORD ;
	VAR Mailbox_Channel : [VOLATILE] $WORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$AST_IN_PROG : [UNSAFE] UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$ATTACH (
	Process_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$BBCCI (
	Position : INTEGER ;
	Bit_Zero_Address : [UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$BBSSI (
	Position : INTEGER ;
	Bit_Zero_Address : [UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CALLG (		{ #### ????????? #### }
	%REF Argument_List : [UNSAFE] ARRAY [$L1..$U1:INTEGER] OF UNSIGNED ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION User_Procedure :
			[UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LIB$CHAR is not implemented here: use Pascal CHR() routine    ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$CONVERT_DATE_STRING (
	Date_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Date_Time : [VOLATILE] $QUAD ;
	VAR User_Context : [VOLATILE] UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	%REF Defaults : ARRAY [$L5..$U5:INTEGER] OF $UWORD ;
	%REF Defaulted_Fields : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CRC (
	%REF Crc_Table : ARRAY [$L1..$U1:INTEGER] OF INTEGER ;
	Initial_Crc : INTEGER ;
	Stream : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$CRC_TABLE (
	Polynomial_Coefficient : Mask_Longword ;
	%REF Crc_Table : ARRAY [$L2..$U2:INTEGER] OF INTEGER )
		; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CREATE_DIR (
	Device_Directory_Spec:
		 [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Owner_Uic : UNSIGNED := %IMMED 0 ;
	Protection_Enable : Mask_Word := %IMMED 0 ;
	Protection_Value  : [UNSAFE] $UWORD := %IMMED 0 ;
	Maximum_Versions : $UWORD := %IMMED 0 ;
	Relative_Volume_Number : $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CREATE_USER_VM_ZONE (
	VAR Zone_Id : [VOLATILE] UNSIGNED ;
	User_Argument : [UNSAFE] UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Allocation_Procedure :
		[UNSAFE] UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Deallocation_Procedure :
		[UNSAFE] UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Reset_Procedure :
		[UNSAFE] UNSIGNED := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Delete_Procedure :
		[UNSAFE] UNSIGNED := %IMMED 0 ;
	Zone_Name : [CLASS_S] PACKED ARRAY [$L7..$U7:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CREATE_VM_ZONE (
	VAR Zone_Id : [VOLATILE] UNSIGNED ;
	Algorithm : INTEGER := %IMMED 0 ;
	Algorithm_Argument : INTEGER := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Extend_Size : INTEGER := %IMMED 0 ;
	Initial_Size : INTEGER := %IMMED 0 ;
	Block_Size : INTEGER := %IMMED 0 ;
	Alignment : INTEGER := %IMMED 0 ;
	Page_Limit : INTEGER := %IMMED 0 ;
	Smallest_Block_Size : INTEGER := %IMMED 0 ;
	Zone_Name :
		[CLASS_S] PACKED ARRAY [$L11..$U11:INTEGER] OF CHAR := %IMMED 0;
	Number_Of_Areas : INTEGER := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] PROCEDURE Get_Page ;
	%IMMED [ASYNCHRONOUS,UNBOUND] PROCEDURE Free_Page )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$CRF_INS_KEY (		{ #### ????????? #### }
	%REF Control_Table : ARRAY [$L1..$U1:INTEGER] OF INTEGER ;
	Key_String : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Symbol_Value : INTEGER ;
	Flags : Mask_Longword ) ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$CRF_INS_REF (		{ #### ????????? #### }
	%REF Control_Table : ARRAY [$L1..$U1:INTEGER] OF INTEGER ;
	Longword_Integer_Key : INTEGER ;
	Reference_String : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR ;
	VAR Longword_Integer_Reference : [VOLATILE] INTEGER ;
	Ref_Definition_Indicator : INTEGER ) ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$CRF_OUTPUT (		{ #### ????????? #### }
	%REF Control_Table : ARRAY [$L1..$U1:INTEGER] OF INTEGER ;
	Output_Line_Width : INTEGER ;
	Page1 : INTEGER ;
	Page2 : INTEGER ;
	Mode_Indicator : INTEGER ;
	Delete_Save_Indicator : INTEGER ) ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CURRENCY (
	%DESCR Currency_String : VARYING [$L1] OF CHAR ;
	Resultant_Length : $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVT_DX_DX (		{ ### ????????? ### }
	%DESCR Source_Item : [UNSAFE] UNSIGNED ;
	%DESCR Destination_Item : [UNSAFE] UNSIGNED ;
	VAR Word_Integer_Dest_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVT_FROM_INTERNAL_TIME (
	Operation : UNSIGNED ;
	%REF Resultant_Time : [VOLATILE] UNSIGNED ;
	Input_Time : $QUAD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVTF_FROM_INTERNAL_TIME (
	Operation : UNSIGNED ;
	%REF Resultant_Time : [VOLATILE] REAL ;
	Input_Time : $QUAD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVT_TO_INTERNAL_TIME (
	Operation : UNSIGNED ;
	Input_Time : [UNSAFE] INTEGER ;
	%REF Resultant_Time : [VOLATILE] $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVTF_TO_INTERNAL_TIME (
	Operation : UNSIGNED ;
	Input_Time : REAL ;
	%REF Resultant_Time : [VOLATILE] $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVT_DTB (
	%IMMED Byte_Count : INTEGER ;
	%REF Numeric_String : PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	VAR Result : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVT_HTB (
	%IMMED Byte_Count : INTEGER ;
	%REF Numeric_String : PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	VAR Result : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVT_OTB (
	%IMMED Byte_Count : INTEGER ;
	%REF Numeric_String : PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	VAR Result : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$CVT_VECTIM (
	%REF Input_Time : PACKED ARRAY [$L1..$U1:INTEGER] OF $UWORD ;
	%REF Resultant_Time : [VOLATILE] $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DATE_TIME (
	%DESCR Date_Time_String : VARYING [$L1] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DAY (
	VAR Number_Of_Days : [VOLATILE] INTEGER ;
	User_Time : $UQUAD := %IMMED 0 ;
	VAR Day_Time : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DAY_OF_WEEK (
	User_Time : $UQUAD ;
	VAR Day_Number : [VOLATILE,UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DECODE_FAULT (
	%REF Signal_Arguments    : PACKED ARRAY [$L1..$U1:INTEGER] OF UNSIGNED ;
	%REF Mechanism_Arguments : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED ;
	%IMMED [UNBOUND,ASYNCHRONOUS] FUNCTION User_Procedure : UNSIGNED ;
	%IMMED Unspecified_User_Argument : [UNSAFE] UNSIGNED := %IMMED 0 ;
	%REF Instruction_Definitions :
			PACKED ARRAY [$L5..$U5:INTEGER] OF $UBYTE := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DEC_OVER (
	New_Setting : [UNSAFE] $UBYTE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DELETE_FILE (
	Filespec : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Default_Filespec :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Related_Filespec :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Success_Procedure : BOOLEAN := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Error_Procedure   : BOOLEAN := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Confirm_Procedure : BOOLEAN := %IMMED 0 ;
	%IMMED User_Specified_Argument : [UNSAFE] UNSIGNED := %IMMED 0 ;
	%DESCR Resultant_Name : VARYING [$L8] OF CHAR := %IMMED 0 ;
	VAR File_Scan_Context : [VOLATILE,UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DELETE_LOGICAL (
	Logical_Name : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Table_Name :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DELETE_SYMBOL (
	Symbol : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Table_Type_Indicator : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DELETE_VM_ZONE (
	Zone_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DIGIT_SEP (
	%DESCR Digit_Separator_String : VARYING [$L1] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DISABLE_CTRL (
	Disable_Mask : Mask_Longword ;
	VAR Old_Mask : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$DO_COMMAND (
	Command_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$EDIV (
	Longword_Integer_Divisor  : INTEGER ;
	Quadword_Integer_Dividend : $QUAD ;
	VAR Longword_Integer_Quotient  : [VOLATILE] INTEGER ;
	VAR Remainder : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$EMODD (
	Floating_Point_Multiplier : DOUBLE ;
	Multiplier_Extension : $BYTE ;
	Floating_Point_Multiplicand : DOUBLE ;
	VAR Integer_Portion : [VOLATILE] INTEGER ;
	VAR Fractional_Portion : [VOLATILE] DOUBLE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$EMODF (
	Floating_Point_Multiplier : REAL ;
	Multiplier_Extension : $BYTE ;
	Floating_Point_Multiplicand : REAL ;
	VAR Integer_Portion : [VOLATILE] INTEGER ;
	VAR Fractional_Portion : [VOLATILE] REAL )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$EMODG (
	Floating_Point_Multiplier : DOUBLE ;
	Multiplier_Extension : $UWORD ;
	Floating_Point_Multiplicand : DOUBLE ;
	VAR Integer_Portion : [VOLATILE] INTEGER ;
	VAR Fractional_Portion : [VOLATILE] DOUBLE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$EMODH (
	Floating_Point_Multiplier : QUADRUPLE ;
	Multiplier_Extension : $UWORD ;
	Floating_Point_Multiplicand : QUADRUPLE ;
	VAR Integer_Portion : [VOLATILE] INTEGER ;
	VAR Fractional_Portion : [VOLATILE] QUADRUPLE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$EMUL (
	Longword_Integer_Multiplier : INTEGER ;
	Longword_Integer_Multiplicand : INTEGER ;
	Addend : INTEGER ;
	VAR Product : [VOLATILE] $QUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$ENABLE_CTRL (
	Enable_Mask  : Mask_Longword ;
	VAR Old_Mask : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LIB$ESTABLISH is not implemented here: use Pascal ESTABLISH   ****
 ****  routine.							     ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$EXTV (
	Position : INTEGER ;
	SIZE : $UBYTE ;
	%IMMED Base_Address : [UNSAFE] UNSIGNED )
		: INTEGER ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$EXTZV (
	Position : INTEGER ;
	SIZE : $UBYTE ;
	%IMMED Base_Address : [UNSAFE] UNSIGNED )
		: INTEGER ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FFC (
	Position : INTEGER ;
	SIZE : $UBYTE ;
	Base : [UNSAFE] UNSIGNED ;
	VAR Find_Position : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FFS (
	Position : INTEGER ;
	SIZE : $UBYTE ;
	Base : [UNSAFE] UNSIGNED ;
	VAR Find_Position : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FID_TO_NAME (
	Device_Name : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	File_Id : ARRAY [$L2..$U2:INTEGER] OF $UWORD ;
	%DESCR Filespec : VARYING [$L3] OF CHAR ;
	VAR Filespec_Length : [VOLATILE] $UWORD := %IMMED 0 ;
	%REF Directory_Id : ARRAY [$L5..$U5:INTEGER] OF $UWORD := %IMMED 0 ;
	VAR Acp_Status : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FILE_SCAN (
	Fab : FAB$TYPE ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION User_Success_Procedure :UNSIGNED;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION User_Error_Procedure : UNSIGNED ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FILE_SCAN_END (
	VAR Fab : [VOLATILE] FAB$TYPE := %IMMED 0 ;
	VAR Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FIND_FILE (
	Filespec : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Resultant_Filespec : VARYING [$L2] OF CHAR ;
	VAR Context : [VOLATILE] UNSIGNED ;
	Default_Filespec :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Related_Filespec :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Status_Value : [VOLATILE] UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FIND_FILE_END (
	Context : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FIND_IMAGE_SYMBOL (
	Filename : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Symbol   : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	VAR Symbol_Value : [VOLATILE] INTEGER ;
	Image_Name :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FIND_VM_ZONE (
	VAR Context : [VOLATILE] UNSIGNED ;
	VAR Zone_Id : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS,UNBOUND] FUNCTION LIB$FIXUP_FLT (
	%REF Signal_Arguments : PACKED ARRAY [$L1..$U1:INTEGER] OF UNSIGNED ;
	%REF Mechanism_Arguments : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED ;
	New_Operand : REAL := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FLT_UNDER (
	New_Setting : [UNSAFE] $UBYTE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FORMAT_DATE_TIME (
	%DESCR Date_String : VARYING [$L1] OF CHAR ;
	%REF DATE : $UQUAD := %IMMED 0 ;
	VAR User_Context : [VOLATILE] UNSIGNED := %IMMED 0 ;
	VAR Date_Length  : [VOLATILE] INTEGER  := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FREE_DATE_TIME_CONTEXT (
	VAR User_Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FREE_EF (
	Event_Flag_Number : INTEGER )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 **** LIB$FREE_LUN is not implemented as it is unneccesary in Pascal ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$FREE_TIMER (
	VAR Handle_Address : [VOLATILE,UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FREE_VM (
	Number_Of_Bytes : INTEGER ;
	Base_Address : [UNSAFE] UNSIGNED ;
	Zone_Id : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$FREE_VM_PAGE (
	Number_Of_Pages : INTEGER ;
	Base_Address : [UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GETDVI (
	Item_Code : INTEGER ;
	Channel : $UWORD := %IMMED 0 ;
	Device_Name :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Longword_Integer_Value : [VOLATILE] INTEGER := %IMMED 0 ;
	%DESCR Resultant_String : VARYING [$L5] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GETJPI (
	Item_Code : INTEGER ;
	VAR Process_Id : [VOLATILE] UNSIGNED := %IMMED 0 ;
	Process_Name :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	%REF Resultant_Value :
	  [VOLATILE,UNSAFE] PACKED ARRAY [$L4..$U4:INTEGER] OF $UBYTE:=%IMMED 0;
	%DESCR Resultant_String : VARYING [$L5] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GETQUI (
	Function_Code : INTEGER ;
	Item_Code : INTEGER := %IMMED 0 ;
	Search_Number : INTEGER := %IMMED 0 ;
	Search_Name :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Search_Flags : [UNSAFE] UNSIGNED ;
	%REF Resultant_Value :
	  [VOLATILE,UNSAFE] PACKED ARRAY [$L6..$U6:INTEGER] OF $UBYTE:=%IMMED 0;
	%DESCR Resultant_String : VARYING [$L7] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GETSYI (
	Item_Code : INTEGER ;
	%REF Resultant_Value :
	  [VOLATILE,UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE:=%IMMED 0;
	%DESCR Resultant_String : VARYING [$L3] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 ;
	VAR Cluster_System_Id : [VOLATILE] UNSIGNED := %IMMED 0 ;
	Node_Name :
		[CLASS_S] PACKED ARRAY [$L6..$U6:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_COMMAND (
	%DESCR Resultant_String : VARYING [$L1] OF CHAR ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_COMMON (
	%DESCR Resultant_String : VARYING [$L1] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_DATE_FORMAT (
	%DESCR Format_String : VARYING [$L1] OF CHAR ;
	VAR User_Context : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_EF (
	VAR Event_Flag_Number : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_FOREIGN (
	%DESCR Resultant_String : VARYING [$L1] OF CHAR ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 ;
	%REF Flags : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_INPUT (
	%DESCR Resultant_String : VARYING [$L1] OF CHAR ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LIB$GET_LUN is not implemented as it is unneccesary in Pascal ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$GET_MAXIMUM_DATE_LENGTH (
	VAR Date_Length : [VOLATILE] INTEGER ;
	VAR User_Context : [VOLATILE] UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_SYMBOL (
	Symbol : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Resultant_String : VARYING [$L2] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 ;
	VAR Table_Type_Indicator : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_USERS_LANGUAGE (
	%DESCR Language : VARYING [$L1] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_VM (		{ ### ??????? ### }
	Number_Of_Bytes : INTEGER ;
	VAR Base_Address : [VOLATILE,UNSAFE] UNSIGNED ;
	Zone_Id : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$GET_VM_PAGE (	{ ### ??????? ### }
	Number_Of_Pages : INTEGER ;
	Base_Address : [UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LIB$ICHAR is not implemented: use the Pascal ORD()   routine  ****
 ****  LIB$INDEX is not implemented: use the Pascal INDEX() routine  ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$INIT_DATE_TIME_CONTEXT (
	VAR User_Context : [VOLATILE] UNSIGNED ;
	Component : INTEGER ;
	Init_String : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$INIT_TIMER (
	VAR Context : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$INSERT_TREE (
	VAR Treehead : UNSIGNED ;
	Symbol : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Flags : Mask_Longword ;
	[ASYNCHRONOUS] FUNCTION User_Compare_Routine : UNSIGNED ;
	[ASYNCHRONOUS] FUNCTION User_Allocation_Procedure: UNSIGNED ;
	VAR New_Node : [VOLATILE] UNSIGNED ;
	%IMMED User_Data : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;		{ RTL man. has INTEGER return }

[ASYNCHRONOUS] FUNCTION LIB$INSQHI (
	%REF Entry :
	   [VOLATILE,ALIGNED(3)] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	VAR Header : [VOLATILE,ALIGNED(3)] $QUAD ;
	Retry_Count : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$INSQTI (
	%REF Entry :
	   [VOLATILE,ALIGNED(3)] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	VAR Header : [VOLATILE,ALIGNED(3)] $QUAD ;
	Retry_Count : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$INSV (
	Longword_Integer_Source : INTEGER ;
	Position : INTEGER ;
	SIZE : $UBYTE ;
	VAR Base_Address : [UNSAFE] UNSIGNED )
		; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$INT_OVER (
	New_Setting : [UNSAFE] $UBYTE )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LIB$LEN  is not implemented here: use Pascal LENGTH() routine ****
 ****  LIB$LOCC is not implemented here: use Pascal INDEX()  routine ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$LOOKUP_KEY (
	Search_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Key_Table_Array : ARRAY [$L2..$U2:INTEGER] OF UNSIGNED ;
	VAR Key_Value : [VOLATILE] UNSIGNED := %IMMED 0 ;
	%DESCR Keyword_String : VARYING [$L4] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$LOOKUP_TREE (
	Treehead : UNSIGNED ;
	Symbol : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	[ASYNCHRONOUS] FUNCTION User_Compare_Routine : INTEGER ;
	VAR New_Node : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$LP_LINES : INTEGER ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LIB$MATCHC is not implemented here: use Pascal INDEX routine  ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$MATCH_COND (
	Match_Condition_Value : UNSIGNED ;
	Compare_Condition_Value : [UNSAFE,LIST] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$MOVC3 (
	Word_Integer_Length : $UWORD ;
	%REF Source : [UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%REF Destination : [UNSAFE] PACKED ARRAY [$L3..$U3:INTEGER] OF $UBYTE )
		; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$MOVC5 (
	Word_Integer_Source_Length : $UWORD ;
	%REF Source : [UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	Fill : [UNSAFE] $UBYTE ;
	Destination_Length : $UWORD ;
	%REF Destination : [UNSAFE] PACKED ARRAY [$L5..$U5:INTEGER] OF $UBYTE )
		; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$MOVTC (
	Source_String     : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Fill_Character    : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Translation_Table : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR ;
	%DESCR Destination_String : VARYING [$L4] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$MOVTUC (
	Source_String     : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Stop_Character    : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Translation_Table : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR ;
	%DESCR Destination_String : VARYING [$L4] OF CHAR ;
	Fill_Character :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$MULT_DELTA_TIME (
	Multiplier : INTEGER ;
	%REF Delta_Time : [VOLATILE] $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$MULTF_DELTA_TIME (
	Multiplier : REAL ;
	%REF Delta_Time : [VOLATILE] $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$PAUSE : UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$POLYD (
	Polynomial_Argument : DOUBLE ;
	Degree : $WORD ;
	Coefficient : PACKED ARRAY [$L3..$U3:INTEGER] OF DOUBLE ;
	VAR Floating_Point_Result : [VOLATILE] DOUBLE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$POLYF (
	Polynomial_Argument : REAL ;
	Degree : $WORD ;
	Coefficient : PACKED ARRAY [$L3..$U3:INTEGER] OF REAL ;
	VAR Floating_Point_Result : [VOLATILE] REAL )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$POLYG (
	Polynomial_Argument : DOUBLE ;
	Degree : $WORD ;
	Coefficient : PACKED ARRAY [$L3..$U3:INTEGER] OF DOUBLE ;
	VAR Floating_Point_Result : [VOLATILE] DOUBLE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$POLYH (
	Polynomial_Argument : QUADRUPLE ;
	Degree : $WORD ;
	Coefficient : PACKED ARRAY [$L3..$U3:INTEGER] OF QUADRUPLE ;
	VAR Floating_Point_Result : [VOLATILE] QUADRUPLE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$PUT_COMMON (
	Source_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] $WORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$PUT_OUTPUT (
	Message_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$RADIX_POINT (
	%DESCR Radix_Point_String : VARYING [$L1] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$REMQHI (
	VAR Header : [VOLATILE,ALIGNED(3)] $QUAD ;
	VAR Remque_Address : [VOLATILE,UNSAFE] UNSIGNED ;
	Retry_Count : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$REMQTI (
	VAR Header : [VOLATILE,ALIGNED(3)] $QUAD ;
	VAR Remque_Address : [VOLATILE,UNSAFE] UNSIGNED ;
	Retry_Count : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$RENAME_FILE (
	Old_Filespec : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	New_Filespec : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Default_Filespec :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Related_Filespec :
		[CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION
			User_Success_Procedure : BOOLEAN := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION
			User_Error_Procedure   : BOOLEAN := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION
			User_Confirm_Procedure : BOOLEAN := %IMMED 0 ;
	User_Specified_Argument : [UNSAFE] UNSIGNED := %IMMED 0 ;
	%DESCR Old_Resultant_Name : VARYING [$L10] OF CHAR := %IMMED 0 ;
	%DESCR New_Resultant_Name : VARYING [$L11] OF CHAR := %IMMED 0 ;
	VAR File_Scan_Context : [VOLATILE,UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$RESERVE_EF (
	Event_Flag_Number : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$RESET_VM_ZONE (
	Zone_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

{************************************************************************
 ****								     ****
 ****  LIB$REVERT is not implemented here: use Pascal REVERT routine ****
 ****								     ****
 ************************************************************************}

[ASYNCHRONOUS] FUNCTION LIB$RUN_PROGRAM (
	Program_Name : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SCANC (
	Source_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Table_Array : [UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	Byte_Integer_Mask : [UNSAFE] $UBYTE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SCOPY_DXDX (
	Source_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Destination_String : VARYING [$L2] OF CHAR )
		: $UWORD ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SCOPY_R_DX (
	Word_Integer_Source_Length : $UWORD ;
	%REF Source_String_Address : PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	%DESCR Destination_String : VARYING [$L3] OF CHAR )
		: $UWORD ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SET_LOGICAL (
	Logical_Name  : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Value_String :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Table : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Attributes : UNSIGNED := %IMMED 0 ;
	%REF Item_List :
		[UNSAFE] ARRAY [$L5..$U5:INTEGER] OF $UBYTE := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SET_SYMBOL (
	Symbol : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Value_String : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Table_Type_Indicator : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SFREE1_DD (
	VAR Descriptor_Address : [VOLATILE] $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SFREEN_DD (
	Number_Of_Descriptors : UNSIGNED ;
	%REF First_Descriptor_Array :
		[VOLATILE] ARRAY [$L2..$U2:INTEGER] OF $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SGET1_DD (
	Word_Integer_Length  : $UWORD ;
	%REF Descriptor_Part : [VOLATILE] $UQUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SHOW_TIMER (
	Handle_Address : [UNSAFE] UNSIGNED := %IMMED 0 ;
	Code : INTEGER := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Action_Procedure : UNSIGNED := %IMMED 0 ;
	%IMMED User_Argument_Value : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SHOW_VM (
	Code : INTEGER := %IMMED 0 ;
	[ASYNCHRONOUS] FUNCTION User_Action_Procedure : UNSIGNED := %IMMED 0 ;
	User_Specified_Argument : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SHOW_VM_ZONE (
	Zone_Id : UNSIGNED ;
	Detail_Level : INTEGER := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] FUNCTION
		User_Action_Procedure : UNSIGNED := %IMMED 0 ;
	%IMMED User_Arg : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$SIGNAL (
	%IMMED Condition_Value  : [UNSAFE] UNSIGNED ;
	%IMMED Other_Parameters : [UNSAFE,LIST] UNSIGNED := %IMMED 0 )
		; EXTERNAL ;

[ASYNCHRONOUS,UNBOUND] FUNCTION LIB$SIG_TO_RET (
	%REF Signal_Arguments : PACKED ARRAY [$L1..$U1:INTEGER] OF UNSIGNED ;
	%REF Mechanism_Arguments : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS,UNBOUND] FUNCTION LIB$SIG_TO_STOP (
	%REF Signal_Arguments : PACKED ARRAY [$L1..$U1:INTEGER] OF UNSIGNED ;
	%REF Mechanism_Arguments : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS,UNBOUND] FUNCTION LIB$SIM_TRAP (
	%REF Signal_Arguments : PACKED ARRAY [$L1..$U1:INTEGER] OF UNSIGNED ;
	%REF Mechanism_Arguments : PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SKPC (
	Character_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	Source_String    : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SPANC (
	Source_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%REF Table_Array : [UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	Byte_Integer_Mask : Mask_Byte )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SPAWN (
	Command_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR := %IMMED 0 ;
	Input_File :
		[CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 ;
	Output_File :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Process_Name :
		[CLASS_S] PACKED ARRAY [$L5..$U5:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Process_Id : UNSIGNED := %IMMED 0 ;
	VAR Completion_Status : UNSIGNED := %IMMED 0 ;
	Byte_Integer_Event_Flag_Num : $UBYTE := %IMMED 0 ;
	%IMMED [ASYNCHRONOUS,UNBOUND] PROCEDURE Ast_Address ;
	Varying_Ast_Argument : [UNSAFE] UNSIGNED := %IMMED 0 ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L11..$U11:INTEGER] OF CHAR := %IMMED 0;
	Cli : 	[CLASS_S] PACKED ARRAY [$L12..$U12:INTEGER] OF CHAR := %IMMED 0)
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$STAT_TIMER (
	Code : INTEGER ;
	%REF Value_Argument :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF UNSIGNED ;
	Handle_Address : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$STAT_VM (
	Code : INTEGER ;
	VAR Value_Argument : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] PROCEDURE LIB$STOP (
	%IMMED Condition_Value  : [UNSAFE] UNSIGNED ;
	%IMMED Other_Parameters : [UNSAFE,LIST] UNSIGNED := %IMMED 0 )
		; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SUB_TIMES (
	Time1 : $QUAD ;
	Time2 : $QUAD ;
	%REF Resultant_Time : [VOLATILE] $QUAD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SUBX (
	%REF Minuend_Array :
		[UNSAFE] PACKED ARRAY [$L1..$U1:INTEGER] OF INTEGER ;
	%REF Subtrahend_Array :
		[UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF INTEGER ;
	%REF Difference_Array :
		[UNSAFE] PACKED ARRAY [$L3..$U3:INTEGER] OF INTEGER ;
	Array_Length : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SYS_ASCTIM (
	VAR Resultant_Length : [VOLATILE] $WORD := %IMMED 0 ;
	%DESCR Time_String : VARYING [$L2] OF CHAR ;
	User_Time : $QUAD := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SYS_FAO (
	Character_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] $WORD := %IMMED 0 ;
	%DESCR Resultant_String : VARYING [$L3] OF CHAR ;
	%REF Directive_Argument : [UNSAFE,LIST] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SYS_FAOL (
	Character_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Resultant_Length : [VOLATILE] $WORD := %IMMED 0 ;
	%DESCR Resultant_String : VARYING [$L3] OF CHAR ;
	%REF Directive_Argument : [UNSAFE,LIST] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$SYS_GETMSG (
	Message_Id : INTEGER ;
	VAR Message_Length : [VOLATILE] $WORD := %IMMED 0 ;
	%DESCR Destination_String : VARYING [$L3] OF CHAR ;
	Flags : Mask_Longword := %IMMED 0 ;
	%REF Unsigned_Resultant_Array :
		[UNSAFE] PACKED ARRAY [$L5..$U5:INTEGER] OF $UBYTE := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

{ #############  HMMMMMMMMMMMMM???????????  GONE ??????????????  ##########
[ASYNCHRONOUS] FUNCTION LIB$SYS_TRNLOG (
	Logical_Name : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	VAR Destination_Length : [VOLATILE] $WORD := %IMMED 0 ;
	%DESCR Destination_String : VARYING [$L3] OF CHAR ;
	VAR Table : [VOLATILE] $BYTE := %IMMED 0 ;
	VAR Acc_Mode : [VOLATILE] $BYTE := %IMMED 0 ;
	Dsb_Mask : $UBYTE := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;
############################################################################ }

[ASYNCHRONOUS] FUNCTION LIB$TPARSE (		{ #### ????????????? #### }
	%REF Argument_Block :
		[UNSAFE] PACKED ARRAY [$L1..$U1:INTEGER] OF $UBYTE ;
	%REF State_Table : [UNSAFE] PACKED ARRAY [$L2..$U2:INTEGER] OF $UBYTE ;
	%REF Key_Table   : [UNSAFE] PACKED ARRAY [$L3..$U3:INTEGER] OF $UBYTE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$TRA_ASC_EBC (
	Source_String : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Byte_Integer_Dest_String : VARYING [$L2] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$TRA_EBC_ASC (
	Byte_Integer_Source_String :
		[CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR Destination_String : VARYING [$L2] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$TRAVERSE_TREE (
	Treehead : UNSIGNED ;
	[ASYNCHRONOUS] FUNCTION User_Action_Procedure : UNSIGNED ;
	User_Data_Address : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$TRIM_FILESPEC (
	Old_Filespec : [CLASS_S] PACKED ARRAY [$L1..$U1:INTEGER] OF CHAR ;
	%DESCR New_Filespec : VARYING [$L2] OF CHAR ;
	Word_Integer_Width : $UWORD := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$VERIFY_VM_ZONE (
	Zone_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION LIB$WAIT (
	Seconds : REAL )
		: UNSIGNED ; EXTERNAL ;

