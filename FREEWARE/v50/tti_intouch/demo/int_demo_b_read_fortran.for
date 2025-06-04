      program for5
!5. A program to read records from a sequential file:
      integer r
      structure /test_file_record/
         character name*30
         character sex*1
         character bdate*6
         character street*30
         character city*12
         character zip*9
         character ethnic*1
         character parent*24
         character phone*10
         character bphone*10
         character ephone*10
         character balance*8
         byte %fill(49)
      end structure
      record /test_file_record/ rec
      open (unit=1,file='sys$scratch:tti_vendor.tmp',status='old',
     &   readonly,shared)
      i=0
  100 continue
      read (unit=1,fmt=140,end=200) rec.name,rec.sex,rec.bdate,
     &   rec.street,rec.city,rec.zip,rec.ethnic,rec.parent,
     &   rec.phone,rec.bphone,rec.ephone,rec.balance
  140 format (a30,a1,a6,a30,a12,a9,a1,a24,3a10,a8)
      i=i+1
      r=r+1
      if (r.eq.25) then
         r=0
         print *,'On record:',i
      end if
      go to 100

  200 print *,'Records read:',i
      end
