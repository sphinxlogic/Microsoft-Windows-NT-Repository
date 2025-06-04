    %PAGE
    %SBTTL "LSTMAIL Common Definitions"
     
    !+
    ! External Constants
    !-

    EXTERNAL LONG CONSTANT bas$k_canfinfil, &
			   bas$k_canopefil, &
			   bas$k_proc__tra, &
			   bas$k_filattnot, &
			   bas$k_endfildev, &
			   bas$k_illallcla, &
			   bas$k_recnotfou

    EXTERNAL LONG CONSTANT cli$_present, &
			   cli$_defaulted, &
			   cli$_absent, &
			   cli$_negated, &
			   ss$_normal, &
			   str$_match, &
			   str$_nomatch

    !+
    ! Local Constants
    !-

    DECLARE LONG CONSTANT opt_m_directory     =   1, &
			  opt_m_select_flags  =   2, &
			  opt_m_select_folder =   4, &
			  opt_m_select_before =   8, &
			  opt_m_select_since  =  16, &
			  opt_m_select_from   =  32, &
			  opt_m_select_subj   =  64, &
			  opt_m_select_to     = 128

    DECLARE LONG CONSTANT lstm_c_list_messages = 1, &
			  lstm_c_list_folders  = 2

    DECLARE LONG CONSTANT mmh_m_newmsg     = X"0001"L, &
			  mmh_m_replied    = X"0002"L, &
			  mmh_m_skip1      = X"0004"L, &
			  mmh_m_extmsg     = X"0008"L, &
			  mmh_m_extfnf     = X"0010"L, &
			  mmh_m_sysmsg     = X"0020"L, &
			  mmh_m_extnstd    = X"0040"L, &
			  mmh_m_marked     = X"0080"L, &
			  mms_m_skip2      = X"0100"L

    !+
    ! RECORD Declarations
    !-
     
    RECORD item_list_2
	GROUP item (1 TO 15)
	    VARIANT
		CASE
		    WORD	buflen
		    WORD	itmcod
		    LONG	bufadr
		CASE
		    LONG	terminator
	    END VARIANT
	END GROUP item
    END RECORD item_list_2
      

    RECORD item_list_3
	GROUP item (1 TO 15)
	    VARIANT
		CASE
		    WORD	buflen
		    WORD	itmcod
		    LONG	bufadr
		    LONG	lenadr
		CASE
		    LONG	terminator
	    END VARIANT
	END GROUP item
    END RECORD item_list_3
      

    !+
    ! COMMON Declarations
    !-
     
    COMMON LONG	    mailfile_context, &
		    message_context, &
		    option_flag, &
		    select_flag, &
	   WORD	    folder_pattern_len, &
		    select_before_len, &
		    select_from_len, &
		    select_since_len, &
		    select_subj_len, &
		    select_to_len, &
	   STRING   folder_pattern = 20, &
		    mail_directory_spec = 128, &
		    mail_file_name = 32, &
		    select_before = 23, &
		    select_from = 128, &
		    select_since = 23, &
		    select_subj = 128, &
		    select_to = 128, &
	   item_list_2	    fscn_list, &
	   item_list_3	    input_list, &
			    null_list, &
			    output_list
