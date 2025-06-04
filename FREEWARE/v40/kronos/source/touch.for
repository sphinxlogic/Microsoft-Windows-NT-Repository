      PROGRAM touch_MAIN
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **          Touch            **
C*                  **                           **
C*                  *******************************
C*                  *******************************
C*
C*     PROGRAM :
C*          TOUCH
C*
C*     AUTHOR :
C*          Arthur E. Ragosta
C*          RAGOSTA%MRL.DECNET@AMES.ARC.NASA.GOV
C*          MS 219-3
C*          NASA Ames Research Center
C*          Moffett Field, Ca.  94035
C*          (415) 604-5558
C*
C*     PURPOSE :
C*          TOUCH MODIFIES ONE OR MORE FILES REVISION DATES.
C*
C*     QUALIFIERS :
C*          /EXCLUDE= - LIST OF FILESPECS TO BE BYPASSED
C*
C*     PARAMETERS :
C*          FILESPEC  - LIST OF FILESPECS TO BE PROCESSED
C*
C*     SUBPROGRAM REFERENCES :
C*          TOUCH
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          2-MAY-1990  -  INTIAL VERSION
C*          6-FEB-1991  -  EXCESSIVE INITIALIZATION CODE REMOVED
C*
C***********************************************************************
C*
      CHARACTER *80 P(5), Q(5), FNAME
      CHARACTER *127 FILES, ONEFILE, DEFILE, EXFILE
C
      LOGICAL MATCHD, AMBIG, LIST
C
C --- THE QUALIFIER LIST MUST BE ALPHABETICAL !!!!
C
cc      PARAMETER (NQUALS=1)
cc      CHARACTER *20 QUALS(NQUALS)
cc      DATA QUALS/ 'EXCLUDE' /
C
C --- GET USER PARAMETERS AND QUALIFIERS
C
      CALL GETFOR (NQ, Q, NP, P)
C
C --- IF THE USER DIDN'T ENTER A FILE LIST ON THE COMMAND LINE, ASK
C ---  FOR IT
C
      IF (NP .EQ. 0) THEN
         WRITE(6,900)
         READ(5,910) P(1)
      ENDIF
C
C --- MAYBE HE CHANGED HIS MIND?
C
      IF (P(1) .EQ. ' ') THEN
         WRITE(6,920)
         CALL EXIT
      ENDIF
C
      CALL LEFT(P(1))
      FILES = P(1)
      EXFILE = ' '
      DEFILE = ' '
C
      IPTR = 1
C
C --- OK, WE HAVE THE FILE LISTS NOW
C
C --- LET'S CHECK OUT THE QUALIFIERS
C
      IF (NQ .GE. 1) THEN
         IF (Q(1)(1:3) .EQ. 'EXC') then
            l = index(q(1),'=')
            if (l .gt. 0) EXFILE = Q(1)(L+1:)
         endif
      ENDIF
C
C$$
C$$  TOOL INITIALIZATION CODE HERE
C$$
C
C
C --- OK, NOW LET THE TOOL DO ITS STUFF
C
C
C --- EXTRACT FILE NAMES ONE AT A TIME
C
100   CALL GETFILEX ( FILES, ONEFILE, DEFILE, EXFILE )
C
C --- GENERATE NAME OF OUTPUT FILE FOR THIS INPUT FILE
C
      IF (ONEFILE .NE. ' ') THEN
         CALL TOUCH ( ONEFILE )
         GO TO 100
      ENDIF
C$$
C$$  TOOL FINALIZATION CODE HERE
C$$
      CALL EXIT
C
900   FORMAT(' File ? ',$)
910   FORMAT(A20)
920   FORMAT(' No file specified.')
      END
