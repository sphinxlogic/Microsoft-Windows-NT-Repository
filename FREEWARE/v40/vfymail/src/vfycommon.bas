    %PAGE
    %SBTTL "VFYMAIL Common Definitions"
     
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

    !+
    ! Local Constants
    !-

    DECLARE LONG CONSTANT vfy_m_repair     = 1, &
			  vfy_m_statistics = 2

    DECLARE STRING CONSTANT lost_folder_id = "LOST_MESSAGES"

    !+
    ! RECORD Declarations
    !-
     
    RECORD item_list_2
	VARIANT
	    CASE
		WORD	buflen
		WORD	itmcod
		LONG	bufadr
	    CASE
		LONG	terminator
	END VARIANT
    END RECORD item_list_2
      

    RECORD item_list_3
	VARIANT
	    CASE
		WORD	buflen
		WORD	itmcod
		LONG	bufadr
		LONG	lenadr
	    CASE
		LONG	terminator
	END VARIANT
    END RECORD item_list_3
      

    RECORD node_text_type
	VARIANT
	    CASE
		STRING	textrec = 300	    ! entire record
	    CASE
		STRING	filename = 32	    ! mailfile or external file name
	END VARIANT
    END RECORD node_text_type
      

    RECORD node_type
	BYTE	FILL(10)	    ! node header
	WORD	text_len	    ! text length
	VARIANT
	    CASE
		STRING text = 300
	    CASE
		node_text_type	text1
	END VARIANT
    END RECORD node_type
      

    !+
    ! COMMON Declarations
    !-
     
    COMMON (global) LONG	folder_count, &
				external_count, &
				external_error_count, &
				external_file_tree, &
				folder_tree, &
				folder_id, &
				internal_count, &
				internal_error_count, &
				internal_name_tree, &
				mail_file_count, &
				mail_file_tree, &
				mailfile_context, &
				message_context, &
				message_sender_len, &
				message_subject_len, &
				option_flags, &
		    STRING	lost_folder_name = 255, &
				mail_directory_spec = 128, &
				mail_file_name = 32, &
				message_date = 11, &
				message_sender = 255, &
				message_subject = 255, &
		    item_list_2	fscn_list(1 TO 15), &
		    item_list_3 input_list(1 TO 15), &
				null_list, &
				output_list(1 TO 15)
