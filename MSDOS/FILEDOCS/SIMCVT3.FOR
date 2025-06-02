C  SIMCVT3.FOR
C
C  This is a updated version of SIMCVT2.FOR, the new version does not
C  require to edit SIMIBM.IDX file first.  Make sure the SIMIBM.IDX and
C  SIMCVT3.EXE are in the same diretory.
C  To compile and run on a VAX/VMS machine, do the following:
C                   FORTRAN SIMCVT3
C                   LINK    SIMCVT3
C                   RUN     SIMCVT3
C
C  Please direct any questions or comments to me. I hope you find
C  this program helpful.  **PLEASE READ THE EXPLANATION BY ROGER KINGSLEY
C  BELOW FOR DETAIL**
C
C  Dustin Fu
C  Computer Operator
C  Academic Computing Services
C  University of Texas at Arlington
C  Bitnet: c015fdh@utarlg
C  THEnet: UTARLG::C015FDH
C  Internet: c015fdh@utarlg.uta.edu
C
C  P.S. I like to express my appreciation to Roger Kingsley for his work
C       to perfect this program.  
C
C++ ! RAK
C  This program has been modified so as to eliminate the need for the ! RAK
C  above pre-edit.  The modification involves commenting out one line ! RAK
C  at the beginning of the main loop, and inserting a block of code in ! RAK
C  its place.  All modified lines end with " ! RAK" for easy ! RAK
C  identification. ! RAK
C-- ! RAK
C
C++ ! RAK
C  Modified by ! RAK
C  Roger A. Kingsley ! RAK
C  University Secretary ! RAK
C  The University of Winnipeg ! RAK
C  Winnipeg, Manitoba, Canada ! RAK
C  BitNet:  KINGSLEY@UWPG02 ! RAK
C-- ! RAK
C
      PROGRAM IDX2LST
      CHARACTER BUF1*255, BUF2*255, CBUF1*1 ! RAK
      INTEGER LBUF1,LBUF2 ! RAK
      INTEGER   LGTH2, BITS2, DT2, REV2
      CHARACTER FS1*4, DIR1*20
      CHARACTER FS2*4, DIR2*20, FLNM2*12, DESCR2*46
      CHARACTER DT*9, STYLE*1
C
      FS1 = ' '
      DIR1 = ' '
C
      OPEN(UNIT=1,FILE='SIMIBM.IDX',STATUS='OLD')
      OPEN(UNIT=2,FILE='SIMIBM.LST',STATUS='NEW')
C
      CALL DATE(DT)
      WRITE(2,*) 'WSMR-SIMTEL20.ARMY.MIL PUBLIC DOMAIN LISTING AS OF '
     +           ,DT
      WRITE(2,*) ' '
      WRITE(2,*) 'NOTE: Type B is Binary, Type A is ASCII'
C
C111   READ(1,*,END=999) FS2,DIR2,FLNM2,REV2,LGTH2,BITS2,DT2,DESCR2 ! RAK
C++ ! RAK
C     The above line "111   READ(1,*,END=999) FS2,..." has been ! RAK
C     commented out, and the following block of code inserted, ending ! RAK
C     with the line "      READ (BUF2(1:LBUF2),*) FS2,..." ! RAK
C ! RAK
C     The program has been modified to read a line from SIMIBM.IDX in ! RAK
C     its original format, to make the alterations mentioned above, ! RAK
C     and to re-read the edited line. ! RAK
C-- ! RAK
111   CONTINUE ! RAK
      READ (1,444,END=999) LBUF1,BUF1 ! RAK
444   FORMAT (Q,A) ! RAK
      LBUF2 = 0 ! RAK
      DO I=1,LBUF1 ! RAK
         CBUF1 = BUF1(I:I) ! RAK
         IF (CBUF1.EQ.'"') THEN ! RAK
            LBUF2 = LBUF2+1 ! RAK
            BUF2(LBUF2:LBUF2)='''' ! RAK
         ELSE IF (CBUF1.EQ.'''') THEN ! RAK
            LBUF2=LBUF2+2 ! RAK
            BUF2(LBUF2-1:LBUF2) = '''''' ! RAK
         ELSE ! RAK
            LBUF2=LBUF2+1 ! RAK
            BUF2(LBUF2:LBUF2) = CBUF1 ! RAK
         END IF ! RAK
      END DO ! RAK
      READ(BUF2(1:LBUF2),*) FS2,DIR2,FLNM2,REV2,LGTH2,BITS2,DT2,DESCR2 ! RAK
C++ ! RAK
C     The modifications end here ! RAK
C-- ! RAK
C
      IF ((FS1 .NE. FS2) .OR. (DIR1 .NE. DIR2)) THEN
         WRITE(2,*) ' '
         WRITE(2,*) 'Directory ', FS2, DIR2
         WRITE(2,*) ' Filename   Type Length   Date    Description'
         WRITE(2,*) '=============================================='
         IF (BITS2 .EQ. 8) THEN
             STYLE = 'B'
         ELSE
             STYLE = 'A'
         ENDIF
         WRITE(2,1001) FLNM2, STYLE, LGTH2, DT2, DESCR2
         FS1 = FS2
         DIR1 = DIR2
      ELSE
         IF (BITS2 .EQ. 8) THEN
             STYLE = 'B'
         ELSE
             STYLE = 'A'
         ENDIF
         WRITE(2,1001) FLNM2, STYLE, LGTH2, DT2, DESCR2
         FS1 = FS2
         DIR1 = DIR2
      ENDIF
      GOTO 111
1001  FORMAT(1X, A, 2X, A, I8, I8, 2X, A)
999   CLOSE(UNIT=1)
      CLOSE(UNIT=2)
      STOP
      END
