25000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! L E V E L   B R E A K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Do a level break.
        ! If summary report:
        !   Reset record number to first record at lowest level
        !   Print detail line (totaled fields print totals)
        !   Reset information for lowest level
        !   Set deepest level for col totals = second deepest level
        !   Exit if break is at lowest level (no need to print col totals)
        ! Detail & summary reports:
        !   For each level from deepest level to break level:
        !     Print totals on separate lines
        !     Reset break hold variable
        !
        ! Variables to watch:
        !   levels = nbr of level breaks ( = lowest or most minor level)
        !   brk    = level at which break occurred
        !   deepest_level = lowest level to print totals
        !     detail:  = levels (all totals print on extra lines)
        !     summary: = levels - 1 (lowest totals print on detail line)
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine level_break
          do_page_break% = false

{IF summary = true} {INCLUDE tti_run:guide_shell_level_break_summary.inc}

          for level% = deepest_level% to brk% step -1
            if  page_break(level%) > 0  then do_page_break% = true
            if  do_totals%  or  do_groupcount%  then
              if  do_page_break% and (level% = 0)  then
                grand_total? = true
                        !++amr++ 4.1j
                gosub new_page
              end if

              gosub print_totals
            end if
            break_hold$(level%) = break_compare$(level%)
            z% = level% - 1%
            if  z% >= 0%  then  groupcount(z%) = groupcount(z%) + 1
            groupcount(level%) = 0
            level_count(level%) = 0
            for z = 1 to totals_count%
              total(z,level%) = 0
            next z
          next level%

          if  making_report then 
            print #out_ch:
            line_counter% = line_counter% + 1
          end if

          last_break_level% = brk%

          !if  do_page_break% and (brk% > 0)  then gosub new_page
          !++amr++ 4.1d -- code above deleted.  replacement code below.

          if  (do_page_break%)  and  &
              (brk% > 0)        then
            line_counter% = lines_per_page%
          end if
                !++amr++ 4.1d
        end routine



