C***********************************************************************
C System Name:   VMS Password Policy
C
C Module Name:   Add_Dictionary
C
C Version:	 1.0
C
C Programmer:    M. Edward (Ted) Nieland
C
C Date Created:  January 1991
C
C Brief Description of Module Function:
C
C Adds words to a supplementary dictionary for use by the 
C associated VMS Password Policy module to reference.
C
C This program will only add the word if the word is not in the
C VMS password dictionary or in the supplemental password
C dictionary
C
C
C Revision History:
C-------------------------------------------------------------------------------
C| Date      | Name            | Reason  (SPR # if applicable)     | Version # |
C-------------------------------------------------------------------------------
C|           |                 |                                   |           |
C-------------------------------------------------------------------------------
C
C Data Dictionary:   
C
C Inputs:	File SYS$LIBRARY:VMS$PASSWORD-POLICY-WORDS.DATA
C               File SYS$LIBRARY:VMS$PASSWORD_DICTIONARY.DATA
C
C
C Outputs:      
C
C Local Variables:    
C                     Stat     I - Return status
C                     L        I - String Length of WORD
C                     K        I - Indice Counter
C                     WORD     C - Password to check
C                     READWORD C - Word read from file
C                     SAMEWORD B - Array equivalenced to WORD
C 
C Global Variables:
C
C Functions called:     LENSTR         I - Character string length
C                       STR$UPCASE     I - Change a string to uppercase    
C                                                                               
C
C Subroutines called:   LIB$SIGNAL  - Signal error
C
C***********************************************************************
C

	program edit_dict


	implicit none
	integer stat, l, lenstr, k, str$case_blind_compare
	character*32 word, readword
	byte sameword(32)


	equivalence (word,sameword)

C
C Open input and output data streams
C

	OPEN(UNIT=5,FILE='SYS$INPUT:',STATUS='OLD')
	OPEN(UNIT=6,FILE='SYS$OUTPUT:',STATUS='OLD')

C
C  Open up system provided dictionary
C

	open(unit=12,status='old',shared,
	1	readonly, access='KEYED',form='UNFORMATTED',
	1	file='sys$library:vms$password_dictionary.data')

C
C Open supplementary dictionary.  Need to be able to write to this one.
C
	open(unit=15,status='old',access='KEYED',form='UNFORMATTED',
	1	FILE='SYS$LIBRARY:VMS$PASSWORD-POLICY-WORDS.DATA')

	Write(6,3) 

3	FORMAT(' VMS Password Policy -- Add words to',
	1	' Supplemental Dictionary'//)

C
C Request the first word
C

1	Write(6,5)
5	Format(' Enter word to be added (CTRL-Z to exit): ',$)

	READ(5,10,END=999) word
10	Format(a)

C
C Get the length of the word
C
	l = lenstr(word)

C
C Convert the word to lowercase 
C
	do k=1,l
		sameword(k) = sameword(k).or.32
	enddo
C
C bad the rest of the array with null characters
C
	do k=l+1,32
		sameword(k) = 0
	enddo

C
C Do a lookup on the word in the system provided dictionary
C
	read(12,key=word(1:l),iostat=stat) readword
C
C If a word was found make sure it is the actual word and not a 
C fragment.  If the word was found notify the user.
C

	If ((STR$CASE_BLIND_COMPARE(readword,word).eq.0).AND.
	1	(stat.eq.0)) THEN

		Type *, ' Word: ',word(1:l),
	1		' is in the VMS provided Dictionary.'

		goto 1
	ENDIF

C
C Do a lookup on the word in the supplemental dictionary
C

100	read(15,key=word(1:l),iostat=stat) readword
	
C
C If a word was found make sure it is the actual word and not a 
C fragment.  If the word was found notify the user.
C
	If ((STR$CASE_BLIND_COMPARE(readword,word).eq.0).AND.
	1	(stat.eq.0)) THEN

		Type *, ' Word: ',word(1:l),
	1		 ' is in the Password Policy Dictionary.'

		goto 1
	ENDIF

C
C The owrd is not in either dictionary so add it to the supplemental
C dictionary.
C

200	write(15,iostat=stat) word

	write(6,25) word
25	Format(' The word "',a,'" has been placed in the ',
	1	'Password Policy Dictionary.')

C
C Go get a another word
C
	goto 1


999	end


	INTEGER FUNCTION LENSTR(STRING)
C***********************************************************************
C   Function to accept string  STRING  and calculate its length  LENSTR. 
C   Here LENSTR does not equal X for a CHARACTER*X character string, but
C   rather is the last position in  STRING  with a non-blank character.
C***********************************************************************
        IMPLICIT NONE 
        INTEGER*4     I,ILENGTH
	CHARACTER*(*) STRING,SPACE,NULL
	PARAMETER (SPACE=CHAR(32), NULL=CHAR(0))
	LENSTR = 0
	ILENGTH=LEN(STRING)
	DO   I = ILENGTH,1,-1
	  IF ((STRING(I:I).NE.SPACE).AND.(STRING(I:I).NE.NULL)) THEN
		LENSTR = I
	 	RETURN
	  ENDIF
	ENDDO
	RETURN
	END
