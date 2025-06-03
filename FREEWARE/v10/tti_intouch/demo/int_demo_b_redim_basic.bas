1       !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! Program: BASIC_REDIM.BAS
        ! System : INTOUCH DEMONSTRATION SYSTEM
        ! Author : Daniel James Swain
        ! Date   :  4-JAN-1991
        ! Purpose: To illustrate redimensioning arrays with BASIC
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

        increment = 10
        z5 = 1%
        dim table(z%)
        dim table_save(z%)

        print 'Array redimension from 1 to 4000 started...'
        for i = 1 to 4000
          when error in
            table(i) = i
          use
            cur_elements = i - 1
            dim table_save(cur_elements)
            for z = 1 to cur_elements
              table_save(z) = table(z)
            next z
            z = cur_elements + increment
            dim table(z)
            for z = 1 to cur_elements
              table(z) = table_save(z)
            next z
            retry
          end when
        next i

        end
