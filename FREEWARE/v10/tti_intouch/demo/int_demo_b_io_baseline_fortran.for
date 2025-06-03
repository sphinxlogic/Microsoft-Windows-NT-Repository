      program for2
! Purpose: Establish performance baseline for opening/closing a file
      open (unit=1,file='sys$scratch:tti_vendor.tmp',status='old')
      close (unit=1)
      end
