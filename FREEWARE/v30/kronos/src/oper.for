      PROGRAM Operator
C*
C*                  *******************************
C*                  *******************************
C*                  **                           **
C*                  **         Operator          **
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
C*          (415) 604-5558
C*
C*     DESCRIPTION :
C*          This program sends a message to the operator's log and 
C*           console.  Message text is first parameter.
C*
C*     SUBPROGRAM REFERENCES :
C*          OPER
C*
C*     ASSUMPTIONS AND RESTRICTIONS :
C*          Not transportable.
C*
C*     LANGUAGE AND COMPILER :
C*          ANSI FORTRAN 77
C*
C*     CHANGE HISTORY :
C*          22-MAR-1991  -  INITIAL VERSION
C*
C***********************************************************************
C*
      character *132 p(2), q(2)
c
      istat = 1
      call getfor (nq, q, np, p)
      if (np .eq. 0) then
         istat = 2
      else
         call oper (p(1)(1:length(p(1))),'CENTRAL')
      endif
      call exit (istat)
      END
C
C---END Operator
C
