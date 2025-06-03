$!++
$! Sample NEWSRDR startup command procedure for NEWSRDR V4.8
$! 17-MAY-1994
$!--
$!
$! Edit the following as needed for your installation:
$!
$ NEWSRDR_DIR	    = "SYS$SYSTEM:"
$ NEWSRDR_HELP_DIR  = "SYS$HELP:"
$ NEWSRDR_DOC_DIR   = ""                    ! for on-line docs; see below
$ NEWSRDR_DEF       = "DEFINE/SYSTEM/EXEC/NOLOG"
$!
$! IMPORTANT: edit the next line to include _only_ those privileges required
$!            by your E-mail system, plus EXQUOTA if you want to allow
$!  	      NEWSRDR to write out users' profiles & NEWSRC files even if
$!  	      they're over quota.
$!
$ NEWSRDR_PRIVS     = "/PRIVILEGE=(EXQUOTA,WORLD,OPER,NETMBX,SYSPRV,SYSLCK)"
$!
$!  Comment out the next line if you don't want NEWSRDR installed /SHARE.
$!
$ NEWSRDR_SHARE     = "/SHARE/OPEN/HEADER"
$!
$! Logicals needed by all systems (substitute appropriate values):
$!
$ NEWSRDR_DEF NEWSRDR_MAIL_NODE     "local.host"
$ NEWSRDR_DEF NEWSRDR_MAIL_PROTOCOL "MX%"
$ NEWSRDR_DEF NEWSRDR_NODE_NAME     "local.host"
$ NEWSRDR_DEF NEWSRDR_ORGANIZATION  "Organization, City, State, etc."
$ NEWSRDR_DEF NEWSRDR_SERVER        "server.hostname"
$!
$! Make sure NETLIB has been started up.
$!
$ IF F$TRNLNM ("NETLIB_SHR") .EQS. ""
$ THEN
$   IF F$SEARCH ("SYS$STARTUP:NETLIB_STARTUP.COM") .EQS. ""
$   THEN
$   	IF F$SEARCH ("SYS$LOGIN:NETLIB_LOGIN.COM") .NES. "" THEN -
    	    	@SYS$LOGIN:NETLIB_LOGIN
$   ELSE
$   	@SYS$STARTUP:NETLIB_STARTUP
$   ENDIF
$ ENDIF
$!
$!-----------
$!
$! If you would like to have the NEWSRDR on-line documentation be made
$! available to your Bookreader users automatically, copy the DECW$BOOK
$! and DECW$BOOKSHELF files from the NEWSRDR distribution to a directory
$! of your choosing and set the symbol NEWSRDR_DOC_DIR above to point
$! to that directory.  The DECW$BOOK search list logical will automatically
$! be updated to include the specified directory.
$!
$ IF NEWSRDR_DOC_DIR .NES. ""
$ THEN
$   NEWSRDR_DEF NEWSRDR_DOCUMENTS 'NEWSRDR_DOC_DIR
$   IF F$TRNLNM ("DECW$BOOKSHELF","LNM$SYSTEM") .NES. ""
$   THEN
$   	CALL ADD_TO_SEARCHLIST DECW$BOOKSHELF NEWSRDR_DOCUMENTS:LIBRARY.DECW$BOOKSHELF;
$   ELSE
$   	CALL ADD_TO_SEARCHLIST DECW$BOOK NEWSRDR_DOCUMENTS:
$   	CALL ADD_TO_SEARCHLIST DECW$BOOK NEWSRDR_DOCUMENTS: DECW$LOGICAL_NAMES
$   ENDIF
$ ENDIF
$!
$!-----------
$!
$! If you want to have NEWSRDR display an announcement message each
$! time it runs, un-comment and edit the following.  If you want to
$! place the announcement message in a file, use "@file-spec" as the
$! equivalence string (just as for SYS$ANNOUNCE).
$!
$! NEWSRDR_DEF NEWSRDR_ANNOUNCE "Your message here"
$!
$! If you want your first-time NEWSRDR users to become automatically
$! subscribed to some newsgroups, un-comment and edit the following.
$! Wildcards are allowed in the newsgroup names.
$!
$! NEWSRDR_DEF NEWSRDR_INITIAL_GROUPS "news.announce.newusers","alt.flame"
$!
$!-----------
$!
$! If you want to prevent ordinary users from setting their own Reply-To
$! header in the messages they post, change the following to TRUE:
$!
$! NEWSRDR_DEF NEWSRDR_DISABLE_USER_REPLY_TO FALSE
$!
$! By default, users may specify their own Reply-To addresses by defining
$! NEWSRDR_REPLY_TO.
$!
$!-----------
$!
$! Some UNIX NNTP servers cannot handle the NEWGROUPS command and
$! die when they receive it.  When this happens, NEWSRDR exits with a
$! PROTOERR just after connecting to the server.  Uncomment the following
$! definition if you see this behaviour.
$!
$! NEWSRDR_DEF NEWSRDR_DO_NEWGROUPS FALSE
$!
$! (You should also contact the news administrator on the server to
$!  ask why the server doesn't support NEWGROUPS.)
$!
$!-----------
$!
$! Most NNTP servers do not require clients to generate Message-ID
$! or Date headers.  If yours does, you should uncomment the following.
$! Check with the news manager at your site to make sure.
$!
$! NEWSRDR_DEF NEWSRDR_DO_MESSAGE-ID	TRUE
$! NEWSRDR_DEF NEWSRDR_DO_DATE	    	TRUE
$!
$!---- The following section is needed only NEWSRDR_DO_DATE is TRUE ----
$!  Set the following based on your locality's offset from Universal
$!  Coordinated Time.  The following is for US Eastern time.
$!
$! Use a VMS delta time specification preceded by "-" for negative
$! offsets or "+" for postive offsets.
$!
$! NEWSRDR_DEF NEWSRDR_GMT_OFFSET    "-0 05:00:00"
$!
$! Change the following to FALSE if your locality does not observe
$! US standard daylight savings time.
$!
$! NEWSRDR_DEF NEWSRDR_US_DST_ZONE   TRUE
$!
$!---- The above section is needed only NEWSRDR_DO_DATE is TRUE ----
$!
$! If your NNTP server does not require you to include a Path: header
$! in your postings, uncomment the following to have it eliminated.
$! (Note that this may break some mailed replies from those news sites
$! that still use Path for building reply addresses.)
$!
$! NEWSRDR_DEF NEWSRDR_DO_PATH	    FALSE
$!
$! Most UNIX NNTP servers prefer having the Path: header arrive in 
$! bang notation (host!user).  Uncomment the following if your server
$! prefers @-notatation (user@host).  (Ignored if NEWSRDR_DO_PATH is FALSE.)
$!
$! NEWSRDR_DEF NEWSRDR_BANG_PATH    FALSE
$!
$! Most news systems handle addresses in From and Reply-To headers
$! that are in @-notation.  Uncomment the following if you need
$! bang notation for addresses in those headers.
$!
$! NEWSRDR_DEF NEWSRDR_BANG_ADDRESS TRUE
$!
$! InterNetNews uses a special "MODE READER" command to switch to
$! doing news serving for clients.  Uncommenting the following will
$! cause NEWSRDR to send the MODE READER command when it connects
$! to the NNTP server:
$!
$! NEWSRDR_DEF NEWSRDR_INN_SERVER TRUE
$!
$ IF NEWSRDR_HELP_DIR .NES. "SYS$HELP:" THEN -
    NEWSRDR_DEF NEWSRDR_HELP 'NEWSRDR_HELP_DIR'NEWSRDR_HELP.HLB
$!
$!-----
$!
$! * IMPORTANT: You should modify the definition of NEWSRDR_PRIVS to reflect
$! * the actual privileges needed by NEWSRDR on your system to work with your
$! * mailer.
$! *
$! * The privileges required will be the same as those required for
$! * SYS$SYSTEM:MAIL.EXE.
$!
$ IF "''NEWSRDR_PRIVS'" .NES. ""
$ THEN
$   IF F$PRIVILEGE ("CMKRNL")  ! can't INSTALL without this
$   THEN
$   	icmd := create
$   	IF F$FILE ("''NEWSRDR_DIR'NEWSRDR.EXE", "KNOWN") THEN icmd := replace
$   	INSTALL 'icmd' 'NEWSRDR_DIR'NEWSRDR'NEWSRDR_SHARE''NEWSRDR_PRIVS'
$   ENDIF
$ ENDIF
$!
$!-----
$! Optional extras follow.
$!-----
$!
$! Uncomment the following if you need the Kanji character conversion
$! library:
$!
$! NEWSRDR_DEF/EXEC NEWSRDR_CHARACTER_CONVERSION -
$!                    SYS$SHARE:KANJI_CONVERSION.EXE
$! icmd := create
$! IF F$FILE ("NEWSRDR_CHARACTER_CONVERSION","KNOWN") THEN icmd := replace
$! INSTALL 'icmd' NEWSRDR_CHARACTER_CONVERSION/OPEN/SHARE/HEADER
$!
$!-----
$!
$! If you're using Message Exchange and have installed a username-to-mailname
$! conversion module for that package, you can also use it with NEWSRDR:
$!
$! IF F$TRNLNM ("MX_SITE_NAME_CONVERSION") .NES. "" THEN -
$!    	NEWSRDR_DEF/EXEC NEWSRDR_NAME_CONVERSION -
$!    	    'F$TRNLNM ("MX_SITE_NAME_CONVERSION")
$!
$! Use the following if you have installed a NEWSRDR-specific username-to
$! mailname conversion library:
$!
$! NEWSRDR_DEF/EXEC NEWSRDR_NAME_CONVERSION -
$!                    SYS$SHARE:your_name_conversion.EXE
$! icmd := create
$! IF F$FILE ("NEWSRDR_NAME_CONVERSION","KNOWN") THEN icmd := replace
$! INSTALL 'icmd' NEWSRDR_NAME_CONVERSION/OPEN/SHARE/HEADER
$!
$!-----
$!
$! Use the following if you have installed a NEWSRDR-specific mail address
$! conversion library:
$!
$! NEWSRDR_DEF/EXEC NEWSRDR_ADDRESS_CONVERSION -
$!                    SYS$SHARE:your_address_conversion.EXE
$! icmd := create
$! IF F$FILE ("NEWSRDR_ADDRESS_CONVERSION","KNOWN") THEN icmd := replace
$! INSTALL 'icmd' NEWSRDR_ADDRESS_CONVERSION/OPEN/SHARE/HEADER
$!
$ EXIT
$!
$! Subroutine for adding a directory to a search list logical name
$!
$ADD_TO_SEARCHLIST: SUBROUTINE   ! P1=lognam P2=value [P3=table]
$ IF "''P3'" .EQS. "" THEN P3 = "LNM$SYSTEM"
$ IF F$TRNLNM (P1,P3) .EQS. "" THEN EXIT
$ ACMODE = "EXECUTIVE"
$ IF F$TRNLNM (P1,P3,,"EXECUTIVE") .EQS. "" THEN ACMODE = "SUPERVISOR"
$ maxidx = F$TRNLNM (P1,P3,,ACMODE,,"MAX_INDEX")
$ i = -1
$ bookdef = ""
$loop:
$ i = i + 1
$ book = F$TRNLNM (P1,P3,i,ACMODE)
$ IF book .EQS. P2 THEN EXIT
$ bookdef = bookdef + "," + book
$ IF i .LT. maxidx THEN GOTO loop
$ bookdef = bookdef - "," + "," + P2
$ DEFINE/NOLOG/TABLE='P3'/'ACMODE' 'P1 'bookdef
$ EXIT
$ ENDSUBROUTINE
