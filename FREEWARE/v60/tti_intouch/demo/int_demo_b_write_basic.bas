1       !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Program: BASIC_SCREEN_IO.BAS
        ! System : INT_DEMO
        ! Author : Daniel James Swain
        ! Date   :  3-JAN-1991
        ! Purpose: To benchmark adding 200 records to an indexed file. 
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        map (index_map) string key$ = 4, &
                               data$ = 1000, &
                               fill$ = 251

        print 'Creating Indexed file...'
        open 'sys$scratch:int_demo_indexed.tmp' for output as file #1, &
                access write, allow modify, map index_map, &
                indexed fixed, primary key key$

        print 'Populating file...'
        the_data$ = 'DATA DATA DATA DATA '
        the_data$ = the_data$ + the_data$ + the_data$ + the_data$ + the_data$
        the_data$ = the_data$ + the_data$ + the_data$ + the_data$ + the_data$
        the_data$ = the_data$ + the_data$
        for i = 1 to 200
          r = r + 1
          if  r = 25  then
            r = 0
            print 'Records written:'; i
          end if
          rset key$ = num1$(i)
          lset data$ = the_data$
          put #1
        next i

        close #1

        end
