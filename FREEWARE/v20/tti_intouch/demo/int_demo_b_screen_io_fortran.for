      program int_demo_b_screen_io_fortran
!4. A program to print numbers to the screen, in a loop. 
!   Print as many numbers as possible on a single line:
!   EX: 1 2 3 4 5 6 7 8 9 10 ...
      do 100 i=1,1000
  100 print 200,i
  200 format ('+',i5,$)
      end
