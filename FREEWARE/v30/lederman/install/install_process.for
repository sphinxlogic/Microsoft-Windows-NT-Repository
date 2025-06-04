	PROGRAM INSTALL_PROCESS
C
C Parse an INSTALL/LIST/FULL listing to extract useful information,
C and put it into a more easily processed form (i.e., fixed length
C aligned fields which can be examined with Datatrieve, etc.)
C
C B. Z. Lederman	29-Sep-1987
C
C System RTL calls.
C
	INTEGER*4 STR$UPCASE, STR$TRIM, STR$CONCAT
	INTEGER*4 OTS$CVT_TU_L, OTS$CVT_L_TU
C
C Local Variables.
C
	INTEGER*4 IE, I1			! temp value for conversions
	INTEGER*2 IN_LEN	/0/		! length of input string
	LOGICAL OUT_PUT		/.FALSE./
	CHARACTER*64 FILE_SPEC			! extracted file specification
	CHARACTER*32 IMAGE			! extracted image name
	CHARACTER*8  QUAL			! extracted install qualifiers
	DATA QUAL / '        '/
	CHARACTER*6  ENTRY			! number of image entries
	CHARACTER*6  SHARED			! maximum shared
	CHARACTER*6  GLOBAL			! global sections
	DATA ENTRY, SHARED, GLOBAL / '000000', '000000', '000000' /
	CHARACTER*132	LINEIN			! text input
C
	CHARACTER*64	IN_FILE
	CHARACTER*64	OUT_FILE
C
	WRITE (6, 10)
   10	FORMAT ('$ Enter INSTALL listing file name: ')
	READ (5, '(A)', END = 950) IN_FILE
	OPEN (UNIT = 1, FILE = IN_FILE, STATUS = 'OLD', ERR = 970)
C
	WRITE (6, 20)
   20	FORMAT ('$ Enter processed output file name: ')
	READ (5, '(A)', END = 950) OUT_FILE
	OPEN (UNIT = 2, FILE = OUT_FILE, STATUS = 'NEW', ERR = 980,
     1		CARRIAGECONTROL = 'LIST', ORGANIZATION = 'SEQUENTIAL',
     1	      RECL = 255, RECORDTYPE = 'VARIABLE')
C
C	Loop forever (until end of input file)
C
  200	READ (1, '(A)', END = 900) LINEIN
	CALL STR$UPCASE (LINEIN, LINEIN)		! convert to upper case
	CALL STR$TRIM (LINEIN, LINEIN, IN_LEN)		! trim and get length
C
	IF (IN_LEN .EQ. 0) THEN				! end of a data set
	    IF (OUT_PUT) THEN				! if we have something
		WRITE (2, '(A)') FILE_SPEC // IMAGE // QUAL //
     1			ENTRY // SHARED // GLOBAL	! write previous record
		IMAGE = '                                '
		ENTRY = '000000'			! blank out
		SHARED = '000000'			! old data
		GLOBAL = '000000'
		QUAL = '        '
		OUT_PUT = .FALSE.			! reset write indicator
	    ENDIF
	    GOTO 200					! get next input record
	ENDIF
C
C Pull out the information line-by-line by looking for unique
C  identification for each line.
C
	IF (LINEIN(1:5) .EQ. 'DISK$') THEN
	    FILE_SPEC = LINEIN			! start with disk information
	ELSE IF (LINEIN(9:15) .EQ. 'ENTRY A') THEN
	    I = INDEX(LINEIN, '=')		! find start of data
	    I = I + 2				! skip over = and blank space
C
C This seems a bit tedious, but it's the easiest way to pull out
C left justified digits with blank spaces from an arbitrary field.
C The OTS is what would be used with a re-read anyway.
C
	    CALL OTS$CVT_TU_L(LINEIN(I:IN_LEN), IE, %VAL(4), %VAL(17))
C
C Put it back into a zero-filled field (just as easy to do it now
C   as with a FORMAT statement)
C
	    IF (IE .GT. 0) CALL OTS$CVT_L_TU(IE, ENTRY, %VAL(6), %VAL(4))
	    OUT_PUT = .TRUE.
	ELSE IF (LINEIN(9:15) .EQ. 'CURRENT') THEN
	    I1 = INDEX(LINEIN, '=')		! find start of data
	    I = INDEX(LINEIN(I1:IN_LEN), '/')	! find maximum data
	    I = I + I1				! skip over "/"
	    CALL OTS$CVT_TU_L(LINEIN(I:IN_LEN), IE, %VAL(4), %VAL(17))
	    IF (IE .GT. 0) CALL OTS$CVT_L_TU(IE, SHARED, %VAL(6), %VAL(4))
	    OUT_PUT = .TRUE.
	ELSE IF (LINEIN(9:15) .EQ. 'GLOBAL ') THEN
	    I = INDEX(LINEIN, '=')		! find start of data
	    I = I + 2				! skip over = and blank space
	    CALL OTS$CVT_TU_L(LINEIN(I:IN_LEN), IE, %VAL(4), %VAL(17))
	    IF (IE .GT. 0) CALL OTS$CVT_L_TU(IE, GLOBAL, %VAL(6), %VAL(4))
	    OUT_PUT = .TRUE.
	ELSE
	    I = INDEX(LINEIN, ';')		! look for semi-colon
	    IF (I .GT. 0) THEN			! if there is one
		I = I - 1			! don't copy semi-colon
		IMAGE = LINEIN(4:I)		! take out image name
		I1 = INDEX(LINEIN, 'OPEN')	! look for installation
		IF (I1 .GT. 0) QUAL(1:1) = 'O'	! qualifiers
		I1 = INDEX(LINEIN, 'HDR')
		IF (I1 .GT. 0) QUAL(2:2) = 'H'
		I1 = INDEX(LINEIN, 'SHAR')
		IF (I1 .GT. 0) QUAL(3:3) = 'S'
		I1 = INDEX(LINEIN, 'PRV')
		IF (I1 .GT. 0) QUAL(4:4) = 'P'
		I1 = INDEX(LINEIN, 'PROT')
		IF (I1 .GT. 0) QUAL(5:5) = 'P'
		I1 = INDEX(LINEIN, 'LNKBL')
		IF (I1 .GT. 0) QUAL(6:6) = 'L'
		I1 = INDEX(LINEIN, 'CMODE')
		IF (I1 .GT. 0) QUAL(7:7) = 'C'
		I1 = INDEX(LINEIN, 'NOPURG')
		IF (I1 .GT. 0) QUAL(8:8) = 'N'
	    ENDIF
	ENDIF
	GOTO 200				! get next input record
C
  900	CONTINUE
C
  950	CALL EXIT				! also closes files
  970	STOP ' Error opening input file '
  980	STOP ' Error opening output file '
	END
