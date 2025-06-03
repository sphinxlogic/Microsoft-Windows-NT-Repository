      PROGRAM Gen_Calendar 
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **       Gen_Calendar        **
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
C*          Generates the calendar files to be read by Kronos.
C*          See the commentary in file CALENDAR.INPUT for rules.
C*
C*     SUBPROGRAM REFERENCES :
C*          INIT,  GETLINE,  DO_LINE
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          Some holidays can't be accurately calculated.
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           1-OCT-1994  -  INITIAL VERSION
C*           9-AUG-1995  -  Modified for automated running
C*
C***********************************************************************
C*
      CHARACTER *132 LINE
      character *10 p(2), q(2)
      LOGICAL EOF
      INTEGER YEAR
C
      call getfor (nq, q, np, p)
      if (np .lt. 1) then
         year = -1
      else
         call right(p(1))
         read(p(1),900,err=100) year
      endif
      CALL INIT (YEAR)
      EOF = .FALSE.
C
C --- EACH LINE IS INDEPENDENT OF THE REST... JUST DO IT
C
10    CALL GETLINE (LINE, LL, EOF)       
      IF (LL .GT. 0) CALL DO_LINE (YEAR, LINE, LL)
      IF (.NOT. EOF) GO TO 10
C
      CLOSE(90)
      CLOSE(91)
      STOP
100   call exit (3)
900   format(i10)
      END
C
C---END Gen_Calendar
C
      SUBROUTINE INIT (IYEAR)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **           INIT            **
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
C*          Intializes the files and determines what year to
C*          create.
C*
C*     INPUT ARGUMENTS :
C*          IYEAR - the year we are creating
C*
C*     SUBPROGRAM REFERENCES :
C*          DATE,  PROMPT,  RIGHT,  PARSE,  EXIT
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           1-OCT-1994  -  INITIAL VERSION
C*
C***********************************************************************
C*
      CHARACTER *80 OUTFILE, INFILE
      CHARACTER *4 DEF, TEMP
      CHARACTER *9 DAT
C
      INFILE = 'CALENDAR.INPUT'
C
C --- Determine which output file to create
C     (e.g., CALENDAR.1999)
C
      if (iyear .eq. -1) then
         CALL DATE (DAT)
         IF (DAT(8:8) .EQ. '9') THEN
            DEF = '19' // DAT(8:9) 
         ELSE
            DEF = '20' // DAT(8:9) 
         ENDIF
         READ (DEF,900) IYEAR
         IYEAR = IYEAR + 1
      endif
c
      IF (IYEAR .LT. 1900) THEN
         IF (IYEAR .GT. 90) THEN
            IYEAR = IYEAR + 1900
         ELSE
            IYEAR = IYEAR + 2000
         ENDIF
      ENDIF
      WRITE(OUTFILE(2:),900) IYEAR
      OUTFILE(1:1) = '.'
      CALL PARSE (OUTFILE, 'KRONOS_DIR:CALENDAR.2000', 'FULL', OUTFILE)
C
C --- Make sure input exists and create output
C
      OPEN (UNIT=90, FILE=INFILE, STATUS='OLD', ERR=1000)
      OPEN (UNIT=91, FILE=OUTFILE, STATUS='NEW', CARRIAGECONTROL='LIST',
     $ ERR=1001)
      RETURN
1000  WRITE(6,*) 'CALENDAR.INPUT doesn''t exist.'
      CALL EXIT
1001  WRITE (6,*) 'Unable to create ' // outfile 
      CALL EXIT
900   FORMAT(I4)
910   FORMAT(A4)
      END
C
C---END INIT
C
      SUBROUTINE GETLINE (LINE, LL, EOF)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          GETLINE          **
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
C*          Read lines from input file.  Strip comments.
C*
C*     OUTPUT ARGUMENTS :
C*          LINE - the line, minus comments
C*          LL - length of LINE
c*          EOF - set .true. if all lines have been read
C*
C*     SUBPROGRAM REFERENCES :
C*          LEFT,  LENGTH,  STRIP_EXC
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          Note that LINE is not capitalized!
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           1-OCT-1994  -  INITIAL VERSION
C*
C***********************************************************************
C*
      CHARACTER *(*) LINE
      LOGICAL EOF
C
      READ(90,900,END=100) LINE
      CALL LEFT(LINE)
c
c --- Double comments are passed to output file.
c
      IF (LINE(1:2) .EQ. '!!') WRITE(91,900) LINE(2:length(line))
c
c --- remove trailing comments
c
      CALL STRIP_EXC (LINE)
      LL = LENGTH(LINE)
      RETURN
c
100   EOF = .TRUE.
      LL = 0
      RETURN
900   FORMAT(A)
      END
C
C---END GETLINE
C
      SUBROUTINE DO_LINE (IYEAR, LINE, LL)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          DO_LINE          **
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
C*          Parse the line.  Determine if it's a HOLIDAY, REMINDER, or
C*          one time job.  Call appropriate generation routine.
C*
C*     INPUT ARGUMENTS :
C*          IYEAR - the working year
C*          LINE  - line containing directive
C*          LL    - length of LINE
C*
C*     SUBPROGRAM REFERENCES :
C*          LEFT,  GETOKE,  LENGTH,  MUNG_DAY,  DO_HOLIDAY,  DO_JOB
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*           1-OCT-1994  -  INITIAL VERSION
C*
C***********************************************************************
C*
      CHARACTER *(*) LINE
      CHARACTER *80 TOKEN
      CHARACTER *3 DAY
      CHARACTER *1 TYPE
      LOGICAL ERROR
C
      CALL LEFT(LINE)
C
C --- FORMATS ARE:
C ---   MM/DD H "DESCRIP"
C ---   MM/DD JOB
C ---   MM/WW/DAY H "DESCRIP" 
C ---   MM/WW/DAY JOB
C
      NWEEK = 0
      IPTR = 0
      ERROR = .FALSE.
C
C -- GET MONTH
C
      CALL GETOKE (LINE, LL, IPTR, TOKEN, TYPE, ERROR)
      IF (ERROR) GO TO 1000
      IF (LENGTH(TOKEN) .EQ. 1) TOKEN = '0' // TOKEN
      READ(TOKEN,900,ERR=1000) MONTH
C
C --- SEPERATOR
C
      CALL GETOKE (LINE, LL, IPTR, TOKEN, TYPE, ERROR)
      IF (ERROR) GO TO 1000
C
C --- DAY OR WEEK
C
      CALL GETOKE (LINE, LL, IPTR, TOKEN, TYPE, ERROR)
      IF (ERROR) GO TO 1000
      IF (LENGTH(TOKEN) .EQ. 1) TOKEN = '0' // TOKEN
      READ(TOKEN,900,ERR=1000) NDAY
C
C --- SEPERATOR OR HOLIDAY FLAG
C
      CALL GETOKE (LINE, LL, IPTR, TOKEN, TYPE, ERROR)
      IF (ERROR) GO TO 1000
C
C --- IF THE DAY OF THE MONTH MUST BE CALCULATED, DO IT NOW...
C ---  THEN IT LOOKS JUST LIKE EVERYTHING ELSE
C
      IF (TOKEN .EQ. '/') THEN
         NWEEK = NDAY
         CALL GETOKE (LINE, LL, IPTR, TOKEN, TYPE, ERROR)
         IF (ERROR) GO TO 1000
         DAY = TOKEN
         CALL MUNG_DAY (IYEAR, DAY, MONTH, NWEEK, NDAY)
         CALL GETOKE (LINE, LL, IPTR, TOKEN, TYPE, ERROR)
         IF (ERROR) GO TO 1000
      ENDIF
C
      IF ((TOKEN .EQ. 'H') .OR. (TOKEN .EQ. 'h') .OR.
     $    (TOKEN .EQ. 'R') .OR. (TOKEN .EQ. 'r')) THEN  ! HOLIDAY
         CALL DO_HOLIDAY (iyear, MONTH, NDAY, TOKEN, IPTR, LINE)
      ELSE
         CALL DO_JOB (MONTH, NDAY, TOKEN, IPTR, LINE)
      ENDIF
      RETURN
C
1000  WRITE(6,*) 'ERROR IN DO_LINE'
      RETURN
900   FORMAT(I2)
      END
C
C---END DO_LINE
C
      SUBROUTINE MUNG_DAY (IYEAR, DAY, MONTH, NWEEK, NDAY)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **         MUNG_DAY          **
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
C*        CALCULATE DAY OF MONTH BASED ON 'DAY' AND 'NWEEK'
C*        (E.G., MONDAY OF THE THIRD WEEK).
C*
C*     INPUT ARGUMENTS :
C*          IYEAR - WORKING YEAR
C*          DAY   - DAY OF WEEK (C3)
C*          MONTH - THE WORKING MONTH
C*          NWEEK - WHICH WEEK OF THE MONTH
C*
C*     OUTPUT ARGUMENTS :
C*          NDAY  - NUMERICAL DAY OF THE MONTH
C*
C*     SUBPROGRAM REFERENCES :
C*          CAPS, WEKDAY,
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          14-NOV-1994  -  INITIAL VERSION
C*
C***********************************************************************
C*
      CHARACTER *23 DATE
      CHARACTER *9 WDAY
      CHARACTER *3 DAY, MONTHS(12)
      LOGICAL LEAP_YEAR
      DATA MONTHS / 'JAN', 'FEB', 'MAR', 'APR', 'MAY', 'JUN',
     $              'JUL', 'AUG', 'SEP', 'OCT', 'NOV', 'DEC'/
      INTEGER MONTH, NDAY, NWEEK
C
      CALL CAPS (DAY)
C
C --- FROM START OF MONTH
C
      IF (NWEEK .GT. 0) THEN
C
C --- GET WEEKDAY OF THE FIRST OF THE MONTH
C
         NDAY = 1
10       WRITE (DATE,900) NDAY, MONTHS(MONTH), IYEAR
         CALL WEKDAY (DATE, WDAY)
         IF (WDAY(1:3) .NE. DAY) THEN
            NDAY = NDAY + 1
            GO TO 10
         ENDIF
         NDAY = NDAY + (NWEEK -1) * 7
      ELSE
C
C --- FROM END OF MONTH
C
         NDAY = LAST_DAY (MONTHS(MONTH))
         IF ((MONTH .EQ.2) .AND. LEAP_YEAR(IYEAR)) NDAY = NDAY + 1
         NWEEK = -NWEEK
20       WRITE (DATE,900) NDAY, MONTHS(MONTH), IYEAR
         CALL WEKDAY (DATE, WDAY)
         IF (WDAY(1:3) .NE. DAY) THEN
            NDAY = NDAY - 1
            GO TO 20
         ENDIF
         NDAY = NDAY - (NWEEK-1) * 7
      ENDIF
      RETURN
900   FORMAT(I2.2,'-',A3,'-',I4,' 00:00:00.00')
      END
C
C---END MUNG_DAY
C
      SUBROUTINE DO_HOLIDAY (iyear, MONTH, NDAY, TOKEN, IPTR, LINE)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **        DO_HOLIDAY         **
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
C*          FOR A HOLIDAY OR REMINDER, OUTPUT THE ENTRY...
C*          CORRECT FOR WEEKENDS, IF NECESSARY
C*
C*     INPUT ARGUMENTS :
C*          IYEAR - WORKING YEAR
C*          MONTH - WORKING MONTH
C*          NDAY  - DAY OF THE MONTH
C*          TOKEN - MOST RECENTLY READ TOKEN FROM THE INPUT LINE.
C*                   (= "H" OR "R" FOR HOLIDAY)
C*          IPTR  - POINTER INTO "LINE" FOR MORE PARSING
C*          LINE  - INPUT LINE
C*
C*     SUBPROGRAM REFERENCES :
C*          LENGTH, WEKDAY
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          CAN'T MOVE 1ST OF JANUARY (OR DECEMBER 31) IF THAT WOULD
C*          IMPACT ANOTHER YEAR
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          14-NOV-1994  -  INITIAL VERSION
C*
C***********************************************************************
C*
      CHARACTER *(*) TOKEN, LINE
      CHARACTER *80 PARAM
      character *23 d8
      character *7 wday
      character *3 i2m
C
      LL = LENGTH(LINE)
C
C --- HOLIDAY
C
      IF ((TOKEN .EQ. 'H') .OR. (TOKEN .EQ. 'h')) THEN
c
c --- move the date if it's a weekend........................
c
         write(d8,930) nday, i2m(month), iyear
         call wekday (d8, wday)
         if (wday .eq. 'SATURDAY') then
            nday = nday - 1
            if (nday .lt. 1) then
               if (month .eq. 1) then
                  write(6,*) 'January 1st not moved!'
                  nday = 1
               else
                  month = month - 1
                  nday = last_day ( i2m (month)) 
               endif
            endif
         else if (wday .eq. 'SUNDAY') then
            nday = nday + 1
            if (nday .gt. last_day ( i2m (month))) then
               if (month .eq. 12) then
                  write(6,*) 'December 30th not moved.'
                  nday = 30
               else
                  nday = 1
                  month = month + 1
               endif
            endif
         endif
         IF (IPTR .LT. LL) THEN
            WRITE(91,920) MONTH, NDAY, LINE(IPTR:LL)
         ELSE
            WRITE(91,900) MONTH, NDAY
         ENDIF
C
C --- REMINDER
C
      ELSE
         PARAM = '/PARAM=(' // LINE(IPTR:LL)
         LP = LENGTH(PARAM)
         PARAM (LP+1:) = ')'
         if (nday .gt. 2) then
            nday = nday - 2
         else
            if (month .eq. 1) then
               nday = 1    ! Not good, but can't effect previous year
               write(6,*)'Reminder may be late.'
            else
               month = month - 1
               nday = last_day ( i2m (month)) - 2 + nday 
            endif
         endif
         WRITE(91,910) MONTH, NDAY, PARAM(1:LP+1)
      ENDIF
      RETURN
900   FORMAT(I2,'/',I2.2,' HOLIDAY')
910   FORMAT(I2,'/',I2.2,' AT 9:00 KRONOS_ROOT:[MISC]REMINDER ',A)
920   FORMAT(I2,'/',I2.2,' HOLIDAY  !',A)
930   format(i2.2,'-',a3,'-',i4,' 00:00:00.00')
      END
C
C---END DO_HOLIDAY
C
      SUBROUTINE DO_JOB (MONTH, NDAY, TOKEN, IPTR, LINE)
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          DO_JOB           **
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
C*          OUTPUT A ONETIME JOB
C*
C*     INPUT ARGUMENTS :
C*          MONTH - WORKING MONTH
C*          NDAY  - DAY OF THE MONTH
C*          TOKEN - LEFT OVER FROM DETERMINATION OF HOLIDAY STATUS
C*          IPTR  - POINTER TO REST OF LINE
C*          LINE  - ORIGINAL INPUT LINE
C*
C*     SUBPROGRAM REFERENCES :
C*          LENGTH
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          14-NOV-1994  -  INITIAL VERSION
C*
C***********************************************************************
C*
      CHARACTER *(*) TOKEN, LINE
C
      LL = LENGTH(LINE)
      WRITE(91,900) MONTH, NDAY, TOKEN(1:LENGTH(TOKEN)) // ' ' //
     $  LINE(IPTR:LL)
      RETURN
900   FORMAT(I2,'/',I2.2,' ',A)
      END
C
C---END DO_JOB
C
