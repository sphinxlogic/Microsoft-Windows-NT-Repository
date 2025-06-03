%TITLE "VFYMAIL"
%SBTTL "Check for missing external files and messages"
%IDENT "V05.01"
      
PROGRAM vfymail

!+
! FACILITY:
!
!    Utility to check for extra or missing external (MAIL$...)
!	 mail files.
!
! FUNCTIONAL DESCRIPTION:
!
!    Checks for missing or extra external mail files.  Command
!	 syntax is:
!
!	    VFYMAIL [/qualifiers] [mail-directory] 
!
!	 Parameters:
!
!	    mail-directory
!		Mail directory to process.  If omitted, defaults to users'
!		default mail directory.
!
!	 Command qualifiers:
!
!	    /[NO]OUTPUT
!		Specifies file to direct output to.  If omitted, defaults
!		to SYS$OUTPUT.  If partial filespec given, defaults to
!		filename of VFYMAIL and filetype of LIS.
!
!	    /[NO]REPAIR
!		Specifies that messages with missing external files are to
!		be placed in the mail folder MISSING_MAIL_FILES.  External
!		files not associated with a message in any mail file are
!		renamed with a filetype of LOST.
!
! IMPLICIT INPUTS:
!
!    None
!
! IMPLICIT OUTPUTS:
!
!    None
!
! SIDE EFFECTS:
!
!    None
!
! AUTHOR:  David L. Kachelmyer
!
! CREATION DATE: 06-Jun-1985
!
! MODIFICATION HISTORY:
!
!   V5.01		David L. Kachelmyer	December 31, 1988
!	Fix SS$_ACCVIO problem in ALLOCATE_NODE - Don't use the
!	LIB$ROUTINES module; define the routines in the program for
!	stability.
!	Move modules from BASIC_STARLET text library to separate external
!	files.
!
!   V05.00		David L. Kachelmyer	13-Dec-1987
!	Modify to work under VMS Version 5.
!	Use MAIL$ routines to access mail file and profile information.
!	Since the MAIL$ routines signal errors by default, no need to use
!	the CHKSTS function on them.
!	Modify to use new BASIC V3 features.
!	Merge VFYMAIL.BAS and VFYROUTINES.BAS into one source file.
!
!   V02.07		David L. Kachelmyer	13-Jun-1986
!	Fix incorrect reference to output_spec that destroys first
!	external mail file in mail directory if mail_directory parameter
!	explicitly specified.
!
!   V02.06		David L. Kachelmyer	23-May-1986
!	Check for presense of anything other than a device and directory
!	in the MAIL_DIRECTORY parameter.
!
!   V02.04		David L. Kachelmyer	22-May-1986
!	Terminate processing cleanly on failure to open a mail file to
!	check for external references.  Exit with severity = 4.
!
!   V02.03		David L. Kachelmyer	22-May-1986
!	Add 'Scanning mail directory' message to identify directory being
!	processed.
!	Display mail file names without directory spec, since it's just
!	been displayed.
!
!   V02.02		David L. Kachelmyer	21-May-1986
!	Deallocate lib$find_file contexts after use.
!	Parse MAIL_DIRECTORY to convert to a device and directory, in case
!	this needs to be printed out at some point.
!	Incidentally, check to see if specified directory contains any
!	mail files.
!
!   V02.01		David L. Kachelmyer	20-May-1986
!	Fix bug that used DEFAULT_MAIL_DIRECTORY instead of specified mail
!	directory for accessing mail file.
!
!   V02.00		David L. Kachelmyer	19-May-1986
!	Move messages with lost external files to mail folder
!	LOST_MAIL_FILES.
!
!   V01.05		David L. Kachelmyer	19-May-1986
!	Make CHKSTS an external function to avoid duplicate code in
!	modules.  Rework command parsing code.
!
!   V01.04		David L. Kachelmyer	19-May-1986
!	Split out common definitions into VFMCOMMON.
!
!   V01.03		David L. Kachelmyer	 9-May-1986
!	Fix rename bug with /REPAIR.
!
!   V01.02		David L. Kachelmyer	 6-May-1986
!	Drop image SYSPRV if process doesn't own it.  This is to prevent
!	abuse if image is installed with SYSPRV.
!
!   V01.01		David L. Kachelmyer	26-Jul-1985
!	Change passed routine names from external constants to external
!	variables.
!
!   V01.00		David L. Kachelmyer	 6-Jun-1985
!	Original program.
!--

    OPTION TYPE = EXPLICIT, &
	   CONSTANT TYPE = INTEGER, &
	   HANDLE = ERROR
 
    %PAGE
    %SBTTL "Declarations"
     
    !+
    ! External Constants
    !-
     
    EXTERNAL LONG CONSTANT cli$_present, &
			   cli$_negated, &
			   cli$_locpres, &
			   cli$_locneg, &
			   cli$_defaulted, &
			   cli$_absent, &
			   cli$_comma, &
			   cli$_concat, &
			   rms$_fnf, &
			   rms$_nmf, &
			   rms$_normal, &
			   ss$_normal, &
			   str$_match, &
			   str$_nomatch
    
    !+
    ! Local Constants
    !-

    %INCLUDE "$fscndef" %FROM %LIBRARY
    %INCLUDE "$maildef" %FROM %LIBRARY
    %INCLUDE "$mailmsgdef"

    !+
    ! Local Constants
    !-
     
    DECLARE STRING CONSTANT &
			mail_dollar_name = "MAIL$%%%%%%%%%%%%%%%%"
     
    !+
    ! External Routines
    !-
     
    !%INCLUDE "lib$routines" %FROM %LIBRARY
    %INCLUDE "mail$routines"
    %INCLUDE "str$routines" %FROM %LIBRARY

    EXTERNAL LONG FUNCTION cli$get_value ( &
			       STRING BY DESC, &
			       STRING BY DESC), &
			   cli$present ( &
			       STRING BY DESC)
    EXTERNAL LONG FUNCTION allocate_node, &
			   build_internal_namelist, &
			   chksts, &
			   compare_node, &
			   match_filename
     
    EXTERNAL LONG FUNCTION lib$find_file ( &
			       STRING BY DESC, &
			       STRING BY DESC, &
			       LONG BY REF, &
			       STRING BY DESC, &
			       STRING BY DESC, &
			       LONG BY REF, &
			       LONG BY REF), &
			   lib$find_file_end ( &
			       LONG BY REF), &
			   lib$insert_tree ( &
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
			       ANY)
    EXTERNAL LONG FUNCTION sys$filescan ( &
			       STRING BY DESC, &
			       ANY BY REF, &
			       LONG BY REF)

    !+
    ! MAP Declarations
    !-
     
    MAP (main_local) STRING	default_mail_directory = 255, &
		     WORD	default_mail_directory_len, &
		     LONG	find_file_context, &
				filescan_flags, &
				fscn_flag_value, &
				new_node_loc, &
				stat, &
				x1, &
				x2

    !+
    ! Local Variables
    !-
     
    DECLARE STRING  file_name, &
		    file_spec, &
		    mail_directory, &
		    newmail_flag, &
		    output_line, &
		    output_spec

    %INCLUDE "vfycommon"

    %PAGE
    %SBTTL "Program Initialization"
    
    WHEN ERROR IN
	stat = CTRLC
	! Enable ^C trapping.

	fscn_flag_value = NOT (fscn$m_device OR fscn$m_root OR fscn$m_directory)
	! Generate mask value for checking for presence of flags other than
	! these.

	external_file_tree, internal_name_tree = 0  
	internal_count, external_count, mail_file_count = 0
	external_error_count, internal_error_count = 0
	! Zero counters	and tree headers.

	null_list::terminator = 0
	! Initialize null item list.

	lost_folder_name = lost_folder_id
	! Give the folder-id something we can LOC() to.

	output_list(1)::itmcod = mail$_mailfile_mail_directory
	output_list(1)::buflen = LEN (default_mail_directory)
	output_list(1)::bufadr = LOC (default_mail_directory)
	output_list(1)::lenadr = LOC (default_mail_directory_len)
	output_list(2)::terminator = 0

	stat = mail$mailfile_begin ( &
		    mailfile_context, &
		    null_list BY REF, &
		    output_list() BY REF)
	! Initialize mail processing.
	! Get default mail directory specification.

	%PAGE
	%SBTTL "Parse command line"
Parse_command:

	option_flags = 0

	stat = cli$present("OUTPUT")
	SELECT stat

	    CASE = cli$_present
		stat = cli$get_value ("OUTPUT", output_spec)
		SELECT stat

		    CASE = ss$_normal
			! No action necessary.

		    CASE = cli$_absent
			output_spec = "SYS$OUTPUT"
		    
		    CASE ELSE
			stat = chksts (stat)

		END SELECT

	    CASE = cli$_negated
		output_spec = "NLA0:"

	    CASE = cli$_absent
		output_spec = "SYS$OUTPUT"

	    CASE ELSE
		stat = chksts (stat)

	END SELECT
	! Get /OUTPUT qualifier and value.
	  

	stat = cli$present("REPAIR")
	SELECT stat

	    CASE = cli$_present
		option_flags = option_flags OR vfy_m_repair

	    CASE = cli$_absent, = cli$_negated
		! No action necessary.
	    
	    CASE ELSE
		stat = chksts (stat)

	END SELECT
	! Get /REPAIR qualifier.


	stat = cli$present("STATISTICS")
	SELECT stat

	    CASE = cli$_present
		option_flags = option_flags OR vfy_m_statistics

	    CASE = cli$_absent, = cli$_negated
		! No action necessary.
	    
	    CASE ELSE
		stat = chksts (stat)

	END SELECT
	! Get /STATISTICS qualifier.


	stat = cli$get_value("MAIL_DIRECTORY",mail_directory)
	SELECT stat

	    CASE = ss$_normal
		fscn_list(1)::terminator = 0
		stat = chksts (sys$filescan ( &
				   mail_directory, &
				   fscn_list() BY REF, &
				   filescan_flags))
		
		IF (filescan_flags AND fscn_flag_value) <> 0
		THEN
		    PRINT "Invalid mail directory specification - "; &
				"specify device and directory only"
		    GO TO exit_program
		END IF
		! Ensure that only a device and directory were specified.

		stat = lib$find_file ( &
			   "*.MAI", &
			   file_spec, &
			   find_file_context, &
			   mail_directory,,,)
		SELECT stat

		    CASE = rms$_normal
			x1 = POS (file_spec, "]", 1)
			x1 = POS (file_spec, ">", 1)  IF x1 = 0
			mail_directory = SEG$ (file_spec,1,x1)

		    CASE = rms$_fnf
			PRINT "Directory contains no mail files"
			GO TO exit_program
		    
		    CASE ELSE
			stat = chksts (stat)

		END SELECT
		stat = chksts (lib$find_file_end (find_file_context))
		! Use lib$find_file to parse supplied directory spec and
		!  extract device and directory specification from filespec.
		! Detect whether or not there are any mail files in this
		!  directory.  If none, then issue error and exit.
		! Deallocate lib$find_file context after we're done with it.

	    CASE = cli$_absent
		mail_directory = LEFT$ (default_mail_directory, &
					default_mail_directory_len)
	    
	    CASE ELSE
		stat = chksts (stat)

	END SELECT
	mail_directory_spec = mail_directory
	! Get mail directory name.
	! Place copy in common for other routines.


	OPEN output_spec FOR OUTPUT AS FILE #5, &
		       DEFAULTNAME "VFYMAIL.LIS"
	! Open output file.


	%PAGE
	%SBTTL "Build external file tree"
Build_external_tree:

	find_file_context = 0

	PRINT #5, "%VFYMAIL-I-PROMAIDIR, processing mail directory "; &
		    mail_directory

External_file_loop:

	WHILE 1
	 
	    stat = lib$find_file ( &
		       "*.MAI", &
		       file_spec, &
		       find_file_context, &
		       mail_directory,,,) 
	    SELECT stat
		CASE = rms$_normal
		    x1 = POS (file_spec, "]", 1)
		    x1 = POS (file_spec, ">", 1)  IF x1 = 0
		    x2 = POS (file_spec, ".", x1)
		    file_name = SEG$ (file_spec,x1+1,x2-1)
		    ! Note that this won't work if we're using angle-brackets.

		CASE = rms$_nmf, = rms$_fnf
		    stat = chksts (lib$find_file_end (find_file_context))
		    EXIT External_file_loop
		
		CASE ELSE
		    stat = chksts (stat)

	    END SELECT
	    ! Perform lookup on *.MAI files and extract filename.

	    stat = str$match_wild (file_name, mail_dollar_name)
	    SELECT stat

		CASE = str$_match
		    stat = lib$insert_tree ( &
			       external_file_tree, &
			       file_name, &
			       1, &
			       LOC (compare_node), &
			       LOC (allocate_node), &
			       new_node_loc,)
		    external_count = external_count + 1

		CASE = str$_nomatch
		    stat = lib$insert_tree ( &
			       mail_file_tree, &
			       file_name, &
			       1, &
			       LOC (compare_node), &
			       LOC (allocate_node), &
			       new_node_loc,)
		    mail_file_count = mail_file_count + 1
		
		CASE ELSE
		    stat = chksts (stat)

	    END SELECT
	    ! If this is a MAIL$ssn.MAI file, place name in list of
	    !   external ID file names.
	    ! If this is not a MAIL$ssn.MAI file, place in list of
	    !   mail file names.
	    
	NEXT 


	%PAGE
	%SBTTL "Build internal name tree"

	stat = chksts (lib$traverse_tree ( &
			   mail_file_tree, &
			   LOC (build_internal_namelist),))
	! Process list of mail files.
	! For each mail file, open it and look for external file references.
	! Note that list will contain references from ALL mail files in
	!  the mail directory.


	%PAGE
	%SBTTL "Process external file-name tree"

	stat = chksts (lib$traverse_tree ( &
			   external_file_tree, &
			   LOC (match_filename), &
			   2))
	! Check list of external files against list of external file
	!  references.  Action routine will display name of superfluous
	!  external files.
	! If /REPAIR is specified, unreferenced MAIL$ssn.MAI files are
	!  renamed with a file type of '.LOST'.

    USE
	SELECT ERR

	    CASE = bas$k_endfildev, = bas$k_proc__tra
		CONTINUE exit_program
	     
	    CASE = mail$_openin
		CONTINUE exit_program

	    CASE ELSE
		EXIT HANDLER
	 
	END SELECT
    END WHEN

    
    IF option_flags AND vfy_m_statistics
    THEN
	PRINT #5
	PRINT #5, "Total mail files:               "; mail_file_count
	PRINT #5, "Total external files:           "; external_count
	PRINT #5, "Total external file references: "; internal_count
	PRINT #5, "Total missing files:            "; external_error_count
	PRINT #5, "Total missing messages:         "; internal_error_count
    END IF


    %PAGE
    %SBTTL "Exit program"
Exit_program:
    
    WHEN ERROR IN
	CLOSE #5
	
	IF message_context <> 0
	THEN
	    stat = mail$message_end ( &
			message_context, &
			null_list, &
			null_list)
	END IF
	  
	stat = mail$mailfile_end ( &
		    mailfile_context, &
		    null_list, &
		    null_list)

	EXIT PROGRAM ss$_normal

    USE
	SELECT ERR

	    CASE = bas$k_proc__tra
		CONTINUE
	     
	    CASE ELSE
		EXIT HANDLER
	 
	END SELECT
    END WHEN

END PROGRAM
