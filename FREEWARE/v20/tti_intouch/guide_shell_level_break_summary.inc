          if  levels% > 0  then 
            level% = levels%  ! write detail line which is lowest level
            gosub print_detail_line
            z% = levels% - 1%
            groupcount(z%) = groupcount(z%) + 1
            groupcount(levels%) = 0
          end if
          break_hold$(levels%) = break_compare$(levels%)
          if  levels% > 0  then
            for z = 1 to totals_count%
              total(z,levels%) = 0
            next z
          end if
          level_count(levels%) = 0
          last_break_level% = brk%
          if  (brk% = levels%) and (brk% > 0)  then  exit routine
