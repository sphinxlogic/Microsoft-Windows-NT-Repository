      SUBROUTINE Show_sched 
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **        Show_sched         **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV 
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035-1000
C*          (650) 604-5558
C*
C*     DESCRIPTION :
C*          Show the schedule of jobs to be run
C*
C*     COMMON BLOCKS :
C*
C*     SUBPROGRAM REFERENCES :
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          30-MAR-1995  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *80 pline
      character *4 on, off, resp
c
      on = char(27) // '[7m'
      off= char(27) // '[0m'
      pline = 
     $         on // 'W' // off // 'eek or ' //
     $         on // 'D' // off // 'ay ?  [ D ] '
      lp = length(pline) + 1
      call prompt (pline(1:LP))
      read(5,900) resp
      call left(resp)
      call caps(resp)
c
      if (resp(1:1) .eq. 'W') then     ! Week
         write(6,930)
      else                             ! Day
         call show_day
      endif
c
      write(6,920) char(27), char(27)
      read(5,910,end=100)
100   return
c
900   format(a)
910   format()
920   format(//,' ',a1,
     $ '[1m --- Press <RETURN> to continue. ---',a1,'[0m')
930   format(' Not yet available.')
      END
C
C---END Show_sched
C
      subroutine show_day
      include 'kronos.cmn'
      character *128 temp
      logical on_code, at_code
c
      call clear
      write(6,900) now
      itemp = hour
c
c --- loop over each hour in the day
c
      do 10 i = 0, 23
         write(6,910) i, i+1
c
c  ----  we have to fool at_code into doing the work by 
c           temporarily setting the hour to the loop variable
c
         hour = i
c
c  ----  brute force, check each job!
c
         do 5 j = 1, num_jobs
c
c   -----   letting on_code do the work is OK, since we're checking 
c              today
c
            if (on_code(j)) then
               if (at_code(j)) then
                  call strcpy (temp, entry(j).fptr)
                  write (6,920) temp(1:length(temp))
               endif
            endif
5        continue
10    continue
c
c --- restore "hour" incase used elsewhere in KRON
c
      hour = itemp
      return
900   format(/, 'Kronos job schedule for ',a,//)
910   format('   Jobs scheduled between ',i2.2, ':00 and ',
     $ i2.2,':00')
920   format('      ', a)
      end
c
c---end show_day
c
