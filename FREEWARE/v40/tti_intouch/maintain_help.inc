1       !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  H E L P
        !    Include module to do generalized HELP screens
        !
        !  Author : Robert Ringrose
        !  Purpose: Allow HELP to function easily
        !  Date   : 8-21-89
        !
        !  Entry point:  h_help_init    (opens help files, initializes)
        !                h_help         (saves the screen, gives help,
        !                                and restores the screen)
        !   h_help expects 1) h_help_init has been called
        !                  2) h_key$ = the help key to look up
        !                     This key is NOT reset; however, if it is
        !                     blank the routine will simple message
        !                     "No help available"
        !                  3) h_box_top = top of help box (reset to 3)
        !                  4) h_box_bottom = bottom of help box
        !                                       (reset to 20)
        !                  5) h_filename$ = the help file
        !                  6) h_channel = the channel to use.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  H   H E L P   I N I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Initializes variables and such used by HELP
        !    Result:
        !       variables necessary to HELP are initialized
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine h_help_init

        h_box_def_top = 3
        h_box_def_bottom = 20
        h_box_top = h_box_def_top
        h_box_bottom = h_box_def_bottom

        end routine



90600   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H   H E L P
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Expects:
        !   h_help_init has been called
        !   h_channel = the channel to use.
        !   h_filename$ = the file name to look up in
        !   h_key$ = the help key to look up
        !                     This key is NOT reset; however, if it is
        !                     blank the routine will simple message
        !                     "No help available"
        !   h_box_top = top of help box (reset)
        !   h_box_def_top = default top of help box
        !   h_box_bottom = bottom of help box (reset)
        !   h_box_def_bottom = default bottom of help box
        !   
        ! Result:
        !   _HELP IS TURNED OFF!! Beware of this when you call the 
        !     routine.
        !   The screen, with the possible exception of the message area,
        !       is reset
        !   h_msg_displayed = true if a message was displayed, false
        !       otherwise.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine h_help

90620   h_msg_displayed = false
        h_title$ = ""
        gosub h_find_text
        if  not(_error)  then 
          ask window:current h_hold_screen$
          gosub h_box
          gosub h_display_text
          close #h_channel
          set window:current h_hold_screen$
        else
          h_msg_displayed = true
        end if

90699   end routine



90700   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  H   B O X
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Display the help box
        ! Expects:
        !   h_box_top is the top line of the help box
        !   h_box_bottom is the bottom line of the help box
        ! Result:
        !   help box is set up
        !   h_screen_width
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine h_box

90720   ask margin h_screen_width
        clear area box: h_box_top, 1, h_box_bottom, h_screen_width

90799   end routine



90800   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! h   f i n d   t e x t
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Open the help file and find the help text.
        !  Expects:
        !    h_channel = the channel to use (free)
        !    h_filename$ = the file name to look up in
        !    h_key$ = the help key to look up
        !                     This key is NOT reset; however, if it is
        !                     blank the routine will simple message
        !                     "No help available"
        !  Result:
        !    lines from the help file are read until you run out of
        !      help file (key not found) or you find the key.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine h_find_text

90820   when exception in
          open #h_channel : name h_filename$, access input
        use
          message error : "Help file not found - no help available"
        end when
        if  _error  then  exit routine

        z0$ = "$KEY " + trim$(ucase$(h_key$))
        when exception in
          do
            line input #h_channel: h_text_line$
        ! Unnecessary +++ RPR
        !    if  h_text_line$[1:1] = "!"  then repeat do
            if  h_text_line$[1:1] <> "$"  then repeat do
          loop while  edit$(h_text_line$,8 + 16 + 32 + 128) <> z0$
        use
          close #h_channel
          message error: "No help available"
        end when

90899   end routine



90900   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H   d i s p l a y   t e x t
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  display the help text
        !       (delays when get to bottom of screen)
        !  Expects:
        !    The help screen has been set up
        !    channel #h_channel is the help file
        !    the help key has been found already
        !    h_box_top is the top line of the help box
        !    h_box_bottom is the bottom line of the help box
        !  result:
        !    The text is printed, with appropriate page breaks and waits,
        !      until a line beginning with '$KEY' is read.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine h_display_text

90920   h_cur_down = h_box_top + 4
        gosub h_display_title
        line input #h_channel: h_text_line$
        do until  ucase$(h_text_line$[1:4]) = '$KEY'
          if  h_cur_down >= h_box_bottom  then
            print at 23, 1:;
            delay
            if  _back or _exit  then exit routine
            h_cur_down = h_box_top + 4
            clear area box: h_box_top, 1, h_box_bottom, h_screen_width
            gosub h_display_title
          end if
          gosub h_write_text_line
          when exception in
            line input #h_channel: h_text_line$
          use
            h_text_line$ = '$KEY'
          end when
        loop
        print at 23, 1:;
        delay

90999   end routine



91000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H   w r i t e   t e x t   l i n e
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Write a text line at h_cur_down, 2.
        !  Seperate procedure so it could get fancy (hilight parts...)
        !
        ! Expects:
        !   h_text_line$ = the line to print
        !   h_cur_down = the distance down the screen (not the help 
        !     screen, the real screen)
        ! result:
        !   prints the line h_cur_down distance down.
        !     if the line is a command (begins with $), don't print
        !     and do the command instead.
        !   h_cur_down is incremented (or set to help_bottom if there 
        !     is a page break)
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine h_write_text_line

91020   if  h_text_line$[1:1] <> '$'  then
          if  h_cur_down = h_box_top + 1 and  &
              trim$(h_text_line$) = ""  then  exit routine
          print at h_cur_down, 2:h_text_line$[1:h_screen_width-2]
          h_cur_down = h_cur_down + 1
          exit routine
        end if
        select case ucase$(h_text_line$[2:4])
          case is = 'BRE'
            if  elements(h_text_line$,' ') = 2  then
              h_break = val(element$(h_text_line$, 2, ' '))
            else
              h_break = h_box_bottom
            end if
            if  h_cur_down >= h_box_bottom - h_break  then &
              h_cur_down = h_box_bottom
          case is = "TIT"       ! Title
            z = pos(h_text_line$," ")
            h_title$ = h_text_line$[z+1:999]
            gosub h_display_title
          case else
        end select

91099   end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  H   D I S P L A Y   T I T L E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !  Display the help box's title
        !  Expects:  h_title$ has the title
        !            The box is on the screen
        !            The display location can be overwritten.
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  routine h_display_title

        print at h_box_top + 1,2, bold: h_title$

        end routine
