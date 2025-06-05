        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! I N P U T   D R I V E R
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! dispatch to each input statement
        !
        ! Expected:
        !       each input statement will be in a routine: input_usr_xxx
        !
        ! Result  :
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine input_driver
          clear
          z$ = space$(margin)
          cset z$ = title$(1)
          print at 1, 1, bold : z$;
          rset z$ = "\ = Back  HELP = Help"
          z$[1:11] = "EXIT = Exit"
          print at 24, 1, reverse : z$;
          print at 3, 1:;
          set system : comment "Waiting for input"
          {INPUT_STMTS}
