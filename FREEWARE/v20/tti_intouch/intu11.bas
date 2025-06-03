1 sub first
   ! INTU11 - USER-11 interface module
   ! Daniel Esbensen
   ! Written Oct 1, 1985
   ! Copyright (c) 1985, TTI
  end sub

150  sub user11_setup (status%, program$, help$, flag%)
        call u11_setup (program$, help$, flag%)
        status% = 0%
     end sub

250  sub user11_open (status%, dbch%, filename$, omode$)
        call u11_opendb (status%, dbch%, filename$, omode$)
     end sub

350  sub user11_add (status%, recnum, dbch%, ky$, flag%)
       call u11_add (recnum, dbch%, ky$, flag%)
       select sgn(recnum)
       case -1
         status% = 134%   ! duplicate key
       case 0
         status% = 4%     ! no room in file
       case 1
         status% = 0%
       end select
     end sub

450  sub user11_delete (status%, recnum, dbch%, ky$, flag%)
       call u11_delete (recnum, dbch%, ky$, flag%)
       if recnum = -1 then
         status% = 155%   ! not found
       else
         status% = 0%
       end if
     end sub

550  sub user11_put (status%, dbch%, recnum, f%, l%, ndata$)
       call u11_put (dbch%, recnum, f%, l%, ndata$)
       call u11_unlock (dbch%)
       status% = 0%
     end sub

650  sub user11_close (status%, dbch%)
        on error goto 670
        call u11_close (dbch%)
        status% = 0%
        exit sub
670     status% = err
        resume 680
680  end sub

730  sub user11_getnext (status%, iobuf$, dbch%, recnum, f%, l%)
        %include "u11_library:u11_diskio_common"

       recnum = recnum + 1.

   rec_scan:
     for rec = recnum to u11_ad_last_active_record(dbch%)
       call u11_get (iobuf$, dbch%, rec, f%, l%)
       if ascii(iobuf$) <> 0% then
         status% = 0%
         recnum = rec
         exit sub
       end if
     next rec
     status% = 11%  ! end of file
     end sub

760  sub user11_getcur (status%, iobuf$, dbch%, recnum, f%, l%)
        %include "u11_library:u11_diskio_common"

       if recnum > u11_ad_last_active_record(dbch%) then
         status% = 11%
         exit sub
       end if

       call u11_get (iobuf$, dbch%, recnum, f%, l%)
       if ascii(iobuf$) = 0% then
         status% = 155%
         exit sub
       end if
       status% = 0%
     end sub

850  sub user11_find (status%, recnum, dbch%, ky$, flag%)
        call u11_find (recnum, dbch%, ky$, flag%)
        if recnum < 0 then
          status% = 155%
          exit sub
        end if
        status% = 0%
     end sub

900  sub user11_rewind (status%, recnum, dbch%)
        recnum = 0
        status% = 0%
     end sub

1000  sub user11_unlock (status%, dbch%)
        call u11_unlock (dbch%)
        status% = 0%
      end sub

2000  sub user11_info (status%, result%, opcode%, dbch%)
        %include "u11_library:u11_diskio_common"

       select opcode%
       case 1%  ! record length
         result% = u11_al_record_size(dbch%)
       case 2%  ! number of keys
         z% = u11_aw_key_size(dbch%) - 3%
         if  z% > 0%  then
           result% = 1%
         else
           result% = 0%
         end if
       case 3%  ! keysize
         z% = u11_aw_key_size(dbch%)
         if  z% > 0%  then
           result% = z% - 3%
         else
           result% = 0%
         end if
       end select
       status% = 0%
     end sub
