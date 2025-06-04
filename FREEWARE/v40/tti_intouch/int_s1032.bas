1 sub first
   ! INT_S1032 - System 1032 interface module
   ! Daniel Esbensen
   ! Written September 20, 1987
   ! Copyright (c) 1987, TTI
  end sub

150  sub s1032_setup (status%)
        %include 'tti_run:int_s1032.inc'
        debug_flag% = false
        if  debug_flag%  then
          call dm_begin ('DEBUG')
        else
          call dm_begin
          s1032_status = dm_cmd ('set message all no print')
        end if
        call dm_common by desc ('S1032_VALUE', s1032_value) ! common information
        status% = 0%
     end sub

250  sub s1032_open (status%, ds_index%, filename$, omode$)

        %include 'tti_run:int_s1032.inc'
        external long function dm_open_rd

        z0$ = edit$(filename$, 8%+16%+32%+128%)
        z0% = instr(1%, z0$, ' IN ')
        if  z0% > 0%  then
          ds_name$ = left(z0$, z0%-1%)
          opn_cmd$ = 'open ds ' + ds_name$ + ' in ' &
                                + right(z0$, z0%+4%) + ' ' + omode$
        else
          ds_name$ = z0$
          opn_cmd$ = 'open ds ' + filename$ + ' ' + omode$
       end if
       s1032_status = dm_cmd (opn_cmd$)
       call get_ds_index (ds_index%, edit$(ds_name$, -1%))

       if instr(1%, omode$, 'READONLY') > 0% then
         ds_mode%(ds_index%) = mode_input
       else
         ds_mode%(ds_index%) = mode_outin
       end if

       ! Get the logical recordsize for us
       s1032_status = dm_open_rd (ds_name$+'_binary', ds_name$, &
                        s1032_ds_id (ds_index%), s1032_rsize (ds_index%))

       ! Make a single record_descriptor (for speed we hope)
       !rd_name$ = ds_name$ + '_rd'
       !s1032_status = dm_cmd ('rd ' + rd_name$+';', &
                             !'field ' + ds_name$ + '_all' + ' text length ' &
                                 !+ num1$(s1032_rsize (ds_index%)) + ';', &
                             !'end_rd')

       ! now associate each record with just this one large field
       !s1032_status = dm_open_rd (rd_name$, ds_name$, &
                        !s1032_ds_id (ds_index%), s1032_rsize (ds_index%))
       !if s1032_status <> 1% then
         !print 'status: '; s1032_status
       !end if

       status% = 0%

     end sub

350  sub s1032_add (status%, ds_index%, iobuf$, rec_id%)
        %include 'tti_run:int_s1032.inc'
        external long function dm_add_rec

        call set_ds (ds_index%)

        status% = dm_add_rec (iobuf$, s1032_ds_id (ds_index%))
        if status% <> 1% then
          exit sub
        end if

        status% = dm_cmd ('GETRECORD;', &
                          'LET S1032_VALUE = $ID')
        if status% <> 1% then
          exit sub
        end if
        rec_id% = s1032_value

     end sub


450  sub s1032_delete (status%, ds_index%, rec_id%)
        %include 'tti_run:int_s1032.inc'
        call set_ds (ds_index%)

       !call u11_delete (recnum, ds_index%, ky$, flag%)
       !if recnum = -1 then
         status% = 155%   ! not found
       !else
         status% = 0%
       !end if
  print 'in s1032_delete'
     end sub

550  sub s1032_put (status%, ds_index%, iobuf$)
        %include 'tti_run:int_s1032.inc'
        external long function dm_change_rec
        call set_ds (ds_index%)

        status% = dm_change_rec (iobuf$, s1032_ds_id (ds_index%))

     end sub


650  sub s1032_close (status%, ds_index%)
        %include 'tti_run:int_s1032.inc'
        call set_ds (ds_index%)

        call dm_close_rd (s1032_ds_id (ds_index%))
        status% = dm_cmd ('CLOSE')

 680  end sub



730  sub s1032_getnext (status%, ds_index%, iobuf$, rec_id%)
        %include 'tti_run:int_s1032.inc'
        external long function dm_read_rec

        call set_ds (ds_index%)
        status% = dm_cmd ('GETRECORD;', &
                          'LET S1032_VALUE = $ID')
        rec_id% = s1032_value
        if status% <> 1% then
          exit sub
        end if
        if ds_mode%(ds_index%) = mode_outin then
          status% = dm_cmd ('LOCK ON RECORD')
          if status% <> 1% then
            exit sub
          end if
        end if
        status% = dm_read_rec (iobuf$ by desc, s1032_ds_id (ds_index%))
        if status% <> 1% then
          exit sub
        end if

     end sub


760  sub s1032_getcur (status%, ds_index%, iobuf$, rec_id%)
        %include 'tti_run:int_s1032.inc'
        external long function dm_read_rec
        call set_ds (ds_index%)
        status% = dm_cmd ('GETREC ' + num1$(rec_id%))
        if status% <> 1% then
          exit sub
        end if

        status% = dm_read_rec (iobuf$ by desc, s1032_ds_id (ds_index%))
        if status% <> 1% then
          exit sub
        end if

        !s1032_status = dm_cmd ('LOCK ON RECORD')

     end sub


850  sub s1032_find (status%, ds_index%, ky$)
        %include 'tti_run:int_s1032.inc'
        call set_ds (ds_index%)

        !@call u11_find (recnum, ds_index%, ky$, flag%)
        !if recnum < 0 then
          status% = 155%
          !exit sub
        !end if
  print 's1032_find'
        status% = 0%
     end sub

900  sub s1032_rewind (status%, ds_index%)
        %include 'tti_run:int_s1032.inc'
        call set_ds (ds_index%)
        s1032_status = dm_cmd ('FIND ALL')
        status% = 0%
     end sub

1000  sub s1032_unlock (status%, ds_index%)
        %include 'tti_run:int_s1032.inc'
        call set_ds (ds_index%)
        s1032_status = dm_cmd ('LOCK OFF RECORD')
        status% = 0%
      end sub

2000  sub s1032_info (status%, ds_index%, result%, opcode%)
        !
        ! Get structure information
        !
        !  1  = record length
        !  2  = number of keys
        !  3  = keysize  (if applicable)
        !  4  = size of CURRENT

        %include 'tti_run:int_s1032.inc'
        call set_ds (ds_index%)
       select opcode%
       case 1%  ! record length
         result% = s1032_rsize (ds_index%)
       case 2%  ! number of keys
         result% = 0%
       case 3%  ! keysize
         result% = 0%
       case 4%
         result% = 4%  ! long_word size in bytes
       end select
       status% = 0%

     end sub


12000  sub get_ds_index (ds_index%, ds_name$)
       !
       ! Get dataset ID given the dataset name
       !
       ! The existing dataset names are scanned.  If this is a new
       ! one, then the new dataset is added.
       !
       ! Returns:  ds_index%  = dataset identifier
       %include 'tti_run:int_s1032.inc'

12020  scan_loop:
        for z%=1% to s1032_ids
          if ds_name$ = ds_names$(z%) then
            found% = true%
            ds_index% = z%
            exit scan_loop
          end if
        next z%
        if not found% then
          s1032_ids = s1032_ids + 1%
          ds_index% = s1032_ids
          ds_names$(ds_index%) = ds_name$
        end if

12099   end sub


12100   sub set_ds (ds_index%)
        !
        ! Make the given dataset the CURRENT one
        ! if it isn't the current one already.
        !
        ! ds_index%  = dataset ID
       %include 'tti_run:int_s1032.inc'

12120   if  ds_index% <> current_ds_index then
          s1032_status = dm_cmd ('set dataset ' + ds_names$(ds_index%))
          call dm_set_rd (s1032_ds_id (ds_index%))
          current_ds_index = ds_index%
        end if

12199   end sub
