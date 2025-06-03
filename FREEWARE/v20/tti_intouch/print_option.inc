1  routine tti$print_option

        ! PRINT_OPTION.INC
        !
        ! Print out a list file to screen, printer port, or system
        ! printer
        !
        ! HISTORY:
        !       16-MAR-1989 jrw  Support for form types DEFAULT and ONEUP
        !        2-Aug-1989 rpr  Allow them to use LAS and laserprint it
        !       13-Feb-1990 amr  Add facility for default and copies
        !       13-May-1991 djs  Added save option
        !       06-nov-92   djs  Added wide/narrow options for system printer
        !       21-Dec-1992 amr  Build up the OPTS_CHOSEN$ string
        !       11-Jan-1992 amr  add _BACK and _EXIT to OPT_CHOSEN$
        !       30-Jun-1994 amr  trap bad disk or directory or ??? on SAVE.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! A S K   F O R   P R I N T   O U T P U T   O P T I O N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask for the print output file option.
        !
        ! EXPECTED:
        !       u_str$       = file name of the list file to print
        !       u_scr_width% = screen width
        !       u_dispatch$  = routine to dispatch to after clear screen
        !                      (u_dispatch$ is cleared on reference)
        !       u_help$      = help topic (if u_help$ is not null then it
        !                      is assumed tti_run:help.inc is %included)
        !                      (u_help$ is cleared on reference)
        !       u_default$   = default output option
        !       u_copies%    = number of copies to print
        !       tti_delete_after_print = is set by init routine
        !               if true and report hasn't been queued to a spooler
        !               then will delete the file at the end
        !       tti_report_queued is set by the print routine to indicate
        !               that the report was queued to a spooler
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  prnt_ask_option:

        gosub prnt_init
        gosub prnt_build_menu
        do
          line input menu po_menu$, default po_menu_default$ : u_reply$
          if  (_back)  then
            opts_chosen$ = opts_chosen$ + ("BACK" + ",")
            exit do
          end if
          if  (_exit)  then
            opts_chosen$ = opts_chosen$ + ("EXIT" + ",")
            exit do
          end if
          if  (_help)  then
            gosub prnt_do_help
            repeat do
          end if
          print at 22, 1:;
          prnt_opt$ = ucase$(u_reply$[1:3])
          select case prnt_opt$
          case "SCR"    : gosub screen_print
          case "PRI"    : gosub port_print
          case "SYS"    : gosub prnt_sys_print
          case "LAS"    : gosub prnt_laser_print
          case "SAV"    : gosub prnt_save_report
          case else
            gosub prnt_to_queue
          end select
          opts_chosen$ = opts_chosen$ + (prnt_opt$ + ",")
                ! build up a string of the chosen print options
          clear
          set window : current po$save_window$
        loop

        if  tti_delete_after_print  and  &
            not  tti_report_queued  then  kill po$print_file$ + ';'
                ! kill the file if they asked for this
                ! don't do it if the report was queued to a printer

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T - I N I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! EXPECTED:
        !   u_additional_queues$ 
        !                      any additional queue options to present
        !       u_str$           = filename to print
        !       u_dispatch$      = routine to run before showing options
        !       u_help$          = help segment name
        !       u_default$       = default print option
        !       u_copies%        = number of copies to print
        !       u_delete%        = true if file should be deleted on exit
        !       tti_print_options is a symbol that might be defined. if
        !       so then it contains comma seperated commands such as 
        !       delete for delete print file when finished
        !
        ! RESULT:
        !   po$additional_queues$ 
        !                      additional queues to present
        !       po$print_file$     = filename to print
        !       po$dispatch$     = routine to run before showing options
        !       po$help$         = help segment name
        !       po$default$      = default print option
        !       po$copies%       = number of copies to print
        !       po$max_screens%  = maximum number of saved screens
        !       po$screen$()     = array of saved screens (for backing up)
        !       po$delete%       = delete flag
        !       original_margin% = saved, original margin
        !       tti_print_options$ = any options set by symbol
        !       tti_delete_after_print = true if print file should be deleted
        !       tti_report_queued = false.  No report queued yet
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine prnt_init
          clear
          po$print_file$ = u_str$
          u_str$       = ''
          po$dispatch$ = u_dispatch$
          u_dispatch$  = ''
          po$help$     = u_help$
          u_help$      = ''
          po$default$  = ucase$(unquote$(u_default$))
          u_default$   = ""
          if  po$default$ = ""  then  po$default$ = "SCREEN"  !++amr++ 02-13-90
          po$copies%   = u_copies%
          u_copies%    = 0%
          if  po$copies% = 0%  then  po$copies% = 1%          !++amr++ 02-13-90
          po$delete%   = u_delete%
          u_delete%    = false
          po$additional_queues$ = u_additional_queues$
          u_additional_queues$ = ''
          po$max_screens% = 1000%
          dim po$screen$(po$max_screens%)
          ask margin original_margin%
          gosub check_laser_printer
          gosub process_options_symbol
          tti_report_queued = false
          opts_chosen$ = ""
                ! string of print options chosen by user
                !++amr++ 12/21/92
          if po$dispatch$ = '' then
            message "Ready to print " + lcase$(po$print_file$) 
          else
            dispatch po$dispatch$
          end if
          ask window : current po$save_window$
        end routine



12000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R O C E S S   O P T I O N S   S Y M B O L
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !  translate the tti_print_options symbol and set up the related
        !  fields
        ! Expected:
        !   po$delete%         field saying this file to be deleted
        !
        ! Locals:
        !   tti_print_options$ translation of symbol
        !
        ! Results:
        !   tti_narrow_print$  print narrow command for system printer
        !   tti_wide_print$    print wide command for system printer
        !   tti_delete_after_print 
        !                      true if file should be deleted
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine process_options_symbol
          ask system, symbol "tti_print_options" : value tti_print_options$
          if  match(tti_print_options$, 'DELETE') > 0  then
            tti_delete_after_print = true
          else
            tti_delete_after_print = false
          end if
          if  po$delete%  then  tti_delete_after_print = true
          tti_wide_print$ = ''
          z = item(tti_print_options$, 'WIDE')
          if  z > 0 then &
            tti_wide_print$ = element$(element$(tti_print_options$,z), 2, ' ')
          tti_narrow_print$ = ''
          z = item(tti_print_options$, 'NARROW')
          if  z > 0 then &
            tti_narrow_print$ = element$(element$(tti_print_options$,z), 2, ' ')
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  C H E C K   L A S E R   P R I N T E R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Check to see if they have a logical "laser".  If they do,
        !    allow them to use "LAS" as an option.
        !  Expects:  There is a logical "laser" set up if they have a
        !               laser printer
        !  Result:   have_laser% = true or false, depending on the
        !               existence of the "laser" logical.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  check_laser_printer:

        ask system, symbol "laser": value z0$

        have_laser% = (z0$ <> "")

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   P R I N T   O U T P U T   O P T I O N S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Display the print output options on the screen.
        !
        ! po$dispatch$ <> ''  if a routine to dispatch to after clear
        ! have_laser% = true if they can say "LAS" and laser it
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        routine prnt_build_menu
          laser_opt$ = ''
          if  have_laser% then laser_opt$ = '"Laser Printer"=las, '
          po_menu$ = '%title " Output Options ", %at 18, center, '
          if  po$additional_queues$ = '' then
            po_menu$ = po_menu$ + '%items 2, '
          else
            po_menu$ = po_menu$ + '%items 3, '
          end if
          po_print_opt$ = '"Screen"=SCR, "Printer Port"=pri, ' + &
            '"System Printer"=sys, "Save to Disk"=sav, ' + laser_opt$
          if  have_laser% then po$result$ = po$result$ + ',LAS'
          for z = 1 to elements(po$additional_queues$)
            z$ = element$(po$additional_queues$, z)
            po_print_opt$ = po_print_opt$ + z$ + ', '
          next z
          z1 = len(po$default$)
          for z = 1 to elements(po_print_opt$)
            z$ = element$(po_print_opt$, z)
            z1$ = ucase$(unquote$(element$(z$, 1, '=')))
            if  po$default$ = z1$[1:z1] then
              po_menu_default$ = '#' + str$(z)
              exit for
            end if
          next z
          po_menu$ = po_menu$ + po_print_opt$ + 'exit'
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T - D O _ H E L P
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Take care "HELP"
        !
        ! EXPECTED:
        !       po$help$    = help segment name.
        !       po$print_file$      = name of file to print
        !
        ! RESULT:
        !       help_topic$ = help segment name.
        !
        !++amr++ 02/13/90
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  prnt_do_help:

        if  (po$help$ = '')  then
          message "Select where to print " + lcase$(po$print_file$)
        else
          clear
          help_topic$ = po$help$
          dispatch "help"
        end if

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R I N T   R E P O R T   T O   P R I N T E R   P O R T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Print to the printer port.
        !
        ! EXPECTED:
        !       u_scr_width%     = screen/file width
        !       po$print_file$           = name of the file to print out
        !       po$copies%       = number of copies to print
        !       original_margin% = original/saved screen width
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  port_print:

        clear
        set margin u_scr_width%
        print at 1, 1:;

        set port on

        for copies% = 1% to po$copies%
          prnt_ch = _channel
          open #prnt_ch : name po$print_file$, access input

          do
            when exception in
              line input #prnt_ch : text$
            use
            end when
            if  _error  then  
              set error off
              exit do
            end if
            print text$
          loop
          print chr$(12) ! force final form feed so that laser printers will
                         ! print the last page. won't hurt other printers
          close #prnt_ch
        next copies%

        set port off
        set margin original_margin%

        set port on
        set port off

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T _ S Y S _ P R I N T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Print to system print  OR  other queue.
        ! Ask for queue to use.
        !
        ! EXPECTED:
        !       po$print_file$      = name of file to print.
        !       po$copies%  = number of copies to print.
        !       tti_delete_after_print = true if they want report file deleted
        !
        ! RESULT:
        !       queue_name$ = queue to use.
        !       tti_report_queued = true to indicate that the report file
        !                       has been queued to a spooler
        !
        !++amr++ 02/13/90
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  prnt_sys_print:

        clear area 18, 1, 19, 80

        gosub setup_default_queue
        message "Enter any queue qualifiers after the queue name"
        do
          line input &
            prompt "Print queue? ", &
            default tti_sys_default$, &
            length 60, &
            at 21, 1 : &
            u_reply$

          if  (_exit)  or  (_back)  then  exit do

          if  (_help)  then
            message "Enter the name of the print queue to use"
            repeat do
          end if

          queue_name$ = u_reply$

          gosub prnt_set_up_pass
          if  tti_delete_after_print  then
            z$ = "print/delete"
          else
            z$ = "print"
          end if
          pass z$ + &
                     "/copies=" + str$(po$copies%) + &
                     "/noidentify/notify/queue=" + queue_name$ + ' ' + &
                     po$print_file$
          gosub prnt_check_pass
          if  error  then  repeat do
          tti_report_queued = true ! set flag saying a report has been queued
        end do

        return



13000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S E T U P   D E F A U L T   Q U E U E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !   set up the default system print queue
        ! Expected:
        !   tti_narrow_print$  print narrow queue
        !   tti_wide_print$    print wide queue
        !
        ! Locals:
        !
        ! Results:
        !   tti_sys_default$   default queue
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine setup_default_queue
          tti_sys_default$ = "SYS$PRINT"
          if  u_scr_width% > 80  then
            if  tti_wide_print$ <> '' then tti_sys_default$ = tti_wide_print$
          else
            if  tti_narrow_print$ <> '' then tti_sys_default$ = tti_narrow_print$
          end if
        end routine



14000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T   T O   Q U E U E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !   user selected one of the additional queues
        !   set up a generic command to pass to DCL
        !
        ! Expected:
        !   u_reply$           queue name from input menu
        !
        ! Locals:
        !
        ! Results:
        !   tti_report_queued  set to true
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine prnt_to_queue
          queue_name$ = u_reply$ ! from input menu
          gosub prnt_set_up_pass
          if  tti_delete_after_print  then
            z$ = "print/delete/queue="
          else
            z$ = "print/queue="
          end if
          pass z$ + queue_name$ + &
                     "/copies=" + str$(po$copies%) + &
                     "/noidentify/notify " + &
                     po$print_file$
          gosub prnt_check_pass
          if  error  then  
            message error : systext$
            exit routine
          end if
          tti_report_queued = true ! set flag saying a report has been queued
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T _ L A S E R _ P R I N T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Print to laser printer  IF  there is one set up.
        !
        ! EXPECTED:
        !       have_laser%      = TRUE  if  symbol LASER is set up.
        !       po$print_file$           = name of file to print.
        !       u_scr_width%     = width of file to print.
        !       po$copies%       = number of copies to print.
        !
        ! RESULT:
        !       laser_qualifier$ = LASER command qualifier.
        !
        !++amr++ 02/13/90
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  prnt_laser_print:

        if  have_laser%  then
          if  u_scr_width% > 80  then
            laser_qualifier$ = '/L'
          else
            laser_qualifier$ = '/F'
          end if

          for copies% = 1% to po$copies%
            pass 'laser ' + po$print_file$ + ' ' + laser_qualifier$
            gosub prnt_check_pass
            if  error  then  exit for
          next copies%
        else
          message error : "Invalid option: " + u_reply$
        end if

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T   S A V E   R E P O R T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !
        ! Expected:
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine prnt_save_report

        clear area 18, 1, 19, 80
        po$filespec$ = lcase$(po$print_file$)
        gosub po$break_apart_filespec
        po$save_filename$ = po$disk$ + po$directory$ + po$filename$ + '.lis'
        do
          line input &
            prompt "Filename to save as? ", &
            default po$save_filename$, &
            length 60, &
            at 21, 1 : &
            u_reply$
          if  (_exit)  or  (_back)  then  exit do
          if  (_help)  then
            message "Enter the name to save this report under"
            repeat do
          end if
          po$filespec$ = lcase$(u_reply$)
          gosub po$break_apart_filespec
          if  po$extension$ = ''  then  po$filespec$ = po$filespec$ + '.lis'
          message "Saving " + po$print_file$ + " as " + po$filespec$

          !++amr++ 06/30/94 -- code below deleted.  replacement code below.
          !pass "copy " + po$print_file$ + " " + po$filespec$
          !gosub prnt_check_pass
          !if  error  then  repeat do
          !++amr++ 06/30/94 -- code above deleted.  replacement code below.

          !++amr++ 06/30/94 -- begin new code.

          pass "set message/nofacility/noidentification/noseverity/notext"

          pass "copy " + po$print_file$ + " " + po$filespec$

          systext_msg$ = systext$

          ask system, pass : success ok?

          pass "set message/facility/identification/severity/text"

          if  not ok?  then
            message error : systext_msg$
            repeat do
          end if

          !++amr++ 06/30/94 -- end new code.
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P O $ B R E A K   A P A R T   F I L E S P E C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! take a file spec and return the disk, directory, filename and
        ! extension
        !
        ! Expected:
        !       po$filespec$ = file spec
        !
        ! Result  :
        !       po$disk$      = disk
        !       po$directory$ = directory
        !       po$filename$  = filename
        !       po$extension$ = extension
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine po$break_apart_filespec

        po$disk$      = ""
        po$directory$ = ""
        po$filename$  = ""
        po$extension$ = ""
        z1 = 1
        z = pos(po$filespec$, ":", z1)
        if  z > 0  then  
          po$disk$ = po$filespec$[z1:z]
          z1 = z + 1
        end if
        z = pos(po$filespec$, "]", z1)
        if  z > 0  then  
          po$directory$ = po$filespec$[z1:z]
          z1 = z + 1
        end if
        z = pos(po$filespec$, ".", z1)
        if  z > 0  then  
          po$filename$ = po$filespec$[z1:z-1]
          z1 = z + 1
          po$extension$ = po$filespec$[z+1:len(po$filespec$)]
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T _ S E T _ U P _ P A S S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  prnt_set_up_pass:

        !clear ++DJS++ 03-MAR-1993
        !z1$ = "Submitting " + po$print_file$ ++DJS++ 03-MAR-1993
        !z1% = (original_margin% / 2%) - (len(z1$) / 2%) ++DJS++ 03-MAR-1993
        !print at 10, z1% : z1$ ++DJS++ 03-MAR-1993
        message delay : 'Submitting '; po$print_file$; '...'
        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R N T _ C H E C K _ P A S S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  prnt_check_pass:

        ask system, pass : success z1%

        error = (z1% = false)

        if  error  then  message error : ""

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R I N T   R E P O R T   T O   S C R E E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Print a list file to the screen.
        !
        ! EXPECTED:
        !        First heading line begins with a CHR$(12%)
        !        All heading lines end with a CHR$(0%)
        !        po$print_file$ = name of the .list file to print out
        !        u_scr_width% = screen width
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  screen_print:

        clear
        print at 1, 1:;
        set margin u_scr_width%
        prnt_ch = _channel
        open #prnt_ch : name po$print_file$, access input
        po$screen_ctr% = 0%
        head_lines% = 0%
        first_page% = true

        do
          when exception in
            line input #prnt_ch : text$
            eof% = false
          use
            eof% = true
          end when

          if  eof%  then
            delay
            if _back or _terminator = 'PREV' then
              gosub do_back_page
              repeat do
            end if
            exit do
          end if

          if  right$(text$, 1%) = chr$(0%)  then
            gosub screen_print_heading_line
            if  po$do_delay and _exit  then exit do
            if  po$do_delay and _back then
              gosub do_back_page
              repeat do
            end if
            iterate do
          end if

          ask window: row line_ctr%
          if  line_ctr% = head_lines%  then
            clear area head_lines% + 1%, 1%, 22%, u_scr_width%
          end if

          ask window: row line_ctr%
          if  line_ctr% >= 22%  then
            delay
            if _back  or _terminator = 'PREV' then
              gosub do_back_page
              repeat do
            end if
            if  _exit  then exit do

            clear area head_lines% + 1%, 1%, 22%, u_scr_width%
            print at head_lines% + 1%, 1%:;
          end if

          print text$
        loop

        close #prnt_ch
        set margin original_margin%

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R I N T   R E P O R T   H E A D I N G   L I N E S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! This routine is used by SCREEN_PRINT
        !
        ! EXPECTED:
        !        text$ = text line (that ends with a CHR$(0%)
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  screen_print_heading_line:

        po$do_delay = false

        if  left$(text$, 1%) = chr$(12%)  then
          if  first_page%  then
            first_page% = false
          else
            delay
            po$do_delay = true
            if  _exit  or _back  or _terminator = 'PREV' then return
          end if

          clear
          print at 1%, 1%:;
          text$ = text$[2:999]
          head_lines% = 0%
          po$screen_ctr% = po$screen_ctr% + 1%
          ask #prnt_ch: current po$screen$(po$screen_ctr%)
        end if

        print text$

        head_lines% = head_lines% + 1%

        return



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! B a c k   o n e   p a g e
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Causes us to go back one page
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  do_back_page:

        select case po$screen_ctr%
        case 0%
          close #prnt_ch
          prnt_ch = _channel
          open  #prnt_ch: name po$print_file$, access input
          po$screen_ctr% = 1%  ! no pages is just like one page
        case 1%
          set #prnt_ch: current po$screen$(po$screen_ctr%)
        case else
          po$screen_ctr% = po$screen_ctr% - 1%
          set #prnt_ch: current po$screen$(po$screen_ctr%)
        end select

        po$screen_ctr% = po$screen_ctr% - 1%  ! to repeat this one
        first_page% = true  ! act like the first page (no DELAY)
        head_lines% = 0%

        return

  end routine ! tti$print_option
