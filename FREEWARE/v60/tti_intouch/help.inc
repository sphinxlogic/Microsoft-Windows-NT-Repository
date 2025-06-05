        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Module      HELP
        ! Purpose     Get help about topics
        ! Date        6 Feb 1989
        ! Written by  Robert Steven van Keuren, Software Design and Consulting
        !         for Touch Technologies, Inc.
        ! Version     1.1b
        ! Modified    23 Jan 1990
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Expected:
        !   help_structure$    Help structure to open
        !   help_topic$        First topic to display; null if none.
        !   help_noprompt%     False if we are to prompt for further topics
        !                        (default);
        !                      true to display one topic and return.
        !   help_norestore%    True if we are not to restore the screen
        !                        on exit;
        !                      False if we are to restore the screen (default).
        !
        ! Result:
        !   help_topic$        Reset to a null string.
        !
        !
        ! Summary
        !   This is a module to provide help for the user within a program.
        !   It is set up as an include file you can use in any program.
        !
        !   The help text is stored in an indexed structure, 
        !   with one record for each topic.
        !
        !   Each topic can have RELATED TOPICS.  After reading one topic
        !   the user can select any related topic, and read that.  The
        !   user can also enter the name of any other topic in the 
        !   structure, or all or part of a subtopic name.
        !
        ! How to use:
        !   1.  Include the module in your program.
        !
        !   2.  Set HELP_STRUCTURE$ to the help structure name.
        !          (use  FILENAME.DAT  if using the common help structure)
        !       GOSUB HELP_INITIALIZE
        !       This must be done once before displaying any help.
        !
        !   3.  Each time you want to provide help or information,
        !       set HELP_TOPIC$ to the initial topic to display (this 
        !       is optional); then GOSUB HELP.
        !
        !       If you do not set HELP_TOPIC$, the routine looks for and
        !       displays the record with the key HELP.
        !
        !       After displaying the topic, the routine displays any
        !       related topics and prompts the user for additional topics.
        !       The user can enter all or part of a related topic name,
        !       or the complete name of any topic in the file.
        !
        !       The module translates any related topic name to the full 
        !       topic name, and displays the topic.  If no topic matches
        !       the topic entered, the module does a partial key search
        !       on the subtopic field.  If there are multiple matches, 
        !       the module gives the user a menu of matching subtopics.
        !
        !   4.  Set up the help file.
        !
        !
        !   Create the Help file
        !
        !     You can create the help file HELP_MAINT or use the 
        !     CONVERT_HELP program (if you are converting a standard VMS
        !     help library).
        !
        !   Help File Structure
        !
        !     The help file can be any RMS indexed file.  It must have the
        !     following fields:
        !
        !     Field 1:  TOPIC       Topic name--key field, noduplicates.
        !                           This field needs to be long enough
        !                           for the longest topic name.  If you
        !                           are converting from a VMS help library,
        !                           make this long enough to hold the entire
        !                           string of "topic subtopic ...".
        !     Field 2:  SUBTOPIC    Subtopic name--secondary key field,
        !                           usually contains the last word of the
        !                           topic field.  Duplicates allowed.
        !     Field 3:  TEXT        Text to display.  This field needs
        !                           to be long enough to hold the longest
        !                           help text, such as 1500 to 4000 characters.
        !
        !   To provide related topics, the text of the topic may end
        !   with the following:
        !
        !     Related topics:
        !       topic-1    [nickname]
        !       topic-2    [nickname]
        !       topic-3    [nickname]
        !         .
        !         .
        !         .
        !
        !   where
        !     topic-n     Name of a related topic
        !                 Enclose in quotes if name has spaces in it.
        !     nickname    Nickname for the related topic (optional),
        !                 so the user can type in a shorter or more
        !                 meaningful name.
        !                 Enclose in quotes if nickname has spaces in it.
        !
        !   Example:
        !
        !     Related topics:
        !       ask_structure_current    current
        !       ask_structure_id         id
        !       ask_structure_fields     fields
        !
        ! The conversion program CONVERT_HELP automatically produces
        ! related topics between parent topics and subtopics in VMS
        ! help files.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   I N I T I A L I Z E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Initialize for HELP.
        ! Open the default help structure.
        ! This routine must be called by the calling program
        ! to open up the help structure before displaying topics.
        ! Set the no-prompt and no-restore flags to themselves so 
        ! INTOUCH doesn't complain if they're not set.
        !
        ! Expected:
        !   help_norestore%    True if we are not to restore screen on exit
        !   help_noprompt%     True if we are not to prompt for another topic
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_initialize

        help_noprompt% = help_noprompt%
        help_norestore% = help_norestore%

        help_structure$ = ucase$(help_structure$)
        
        !gosub help_open ++DJS++ 16-DEC-1992 open only when used
        highlight_attribute$ = 'bold,reverse'

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! This is the top-level routine for displaying help.
        ! Initialize.
        ! Repeat
        !   Ask for a topic unless we already have one.
        !   Find the topic.
        !   Display it, if we found it.
        !
        ! Expected:
        !   HELP structure has been opened.
        !   help_topic$        Initial topic to display
        !   help_noprompt%     True if we are to show one topic and return.
        !
        ! Used:
        !   help_state$        Next procedure to run
        !   have_topic%        True if we already have a topic.
        !                      Reset to false to ask for topic next time
        !   saved_zonewidth%   Saved zonewidth
        !
        ! Result  :
        !   help_topic$        Reset to null string
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help

        do
          gosub help_init
          if  _error then exit do
          help_state$ = 'find'
          do
            on  item('ask, find, get_text, display, next_screen', help_state$) &
                gosub help_ask_topic, &
                      help_find_topic, &
                      help_get_text, &
                      help_display, &
                      help_display_next_screen &
                else exit do
          loop
          if help_noprompt% then delay  !++ dme ++
        end do
        set zonewidth saved_zonewidth%
        set scroll 1, 24
        help_topic$ = ''
        if  not help_norestore%  then  set window : current help_orig_screen$

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   I N I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Initialize for HELP.
        ! These things are here so that this include file can be
        ! used from any program.
        !
        ! Initialize variables.
        ! Save current zonewidth so we can restore it later.
        ! Set zonewidth (for related topics).
        ! Set scrolling region.
        ! Get the screen width.
        ! Print bottom line of screen.
        !
        ! See also the HELP_INITIALIZE routine, which must be called
        ! first, to open the help structure.
        !
        ! Expected:
        !   help_topic$        Initial topic to display, if any
        !
        ! Result  :
        !   help_topic$        Initial topic to display, set to
        !                      HELP if no topic was given.
        !   saved_zonewidth%   Previous zone width
        !   screenwidth%       Screen width (80 or 132)
        !   help_orig_screen$  Screen as it is at the beginning
        !   help_bottom$       Bottom line of screen
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_init

        if  not help_norestore%  then  ask window: current help_orig_screen$
        gosub help_init_variables

        ask zonewidth saved_zonewidth%
        set zonewidth 16
        set scroll 2, 21

        help_bottom$ = rpad$('Exit', screenwidth%)
        lset help_bottom$[65:79] = 'Help   \ = Back'
        print at 24, 1, reverse: help_bottom$;
        ask structure help : id z$  ! ++DJS++ 16-DEC-1992
        if  len(z$) = 0 then gosub help_open ! ++DJS++ 16-DEC-1992
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   I N I T   V A R I A B L E S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Initialize variables.
        !
        ! Expected:
        !
        ! Result  :
        !   crlf$              CR-LF pair
        !   _extracted         Number of initial help records extracted
        !   have_topic%        We-have-a-topic topic flag.  Set to true,
        !                      so we don't ask for a topic.
        !   help_prompt$       Prompt at bottom
        !   help_prompt_row%   Row to print prompts on
        !   help_topic$        Initial topic to display;
        !                      set to HELP if no topic was given.
        !   lf$                Line feed
        !   maxrow%            Max rows on screen to print on
        !   max_topic_size%    Max topic size, for formatting
        !   quotes$            Two types of quotes
        !   related_topic$(,)  Array of related topics and nicknames
        !   related_topics%    Number of related topics
        !   screenwidth%       Screen width (80 or 132)
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_init_variables

        dim records$(1)
        dim related_topic$(1, 1)

        lf$              = chr$(10)
        crlf$            = chr$(13) + lf$
        have_topic%      = true
        help_prompt$     = 'Help topic? '
        if  help_topic$  = ''  then  help_topic$ = 'HELP_TOPIC'
        maxitems%        = 18
        maxrow%          = 21
        max_topic_size%  = 15
        help_prompt_row% = 22
        quotes$          = '"' + "'"
        related_topics%  = 0

        ask margin screenwidth%

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   O P E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Open a structure for HELP routines.
        ! This routine is called by HELP_INITIALIZE
        ! before displaying any topics.
        !
        ! Expected:
        !   help_structure$    Information structure to open
        !                      Null string if no structure to open.
        !
        ! Result  :
        !   help               Tag name for help structure
        !   error%             Error flag, true if error occurred in open
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_open

        error% = false
        message 'Working...'        
        when exception in
        if right$(help_structure$, 4) = '.DAT' then
          open structure help: name 'tti_run:help', datafile help_structure$
        else
          open structure help: name help_structure$
        end if
        use
          message error: extext$ + ' for ' + help_structure$
          error% = true
        end when
        message ''

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   A S K   T O P I C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask for topic.
        ! Clear just to the right of the prompt, since we are going
        ! to print the prompt anyway.
        ! If they request a list of topics, print one, and allow selection.
        ! Exit if wanted.
        ! If they press the help key, set the topic to 'help'.
        !
        ! Expected:
        !   help_prompt$       Prompt for command or next item
        !
        ! Result :
        !   have_topic%        True if we already have a topic
        !   help_topic$        Topic requested
        !   help_state$        Next procedure to run:  find or exit.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_ask_topic

        do
          clear area help_prompt_row%, len(help_prompt$), &
                help_prompt_row%, screenwidth%
          line input at help_prompt_row%, 1, prompt help_prompt$: help_topic$
          print at help_prompt_row%,1, erase:;
          if  (_exit or _back)  and  (_terminator <> 'UP')  then
            help_state$ = 'exit'
            exit routine
          end if
          help_topic$ = ucase$(help_topic$)
          !### Change this later to some other LIST-type key. ###
          if  (_reply = '*')  or  (_terminator = 'FIND')  then
            help_topic$ = ""
            gosub help_search_subtopic
            if  help_topic$ = ""  then  repeat do
          end if
          help_topic$ = trim$(help_topic$)
          help_state$ = 'find'
          if  _help  then  help_topic$ = 'HELP_TOPIC'
          if  help_topic$ = ''  then
            gosub help_check_terminators
            if  repeat%  then  repeat do
          end if
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   C H E C K   T E R M I N A T O R S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check for special line terminators.
        !
        ! Expected:
        !   _terminator        Terminator for last input
        !   help_multi%        True if printing multi-screen text
        !
        ! Result  :
        !   help_state$        Next routine to do
        !   repeat%            True if we are to repeat topic question
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_check_terminators

        repeat% = false

        ! if  _terminator = 'PF1'  then
        !   key input at help_prompt_row%, len(help_prompt$) + 1: key$
        !   if  ucase$(key$) = 'P'  then
        !     gosub help_print_topic
        !  else
        !    message error: 'Unknown command: ' + key$
        !  end if
        !  repeat% = true
        ! else
        if  help_multi%  then
          gosub help_calc_next_screen
          !if  help_state$ = 'find'  then  help_state$ = 'exit'  ++DJS++ 16-DEC-1992
          if  help_state$ = 'find'  then  
            if  match('RETURN,ENTER', _terminator) > 0  then 
              help_state$ = 'exit'
            else
              repeat% = true
            end if
          end if
        elseif  match('RETURN,ENTER', _terminator) > 0  then 
          help_state$ = 'exit'
        else
          repeat% = true
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   C A L C   N E X T   S C R E E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Calculate the first and last lines for the next screen.
        !
        ! Expected:
        !   first%             First line on previous screen
        !   last%              Last line printed
        !   last_piece%        Last line of text
        !   maxrow%            Max screen print rows
        !   _terminator        Terminator for last input
        !   _back              True if user wants to back up
        !
        ! Result  :
        !   help_state$        Next routine to do
        !   first%             First line to print for next screen
        !   last%              New last line to print
        !   top%               New top line on screen
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_calc_next_screen

        if  _terminator = 'UP'  then
          last%  = top% - 1
          first% = max(last%, 1)
          top%   = first%
          bottom% = top% + maxrow% - 3
          help_state$ = 'next_screen'
        elseif  _terminator = 'DOWN'  then
          first% = bottom% + 1
          last% = min(first%, last_piece%)
          if  last% > bottom%  then  top% = top% + 1
          bottom% = last%
          help_state$ = 'next_screen'
        elseif  _back  or  (_terminator = 'PREV')  then
          last%  = top% - 1
          first% = max(1, top% - maxrow% + 2)
          top%   = first%
          bottom% = top% + maxrow% - 3
          help_state$ = 'next_screen'
        else    ! Next or Return
          first% = bottom% + 1
          if  bottom% < last_piece%  then  help_state$ = 'next_screen'
          last% = min(last_piece%, first% + maxrow% - 3)
          top%  = max(1, last% - maxrow% + 3)
          bottom% = last%
        end if 

 ! message 'Calc: First: ' + str$(first%) + '  Last: ' + str$(last%) &
 !             + '  Top: ' + str$(top%) + '  Bottom: ' + str$(bottom%)

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   F I N D   T O P I C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Find a help topic in the help structure.
        ! Uppercase and trim leading and trailing spaces.
        ! Translate any nickname.
        ! Bring the help topic into memory.
        ! If any problem, report the error and set the flag.
        ! Set up to get the text next.
        ! If we didn't find any record, check subtopics.
        !
        ! Expected:
        !   help_topic$        Topic to find
        !
        ! Result  :
        !   error%             True if some error occurred
        !   help_topic$        Topic, uppercased and possibly translated
        !   help_state$        Next routine to run:  get_text or ask
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_find_topic

        help_topic$ = ucase$(trim$(help_topic$))
        gosub help_translate_nickname
        when exception in
          set structure help, field topic: key help_topic$
        use
          message error : extext$ + ' at ' + exlabel$
          error% = true
          halt
        end when

        help_state$ = 'get_text'
        if  _extracted = 0  then  gosub help_search_subtopic

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! T R A N S L A T E   N I C K N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Translate a nickname to a complete topic name.
        ! Allow partial nicknames.
        ! May want to handle multiple matches later.
        ! (May also want to allow abbreviating main topics)
        !
        ! Expected:
        !   help_topic$        Topic entered by user
        !   related_topic$     Array of related topics and nicknames
        !   related_topics%    Number of related topics
        !
        ! Result  :
        !   help_topic$        Full topic name
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_translate_nickname

        for z% = 1 to related_topics%
          if  pos(related_topic$(z%, 2), help_topic$) = 1  then
            help_topic$ = related_topic$(z%, 1)
            exit for
          end if
        next z%

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   S E A R C H   S U B T O P I C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! We didn't find an exact match on topic, so try a partial
        ! key lookup on the SUBTOPIC field.
        ! If there are no matches, tell user, set flag, and end now.
        ! If one, get it; save complete topic name.
        ! If we find more than one, display a menu.
        !
        ! Expected:
        !   help_topic$        Topic to look for (partial key)
        !
        ! Used:
        !   extracted%         Number of records extracted
        !
        ! Result  :
        !   help_state$        Next routine to do
        !   error%             True if no records found or selected
        !   help_topic$        Topic finally settled on!
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_search_subtopic

        do
          n% = 1
          extract structure help, field subtopic: partial key help_topic$
          end extract
          extracted% = _extracted

          select case extracted%
            case 0
              message error: "Can't find topic " + help_topic$
              error% = true
              help_state$ = 'ask'
              exit routine
            case 1
              set structure help: pointer 1
              help_topic$ = help(topic)
            case else
              gosub help_do_multiple_subtopics
              if  repeat_search%  then  repeat do
          end select
        end do

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   D O   M U L T I P L E   S U B T O P I C S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! We found multiple matches in the SUBTOPIC field,
        ! so display a menu and ask user which one to display.
        ! Handle multiple screens of items.
        ! End now if user wants to back up or exit, or enters nothing,
        !   or enters a subtopic name.
        ! Retrieve the record and get the full topic name.
        !
        ! Expected:
        !   help_topic$        Topic name entered by user
        !
        ! Used:
        !   help_ptr%          Pointer to help record wanted
        !   exit%              True if user wants to exit
        !
        ! Result  :
        !   repeat_search%     True if we are to repeat subtopic search
        !   help_state$        Next routine to do
        !   help_topic$        Full topic name
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_do_multiple_subtopics

        repeat_search% = false
        gosub help_subtopic_menu
        if  exit%  then  
          clear area 2, 1, help_prompt_row%, screenwidth%
          exit% = false
          help_state$ = 'ask'
          exit routine
        end if

        if  not have_topic%  then
          set structure help: pointer help_ptr%
          help_topic$ = help(topic)
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   S U B T O P I C   M E N U
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Display menu of subtopics and ask user to select one.
        !
        ! Expected:
        !   help_bottom$       Bottom line of screen
        !
        ! Used:
        !   repeat%            True if we are to repeat menu display 
        !                      and question
        !   top_ptr%           Number of first record displayed on screen
        !
        ! Result  :
        !   help_ptr%          Number of record selected
        !   exit%              True if user wants to exit from menu
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_subtopic_menu

        top_ptr% = 1
        do
          gosub help_display_multiple_subtopics
          gosub help_ask_menu_item
        loop while repeat%
        print at 24, 1, reverse: help_bottom$;

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   D I S P L A Y   M U L T I P L E   S U B T O P I C S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Display multiple subtopics as a menu.
        ! Clear the screen.
        !
        ! Expected:
        !   extracted%         Number of matches found
        !   maxitems%          Max menu items per screen
        !   top_ptr%           First record on screen
        !
        ! Used:
        !   ptr%               Record number within collection
        !   n%                 Item number in menu
        !
        ! Result  :
        !   items%             Number of items in menu
        !   last_ptr%          Record number of bottom record on screen
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_display_multiple_subtopics

        n% = 0
        ptr% = top_ptr%
        ask structure help, field subtopic: length z%
        items% = min(maxitems%, (extracted% - top_ptr% + 1))
        last_ptr% = top_ptr% + items% - 1
        clear
        for n% = 1 to items%
          set structure help: pointer ptr%
          print at n% + 1, 4, bold, using '##': n%;
          print tab(8); help(subtopic);
          if  help(subtopic) <> help(topic)  then  print tab(9+z%); help(topic);
          ptr% = ptr% + 1
        next n%
        if  items% < maxitems%  then  
          clear area items% + 2, 1, maxitems% + 1, screenwidth%
        end if
        gosub help_subtopic_footer

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   S U B T O P I C   F O O T E R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Display bottom line for subtopic menu.
        ! Show Prev Screen and Next Screen if appropriate.
        !
        ! Expected:
        !   last_ptr%          Bottom record on screen
        !   top_ptr%           Top record in menu on screen
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_subtopic_footer

        print at 24, 1, reverse: &
              rpad$('Exit                                         ' &
                  + '                          \ = Back', screenwidth%);

        if  top_ptr% > 1  then  
          print at 24, 10, reverse: 'PrevScreen';
        else
          print at 24, 10, reverse: space$(10);
        end if

        if  last_ptr% < extracted%  then
          print at 24, 22, reverse: 'NextScreen';
        else
          print at 24, 22, reverse: space$(10);
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   A S K   M E N U   I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask for a menu item number.
        !
        ! Expected:
        !   top_ptr%           Number of top record on screen
        !   items%             Number of items displayed on screen
        !
        ! Result  :
        !   exit%              True if user wants to exit from menu
        !   help_ptr%          Number of record selected
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_ask_menu_item

        repeat% = false
        exit%   = false
        related_topics% = 0%
        do
          clear area help_prompt_row%, 1, help_prompt_row%, screenwidth%
          line input at help_prompt_row%, 1, prompt 'Help item number or name? ': &
                item$
          if  _exit or _back  then
            exit% = true
            exit routine
          end if
          if  _help  then
            message 'Enter an item number or name, EXIT, Next Screen,' &
                  + ' or Prev Screen'
            repeat do
          end if
          gosub help_check_item
          if  exit% or repeat% or repeat_search%  then  exit routine
          if  error%  then  repeat do
        end do
        help_ptr% = item% + top_ptr% - 1

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   C H E C K   I T E M
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check the user's reply for special answers.
        ! User can press Return or Next Screen for next screenful of items,
        ! or Backslash or Prev Screen for previous screenful.
        !
        ! Expected:
        !   _terminator        Terminator that ended the reply
        !   item$              User's reply
        !   last_ptr%          Bottom record on screen
        !
        ! Result  :
        !   exit%              True if they want to exit from menu
        !   error%             True if no numeric item selected
        !   repeat%            True if we are to repeat the display 
        !                      and question
        !   item%              Numeric value of reply
        !   have_item%         Set to true if we have an item
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_check_item

        repeat% = false

        if  _back  or  (_terminator = 'PREV')  then
          if  top_ptr% = 1  then
            exit% = true
            exit routine
          end if
          top_ptr% = max(1, top_ptr% - maxitems%)
          repeat% = true
          exit routine
        end if
        item$ = trim$(item$)
        if  _terminator = 'NEXT'  then
          if  last_ptr% = extracted%  then
            message error: "There are no more items to see"
            error% = true
            exit routine
          end if
          top_ptr% = min(top_ptr% + maxitems%, extracted%)
          repeat% = true
          exit routine
        end if
        if  item$ = ''  then
          if  last_ptr% = extracted%  then  
            exit% = true
            exit routine
          else 
            top_ptr% = min(top_ptr% + maxitems%, extracted%)
            repeat% = true
          end if
          exit routine
        end if

        gosub help_check_item_numeric

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   C H E C K   I T E M   N U M E R I C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Check whether the user entered a numeric item from menu.
        !
        ! Expected:
        !   items%             Max items on menu
        !   item$              User's reply to Item question
        !
        ! Used:
        !
        ! Result  :
        !   repeat_search%     True if we are to repeat subtopic search
        !   help_topic$        Topic entered by user
        !   have_topic%        Have-topic flag, set to true so we don't 
        !                      ask for one
        !   error%             True if no item number selected
        !   repeat%            True if we are to repeat menu
        !   item%              Item number, if user entered a number
        !                      Zero if no item number entered.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_check_item_numeric

        repeat% = false
        error%  = false
        have_topic% = false
        repeat_search% = false
        if  valid(item$, 'integer')  then  
          item% = val(item$)
          if  item% < 1  or  item% > items%  then
            message error: 'Item ' + item$ + ' is out of range...' &
                  + 'range is 1 to ' + str$(items%)
            repeat% = true
            exit routine
          end if
        else
          help_topic$ = item$
          error% = true
          item% = 0
          have_topic% = true
          repeat_search% = true
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   G E T   T E X T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Get the text and related topics for a topic.
        ! Get just the text without the "Related" part.
        ! Format the text.
        ! Add a blank line if there's not one there already.
        ! Add on the related topic text.
        ! Find out where the related topics start.
        ! Set up for any related topics.
        ! Format the text to print.
        !
        ! Expected:
        !   crlf$              CR-LF pair
        !   lf$                Line feed
        !
        ! Result  :
        !   text$              Text of topic (with related topics)
        !   related%           Starting point of any related topics
        !   help_fmt_text$          Formatted text to display
        !   last_piece%        Number of lines in formatted text
        !   help_state$        Next thing to do
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_get_text

        text$ = help(text)

        related% = pos(lcase$(text$), lf$ + 'related topics') - 1
        gosub help_related_topics

        if  related% < 1  then  z% = len(text$)  else  z% = related%
        help_fmt_text$ = wrap$(text$[1 : z%], 1, 80)
        z% = len(help_fmt_text$)
        if  help_fmt_text$[z%-1:z%] <> crlf$  then  help_fmt_text$ = help_fmt_text$ + crlf$
        help_fmt_text$ = help_fmt_text$ + rel_topic_text$
        last_piece% = pieces(help_fmt_text$)

        help_state$ = 'display'

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   R E L A T E D   T O P I C S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Get related topics.
        ! Get the first 'related topics' line and remove it.
        ! Find out how many related topics there are.
        ! If none, end now.
        ! Redimension and load array of topics and nicknames.
        !
        ! Expected: 
        !   related%            Starting point of related topics
        !
        ! Result  :
        !   rel_topic_text$     Related topic text to print
        !                       Reset to null string if no related topics.
        !   related_topics%     Number of related topics
        !   related_topic$(,2)  Array of related topic names and nicknames, 
        !                       redimensioned and loaded.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_related_topics

        rel_topic_text$ = ''
        rel_text$ = ''
        if  related% < 1  then
          related_topics% = 0
          exit routine
        end if

        gosub get_related_topic_text
        if  edit$(rel_text$, 4 + 16) = ''  then  exit routine
        if  related_topics% = 0  then  exit routine
        redim related_topic$(related_topics%, 2)

        rel_topic_text$ = crlf$ + 'Related topics: ' + lf$ + lf$
        gosub help_load_rel_topic_array
        rel_topic_text$ = wrap$(rel_topic_text$, 1, 80)

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! G E T   R E L A T E D   T O P I C   T E X T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Get the related topics from the text.
        ! Format:
        !
        !     Related topics:
        !       topic   [nickname]
        !       topic   [nickname]
        !       ...
        !
        ! Remove the "Related topics:" line, with its CR-LF.
        !
        ! Expected:
        !   text$              Text of topic
        !   related%           Last char before related topics
        !   lf$                Line feed character (ascii 10)
        !
        ! Result  :
        !   related_topics%    Number of related topics
        !   rel_text$          List of related topics, separated by LF
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine get_related_topic_text

        rel_text$ = text$[related% + 2: len(text$)]
        z$ = piece$(rel_text$, 1, lf$)
        z% = len(z$)
        rel_text$[1 : z% + 1] = ''

        rel_text$ = edit$(rel_text$, 8 + 128)
        related_topics% = pieces(rel_text$, lf$)

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   L O A D   R E L   T O P I C   A R R A Y
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Load related topic arrays.
        ! Remove quotes from topic and nickname.
        !
        ! Expected:
        !   related_topics%    Number of related topics
        !   rel_text$          Text containing related topics
        !
        ! Used:
        !   ref%               Array index for related topics
        !   reference$         Related topic cross-reference
        !   refline$           One line containing reference for related topic
        !   nickname$          Nickname for reference
        !
        ! Result  :
        !   related_topic$     Array of related topics and nicknames
        !      *,1  Reference
        !      *,2  Nickname
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_load_rel_topic_array

        for ref% = 1 to related_topics%
          refline$   = edit$(piece$(rel_text$, ref%, lf$), 4+8+16+128)
          reference$ = element$(refline$, 1, ' ')
          z$ = reference$[1:1]
          if  pos(quotes$, z$) > 0  then
            reference$[1:1] = ''
            z% = len(reference$)
            if  reference$[z%:z%] = z$  then  reference$[z%:z%] = ''
          end if
          if  reference$ = ''  then  iterate for
          gosub help_get_nickname
          related_topic$(ref%, 1) = ucase$(reference$)
          related_topic$(ref%, 2) = ucase$(nickname$)
        next ref%

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   G E T   N I C K N A M E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Get a nickname for a topic, if any.
        !
        ! Expected:
        !   rel_topic_text$    Text for related topics, so far
        !   reference$         Cross-reference full topic name
        !   quotes$            Single and double quotes
        !   refline$           Line with reference and nickname
        !   max_topic_size%    Max size allowed for topic names
        !
        ! Result  :
        !   reference$         Related topic
        !   quotes$            Single and double quotes
        !   nickname$          Nickname
        !   rel_topic_text$    Text for related topics, updated with
        !                      new topic or nickname.
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_get_nickname

        nickname$ = element$(refline$, 2, ' ')
        nickname$ = change$(nickname$, ' ', '_')  !++ debug dme ++
        z$ = nickname$[1:1]
        if  pos(quotes$, z$) > 0  then
          nickname$[1:1] = ''
          z% = len(nickname$)
          if  nickname$[z%:z%] = z$  then  nickname$[z%:z%] = ''
        end if
        if  nickname$ = ''  then  nickname$ = reference$
        z$ = rpad$(nickname$, max_topic_size%)
        if  len(nickname$) > max_topic_size% - 1  then
          z$ = rpad$(nickname$, max_topic_size% * 2)
        end if
        if len(z$) > 40 then z$=chr$(ord('LF'))+z$+chr$(ord('LF'))
        rel_topic_text$ = rel_topic_text$ + z$

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   D I S P L A Y
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Display the text and related topics.
        !
        ! Print all of it if it will fit on the screen;
        ! otherwise just print one screen at a time.
        !
        ! Expected:
        !   help_topic$        Topic to print
        !   text$              Text of topic
        !   last_piece%        Number of lines in formatted text
        !   related%           Start of related topics
        !   rel_topic_text$    Text for related topics
        !   maxrow%            Max print rows on screen
        !   help_fmt_text$          Formatted text
        !
        ! Result  :
        !   help_multi%        True if on multi-screen topic; false otherwise
        !   help_noprompt%     True if we don't prompt for another topic
        !   help_state$        Next routine to run:  ask for topic, or exit
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_display

        clear area 1, 1, 23, screenwidth%
        print at 1, 1: help_topic$

        if  last_piece% < (maxrow% - 2) then 
          help_multi% = false
          for line% = 1 to last_piece%
            help_line$ = piece$(help_fmt_text$, line%)
            print_help_line
          next line%
          !print help_fmt_text$ ++DJS++ 16-DEC-1992
          top% = 1
          bottom% = last_piece%
          last% = last_piece%
        else
          gosub help_multi_screen_init
          gosub help_display_one_screen
        end if
        help_state$ = 'ask'
        if  last% >= last_piece%  then  help_prompt$ = 'Topic? '

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   M U L T I   S C R E E N   I N I T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Initialize for multi-screen display of text.
        !
        ! Calculate which lines to print, and which lines will be the
        ! top and bottom ones on the screen.  This is the same for the
        ! first screen, but it will be different after that.
        !
        ! Expected:
        !   last_piece%        Number of lines in formatted text
        !   maxrow%            Max print rows on screen
        !
        ! Result  :
        !   first%             First line to print on screen
        !   top%               First line displayed on screen
        !   last%              Last line to print
        !   bottom%            Last line displayed
        !   last_piece%        Number of lines in formatted text
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_multi_screen_init

        first%  = 1
        top%    = first%
        last%   = min(first% + maxrow% - 3, last_piece%)
        bottom% = last%

        help_multi% = true

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   D I S P L A Y   N E X T   S C R E E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Display the next screen for a multi-screen topic.
        !
        ! Expected:
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_display_next_screen

        gosub help_display_one_screen

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   D I S P L A Y   O N E   S C R E E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Display one screen of a topic that has multiple screens.
        ! Don't print anything if first row to print is greater than last.
        ! Scroll upwards if UP or PrevScreen.
        ! Set up to ask for another topic.
        ! Modify the next prompt as appropriate.
        !
        ! Expected:
        !   bottom%            last line to display
        !   screenwidth%       width of screen
        !   _back              True if user asked to back up
        !   _terminator        Terminator from most recent input
        !   help_fmt_text$     Formatted text to display
        !   first%             First line to display
        !   last%              Last line to display
        !
        ! Used:
        !   line%              Current line
        ! 
        ! Result  :
        !   help_prompt$       Next prompt to use
        !   help_state$        Next routine to run:  ask for topic
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_display_one_screen

        if  first% <= last%  then
          if  (_terminator = 'PREV')  or  _back  then
            !for line% = last% to first% step -1 ++DJS++ 16-DEC-1992
            clear area 2, 1, 21, screenwidth%
            print at 2, 1:;
            for line% = first% to bottom%
              !print at 2, 1: chr$(27) + 'M'; ++DJS++ 16-DEC-1992
              help_line$ = piece$(help_fmt_text$, line%)
              print_help_line
              !print at 2, 1: piece$(help_fmt_text$, line%); ++DJS++ 16-DEC-1992
            next line%
          else
            if  help_state$ = 'next_screen'  then  print at 21, 1:;
            for line% = first% to last%
              help_line$ = piece$(help_fmt_text$, line%)
              print_help_line
              !print piece$(help_fmt_text$, line%) ++DJS++ 16-DEC-1992
            next line%
          end if
        end if

        help_state$ = 'ask'
        gosub help_set_prompt

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   S E T   P R O M P T
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Set up prompt for next input.
        ! This depends on whether we have more text to display.
        !
        ! Expected:
        !   help_noprompt%     Whether we prompt for another topic
        !   last_piece%        Last line in the topic text
        !   last%              Last line displayed
        !
        ! Result  :
        !   help_prompt$       Prompt text
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine help_set_prompt

        if  last% < last_piece%  then
          if  help_noprompt%  then
            help_prompt$ = 'Press RETURN to continue... '
          else
            help_prompt$ = 'Press RETURN to continue or enter a topic... '
          end if
          print at 24, 22, reverse: 'NextScreen';
        elseif  help_noprompt%  then
          help_state$ = 'exit'
          exit routine
        else
          help_prompt$ = 'Topic? '
          print at 24, 22, reverse: space$(10);
        end if

        if  first% > 1  then
          print at 24, 10, reverse: 'PrevScreen';
        else
          print at 24, 10, reverse: space$(10);
        end if

        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   P R I N T   T O P I C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Print the current help topic to a file.
        ! ### This routine is not used ###
        !
        ! Expected:
        !   help_fmt_text$     Formatted help text
        ! 
        ! Used:
        !   help_out_ch%       Output channel
        !   help_outfile$      Output file spec
        !   u_str$             File to print
        !   u_scr_width%       Screen width for print_option
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! routine help_print_topic

        ! message 'Printing topic to file...'
        ! delay .4
        ! help_outfile$ = 'sys$scratch:help.lis'
        ! help_out_ch% = 2
        ! open #help_out_ch%: name help_outfile$, access output

        ! print #help_out_ch%: help(topic); chr$(0)
        ! for z% = 1 to last_piece%
        !   print #help_out_ch%: piece$(help_fmt_text$, z%)
        ! next z%
        ! close #help_out_ch%
        ! message 'Help topic printed to ' + help_outfile$

        ! u_scr_width% = 80
        ! u_str$ = help_outfile$
        ! gosub prnt_ask_option

        ! gosub help_restore_screen

        ! end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! H E L P   R E S T O R E   S C R E E N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Restore the screen (such as after returning from PRINT_OPTION).
        ! ### This routine is not used ###
        !
        ! Expected:
        !   help_bottom$       Bottom line of screen
        !   bottom%            Current bottom line on screen
        !   top%               Current top line on screen
        !
        ! Result  :
        !   first%             First line to display after print
        !   last%              Last line to display after print
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! routine help_restore_screen

        ! clear
        ! print at 24, 1, reverse: help_bottom$;
        ! print at 1, 1: help(topic)
        ! first% = top%
        ! last% = bottom%
        ! gosub help_display_one_screen

        ! end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R I N T   H E L P   L I N E
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !   print the help line.  find any text inside {} and highlight
        !   them using the highlight attribute
        !
        ! Expected:
        !   highlight_attribute$ 
        !                      highlight attribute string
        !   help_line$         line of text to display
        !
        ! Locals:
        !   highlight_text$    text to highlight
        !   left_brace         position of left brace
        !   right_brace        position of last right brace found
        !
        ! Results:
        !   help line is printed
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine print_help_line
          right_brace = 1
          do
            left_brace = pos(help_line$, '{', right_brace)
            if  left_brace = 0 then exit do
            print help_line$[right_brace:left_brace-1]; ! print part prior to {
            right_brace = pos(help_line$, '}', left_brace)
            if  right_brace = 0 then right_brace = len(help_line$) + 1
            highlight_text$ = help_line$[left_brace+1:right_brace-1]
            print attributes highlight_attribute$ : highlight_text$;
            right_brace = right_brace + 1
          loop
          print mid$(help_line$, right_brace)
        end routine




