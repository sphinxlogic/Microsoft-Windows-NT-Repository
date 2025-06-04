        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Program       : UTILITY_ROUTINES.INC
        ! Package       : menu system
        ! Author        : Allen Roske
        ! Date          : May 1988
        ! Purpose       : commonly used utility routines
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! ask                   get user response
        ! askyn                 ask a yes/no question
        ! screen_frame          paint screen frame
        ! get_date              get many versions of a date in yyMMDD
        ! get_time              get time in HH:MM AM or HH:MM PM format
        ! ymd_to_mdy            change date from yyMMDD to MMDDyy
        ! mdy_to_ymd            change date from MMDDyy to yyMMDD
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! A S K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Prompt the user and get their reply.  Perform the indicated
        ! validations.
        !
        ! EXPECTED:
        !        u_prompt$   = prompt text
        !        u_default$  = default response
        !        u_required% = response required flag
        !        u_maxlen%   = maximum length
        !        u_numeric%  = numeric entry only
        !        u_allow$    = allowed responses (separated by commas)
        !        u_range$    = allowed range (ex: "1 TO 99")
        !        u_date%     = verify date
        !        u_uconly%   = convert to upper case flag
        !        u_help$     = the name of a help topic in the
        !                      HELP structure.
        !        the_width   = screen width - set by screen_frame
        ! RETURNED:
        !        u_reply$ = what the user entered
        !        u_rel    = numeric version  if  u_numeric% = true
        !
        ! MENTIONED:
        !        help_topic$ = the name of a help topic in the
        !                      HELP structure.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine ask

        do
          clear area 21, 1, 21, the_width
          u_reply$ = ""
          u_rel    = real(0)
          prompt$  = u_prompt$ + "? "
          line input &
            prompt prompt$, default u_default$, &
            length u_maxlen%, at 21, 1 : u_reply$
          if  (_back)  or  (_exit)  then  exit do
          if  (_help)  then
            help_topic$ = u_help$
            gosub help
            repeat do
          end if
          gosub do_ask_checks
          if  (error)  then  repeat do
        end do
        gosub do_ask_reset

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _ C H E C K S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Perform the various validations on the user's response
        !
        ! EXPECTED:
        !        u_required% = response required flag
        !        u_maxlen%   = maximum input length
        !        u_numeric%  = numeric entry only flag
        !        u_allow$    = allowed responses (separated by commas)
        !        u_range$    = range check, i.e. "1 TO 5"
        !        u_date%     = verify date
        !        u_uconly%   = convert to upper case flag
        !
        ! RESULT:
        !        error = true  if  any problem
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine do_ask_checks

        do
          gosub do_ask_check_required
          if  (error)  then  exit do

          gosub do_ask_check_numeric
          if  (error)  then  exit do

          gosub do_ask_check_allow
          if  (error)  then  exit do

          gosub do_ask_check_range
          if  (error)  then  exit do

          gosub do_ask_check_date
          if  (error)  then  exit do

          gosub do_ask_check_uconly
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _ C H E C K _ R E Q U I R E D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check that we have a response  if  u_required% = true
        !
        ! EXPECTED:
        !        u_reply$ = what they entered
        !
        ! RETURNED:
        !        error = true  if  no response given
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine do_ask_check_required

        error = false

        if  (u_required%)  then
          z1$ = edit$(u_reply$, 2)
                ! discard all spaces and tabs

          if  z1$ = ""  then
            message error : "Response required"
            error = true
          end if
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _  C H E C K _  N U M E R I C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check that the response is a valid number
        !
        ! EXPECTED:
        !        u_reply$ = what the user entered
        !
        ! RESULT:
        !        error = true  if  response not numeric
        !        u_rel = numeric value of response  if  numeric
        !                                                                 
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                                                                        
        routine do_ask_check_numeric

        error = false

        if  (u_numeric%)  then
          u_rel = 0.

          if  valid(u_reply$, "number")  then
            u_rel = val(u_reply$)
          else
            message error : "Numeric entry expected"
            error = true
          end if
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _ C H E C K _ A L L O W
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check that the reply is among the allowed responses
        !
        ! EXPECTED:
        !        u_reply$ = what the user entered
        !        u_allow$ = list of allowed responses separated by
        !                   commas
        !
        ! RETURNED:
        !        error = true  if  reply not among allowed responses
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine do_ask_check_allow

        error = false

        do
          if  u_allow$ <> ""  then
            z1$ = ucase$(trim$(u_reply$))

            if  (z1$ = "")  and  (u_required% = false)  then
              exit do
            end if
                ! don't check  if  null string  and  response not 
                ! required

            if  match(u_allow$, z1$) = 0  then
              message error : 'Valid responses: "' + u_allow$ + '"'
              error = true
            end if
          end if
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _ C H E C K _ R A N G E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check response is within a specific range
        !
        ! EXPECTED:
        !        u_reply$  = what the user entered
        !        u_range$ = range string (ex: "1 TO 100")
        !
        ! RETURNED:
        !        u_rel = numeric version of what they entered (if OK)
        !        error = true  if  response not within range
        !
        ! Exceeds 22 lines to complete range checking in one place.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine do_ask_check_range

        error = false
        do
          if  u_range$ <> ""  then
            u_rel = 0.
            z1$ = ucase$(trim$(u_reply$))
            if  (z1$ = "")  and  (u_required% = false)  then
              exit do
            end if
                ! don't check  if  null string and response not 
                !required

            lbound$ = element$(u_range$, 1, " ")
            ubound$ = element$(u_range$, 3, " ")
            lbound  = val(lbound$)
            ubound  = val(ubound$)
            if  valid(z1$, "number")  then
              u_rel = val(z1$)
            else
              message error : "Numeric entry expected"
              error = true
              exit do
            end if
            if  (u_rel < lbound)  or  (u_rel > ubound)  then
              message error : "Enter a number between " + &
                              lbound$ + " and " + ubound$
              error = true
            end if
          end if
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _ C H E C K _ D A T E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check response is a valid date
        !
        ! EXPECTED:
        !        u_reply$  = what the user entered
        !
        ! RETURNED:
        !        error = true  if  response not a valid date
        !
        !        u_day          = "julian" of date
        !        u_day$         = day of week (Monday, Tuesday, etc.)
        !        u_yymmdd$    = yymmdd version of entered date
        !        u_mmddyy$    = mmddyy version of entered date
        !        u_ddmonyy$   = dd-mon-yy version of entered date
        !        u_monthddyy$ = month dd, yy version of entered date
        !                         (padded to 18 characters)
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine do_ask_check_date

        error = false
        do
          if  (u_date%)  then
            if  (trim$(u_reply$) = "")  and  (u_required% = false)  then
              exit do
            end if
                ! don't check  if  null string and response not required

            ! verify the date
            u_mmddyy$ = u_reply$
            gosub mdy_to_ymd

            if  (valid(u_yymmdd$, "date"))  then
              u_str$ = u_yymmdd$
!              gosub get_date
            else
              message error : "Invalid date: " + u_reply$
              error = true
            end if
          end if
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _ C H E C K _ U C O N L Y
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Convert u_reply$ to upper case  if  u_uconly% = true
        !
        ! EXPECTED:
        !        u_reply$ = what the user entered
        !
        ! RESULT:
        !        u_reply$ = changed to all upper case
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine do_ask_check_uconly

        if  (u_uconly%)  then
          u_reply$ = ucase$(u_reply$)
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! D O _ A S K _ R E S E T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Reset all the variables used by the ask routine
        !
        ! EXPECTED:
        !        u_prompt$   = prompt text
        !        u_default$  = default response
        !        u_required% = response required flag
        !        u_maxlen%   = maximum length
        !        u_numeric%  = numeric entry only
        !        u_allow$    = allowed responses (separated by commas)
        !        u_range$    = allowed range (ex: "1 TO 99")
        !        u_date%     = verify date
        !        u_uconly%   = convert to upper case flag
        !        u_help$     = help topic in HELP structure
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine do_ask_reset

        u_prompt$   = ""
        u_default$  = ""
        u_required% = false
        u_maxlen%   = 0%
        u_numeric%  = false
        u_allow$    = ""
        u_range$    = ""
        u_date%     = false
        u_uconly%   = false
        u_help$     = ""

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! A S K Y N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask a YES/NO question.
        !
        ! EXPECTED:
        !        u_prompt$ = prompt text
        !
        ! RESULT:
        !        u_reply$ = "Y" or "N"
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine askyn

        prompt_sav$ = u_prompt$

        do
          u_prompt$   = prompt_sav$ + " (Y/N)"
          u_default$  = "No"
          u_required% = true
          u_maxlen%   = 4%
          u_uconly%   = true

          gosub ask

          if  (_exit)  or  (_back)  then  exit do

          if  valid(u_reply$, "yes/no")  then
            u_reply$ = u_reply$[1:1]
          else
            message error : "YES or NO"
            repeat do
          end if
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S C R E E N _ F R A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Paint the screen with the top and bottom frames.
        !
        ! EXPECTED:
        !        u_str$ = header text
        !
        ! Results:
        !        the_width   = screen width - set by screen_frame
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine screen_frame

        ask margin the_width
        if  save_bottom_frame  then
          save_bottom_frame = false
          clear area 1, 1, 21, the_width
        else
          clear
        end if

        z3$ = space$(the_width)        
        sf_head$ = u_str$

        z$ = system_name$ + " " + system_version$
        z = len(z$)
        z3$[2:2 + z - 1] = z$
        u_str$ = date$
        gosub get_date
        z = len(u_ddmonyy$)
        z = the_width - z
        z3$[z:the_width - 1] = u_ddmonyy$

        print reverse, at 1, 1 : z3$;
        sf_head$ = " " + sf_head$ + " "
        z = (the_width/2) - (len(sf_head$)/2)
        print at 1, z, bold, reverse : sf_head$;
        z3$ = "EXIT = Exit" + &
              space$(the_width - 32) + &
              "\ = Back  HELP = Help"

        print reverse, at 24, 1 : z3$;
        if  bottom_frame_text$ = ''  then  exit routine
        z = the_width - (len(bottom_frame_text$)/2)
        print at 24, z, bold, reverse : bottom_frame_text$;

        end routine


                                                                         
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! G E T _ D A T E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! EXPECTED:
        !       u_str$ = yyMMDD version of date to "elaborate"
        !
        ! RESULT:
        !       u_day          = "julian" of entered date
        !       u_day$         = day of week (Monday, Tuesday, etc.)
        !       u_yymmdd$    = yymmdd version of entered date
        !       u_mmddyy$    = mmddyy version of entered date
        !       u_ddmonyy$   = dd-mon-yy version of entered date
        !       u_monthddyy$ = month dd, yy version of entered date
        !                        (padded to 18 characters)
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine get_date

        if  trim$(u_str$) = ""  then
          u_day          = 0.
          u_day$         = ""
          u_yymmdd$    = ""
          u_mmddyy$    = ""
          u_ddmonyy$   = ""
          u_monthddyy$ = space$(18)
        else               
          u_day          = days(u_str$)

          u_day$         = day$(u_day)
          u_yymmdd$    = date$(u_day, 0)
          u_mmddyy$    = date$(u_day, 1)
          u_ddmonyy$   = date$(u_day, 3)
          u_monthddyy$ = rpad$(date$(u_day, 4), 18)
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! G E T _  T I M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Convert a 24-hour time string into a 12-hour time string.
        !
        ! EXPECTED:
        !       u_str$ = HH:MM (24 hr) version of time to "elaborate"
        !
        ! RESULT: 
        !       u_time$ = HH:MM AM  or  HH:MM PM
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine get_time

        z1$ = u_str$[1:2]
        z2$ = u_str$[4:5]

        select case z1$
        case is < "12"
          u_time$ = z1$ + ":" + z2$ + " AM"
        case is = "12"
          u_time$ = z1$ + ":" + z2$ + " PM"
        case is > "12"
          z1% = val(z1$)
          z1% = z1% - 12
          u_time$ = lpad$(str$(z1%), 2, "0") + ":" + z2$ + " PM"
        end select

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Y M D _ T O _ M D Y
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Change a date in yyMMDD format to MMDDyy format
        !
        ! EXPECTED:
        !        u_yymmdd$ = date in yyMMDD format if 8 char date
        !
        ! RETURNED:
        !        u_mmddyy$ = date in MMDDyy format
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine ymd_to_mdy

        if  len(u_yymmdd$) = 6 then
          u_mmddyy$ = u_yymmdd$[3:6] + u_yymmdd$[1:2]
        else
          u_mmddyy$ = u_yymmdd$[5:8] + u_yymmdd$[1:4]
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M D Y _ T O _ Y M D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Change a date in MMDDyy format to yyMMDD format
        !
        ! EXPECTED:
        !        u_mmddyy$ = date in MMDDyy format
        !
        ! RETURNED:
        !        u_yymmdd$ = date in yyMMDD format
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine mdy_to_ymd

        if  len(u_mmddyy$) = 6 then
          u_yymmdd$ = u_mmddyy$[5:6] + u_mmddyy$[1:4]
        else
          u_yymmdd$ = u_mmddyy$[5:8] + u_mmddyy$[1:4]
        end if

        end routine


        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! E R R _ P A U S E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Print out an error message, ring the bell,
        ! and pause while they read it
        !
        ! EXPECTED:
        !        u_str$ = error message to print out
        !        the_width   = screen width - set by screen_frame
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine err_pause

        clear area 22, 1, 22, the_width

        gosub center
                ! given u_str$, returns u_int%

        print bold, at 22, u_int% : u_str$; &
          chr$(7);

        delay

        clear area 22, 1, 22, the_width

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! M S G _ P A U S E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Print out a message and pause while they read it
        !
        ! EXPECTED:
        !        u_str$ = message to print out
        !        the_width   = screen width - set by screen_frame
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine msg_pause

        clear area 22, 1, 22, the_width

        gosub center
                ! given u_str$, returns u_int%

        print bold, at 22, u_int% : u_str$;

        delay

        clear area 22, 1, 22, the_width

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! C E N T E R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Returns the screen column to use in order to center u_str$
        ! on an screen
        !
        ! EXPECTED:
        !        u_str$ = string to be centered
        !        the_width = width of screen - set by screen_frame
        !
        ! RESULT:
        !        u_int% = starting column to print u_str$ on
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine center

        u_int% = (the_width/2) - (len(u_str$) / 2)

        end routine


        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! I N P U T   R E S P O N S E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !   Ask the expected prompt.
        !   Allows, through various flags (see do_ask_checks), you to
        !   automatically check for stuff.  Because these flags are
        !   automatically reset, you don't need to worry about them
        !   unless you WANT it to check for something.
        !   Routine is over 22 lines.
        !
        ! Expected:
        !   help$              help topic
        !   default$           default response
        !   uc_response?        upper case flag
        !   length             max input length
        !   prompt$            prompt text
        !   validation$        validation rules
        !   response_message$  message to display
        !
        ! Locals:
        !
        ! Results:
        !   finished_entry?     flag signifying entry is finished
        !   reply$  = user's reply
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine input_response
          init_ask_vars
          clear area 21, 1, 21, 80
          do
            if  ask_message$ <> '' then message ask_message$
            line input  prompt ask_prompt$, default ask_default$, &
                length ask_length, at 21, 1 : reply$
            clear area 21, 1, 21, 80
            if  _exit or _back  then exit do
            if  _help  then
              if  tmp_help_topic$ = '' then
                message error : 'No help is available'
              else
                help_topic$ = tmp_help_topic$
!                gosub h_help
              end if
              repeat do
            end if
            if not valid(reply$, ask_validation$, true) then repeat do
          end do
          reply$ = trim$(reply$)
          if  ask_uc_response? then reply$ = ucase$(reply$)
          if  reply$ = 'NONE' then reply$ = 'none' ! don't want this uppercased
          if  _terminator = 'PF4' then finished_entry? = true
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! I N I T   A S K   V A R S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !   initialize the variables that ask uses.  Reset the variables
        !   that the programmer passed so that they don't get used next time
        !   
        ! Expected:
        !   response_message$  message to display
        !   help$              help topic
        !   default$           default response
        !   uc_response?        upper case flag
        !   length             max input length
        !   prompt$            prompt text
        !   validation$        validation rules
        !
        ! Locals:
        !
        ! Results:
        !   finished_entry?     flag signifying entry is finished
        !   ask_message$       message to display
        !   tmp_help_topic$    help topic
        !   ask_default$       default response
        !   ask_uc_response?    upper case flag
        !   ask_length         max input length
        !   ask_prompt$        prompt text
        !   ask_validation$    validations rules
        !   reply$             user's response is blanked
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine init_ask_vars
          reply$ = ""
          ask_validation$ = validation$
          validation$ = ''
          ask_prompt$ = prompt$
          if  pos(ask_prompt$, '?') = 0 then ask_prompt$ = ask_prompt$ + '? '
          prompt$ = ''
          ask_length = length
          length = 0
          ask_uc_response? = uc_response?
          uc_response? = false
          ask_default$ = default$
          default$ = ''
          tmp_help_topic$ = help$
          help$ = ''
          ask_message$ = response_message$
          response_message$ = ''
          finished_entry? = false
        end routine



