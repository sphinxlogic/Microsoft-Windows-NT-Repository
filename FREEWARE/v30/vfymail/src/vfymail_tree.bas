%TITLE "BUILD_INTERNAL_NAMELIST"
%SBTTL "Scan mail file for external references"
%IDENT "V05.01"

FUNCTION LONG build_internal_namelist (node_type node, LONG user_data)

![function-header]
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    %PAGE
    %SBTTL "Declarations"

    !+
    ! Local Constants
    !-

    %INCLUDE "$maildef" %FROM %LIBRARY
    %INCLUDE "$mailmsgdef"

    !+
    ! External Constants
    !-

    EXTERNAL LONG CONSTANT ss$_normal

    !+
    ! External Routines
    !-
     
    %INCLUDE "mail$routines"
    EXTERNAL LONG FUNCTION chksts, &
			   mailfile_folder_routine

    !+
    ! MAP Declarations
    !-
     
    MAP (build_local) node_text_type	node_rec, &
		      STRING	mailfile_spec = 255, &
		      LONG	stat, &
				new_node_loc, &
				mailfile_indexed_flag
     
    !+
    ! Local Variables
    !-
     
    DECLARE STRING  mail_directory, &
		    mailfile_name, &
		    file_name
     
    %INCLUDE "vfycommon"

    %PAGE
    %SBTTL "Action routine logic"

    WHEN ERROR IN
	mail_file_name = SEG$ (node::text, 1, node::text_len)
	mailfile_spec = EDIT$ (mail_directory_spec+mail_file_name+".MAI", 2)
	! Extract mailfile name from node parameter,
	! Build full mailfile filespec,
	! Place copy of mail filename in common for MAILFILE_FOLDER_ROUTINE.

	input_list(1)::itmcod = mail$_mailfile_name
	input_list(1)::buflen = LEN (mailfile_spec)
	input_list(1)::bufadr = LOC (mailfile_spec)
	input_list(1)::lenadr = 0
	input_list(2)::itmcod = mail$_nosignal
	input_list(2)::buflen = 0
	input_list(2)::bufadr = 0
	input_list(2)::lenadr = 0
	input_list(3)::terminator = 0

	output_list(1)::itmcod = mail$_mailfile_indexed
	output_list(1)::buflen = 4
	output_list(1)::bufadr = LOC (mailfile_indexed_flag)
	output_list(1)::lenadr = 0
	output_list(2)::terminator = 0

	stat = mail$mailfile_open ( &
		    mailfile_context, &
		    input_list() BY REF, &
		    output_list() BY REF)
	! Open specified mail file.
	SELECT stat

	    CASE = mail$_openin
		PRINT #5, "%VFYMAIL-E-ERROPEMAI, error opening mail file "; &
			mailfile_spec
		option_flags = option_flags and not vfy_m_repair
		EXIT FUNCTION stat
		! If we don't have access to a mailfile, then ensure that
		!  the REPAIR option is disabled and exit the routine.
	    
	    CASE ELSE
		stat = chksts (stat)

	END SELECT

	IF mailfile_indexed_flag = 0
	THEN
	    mail_file_count = mail_file_count - 1
	    PRINT #5, "%VFYMAIL-I-BYPSEQMAI, bypassing sequential mailfile "; &
		    TRM$ (mail_file_name); ".MAI"
	ELSE	  
	    PRINT #5, "%VFYMAIL-I-PROMAIFIL, processing mailfile "; &
		    TRM$ (mail_file_name); ".MAI"

	    input_list(1)::itmcod = mail$_mailfile_folder_routine
	    input_list(1)::buflen = 4
	    input_list(1)::bufadr = LOC (mailfile_folder_routine)
	    input_list(1)::lenadr = 0
	    input_list(2)::terminator = 0

	    stat = mail$mailfile_info_file ( &
			mailfile_context, &
			input_list() BY REF, &
			null_list BY REF)
	END IF
	! If this is a non-ISAM mail file, display message and bypass
	!  processing for it.
	! Otherwise, use folder action routine to scan thru messages
	!  in each folder.

	stat = mail$mailfile_close ( &
		    mailfile_context, &
		    null_list BY REF, &
		    null_list BY REF)
	! Close file when done.

	EXIT FUNCTION  ss$_normal

    USE
	EXIT HANDLER
    END WHEN

END FUNCTION  ss$_normal  ! End of FUNCTION build_internal_namelist

%TITLE "MATCH_FILENAME"
%SBTTL "Search binary tree for matching name"
%IDENT "V05.01"

FUNCTION LONG match_filename (node_type node, LONG user_data)

![function-header]
    
    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR

    %PAGE
    %SBTTL "Declarations"

    !+
    ! Local Constants
    !-

    %INCLUDE "$maildef" %FROM %LIBRARY

    !+
    ! External Constants
    !-
     
    EXTERNAL LONG CONSTANT lib$_normal, lib$_keynotfou, ss$_normal

    !+
    ! External Routines
    !-
     
    %INCLUDE "mail$routines"
    EXTERNAL LONG FUNCTION chksts, &
			   compare_node1, &
			   set_delprot

    EXTERNAL LONG FUNCTION lib$insert_tree ( &
			       LONG BY REF, &
			       ANY, &
			       LONG BY REF, &
			       ANY BY VALUE, &
			       ANY BY VALUE, &
			       LONG BY REF, &
			       ANY), &
			   lib$lookup_tree ( &
			       LONG BY REF, &
			       ANY, &
			       ANY BY VALUE, &
			       LONG BY REF), &
			   lib$traverse_tree ( &
			       LONG BY REF, &
			       ANY BY VALUE, &
			       ANY), &
			   lib$rename_file ( &
			       STRING BY DESC, &
			       STRING BY DESC, &
			       STRING BY DESC, &
			       STRING BY DESC, &
			       LONG BY REF, &
			       ANY BY VALUE, &
			       ANY BY VALUE, &
			       ANY BY VALUE, &
			       ANY, &
			       STRING BY DESC, &
			       STRING BY DESC, &
			       LONG BY REF)
     
    !+
    ! MAP Declarations
    !-

    MAP (match_local) node_text_type	node_rec

    !+
    ! Local Variables
    !-
     
    DECLARE LONG	newnode, &
			stat, &
			stat1, &
	    STRING	node_name, &
			mail_directory, &
			mailfile_name, &
			mailfile_spec, &
			key_value

    %INCLUDE "vfycommon"

    %PAGE
    %SBTTL "Action routine mail logic"

    WHEN ERROR IN
	mail_directory = TRM$ (mail_directory_spec)
	LSET node_rec::textrec = SEG$ (node::text, 1, node::text_len)
	node_name = TRM$ (node_rec::filename)

	stat = lib$lookup_tree (internal_name_tree, &
				    node_name, &
				    LOC (compare_node1), &
				    newnode)
	SELECT stat
	     
	    CASE = lib$_normal

	    CASE = lib$_keynotfou
		PRINT #5, "%VFYMAIL-I-NOMESFOU, "; &
		    "no message found for file "; node_name; ".MAI"
		internal_error_count = internal_error_count + 1
		 
		IF (option_flags AND vfy_m_repair) <> 0
		THEN
		    
		    WHEN ERROR IN
			stat1 = chksts (set_delprot ( &
					    mail_directory+node_name+".MAI"))
			stat1 = chksts (lib$rename_file ( &
					    mail_directory+node_name+".MAI", &
					    ".LOST",,,,,,,,,,))
		    USE
			PRINT #5, "%VFYMAIL-E-FILNOTREN, file not renamed "; &
				mail_directory; node_name; ".MAI"
		    END WHEN
		    ! If lack of a VMS error condition gets to be a problem,
		    !  then modify to display VMS error message.

		END IF

	    CASE ELSE
		PRINT "Bad status from Lookup_tree"	     
		stat = chksts (stat)

	END SELECT
	 
	EXIT FUNCTION 1
    USE
	EXIT HANDLER
    END WHEN

END FUNCTION  ss$_normal ! End of FUNCTION match_filename
