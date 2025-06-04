	! INTOUCH / System 1032 interface module
        ! Common declarations

        external long function dm_cmd

        declare long constant true  = -1
        declare long constant false = 0
        declare long s1032_status
        declare long constant mode_input = 1
        declare long constant mode_outin = 2

         map (s1032_map) long    s1032_ids, &
                         long    current_ds_index, &
                         long    s1032_value, &
                         string  ds_names$ (32) = 32, &
                         long    s1032_ds_id (32), &
                         long    s1032_full_ds_id (32), &
                         long    s1032_rsize (32), &
                         long    ds_mode% (32)
