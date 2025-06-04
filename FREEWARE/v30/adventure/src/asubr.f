C Adventure - subroutines
C
C 30-Aug-93	RMS	Revised for portability.
C
	SUBROUTINE VOCAB(ID1,INIT,V)
C
C Look up ID1 in the vocabulary (ATAB) and return its "definition" (KTAB),
C or -1 if not found.  If INIT is positive, this is an init call setting
C up a keyword variable, and not finding it constitutes a bug.  It also means
C that only ktab values which taken over 1000 equal INIT may be considered.
C (thus "steps", which is a motion verb as well as an object, may be located
C as an object.)  And it also means the KTAB value is taken mod 1000.
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
	CHARACTER*(*) ID1
C
	DO 1 I = 1,TABSIZ
	IF(KTAB(I).EQ.-1) GOTO 2
	IF(INIT.GE.0 .AND. KTAB(I)/1000.NE.INIT) GOTO 1
	IF(ATAB(I).EQ.ID1) GOTO 3
1	CONTINUE
	CALL BUG(21)
C
2	V = -1
	IF(INIT.LT.0) RETURN
	TYPE 100,ID1
100	FORMAT(' Keyword = ',A)
	CALL BUG(5)
C
3	V = KTAB(I)
	IF(INIT.GE.0) V = MOD(V,1000)
	RETURN
	END

	SUBROUTINE DSTROY(OBJECT)
C
C Permanently eliminate OBJECT by moving to a non-existent location.
C
	IMPLICIT INTEGER (A-Z)
C
	CALL MOVE(OBJECT,0)
	RETURN
	END

	SUBROUTINE JUGGLE(OBJECT)
C
C Juggle OBJECT by picking it up and putting it down again, the purpose
C being to get the object to the front of the chain of things at its loc.
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	I = PLACE(OBJECT)
	J = FIXED(OBJECT)
	CALL MOVE(OBJECT,I)
	CALL MOVE(OBJECT+OBJMAX,J)
	RETURN
	END

	SUBROUTINE MOVE(OBJECT,WHERE)
C
C Place any OBJECT anyWHERE by picking it up and dropping it.  May already be
C toting, in which case the carry is a no-op.  Mustn't pick up objects which
c are not at any loc, since carry wants to remove objects from ATLOC chains.
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	IF(OBJECT.GT.OBJMAX) GOTO 1
	FROM = PLACE(OBJECT)
	GOTO 2
1	FROM = FIXED(OBJECT-OBJMAX)
2	IF(FROM.GT.0. AND. FROM.LE.300) CALL CARRY(OBJECT,FROM)
	CALL DROP(OBJECT,WHERE)
	RETURN
	END

	INTEGER FUNCTION PUT(OBJECT,WHERE,PVAL)
C
C PUT is the same as MOVE, except it returns a value used to set up the
C negated PROP values for the repository objects.
C
	IMPLICIT INTEGER (A-Z)
C
	CALL MOVE(OBJECT,WHERE)
	PUT = (-1)-PVAL
	RETURN
	END

	SUBROUTINE CARRY(OBJECT,WHERE)
C
C Start toting OBJECT, removing it from the list of things at its former
C location.  Incr HOLDNG unless it was already being toted.  If OBJECT>OBJMAX
C (moving "fixed" second loc), don't change PLACE or HOLDNG.
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	IF(OBJECT.GT.OBJMAX) GOTO 5
	IF(PLACE(OBJECT).EQ.-1) RETURN
	PLACE(OBJECT) = -1
	HOLDNG = HOLDNG+1
5	IF(ATLOC(WHERE).NE.OBJECT) GOTO 6
	ATLOC(WHERE) = LINK(OBJECT)
	RETURN
6	TEMP = ATLOC(WHERE)
7	IF(LINK(TEMP).EQ.OBJECT) GOTO 8
	TEMP = LINK(TEMP)
	GOTO 7
8	LINK(TEMP) = LINK(OBJECT)
	RETURN
	END

	SUBROUTINE DROP(OBJECT,WHERE)
C
C Place OBJECT at a given loc, prefixing it onto the ATLOC list.  Decrement
C HOLDNG if the object was being toted.
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	IF(OBJECT.GT.OBJMAX) GOTO 1
	IF(PLACE(OBJECT).EQ.-1) HOLDNG = HOLDNG-1
	PLACE(OBJECT) = WHERE
	GOTO 2
1	FIXED(OBJECT-OBJMAX) = WHERE
2	IF(WHERE.LE.0) RETURN
	LINK(OBJECT) = ATLOC(WHERE)
	ATLOC(WHERE) = OBJECT
	RETURN
	END

	SUBROUTINE BUG(NUM)
	IMPLICIT INTEGER (A-Z)
C
C The following conditions are currently considered fatal bugs.  Numbers < 20
C are detected while reading the database; the others occur at run time.
C	0	MESSAGE LINE > 70 CHARACTERS
C	1	NULL LINE IN MESSAGE
C	2	TOO MANY WORDS OF MESSAGES
C	3	TOO MANY TRAVEL OPTIONS
C	4	TOO MANY VOCABULARY WORDS
C	5	REQUIRED VOCABULARY WORD NOT FOUND
C	6	TOO MANY RTEXT OR MTEXT MESSAGES
C	7	TOO MANY HINTS
C	8	LOCATION HAS COND BIT BEING SET TWICE
C	9	INVALID SECTION NUMBER IN DATABASE
C	10	INVALID ACTSPK (VERB) NUMBER
C	11	TOO MANY LOCATION (LTEXT OR STEXT) ENTRIES
C	12	TOO MANY PLAYER CLASSES
C	13	INVALID OBJECT NUMBER
C	20	SPECIAL TRAVEL (500>L>300) EXCEEDS GOTO LIST
C	21	RAN OFF END OF VOCABULARY TABLE
C	22	VOCABULARY TYPE (N/1000) NOT BETWEEN 0 AND 3
C	23	INTRANSITIVE ACTION VERB EXCEEDS GOTO LIST
C	24	TRANSITIVE ACTION VERB EXCEEDS GOTO LIST
C	25	CONDITIONAL TRAVEL ENTRY WITH NO ALTERNATIVE
C	26	LOCATION HAS NO TRAVEL ENTRIES
C	27	HINT NUMBER EXCEEDS GOTO LIST
C	28	INVALID MONTH RETURNED BY DATE FUNCTION
C
	TYPE 1, NUM
1	FORMAT (/' Fatal error',I3,'.  Consult your local wizard.'/)
	CALL EXIT
	END
C
	SUBROUTINE INIRND(I,J)
	IMPLICIT INTEGER(A-Z)
	COMMON /SEED/ RNSEED
C
	RNSEED = (ISHFT(I,16) + J) .OR. 1
	RETURN
	END
C
	INTEGER FUNCTION RND(RANGE)
	IMPLICIT INTEGER(A-Z)
	REAL RAN
	COMMON /SEED/ RNSEED
C
	RND = RAN(RNSEED) * FLOAT(RANGE)
	RETURN
	END

	SUBROUTINE SPEAK(N)
C
C Print the message in record n of the random message file.
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	IF(N.EQ.0) RETURN
	IF(TLINES(N)(1:3) .EQ. '>$<') RETURN
	TEMP = N
1	OLDRCN = TRECNO(TEMP)
	TYPE 2,TLINES(TEMP)(1:NBLEN(TLINES(TEMP)))
2	FORMAT(' ',A)
	TEMP = TEMP+1
	IF(TRECNO(TEMP) .EQ. OLDRCN) GO TO 1
	RETURN
	END

	SUBROUTINE PSPEAK(MSG,SKIP)
C
C Find the SKIP+1st message for object msg and print it.
C MSG should be the index of the object.
C (INVEN+N+1 message is PROP = N message).
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	M = PTEXT(MSG)
	IF(SKIP.LT.0) GOTO 9
	OLDRCN = MSG
	DO 3 I = 1,SKIP+1
1	M = M+1
	IF(TRECNO(M-1).EQ.OLDRCN) GO TO 1
	OLDRCN = TRECNO(M-1)
3	CONTINUE
	M = M-1
9	CALL SPEAK(M)
	RETURN
	END

	SUBROUTINE RSPEAK(MSG)
C
C Print the i-th "random" message (section 6 of database).
C
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	IF(MSG.NE.0) CALL SPEAK(RTEXT(MSG))
	RETURN
	END

	SUBROUTINE GETIN(WORD1,WORD2)
C
C Get a command from the adventurer.  Snarf out the first word, pad it with
C Blanks, and return it in WORD1.  If a second word appears, it is returned
C in WORD2, else WORD2 is set to blanks.
C
	IMPLICIT INTEGER (A-Z)
	CHARACTER*72 FRST
	CHARACTER*8 WORD1,WORD2
	CHARACTER*1 J
C
	TYPE 1
1	FORMAT(' >',$)
2	ACCEPT 3,FRST
3	FORMAT(A)
C
	LUCVT = ICHAR('A')-ICHAR('a')
	DO 5 I = 1,72
	IF((FRST(I:I).GE.'a') .AND. (FRST(I:I).LE.'z'))
	1	FRST(I:I) = CHAR(ICHAR(FRST(I:I))+LUCVT)
5	CONTINUE
	INLEN = NBLEN(FRST)
	IF(INLEN.LE.0) GO TO 2
	WORD1 = ' '
	WORD2 = ' '
	I = 1					! input ptr.
	OP = 0					! output ptr.
C
50      OP = OP+1				! adv output ptr.
	CP = 0					! char ptr = 0.
C
200     IF(I.GT.INLEN) GO TO 2000		! end of input?
	J = FRST(I:I)				! no, get character
	I = I+1					! advance ptr.
	IF(J.EQ.' ') GO TO 1000                 ! space?
	CP = CP+1				! adv char ptr.
	IF(CP.GT.8) GO TO 200			! too much input?
	IF(OP.EQ.1) WORD1(CP:CP) = J		! insert char in word1
	IF(OP.EQ.2) WORD2(CP:CP) = J		! insert char in word2
	GO TO 200
C
C Space.
C
1000    IF(CP.EQ.0) GO TO 200                   ! any word yet?
	GO TO 50                                ! yes, adv op.
C
C End of input, see if partial word available.
C
2000	IF(CP.EQ.0) OP = OP-1			! any last word?
	RETURN					! done
C
	END

	LOGICAL FUNCTION YES(X,Y,Z)
C
C Print message X, wait for yes/no answer.  If yes, print Y and return
C TRUE; if no, print Z and return FALSE.
C
	IMPLICIT INTEGER (A-Z)
	CHARACTER*8 REPLY,JUNK1
C
1	IF(X.NE.0) CALL RSPEAK(X)
	CALL GETIN(REPLY,JUNK1)
	IF(REPLY.EQ.'Y' .OR. REPLY.EQ.'YE' .OR. REPLY.EQ.'YES') GOTO 10
	IF(REPLY.EQ.'N' .OR. REPLY.EQ.'NO') GOTO 20
	TYPE 9
9	FORMAT(' Please answer the question.')
	GOTO 1
10	YES = .TRUE.
	IF(Y.NE.0) CALL RSPEAK(Y)
	RETURN
20	YES = .FALSE.
	IF(Z.NE.0) CALL RSPEAK(Z)
	RETURN
C
	END

        INTEGER FUNCTION NBLEN(STRING)
C
C Compute string length without trailing blanks
C
        IMPLICIT INTEGER(A-Z)
        CHARACTER*(*) STRING
C
        NBLEN = LEN(STRING)			! get nominal length
100     IF(NBLEN.LE.0) RETURN                   ! any string left?
        IF(STRING(NBLEN:NBLEN).NE.' ') RETURN   ! found a non-blank?
        NBLEN = NBLEN-1				! no, trim len by 1
        GO TO 100                               ! and continue.
C
        END

	SUBROUTINE SAVEGM(F2)
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	OPEN (UNIT=1,FILE='ASAVE',ACCESS='SEQUENTIAL',
	1	STATUS='UNKNOWN',FORM='UNFORMATTED',ERR=200)
C
	WRITE(1) VMAJ,VMIN,VEDIT
	WRITE(1) RTEXT,KTAB,ATAB
	WRITE(1) ATLOC,LINK,PLACE,FIXED,HOLDNG
	WRITE(1) PTEXT,ABB,LINUSE,TRVS,CLSSES
	WRITE(1) OLDLOC,LOC,CVAL,NEWLOC,KEY
	WRITE(1) PLAC,FIXD,ACTSPK,COND,HINTS
	WRITE(1) HNTMAX,PROP,TALLY,TALLY2,HINTLC
	WRITE(1) CHLOC,CHLOC2,DSEEN,DFLAG,DLOC,DALTLC
	WRITE(1) TURNS,LMWARN,KNFLOC,DETAIL,ABBNUM
	WRITE(1) NUMDIE,MAXDIE,DKILL,FOOBAR,BONUS
	WRITE(1) CLOCK1,CLOCK2,CLOSNG,PANIC,CLOSED
	WRITE(1) GAVEUP,SCORNG,ODLOC,CTEXT,STEXT,LTEXT
	WRITE(1) TRAVEL,TRVCON,TRVLOC,MAXTRS,HINTED
C
	TYPE 100
100	FORMAT(' Saved.')
	CLOSE (UNIT=1)
	F2=0
	RETURN
C
200	TYPE 300
300	FORMAT(' Open error, SAVE fails.')
	F2=-1
	RETURN
	END

	SUBROUTINE RSTRGM(F2)
	IMPLICIT INTEGER (A-Z)
	INCLUDE 'aparam.for'
C
	OPEN (UNIT=1,FILE='ASAVE',ACCESS='SEQUENTIAL',
	1	STATUS='OLD',READONLY,FORM='UNFORMATTED',ERR=200)
C
	READ(1) I1,I2,I3
	IF((I1.NE.VMAJ).OR.(I2.NE.VMIN)) GO TO 50
C
	READ(1) RTEXT,KTAB,ATAB
	READ(1) ATLOC,LINK,PLACE,FIXED,HOLDNG
	READ(1) PTEXT,ABB,LINUSE,TRVS,CLSSES
	READ(1) OLDLOC,LOC,CVAL,NEWLOC,KEY
	READ(1) PLAC,FIXD,ACTSPK,COND,HINTS
	READ(1) HNTMAX,PROP,TALLY,TALLY2,HINTLC
	READ(1) CHLOC,CHLOC2,DSEEN,DFLAG,DLOC,DALTLC
	READ(1) TURNS,LMWARN,KNFLOC,DETAIL,ABBNUM
	READ(1) NUMDIE,MAXDIE,DKILL,FOOBAR,BONUS
	READ(1) CLOCK1,CLOCK2,CLOSNG,PANIC,CLOSED
	READ(1) GAVEUP,SCORNG,ODLOC,CTEXT,STEXT,LTEXT
	READ(1) TRAVEL,TRVCON,TRVLOC,MAXTRS,HINTED
C
	TYPE 100
100	FORMAT(' Restored.')
	CLOSE (UNIT=1)
	F2=0
	RETURN
C
50	TYPE 110
110	FORMAT(' File is obsolete, RESTORE fails.')
	CLOSE (UNIT=1)
	F2=-1
	RETURN
C
200	TYPE 300
300	FORMAT(' Open error, RESTORE fails.')
	F2=-1
	RETURN
	END
