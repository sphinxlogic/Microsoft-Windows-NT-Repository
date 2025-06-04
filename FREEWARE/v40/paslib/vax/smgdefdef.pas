 
(* Definitions for RTL Screen Management Facility                           *)
(*                                                                          *)
 
(*                                                                          *)
(* Define the out-of-band information table used when an out-of-band        *)
(* AST routine is called as it's own record.                                *)
(*                                                                          *)
 
TYPE	OUT_OF_BAND_TABLE = RECORD CASE INTEGER OF
	1: (SMG$L_PBD_ID : UNSIGNED;    (* Pasteboard ID                    *)
	    SMG$L_USER_ARG : UNSIGNED;  (* User's AST argument              *)
	    SMG$R_CHAR_OVERLAY : [BYTE(4)] RECORD END;
	    );
	2: (SMG$B_CHAR : [POS(64)] $UBYTE; (* The character typed           *)
	    );
	3: (SMG$L_CHAR : [POS(64)] UNSIGNED; (* The character and 3 spaces  *)
	    )
	END;
 
(*                                                                          *)
(* Define the subprocess information table used when a subprocess           *)
(* AST routine is called as it's own record.                                *)
(*                                                                          *)
 
TYPE	SUBPROCESS_INFO_TABLE = RECORD
	    SMG$L_DISP_ID : UNSIGNED;   (* Display ID                       *)
	    SMG$L_USR_ARG : UNSIGNED;   (* User's AST argument              *)
	    SMG$L_STATUS : UNSIGNED;    (* Status of executed command       *)
	END;
 
(*                                                                          *)
(* Define an information table that can be used by both                     *)
(* SMG$GET_PASTEBOARD_ATTRIBUTES and SMG$GET_KEYBOARD_ATTRIBUTES            *)
(* as it's own record.                                                      *)
(*                                                                          *)
 
TYPE	ATTRIBUTE_INFO_BLOCK = RECORD CASE INTEGER OF
	1: (SMG$L_DEV_CHAR : UNSIGNED;  (* Device characteristics           *)
	    SMG$L_DEV_DEPEND : UNSIGNED; (* Specific characteristics (1)    *)
	    SMG$L_DEV_DEPEND2 : UNSIGNED; (* Specific characteristics (2)   *)
	    SMG$B_DEV_CLASS : $UBYTE;   (* Device class (e.g. DC$_TERM)     *)
	    SMG$R_DEV_OVERLAY : [BYTE(1)] RECORD END;
	    SMG$B_DEV_TYPE : $UBYTE;    (* Physical device type (e.g. DT$_VT100) *)
	    SMG$R_ROW_OVERLAY : [BYTE(1)] RECORD END;
	    SMG$W_NUM_COLUMNS : $UWORD; (* Terminal width                   *)
	    SMG$R_COUNT_OVERLAY : [BYTE(2)] RECORD END;
	    SMG$W_DEV_SPEED : $UWORD;   (* Terminal Speed (+)               *)
	    SMG$W_DEV_FILL : $UWORD;    (* Fill characteristics (+)         *)
	    SMG$W_PHYS_CURSOR_ROW : $UWORD; (* Row where physical cursor is *)
	    SMG$W_PHYS_CURSOR_COL : $UWORD; (* Col where physical cursor is *)
	    SMG$L_DISPLAY_ID : UNSIGNED; (* Display containing phy cursor   *)
	    );
	2: (SMG$B_PBD_TYPE : [POS(104)] $UBYTE; (* Internal SMG device type *)
	    SMG$B_NUM_ROWS : [POS(120)] $UBYTE; (* Number of rows on device *)
	    SMG$W_TYPEAHD_CNT : [POS(144)] $UWORD; (* Number of chars in typeahead buffer (+) *)
	    );
	3: (SMG$B_RECALL_NUM : [POS(104)] $UBYTE; (* Size of recall buffer (+) *)
	    SMG$B_TYPEAHD_CHAR : [POS(120)] $UBYTE; (* First char in typeahead buffer (+) *)
	    SMG$R_COUNT_STRUCT : [POS(144), BYTE(2)] RECORD END;
	    );
	4: (SMG$B_PBD_COLOR : [POS(144)] $UBYTE; (* PBD background color    *)
	    SMG$B_DEV_PARITY : [POS(152)] $UBYTE; (* Parity attributes (+)  *)
	    )
	END;
 
