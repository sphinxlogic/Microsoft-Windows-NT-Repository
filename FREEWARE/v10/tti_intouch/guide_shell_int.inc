30000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   S T A T S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_stats
          if  not show_stats  then exit routine
          clear
          cbase% = scr_width%/2 - 15
          rbase% = 2
          r1% = rbase% + 7
          r2% = r1% + 1
          r3% = r2% + 1
          r4% = r3% + 1
          r5% = r4% + 1
          buf$ = space$(30)
          clear area box: rbase%, cbase%, rbase%+13, cbase%+31
          cset buf$ = 'GQL'
          print at rbase%+1, cbase%+1, reverse : buf$;
          cset buf$ = 'Report Generation Statistics'
          print at rbase%+2, cbase%+1, reverse : buf$;
          for z = 1 to 3
            cset buf$ = title$(z)
            print at rbase%+2+z, cbase%+1 : buf$;
          next z
          print at r1%, cbase%+2 : 'Records searched:';
          print at r2%, cbase%+2 : 'Records selected:';
          print at r3%, cbase%+2 : 'Records read    :';
          print at r4%, cbase%+2 : 'Pages printed   :';
          print at r5%, cbase%+2 : 'EST time to go  :';
          gosub show_recs_searched
          gosub show_recs_selected
          gosub show_recs_printed
          gosub show_pages_printed
        end routine



31000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   E X T R A C T   S T A T U S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_extract_status
          if  not show_stats  then exit routine
          rec% = rec% + 1
          sta% = sta% + 1
          if  sta% <> 25  then  exit routine
          sta% = 0
          if  time(0) - start_time <= 3  then  exit routine
          start_time = time(0)
          ask window: typeahead z0$
          if pos(z0$, cntrl_z$) > 0 then
            set window: typeahead change$(z0$, cntrl_z$, '')
            bailout% = true
          end if
          gosub show_recs_searched
          gosub show_recs_selected
        end routine



32000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   R E P O R T   S T A T U S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_report_status
          detail_rec% = detail_rec% + 1
            !++amr++ 4.1d
            ! moved from below to above following IF statement
            ! the cutoff depends on this variable

          if  not show_stats  then exit routine
          sta% = sta% + 1
          if  sta% <> 25  then  exit routine
          sta% = 0
          if  time(0) - start_time <= 3  then  exit routine
          start_time = time(0)
          ask window: typeahead z0$
          if pos(z0$, cntrl_z$) > 0 then
            set window: typeahead change$(z0$, cntrl_z$, '')
            bailout% = true
          end if
          gosub show_recs_printed
          gosub show_eoj
        end routine



33100   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   R E C S   S E A R C H E D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_recs_searched
          if  not show_stats  then exit routine
          print at r1%, cbase%+22, using '###,###' : rec%;
        end routine



33200   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   R E C S   S E L E C T E D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_recs_selected
          if  not show_stats  then exit routine
          print at r2%, cbase%+22, using '###,###' : found%;
        end routine



33300   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   R E C S   P R I N T E D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_recs_printed
          if  not show_stats  then exit routine
          print at r3%, cbase%+22, using '###,###' : detail_rec%;
        end routine



33400   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   P A G E S   P R I N T E D
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_pages_printed
          if  not show_stats  then exit routine
          print at r4%, cbase%+22, using '###,###' : page_counter%;
        end routine



33500   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S H O W   E O J
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine show_eoj
          if  not show_stats  then exit routine
          if  (found% = 0) or (detail_rec% = 0)  then exit routine
          z0 = time(0)
          z1 = z0 - start_print_time
          if  z1 = 0  then exit routine
          until_eoj = (found% - detail_rec%) / (detail_rec% / z1)
          hh = int(until_eoj / 3600)
          until_eoj = mod(until_eoj, 3600)
          mm = int(until_eoj / 60)
          ss = mod(until_eoj, 60)
          until_eoj = (hh * 10000) + (mm * 100) + ss
          print at r5%, cbase% + 21, using "##:##:##": until_eoj;
        end routine



90000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! A S K   F O R   P R I N T   O U T P U T   O P T I O N
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Ask for the print output file option.
        !
        ! EXPECTED:
        !       u_str$       = file name of the .LST file to print
        !       u_scr_width%% = screen with
        !       u_dispatch$ = routine to dispatch to after clear screen
        !                     (u_dispatch$ is cleared on reference)
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        %include 'tti_run:print_option'

