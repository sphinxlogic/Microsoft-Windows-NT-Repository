23000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! S E T   B R E A K   C O M P A R E S
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine set_break_compares
          when exception in
            {BREAKCOMPARE}
          use
            if  extype = no_current_record  then continue
          end when
        end routine



        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! C H E C K   L E V E L   B R E A K
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! check to see if a level break has occured
        !
        ! Expected:
        !       levels = number of level breaks set
        !       break_hold$(brk) = last contents of break fields
        !       break_compare$(brk) = current contents of break fields
        !
        ! Result  :
        !       level_count() is incremented
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine check_level_break
          for brk% = 1 to levels%
            if  first_line%  then
              break_hold$(brk%) = break_compare$(brk%)
              iterate for
            end if
            if  break_compare$(brk%) <> break_hold$(brk%)  then
              gosub level_break
              exit for
            end if
          next brk%
          for i = 0 to levels%
            level_count(i) = level_count(i) + 1
          next i
        end routine



