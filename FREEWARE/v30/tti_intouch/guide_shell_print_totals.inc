27000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R I N T   T O T A L S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine print_totals
          if  tsuppress% and (level_count(level%) = 1) &
              and (level% = levels%) then exit routine
          if  skip_break_totals(level%) then exit routine
          if  line_counter% >= lines_per_page%  then gosub new_page
          {PRTTOTALS}
        end routine



