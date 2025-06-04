      PROGRAM Checksum 
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **         Checksum          **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV (Internet)
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035-1000
C*          (415) 604-5558
C*
C*     DESCRIPTION :
C*          This program reads a list of files from the standard input
C*          and calculates a checksum of these files using two different
C*          methods.  A file with three columns (checksum1, checksum2,
C*          filename) is created for comparison with last week's.
C*
C*     SUBPROGRAM REFERENCES :
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          21-AUG-1995  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *255 inspec, outspec
      logical is_made
      common /table/ crc_table(16,2), i_crc, i_auto
      integer crc_table
c
c --- initialize crc tables
c
      i_crc_16 = '120001'O
      i_autodin = 'edb88320'X
      call lib$crc_table (i_crc_16, crc_table(1,1))
      call lib$crc_table (i_autodin, crc_table(1,2))
      is_made = .false.
c
c --- read the next file spec (wildcards allowed)
c
10    read(5,900,end=100,err=1000) inspec
c
c --- in case of wildcards, loop until all processed
c
20    call getfile (inspec, outspec)
      if (outspec .ne. ' ') then
         call checks (outspec)
         if (.not. is_made) then
            is_made = .true.
            OPEN (UNIT=1, FILE='CHECKSUM.OUT', STATUS='NEW',
     $       carriagecontrol='LIST',  ERR=1000)
         endif
         write (1,910) i_crc, i_auto, outspec(1:length(outspec))
      else
         go to 10
      endif
      go to 20
c
c --- if we get here, all done
c
100   write (6,*) 'Normal end of processing.'
      if (is_made) then
         close(1)
      else
         write(6,*) 'No files processed!!!'
      endif
      stop
c
c --- oh no! 
c
1000  write (6,*) 'Error reading list of file specs.'
      stop
900   format(a)
910   format(' ',i6,2x,i12,2x,a)
      end
C
C---END Checksum
C
      SUBROUTINE Checks (infile)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          Checks           **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     AUTHOR :
C*          Arthur E. Ragosta  
C*          RAGOSTA@MERLIN.ARC.NASA.GOV (Internet)
C*          
C*          MS 219-1
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035-1000
C*          (415) 604-5558
C*
C*     DESCRIPTION :
C*
C*     INPUT ARGUMENTS :
C*          infile - input file spec
C*
C*     OUTPUT ARGUMENTS :
C*          i_crc  - checksum using crc 16
C*          i_auto - checksum using autodin
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
C*          21-AUG-1995  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *(*) infile
      external myopen
c
ccc      write(6,*) 'Processing ' // infile(1:length(infile))
      OPEN (UNIT=2, FILE=infile(1:length(infile)), STATUS='OLD',
     $  useropen=myopen, ERR=100)
c
ccc      write(6,*) 'Done ' // infile(1:length(infile))
      close(2,err=200)
      return
c
100   i_crc = 0
      i_auto = 0
200   return
      end
c
c---end checks
c
      integer function myopen (fab,rab,lun)
      include '($fabdef)'
      include '($rabdef)'
      include '($syssrvnam)'
      record /fabdef/ fab
      record /rabdef/ rab
      character *512 buff
      common /table/ crc_table(16,2), i_crc, i_auto
      integer crc_table
c
      i_crc  = 0
      i_auto = 0
      myopen = 1
c
c --- set block mode
c
      fab.fab$b_fac = fab.fab$b_fac .or. fab$m_bio
c
      istat = sys$open(fab)
      if (istat) then
         istat = sys$connect(rab)
         if (.not. istat) then
            i_crc  = istat
            i_auto = 0
            myopen = istat
            return
         endif
      else
         i_crc  = istat
         i_auto = 0
         myopen = istat
         return
      endif
c
c --- num bytes to read/buffer
c
      rab.rab$w_usz = 512
      rab.rab$l_ubf = %loc (buff)
c
c --- read blocks
c
10    istat = sys$read(rab)
      if (.not. istat) then
c        if (istat .ne. rms$_eof) ???
         return
      endif
      i_crc  = lib$crc (crc_table(1,1), i_crc, buff)
      i_auto = lib$crc (crc_table(1,2), i_auto, buff)
      go to 10
c
      end
c
c---end myopen
c
