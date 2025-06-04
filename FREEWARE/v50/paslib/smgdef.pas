{************************************************************************
*									*
*  SMGDEF								*
*									*
*  Declarations for SMG$ (Screen Management facility) RTL routines.	*
*									*
************************************************************************}


[ASYNCHRONOUS] FUNCTION SMG$ADD_KEY_DEF (
	Key_Table_Id : UNSIGNED ;
	Key_Name : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	If_State : [CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR := %IMMED 0 ;
	Attributes : Mask_Longword := %IMMED 0 ;
	Equivalence_String :
		[CLASS_S] PACKED ARRAY [E..F:INTEGER] OF CHAR:= %IMMED 0 ;
	State_String : [CLASS_S] PACKED ARRAY [G..H:INTEGER] OF CHAR:= %IMMED 0)
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$BEGIN_DISPLAY_UPDATE (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$BEGIN_PASTEBOARD_UPDATE (
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CANCEL_INPUT (
	Keyboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CHANGE_PBD_CHARACTERISTICS (
	Pasteboard_Id : UNSIGNED ;
	Desired_Width : INTEGER := %IMMED 0 ;
	VAR Width : [VOLATILE] INTEGER := %IMMED 0 ;
	Desired_Height : INTEGER := %IMMED 0 ;
	VAR Height : [VOLATILE] INTEGER := %IMMED 0 ;
	Desired_Background_Color : UNSIGNED := %IMMED 0 ;
	VAR Background_Color : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CHANGE_RENDITION (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER ;
	Start_Column : INTEGER ;
	Number_Of_Rows : INTEGER ;
	Number_Of_Columns : INTEGER ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CHANGE_VIEWPORT (
	Display_Id : UNSIGNED ;
	Viewport_Row_Start : INTEGER := %IMMED 0 ;
	Viewport_Column_Start : INTEGER := %IMMED 0 ;
	Viewport_Number_Rows : INTEGER := %IMMED 0 ;
	Viewport_Number_Columns : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CHANGE_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED ;
	Number_Of_Rows : INTEGER := %IMMED 0 ;
	Number_Of_Columns : INTEGER := %IMMED 0 ;
	Display_Attributes : Mask_Longword := %IMMED 0 ;
	Video_Attributes   : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CHECK_FOR_OCCLUSION (
	Display_Id : UNSIGNED ;
	Pasteboard_Id : UNSIGNED ;
	VAR Occlusion_State : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CONTROL_MODE (
	Pasteboard_Id : UNSIGNED ;
	New_Mode : UNSIGNED := %IMMED 0 ;
	VAR Old_Mode : [VOLATILE] UNSIGNED := %IMMED 0 ;
	Buffer_Size : $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$COPY_VIRTUAL_DISPLAY (
	Current_Display_Id : UNSIGNED ;
	VAR New_Display_Id : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CREATE_KEY_TABLE (
	VAR Key_Table_Id : [VOLATILE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CREATE_MENU (
	Display_Id : UNSIGNED ;
	Choices : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Menu_Type : Mask_Longword := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Row : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CREATE_PASTEBOARD (
	VAR Pasteboard_Id : [VOLATILE] UNSIGNED ;
	Output_Device : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR:=%IMMED 0;
	VAR Number_Of_Pasteboard_Rows : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Number_Of_Pasteboard_Columns : [VOLATILE] INTEGER := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	%REF Type_Of_Terminal : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CREATE_SUBPROCESS (
	Display_Id : UNSIGNED ;
	%IMMED [UNBOUND, ASYNCHRONOUS] PROCEDURE Ast_Routine := %IMMED 0 ;
	%IMMED Ast_Argument : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CREATE_VIRTUAL_DISPLAY (
	Number_Of_Rows : INTEGER ;
	Number_Of_Columns : INTEGER ;
	VAR New_Display_Id : [VOLATILE] UNSIGNED ;
	Display_Attributes : Mask_Longword := %IMMED 0 ;
	Video_Attributes   : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CREATE_VIRTUAL_KEYBOARD (
	VAR Keyboard_Id : [VOLATILE] UNSIGNED ;
	Input_Device :
		[CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	Default_Filespec :
		[CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR := %IMMED 0 ;
	%DESCR Resultant_Filespec : VARYING [E] OF CHAR := %IMMED 0 ;
	Recall_Size : $UBYTE := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CREATE_VIEWPORT (
	Display_Id : UNSIGNED ;
	Viewport_Row_Start : INTEGER ;
	Viewport_Column_Start : INTEGER ;
	Viewport_Number_Rows : INTEGER ;
	Viewport_Number_Columns : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CURSOR_COLUMN (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$CURSOR_ROW (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DEFINE_KEY (
	Key_Table_Id : UNSIGNED ;
	Command_String : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DEL_TERM_TABLE
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_CHARS (
	Display_Id : UNSIGNED ;
	Number_Of_Characters : INTEGER ;
	Start_Row : INTEGER ;
	Start_Column : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_KEY_DEF (
	Key_Table_Id : UNSIGNED ;
	Key_Name : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	If_State : [CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_LINE (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER ;
	Number_Of_Rows : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_MENU (
	Display_Id : UNSIGNED ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_PASTEBOARD (
	Pasteboard_Id : UNSIGNED ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_SUBPROCESS (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_VIEWPORT (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DELETE_VIRTUAL_KEYBOARD (
	Keyboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DISABLE_BROADCAST_TRAPPING (
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DISABLE_UNSOLICITED_INPUT (
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DRAW_CHAR (
	Display_Id : UNSIGNED ;
	Flags : Mask_Longword ;
	Row : INTEGER := %IMMED 0 ;
	Column : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DRAW_LINE (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER ;
	Start_Column : INTEGER ;
	End_Row : INTEGER ;
	End_Column : INTEGER ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$DRAW_RECTANGLE (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER ;
	Start_Column : INTEGER ;
	End_Row : INTEGER ;
	End_Column : INTEGER ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$ENABLE_UNSOLICITED_INPUT (
	Pasteboard_Id : UNSIGNED ;
	%IMMED [UNBOUND, ASYNCHRONOUS] PROCEDURE Ast_Routine ;
	%IMMED Ast_Argument : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$END_DISPLAY_UPDATE (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$END_PASTEBOARD_UPDATE (
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$ERASE_CHARS (
	Display_Id : UNSIGNED ;
	Number_Of_Characters : INTEGER ;
	Start_Row : INTEGER ;
	Start_Column : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$ERASE_COLUMN (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER := %IMMED 0 ;
	Column_Number : INTEGER := %IMMED 0 ;
	End_Row : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$ERASE_DISPLAY (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER := %IMMED 0 ;
	Start_Column : INTEGER := %IMMED 0 ;
	End_Row : INTEGER := %IMMED 0 ;
	End_Column : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$ERASE_LINE (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER := %IMMED 0 ;
	Start_Column : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$ERASE_PASTEBOARD (
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$EXECUTE_COMMAND (
	Display_Id : UNSIGNED ;
	Command_Desc : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Flags : Mask_Longword := %IMMED 0 ;
	VAR Ret_Status : [VOLATILE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$FIND_CURSOR_DISPLAY (
	Pasteboard_Id : UNSIGNED ;
	VAR Display_Id : [VOLATILE] UNSIGNED ;
	Pasteboard_Row : INTEGER := %IMMED 0 ;
	Pasteboard_Column : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$FLUSH_BUFFER (
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_BROADCAST_MESSAGE (
	Pasteboard_Id : UNSIGNED ;
	%DESCR Message : VARYING [A] OF CHAR := %IMMED 0 ;
	VAR Message_Length : [VOLATILE] $UWORD ;
	VAR Message_Type   : [VOLATILE] $UWORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_CHAR_AT_PHYSICAL_CURSOR (
	Pasteboard_Id : UNSIGNED ;
	VAR Character_Code : [VOLATILE] $UBYTE )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_DISPLAY_ATTR (
	Display_Id : UNSIGNED ;
	VAR Height : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Width : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Display_Attributes : [VOLATILE] Mask_Longword := %IMMED 0 ;
	VAR Video_Attributes   : [VOLATILE] Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 ;
	VAR Flags : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_KEY_DEF (
	Key_Table_Id : UNSIGNED ;
	Key_Name : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	If_State : [CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Attributes : [VOLATILE] Mask_Longword := %IMMED 0 ;
	%DESCR Equivalence_String : VARYING [E] OF CHAR := %IMMED 0 ;
	%DESCR State_String : VARYING [F] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_KEYBOARD_ATTRIBUTES (
	Keyboard_Id : UNSIGNED ;
	P_Kit : [UNSAFE] UNSIGNED ;
	P_Kit_Size : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_NUMERIC_DATA (
	Termtable_Address : [UNSAFE] UNSIGNED ;
	Request_Code : INTEGER ;
	VAR Buffer_Address : [VOLATILE,UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_PASTEBOARD_ATTRIBUTES (
	Pasteboard_Id : UNSIGNED ;
	%REF Pasteboard_Info_Table :
		[UNSAFE] PACKED ARRAY [A..B:INTEGER] OF $UBYTE ;
	Pasteboard_Info_Table_Size : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_PASTING_INFO (
	Display_Id : UNSIGNED ;
	Pasteboard_Id : UNSIGNED ;
	VAR Flags : [VOLATILE] Mask_Longword := %IMMED 0 ;
	VAR Pasteboard_Row : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Pasteboard_Column : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_TERM_DATA (
	Termtable_Address : [UNSAFE] UNSIGNED ;
	Request_Code : UNSIGNED ;
	Maximum_Buffer_Length : INTEGER ;
	VAR Return_Length : [VOLATILE] INTEGER ;
	VAR Buffer_Address : [VOLATILE,UNSAFE] UNSIGNED ;
	Input_Argument_Vector :
		PACKED ARRAY [A..B:INTEGER] OF UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$GET_VIEWPORT_CHAR (
	Display_Id : UNSIGNED ;
	VAR Viewport_Row_Start      : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Viewport_Column_Start   : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Viewport_Number_Rows    : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Viewport_Number_Columns : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$HOME_CURSOR (
	Display_Id : UNSIGNED ;
	Position_Code : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$INIT_TERM_TABLE (
	Terminal_Name : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	VAR Termtable_Address : [VOLATILE,UNSAFE] UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$INIT_TERM_TABLE_BY_TYPE (
	Terminal_Type : $BYTE ;
	VAR Termtable_Address : [VOLATILE,UNSAFE] UNSIGNED ;
	%DESCR Terminal_Name : VARYING [A] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$INSERT_CHARS (
	Display_Id : UNSIGNED ;
	Character_String : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Start_Row : INTEGER ;
	Start_Column : INTEGER ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$INSERT_LINE (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER ;
	Character_String :
		[CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	Direction : UNSIGNED := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$INVALIDATE_DISPLAY (
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$KEYCODE_TO_NAME (
	Key_Code : $UWORD ;
	%DESCR Key_Name : VARYING [A] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$LABEL_BORDER (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	Position_Code : UNSIGNED := %IMMED 0 ;
	Units : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$LIST_KEY_DEFS (
	Key_Table_Id : UNSIGNED ;
	VAR Context : [VOLATILE] INTEGER ;
	%DESCR Key_Name : VARYING [A] OF CHAR := %IMMED 0 ;
	%DESCR If_State : VARYING [B] OF CHAR := %IMMED 0 ;
	VAR Attributes : [VOLATILE] Mask_Longword := %IMMED 0 ;
	%DESCR Equivalence_String : VARYING [C] OF CHAR := %IMMED 0 ;
	%DESCR State_String : VARYING [D] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$LIST_PASTING_ORDER (
	Pasteboard_Id : UNSIGNED ;
	VAR Context : [VOLATILE] UNSIGNED ;
	VAR Display_Id : [VOLATILE] UNSIGNED ;
	VAR Pasteboard_Row : [VOLATILE] INTEGER := %IMMED 0 ;
	VAR Pasteboard_Column : [VOLATILE] INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$LOAD_KEY_DEFS (
	Key_Table_Id : UNSIGNED ;
	Filespec : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Default_Filespec :
		   [CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$LOAD_VIRTUAL_DISPLAY (
	VAR Display_Id : [VOLATILE] UNSIGNED ;
	Filespec : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$MOVE_TEXT (
	Display_Id : UNSIGNED ;
	Top_Left_Row    : UNSIGNED ;
	Top_Left_Column : UNSIGNED ;
	Bottom_Right_Row    : UNSIGNED ;
	Bottom_Right_Column : UNSIGNED ;
	Display_Id2 : UNSIGNED ;
	Top_Left_Row2    : UNSIGNED := %IMMED 0 ;
	Top_Left_Column2 : UNSIGNED := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$MOVE_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED ;
	Pasteboard_Id : UNSIGNED ;
	Pasteboard_Row : INTEGER ;
	Pasteboard_Column : INTEGER ;
	Top_Display_Id : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$NAME_TO_KEYCODE (
	Key_Name : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	VAR Key_Code : [VOLATILE] $UWORD )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PASTE_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED ;
	Pasteboard_Id : UNSIGNED ;
	Pasteboard_Row : INTEGER ;
	Pasteboard_Column : INTEGER ;
	Top_Display_Id : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$POP_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED ;
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PRINT_PASTEBOARD (
	Pasteboard_Id : UNSIGNED ;
	Queue_Name : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	Copies : INTEGER := %IMMED 0 ;
	Form_Name  : [CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_CHARS (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Start_Row : INTEGER := %IMMED 0 ;
	Start_Column : INTEGER := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_CHARS_HIGHWIDE (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Start_Row : INTEGER := %IMMED 0 ;
	Start_Column : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_CHARS_MULTI (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Line_Number : INTEGER := %IMMED 0 ;
	Column_Number : INTEGER := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_CHARS_WIDE (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Start_Row : INTEGER := %IMMED 0 ;
	Start_Column : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_HELP_TEXT (
	Display_Id : UNSIGNED ;
	Keyboard_Id : UNSIGNED := %IMMED 0 ;
	Help_Topic : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	Help_Library : [CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR :=%IMMED 0;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_LINE (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Line_Advance : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 ;
	Direction : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_LINE_HIGHWIDE (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Line_Adv : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_LINE_MULTI (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Line_Advance : INTEGER := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Direction : UNSIGNED := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_LINE_WIDE (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR ;
	Line_Advance : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_PASTEBOARD (
	Pasteboard_Id : UNSIGNED ;
	%IMMED [UNBOUND, ASYNCHRONOUS] PROCEDURE Action_Routine ;
	%REF User_Argument :
		[UNSAFE] PACKED ARRAY [A..B:INTEGER] OF $UBYTE := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_STATUS_LINE (
	Pasteboard_Id : UNSIGNED ;
	TEXT : PACKED ARRAY [A..B:INTEGER] OF CHAR )
		: UNSIGNED ; EXTERNAL ;

{####### [ASYNCHRONOUS] FUNCTION SMG$PUT_VIRTUAL_DISPLAY_ENCODED (
	Display_Id : UNSIGNED ;
	Encoded_Length : UNSIGNED ;
	Encoded_Text : [UNSAFE] PACKED ARRAY [A..B:INTEGER] OF $UBYTE ;
	Line_Number : INTEGER := %IMMED 0 ;
	Column_Number : INTEGER := %IMMED 0 ;
	Placeholder_Arg : UNSIGNED := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$PUT_WITH_SCROLL (
	Display_Id : UNSIGNED ;
	TEXT : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	Direction : UNSIGNED := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Character_Set : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ; ###########}

[ASYNCHRONOUS] FUNCTION SMG$READ_COMPOSED_LINE (
	Keyboard_Id : UNSIGNED ;
	Key_Table_Id : UNSIGNED := %IMMED 0 ;
	%DESCR Resultant_String : VARYING [A] OF CHAR ;
	Prompt_String : [CLASS_S] PACKED ARRAY [B..C:INTEGER] OF CHAR:=%IMMED 0;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 ;
	Display_Id : UNSIGNED := %IMMED 0 ;
	Flags : UNSIGNED := %IMMED 0 ;
	Initial_String :
		[CLASS_S] PACKED ARRAY [D..E:INTEGER] OF CHAR := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	VAR Word_Terminator_Code : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$READ_FROM_DISPLAY (
	Display_Id : UNSIGNED ;
	%DESCR Resultant_String : VARYING [A] OF CHAR ;
	Terminator_String :
		[CLASS_S] PACKED ARRAY [B..C:INTEGER] OF CHAR := %IMMED 0 ;
	Start_Row : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$READ_KEYSTROKE (
	Keyboard_Id : UNSIGNED ;
	VAR Word_Terminator_Code : [VOLATILE] $UWORD ;
	Prompt_String : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR:=%IMMED 0;
	Timeout : INTEGER := %IMMED 0 ;
	Display_Id : UNSIGNED := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$READ_STRING (
	Keyboard_Id : UNSIGNED ;
	%DESCR Resultant_String : VARYING [A] OF CHAR ;
	Prompt_String :
		[CLASS_S] PACKED ARRAY [B..C:INTEGER] OF CHAR := %IMMED 0 ;
	Maximum_Length : INTEGER := %IMMED 0 ;
	Modifiers : Mask_Longword := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 ;
	Terminator_Set :
		[CLASS_S] PACKED ARRAY [D..E:INTEGER] OF CHAR := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 ;
	VAR Word_Terminator_Code : [VOLATILE] $UWORD := %IMMED 0 ;
	Display_Id : UNSIGNED := %IMMED 0 ;
	Initial_String :
		[CLASS_S] PACKED ARRAY [F..G:INTEGER] OF CHAR := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 ;
	Terminator_String :
		[CLASS_S] PACKED ARRAY [H..J:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$READ_VERIFY (
	Keyboard_Id : UNSIGNED ;
	%DESCR Resultant_String : VARYING [A] OF CHAR ;
	Initial_String  : [CLASS_S] PACKED ARRAY [$L3..$U3:INTEGER] OF CHAR ;
	Picture_String  : [CLASS_S] PACKED ARRAY [C..D:INTEGER] OF CHAR ;
	Fill_Character  : [CLASS_S] PACKED ARRAY [E..F:INTEGER] OF CHAR ;
	Clear_Character : [CLASS_S] PACKED ARRAY [G..H:INTEGER] OF CHAR ;
	Prompt_String :
		     [CLASS_S] PACKED ARRAY [J..K:INTEGER] OF CHAR := %IMMED 0 ;
	Modifiers : Mask_Longword := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 ;
	Terminator_Set :
		     [CLASS_S] PACKED ARRAY [M..N:INTEGER] OF CHAR := %IMMED 0 ;
	Initial_Offset : INTEGER := %IMMED 0 ;
	VAR Word_Terminator_Code : [VOLATILE] $UWORD := %IMMED 0 ;
	Display_Id : UNSIGNED := %IMMED 0 ;
	Alternate_Echo_String :
		     [CLASS_S] PACKED ARRAY [P..Q:INTEGER] OF CHAR := %IMMED 0 ;
	Alternate_Display_Id : UNSIGNED := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$REMOVE_LINE (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER ;
	Start_Column : INTEGER ;
	End_Row : INTEGER ;
	End_Column : INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$REPAINT_LINE (
	Pasteboard_Id : UNSIGNED ;
	Start_Row : INTEGER ;
	Number_Of_Lines : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$REPAINT_SCREEN (
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$REPASTE_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED ;
	Pasteboard_Id : UNSIGNED ;
	Pasteboard_Row : INTEGER ;
	Pasteboard_Column : INTEGER ;
	Top_Display_Id : UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$REPLACE_INPUT_LINE (
	Keyboard_Id : UNSIGNED ;
	Replace_String :
		[CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	Line_Count : $UBYTE := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$RESTORE_PHYSICAL_SCREEN (
	Pasteboard_Id : UNSIGNED ;
	Display_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$RETURN_CURSOR_POS (
	Display_Id : UNSIGNED ;
	VAR Start_Row    : [VOLATILE] INTEGER ;
	VAR Start_Column : [VOLATILE] INTEGER )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$RETURN_INPUT_LINE (
	Keyboard_Id : UNSIGNED ;
	%DESCR Resultant_String : VARYING [A] OF CHAR ;
	Match_String :
		[CLASS_S] PACKED ARRAY [B..C:INTEGER] OF CHAR := %IMMED 0 ;
	Byte_Integer_Line_Number : $UBYTE := %IMMED 0 ;
	VAR Resultant_Length : [VOLATILE] $UWORD := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$RING_BELL (
	Display_Id : UNSIGNED ;
	Number_Of_Times : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SAVE_PHYSICAL_SCREEN (
	Pasteboard_Id : UNSIGNED ;
	VAR Display_Id : [VOLATILE] UNSIGNED ;
	Desired_Start_Row : INTEGER := %IMMED 0 ;
	Desired_End_Row   : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SAVE_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED ;
	Filespec : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SCROLL_DISPLAY_AREA (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER := %IMMED 0 ;
	Start_Column : INTEGER := %IMMED 0 ;
	Height : INTEGER := %IMMED 0 ;
	Width : INTEGER := %IMMED 0 ;
	Direction : Mask_Longword := %IMMED 0 ;
	Count : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SCROLL_VIEWPORT (
	Display_Id : UNSIGNED ;
	Direction : Mask_Longword := %IMMED 0 ;
	Count : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SELECT_FROM_MENU (
	Keyboard_Id : UNSIGNED ;
	Display_Id : UNSIGNED ;
	VAR Selected_Choice_Number : [VOLATILE] $UWORD ;
	Default_Choice_Number : $UWORD := %IMMED 0 ;
	Flags : Mask_Longword := %IMMED 0 ;
	Help_Library :
		[CLASS_S] PACKED ARRAY [$L6..$U6:INTEGER] OF CHAR := %IMMED 0 ;
	Timeout : INTEGER := %IMMED 0 ;
	VAR Word_Terminator_Code : [VOLATILE] $UWORD := %IMMED 0 ;
	%DESCR Selected_Choice_String : VARYING [$U9] OF CHAR := %IMMED 0 ;
	Rendition_Set : Mask_Longword := %IMMED 0 ;
	Rendition_Complement : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_BROADCAST_TRAPPING (
	Pasteboard_Id : UNSIGNED ;
	%IMMED [UNBOUND, ASYNCHRONOUS] PROCEDURE Ast_Routine := %IMMED 0 ;
	%IMMED Ast_Argument : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_CURSOR_ABS (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER := %IMMED 0 ;
	Start_Column : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_CURSOR_MODE (
	Pasteboard_Id : UNSIGNED ;
	Flags : Mask_Longword )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_CURSOR_REL (
	Display_Id : UNSIGNED ;
	Delta_Row : INTEGER := %IMMED 0 ;
	Delta_Column : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_DEFAULT_STATE (
	Key_Table_Id : UNSIGNED ;
	New_State : [CLASS_S] PACKED ARRAY [A..B:INTEGER] OF CHAR := %IMMED 0 ;
	%DESCR Old_State : VARYING [C] OF CHAR := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_DISPLAY_SCROLL_REGION (
	Display_Id : UNSIGNED ;
	Start_Row : INTEGER := %IMMED 0 ;
	End_Row   : INTEGER := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_KEYPAD_MODE (
	Keyboard_Id : UNSIGNED ;
	Flags : Mask_Longword )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_OUT_OF_BAND_ASTS (
	Pasteboard_Id : UNSIGNED ;
	Control_Character_Mask : Mask_Longword ;
	%IMMED [UNBOUND,ASYNCHRONOUS] PROCEDURE Ast_Routine ;
	%IMMED Ast_Argument : [UNSAFE] UNSIGNED := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_PHYSICAL_CURSOR (
	Pasteboard_Id : UNSIGNED ;
	Pasteboard_Row : UNSIGNED ;
	Pasteboard_Column : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SET_TERM_CHARACTERISTICS (
	Pasteboard_Id : UNSIGNED ;
	On_Characteristics1  : Mask_Longword := %IMMED 0 ;
	On_Characteristics2  : Mask_Longword := %IMMED 0 ;
	Off_Characteristics1 : Mask_Longword := %IMMED 0 ;
	Off_Characteristics2 : Mask_Longword := %IMMED 0 ;
	VAR Old_Characteristics1 : [VOLATILE] Mask_Longword := %IMMED 0 ;
	VAR Old_Characteristics2 : [VOLATILE] Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$SNAPSHOT (
	Pasteboard_Id : UNSIGNED ;
	Flags : Mask_Longword := %IMMED 0 )
		: UNSIGNED ; EXTERNAL ;

[ASYNCHRONOUS] FUNCTION SMG$UNPASTE_VIRTUAL_DISPLAY (
	Display_Id : UNSIGNED ;
	Pasteboard_Id : UNSIGNED )
		: UNSIGNED ; EXTERNAL ;

