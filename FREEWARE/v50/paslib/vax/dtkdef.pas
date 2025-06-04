{************************************************************************
*									*
*  DTKDEF								*
*									*
*  Declarations for DTK$ (DECtalk) RTL routines.			*
*									*
************************************************************************}

[ASYNCHRONOUS] FUNCTION DTK$ANSWER_PHONE (
	Voice_Id : UNSIGNED ;
	Number_Of_Rings : INTEGER := %IMMED 0 ;
	TEXT : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$CHECK_HDWR_STATUS (
	Voice_Id : UNSIGNED ;
	%REF Hdrwr_Status : Mask_Longword )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$DIAL_PHONE (
	Voice_Id : UNSIGNED ;
	Phone_Number : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Dial_Mode : Mask_Longword := %IMMED 0 ;
	TEXT : [CLASS_S] PACKED ARRAY [$L4..$U4:INTEGER] OF CHAR := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$HANGUP_PHONE (
	Voice_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$INITIALIZE (
	VAR Voice_Id : [VOLATILE] UNSIGNED ;
	Output_Device : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	VAR Device_Type : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$LOAD_DICTIONARY (
	Voice_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Substitution : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$READ_KEYSTROKE (
	Voice_Id : UNSIGNED ;
	VAR Key_Code : [VOLATILE] INTEGER ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$READ_STRING (
	Voice_Id : UNSIGNED ;
	%DESCR Resultant_String : VARYING [$L2] OF CHAR ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 ;
	Longword_Integer_Termin_Code : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$RETURN_LAST_INDEX (
	Voice_Id : UNSIGNED ;
	VAR P_Index : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SET_INDEX (
	Voice_Id : UNSIGNED ;
	P_Index : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SET_KEYPAD_MODE (
	Voice_Id : UNSIGNED ;
	Keypad_Mode : Mask_Longword )
		:UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SET_LOGGING_MODE (
	Voice_Id : UNSIGNED ;
	New_Mode : Mask_Longword := %IMMED 0 ;
	VAR Old_Mode : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SET_MODE (
	Voice_Id : UNSIGNED ;
	New_Mode : Mask_Longword := %IMMED 0 ;
	VAR Old_Mode : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SET_SPEECH_MODE (
	Voice_Id : UNSIGNED ;
	New_Mode : Mask_Longword ;
	VAR Old_Mode : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SET_TERMINAL_MODE (
	Voice_Id : UNSIGNED ;
	New_Mode : Mask_Longword := %IMMED 0 ;
	VAR Old_Mode : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SET_VOICE (
	Voice_Id : UNSIGNED ;
	New_Voice : INTEGER := %IMMED 0 ;
	Speech_Rate : INTEGER := %IMMED 0 ;
	Comma_Pause : UNSIGNED := %IMMED 0 ;
	Period_Pause : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SPEAK_FILE (
	Voice_Id : UNSIGNED ;
	Filespec : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Completion_Mode : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SPEAK_PHONEMIC_TEXT (
	Voice_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Completion_Mode : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SPEAK_TEXT (
	Voice_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Completion_Mode : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$SPELL_TEXT (
	Voice_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [$L2..$U2:INTEGER] OF CHAR ;
	Completion_Mode : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION DTK$TERMINATE (
	Voice_Id : UNSIGNED )
		:UNSIGNED ; EXTERNAL ;

