      program int_demo_b_write_fortran
! Purpose: To benchmark adding 200 records to an indexed file. 
      integer r
      structure /test_file_record/
         character key*5
         character data*1000
         byte %fill(250)
      end structure
      record /test_file_record/ rec
      print *,'Creating Indexed file...'
      open (unit=1,file='sys$scratch:int_demo_indexed.tmp',
     &   status='new',organization='indexed',access='keyed',
     &   recordtype='fixed',form='unformatted',recl=1255,shared,
     &   key=(1:5:character))
      print *,'Populating file...'                     
      do 100 i=1,1000,5
  100 rec.data(i:i+4)='DATA '
      do 140 i=1,200
         r=r+1
         if (r.eq.25) then
            r=0
            print *,'Records written:', i
         end if
         write (rec.key,fmt=120) i
  120    format (i5) 
         write (unit=1) rec
  140 continue
      close (unit=1)
      end
