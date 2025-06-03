90000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  PARSE_USING.INC -
        !    parses the screen file for maintain
        !    Creates a list of fields, in order, to substitute for the
        !      list created normally by Maintain.
        !
        !    Note that ANY arrays used by these (to mean, for example,
        !       display only) should be set by store_def_info in maintain.int
        !       to the default.  DO NOT rely upon Intouch's initialization.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  U S E   C O M M A N D   F I L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Read a file for the list of fields usable and other such commands.
        !  File format is:
        !     $field fieldname
        !     $option display
        !     $field anotherfieldname
        !     $valid Y,N
        !
        ! Expected: par_usefile$  = name of a file containing field names
        !       This file must exist - if it doesn't, the program halts.
        !           valid_options$ = list of valid options
        !           all_valid_options$ = list of all possible valid options
        ! Result:
        !           the file is read, the fields designated are loaded
        !       and modified as necessary, and valid_options$ is modified
        !       as directed.
        !          display_file$ has the name of the "display" file
        !       with background screen information; "" if not specified.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine use_command_file

        error = true
        gosub initialize_to_use_file
        if  _error  then exit routine
        when exception in
          do
            line input #field_ch: using_line$
            if trim$(using_line$) = ''  or  using_line$[1:1] = '!' then &
                  repeat do
            last_line$ = last_line$ + " " + using_line$
            if  right$(using_line$,1) = "&"  then
              last_line$[len(last_line$):len(last_line$)] = ""
              repeat do
            end if
            using_line$ = trim$(last_line$)
            last_line$ = ""
            gosub parse_file_line
            if  _error  then exit do
          loop
        use
          error = false
        end when
        close #field_ch

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  I N I T I A L I Z E   T O   U S E   F I L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Initialize variables to "use" a file; open the file.
        !  Result:
        !    in_field   = false (not currently in a field)
        !    nbr_fields = 0     (not using any fields YET)
        !    last_line$ = ""    (last line was empty)
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine initialize_to_use_file

        in_field = false        !  Not currently in a field
        z = pos(par_usefile$, "]")
        z1 = pos(par_usefile$, ".")
        if  z1 = 0  or  z1 < z  then  &
            par_usefile$ = par_usefile$ + ".MAIN"
        default_uppercase = false
        when exception in
          open #field_ch: name par_usefile$
        use
          message error: extext$ + ' for ' + par_usefile$
        end when
        nbr_fields = 0
        last_line$ = ""

        end routine



91000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   F I L E   L I N E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! This routine will parse a line of the file and break the data into
        ! seperate arrays
        ! If in_field = false then skip the option and valid lines
        !
        ! Expected:
        !          using_line$ = one line from the using file
        !          in_field = true if the field last mentioned
        !               can be handled by Maintain
        !          nbr_fields = the number of fields so far
        !           valid_options$ = list of valid options
        !           all_valid_options$ = list of all possible valid options
        !
        ! Result  :
        !    If the field is not found or an invalid command is entered, 
        !      it stops parsing and displays an error message.
        !          if $field then the field is set up
        !          if $option then field_option$() is loaded
        !          if $valid then  field_valid$() is loaded
        !          if  $field and the field cannot be handled by Maintain,
        !               in_field = false
        !          if $menu  then valid_options$ is modified.
        !      Store_def_info is used to store the definition information
        !          nbr_fields = the number of fields so far is updated
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_file_line

91020   z = min(pos(using_line$ + " "," "), pos(using_line$ + ":",":"))
        first$ = ucase$(trim$(using_line$[1:z-1]))
        the_rest$ = trim$(using_line$[z:999])
        if  the_rest$[1:1] = ":"  then &
          the_rest$ = trim$(the_rest$[2:999])
        second$ = element$(the_rest$,1," ")
        select case first$
          case '$DEFAULT'
            gosub parse_default_command
          case '$BEGIN_BACKGROUND'
            gosub parse_background
          case '$USE_BACKGROUND'
            gosub parse_screen_name
          case '$MODE'
            gosub parse_start_mode
          case '$OPEN'
            gosub parse_open
          case '$RELATE'
            gosub parse_relate
          case '$FIELD'
            show_error = true
            gosub parse_field
          case '$KEY'
            gosub parse_key_command
          case '$LABEL'
            show_error = false
            gosub parse_field
          case '$UPPERCASE'
            if  in_field  then  fld_uppercase(nbr_fields) = true
          case '$LOWERCASE'
            if  in_field  then  fld_uppercase(nbr_fields) = false
          case '$ROW'
            if  in_field  then  gosub parse_row_request
          case '$COLUMN'
            if  in_field  then  gosub parse_column_request
          case '$OPTION'
            if  in_field  then  gosub parse_option
          case '$VALID'
            if  in_field  then  gosub parse_validation_rules
          case '$ATTRIBUTE'
            if  in_field  then  gosub parse_attribute
          case '$MENU'
            gosub parse_menu_modification
          case '$PROMPT'
            if  in_field  then  gosub parse_new_prompt
          case '$DLEN', '$LENGTH'
            if  in_field  then  gosub parse_new_display_len
          case '$FORMAT','$MASK'
            if  in_field  then  gosub parse_new_mask
          case '$MSG','$MESSAGE','$HELP'
            if  in_field  then  gosub parse_new_help
          case else
            message error : "Unknown command:  " + using_line$
        end select                 

91099   end routine    



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   D E F A U L T   C O M M A N D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$DEFAULT" + something.  Change the default stuff
        !       accordingly (such as DEFAULT UPPERCASE)
        !  Expects:
        !       second$ has the second parameter
        !  Result:
        !       default_uppercase = true or false, depending on second$
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_default_command

        select case ucase$(second$)
          case "UPPERCASE":  default_uppercase = true
          case "LOWERCASE":  default_lowercase = false
          case else : message error : "Unknown default option:  " + second$
        end select

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   R O W   R E Q U E S T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$ROW", so store it.  Check integer; check legal value.
        !       if row not_displayed  then  set the column, too.
        !  Expects:
        !       second$ has the row number
        !       nbr_fields = the number of fields so far
        !       background_start = starting row allowed
        !       background_end   = last row allowed
        !  Result:
        !       req_row(nbr_fields) has the requested row (after checking)
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_row_request

        if  valid(second$, "INTEGER")  then
          z = val(second$)
          if  z = not_displayed  then  
            req_col(nbr_fields) = not_displayed
            exit routine
          end if
!++ debug djs ++ 09-01-89 if  z < 0  then
!++ debug djs ++ 09-01-89   message error : "Negative rows not allowed:  " + using_line$
          if  z < background_start  or  z > background_end  then
            message error : "Row is out of bounds:  " + using_line$
          else
            req_row(nbr_fields) = z
          end if
        else
          message error : "Integer row number expected:  " + using_line$
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   C O L U M N   R E Q U E S T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$COLUMN", so store it.  Check integer; check legal value.
        !       If it is already set to not_displayed then don't change it.
        !  Expects:
        !       second$ has the column number
        !       nbr_fields = the number of fields so far
        !  Result:
        !       req_col(nbr_fields) has the requested column (after checking)
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_column_request

        if  valid(second$, "INTEGER")  then
          z = val(second$)
          if  z < 0 then 
            message error : "Negative columns not allowed:  " + using_line$
          else
            if  req_col(nbr_fields) <> not_displayed  then &
              req_col(nbr_fields) = z
          end if
        else
          message error : "Integer column number expected:  " + using_line$
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   S C R E E N   N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$USE_BACKGROUND".  Store the rest of the line as the 
        !       screenname, trimmed and in upper case.
        !  This is not a subcommand of $FIELD, so reset in_field to false.
        !  Expects:
        !       the_rest$ has the rest of the input
        !       nbr_fields = the number of fields so far
        !  Result:
        !       The screen name is stored FOR THE NEXT FIELD.
        !       req_screen$(nbr_fields + 1) has the screen name.
        !       in_field = false
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_screen_name

        gosub unquote_the_rest
        z$ = ucase$(trim$(the_rest$))
        if  z$ = ""  then
          req_screen$(nbr_fields + 1) = new_screen$
          in_field = false
        else
          req_screen$(nbr_fields + 1) = ucase$(trim$(the_rest$))
          in_field = false
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! C H E C K   V A L I D   B A C K G R O U N D   N A M E S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check for the background names used if any.  Make sure that
        ! the background text was found for it
        !
        ! this routine is called from maintain.int
        !
        ! Expected:
        !       req_screen$(field_number) = background name or blank        
        !       screen_names$             = names of backgrounds found
        !
        ! Result  :
        !       _error = true if there were any problems
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine check_valid_background_names

        screen_names$[1:1] = ""
        for change_field = 1 to nbr_fields
          z$ = req_screen$(change_field)
          if  z$ = ""  or  z$ = new_screen$  then  iterate for
          if  match(screen_names$, z$) = 0  then 
            message error : "Unknown background name:  " + z$ + &
                " at field: " + str_name$(change_field)
            exit routine
          end if
        next change_field

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   S T A R T   M O D E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$MODE".  Store the rest of the line as the starting
        !       mode (width)
        !  This is not a subcommand of $FIELD, so reset in_field to false.
        !  Expects:
        !       the_rest$ has the rest of the input
        !  Result:
        !       start_mode$ = the rest of the input
        !       in_field = false
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_start_mode

        gosub unquote_the_rest
        if  match("WIDE,NARROW,132,80", the_rest$) = 0  then
          message error : "Unrecognized screen mode:  " + using_line$
          exit routine
        end if
        start_mode$ = the_rest$

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   B A C K G R O U N D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$BEGIN_BACKGROUND".  Store the rest of the line as the 
        !       background name
        !  Expects:
        !       the_rest$ has the rest of the input
        !  Result:
        !       display_filename$ = the display filename
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_background

        gosub unquote_the_rest
        screen_names$ = screen_names$ + "," + ucase$(the_rest$)
        gosub start_new_display_screen
        gosub read_background

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   N E W   P R O M P T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$PROMPT".  Store the rest of the line as the prompt.
        !  Expects:
        !       the_rest$ has the rest of the input
        !       nbr_fields = the number of fields so far
        !  Result:
        !       str_prompt$(nbr_fields) is changed to the desired prompt
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_new_prompt

        gosub unquote_the_rest
        if  str_prompt$(nbr_fields) = ""  and &
            fld_option$(nbr_fields) = "DISPLAY"  then &
            fld_option$(nbr_fields) = ""        ! Allow them to
                                                ! use fields which didn't
                                                ! have a prompt.
        str_prompt$(nbr_fields) = the_rest$

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   N E W   D I S P L A Y   L E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$DLEN" or "$LENGTH".  Store the new display length
        !  Expects:
        !       second$ has the desired display length
        !       nbr_fields = the number of fields so far
        !  Result:
        !       second$ is checked for positive integer value
        !       _error = true if there was a problem
        !       str_dlen(nbr_fields) is changed to the display length
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_new_display_len

        if  not(valid(second$,"INTEGER"))  then
          message error : "Numeric display length expected:  " + using_line$
          exit routine
        end if
        dlen = val(second$)
        if  dlen < 0  then
          message error : "Positive display length expected:  " + using_line$
          exit routine
        end if

        if  str_dlen(nbr_fields) > dlen  then & ! The mask is too long, cut it
          str_mask$(nbr_fields) = str_mask$(nbr_fields)[1:dlen]
                                ! Close enough.  If they don't like it,
                                !  they can create their own masks, which fit.
        str_dlen(nbr_fields) = dlen

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   N E W   M A S K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$MASK".  Store it.
        !  Expects:
        !       the_rest$ has the rest of the input
        !       nbr_fields = the number of fields so far
        !  Result:
        !       str_mask$(nbr_fields) is changed to the desired printmask
        !       str_dlen(nbr_fields) is changed to the appropriate display
        !               length
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_new_mask

        gosub unquote_the_rest
        str_mask$(nbr_fields) = the_rest$
        str_dlen(nbr_fields) = len(the_rest$) - elements(the_rest$,'~') + 1

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   N E W   H E L P
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$HELP" or "$MSG" or "$MESSAGE"; store the help text.
        !  Expects:
        !       the_rest$ has the message
        !  Result:
        !       str_help$(nbr_fields) is changed to the help message
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_new_help

        gosub unquote_the_rest
        str_help$(nbr_fields) = the_rest$

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   M E N U   M O D I F I C A T I O N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$MENU"; check to see what the next word is
        !    and set/change the menu options accordingly.
        !  Expects:
        !           valid_options$ = list of valid options
        !           all_valid_options$ = list of all possible valid options
        !  Result:
        !       valid_options$ is modified
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_menu_modification

        z = pos(the_rest$ + " ", " ") + 1
        items$ = ucase$(change$(the_rest$[z:999],"'" + '"'))
                ! Upper case, no quotes
        gosub check_ok_menu_items
        if  _error  then exit routine
        select case ucase$(second$)
          case 'ITEMS': valid_options$ = items$
          case 'REMOVE'
            z1$ = ""
            for t = 1 to elements(valid_options$)
              z$ = element$(valid_options$, t)
              if  match(items$, z$) = 0  then z1$ = z1$ + ',' + z$
            next t
            valid_options$ = z1$[2:len(z1$)]
          case else
            message error : "Unknown $MENU subcommand:  " + second$
        end select

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  C H E C K   O K   M E N U   I T E M S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Check that the items in items$ are truely valid.
        !  Expects:
        !    items$ = the list of items to check
        !    all_valid_options$ = list of all possible menu options.
        !  Result:
        !    Message is printed and the program stops parsing if any of them are
        !       invalid.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine check_ok_menu_items

        for t = 1 to elements(items$)
          z$ = element$(items$,t)
          if  match(all_valid_options$, z$) = 0  then
            message error : "Invalid menu item:  " + z$
            exit for
          end if
        next t

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  P A R S E   O P T I O N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  They said "$OPTION".  Check that it is valid and store it.
        !  Expects:
        !       second$ has the option they selected.
        !       in_field = false if the field is being skipped (do
        !         nothing)
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_option

        gosub unquote_the_rest
        second$ = ucase$(second$)
        if  match("DISPLAY,NOCHANGES,HIDDEN", second$) = 0  then
          message error : "Unknow $OPTION subcommand:  " + second$
        end if

        fld_option$(nbr_fields) = second$
        if  second$ = 'HIDDEN'  then  req_row(nbr_fields) = not_displayed
                ! don't want hidden fields taking up room on the screen
        end routine



93000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   V A L I D A T I O N   R U L E S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! This routine parses the $valid line of the text file
        !    Makes no checks on what the validation rules are.
        !    The validation rules can be found in validation.inc
        !
        ! Expected:
        !       using_line$ has the current line from the file
        !
        ! Result  :
        !       valid_element$(,) = array with valid line, parsed
        !       max_validation() = has number of validation elements
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine parse_validation_rules

93020   z$ = parse$(the_rest$)
        nbr_valids = max_validation(nbr_fields)
                                        ! Might have validation from 
                                        !   store_def_info
        parameters_needed = 0
        do
          rule$ = parse$
          if rule$ = "" then exit do
          if rule$ = ";"  then repeat do        ! Concatenate rules with a ";"
          nbr_valids = nbr_valids + 1
          valid_element$(nbr_fields,nbr_valids) = rule$
          if  parameters_needed = 0  then
            gosub check_need_parameters
          else
            gosub check_ok_parameter
          end if
          if  _error  then exit routine
        loop

        max_validation(nbr_fields) = nbr_valids

93099   end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  C H E C K   N E E D   P A R A M E T E R S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  See if this is something which needs parameters; if so,
        !    set the number of parameters expected and what they must be.
        !  Expects:
        !       rule$ = the validatin rule, upper case with no leading
        !               or trailing spaces.
        !  Result:
        !       parameters_needed = # parameters needed for this command
        !       param_integer = true if they must be integers
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine check_need_parameters

        select case rule$
          case 'NUMBER', 'INTEGER', 'DATE', 'REQUIRED'
            parameters_needed = 0
          case 'ALLOW', 'CONTAINS'
            parameters_needed = 1
            param_integer = false
          case 'RANGE', 'DIGITS'
            parameters_needed = 2
            param_integer = true
          case else
            message error : "Unknown validation rule:  " + rule$
        end select

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  C H E C K   O K   P A R A M E T E R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Check that the parameter is of the proper type; decrement
        !    the counter which says how many parameters are needed.
        !  Expects:
        !    parameters_needed = # parameters needed (>0)
        !    param_integer = true if the parameter must be an integer.
        !    rule$ = the parameter itself
        !    using_line$ = the entire line, for error display
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine check_ok_parameter

        parameters_needed = parameters_needed - 1
        if  param_integer  and  not(valid(rule$,"INTEGER"))  then &
          message error : "Integer parameter expected:  " + using_line$

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  R E A D   B A C K G R O U N D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Read the background screens
        !  Expects:
        !       
        !  Result:
        !       store_contents$(*,*) contains stored screen information
        !       screen_names$ is a list of screen names, uppercase and
        !         trimmed, in the order that they are stored.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine read_background

        when exception in
          do
            line input #field_ch: d_line$
            select case d_line$[1:1]
              case "!":
              case "$": gosub display_file_command
              case else
                if  in_screen and store_line <= background_end  then &
                  store_contents$(nbr_stored_screens, store_line) = d_line$
                store_line = store_line + 1
            end select
          loop while (not(problem) and not(eos))
        use
        end when
        set error off
        if problem then set error on

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  D I S P L A Y   F I L E   C O M M A N D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  In the display file, they used a command.  Interpret the
        !       command and set variables appropriately.
        !  Expects:
        !       d_line$ has the line of text; it begins with a "$"
        !  Result:
        !       in_screen, store_line, nbr_screens, and screen_names$
        !         are modified as necessary
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine display_file_command

        z = min(pos(d_line$ + " "," "), pos(d_line$ + ":",":"))
        first$ = ucase$(trim$(d_line$[1:z-1]))
        the_rest$ = trim$(d_line$[z:999])
        if  the_rest$[1:1] = ":"  then &
          the_rest$ = trim$(the_rest$[2:999])
        gosub unquote_the_rest

        select case first$
          case "$TITLE"
            if  in_screen  then &
                    store_title$(nbr_stored_screens) = the_rest$
          case "$END_BACKGROUND"
            in_screen = false
            eos = true
          case "$PRINT"
                if  in_screen and store_line <= background_end  then &
                  store_contents$(nbr_stored_screens, store_line) = d_line$
                store_line = store_line + 1
          case else
            message error : "Unknown display command:  " + d_line$
            problem = true
        end select

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  S T A R T   N E W   D I S P L A Y   S C R E E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Start a new background screen.  Increase the number of
        !       screens and reset the screen itself to blank.
        !  Expects:
        !       nbr_stored_screens = the number of screens stored so far
        !       background_start = the line number the background screen
        !         starts at
        !  Result:
        !       in_screen is true (you are now in a screen)
        !       store_line = background_start (store at the top of the screen)
        !       nbr_stored_screens is incremented
        !       screen_contents$(nbr_stored_screens,*) is set to blank.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine start_new_display_screen

        in_screen = true
        eos = false
        problem = false
        store_line = background_start
        nbr_stored_screens = nbr_stored_screens + 1
        for z = background_start to background_end
          store_contents$(nbr_stored_screens, z) = ""
        next z

        end routine
        


        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   O P E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Found an $open statement.  parse the file name(s) and open the
        ! structure.
        !
        ! Expected:
        !       the_rest$ = the using line without $open
        !
        ! Result  :
        !       the structure has been opened
        !       the name has been included into valid_structures$
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_open

        gosub unquote_the_rest
        the_rest$ = change$(the_rest$, ",", " ") !in case the seperate by ,
        the_rest$ = ucase$(edit$(the_rest$, 16%))
        structure_given = false
        datafile_given = false
        name_given = false
        par_structure$ = ""
        par_datafile$ = ""
        par_filename$ = ""
        t = 0
        do
          t = t + 1
          select case element$(the_rest$, t, ' ')
            case '': exit do
            case 'DATAFILE'
              if  datafile_given  then &
                 message error : "Cannot use two data files"
              datafile_given = true
              t = t + 1
              par_datafile$ = element$(the_rest$, t, ' ')
            case "STRUCTURE"
            case "NAME"
              if  name_given  then &
                 message error : "Cannot use two names"
              name_given = true
              t = t + 1
              par_filename$ = element$(the_rest$, t, ' ')
            case else
              if  structure_given  then &
                 message error : "Cannot use two structures"
              structure_given = true
              par_structure$ = element$(the_rest$, t, ' ')
          end select
          if  _error  then exit do
        loop        
        gosub open_structure

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! O P E N   S T R U C T U R E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Actually open a structure. Only the main structure is open for
        ! modification
        !
        ! This routine is longer than 22 lines due to the select case stmt
        !
        ! Expected:
        !       par_structure$ = name of structure to open
        !       par_datafile$  = name of datafile if any
        !       par_filename$  = synonym name if any
        !       open_structures = number of structures already open
        !       valid_structures$ = names of already open structure
        !
        ! Result  :
        !       open_structures is incremented
        !       valid_structures$ is updated with the name of the new structure
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine open_structure

        gosub open_structure_init
        if  _error  then  exit routine
        select case open_structures
        case 1
          when exception in
            if  datafile_given  then
              open structure str1:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str1: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + par_structure$
          end when
          if  _error  then  exit routine
          ask structure str1 : id z$
          str_id$(1) = z$
          set structure struc : id z$
          main_structure$ = par_structure$
        case 2
          when exception in
            if  datafile_given  then
              open structure str2:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str2: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str2 : id z$
          str_id$(2) = z$
        case 3
          when exception in
            if  datafile_given  then
              open structure str3:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str3: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str3 : id z$
          str_id$(3) = z$
        case 4
          when exception in
            if  datafile_given  then
              open structure str4:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str4: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str4 : id z$
          str_id$(4) = z$
        case 5
          when exception in
            if  datafile_given  then
              open structure str5:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str5: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str5 : id z$
          str_id$(5) = z$
        case 6
          when exception in
            if  datafile_given  then
              open structure str6:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str6: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str6 : id z$
          str_id$(6) = z$
        case 7
          when exception in
            if  datafile_given  then
              open structure str7:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str7: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str7 : id z$
          str_id$(7) = z$
        case 8
          when exception in
            if  datafile_given  then
              open structure str8:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str8: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str8 : id z$
          str_id$(8) = z$
        case 9
          when exception in
            if  datafile_given  then
              open structure str9:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str9: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str9 : id z$
          str_id$(9) = z$
        case 10
          when exception in
            if  datafile_given  then
              open structure str10:name par_structure$, access outin, &
                 datafile par_datafile$
            else
              open structure str10: name par_structure$, access outin
            end if
          use
            message error: extext$ + '  for  ' + the_rest$
          end when
          if  _error  then  exit  routine
          ask structure str10 : id z$
          str_id$(10) = z$
        end select
        valid_structures$ = valid_structures$ + "," + open_name$
        if  valid_structures$[1:1] = ","  then  &
                valid_structures$ = valid_structures$[2:999]

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! O P E N   S T R U C T U R E   I N I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! initialize for open structure
        ! get structure name without directory or logicals
        ! make sure it hasn't been opened yet
        ! make sure not more than max_structures structures are opened
        !
        ! Expected:
        !       par_structure$ is the name of the structure to be opened
        !       par_filename$ is the synonym name of the structure if given
        !       valid_structures$ contains the names of already opened str.
        !       open_structures = number of opened structures
        !       max_structures  = max number of structures allowed
        !
        ! Result  :
        !       open_structures is incremented
        !       open_name$ = name of structure to be opened without dir.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine open_structure_init

        if  par_filename$ <> ""  then
          open_name$ = par_filename$
        else
          z = pos(par_structure$, ']')
          if  z = 0  then
            z = pos(par_structure$, ':')
          end if
          open_name$ = par_structure$[z+1:999]
        end if
        if  match(valid_structures$, open_name$) <> 0  then
          message error : "Structure " + open_name$ + " has already been opened"
          exit routine
        end if
        if  open_structures = max_structures  then
          message error : "Attempt to open more than 10 structures"
          error = true
          exit routine
        end if
        open_structures = open_structures + 1

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   R E L A T E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Parse a relate statement.  Will establish a relationship between
        ! two already opened structures
        !
        ! Expected:
        !       the_rest$ = the relationship expression
        !
        ! Result  :
        !       relate_str(x,x) = the two structures to be related
        !       relate_fld$(,)  = the two fields to relate
        !       nbr_relates     is incremented
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_relate

        gosub parse_relate_setup
        if  _error  then  exit routine
        for  rel = 1 to rel_elements
          the_field$ = element$(the_rest$, rel, ' ')
          if  the_field$ = 'TO'  then  iterate for
          gosub breakout_structure
          if  _error  then  exit routine
          gosub parse_relate_field_check
          if  _error  then  exit routine
          rel_phrase = rel_phrase + 1
          relate_str(nbr_relates, rel_phrase) = u_str_number
          relate_fld$(nbr_relates, rel_phrase) = u_field$
        next rel
        gosub parse_relate_finish

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   R E L A T E   S E T U P
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! initialize for parsing the relate expression
        !
        ! Expected:
        !       the_rest$ = relate expression
        !       nbr_relates = number of relates already established
        !       max_relates = maximum number of relates allowed
        !
        ! Result  :
        !       nbr_relates is incremented
        !       rel_phrase = 0
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_relate_setup

        gosub unquote_the_rest
        the_rest$ = ucase$(edit$(the_rest$, 16%))
        rel_elements = elements(the_rest$, " ")
        if  nbr_relates = max_relates  then
          message error : "Maximum number of relates has been exceeded: " + &
                "maximum is " + str$(max_relates)
          error = true
          exit routine
        end if
        nbr_relates = nbr_relates + 1
        rel_phrase = 0

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   R E L A T E   F I E L D   C H E C K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check to make sure that the field is part of the structure
        !
        ! Expected:
        !       u_str_number = structure number to check
        !       u_field$     = field name to check
        !
        ! Result  :
        !       _error is true if there is a problem
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_relate_field_check

        set structure struc : id str_id$(u_str_number)
        ask structure struc, field #u_field$ : number z
        if  z = 0  then
          message error : "Relate invalid: Field " + &
                u_field$ + " is not a field in structure: " + &
                element$(valid_structures$, u_str_number, ',')
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   R E L A T E   F I N I S H
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check to see that the relate expression was okay.  Check that
        ! relate to field is a key in the related structure.
        ! check that all elements were defined
        !
        ! Expected:
        !       relate_str(,)  = the two structures to relate
        !       relate_fld$(,) = the two fields to relate
        !       nbr_relates    = the relate index
        !       the_rest$      = the relate expression
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_relate_finish

        for z = 1 to 2
          if  relate_str(nbr_relates, z) = 0  or  &
              relate_fld$(nbr_relates, z) = ""  then
            message error : "Invalid relate expression: " + the_rest$
            error = true
            exit routine
          end if
        next z
        gosub relate_key_field
        if  _error  then
          error = true
          exit routine
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! R E L A T E   K E Y   F I E L D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check to see if the field given is a key
        !
        ! Expected:
        !       relate_str(,2)  = number of the structure
        !       relate_fld$(,2) = name of the field
        !       nbr_relates     = index to relate arrays
        !
        ! Result  :
        !       _error is true if the field is not a key
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine relate_key_field

        set structure struc : id str_id$(relate_str(nbr_relates, 2))
        ask structure struc, field #relate_fld$(nbr_relates, 2) : keyed z
        if  z  then  exit routine
        message error : "related structure/field is not a keyed field: " + &
                the_rest$
        error = true

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! F I N I S H   R E L A T E   S E T U P
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! finish processing the relate arrays
        ! check for multiple relates to one structure
        ! store the related structure index for the field if it is the key
        ! to a relate
        !
        ! Expected:
        !       nbr_relates = number of relations defined
        !       nbr_fields  = number of fields defined
        !       relate_str() = relate structure information
        !       relate_fld$()   = relation information
        !       str_name$() = name of fields
        !
        ! Result  :
        !       fld_relate_key$() is set to relation indexs that the field
        !                         triggers
        !       fld_relate_in()   is set to relation index that finds the
        !                         record for this field
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine finish_relate_setup

        gosub finish_relate_arrays
        gosub finish_relate_check_multiple
        gosub finish_relate_check_structures

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! F I N I S H   R E L A T E   A R R A Y S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! store the related structure index for the field if it is the key
        ! to a relate
        !
        ! Expected:
        !       nbr_relates = number of relations defined
        !       nbr_fields  = number of fields defined
        !       relate_str() = relate structure information
        !       relate_fld$()   = relation information
        !       str_name$() = name of fields
        !
        ! Result  :
        !       fld_relate_key$() is set to relation indexs that the field
        !                         triggers
        !       fld_relate_in()   is set to relation index that finds the
        !                         record for this field
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine finish_relate_arrays

        for z = 1 to nbr_relates
          z2 = relate_str(z, 1)
          z$ = relate_fld$(z, 1)
          for z1 = 1 to nbr_fields
            if  str_name$(z1) = z$  and  &
                fld_structure(z1) = z2  then
              fld_relate_key$(z1) = fld_relate_key$(z1) + " " + str$(z)
              if  fld_relate_key$(z1)[1:1] = " "  then &
                fld_relate_key$(z1) = fld_relate_key$(z1)[2:999]
            end if
            if  fld_structure(z1) = relate_str(z, 2)  then
              fld_relate_in(z1) = z
            end if
          next z1
        next z

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! F I N I S H   R E L A T E   C H E C K   M U L T I P L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check to see if a structure is the target of more than one relate
        !
        ! Expected:
        !       max_relates = number of relates established
        !       relate_str(x, 2) = target structure of a relate
        !
        ! Result  :
        !       _error = true if multiple relates found
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine finish_relate_check_multiple

        for z = 1 to nbr_relates
          z1 = relate_str(z, 2)
          for  z2 = 1 to nbr_relates
            if  z2 = z  then  iterate for
            if  relate_str(z2, 2) = z1  then
              message error : "Structure: " + &
                element$(valid_structures$, z1, ',') + &
                ' is the target of multiple relates'
              exit for
            end if
          next z2
        next z

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! F I N I S H   R E L A T E   C H E C K   S T R U C T U R E S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check to make sure that each of the supplemental structures
        ! has an associated relate
        !
        ! Expected:
        !       valid_structures$ = names of all structures open
        !       relate_str(x, 2)  = structure number of related structure
        !
        ! Result  :
        !       _error = true if a structure hasn't been related
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine finish_relate_check_structures

        z = elements(valid_structures$, ',')
        for z1 = 2 to z
          str_ok = false
          for z2 = 1 to nbr_relates
            if  z1 <> relate_str(z2, 2)  then  iterate for
            str_ok = true
            exit for
          next z2
          if  str_ok  then  iterate for
          z$ = element$(valid_structures$, z1, ',')
          message error : "Structure " + z$ + " has not been related"
        next z1

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   F I E L D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! parse the field name.  Make sure that it is a valid field (in
        ! one structure only or else that the structure was explicitly
        ! given) and that the data type is one we can handle
        !
        ! Expected:
        !       the_rest$ = the using line being processed
        !
        ! Result  :
        !       u_str_number = number of the structure the field is in
        !       u_field$     = name of the field
        !       fld          = number of the field
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_field

        gosub unquote_the_rest
        the_field$ = ucase$(the_rest$)
        gosub breakout_structure
        if  _error  then  exit routine
        gosub process_field_name
        if  _error  then  exit routine
        set structure struc : id str_id$(u_str_number)
        gosub get_def_info
        if  data_type$ = "UN"  or &
            (data_type$ = "DS" and struc_len = 8)  then
          in_field = false      !  Can't handle the field
          exit routine          !    if it is an unsupported data type  
        end if                  !  (unknown or VMS date stamp)
        in_field = true
        nbr_fields = nbr_fields + 1
        fld_option$(nbr_fields) = "" ! blank it out so it isn't left over
        gosub store_def_info
        fld_uppercase(nbr_fields) = default_uppercase
        fld_structure(nbr_fields) = u_str_number
        if  str_prompt$(nbr_fields) = ""  or  &
            fld_structure(nbr_fields) > 1  then 
          fld_option$(nbr_fields) = "DISPLAY"
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! B R E A K O U T   S T R U C T U R E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Get the structure and field broken apart.  Expecting either 
        ! name or str(name).  If the structure is explicitly given then
        ! verify that it is a valid structure name
        !
        ! Expected:
        !       the_field$ contains the using line data
        !       valid_structures$ is a list of valid structure names
        !
        ! Result  :
        !       u_structure$ = name of the structure given
        !       u_field$     = name of the field
        !       u_str_number = the number of the structure
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine breakout_structure

        z = pos(the_field$, '(')
        if  z = 0  then
          u_structure$ = ""
          u_field$ = the_field$
          u_str_number = 0
          exit routine
        end if
        z$ = edit$(change$(the_field$, "()", " "), 16)
        u_structure$ = element$(z$, 1, " ")
        u_field$ = element$(z$, 2, " ")
        u_str_number = match(valid_structures$, u_structure$)
        if  u_str_number = 0  then
          message error : using_line$ + " references an unknown structure"
          exit routine
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R O C E S S   F I E L D   N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! make sure that the field name is valid
        ! if a structure was specified then make sure the field is in it
        ! if not then search all of the structures.  Also make sure that
        ! the field isn't ambiguous if a structure wasn't specified
        !
        ! Expected:
        !       u_str_number = index to structure array that the field is in
        !
        ! Result  :
        !       u_str_number = actual structure index for this field
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine process_field_name

        u_ambiguous = 0
        if  u_str_number > 0  then
          gosub check_field
          if  not(field_found)  then
            gosub bad_field
          end if
          exit routine
        end if
        for u_str_number = 1 to open_structures
          gosub check_field
          if  field_found  then  
            u_ambiguous = u_ambiguous + 1
            str_save = u_str_number
            fld_save = fld
          end if
        next u_str_number
        if  u_ambiguous <> 1  then
          gosub bad_field
          exit routine
        end if
        u_str_number = str_save
        fld = fld_save

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! C H E C K   F I E L D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check a structure to see if a field is a field in the structure
        !
        ! Expected:
        !       str_id$() = array of structure ids
        !       u_str_number = index to str_id array for the struc to check
        !       u_field$     = field name to check
        !
        ! Result  :
        !       field_found  = true if field is in the structure
        !       fld          = field number of the field
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine check_field

        field_found = false
        set structure struc : id str_id$(u_str_number)
        ask structure struc, field #u_field$ : number fld
        if  fld > 0  then  field_found = true
        
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! B A D   F I E L D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! a bad field was found ( not in a structure or was ambiguous)
        ! print a message
        !
        ! Expected:
        !       u_ambiguous = count of the number of structures the field
        !                     was found in
        !
        ! Result  :
        !       in_field = false
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine bad_field

        if  u_ambiguous = 0  then
          z$ = "Field " + u_field$ + " was not found"
        else
          z$ = "Field " + u_field$ + " is ambiguous"
        end if
        if  show_error  then &
          message error : z$
        in_field = false

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   K E Y   C O M M A N D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !       user wants to select the keys that can be accessed
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_key_command

        gosub unquote_the_rest
        the_field$ = ucase$(the_rest$)
        gosub breakout_structure
        if  _error  then  exit routine
        gosub process_field_name
        if  _error  then  exit routine
        if  u_str_number <> 1 then
          message error : using_line$ + &
                ' - key fields must be from the main structure'
          exit routine
        end if
        ask structure str1, field #u_field$ : keyed z, &
                description f_desc$
        if  not z then
          message error : using_line$ + ' - is not a key field'
          exit routine
        end if
        nbr_keys = nbr_keys + 1
        if  f_desc$ = ''  then  f_desc$ = u_field$
        keys$(nbr_keys) = u_field$ + '|' + f_desc$
        user_defined_keys = true

        set structure struc : id str_id$(u_str_number)
        
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P A R S E   A T T R I B U T E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! parse the attribute command.  Break out the selected attributes and
        ! set a bit in the str_attribute() array.
        !
        ! Expected:
        !       the_rest$ = the display attributes desired
        !       nbr_field = index into field arrays
        !
        ! Result  :
        !       str_attribute$() will be loaded with the requested
        !                       attributes
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine parse_attribute

        gosub unquote_the_rest
        the_rest$ = ucase$(edit$(the_rest$, 16%))
        str_attribute$(nbr_fields) = the_rest$

        end routine




        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  U N Q U O T E   T H E   R E S T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  If the_rest$ has quotes around it, remove them.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine unquote_the_rest

        z$ = trim$(the_rest$)
        if  (left$(z$,1) = right$(z$,1))  and &
            (z$[1:1] = "'" or z$[1:1] = '"')  then &
          the_rest$ = z$[2:len(z$) - 1]

        end routine



