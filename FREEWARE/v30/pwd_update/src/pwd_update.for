	PROGRAM PWD_CHANGE
	IMPLICIT NONE
C+
C 
C ABSTRACT: 
C 
C        This program is designed to be installed with SYSPRV and controlled
C        via an ACL to allow non-privileged users to change the password of
C        certain accounts.  It is designed to be used in a help-desk
C        environment.
C
C        The user is prompted for a username and then presented with the
C        last LOGIN and PASSWORD CHANGE date.  The caller should be asked these
C        questions.  If they are answered correctly, the password can be
C        changed.  If not, the account can be disabled.
C
C        This program uses the SYS$SETUAI service to change the password
C        and set any relevant flags, but due to a "feature" of LOGINOUT,
C        it has to modify the UAF file directly to pre-expire the account.
C        (LOGINOUT doesn't look at the PWD_EXPIRED bit to determine if it
C        should prompt for a new user)
C
C        There will be no auditing on the password expiration call, but the
C        call to SYS$SETUAI will provide an audit record.
C        
C        
C NOTE: FMS IS REQUIRED
C
C AUTHOR(S): 
C 
C        Robert Eden                robert@cpvax.tu.com
C        Comanche Peak S.E.S       robert@cpvax.lonestar.org
C        Glen Rose Tx, 76043        eden@fallout.lonestar.org 
C        (817) 897-0491
C 
C MODIFICATION HISTORY:
C 
C        Date     | Name  | Description
C ----------------+-------+-----------------------------------------------------
C 01-MAY-1992     |R.EDEN | removed AUTOTAB from ACTION field
C 12-AUG-1992     |R.EDEN | correctly generate NOW value for expiration check
C 10-OCT-1992     |R.Eden | fixed problem with locked password failure
C [change_entry]
C-
CDEC$ Title 'PWD_UPDATE'
CDEC$ IDENT '2.1'
	INCLUDE 'FMS$EXAMPLES:FDVDEF'
        INCLUDE '(LIB$ROUTINES)'
        INCLUDE '($SSDEF)'
        INCLUDE '($LIBDEF)'
        INCLUDE '($PRVDEF)'
        INCLUDE '($RMSDEF)'
        INCLUDE '($UAIDEF)'

        REAL*8  LOGIN_BIN,PCHANGE_BIN,EXPIRATION,JUNK
        INTEGER AUTH_PRIV(2),DEF_PRIV(2),OK_PRIV(2),NOW(2)
        INTEGER USERLEN,FLAGS
        INTEGER*2 UIC(2)

        CHARACTER  USERNAME *17
 	CHARACTER  ACTION   *1

        CHARACTER  LOGIN    *23
	CHARACTER  PCHANGE  *23
	CHARACTER  PASSWORD *6
        CHARACTER STRING*80

C
C SYSTEM SERVICE AND FMS DEFINITIONS
C
	CHARACTER*12 TCA,WKSP
	INTEGER WKSP_SIZE,TERM
	PARAMETER (WKSP_SIZE=2000)
        INTEGER STATUS, SYS$GETUAI, SYS$GETTIM, SYS$ASCTIM, SYS$SETUAI

        STRUCTURE /ITEMLIST3/
            INTEGER*2 BUFLEN,ITEM
            INTEGER*4 BUFFER, RETLEN
            END STRUCTURE    !ITEMLIST3

        RECORD /ITEMLIST3/ GETUAI_ITEM(9),SETUAI_ITEM(9)

C
C Set up initial variable values
C
        CALL SYS$GETTIM (NOW)

        OK_PRIV(1)   = PRV$M_TMPMBX + PRV$M_NETMBX + PRV$M_OPER +
     1                 PRV$M_GROUP
        OK_PRIV(2)   = 0

        GETUAI_ITEM(1).ITEM  = UAI$_FLAGS
        GETUAI_ITEM(1).BUFLEN= 4
        GETUAI_ITEM(1).BUFFER= %LOC(FLAGS)
        GETUAI_ITEM(1).RETLEN= 0

        GETUAI_ITEM(2).ITEM  = UAI$_UIC
        GETUAI_ITEM(2).BUFLEN= 4
        GETUAI_ITEM(2).BUFFER= %LOC(UIC(1))
        GETUAI_ITEM(2).RETLEN =0

        GETUAI_ITEM(3).ITEM  = UAI$_PRIV
        GETUAI_ITEM(3).BUFLEN= 8
        GETUAI_ITEM(3).BUFFER= %LOC(AUTH_PRIV(1))
        GETUAI_ITEM(3).RETLEN= 0

        GETUAI_ITEM(4).ITEM  = UAI$_DEF_PRIV
        GETUAI_ITEM(4).BUFLEN= 8
        GETUAI_ITEM(4).BUFFER= %LOC(DEF_PRIV)
        GETUAI_ITEM(4).RETLEN= 0

        GETUAI_ITEM(5).ITEM  = UAI$_EXPIRATION
        GETUAI_ITEM(5).BUFLEN= 8
        GETUAI_ITEM(5).BUFFER= %LOC(EXPIRATION)
        GETUAI_ITEM(5).RETLEN= 0

        GETUAI_ITEM(6).ITEM  = UAI$_PWD_DATE
        GETUAI_ITEM(6).BUFLEN= 8
        GETUAI_ITEM(6).BUFFER= %LOC(PCHANGE_BIN)
        GETUAI_ITEM(6).RETLEN= 0

        GETUAI_ITEM(7).ITEM  = UAI$_LASTLOGIN_I
        GETUAI_ITEM(7).BUFLEN= 8
        GETUAI_ITEM(7).BUFFER= %LOC(LOGIN_BIN)
        GETUAI_ITEM(7).RETLEN= 0

        GETUAI_ITEM(8).ITEM  = 0
        GETUAI_ITEM(8).BUFLEN= 0
        GETUAI_ITEM(8).BUFFER= 0
        GETUAI_ITEM(8).RETLEN= 0

        SETUAI_ITEM(1).ITEM  = UAI$_PASSWORD
        SETUAI_ITEM(1).BUFLEN= LEN(PASSWORD)
        SETUAI_ITEM(1).BUFFER= %LOC(PASSWORD)
        SETUAI_ITEM(1).RETLEN= 0

        SETUAI_ITEM(2).ITEM  = UAI$_FLAGS
        SETUAI_ITEM(2).BUFLEN= 4
        SETUAI_ITEM(2).BUFFER= %LOC(FLAGS)
        SETUAI_ITEM(2).RETLEN= 0

        SETUAI_ITEM(3).ITEM  = 0
        SETUAI_ITEM(3).BUFLEN= 0
        SETUAI_ITEM(3).BUFFER= 0
        SETUAI_ITEM(3).RETLEN= 0
 	

C
C set up FMS 
C     
	CALL FDV$ATERM(TCA,LEN(TCA),2)
	CALL FDV$AWKSP(WKSP,WKSP_SIZE)
	CALL FDV$CDISP('PWD_UPDATE')	

C
C main loop 
C
5       CALL FDV$PUTD('LOGIN')
        CALL FDV$PUTD('PCHANGE')
        CALL FDV$PUTD('PASSWORD')
        CALL FDV$PUTD('ACTION')

	CALL FDV$GET(USERNAME,TERM,'USERNAME')
        CALL FDV$RETLE(USERLEN,'USERNAME')
        IF (TERM.EQ.FDV$K_PF_4)    GOTO 999
        IF (TERM.NE.FDV$K_FT_NTR)  GOTO 5
      
C
C get username information
C        
        STATUS = sys$getuai (,, USERNAME,GETUAI_ITEM(1),,,)
      
        if (.not.status) then
            IF (STATUS.EQ.RMS$_RNF) THEN
                    CALL FDV$BELL
                    CALL FDV$PUTL('User not found')
                    GOTO 5
            ELSE 
 	            CALL FDV$BELL
                    STATUS = lib$sys_getmsg (STATUS,, STRING,,)
                    IF (.not.status) call lib$signal(%val(status))
                    CALL FDV$PUTL(STRING)
                    GOTO 5
                    ENDIF
             ENDIF
C
C Check for a DISUSERed account
C
 	IF (IAND(FLAGS,UAI$M_DISACNT).NE.0) THEN
                    CALL FDV$BELL
                    CALL FDV$PUTL('This account has been disabled')
                    GOTO 5
                    ENDIF
C
C Check for an EXPIRED account
C
        STATUS =  lib$sub_times (NOW, EXPIRATION , JUNK)
        IF (STATUS.EQ.LIB$_NORMAL) THEN
                    CALL FDV$BELL
                    CALL FDV$PUTL('This account has expired')
                    GOTO 5
                    ENDIF
C
C Check for a locked account (user can't change password)
C      
 	IF (IAND(FLAGS,UAI$M_LOCKPWD).ne.0) then
                    CALL FDV$BELL
                    CALL FDV$PUTL(
     +                        'Passwords are locked for this account')
                    GOTO 5
                    ENDIF

C
C Check for a priviledged account
C
        AUTH_PRIV(1) = IOR (AUTH_PRIV(1), DEF_PRIV(1))
        AUTH_PRIV(2) = IOR (AUTH_PRIV(2), DEF_PRIV(2))

        IF  ( (                            UIC(2).LE.8)  .OR.
     +        (IAND(AUTH_PRIV(1),NOT(OK_PRIV(1))).NE.0)  .OR.
     +        (IAND(AUTH_PRIV(2),NOT(OK_PRIV(2))).NE.0) ) THEN
              CALL FDV$BELL
              CALL FDV$PUTL(
     +             'You cannot change the password for this account')
              GOTO 5
              ENDIF
      
C
C NOW GENERATE OUR OUTPUT STRINGS
C
        	WRITE(PASSWORD,'(I6.6)') INT(RAN( NOW(1) )*100000.0)

        STATUS = sys$asctim (, LOGIN, LOGIN_BIN ,)
              IF (.NOT.STATUS) THEN
              	  WRITE(STRING,FMT=101) STATUS
101               FORMAT ('ERROR CONVERTING LOGIN DATE:',I)
                  CALL FDV$PUTL(STRING)
                  GOTO 5
                  ENDIF
      
        STATUS = sys$asctim (, PCHANGE, PCHANGE_BIN ,)
              IF (.NOT.STATUS) THEN
                   WRITE(STRING,FMT=102) STATUS
102                FORMAT ('ERROR CONVERTING PCHANGE DATE:',I)
                   CALL FDV$PUTL(STRING)
                   GOTO 5
                   ENDIF
        
        IF (  LOGIN(1:11).EQ.'17-NOV-1858')   LOGIN ='NEVER'
        IF (PCHANGE(1:11).EQ.'   0 00:00:') PCHANGE ='PRE-EXPIRED'

        CALL FDV$PUT( LOGIN(1:17)  , 'LOGIN')
        CALL FDV$PUT( PCHANGE(1:17), 'PCHANGE')
        CALL FDV$PUT( PASSWORD     , 'PASSWORD')

20	CALL FDV$GET(ACTION,TERM,'ACTION')
        IF (TERM.EQ.FDV$K_PF_4)    GOTO 5
        IF (TERM.NE.FDV$K_FT_NTR)  GOTO 20

 	IF (ACTION.EQ.' ') GOTO 20
        IF (ACTION.EQ.'N') THEN  ! SET PASSWORD
              FLAGS = IAND(FLAGS,NOT(UAI$M_PWD_EXPIRED))
              STATUS = sys$setuai (,,USERNAME(1:USERLEN),
     +                     SETUAI_ITEM(1),,,)
                  IF (.NOT.STATUS) THEN
       	            CALL FDV$BELL
                    STATUS = lib$sys_getmsg (STATUS,, STRING,,)
                    IF (.not.status) call lib$signal(%val(status))
                    CALL FDV$PUTL(STRING)
                    GOTO 20
                    ENDIF
               call pre_expire(username(1:userlen))
               CALL FDV$PUTL(USERNAME(1:USERLEN)//
     +                       ' Password Set to '//password)
               ENDIF

        IF (ACTION.EQ.'D') THEN  ! DISABLE ACCOUNT
               FLAGS = IOR(FLAGS,UAI$M_DISACNT)
               STATUS = sys$setuai (,,USERNAME(1:USERLEN),
     +                     SETUAI_ITEM(2),,,)
               IF (.NOT.STATUS) THEN
       	            CALL FDV$BELL
                    STATUS = lib$sys_getmsg (STATUS,, STRING,,)
                    IF (.not.status) call lib$signal(%val(status))
                    CALL FDV$PUTL(STRING)
                    GOTO 20
                    ENDIF
               CALL FDV$PUTL(USERNAME(1:USERLEN)//'ACCOUNT DISABLED')
               ENDIF

        CALL FDV$PUTD('USERNAME')
        GOTO 5
C
 

999	CALL FDV$DTERM(TCA)
	END
        
        SUBROUTINE pre_expire(username)
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    This procedure <<DIRECTLY>> accesses the SYS$SYSTEM:SYSUAF.DAT file
C    to pre-expire the current password.  This is necessary because VMS
C    in it's infinate wisdom (LOGINOUT), will only prompt the user for a
C    new password if the UAF$Q_PWD_DATE field to " 000- 00:00".
C
C    Authorize does this with the /pwdexpired flag, but the SYS$SETUAI 
C    does not have an equivilent routine.
C
C    One would think that if the PWD_EXPIRED flag is set, then the user
C    would be prompted for a new one.  But ONE doesn't work for DEC.  Up
C    through version 5.3-1, this "feature" doesn't exist.  The only
C    way a user get's prompted for a new password is if the PWD_DATE field
C    is negitive, or if the PWD_DATE + PWD_LIFETIME has expired.  This probably
C    explains why AUTHORIZE doesn't set the PWD_EXPIRED flag.
C
C    Oh, by the way... if the PWD_EXPIRED flag is set, that account can't log
C    on... you get a "PASSWORD EXPIRED... CONTACT YOUR SYSTEM MANAGER" message.
C    
C    The UAFDEF file was converted from a C file found on a decus tape
C
C    ***** WARNING ***** THIS PROCEDURE DOES NOT GENERATE A AUDIT ALARM!!!!
C 
C-
        IMPLICIT NONE
        character username*(*)

        include '($ssdef)'
        include 'uafdef'

        INTEGER*4 rec_len, pchange(2)
        RECORD /UAF_RECORD/ REC
       
      OPEN ( UNIT=1,
     1       FILE = 'SYSUAF',
     1       DEFAULTFILE = 'SYS$SYSTEM:*.DAT',
     1       ORGANIZATION = 'INDEXED',
     1       FORM = 'FORMATTED',
     1       ACCESS = 'KEYED',
     1       SHARED,
     1       STATUS='OLD')

        READ (UNIT=1,
     1        KEY = username , KEYID=0,
     1        FMT='(Q,A)',
     1        ERR=666)                rec_len,rec.string

        IF ((REC.UAF$B_RTYPE  .NE.CHAR(UAF$C_USER_ID)).OR.
     +      (REC.UAF$B_VERSION.NE.CHAR(UAF$C_VERSION))) THEN

                      CALL LIB$SIGNAL(%VAL(SS$_FILESTRUCT))

            else
                      rec.uaf$q_pwd_date = 'FFFFFFFFFFFFFFFF'x
                      REWRITE (1,'(a)',ERR=666) rec.string(1:rec_len)
                      endif

888     CLOSE (UNIT=1,ERR=999)
999     RETURN

666     CALL FDV$BELL
        CALL FDV$PUTL('ERROR PRE-EXPIRING PASSWORD!!')
        GOTO 888
        END

	INTEGER FUNCTION VALID1_UAR
C - FROM FMS$EXAMPLES:SAMPFOR.FOR
C	UAR for field validation of any one character field. The
C	UAR associated data has in it the legal characters allowed,
C	except that blank is not allowed unless it appears before
C	the first trailing blank.  For example an assoc. value string
C	'aqr' implies that only the letters a, q, and r are allowed.
C	A string ' aqr' means that blank is acceptable in addition
C	to a, q, and r. Note that this routine is case sensitive
C	(that is, it checks for correct case).  You can get around
C	case sensitivity by using the force upper case field attribute
C	and putting only capitals into the UAR associated value
C	string.
C
C	This routine can be used with any form and field since 
C	it determines the context for itself.

	IMPLICIT NONE

	INCLUDE 'FMS$EXAMPLES:FDVDEF'

	CHARACTER*31	FRMNAM, FLDNAME
	CHARACTER*82	UARVAL
	CHARACTER*1	FVALUE
	INTEGER 	ATCA, AWORKSPACE
	INTEGER		CURPOS, FLDTRM, INSOVR, FINDEX, HELPNUM

C	 Retrieve context: we will ignore TCA address, WKSP address, FRMNAM,
C		CURPOS, FLDTRM, and INSOVR, using only UARVAL, and only the
C		initial, non-blank characters of it.
C	 Retrieve field name and index.
C	 Retrieve field value.

	CALL FDV$RETCX(ATCA,AWORKSPACE,
     +		%DESCR(FRMNAM), %DESCR(UARVAL), CURPOS, FLDTRM,
     1           INSOVR, HELPNUM )
	CALL FDV$RETFN( FLDNAME, FINDEX )
	CALL FDV$RET( FVALUE,  FLDNAME, FINDEX )
C	
C	 To be valid, FVALUE must occur in the string UARVAL
C	
	IF ( INDEX(UARVAL, FVALUE) .GT. 0) THEN
		VALID1_UAR = FDV$K_UVAL_SUC   		! Success
	ELSE
		CALL FDV$PUTL( 'Illegal value' )
		VALID1_UAR = FDV$K_UVAL_FAIL
	ENDIF
	RETURN
	END
