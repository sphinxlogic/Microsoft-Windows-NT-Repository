1       !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Program: INT_DEMO_B_UPDATE_BASIC.BAS
        ! System : INTOUCH DEMONSTRATION SYSTEM
        ! Author : Daniel James Swain
        ! Date   : 10-JAN-1991
        ! Purpose: Illustrate updating data records
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        map (vendor_map) string whole_rec$ = 200
        map (vendor_map) string name$ = 30, sex$ = 1, &
                bdate$ = 6, street$ = 30, city$ = 12, zip$ = 9, &
                ethnic$ = 1, parent$ = 24, phone$ = 10, bphone$ = 10, &
                ephone$ = 10, balance$ = 8


        open 'sys$scratch:tti_vendor.tmp' for input as file #1, &
                access modify, allow modify, map vendor_map, sequential fixed

        when error in
          while -1%
            get #1
            i% = i% + 1%
            r = r + 1
            if  r = 25  then
              r = 0
              print 'On record:'; i%
            end if
            z = val(balance$)
            rset balance$ = num1$(z + 50)
            update #1
          next
        use
          if  err = 11  then
            continue done
          else
            exit handler
          end if
        end when

  done: print 'Records updated:'; i%

        end
