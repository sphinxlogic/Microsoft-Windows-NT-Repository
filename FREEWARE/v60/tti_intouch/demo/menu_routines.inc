        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Program       : Menu_routines.inc
        ! Package       : Communication Expense Report Tracking System
        ! Author        : Allen Roske
        ! Date          : February 1989
        ! W.R. No.      : 
        ! Purpose       : login, security, and menu routines
        ! Modified      : Lisa jordan Quast to work for CERTS
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Modification History
        !----------------------------------------------------------------
        ! 28-JUN-94     MS      Modified to be versatile - removed
        !                       hard coded CERTS stuff.
        ! 17-AUG-94     MS      1. Modified to allow menu items 24 chars
        !                       long (previously 23).
        !                       2. Modified to center the pop-up menu.
        !                       3. Added new routine call to add EXIT 
        !                          as last menu item.
        ! 12-SEP-94     NVA     Need to set MENU_NO_FINISH to TRUE in .int
        !                       if you do not want the user to exit the
        !                       program from the maint menu with _BACK.
        ! 26-OCT-94     JLS     Increased maximum length of menu item 
        !                       descriptions to 48.  But the length of
        !                       a menu's item descriptions should not 
        !                       exceed 24 if there is a $SPLIT command 
        !                       in that menu's MCL.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U   B U I L D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !   build the menu for display
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_build
          menu_initialize
          get_valid_operator
          if  not oper_okay then 
            action$ = 'menu_finish'
            exit routine
          end if
          menu_process_mcl
        end routine




        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I N I T I A L I Z E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Set up the first screen.  This is the screen that is used
        ! when the program first starts up.  Initialize number of access 
        ! tries to zero.  Initialize famous constants.
        !
        ! EXPECTED:
        !  u_str$        = header text (defined in calling program)
        ! 
        ! RESULT:
        !        access_tries% = initialized to zero
        !
        !  u_str$        = heading text
        !  access_tries% = counter for login attempts
        !  access_limit  = number of access tries allowed
        !  err_no_file%  = mcl_err% value for file not found
        !  err_syntax%   = mcl_err% value for syntax error
        !  err_bad_cmd%  = mcl_err% value for invalid command
        !  err_no_item%  = mcl_err% value for item not found in PROCEDURE
        !  err_dup_id%   = mcl_err% value for duplicate item ID
        !  err_bad_id%   = mcl_err% value for item ID error
        !  nospaces%     = edit$() param to discard spaces
        !  nolead%       = edit$() param to discard leading spaces
        !  noextra%      = edit$() param to discard extra spaces
        !  ucase%        = edit$() param to uppercase letters
        !  notrail%      = edit$() param to discard trailing spaces
        !  skipquote%    = edit$() param to supress editing within quotes
        !  action$       = next action to be taken
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_initialize
          set structure mc, field menukey : key "111111"
          screen_frame                        
          access_tries% = 0
          access_limit  = 3
          err_no_file%  = 1% 
          err_syntax%   = 2% 
          err_bad_cmd%  = 3%
          err_no_item%  = 4%
          err_dup_id%   = 5%
          err_bad_id%   = 6%
          nospace%      = 2%
          nolead%       = 8%
          noextra%      = 16%        
          ucase%        = 32%
          notrail%      = 128%
          skipquote%    = 256%
          clear area 21, 1, 21, 80
          u_str$ = 'Initializing ' + system_name$ + ' system...'
          gosub center
          message system_copyright$
          print at 21, u_int% : u_str$;
        end routine


        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ M C L
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Set up the operator ID and initials as symbols so other
        ! programs can access them.
        ! Process the MCL file; and load the internal arrays that
        ! are used to display and run the menu.
        !
        ! RESULT:
        !       menu_info_total% = number of MENU_INFO$(,) rows loaded
        !       menu_menu_total% = number of MENU_MENU$(,) rows loaded
        !       error            = error TRUE or FALSE flag
        !       fatal            = TRUE or FALSE flag
        !       action$          = next action to perform
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_mcl
          do
            menu_set_up_menu
            menu_init_for_process_mcl
            if  _error  then  exit do
            menu_open_mcl_file
            if  _error  then  exit do
            menu_read_mcl_file
            menu_close_mcl_file
            menu_process_command_end
            menu_info_total% = menu_info_slot%
            menu_menu_total% = menu_menu_slot%
            menu_check_called_menus
            menu_build_menu
          end do
          if  fatal  then
            action$ = "menu_finish"
          else
            action$ = "open_all_files"
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ S E T _ U P _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Set up the internal arrays used to store the processed
        ! MCL file information.
        !
        ! RESULT:
        !       info_row_max% = 400
        !       info_col_max% = 5
        !                ! maximum rows and columns for menu_info$(,) array
        !
        !       menu_row_max% = 10
        !       menu_col_max% = 6
        !                ! maximum rows and columns for menu_menu$(,) array
        !       menu_items%   = 0 index to menu_item%
        !
        !       menu_files_max% = 20
        !                ! maximum menu files
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        !       menu_menu$(menu_row_max%, menu_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = menu heading
        !                ! column 3 = beginning MENU_INFO$(,) slot
        !                ! column 4 = ending MENU_INFO$(,) slot
        !                ! column 5 = "N" if all numbered items on this menu
        !
        !       menu_level$(menu_row_max%, 2)
        !                ! row# = menu level
        !                ! column 1 = menu name
        !                ! column 2 = MENU_INFO$(,) slot for last item chosen
        !
        !       mcl_name$(menu_files_max%)
        !                ! file name for each .MCL file
        !
        !       menu_line_ctr%(menu_files_max%)
        !                ! line counter for each file used for menus
        !       menu_item%(info_row_max%) = index to items only
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine menu_set_up_menu

        info_row_max%   = 400
        info_col_max%   = 5
        menu_row_max%   = 20
        menu_col_max%   = 5
        menu_files_max% = 20
        menu_items%     = 0

        dim menu_info$(info_row_max%, info_col_max%)
        dim menu_menu$(menu_row_max%, menu_col_max%)
        dim menu_level$(menu_row_max%, 2)
        dim mcl_name$(menu_files_max%)
        dim menu_line_ctr%(menu_files_max%)
        dim menu_item%(info_row_max%)
        dim menu_ambiguous%(info_row_max%)

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I N I T _ F O R _ P R O C E S S _ M C L
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Initialize and set up variables for processing the MCL file.
        ! Go check for menu errors
        !
        ! EXPECTED:
        !       oper_menu$ = operator's MCL file name.
        !
        ! RESULT:
        !       error            = false
        !       fatal            = false
        !       mcl_fspec$       = oper_menu$
        !       mcl_level%       = menu level counter
        !       do_end%          = false
        !       menu_info_slot%  = menu information array slot counter
        !       menu_menu_slot%  = menu context array slot counter
        !       menu_line_ctr%() = menu line counter
        !       menu_files_max%  = max number of menu files
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_init_for_process_mcl
          mcl_fspec$ = oper_menu$
          mcl_level% = 0
          do_end% = false
          menu_info_slot% = 0
          menu_menu_slot% = 0
          for z1% = 1 to menu_files_max%
            menu_line_ctr%(z1%) = 0
          next z1%
          menu_check_mcl_file
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ O P E N _ M C L _ F I L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Increment the "include" level.
        ! Open a menu source file.
        !
        ! EXPECTED:
        !       mcl_fspec$   = name of MCL file to open
        !       err_no_file% = mcl_err% value for file not found
        !
        ! RESULT:
        !       mcl_level% is incremented
        !       mcl_name$(mcl_level%) is loaded with the MCL file name
        !       error        = TRUE or FALSE flag
        !       mcl_err%     = which menu error occurred
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_open_mcl_file
          mcl_level% = mcl_level% + 1
          mcl_name$(mcl_level%) = mcl_fspec$
          error = false
          when exception in
            open #mcl_level% : name mcl_fspec$, access input
          use
          end when
          if  _error then
            mcl_err% = err_no_file%
            menu_report_mcl_error
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ R E A D _ M C L _ F I L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Read a menu source file line.
        !
        ! EXPECTED:
        !       mcl_level% = source file channel / "include"  level
        !
        ! RESULT:
        !       mcl_line$ = source file line read in
        !       menu_line_ctr%() = line counter for each channel/level
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_read_mcl_file
          do
            line input #mcl_level%, eof eof? : mcl_line$
            if  eof? then exit do
            menu_line_ctr%(mcl_level%) = menu_line_ctr%(mcl_level%) + 1
            menu_process_mcl_line
          loop
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ C L O S E _ M C L _ F I L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Close the menu source file.
        ! Decrement the "include" level.
        !
        ! EXPECTED:
        !       mcl_level% = menu source file channel / "include" level
        !
        ! RESULT:
        !       menu_line_ctr%() is reinitialized
        !       mcl_level% is decremented
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_close_mcl_file
          close #mcl_level%
          menu_line_ctr%(mcl_level%) = 0
          mcl_level% = mcl_level% - 1
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ M C L _ L I N E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a menu source file line.
        !
        ! EXPECTED:
        !       mcl_line$    = raw, unprocessed .MCL file line
        !       err_syntax%  = mcl_err% value for syntax error
        !
        ! RESULT:
        !       mcl_err%     = error number if line is incomprehensible
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_mcl_line
          z1$ = ltrim$(mcl_line$)[1:1]
          select case z1$
          case ""
            ! skip blank lines
          case "!"
            ! skip comment lines
          case "$"
            menu_process_mcl_command
          case "%"
            menu_process_mcl_directive
          case else
            mcl_err% = err_syntax%  
            menu_report_mcl_error
          end select
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ M C L _ C O M M A N D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process MCL lines that are commands.
        ! These lines begin with a "$".
        !
        ! EXPECTED:
        !       mcl_line$    = .MCL file line which begins with a "$"
        !       nolead%      = edit$() param to discard leading spaces
        !       ucase%       = edit$() param to uppercase letters
        !       err_bad_cmd% = mcl_err% value for invalid command
        !
        ! RESULT:
        !       mcl_err%     = error number if unknown command
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_mcl_command
          z1$ = element$(edit$(mcl_line$, nolead% + ucase%), 1, " ")
          select case z1$
          case "$MENU"
            menu_process_command_menu
          case "$HEADING"
            menu_process_command_heading
          case "$TITLE"
            menu_process_command_title
          case "$TEXT"
            menu_process_command_text
          case "$ITEM"
            menu_process_command_item
          case "$SPLIT"
            menu_process_command_split
          case "$END"
            menu_process_command_end
          case else
            mcl_err% = err_bad_cmd%
            menu_report_mcl_error
          end select
        end routine
                            


        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ C O M M A N D _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $MENU command.
        !
        ! EXPECTED:
        !       mcl_line$   = .MCL file text line
        !       err_syntax% = mcl_err% value for syntax error
        !       nolead%     = edit$() param to discard leading spaces
        !       noextra%    = edit$() param to discard extra spaces
        !       ucase%      = edit$() param to uppercase letters
        !       notrail%    = edit$() param to discard trailing spaces
        !
        ! RESULT:
        !       do_end%     = set on (we will need to process a $END)
        !                     (we do this in case we don't ever find one)
        !       cur_menu$   = menu name
        !       item_nbr%   = 0% = item number/counter
        !       item_idz$   = "" = string of unique item IDs (per menu)
        !                           
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_command_menu
          menu_process_command_end
          do_end% = true
          z1$ = edit$(mcl_line$, nolead% + noextra% + ucase% + notrail%)
          if  elements(z1$, " ") <> 2  then
            mcl_err% = err_syntax% 
            menu_report_mcl_error
            exit routine
          end if
          cur_menu$ = z1$[len("$MENU ") + 1:len(z1$)]
          menu_load_arrays_for_command_menu
          item_nbr% = 0%
          item_idz$ = ""
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ L O A D _ A R R A Y S _ F O R _ C O M M A N D _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Load the arrays for a $MENU command.
        !
        ! EXPECTED:
        !       cur_menu$ = name of this menu
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        !       menu_menu$(menu_row_max%, menu_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = menu heading
        !                ! column 3 = beginning MENU_INFO$(,) slot
        !                ! column 4 = ending MENU_INFO$(,) slot
        !                ! column 5 = "N" if all numbered items on this menu
        !
        ! RESULT:
        !       menu_menu_slot% is incremented
        !       menu_info$(,) array is loaded
        !       menu_menu$(,) array is loaded
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_load_arrays_for_command_menu
          menu_increment_menu_info_slot  ! returns MENU_INFO_SLOT%
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$MENU"
          menu_info$(menu_info_slot%, 3) = ""
          menu_info$(menu_info_slot%, 4) = ""
          menu_info$(menu_info_slot%, 5) = ""
          menu_menu_slot% = menu_menu_slot% + 1
          menu_menu$(menu_menu_slot%, 1) = cur_menu$
          menu_menu$(menu_menu_slot%, 2) = ""
          menu_menu$(menu_menu_slot%, 3) = str$(menu_info_slot%)
          menu_menu$(menu_menu_slot%, 4) = ""
          menu_menu$(menu_menu_slot%, 5) = ""
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ C O M M A N D _ H E A D I N G
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $HEADING command.
        !
        ! EXPECTED:
        !       mcl_line$ = .MCL file text line
        !       cur_menu$ = current menu
        !       nolead%   = edit$() param to discard leading spaces
        !       noextra%  = edit$() param to discard extra spaces
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        !       menu_menu$(menu_row_max%, menu_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = menu heading
        !                ! column 3 = beginning MENU_INFO$(,) slot
        !                ! column 4 = ending MENU_INFO$(,) slot
        !                ! column 5 = "N" if all numbered items on this menu
        !
        ! RESULT:
        !       menu_info$(,) is loaded
        !       menu_menu$(,) is loaded
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_command_heading
          z1$ = edit$(mcl_line$, nolead% + noextra%)
          z2$ = z1$[len("$HEADING ") + 1:len(z1$)]
          menu_increment_menu_info_slot
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$HEADING"
          menu_info$(menu_info_slot%, 3) = ""
          menu_info$(menu_info_slot%, 4) = z2$
          menu_info$(menu_info_slot%, 5) = ""
          menu_menu$(menu_menu_slot%, 2) = z2$
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ C O M M A N D _ T I T L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $TITLE command.
        !
        ! EXPECTED:
        !       mcl_line$ = .MCL file text line
        !       cur_menu$ = current menu
        !       nolead%   = edit$() param to discard leading spaces
        !       noextra%  = edit$() param to discard extra spaces
        !
        ! RESULT:
        !       menu_info$(,) is loaded
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_command_title
          z1$ = edit$(mcl_line$, nolead% + noextra%)
          z2$ = z1$[len("$TITLE ") + 1:len(z1$)]
          menu_increment_menu_info_slot
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$TITLE"
          menu_info$(menu_info_slot%, 3) = ""
          menu_info$(menu_info_slot%, 4) = z2$
          menu_info$(menu_info_slot%, 5) = ""
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ C O M M A N D _ T E X T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $TEXT command.
        !
        ! EXPECTED:
        !       mcl_line$    = .MCL file text line
        !       cur_menu$    = current menu
        !       err_bad_cmd% = mcl_err% value for invalid command
        !
        !
        ! RESULT:
        !       menu_info$(,) is loaded
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_command_text
          z1% = pos(mcl_line$, '"', 1)
          if  z1% = 0  then  z1% = pos(mcl_line$, "'", 1)  ! find beginning quote
          if  z1% = 0  then
            z2% = -1%
          else
            z2% = pos(mcl_line$, mcl_line$[z1%:z1%], z1% + 1)
          end if
              ! find ending quote (if we had a beginning)
          if  z2% = 0  then
            mcl_err% = err_bad_cmd%
            menu_report_mcl_error
            exit routine
          end if
          text_text$ = mcl_line$[z1% + 1:z2% - 1]
          menu_load_array_for_command_text
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ L O A D _ A R R A Y _ F O R _ C O M M A N D _ T E X T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Load the array for a $TEXT command.
        !
        ! EXPECTED:
        !       cur_menu$ = current menu name
        !       text_text$ = text string
        !
        ! RESULT:
        !       menu_info$(,) is loaded for a $TEXT command
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_load_array_for_command_text
          menu_increment_menu_info_slot
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$TEXT"
          menu_info$(menu_info_slot%, 3) = ""
          menu_info$(menu_info_slot%, 4) = text_text$
          menu_info$(menu_info_slot%, 5) = ""
        end routine
  

                     
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ C O M M A N D _ I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $ITEM or $ITEM MENU command.
        !
        ! EXPECTED:
        !       mcl_line$    = MCL file text line
        !       err_bad_cmd% = mcl_err% value for invalid command
        !       nolead%      = edit$() param to discard leading spaces
        !       noextra%     = edit$() param to discard extra spaces
        !       ucase%       = edit$() param to uppercase letters
        !       notrail%     = edit$() param to discard trailing spaces
        !
        ! RESULT:
        !       item_line$     = massaged MCL_LINE$
        !       item_elements% = number of "elements" in ITEM_LINE$
        !       mcl_err%       = error number  if  any
        !                                    
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_command_item
          item_line$ = edit$(mcl_line$, nolead% + noextra% + ucase% + notrail%)
          item_elements% = elements(item_line$, " ")
          if  item_line$[1:10] = "$ITEM MENU"  then
            if  item_elements% >= 3  then
              menu_item_menu
            else
              mcl_err% = err_bad_cmd% 
              menu_report_mcl_error
            end if
          else                  ! $ITEM
            if  (item_elements% = 2)  or  (item_elements% = 3)  then
              menu_item_item
            else
              mcl_err% = err_bad_cmd%
              menu_report_mcl_error
            end if
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I T E M _ I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $ITEM command in either of the following forms:
        !
        !       $ITEM <item-id> item-name
        !       $ITEM item-name
        !
        ! EXPECTED:
        !       cur_menu$      = current menu name
        !       item_elements% = number of "elements" in ITEM_LINE$
        !       
        ! RESULT:
        !       item_info$(,) is loaded for a $ITEM command
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_item_item
          select case item_elements%
          case is = 2
            menu_item_item_without_name
          case is = 3
            menu_item_item_with_name
          end select
          if  (error)  then  exit routine
                ! RETURNED:
                !       item_id$        = item ID
                !       item_proc_name$ = item procedure name
                !       item_desc$      = item description
          item_idz$ = item_idz$ + (item_id$ + ",")
                ! add to item IDs string
          menu_increment_menu_info_slot
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$ITEM"
          menu_info$(menu_info_slot%, 3) = item_id$
          menu_info$(menu_info_slot%, 4) = item_proc_name$
          menu_info$(menu_info_slot%, 5) = item_desc$
          menu_items% = menu_items% + 1
          menu_item%(menu_items%) = menu_info_slot%
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I T E M _ I T E M _ W I T H O U T _ N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $ITEM command in the following form:
        !
        !       $ITEM item-name
        !
        ! EXPECTED:
        !       item_nbr%      = item number
        !       item_line$     = massaged MCL_LINE$
        !              
        ! RESULT:
        !       item_id$        = item ID
        !       item_proc_name$ = item procedure name
        !       item_desc$      = item description
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_item_item_without_name
          item_nbr% = item_nbr% + 1%
          item_id$  = str$(item_nbr%)
          menu_check_for_duplicate_item
          if  _error  then  exit routine
          item_proc_name$ = element$(item_line$, 2, " ")
          menu_find_item_desc
          if  _error  then  exit routine
          item_desc$ = proc(desc)
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I T E M _ I T E M _ W I T H _ N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $ITEM command in the following form:
        !
        !       $ITEM <item-id> item-name
        !
        ! EXPECTED:
        !        item_line$     = massaged MCL_LINE$
        !
        ! RESULT:
        !       item_id$        = item ID
        !       item_proc_name$ = item procedure name
        !       item_desc$      = item description
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_item_item_with_name
          item_id$ = element$(item_line$, 2, " ")
          menu_check_item_id
          if  _error  then  exit routine
          item_proc_name$ = element$(item_line$, 3, " ")
          menu_find_item_desc
          if  _error  then  exit routine
          item_desc$ = proc(desc)
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I T E M _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a   $ITEM MENU <item_id> menu-name   command
        !        
        ! EXPECTED:
        !       cur_menu$ = current menuname
        !       item_line$ = massaged MCL_LINE$
        !
        ! RESULT:
        !       menu_info$(,) is loaded
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_item_menu
          if  scan(element$(item_line$, 3, " "), "<>") = 0  then
            menu_item_menu_without_name
          else
            menu_item_menu_with_name
          end if
          if  _error  then  exit routine
                ! RETURNED:
                !       item_id$   = item ID
                !       menu_name$ = menu name (to call)
                !       menu_desc$ = menu description text
          item_idz$ = item_idz$ + (item_id$ + ",")
          menu_increment_menu_info_slot
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$ITEM MENU"
          menu_info$(menu_info_slot%, 3) = item_id$
          menu_info$(menu_info_slot%, 4) = menu_name$
          menu_info$(menu_info_slot%, 5) = menu_desc$
          menu_items% = menu_items% + 1
          menu_item%(menu_items%) = menu_info_slot%
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I T E M _ M E N U _ W I T H O U T _ N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a   $ITEM MENU menu-name   command
        !        
        ! EXPECTED:
        !       mcl_line$  = .MCL file text line
        !       item_line$ = massaged MCL_LINE$
        !       item_nbr%  = item number counter
        !       nolead%    = edit$() param to discard leading spaces
        !       noextra%   = edit$() param to discard extra spaces
        !       notrail%   = edit$() param to discard trailing spaces
        !
        ! RESULT:
        !       item_id$   = item ID
        !       menu_name$ = menu name
        !       menu_desc$ = description text
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_item_menu_without_name
          item_nbr% = item_nbr% + 1%
          item_id$  = str$(item_nbr%)
          menu_check_for_duplicate_item
          if  _error  then  exit routine
          menu_name$ = element$(item_line$, 3, " ")
          z1$ = edit$(mcl_line$, nolead% + noextra% + notrail%)
          z1% = pos(z1$, " ", len("$ITEM MENU ") + 1)
          if  z1% > 0%  then
            menu_desc$ = z1$[z1% + 1:len(z1$)]
          else
            menu_desc$ = menu_name$ + " menu"
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I T E M _ M E N U _ W I T H _ N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a   $ITEM MENU <item_id> menu-name   command
        !        
        ! EXPECTED:
        !       mcl_line$  = .MCL file text line
        !       item_line$ = massaged MCL_LINE$
        !       item_nbr%  = item number counter
        !       nolead%   = edit$() param to discard leading spaces
        !       noextra%  = edit$() param to discard extra spaces
        !       notrail%  = edit$() param to discard trailing spaces
        !
        ! RESULT:
        !       item_id$   = item ID
        !       menu_name$ = menu name
        !       menu_desc$ = description text
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_item_menu_with_name
          item_id$ = element$(item_line$, 3, " ")
          menu_check_item_id
          if  _error  then  exit routine
          menu_name$ = element$(item_line$, 4, " ")
          z1$ = edit$(mcl_line$, nolead% + noextra% + notrail%)
          z1% = pos(z1$, "> ", 1)
          z1% = pos(z1$, " ", z1% + 2)
          if  z1% > 0%  then
            menu_desc$ = z1$[z1% + 1:len(z1$)]
          else
            menu_desc$ = menu_name$ + " menu"
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ C H E C K _ I T E M _ I D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Here only when an item name is specified.
        !
        ! Item IDs must be enclosed between "<" and ">" characters,
        ! be 12 characters max, contain only "legal" characters, and be ++DJS++ 26-NOV-1990
        ! unique within the menu.
        !
        ! EXPECTS:
        !       item_id$    = item ID
        !       item_idz$   = string of unique item IDs
        !       err_syntax% = mcl_err% value for syntax error
        !       err_bad_id% = mcl_err% value for item ID error
        !
        ! RETURNS:
        !       item_id$    = "massaged" item ID
        !       mcl_err%    = error number  if  any
        !
        ! Exceeds 22 lines to completely validate item name in one place.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_check_item_id
          error = false
          if  (left$(item_id$, 1)  <> "<")  and &
              (right$(item_id$, 1) <> ">")  then
            mcl_err% = err_syntax% 
            menu_report_mcl_error
            set error on
            exit routine
          end if
          item_id$ = item_id$[2:len(item_id$)-1]
                ! remove from between the "<" and ">" characters
          if  len(item_id$) > 12%  then
            ! item name too long 12 max ++DJS++ 26-NOV-1990
            mcl_err% = err_bad_id% 
            menu_report_mcl_error
            set error on
            exit routine
          end if
          legal_chars$ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789"
          for  z1% = 1% to len(item_id$)
            if  pos(legal_chars$, item_id$[z1%:z1%]) = 0  then
              mcl_err% = err_bad_id% 
              menu_report_mcl_error
              set error on
              exit for
            end if
          next z1%
          if  _error  then  exit routine
          menu_check_for_duplicate_item
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ C H E C K _ F O R _ D U P L I C A T E _ I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check for a duplicate item.
        !
        ! EXPECTED:
        !       item_id$    = item ID to check.
        !       item_idz$   = string of item IDs we have so far.
        !       err_dup_id% = mcl_err% value for duplicate item ID
        !
        ! RESULT:
        !       error = true  if  a duplicate.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_check_for_duplicate_item
          if  match(item_idz$, item_id$) <> 0  then
            mcl_err% = err_dup_id%   
            menu_report_mcl_error
            set error on
          end if
        end routine


            
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ F I N D _ I T E M _ D E S C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Get the matching PROCEDURE record for the item procedure name.
        !
        ! EXPECTS:
        !       item_proc_name$ = item name
        !       err_no_item%    = mcl_err% value for item not in PROCEDURE
        !
        ! RETURNS:
        !       mcl_err% = error number if item name not found
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_find_item_desc
          set structure proc, field name : key item_proc_name$
          if  _extracted = 0 then 
            mcl_err% = err_no_item%
            menu_report_mcl_error
            set error on
          end if
        end routine


            
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ C O M M A N D _ S P L I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $SPLIT command.
        !
        ! EXPECTED:
        !       mcl_line$ = .MCL file text line
        !
        ! RESULT:
        !       menu_info$(,) is loaded for a $SPLIT command.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_command_split
          menu_increment_menu_info_slot
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$SPLIT"
          menu_info$(menu_info_slot%, 3) = ""
          menu_info$(menu_info_slot%, 4) = ""
          menu_info$(menu_info_slot%, 5) = ""
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ C O M M A N D _ E N D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a $END command.
        ! The $END may be real or implied because we came to a $MENU
        ! or the end of main MCL file.
        !
        ! EXPECTED:
        !       do_end%   = true  if  we need to run this routine
        !       cur_menu$ = current menu name
        !       menu_menu_slot% = menu_menu$(,) slot
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        !       menu_menu$(menu_row_max%, menu_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = menu heading
        !                ! column 3 = beginning MENU_INFO$(,) slot
        !                ! column 4 = ending MENU_INFO$(,) slot
        !                ! column 5 = "N" if all numbered items on this menu
        !
        !       menu_info$(,) is loaded
        !       menu_menu$(,) is loaded
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_command_end
          if  not do_end%  then  exit routine
          menu_increment_menu_info_slot
          menu_info$(menu_info_slot%, 1) = cur_menu$
          menu_info$(menu_info_slot%, 2) = "$END"
          menu_info$(menu_info_slot%, 3) = ""
          menu_info$(menu_info_slot%, 4) = ""
          menu_info$(menu_info_slot%, 5) = ""
          menu_menu$(menu_menu_slot%, 4) = str$(menu_info_slot%)
                ! last MENU_INFO$(,) slot for the current menu
          menu_load_items_type
          do_end% = false
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ L O A D _ I T E M S _ T Y P E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Load column 5 of the MENU_MENU$(,) slot with an "N" if
        ! this menu contains only "numbered" items.
        !
        ! EXPECTED:
        !       menu_menu_slot% = current MENU_MENU$(,) row
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        !       menu_menu$(menu_row_max%, menu_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = menu heading
        !                ! column 3 = beginning MENU_INFO$(,) slot
        !                ! column 4 = ending MENU_INFO$(,) slot
        !                ! column 5 = "N" if all numbered items on this menu
        !
        ! RESULT:
        !        frst_menu_slot%  = first MENU_INFO$(,) slot for this menu
        !        last_menu_slot%  = last MENU_INFO$(,) slot for this menu
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_load_items_type
          frst_menu_slot% = val(menu_menu$(menu_menu_slot%, 3))
          last_menu_slot% = val(menu_menu$(menu_menu_slot%, 4))
          z2% = true  ! z2% = true = all numeric items
          for  z1% = frst_menu_slot% to last_menu_slot%
            if  menu_info$(z1%, 2)[1:5] <> "$ITEM"  then  iterate for
            if  not(valid(menu_info$(z1%, 3), "INTEGER"))  then
              z2% = false
              exit for
            end if
          next z1%
          if  z2%  then  menu_menu$(menu_menu_slot%, 5) = "N" ! set "all numbered items" flag
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ I N C R E M E N T _ M E N U _ I N F O _ S L O T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Increment the MENU_INFO_SLOT% counter
        !
        ! EXPECTED:
        !        menu_info_slot% = menu_info$(,) row counter
        !
        ! RESULT:
        !        menu_info_slot% is incremented by one.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_increment_menu_info_slot
          menu_info_slot% = menu_info_slot% + 1
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ M C L _ D I R E C T I V E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process a MCL directive.
        ! Directives begin with a "%" character.
        !
        ! EXPECTED:
        !       mcl_line$    = MCL file line which begins with a "%"
        !       err_bad_cmd% = mcl_err% value for invalid command
        !       nolead%      = edit$() param to discard leading spaces
        !       ucase%       = edit$() param to convert to uppercase
        !
        ! RESULT:
        !       mcl_err% = error number  if  any
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_mcl_directive
          z1$ = element$(edit$(mcl_line$, nolead% + ucase%), 1, " ")
          select case z1$
          case "%INCLUDE"
            menu_process_directive_include
          case else
            mcl_err% = err_bad_cmd%
            menu_report_mcl_error
          end select
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P R O C E S S _ D I R E C T I V E _ I N C L U D E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Process the %INCLUDE directive.
        !
        ! EXPECTED:
        !       mcl_line$ = MCL file text line
        !       nolead%   = edit$() param to discard leading spaces
        !       noextra%  = edit$() param to discard extra spaces
        !
        ! RESULT:
        !       mcl_fspec$ = file name to include.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_process_directive_include
          z1$ = edit$(mcl_line$, nolead% + noextra%)
          mcl_fspec$ = z1$[len("%INCLUDE ") + 1:len(z1$)] ! name of file to include
          menu_check_mcl_file
          if  _error  then  exit routine
          menu_open_mcl_file
          if  _error  then  exit routine
          menu_read_mcl_file
          menu_close_mcl_file
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D I S P L A Y _ F I R S T _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Set up to display the level 1 menu.
        !
        ! EXPECTED:
        !       menu_menu$(1, 1) = level one menu name
        !
        ! RESULT:
        !       cur_menu%   = 1
        !       menu_level% = 1
        !       menu_level$(1, 1) is loaded with the menu name
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_first_menu
          cur_menu%   = 1
          menu_level% = 1
          menu_level$(menu_level%, 1) = menu_menu$(cur_menu%, 1) ! menu name
          menu_level$(menu_level%, 2) = "" ! last item chosen
          action$ = "menu_paint_menu" 
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ P A I N T _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Paint the screen with a menu.
        !
        ! EXPECTED:
        !       cur_menu%        = current menu number, i.e. menu to paint
        !       frst_menu_slot%  = first MENU_INFO$(,) slot for this menu
        !       last_menu_slot%  = last MENU_INFO$(,) slot for this menu
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        !       menu_menu$(menu_row_max%, menu_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = menu heading
        !                ! column 3 = beginning MENU_INFO$(,) slot
        !                ! column 4 = ending MENU_INFO$(,) slot
        !                ! column 5 = "N" if all numbered items on this menu
        !
        ! RESULT:
        !       cur_menu$     = current menu name
        !       menu_heading$ = current menu heading
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_paint_menu
          if  pop_up_menu then
            menu_menu_frame
            action$ = 'menu_ask_item'
            exit routine
          end if
          cur_menu$       = menu_menu$(cur_menu%, 1)
          menu_heading$   = menu_menu$(cur_menu%, 2)
          frst_menu_slot% = val(menu_menu$(cur_menu%, 3))
          last_menu_slot% = val(menu_menu$(cur_menu%, 4))
          all_numbered_items% = menu_menu$(cur_menu%, 5) = "N"
                ! this is an "all numbered items" menu
          for menu_info_slot% = frst_menu_slot% to last_menu_slot%
            select case menu_info$(menu_info_slot%, 2)
            case "$MENU"          : menu_display_menu
            case "$HEADING"       : ! we got menu_heading$ above
            case "$TITLE"         : menu_display_title
            case "$TEXT"          : menu_display_text
            case "$ITEM"          : menu_display_item
            case "$ITEM MENU"     : menu_display_item_menu
            case "$SPLIT"         : menu_display_split
            case "$END"           : ! nothing to do
            case else             : !??? REPORT ERROR ???
            end select
          next menu_info_slot%
          action$ = "menu_ask_item"
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D I S P L A Y _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Painting menu -- found $MENU
        ! Set up to paint a menu screen.
        !
        ! RESULT:
        !       cur_row% = current row is set to 3
        !       cur_col% = current column is set to 1
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_menu
          menu_menu_frame
          cur_row% = 3
          cur_col% = 1
        end routine


        
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ M E N U _ F R A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Create and paint the menu frame.
        !
        ! EXPECTED:
        !        menu_heading$ = header text
        !        cur_menu$     = current menu name
        !        menu_level%   = menu level
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_menu_frame
          if  pop_up_menu then
            u_str$ = system_title$
            screen_frame
          else
            u_str$ = system_name$
            gosub screen_frame                        
            z3$ = space$(the_width)
            print reverse, at 2, 1 : z3$
            z3$ = " " + menu_heading$ + " "
            z% = (the_width/2) - (len(z3$)/2)
            print reverse, bold, at 2, z% : z3$;
            print reverse, at 2, 2 : cur_menu$;
            z1$ = "Level: " + str$(menu_level%)
            print reverse, at 2, the_width - len(z1$) : z1$; ' ';
          end if
          message menu_message$
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D I S P L A Y _ T I T L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Painting menu -- found $TITLE
        ! Paint a blank line and then the title text.
        !
        ! EXPECTED:
        !       menu_info_slot% = MENU_INFO$(,) slot
        !
        ! RESULT:
        !       cur_row% = current screen row incremented by two
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_title
          cur_row% = cur_row% + 1
          print reverse, bold, at cur_row%, cur_col% : menu_info$(menu_info_slot%, 4)
          cur_row% = cur_row% + 1
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D I S P L A Y _ T E X T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Painting menu -- found $TEXT.
        !
        ! EXPECTED:
        !       menu_info_slot% = MENU_INFO$(,) slot
        !
        ! RESULT:
        !       cur_row% = current screen row incremented
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_text
          print at cur_row%, cur_col% : menu_info$(menu_info_slot%, 4)
          cur_row% = cur_row% + 1
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D I S P L A Y _ I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Painting menu - found $ITEM
        !
        ! EXPECTED:
        !       cur_row% = screen row
        !       cur_col% = screen column
        !       menu_level% = menu level
        !       menu_info_slot% = MENU_INFO$(,) slot
        !       all_numbered_items% = true or false
        !
        ! RESULT:
        !       cur_row% = current row incremented
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_item
          print at cur_row%, cur_col%:;
          if  menu_info_slot% =  val(menu_level$(menu_level%, 2))  then
            print bold : " *";         
          else
            print "  ";
          end if
          if  (all_numbered_items%)  then
            print bold : lpad$(menu_info$(menu_info_slot%, 3), 2); 
            print " "; menu_info$(menu_info_slot%, 5)
          else
            print bold : rpad$(menu_info$(menu_info_slot%, 3), 10); 
            print " "; menu_info$(menu_info_slot%, 5)
          end if
                ! display the item ID and description
          cur_row% = cur_row% + 1
                ! increment line counter
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U  _ D I S P L A Y _ I T E M _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Painting menu -- found $ITEM MENU
        !
        ! EXPECTED:
        !       cur_row% = current screen row
        !       cur_col% = current screen column
        !       menu_level% = menu level
        !       menu_info_slot% = MENU_INFO$(,) slot
        !       all_numbered_items% = true or false
        !
        ! RESULT:
        !       cur_row% = screen row is incremented
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_item_menu
          print at cur_row%, cur_col%:;
          if  menu_info_slot% =  val(menu_level$(menu_level%, 2))  then
            print bold : " *";
          else
            print "  ";
          end if
                ! last item chosen on this menu gets marked with an "*"
          if  (all_numbered_items%)  then
            print bold : lpad$(menu_info$(menu_info_slot%, 3), 2);
            print " "; menu_info$(menu_info_slot%, 5)
          else
            print bold : rpad$(menu_info$(menu_info_slot%, 3), 10);
            print " "; menu_info$(menu_info_slot%, 5)
          end if
          cur_row% = cur_row% + 1
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D I S P L A Y _ S P L I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Painting menu -- found $SPLIT
        ! Adjust row and column to start over at top of column two.
        !
        ! RESULT:
        !       cur_row% = set to row 3
        !       cur_col% = set to column 41
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_split
          cur_row% = 3
          cur_col% = 41
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ A S K _ I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask the user for the menu item ID to run.
        ! They can choose any valid item on any of the
        ! "set of menu screens."
        !
        ! RESULT:
        !       requested_item$ = user's repsonse
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_ask_item
          if  pop_up_menu then
            menu_do_pop_up
            exit routine
          end if
          prompt$ = "Item"
          length  = 12
          uc_response? = true
          validation$ = 'required'
          help$     = "menu item"
          input_response
          clear area 21, 1, 21, 80
          if  _back  then
            menu_do_back
            exit routine
          end if
          if  _exit  then
            menu_do_exit
            exit routine
          end if
          requested_item$ = trim$(reply$)
          action$ = "menu_verify_item"
        end routine

                                                                         
                                               
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U   D O   P O P   U P
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_do_pop_up
          line input menu menu_text$, default input_menu_default$ : reply$
          if _back and menu_no_finish then   !++nva defined in .int
            action$ = 'exit'         
            exit routine              
          end if
            if  _exit or _back then
              action$ = 'menu_finish'
              exit routine
            end if
            input_menu_default$ = _string
            requested_item$ = reply$
            action$ = 'menu_verify_item'
        end routine





        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ V E R I F Y _ I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Find the MENU_INFO$(,) slot for the requested item.
        !
        ! EXPECTED:
        !       requested_item$  = user requested item ID
        !       frst_menu_slot%  = first MENU_INFO$(,) slot for this menu
        !       last_menu_slot%  = last MENU_INFO$(,) slot for this menu
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        ! RESULT:
        !       menu_info_slot% = MENU_INFO$(,) slot for requested item
        !       desired_menu$   = menu desired item is on if we're not
        !                          already on the correct menu
        !
        !       menu_level$(menu_row_max%, 2)
        !                ! row# = menu level
        !                ! column 1 = menu name
        !                ! column 2 = MENU_INFO$(,) slot for last item chosen
        !
        !
        ! Exceeds 22 lines to completely verify item in one place.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_verify_item
          found_count% = 0%
          saved_slot%  = 0%
          for z% = 1% to menu_items%
            menu_info_slot% = menu_item%(z%)
            z1$ = menu_info$(menu_info_slot%, 3%)[1:len(requested_item$)]
            if  requested_item$ = z1$  then
              found_count% = found_count% + 1%
              saved_slot%  = menu_info_slot%
              menu_ambiguous%(found_count%) = menu_info_slot%
            end if
          next z%
                ! find MENU_INFO$(,) slot for requested item
          if  found_count% = 0%  then
            message error : "Item not found: " + requested_item$
            action$ = "menu_ask_item"
            exit routine
          end if
          if  found_count% > 1  then
            menu_display_ambiguous
            if  _error  then
              action$ = "menu_ask_item"
              exit routine
            end if
          end if
          menu_info_slot% = saved_slot%
                ! slot of valid, requested item
          if  not pop_up_menu then &
            menu_level$(menu_level%, 2) = str$(menu_info_slot%)
                ! update last item chosen
          select case menu_info$(menu_info_slot%, 2)
          case "$ITEM"
            menu_get_procedure_info
            if  _error  then
              action$ = "menu_ask_item"
              exit routine
            end if
            action$ = "menu_run_item"
          case "$ITEM MENU"
            desired_menu$ = menu_info$(menu_info_slot%, 4)
            action$ = "menu_call_menu"
          end select
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U   D I S P L A Y   A M B I G U O U S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! an ambiguous selection was entered.  Display them and let the
        ! user pick one
        !
        ! Expected:
        !       found_count% = number of ambiguous items found
        !       menu_ambiguous%() = menu numbers
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_display_ambiguous
          mda_init
          mda_process_items
          if  not mda_done or  reply$ = '' then
            set error on
          else
            saved_slot% = menu_ambiguous%(val(reply$) + mda_base%)
          end if
          set window : current save_window$
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M D A   I N I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! setup to display ambiguous menu item selections
        !
        ! Expected:
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mda_init
          max_ambig_disp% = min(found_count%, 15%)
          mda_display_offset% = 0%
          mda_base% = 0%
          ask window : current save_window$
          clear area 3, 1, 21, current_margin
          clear area box : 3, 1, 5+max_ambig_disp%, current_margin
          z$ = " Ambiguous Menu Items "
          print bold, reverse, at 4, (current_margin/2)-(len(z$)/2) : z$
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M D A   P R O C E S S   I T E M S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! display the selections and get the users response
        !
        ! Expected:
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mda_process_items
          for menu_ambig_ctr% = 1 to found_count%
            mda_display_option
            if  mda_display_offset% = max_ambig_disp%  then
              mda_ask
              if mda_done then  exit for
              mda_display_offset% = 0%
              mda_base% = mda_base% + max_ambig_disp% ! offset into table
            end if
          next menu_ambig_ctr%
          if  not(mda_done)  then
            if mda_display_offset% > 0  then  mda_ask
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M D A   D I S P L A Y   O P T I O N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! display one of the ambiguous options
        !
        ! Expected:
        !       menu_ambiguous%() = array of ambiguous items
        !       menu_info$()      = array of info about items
        !       all_numbered_items% = true if using numbers instead of names
        !
        ! Result  :
        !       mda_display_offset% is incremented
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mda_display_option
          z = menu_ambiguous%(menu_ambig_ctr%)
          mda_display_offset% = mda_display_offset% + 1
          print at 4 + mda_display_offset%, 3: &
                  format$(mda_display_offset%, "##) ");
          if  all_numbered_items%  then
            print lpad$(menu_info$(z, 3), 2); "  "; menu_info$(z, 5)
          else
            print rpad$(menu_info$(z, 3), 10); "  "; menu_info$(z, 5)
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M D A   A S K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! ask if one of the items displayed on the screen is the one the
        ! user wants
        !
        ! Expected:
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mda_ask
          mda_done = false
          if  mda_display_offset% = 15%  then
            message "Press RETURN for the next screen"
          end if
          prompt$ = "Sequence number"
          length = 4
          validation$ = 'integer;allow 1 to ' + str$(mda_display_offset%)
          input_response
          if _back  or  _exit  then
            mda_done = true
            exit routine
          end if
          if  reply$ <> '' then
            mda_done = true
          else                            
            if mda_display_offset% = 15%  then &
                clear area 5, 3, 4+max_ambig_disp%, current_margin - 1
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ G E T _ P R O C E D U R E _ I N F O
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Get the matching PROCEDURE record inforamtion.
        !
        ! EXPECTED:
        !       menu_info_slot% = MENU_INFO$(,) slot for requested item
        !
        ! RESULT:
        !       error = true  if  not found
        !
        !       proc_dispatch_type$  = dispatch type (ROUTINE, COMMAND)
        !       proc_dispatch_text$  = dispatch text
        !       proc_parameter_text$ = parameter
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_get_procedure_info
          set structure proc, field name : key menu_info$(menu_info_slot%, 4)
                ! procedure name for this item
          if  _extracted = 0 then 
            message error : "Item " + menu_info$(menu_info_slot%, 4) + &
                            " not found in PROCEDURE structure"
            exit routine
          end if
          proc_dispatch_type$  = proc(dispatch_type)
          proc_dispatch_text$  = proc(dispatch_text)
          proc_parameter_text$ = proc(parameter_text)
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ R U N _ I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Set up the parameter as a symbol.
        ! Run the chosen item (if a routine or command).
        !
        ! EXPECTED:
        !       proc_dispatch_type$  = procedure dispatch type
        !       proc_dispatch_text$  = routine name or DCL command
        !       proc_parameter_text$ = parameter
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_run_item
          select case proc_dispatch_type$
            case "COMMAND"
              clear
              pass "OPER_PARAMETER :== " + proc_parameter_text$
              pass proc_dispatch_text$
            case "ROUTINE"
                dispatch proc_dispatch_text$
          end select
          action$ = "menu_paint_menu"
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ C A L L _ M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! The chosen item is a $ITEM MENU.
        !
        ! Check if the menu to "call" was already called on our way to
        ! where we are now (we'll go backwards)  OR  if this is a new
        ! menu (we're going forwards).
        !
        ! EXPECTED:
        !       menu_level%   = current menu level
        !       desired_menu$ = menu screen we want to set up
        !
        ! RESULT:
        !       menu_level% = set for new or previous menu
        !       cur_menu$   = set to desired menu
        !       menu_level$(menu_level%, 1) = set to desired menu
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_call_menu
          found = false
          for z1% = 1% to menu_level%
            if  desired_menu$ = menu_level$(z1%, 1)  then
              found = true
              menu_level% = z1%
              exit for
            end if
          next z1%
          if  not found  then
            menu_level% = menu_level% + 1
            menu_level$(menu_level%, 1) = desired_menu$
            menu_level$(menu_level%, 2) = ""
          end if
          cur_menu$ = menu_level$(menu_level%, 1)
          menu_get_menu_number
                ! returns cur_menu%
          action$ = "menu_paint_menu"
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D O _ E X I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! If we're at the level 1 menu then exit program
        ! else go directly to the level 1 menu.
        !
        ! EXPECTED:
        !       menu_level% = current menu level
        !
        ! RESULT:
        !       menu_level% = set to level 1
        !       cur_menu$ = level 1 menu name
        !                  OR
        !       exit program
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_do_exit
          if  menu_level% = 1  then
            action$ = "menu_finish"
          else
            menu_level% = 1
            cur_menu$ = menu_level$(menu_level%, 1)
            menu_get_menu_number
                ! returns cur_menu%
            action$ = "menu_paint_menu"
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ D O _ B A C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Go back one menu level.
        !
        ! EXPECTED:
        !       menu_level% = current menu level
        !
        ! RESULT:
        !       menu_level% = new menu level
        !       cur_menu$   = new menu name
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_do_back
          menu_level% = max(1, menu_level% - 1)
                ! can't go any farther back than level 1
          cur_menu$ = menu_level$(menu_level%, 1)
          menu_get_menu_number
                ! returns cur_menu%
          action$ = "menu_paint_menu"
        end routine

              

        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ G E T _ M E N U _ N U M B E R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Given a menu name, return the menu number.
        !
        ! EXPECTED:
        !        cur_menu$ = menu name
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !
        ! RETURNED:
        !        cur_menu% = menu number
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_get_menu_number
          for  z1% = 1% to menu_menu_total%
            if  menu_menu$(z1%, 1) = cur_menu$  then  exit for
          next z1%
          cur_menu% = z1%
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ C H E C K _ M C L _ F I L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check if the MCL file exists.
        !
        ! EXPECTED:
        !        mcl_fspec$  = file to be checked
        !       err_no_file% = mcl_err% value for file not found
        !
        ! RESULT:
        !        error = true  if  file NOT found
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_check_mcl_file
          error = false
          if  pos(mcl_fspec$, ".") = 0  then
            mcl_fspec$ = mcl_fspec$ + ".MCL"
          end if
          z1$ = findfile$(mcl_fspec$)
          if  z1$ = ""  then
            error = true
            mcl_err% = err_no_file% 
            menu_report_mcl_error
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ C H E C K _ C A L L E D _ M E N U S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Verify that all of the $ITEM MENU <item-id> menu_name
        ! actually have a corresponding $MENU menu_name.
        !
        ! EXPECTED:
        !        menu_info_total% = number of MENU_INFO$(,) slots used
        !        menu_menu_total% = number of MENU_MENU$(,) slots used
        !
        !       menu_info$(info_row_max%, info_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = command
        !                ! column 3 = item number (if $ITEM)
        !                ! column 4 = text 1
        !                ! column 5 = text 2
        !
        !       menu_menu$(menu_row_max%, menu_col_max%)
        !                ! column 1 = menu name
        !                ! column 2 = menu heading
        !                ! column 3 = beginning MENU_INFO$(,) slot
        !                ! column 4 = ending MENU_INFO$(,) slot
        !                ! column 5 = "N" if all numbered items on this menu
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_check_called_menus
          for z1% = 1% to menu_info_total%
            if  menu_info$(z1%, 2) = "$ITEM MENU"  then
              found = false
              for z2% = 1% to menu_menu_total%
                if  menu_menu$(z2%, 1) = menu_info$(z1%, 4)  then
                  found = true
                  exit for
                end if
              next z2%
              if  not found  then
                menu_name$ = menu_info$(z1%, 4)
                menu_report_called_menu_error
              end if
            end if
          next z1%
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ R E P O R T _ C A L L E D _ M E N U _ E R R O R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Report that a $ITEM MENU <item-id> menu_name does not
        ! have a corresponding $MENU menu_name.
        !
        ! EXPECTED:
        !       menu_name$ = menu to call that doesn't exist
        !
        ! RESULT:
        !       fatal = true
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_report_called_menu_error
          fatal = true
          message error : ""
          print bold, at 14, 5 : "Called menu does not exist";
          print at 16, 5 : "Command    : $ITEM MENU";
          print at 17, 5 : "Menu name  : "; menu_name$;
          delay
          clear area 14, 1, 19, 80
          message "Initializing system..."
          print at 23, 52:;
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ R E P O R T _ M C L _ E R R O R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Report an MCL file error.
        !
        ! EXPECTED:
        !  mcl_err%      = error number
        !  mcl_level%    = menu level
        !  mcl_line$     = MCL file line
        !  err_no_file%  = mcl_err% value for file not found
        !  err_syntax%   = mcl_err% value for syntax error
        !  err_bad_cmd%  = mcl_err% value for invalid command
        !  err_no_item%  = mcl_err% value for item not found in PROCEDURE
        !  err_dup_id%   = mcl_err% value for duplicate item ID
        !  err_bad_id%   = mcl_err% value for item ID error
        !
        ! RESULT:
        !  fatal         = true
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_report_mcl_error
          fatal = true
          message error : ""
          print at 14, 5:;
          select case mcl_err%
            case err_no_file% :  print bold : "File not found";
            case err_syntax%  :  print bold : "Syntax error";
            case err_bad_cmd% :  print bold : "Invalid command";
            case err_no_item% :  print bold : "Item "; item_proc_name$; &
                                              " not found in PROCEDURE"
            case err_dup_id%  :  print bold : "Duplicate item ID"
            case err_bad_id%  :  print bold : "Item ID error"
          end select
          print at 16, 5 : "Menu file  : "; mcl_name$(mcl_level%);
          print at 17, 5 : "Line number: "; str$(menu_line_ctr%(mcl_level%));
          print at 18, 5 : "Statement  : "; mcl_line$;
          delay
          clear area 14, 1, 19, 80
          message "Initializing system..."
          print at 23, 52:;
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U   B U I L D   M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_build_menu
          cur_menu% = 1
          dim mbm_last_info(menu_menu_slot%)
          dim mbm_cur_info(menu_menu_slot%)
          dim mbm_menus(menu_menu_slot%)
          for z = 1 to menu_menu_slot%
            mbm_cur_info(z) = val(menu_menu$(z, 3))
            mbm_last_info(z) = val(menu_menu$(z, 4))
          next z
          mbm_menus(1) = 1
          mbm_menu_index = 1
          menu_text$ = '%at 3, center, %autodisplay off, %autovbar off'
          mbm_index = 0
          do
            mbm_index = mbm_index + 1
            if  mbm_index > mbm_last_info(cur_menu%) then
              mbm_menu_index = mbm_menu_index - 1
              if  mbm_menu_index = 0 then exit do
              cur_menu% = mbm_menus(mbm_menu_index)
              menu_text$ = menu_text$ + '}'
              mbm_index = mbm_cur_info(cur_menu%) + 1
            end if
            mbm_get_info
            select case mbm_cmd$
              case "$HEADING"       
                mbm_append_comma
                menu_text$ = menu_text$ + '%title "' + mbm_text1$ + '"'
              case "$TITLE"         
                mbm_append_comma
                menu_text$ = menu_text$ + '%heading "' + &
                  mbm_text1$ + '"'
              case "$ITEM"          : mbm_item
              case "$ITEM MENU"     : mbm_item_menu
              case "$SPLIT"         
                mbm_append_comma
                menu_text$ = menu_text$ + '%split'
              case else
            end select
          loop
          menu_add_exit
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U   A D D   E X I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !  This routine adds 'EXIT' as the last menu item in the main 
        !  pop-up menu.
        !
        ! Expected:
        !  menu_text$ is defined from MENU_BUILD_MENU.
        !  u_exit_text$ is defined from the main program
        ! 
        !
        ! Results:
        !  menu_text$ is complete
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_add_exit
          menu_text$ = menu_text$ + u_exit_text$
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M B M   G E T   I N F O
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mbm_get_info
          mbm_cmd$ = menu_info$(mbm_index, 2)
          mbm_prompt$ = menu_info$(mbm_index, 3)[1:9]
          mbm_text1$ = menu_info$(mbm_index, 4)
          mbm_text2$ = menu_info$(mbm_index, 5)
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M B M   I T E M   
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mbm_item
          mbm_append_comma
          if  valid(mbm_prompt$, 'integer')  then 
            z$ = lpad$(mbm_prompt$, 3)
            !z = 30
            z = 55
          else
            z$ = rpad$(mbm_prompt$, 10)
            !z = 24
            z = 48
          end if
          !+jls+ increase maximum length of menu item description
          menu_text$ = menu_text$ + '"' + z$ + mbm_text2$[1:z] + '"="' + &
            mbm_prompt$ + '"'
        end routine




        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M B M   I T E M   M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mbm_item_menu
          if  valid(mbm_prompt$, 'integer')  then 
            z$ = lpad$(mbm_prompt$, 3)
            !z = 30
            z = 55
          else
            z$ = rpad$(mbm_prompt$, 10)
            !z = 23
            z = 48
          end if
          !+jls+ increase maximum length of menu item description
          mbm_append_comma
          menu_text$ = menu_text$ + '"' + z$ + mbm_text2$[1:z] + &
            '"={%replace, %at 3, center'          
          mbm_cur_info(cur_menu%) = mbm_index
          for z = 1 to menu_menu_slot%
            if  menu_menu$(z, 1) = mbm_text1$ then exit for
          next z
          cur_menu% = z
          mbm_index = mbm_cur_info(cur_menu%)
          mbm_menu_index = mbm_menu_index + 1
          mbm_menus(mbm_menu_index) = cur_menu%
        end routine




        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M B M   A P P E N D   C O M M A
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine mbm_append_comma
          if  right$(menu_text$, 1) <> '{' then &
            menu_text$ = menu_text$ + ',' 
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! G E T   V A L I D   O P E R A T O R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        ! find out how to handle operator id
        !
        ! Expected:
        !       mc(operator_type) = "none", "username", "ask"
        !
        ! Result  :
        !       dispatch to correct routine
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine get_valid_operator
          oper_okay = false
          z$ = mc(operator_type)
          select case z$
            case "NONE"
              let oper_menu$ = default_mcl$
              oper_okay = true
              pop_up_menu = true
              ask system : user oper_id$
              action$ = 'finished'
            case "USERNAME"
              action$ = 'menu_get_username'
            case "ASK"
              action$ = 'menu_ask_operator'
          end select
          do until action$ = 'finished'
            dispatch action$
          loop
          if  fatal then exit routine
          if  oper_okay then exit routine
          repeat routine
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U   G E T   U S E R N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! get the user's username
        !
        ! Expected:
        !
        ! Result  :
        !       write
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_get_username
          ask system : user reply$
          menu_verify_operator
          if  have_operator  then
            action$ = 'menu_get_operator_info'
          else
            action$ = 'menu_ask_password'
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ A S K _ O P E R A T O R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask the user for their operator ID and verify.
        !
        !  u_*            = information to provide for the "ask" routine
        !  action$        = next action to perform
        !  have_operator% = operator flag
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_ask_operator
          prompt$   = "Operator ID"
          length = 15
          uc_response? = true
          validation$ = 'required'
          help$ = 'operator id'
          input_response
          if  _back  or  _exit  then
            action$ = "finished"
            fatal = true
            exit routine
          end if
          menu_verify_operator
          user_id$ = reply$
          if  have_operator  then
            action$ = "menu_get_operator_info"
          else
            action$ = "menu_ask_password"
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ V E R I F Y _ O P E R A T O R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Verify the entered operator ID in the OPERATOR structure.
        !
        ! EXPECTED:
        !       reply$ = user entered operator ID
        !
        ! RESULT:
        !       have_operator% = TRUE   if  found
        !                        FALSE  if  not found
        !
        !  have_operator% = operator flag
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_verify_operator
          set structure oper, field id : key reply$
          have_operator = _extracted = 1
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ G E T _ O P E R A T O R _ I N F O
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Get the information from the operator's OPERATOR record.
        !
        ! EXPECTED:
        !       we have a OPERATOR record
        !
        ! RESULT:
        !       oper_id$       = operator ID
        !       oper_name$     = operator name
        !       oper_password% = operator password
        !       oper_menu$     = operator menu
        !       oper_initials$ = operator initials
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_get_operator_info
          oper_id$       = oper(id)
          oper_name$     = oper(name)
          oper_password% = oper(password)
          oper_menu$     = oper(#system_name$ + '_menu')
          pop_up_menu = true
          if oper(#system_name$ + '_menu_type') = 'ITEM' then &
            pop_up_menu = false
          action$ = "menu_ask_password"
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ A S K _ P A S S W O R D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask the operator for their password.
        ! We ask for the password even if we didn't find the matching
        ! operator record.  If failure, go get next process to do.
        !
        ! EXPECTED:
        !        access_tries%  = number of access tries
        !        have_operator% = TRUE or FALSE flag
        !        oper_password% = password
        !        mc(password_type) = ask if password is to be asked
        !
        !        reply$       = user answer
        !        found          = TRUE or FALSE flag
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_ask_password
          access_tries% = access_tries% + 1
          if  mc(password_type) <> "ASK" then
            if  have_operator  then
              action$ = "finished"
              oper_okay = true
            else
              action$ = "menu_access_failure"
            end if
            exit routine
          end if
          z1$ = "<at 21, 1, noecho, ucase :Password? ^@@@@@@@@@@@@@@>"
          line input screen z1$ : reply$
          clear area 21, 1, 21,  80
          if  not have_operator  then
            found = false
          else
            gosub menu_encrypt_password
            found = u_int% = oper_password%
          end if
          if  found  then
            action$ = "finished"
            oper_okay = true
          else
            gosub menu_access_failure
          end if
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ A C C E S S _ F A I L U R E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Access attempt has failed.
        ! Three tries and you're out.
        !
        ! EXPECTED:
        !        access_tries% = number of access tries
        !        access_limit  = number of access tries allowed
        !        action$       = next action to perform
        !        fatal         = TRUE or FALSE flag
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_access_failure
          message error : "Operator access failure"
          if  access_tries% < access_limit  then
            if  mc(operator_type) = 'ASK' or mc(password_type) = 'ASK' then
              action$ = "finished"
              exit routine
            end if
          end if
          fatal = true
          action$ = "finished"
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ E N C R Y P T _ P A S S W O R D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Encrypt (hash) the entered password.
        !
        ! EXPECTED:
        !       u_str$ = password to encrypt
        !
        ! RESULT:
        !       u_int% = "encrypted" password
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_encrypt_password
          u_int% = convert(hash$(reply$)[1:4])
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M E N U _ F I N I S H
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Exit the program.
        !
        ! EXPECTED:
        !       fatal = true  if  couldn't process menu
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine menu_finish
          clear
          if  fatal  then
            u_str$ = "A B O R T E D"
          else
            u_str$ = "F I N I S H E D"
          end if
          gosub center
          print at 10, u_int% : u_str$
          u_str$ = date$(days(date$), 3)
          gosub center
          print at 13, u_int% : u_str$
          print at 19, 1: ''
          action$ = "exit"
        end routine



