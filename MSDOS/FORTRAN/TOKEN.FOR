      program TOKEN
      character*80 StrTok$, P$, Delimiters$, Token$

      read (unit=*,fmt='(a)') p$
      print *, p$

      Delimiters$ = ' ,;:().?' // CHAR(9) // CHAR(34)
      Token$ = StrTok$(P$, Delimiters$)
      DO WHILE (Token$ .ne. char(0))
          PRINT *, Token$
          Token$ = StrTok$(char(0), Delimiters$)
      ENDDO
      end

      Character*80 FUNCTION StrTok$ (Srce$, Delim$)
      Character*80 Srce$, Delim$, SaveStr$                
c     Tokenize a string in a similar manner to C.  The usage is
c     very similar to the C function.
c
c     Input:  Srce$     =   SouRCE string to tokenize. (see usage note)
c             Delim$    =   DELIMiter string.  Used to determine the
c                           beginning/end of each token in a string.
c
c     Output: StrTok$   =   STRing TOKen.  
c     Usage:  a) First Call StrTok$ with the string to tokenize
c                as Srce$, and the delimiter string used to tokenize
c                Scre$ is in Delim$, as follows
c
c     CHARACTER*80 StrTok$,SOURCE$,DELIM$,C$
c          .
c          .
c         
c     SOURCE$='This is a test. I hope that it Works! "eh" '
c     DELIM$=' ,.;:"{}()!@#$%^&*'
c     C$=StrTok$(SOURCE$,DELIM$)
c     PRINT *,C$,' is the first token'
c     DO WHILE (C$ .ne. char(0))
c         PRINT *, C$
c         C$ = StrTok$(char(0), DELIM$)
c     ENDDO
c       
c
c
      integer Start_, Ln_, BegPos_, EndPos_
      common /strtk/ Start_, SaveStr$

      IF (Srce$(1:1) .ne. char(0)) THEN
          Start_ = 1
          SaveStr$ = Srce$
      ENDIF

      BegPos_ = Start_
      Ln_ = LEN(SaveStr$)
5     continue
      if ( (BegPos_ .le. Ln_) .AND. 
     &( index(Delim$,SaveStr$(BegPos_:BegPos_)) .ne. 0)) then
          BegPos_ = BegPos_ + 1
          goto 5
      endif
      IF (BegPos_ .gt. Ln_) THEN
          StrTok$ = char(0)
          return
      ENDIF
      EndPos_ = BegPos_
10    continue
      if ((EndPos_ .le. Ln_) .AND.
     &   (index(Delim$,SaveStr$(EndPos_:EndPos_)) .eq. 0)) then
          EndPos_ = EndPos_ + 1
          goto 10
      endif
      StrTok$ = SaveStr$(BegPos_:EndPos_)
      Start_ = EndPos_ 
      END 

