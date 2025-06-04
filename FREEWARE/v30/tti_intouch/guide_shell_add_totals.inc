24000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! A D D   T O T A L S
        ! Add level totals here
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine add_totals
          when exception in
            for level% = 0 to levels%
              {ADDTOTALS}
            next level%
          use
            if  extype = no_current_record  then continue
          end when
        end routine



