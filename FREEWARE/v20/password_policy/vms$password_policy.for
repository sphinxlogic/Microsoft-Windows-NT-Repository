C***********************************************************************
C System Name:   VMS Password Policy 
C
C Module Name:   VMS$Password_Policy
C
C Version:	 1.0
C
C Programmer:    M. Edward (Ted) Nieland
C
C Date Created:  January 1990
C
C Brief Description of Module Function:
C
C Hooks into VMS SET PASSWORD program (in VMS 5.4 or later) to filter
C passwords before they are set.  The system will not all passwords 
C is they are:
C	  (1)  USERNAME  or variations thereof
C         (2)  The "Owner's" name (from SYSUAF.DAT) or variations thereof
C         (3)  A list of suspect passwords 
C
C The suspect passwords is in addition to the checking of a system
C dictionary already done by VMS.
C
C This program is based on my original CHECKPASS program that checked
C passwords on prev VMS 5.4 systems.  
C
C
C Revision History:
C-------------------------------------------------------------------------------
C| Date      | Name            | Reason  (SPR # if applicable)     | Version # |
C-------------------------------------------------------------------------------
C| 13 Feb 92 |Nieland, Ted     | Bug causing weak password on      | 1.0.1     |
C|           |                 | to always be true on usernames    |           |
C|           |                 | that are 15 chars and in other    |           |
C|           |                 | similar cirumstances.             |           |
C-------------------------------------------------------------------------------
C| 6 May  92 |Nieland, Ted     | Fixed problem with compiling      | 1.0.2     |
C|           |                 | /running using DEC FORTRAN.       |           |
C-------------------------------------------------------------------------------
C| 20 Oct 95 |Nieland, Ted     | Checked out under VMS 6           |           |
C-------------------------------------------------------------------------------
C
C Data Dictionary:   $UAIDEF from SYS$LIBRARY:FORSYSDEF.TLB
C		     $SSDEF from SYS$LIBRARY:FORSYSDEF.TLB
C
C Inputs:	Files SYS$LIBRARY:VMS$PASSWORD-POLICY-WORDS.DATA
C		Variables PASSWORD - PASSWORD to be checked
C			  USERNAME - Username of account 
C
C Outputs:      Returns a status 
C
C Local Variables:    
C                     I        I - Integer Counter
C		      I1       I - Integer Counter
C		      I2       I - Integer Counter
C		      I3       I - Integer Counter
C                     II       I - Integer Counter
C                     IMAX     I - Integer Counter
C                     III      I - Integer Counter
C                     LO       I - Lenght of OWNER pieces arrays
C                     ALEN     I - Lenth of account string
C                     NAMELEN  I - Lenth of username string
C                     PLEN     I - Lenth of password string
C                     OWNER    C - Owner Field from UAF
C                     ACCOUNTNAME C - Account Field from UAF
C                     WORD     C - String for dictionary lookup
C                     LP       I - Limit to print
C                     MAX      I - Maximum count
C                     N        I - Integer Counter
C                     STAT     I - Function Status
C                     P        I - Password String Length
C                     PASS     C - Password to check
C                     LP       B - Array equivalnced to PASS for numerical use
C                     OWNER_FIELD C - Array of words from Owner Field of UAF
C                     LEN_OWNER_FIELD I - Array of lengths corresponding to OWNER_FIELD
C                     ALPHA    C - Alpha characters
C                     DIGITS   C - Digit characters
C                     BADCHAR  C - characters not allowed in certain cases
C                     ITM      S - Array of Structure used to extract   
C                                  data from the UAF file
C                         BUFLEN       I - BufferLength
C                         ITMCOD       I - Item Code        
C                         BUFADR       I - Buffer Address
C                         RETADR       I - Return Address
C                         END_LIST     I - End of list indicator
C 
C Global Variables:
C
C Functions called:     LENSTR         I - Character string length
C                       LIB$LOCC       I
C                       STR$TRIM       I - Strip trailing blanks/tabs
C                       STR$find_first_in_set                                   
C                       SYS$GETUAI     I - Get UAF information                  
C                       STR$CASE_BLIND_COMPARE I - Compare strings
C                       CYCLE          I - Cycle through some checks
C                                                                               
C
C Subroutines called:   LIB$SIGNAL  - Signal error
C                       INVERT - Invert a string
C
C***********************************************************************

	integer function policy_plaintext (password, username)
	implicit none
	character*(*) password,username

      INCLUDE '($UAIDEF)/NOLIST'
      INCLUDE '($SSDEF)/NOLIST'
C
      STRUCTURE  /ITMLST/
       UNION
        MAP
         INTEGER*2  BUFLEN , ITMCOD
         INTEGER*4  BUFADR , RETADR
        END MAP
        MAP
         INTEGER*4  END_LIST
        END MAP
       END UNION
      END STRUCTURE
C
      RECORD /ITMLST/ ITM(3)

	Character*32 owner, accountname, word
	Character*32 pass
	INTEGER	STR$CASE_BLIND_COMPARE,STR$FIND_FIRST_IN_SET, LENSTR
	INTEGER LO, CYCLE
	PARAMETER                   (LO=5)
        CHARACTER*31     OWNER_FIELD(LO)
        INTEGER*4    LEN_OWNER_FIELD(LO)
	CHARACTER*52 ALPHA
	CHARACTER*10 DIGIT
	CHARACTER*30 BADCHAR
	Integer stat, sys$getuai, n, I, I1, I2, I3, LIB$LOCC, IMAX
	Integer ownerlen, namelen, plen, rstat, III, P, alen
	byte lp(32)

	equivalence (lp,pass)
	DATA ALPHA
     X /'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'/
	DATA DIGIT/'0123456789'/
	DATA BADCHAR/',._!@#$%^&*()+=-{}[]|\"'':;?/~`'/


C
C	Set up the value for rstat for DEC Fortran
C
	rstat = 0

C
C Get the length of the password
C
	plen = LENSTR(password)

C
C Create a lowercase version of the password and pad the string
C with null characters for the exteneded dictionary lookup
C

	Do i = 1,plen
		pass(i:i) = password(i:i)
		lp(i) = lp(i) .or.32
	enddo

	do i=plen+1,32
		lp(i) = 0
	enddo
C
C Get the Owner and Account field information from the UAF.  These fields
C will be used as part of the password checking.
C
C  The "owner" field.
C
      ITM(1).BUFLEN = 31
      ITM(1).ITMCOD = UAI$_OWNER
      ITM(1).BUFADR = %LOC(OWNER)
      ITM(1).RETADR = 0
C
C  The "account" field.
C
      ITM(2).BUFLEN = 31
      ITM(2).ITMCOD = UAI$_ACCOUNT
      ITM(2).BUFADR = %LOC(accountname)
      ITM(2).RETADR = 0
C
      ITM(3).END_LIST = 0
C
C  Now to get the UAF info.
C
      STAT = SYS$GETUAI (,, username, ITM ,,,)
C
C Calculate the length of the owner and account strings
C
	ownerlen = LENSTR(owner)
	alen = LENSTR(accountname)
	namelen = LENSTR(username)

C
C Open up the extended dictionary and make sure the selected
C password is not in that dictionary.  This test supplements
C the testing already done by VMS.
C
	OPEN(UNIT=21,STATUS='OLD',ACCESS='KEYED',FORM='UNFORMATTED',
	1	READONLY,SHARED,
	1	FILE='SYS$LIBRARY:VMS$PASSWORD-POLICY-WORDS.DATA')

C
C check the dictionary
C	
	READ(21, KEY=pass(1:plen), iostat=rstat) word
C
C Close the dictionary file
C
	CLOSE(unit=21)
C
C  If rstat is 0 then a word was found based on the key.
C  Check that word to see if it was the full word and not a 
C  chance fragment.  (if the key was "testin", it would find the
C  word "testing".
C
	IF ((rstat.eq.0).and.
	1  (STR$CASE_BLIND_COMPARE(word,pass).eq.0)) then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF

C
C decode the "OWNER" field in the AUTHORIZE file
C We need to split apart the various words in that field
C
	n=str$find_first_in_set(owner(1:31),ALPHA)
	do i=1,LO
	    len_owner_field(i)=0
	end do
C
	do i=1,LO
	    len_owner_field(i)=lib$locc(' ',owner(n:31))-1
	    if( len_owner_field(i).lt.1 )  go to 185
	    owner_field(i)(1:len_owner_field(i)) =
     X              owner(n:n+len_owner_field(i))
            n = n + len_owner_field(i)+1
	    if( n.gt.31 )  go to 185
            n=n+str$find_first_in_set(owner(n:31),ALPHA)-1
	    if( n.gt.31 )  go to 185
	end do
C                                                                       
185	continue
C
	do i=1,LO
	if( len_owner_field(i).lt.1 )  go to 187
            do i1=1,30
	    if( owner_field(i)(len_owner_field(i):len_owner_field(i))
     X          .eq.badchar(i1:i1) )  then
                               len_owner_field(i)=len_owner_field(i) - 1
                               if( len_owner_field(i).lt.1 )  go to 187
            endif
            end do
186	if( len_owner_field(i).lt.1 )  go to 187
            n=str$find_first_in_set(owner_field(i)(1:len_owner_field(i))
     X                              ,BADCHAR )
            if( n.eq.1 )  then
		len_owner_field(i)=len_owner_field(i) - 1
		    owner_field(i)=    owner_field(i)(2:)
		go to 186
	    endif
            if( (n.gt.1).and.(n.le.len_owner_field(i)) )  then
C
C So split these into two words
C
               do iii=i+1,LO-1
               len_owner_field(LO+i+1-iii)=len_owner_field(LO+i+1-iii-1)
                   owner_field(LO+i+1-iii)=    owner_field(LO+i+1-iii-1)
               end do
               len_owner_field(i+1)    =len_owner_field(i) - n
                   owner_field(i+1)    =owner_field(i)(n+1:)
               len_owner_field(i)      =n-1
                   owner_field(i)      =owner_field(i)(1:n-1)
	    endif
	if( len_owner_field(i).lt.1 )  go to 187
	end do
C
C Now count the number of names found in the "owner" field
C
187	IMAX=0
	do i=1,LO
	if( len_owner_field(i).ge.1 )   then
          IMAX=IMAX + 1
	endif
	end do
C
C       Try combinations of names and initials (up to 26 combinations)
C
	if( imax.lt.2)  go to 195
	do i1=1,3
	do i2=1,3
	do i3=1,3
	if( i1.eq.1 )  then
		pass= owner_field(1)(1:len_owner_field(1))
		p   = len_owner_field(1)
	else if( i1.eq.2 )  then
		pass= owner_field(1)(1:1)
		p   = 1
	else
		pass = ' '
		p   = 0
	endif
	if( i2.eq.1 )  then
                pass(p+1:)=         owner_field(2)(1:len_owner_field(2))
                p         = p + len_owner_field(2)
	else if( i2.eq.2 )  then
                pass(p+1:)=         owner_field(2)(1:1)
                p         = p + 1
	endif
        if( (imax.lt.3).and.(i3.gt.1) )  go to 192
        if(  imax.lt.3                )  go to 191
	if( i3.eq.1 )  then
                pass(p+1:)=         owner_field(3)(1:len_owner_field(3))
                p         = p + len_owner_field(3)
	else if( i3.eq.2 )  then
                pass(p+1:)=         owner_field(3)(1:1)
                p         = p + 1
	endif
191	if( p.eq.0 )  go to 192
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF

C
192	continue
	end do
	end do
	end do
C
C 4*(1-26)  Try first name + any letter
C
C
195	if( imax.lt.1)  go to 199
C
	pass= owner_field(1)(1:len_owner_field(1))
	p   = len_owner_field(1) 
	If (CYCLE( PASS,P,PASSWORD ) .ne. ss$_normal) then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
	if( imax.lt.2)  go to 199
C
C 4*(1-26)  Try second name + any letter
C
	pass= owner_field(2)(1:len_owner_field(2))
	p   = len_owner_field(2) 
	If (CYCLE( PASS,P,PASSWORD ) .ne. ss$_normal) then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
	if( imax.lt.3)  go to 199
C
C 4*(1-26)  Try third name + any letter
C
	pass= owner_field(3)(1:len_owner_field(3))
	p   = len_owner_field(3) 
	If (CYCLE( PASS,P,PASSWORD ) .ne. ss$_normal) then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
	if( imax.lt.4)  go to 199
C
C 4*(1-26)  Try fourth name + any letter
C
	pass= owner_field(4)(1:len_owner_field(4))
	p   = len_owner_field(4) 
	If (CYCLE( PASS,P,PASSWORD ) .ne. ss$_normal) then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
C
199	continue
C
C
C
C
C
C 1.	Check the USERNAME as the password 
C
	PASS = ' '
	PASS = USERNAME(1:namelen)
	p    = namelen
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
C 2.	Try to check the Person's Last Name as the password 
C

	PASS = ' '
	IF( (USERNAME(namelen-1:namelen-1) .EQ. '_').OR.
     x      (USERNAME(namelen-1:namelen-1) .EQ. '$')    )  THEN
		PASS = USERNAME(1:namelen-2)
            p    = namelen - 2
 	ELSE
            PASS = USERNAME(2:namelen)
            p    = namelen - 1
	ENDIF
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
C 3.	Try again to check the Person's Last Name as the password 
C

	PASS = ' '
	IF( (USERNAME(2:2) .EQ. '_').OR.
     x      (USERNAME(2:2) .EQ. '$')    )  THEN
            PASS = USERNAME(3:namelen)
            p    = namelen - 2
	ELSE
            PASS = USERNAME(1:namelen-1)
            p    = namelen - 1
	ENDIF
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
C 4.	Cycle through  USERNAME + any letter etc.
C
        CALL STR$TRIM(PASS,USERNAME(1:namelen),namelen)
	p   = namelen
	If (CYCLE( PASS,P,PASSWORD ) .ne. ss$_normal) then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
C
C
C 5.	Check the Accoount as the password 
C
	PASS = ' '
	PASS = ACCOUNTNAME(1:alen)
	p    = alen
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then
		policy_plaintext = ss$_pwdweak
		return
	ENDIF



	policy_plaintext = ss$_normal
	return 
	end



C
C No checking is done on the hash value at this time
C
	integer function policy_hash (hash, username)

	IMPLICIT NONE
	INCLUDE '($SSDEF)/NOLIST'

	character*(*) username
	integer hash(2)


	policy_hash = ss$_normal
	return 
	end


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
C***********************************************************************
	SUBROUTINE INVERT( PASS,P )
	IMPLICIT NONE
C
C Reverse the order of the letters in PASS
C
	CHARACTER*(*) PASS
	CHARACTER*32  TEMP
	INTEGER*2     P,II
C
	TEMP=PASS
	DO II=1,P
	    PASS(II:II)=TEMP(P+1-II:P+1-II)
	END DO
	RETURN
	END
C
C***********************************************************************
C
	INTEGER FUNCTION CYCLE( PASS,P,PASSWORD)
	IMPLICIT NONE
      INCLUDE '($UAIDEF)/NOLIST'
      INCLUDE '($SSDEF)/NOLIST'
C Cycle prepending & postpending: a single character
C                                 "underscore" character
C                                 "dollar sign" character
C                                 $_letter
C                                 _$letter
C                                 $_$letter
C                                 _$_letter
C                      Also try:  wordword, word_word,word$word,word$_word,
C                                 word_$word,word$_$word, & word_$_word
	CHARACTER*(*)       PASS,PASSWORD
	INTEGER*2           P,PTEMP,U,NALPHA,I1
	CHARACTER*32        TEMP
	PARAMETER (NALPHA=36)
	CHARACTER*(NALPHA)  ALPHA
	INTEGER	STR$CASE_BLIND_COMPARE
C
C
	DATA ALPHA /'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'/
C
	CYCLE = 0 
	 TEMP = PASS
	PTEMP = P
C
C 2       Try    PASS  itself
C
	pass=  TEMP(1:PTEMP)
	p   = PTEMP
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
C 7*2*36  Try    PASS + any letter  etc.
C
	if( ptemp.gt.27 )  go to 9000
	do i1=1,NALPHA
	pass=  TEMP(1:PTEMP)//alpha(i1:i1)
	p   = PTEMP + 1
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass=  TEMP(1:PTEMP)//'_'//alpha(i1:i1)
	p   = PTEMP + 2
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass=  TEMP(1:PTEMP)//'$'//alpha(i1:i1)
	p   = PTEMP + 2
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass=  TEMP(1:PTEMP)//'$_'//alpha(i1:i1)
	p   = PTEMP + 3
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass=  TEMP(1:PTEMP)//'_$'//alpha(i1:i1)
	p   = PTEMP + 3
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass=  TEMP(1:PTEMP)//'$_$'//alpha(i1:i1)
	p   = PTEMP + 4
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass=  TEMP(1:PTEMP)//'_$_'//alpha(i1:i1)
	p   = PTEMP + 4
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	end do
C
C 7*2*36  Try    any letter + PASS   etc.
C
	do i1=1,NALPHA
	pass= alpha(i1:i1)// TEMP(1:PTEMP)
	p   = PTEMP + 1
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass= alpha(i1:i1)//'_'// TEMP(1:PTEMP)
	p   = PTEMP + 2
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass= alpha(i1:i1)//'$'// TEMP(1:PTEMP)
	p   = PTEMP + 2
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass= alpha(i1:i1)//'$_'// TEMP(1:PTEMP)
	p   = PTEMP + 3
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass= alpha(i1:i1)//'_$'// TEMP(1:PTEMP)
	p   = PTEMP + 3
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass= alpha(i1:i1)//'$_$'// TEMP(1:PTEMP)
	p   = PTEMP + 4
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	pass= alpha(i1:i1)//'_$_'// TEMP(1:PTEMP)
	p   = PTEMP + 4
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	end do
C
C 7*2     Try  PASSxxxPASS,  where   (xxx= , _, $, $_, _$, $_$, _$_ )
C
	if( ptemp.gt.15 )  go to 9000
        pass= TEMP(1:PTEMP)//TEMP(1:PTEMP)
        p   =PTEMP + PTEMP
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
        pass= TEMP(1:PTEMP)//'_'//TEMP(1:PTEMP)
        p   =PTEMP + PTEMP + 1
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
        pass= TEMP(1:PTEMP)//'$'//TEMP(1:PTEMP)
        p   =PTEMP + PTEMP + 1
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
	if( ptemp.gt.14 )  go to 9000
        pass= TEMP(1:PTEMP)//'$_'//TEMP(1:PTEMP)
        p   =PTEMP + PTEMP + 2
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
        pass= TEMP(1:PTEMP)//'_$'//TEMP(1:PTEMP)
        p   =PTEMP + PTEMP + 2
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
        pass= TEMP(1:PTEMP)//'$_$'//TEMP(1:PTEMP)
        p   =PTEMP + PTEMP + 3
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
        pass= TEMP(1:PTEMP)//'_$_'//TEMP(1:PTEMP)
        p   =PTEMP + PTEMP + 3
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
	call invert( pass,p )
	If (STR$CASE_BLIND_COMPARE(pass,password).eq.0)  then 
		cycle = ss$_pwdweak
		return
	ENDIF
C
9000	cycle = ss$_normal
	RETURN 
	END

