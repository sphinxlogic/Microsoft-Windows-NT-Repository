$ VERIFY = 'F$VERIFY(F$TRNLNM("DSNLINK_NEW_VERIFY"))'
$ SET = "SET"
$ SET SYMBOL/SCOPE=(noGLOBAL,noLOCAL)
$ GOTO _START_DSNLINK_NEW

COPYRIGHT:
     Copyright © 1991-1996 by Dan Wing.  
     Copyright © 1997-2001 by MadGoat Software.
     ALL RIGHTS RESERVED.

     This code may be freely distributed and modified for no commercial
     gain.  This program is supplied 'as-is', and with no warranty of any
     kind.

INFORMATION AND BUG REPORTS:
     Bug reports:  MadGoat-Bugs@madgoat.com
     Information:  submissions:  Info-MadGoat@madgoat.com
                   subscribe:    Info-MadGoat-Request@madgoat.com
                   
ABSTRACT:
     This program will automatically check the DSNlink ITS databases for new
     articles, and Email you the article titles.

DETAILS:
     It will get list of new DSNlink article titles from the DSN host
     system, optionally compare the list to a history database of
     articles retrieved by DSNlink_NEW, and mail the article titles that
     weren't in the history database to a user or group of users.

     DSNlink_NEW can also automatically retrieve the text of specific DSNlink
     articles and mail them to a user or group of users (called "GET 
     Processing").
    
     See DSNLINK_NEW.DOC for more information.

ABOUT DSNLINK:
     DSNlink is a free service offered by Digital for software support.  It
     is available in the US and Canada, as well as most other countries
     (DSNlink is called "AES" in some regions).  Contact your Digital
     salesperson, or account representative, for information on obtaining
     DSNlink for your site.

PRIVILEGES REQUIRED:
     No special VMS privileges are required.  Access to DSN ITS, DSN$BATCH 
     queue, and ability to send DSNlink mail are necessary to use all 
     features.

ENVIRONMENT:
     DCL.  Expected to work on all versions of VMS supported by DSNlink
     software, on both VAX and AXP platforms.

REQUIRES:
     "DSNlink for OpenVMS VAX" or "DSNlink for OpenVMS AXP".  

PARAMETERS:
     P1 - [optional] Configuration filename.  If not specified,
          DSNLINK_NEW_DIR:DSNLINK_NEW_CONFIG.DAT is used.  If the logical
          DSNLINK_NEW_DIR isn't defined, the location of DSNLINK_NEW.COM is 
          used.

 {** Note -- parameters P2 through P6 are used by DSNlink_NEW.COM internally
     when it submits itself to batch.  They aren't necessary to use any
     of the features of DSNlink_NEW.  To do Get processing, simply extract
     the mail from DSNlink_NEW into a file called SYS$LOGIN:DSNLINK_GET.DAT 
     and then type "@DSNLINK_NEW" and it'll see the file and get the articles
     for you automatically.  Also see DSNLINK_NEW.DOC for more information.}

     P2 - [optional] COMPRESS, which will remove all History records older 
          than SINCE-2 days (SINCE is specified in your configuration file)
      or  [optional] $GET$, which causes GET processing to be performed (to 
          get article text).  
            NOTE:  Using this parameter is NOT normally required, as
                   DSNlink_NEW will automatically invoke GET Processing
                   if the Get file is found (the name of the get file is 
                   specified with .GET_FILE in the configuration file, and
                   defaults to SYS$LOGIN:DSNLINK_GET.DAT).  See
                   DSNLINK_NEW.DOC for more information on how to use GET
                   Processing.
      or  [optional] $CHECKOPEN$, $CHECKCLOSED$, or $CHECKDATABASES$,
          which, are used internally.

     P3 - get file - This is the file containing database names and article 
          titles we're trying to retrieve; only used if P2 is $GET$

     P4 - mail group name - only used if P2 is $GET$.

     P5 - [optional] $ERROR$ rewrite username, used only if P2 is $GET$.
          Also used in the "sent by:" line (in mail messages) if this 
          username is different from the username running DSNlink_NEW.

     P6 - Name of original GET file, used only if P2 is $GET$.  If this
          parameter is non-null, and GET Processing is successful, we'll 
          delete the file in P6.  If GET Processing is unsuccessful,
          we'll include the filename in P6 to our mail sent to the
          $ERROR$ group, allowing the user to resubmit the GET Processing
          job after the DSNlink problem has been corrected.

     P7 - number of articles to be retrieved in GET Processing (only if P2 = 
          $GET$).  Defaults to 1.

 =----------------------------------------------------------------------=

Original idea taken from DSNlink article "[DSNlink] V1.n How To Get Daily 
List of New Database Articles" in the DSNLINK database.  An early version of 
DSNlink_NEW can also be found there.

Created December 31, 1991, by Dan Wing, University Hospital, Denver.
Internet:  dwing@fuggles.com or dwing@cisco.com

 =----------------------------------------------------------------------=

  REVERSE CHRONOLOGICAL MODIFICATION HISTORY
  ------------------------------------------
  5.8	20-FEB-2001	Hunter Goatley, goathunter@goatley.com
    Incorporate additions from Bob Boyd <rlboyd@bellsouth.net>:
	Modify for correct OPEN and CLOSED call retrieval with V2.2.
	Add recurse call at beginning of procedure to make sure batch jobs
	use the most current version of the procedure.
    Also, spelling typos fixed, courtesy of Norm Raphael
    <norman.raphael@ieee.org>.

  5.7	 5-SEP-2000	Hunter Goatley, goathunter@goatley.com
     Work with DSNlink version V3.0, which is the same as V2.2, as far
     as DSNlink_NEW is concerned.  Thanks to Steve Arnold.

  5.6	 8-JAN-1998	Dan Wing, dwing@fuggles.com
     Operate with DSNlink version V2.2 (and older versions).  Suppress
     missing database email if can't connect to DSN host system.  New
     config file setting ".NOTIFY" to control /NOTIFY on Submit commands.

  5.5-1   14-JUL-1997  Malcolm Wade, Malcolm.Wade@asx.com.au
     Prevent duplicate key error during database check if SHOW DATABASE
     command displays duplicate ITS databases.  
  5.5     13-JUN-1997  Alberto Meregalli, meregalli@cesi.it
     Enhanced Get Processing so title match failure causes automatic
     retreival of the first 10 articles.
    
  5.4-6   25-NOV-1996  Dan Wing, dwing@cisco.com
     Turn on Index_Compression for database file.
  5.4-5   11-OCT-1996  Dan Wing, dwing@cisco.com
     Allow defining logical immediately prior to sending non-error mail
     which (with most SMTP mailers) can allow setting the Reply-To
     address so people can simply Reply to send stuff to the
     DSNlink_NEW_Get address and retrieve articles.  Eliminated multiple
     spaces in article titles.
  5.4-4    5-AUG-1996  Dan Wing, dwing@cisco.com
     Prefix debugging output with "## " when displaying other file's
     contents.
  5.4-3   31-JAN-1996  Dan Wing, wing@tgv.com
     Don't do history compression if already doing a database/opencalls/
     closedcalls check.
  5.4-2   18-JAN-1996  Dan Wing, wing@tgv.com
     Allow .CHECK_DATABASES to be omitted from configuration file.
  5.4-1   14-NOV-1995  Dan Wing, wing@tgv.com
     Improved history file compression by analyzing resulting file and
     re-tuning after Merging.  New history file format, with automatic
     conversion from old format to new format (longer field for database
     name).
  5.4     13-NOV-1995  Dan Wing, wing@tgv.com
     Added .CHECK_DATABASES, to check for newly-available and missing DSN
     databases.  Allow user to control Get Processing defaults via
     configuration file.  
     
  5.3-1   30-OCT-1995  Dan Wing, wing@tgv.com
     Collapse exclude strings during compare.
  5.3      2-OCT-1995  Dan Wing, wing@tgv.com
     Allow user to exclude specific article titles (which are updated
     often by Digital, causing them to re-appear in the DSNlink_NEW mail
     messages).
       
  5.2-1   11-APR-1995  Dan Wing, wing@tgv.com
     Allow user to choose when to delete Get file, so that we don't delete 
     GET file if GET Processing fails.
  5.2     31-OCT-1994  Dan Wing, wing@tgv.com
     Don't attempt to reconnect to DSNlink host if we receive "CSC call
     tracking database is unavailable" (CSCERRACC), or "application is
     not registered for this access number" (NOTREGSTRD) errors.  V1.2A
     apparently needs the same handling for GET that was introduced in
     V5.1-1, so make the "slow" GET processing happen if V1.2A is
     detected (reported by rdickens@datastream.co.uk).  Include support for
     PMDF/DELIVER for Get Processing in file PMDF_DELIVER.COM_TEMPLATE.

  5.1-3    3-AUG-1994  Dan Wing, wing@tgv.com
     Allow GET file to have ">" or "#" as first character (in case Forwarding
     automatically inserts 'quote' characters).  Allow specifying priority of
     DSNlink_NEW when it is doing batch job work with .BATCH_PRIORITY.
  5.1-2   10-MAY-1994  Dan Wing, wing@tgv.com
     Use /KEY=&HISTORY_KEY to eliminate problem with single quotes in the
     title retrieved from the DSNlink host.  Thanks to Larry Stone. 
  5.1-1    4-NOV-1993  Dan Wing, dwing@uh01.colorado.edu
     Automatically sense if we're using a version of DSNlink older than V1.2
     -- old versions (V1.1 and V1.1-1) cannot use EDT as their callable editor
     and don't work with GET processing's use of EDT -- so we automatically
     switch to a less efficient method of retrieving articles (pulling them
     over the modem twice).
  5.1     22-SEP-1993  Dan Wing, dwing@uh01.colorado.edu
     Enhancements to GET processing to work with the MX SITE agent, DELIVER,
     and PMDF.  Fix bug when .CHECK_OPENCALLS wasn't in the configuration
     file, and stopped playing with protection of the temporary file created
     for GET processing.  New .SCRATCH_DIR configuration file setting for
     DSNlink_NEW temporary files.  Added .CHECK_CLOSEDCALLS to complement
     .CHECK_OPENCALLS.

  5.0     11-SEP-1993  Dan Wing, dwing@uh01.colorado.edu
     Ability to automatically retrieve article text (not just titles).  Show
     number of articles retrieved from each database.  Inform $ERROR$ when we
     get UNKDBNAM error (instead of everyone).  Ability to send mail to
     DSN%OPENCALLS on a schedule.  Don't update history date every time we
     re-retrieve an article -- only update the date when it is new, or if it
     was last retrieved ALLOW_IF_OLDER (or more) days ago.  Configuration file
     parser no longer uppercases everything.  Fixed other problems and
     inconsistencies with case-sensitivity and error message display.  Send
     Email for configuration file errors when running in batch.  Deliver mail
     to all recipients even if DECnet mail can't be sent to certain nodes.

  4.3-1   27-JUN-1993  Dan Wing, dwing@uh01.colorado.edu
     Ensure history file exists before trying to display its size.
  4.3     24-JUN-1993  Dan Wing, dwing@uh01.colorado.edu
     Add ability to automatically compress history file.

  4.2-2   13-JUN-1993  Dan Wing, dwing@uh01.colorado.edu
     Fixed bug so database name is now displayed when history is disabled.
  4.2-1    5-MAY-1993  Dan Wing, dwing@uh01.colorado.edu
     When Emailing about errors, use several common message files to get
     more information.  Changed default SINCE time to TODAY-10.
  4.2     19-APR-1993  Dan Wing, dwing@uh01.colorado.edu
     Separate history for flash and normal databases by using uppercase/
     lowercase for database name within the history database.
  
  Older modification history is available in the DSNlink_NEW documentation.

 =----------------------------------------------------------------------=

$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$_START_DSNLINK_NEW:
$
$  ON WARNING THEN GOTO _ERROR
$  ON CONTROL_Y THEN GOTO _ABORT
$
$!
$
$  DSNLINK_NEW_VERSION = "V5.8"
$
$  GOSUB _INIT_DEFAULTS
$ show symbol/local/all
$ show symbol/global/all
$!
$ proc = f$environment("procedure")
$ proc_name = f$parse(proc,,,"name")
$ write sys$output f$time(),"Executing ",proc," @ DCL Level ",f$environment("depth")
$ submitted_version = f$parse(proc,,,"version")
$ proc = f$PARSE(";0",proc) ! force to current version
$!
$! See if this is the most current version or if I need to recurse
$! If it isn't, but I've already recursed 1 time, don't do it again.
$!
$ if	( f$parse(f$search(proc),,,"version").nes.submitted_version ) -
  .and.	( f$type('proc_name'_recurse).eqs."" )
$ then
$	'proc_name'_recurse = 1
$	set noon
$	@'proc' "''p1'" "''p2'" "''p3'" "''p4'" "''p5'" "''p6'" "''p7'" "''p8'"
$	EXIT $STATUS
$ endif
$!
$ if f$mode().nes."INTERACTIVE" then $ set output_rate	! flush log buffers to log file 
$! remainder of code follows 
$!
$!
$  IF P1 .EQS. "$VERSION$"
$  THEN
$    SET SYMBOL/SCOPE=GLOBAL
$    DSNLINK_NEW_VERSION == DSNLINK_NEW_VERSION
$    VERIFY = F$VERIFY(VERIFY)
$    EXIT   ! Leaving DSNlink_NEW ...
$  ENDIF
$
$  WRITE SYS$OUTPUT F$FAO("!/DSNlink_NEW, version !AS.!/Copyright © 1991-1996 Dan Wing.  Copyright © 1997-2001 MadGoat Software.!/Started by !AS on node !AS on !AS, !AS !8AS.", -
     DSNLINK_NEW_VERSION, F$EDIT(F$GETJPI(0,"USERNAME"),"TRIM"), -
     F$GETSYI("NODENAME"), F$CVTIME(,,"WEEKDAY"), F$CVTIME(,"ABSOLUTE","DATE"), F$CVTIME(,,"TIME"))
$
$  GOSUB _INIT_DEFAULTS
$
$  IF F$TRNLNM("DSN$SYSTEM") .EQS. "" .AND. -
      F$TRNLNM("DSN_SYSTEM") .EQS. "" THEN GOTO _NO_DSNLINK
$
$  WRITE SYS$OUTPUT F$FAO("DSNlink version !AS.!/", DSN$VERSION)
$
$  IF F$SEARCH(DSN_CONFIG_FILENAME) .EQS. "" THEN GOTO _NO_CONFIG_FILE
$  GOSUB _INIT_PARSE_CONFIG_FILE
$  IF CONFIG_ERROR THEN GOTO _EXIT_ERROR
$  IF BATCH .AND. (LOW_PRIORITY .NE. PRIB) THEN SET PROCESS/PRIORITY='LOW_PRIORITY''DEBUG_LOG'
$
$  ALLOW_IF_OLDER = F$CVTIME("''F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE")'-2-00:00","COMPARISON","DATE")
$
$  IF (INTERACTIVE .AND. F$SEARCH(GET_FILE) .NES. "") .OR. -
      (P2 .EQS. "$GET$") THEN GOTO _DSNLINK_GET
$
$  IF (BATCH .AND. P2 .EQS. "$CHECKOPEN$") 
$  THEN
$    GOSUB _DO_CHECK_OPENCALLS
$    FINISHED_OK = TRUE
$    GOTO _EXIT
$  ENDIF
$
$  IF (BATCH .AND. P2 .EQS. "$CHECKCLOSED$")
$  THEN
$    GOSUB _DO_CHECK_CLOSEDCALLS
$    FINISHED_OK = TRUE
$    GOTO _EXIT
$  ENDIF
$
$  IF (BATCH .AND. P2 .EQS. "$CHECKDATABASES$") THEN GOTO _SHOW_CONFIGURATION
$
$  IF P2 .EQS. "COMPRESS" 
$  THEN 
$    GOSUB _COMPRESS_HISTORY
$    GOTO _EXIT
$  ENDIF
$
$  IF P2 .NES. "" THEN GOTO _BAD_P2
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$_SHOW_CONFIGURATION:
$  COUNT = 0
$  WRITE SYS$OUTPUT F$FAO("  !24<Connection retries:!>!UL", DSN_RETRY_MAX)
$  IF F$EDIT(DSN_AFTER,"UPCASE") .NES. "ONE" 
$  THEN 
$    WRITE SYS$OUTPUT F$FAO("  !24<Next run at:!>!AS (!AS, !AS)", -
     DSN_AFTER, -
     F$CVTIME(DSN_AFTER,,"WEEKDAY"), -
     F$CVTIME(DSN_AFTER,"ABSOLUTE") - ":00.00" - ".00")
$  ELSE
$    WRITE SYS$OUTPUT F$FAO("  !24<Next run at:!>(only one run)")
$  ENDIF
$  WRITE SYS$OUTPUT F$FAO("  !24<History file:!>!AS", DSN_HISTORY_FILE)
$
$  IF DSN_HISTORY_FILE .NES. "NONE" .AND. F$SEARCH(DSN_HISTORY_FILE) .NES. ""
$  THEN 
$    IF DSN_HISTORY_AUTO_COMPRESS .GT. 0 
$    THEN
$      WRITE SYS$OUTPUT F$FAO("  !24<History auto compress:!>at !UL blocks (current size !UL blocks)", -
     DSN_HISTORY_AUTO_COMPRESS, F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF"))
$      IF F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF") .GT. DSN_HISTORY_AUTO_COMPRESS
$      THEN
$        WRITE SYS$OUTPUT F$FAO("!26* compression will be performed")
$        PERFORM_AUTO_COMPRESS = TRUE
$      ENDIF ! history auto compress will be performed
$    ELSE
$      WRITE SYS$OUTPUT F$FAO("  !24<History auto compress:!>Disabled")
$    ENDIF  ! history auto compress enabled
$    ! convert from old history file to new one 
$    IF F$FILE(DSN_HISTORY_FILE,"MRS") .EQ. 108 .AND. BATCH
$    THEN 
$      GOSUB _NEW_HISTORY_FORMAT 
$      IF DSN_HISTORY_AUTO_COMPRESS .GT. 0 THEN GOSUB _COMPRESS_HISTORY
$    ENDIF ! old history file format
$  ENDIF ! dsn_history_file <> none
$
$  IF WRITE_TITLES .NES. "" THEN WRITE SYS$OUTPUT F$FAO("  !24<Write all titles to:!>!AS", WRITE_TITLES)
$
$  IF F$TYPE(EXCLUDE_ARTICLE_0) .NES. ""
$  THEN
$    WRITE SYS$OUTPUT F$FAO("  Excluded article!0UL!%S:", MAX_EXC_COUNT + 1)
$    EXC_COUNT = 0
$    _DIS_EXC_LOOP:
$    IF F$TYPE(EXCLUDE_ARTICLE_'EXC_COUNT') .NES. "" 
$    THEN 
$      IF F$LENGTH(EXCLUDE_ARTICLE_'EXC_COUNT') .GT. 52
$      THEN
$        WRITE SYS$OUTPUT F$FAO("    !20<!AS!>  !52<!AS!>~", EXCLUDE_DB_'EXC_COUNT', EXCLUDE_ARTICLE_'EXC_COUNT')
$      ELSE
$        WRITE SYS$OUTPUT F$FAO("    !20<!AS!>  !52<!AS!>", EXCLUDE_DB_'EXC_COUNT', EXCLUDE_ARTICLE_'EXC_COUNT')
$      ENDIF
$    ENDIF
$    EXC_COUNT = EXC_COUNT + 1
$    IF EXC_COUNT .LE. MAX_EXC_COUNT THEN GOTO _DIS_EXC_LOOP
$  ENDIF
$
$  IF F$EDIT(DSN_CHECK_OPENCALLS,"UPCASE") .NES. "NEVER" 
$  THEN
$    WRITE SYS$OUTPUT F$FAO("  !24<Check opencalls:!>!AS, user: !AS", DSN_CHECK_OPENCALLS, DSN_CHECK_OPENCALLS_USER)
$  ELSE
$    WRITE SYS$OUTPUT F$FAO("  !24<Check opencalls:!>Never")
$  ENDIF
$
$  IF F$EDIT(DSN_CHECK_CLOSEDCALLS,"UPCASE") .NES. "NEVER"
$  THEN 
$    WRITE SYS$OUTPUT F$FAO("  !24<Check closedcalls:!>!AS, user: !AS", DSN_CHECK_CLOSEDCALLS, DSN_CHECK_CLOSEDCALLS_USER)
$  ELSE
$    WRITE SYS$OUTPUT F$FAO("  !24<Check closedcalls:!>Never")
$  ENDIF
$
$  IF F$EDIT(DSN_CHECK_DATABASES,"UPCASE") .NES. "NEVER"
$  THEN
$    WRITE SYS$OUTPUT F$FAO("  !24<Check new databases:!>!AS, mail group: !AS", DSN_CHECK_DATABASES, DSN_CHECK_DATABASES_USER)
$    IF F$GETQUI("DISPLAY_QUEUE","JOB_LIMIT","DSN$BATCH") .LT. 2
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-JOB_LIM, To help performance, increase job limit on DSN$BATCH"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-JOB_LIM, queue to 2 when using CHECK_DATABASES."
$    ENDIF
$  ELSE
$    WRITE SYS$OUTPUT F$FAO("  !24<Check new databases:!>Never")
$  ENDIF
$
$  IF DSN_CHECK_DATABASES_FILE_DEFAULT .NES. DSN_CHECK_DATABASES_FILE THEN -
     WRITE SYS$OUTPUT F$FAO("  !24<New databases file:!>!AS", DSN_CHECK_DATABASES_FILE)
$
$  WRITE SYS$OUTPUT F$FAO("  !24<Since:!>!AS (!AS, !AS)", DSN_SINCE, -
     F$CVTIME(DSN_SINCE,,"WEEKDAY"), F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE"))
$  IF DSN_MAIL_EMPTY
$  THEN
$    WRITE SYS$OUTPUT F$FAO("  !24<Send mail if empty:!>Yes")
$  ELSE
$    WRITE SYS$OUTPUT F$FAO("  !24<Send mail if empty:!>No")
$  ENDIF
$  WRITE SYS$OUTPUT F$FAO("  !24<Batch log file:!>!AS", F$PARSE(LOG_FILE) - ";")
$  IF LOW_PRIORITY .NES. PRIB THEN WRITE SYS$OUTPUT F$FAO("  !24<Batch priority:!>!UL", LOW_PRIORITY)
$  WRITE SYS$OUTPUT F$FAO("  !24<Get file:!>!AS", F$PARSE(GET_FILE) - ";")
$
$_DB_DISPLAY_LOOP:
$  IF MAIL_GROUP_NAME_'COUNT' .NES. "$ERROR$" 
$  THEN
$    TMP = F$FAO("!/Will send !AS new articles from databases: ", -
       MAIL_GROUP_USERS_'COUNT') 
$    GROUP_NAME = MAIL_GROUP_NAME_'COUNT'
$    CALL _DISPLAY_DATABASES SYS$OUTPUT 0 "''TMP'" "''GROUP_NAME'" 2
$  ELSE
$    WRITE SYS$OUTPUT F$FAO("!/Will send !AS any errors.", MAIL_GROUP_USERS_'COUNT')
$  ENDIF  ! not error group
$  COUNT = COUNT + 1
$  IF COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _DB_DISPLAY_LOOP
$
$  WRITE SYS$OUTPUT ""
$  IF BATCH THEN SET OUTPUT_RATE        ! flush stuff to LOG file 
$  IF BATCH .AND. DEBUG THEN SET OUTPUT_RATE=00:00:05
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  ! this needs to be down here so we display the config. information
$  IF (BATCH .AND. P2 .EQS. "$CHECKDATABASES$") 
$  THEN
$    GOSUB _DO_CHECK_DATABASES
$    FINISHED_OK = TRUE
$    GOTO _EXIT
$  ENDIF
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  IF INTERACTIVE
$  THEN
$    ASK "* Is this correct? [Y] " OK_TO_CONTINUE
$    IF (.NOT. OK_TO_CONTINUE) .AND. (OK_TO_CONTINUE .NES. "") THEN GOTO _ABORT
$  ENDIF
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  IF DSN_HISTORY_FILE .NES. "NONE"
$  THEN
$    IF F$SEARCH(DSN_HISTORY_FILE) .EQS. "" 
$    THEN
$      CREATE/FDL=SYS$INPUT:/LOG 'DSN_HISTORY_FILE'
FILE
  BEST_TRY_CONTIGUOUS  yes
  ORGANIZATION         INDEXED
  EXTENSION            9           ! in case it never gets COMPRESSed
RECORD
  FORMAT  FIXED
  SIZE    120        ! date (YYYY-MM-DD) + database + title = 10 + 32 + 78
KEY 0
  INDEX_COMPRESSION  no
  DUPLICATES         no
  NULL_KEY           no
  SEG0_LENGTH        110
  SEG0_POSITION      10
$    ELSE
$      ! test to ensure we can R+W the history file
$      OPEN/READ/WRITE/SHARE=WRITE DSNLINK_NEW_OUTFILE 'DSN_HISTORY_FILE'
$      CLOSE DSNLINK_NEW_OUTFILE
$    ENDIF  ! history file exists
$  ENDIF ! history disabled
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  IF INTERACTIVE .OR. F$EDIT(DSN_AFTER,"UPCASE") .NES. "ONE"
$  THEN
$    IF F$EDIT(DSN_AFTER,"UPCASE") .EQS. "ONE"
$    THEN
$      AFTER_QUAL = ""        ! right now
$    ELSE
$      AFTER_QUAL = "/AFTER=" + QUOTE + F$CVTIME(DSN_AFTER,"ABSOLUTE") + QUOTE
$    ENDIF
$
$    SUBMIT -
     'DSN_NOTIFY' -
     /QUEUE=DSN$BATCH -
     /noPRINTER -
     /KEEP -
     'AFTER_QUAL' -
     /PARAMETERS=('F$PARSE(";",DSN_CONFIG_FILENAME)')  -
     /NAME="DSNlink_NEW" -
     /LOG_FILE='LOG_FILE' -
     'F$PARSE(";",F$ENVIRONMENT("PROCEDURE"))'
$
$    IF F$EDIT(DSN_AFTER,"UPCASE") .EQS. "ONE" .OR. INTERACTIVE
$    THEN
$      FINISHED_OK = TRUE
$      GOTO _EXIT
$    ENDIF  ! dsn_after = "one" or interactive
$  ENDIF ! interactive or dsn_after = "one"
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$! See if we need to send mail to DSN%OPENCALLS and DSN%CLOSEDCALLS.
$! we delay sending the actual mail to immediately before we connect to the
$! host system to try to get two 'streams' going to the host system at 
$! once to shorten the total time we're using the modem.
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  DO_CHECK_OPENCALLS   = FALSE
$
$  IF F$EDIT(DSN_CHECK_OPENCALLS,"UPCASE") .NES. "NEVER" 
$  THEN
$    COUNT = 0
$    _OPENCALLS_CHECK:
$    CHECK_VALUE = F$ELEMENT(COUNT,",",DSN_CHECK_OPENCALLS)
$    IF CHECK_VALUE .EQS. "," THEN GOTO _STOP_OPENCALLS_CHECK
$    IF F$TYPE(CHECK_VALUE) .EQS. "INTEGER"
$    THEN
$    ! it is a day of the month (1, 2, 3)
$      IF F$CVTIME(,,"DAY") .EQ. CHECK_VALUE
$      THEN
$        DO_CHECK_OPENCALLS = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, OPENCALLS triggered by \", CHECK_VALUE, "\"
$      ENDIF
$    ELSE
$    ! it is the day of a week (Monday, Tuesday, ...)
$      IF F$EDIT(F$CVTIME(,,"WEEKDAY"),"UPCASE") .EQS. F$EDIT(CHECK_VALUE,"UPCASE") 
$      THEN
$        DO_CHECK_OPENCALLS = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, OPENCALLS triggered by \", CHECK_VALUE, "\"
$      ENDIF
$      IF F$EDIT(CHECK_VALUE,"UPCASE") .EQS. "ALWAYS"
$      THEN
$        DO_CHECK_OPENCALLS = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, OPENCALLS triggered by \", CHECK_VALUE, "\"
$      ENDIF
$    ENDIF
$    COUNT = COUNT + 1
$    IF .NOT. DO_CHECK_OPENCALLS THEN GOTO _OPENCALLS_CHECK
$  ENDIF ! don't check
$
$_STOP_OPENCALLS_CHECK:
$  IF DO_CHECK_OPENCALLS THEN GOSUB _DO_CHECK_OPENCALLS
$
$!
$!!!
$!
$
$  DO_CHECK_CLOSEDCALLS = FALSE
$
$  IF F$EDIT(DSN_CHECK_CLOSEDCALLS,"UPCASE") .NES. "NEVER" 
$  THEN
$    COUNT = 0
$    _CLOSEDCALLS_CHECK:
$    CHECK_VALUE = F$ELEMENT(COUNT,",",DSN_CHECK_CLOSEDCALLS)
$    IF CHECK_VALUE .EQS. "," THEN GOTO _STOP_CLOSEDCALLS_CHECK
$    IF F$TYPE(CHECK_VALUE) .EQS. "INTEGER"
$    THEN
$    ! it is a day of the month (1, 2, 3)
$      IF F$CVTIME(,,"DAY") .EQ. CHECK_VALUE
$      THEN
$        DO_CHECK_CLOSEDCALLS = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CLOSEDCALLS triggered by \", CHECK_VALUE, "\"
$      ENDIF
$    ELSE
$    ! it is the day of a week (Monday, Tuesday, ...)
$      IF F$EDIT(F$CVTIME(,,"WEEKDAY"),"UPCASE") .EQS. F$EDIT(CHECK_VALUE,"UPCASE") 
$      THEN
$        DO_CHECK_CLOSEDCALLS = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CLOSEDCALLS triggered by \", CHECK_VALUE, "\"
$      ENDIF
$      IF F$EDIT(CHECK_VALUE,"UPCASE") .EQS. "ALWAYS"
$      THEN
$        DO_CHECK_CLOSEDCALLS = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CLOSEDCALLS triggered by \", CHECK_VALUE, "\"
$      ENDIF
$    ENDIF
$    COUNT = COUNT + 1
$    IF .NOT. DO_CHECK_CLOSEDCALLS THEN GOTO _CLOSEDCALLS_CHECK
$  ENDIF ! don't check
$
$_STOP_CLOSEDCALLS_CHECK:
$  IF DO_CHECK_CLOSEDCALLS THEN GOSUB _DO_CHECK_CLOSEDCALLS
$
$!
$!!!
$!
$
$  DO_CHECK_DATABASES = FALSE
$
$  IF F$EDIT(DSN_CHECK_DATABASES,"UPCASE") .NES. "NEVER" 
$  THEN
$    COUNT = 0
$    _DATABASES_CHECK:
$    CHECK_VALUE = F$ELEMENT(COUNT,",",DSN_CHECK_DATABASES)
$    IF CHECK_VALUE .EQS. "," THEN GOTO _STOP_DATABASES_CHECK
$    IF F$TYPE(CHECK_VALUE) .EQS. "INTEGER"
$    THEN
$    ! it is a day of the month (1, 2, 3)
$      IF F$CVTIME(,,"DAY") .EQ. CHECK_VALUE
$      THEN
$        DO_CHECK_DATABASES = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CHECKDATA triggered by \", CHECK_VALUE, "\"
$      ENDIF
$    ELSE
$    ! it is the day of a week (Monday, Tuesday, ...)
$      IF F$EDIT(F$CVTIME(,,"WEEKDAY"),"UPCASE") .EQS. F$EDIT(CHECK_VALUE,"UPCASE") 
$      THEN
$        DO_CHECK_DATABASES = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CHECKDATA triggered by \", CHECK_VALUE, "\"
$      ENDIF
$      IF F$EDIT(CHECK_VALUE,"UPCASE") .EQS. "ALWAYS"
$      THEN
$        DO_CHECK_DATABASES = TRUE
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CHECKDATA triggered by \", CHECK_VALUE, "\"
$      ENDIF
$    ENDIF
$    COUNT = COUNT + 1
$    IF .NOT. DO_CHECK_DATABASES THEN GOTO _DATABASES_CHECK
$  ENDIF ! don't check
$
$_STOP_DATABASES_CHECK:
$! following just causes batch job to be submitted
$  IF DO_CHECK_DATABASES THEN GOSUB _DO_CHECK_DATABASES
$
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!  Build input file for "DSN ITS" command
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  GOSUB _DELETE_TMP_FILES
$  
$  OPEN/WRITE DSNLINK_NEW_OUTFILE 'TMP_DSNFILE'
$
$  COUNT = 0
$  DB_NAME = ""
$_FILE_LOOP_1:
$  IF DB_NAME .NES. "" 
$  THEN 
$    WRITE DSNLINK_NEW_OUTFILE "close ", DB_NAME
$    WRITE DSNLINK_NEW_OUTFILE ""
$  ENDIF
$  DB_NAME = DB_NAME_'COUNT'
$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT)
$  DB_DIRFILE_FLASH = DB_DIRFILE + "_FLASH"
$  WRITE DSNLINK_NEW_OUTFILE "open ", DB_NAME
$  IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_22
$  THEN
$    WRITE DSNLINK_NEW_OUTFILE "directory/all/beginning=", F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE")
$  ELSE
$    WRITE DSNLINK_NEW_OUTFILE "directory/all/since=", QUOTE, F$CVTIME(DSN_SINCE,"ABSOLUTE"), QUOTE
$  ENDIF
$  WRITE DSNLINK_NEW_OUTFILE "extract/directory ", DB_DIRFILE
$  IF DB_FLASH_'COUNT'
$  THEN
$    IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_22
$    THEN
$      WRITE DSNLINK_NEW_OUTFILE "directory/all/flash/beginning=", F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE")
$    ELSE
$      WRITE DSNLINK_NEW_OUTFILE "directory/all/flash/since=", QUOTE, F$CVTIME(DSN_SINCE,"ABSOLUTE"), QUOTE
$    ENDIF
$    WRITE DSNLINK_NEW_OUTFILE "extract/directory ", DB_DIRFILE_FLASH
$  ENDIF ! symbol is TRUE
$  COUNT = COUNT + 1
$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _FILE_LOOP_1
$
$  WRITE DSNLINK_NEW_OUTFILE "exit"
$  CLOSE DSNLINK_NEW_OUTFILE
$
$  WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-I-COUNT, !UL database!%S will be checked for new articles.", COUNT)
$
$!
$!!! connect to DSNlink host system
$!
$
$  GOSUB _PHONE_DSN_HOST
$
$  IF F$SEARCH("''TMP_EXTRACTFILE'*",2) .EQS. "" THEN GOTO _NO_DSNLINK_ARTICLES
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!  Found DSNlink articles.
$!!!  We need to remove articles we've already seen (history records), and
$!!!  then generate the appropriate mail messages
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  IF BATCH THEN SET RESTART_VALUE="Processing retrieved articles"
$  WRITE SYS$OUTPUT "%DSNlink_NEW-S-NEWART, DSNlink articles were retrieved from DSNlink host."
$
$! Remove history records 
$  COUNT = 0
$_PROCESS_HIST_LOOP:
$  DB_NAME = DB_NAME_'COUNT'
$
$  DSNLINK_FLASH = TRUE
$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT) + "_FLASH"
$  IF F$SEARCH(DB_DIRFILE) .NES. "" THEN GOSUB _ELIMINATE_HISTORY_RECORDS
$
$  DSNLINK_FLASH = FALSE
$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT) 
$  IF F$SEARCH(DB_DIRFILE) .NES. "" THEN GOSUB _ELIMINATE_HISTORY_RECORDS
$
$  COUNT = COUNT + 1
$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _PROCESS_HIST_LOOP
$  
$_GENERATE_MAIL_MESSAGES:
$!
$! generate mail messages for each group, as defined in config file
$  MAIL_COUNT = 0
$  _GEN_ART_LOOP:
$  SELECT_MAIL_GROUP_NAME = MAIL_GROUP_NAME_'MAIL_COUNT'
$  IF SELECT_MAIL_GROUP_NAME .NES. "$ERROR$"
$  THEN
$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-S-GENART, Generating mail for group !AS", SELECT_MAIL_GROUP_NAME)
$    GOSUB _BUILD_MESSAGE
$    IF F$SEARCH(TMP_MAILFILE) .NES. ""
$    THEN
$      TMP1 = F$FAO(FAO_SUBJECT, TOTAL_FLASH_ARTICLES, TOTAL_NORMAL_ARTICLES, TOTAL_FLASH_ARTICLES + TOTAL_NORMAL_ARTICLES, SELECT_MAIL_GROUP_NAME, TOTAL_DATABASES)
$      TMP2 = F$FAO(FAO_PERSONAL, TOTAL_FLASH_ARTICLES, TOTAL_NORMAL_ARTICLES, TOTAL_FLASH_ARTICLES + TOTAL_NORMAL_ARTICLES, SELECT_MAIL_GROUP_NAME, TOTAL_DATABASES)
$      IF WRITE_TITLES .NES. "" 
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-I-WRITETITLES, Writing titles to: ", WRITE_TITLES
$        APPEND 'TMP_MAILFILE' 'WRITE_TITLES'/NEW_VERSION/LOG
$      ENDIF
$      CALL _SEND_MAIL_MESSAGE -
     'TMP_MAILFILE' -
     "''TMP1'" -
     "''SELECT_MAIL_GROUP_NAME'" -
     "''TMP2'"
$      DELETE 'TMP_MAILFILE';*
$    ELSE   ! everything was rejected because of history and/or database wasn't selected
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-NOSELECTED, No articles for group ", SELECT_MAIL_GROUP_NAME
$      IF DSN_MAIL_EMPTY 
$      THEN 
$        CREATE 'TMP_MAILFILE'
No new DSNlink articles were found that matched configuration criteria.
$        CALL _SEND_MAIL_MESSAGE 'TMP_MAILFILE' "No new ''SELECT_MAIL_GROUP_NAME' DSNlink articles" "''SELECT_MAIL_GROUP_NAME'"
$        DELETE 'TMP_MAILFILE';*
$      ENDIF ! dsn_mail_empty
$    ENDIF ! no file to mail
$  ENDIF ! error group
$  MAIL_COUNT = MAIL_COUNT + 1
$  IF MAIL_COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _GEN_ART_LOOP
$  FINISHED_OK = TRUE
$  GOTO _POST_PROCESSING
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!  Call DSNlink host, and retry as necessary
$!  This routine is used by "normal" DSNlink_NEW as well as DSNlink_NEW's
$!  "get" routines.
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_PHONE_DSN_HOST:
$  IF BATCH THEN SET RESTART_VALUE="Calling DSNlink host"
$! build DCL jacket around the DSN ITS commands
$! (we have to use this method instead of the old DSN$ITS_INIT method
$!  because EDT was rewinding the input file every time it was invoked, but
$!  it doesn't rewind stuff fed to it in this manner).
$  SET FILE/VERSION_LIMIT=2 'TMP_DSNFILE'
$  OPEN/WRITE DSNLINK_NEW_OUTFILE 'TMP_DSNFILE';       ! new version
$
$  IF PERFORMING_GET
$  THEN
$    IF GET_STYLE .EQS. "EDT"
$    THEN
$      WRITE DSNLINK_NEW_OUTFILE "$! GET_STYLE = EDT"
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE DSN$ITS_EDIT CALLABLE_EDT"
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE EDTSYS NLA0:"
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE EDTINI NLA0:"
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE SYS$ERROR ", TMP_ERRORFILE
$      ! Note: defining SYS$OUTPUT seems to mess up EDT (gives %SYSTEM-F-IVDEVNAM)
$
$      ! Note on SYS$SCRATCH (DSNlink V1.2b):
$      !   when run from user DSN$SERVER, DSN$MAIN.EXE seems to leave on SYSPRV
$      !   which causes problems when trying to delete edit-journal file owned 
$      !   by the owner of the scratch directory ([SYSTEM]) -- so, as a work-
$      !   around, we set DSN$SCRATCH to point to a directory that is owned
$      !   by our UIC no matter what (this controls the creation of the 
$      !   DSN$ITS_pid.TMP file which contains the file we're editing).
$      !   This problem doesn't seem to exist with privileged usernames
$      !   other than DSN$SERVER.
$      !
$      !   In case future versions of DSNlink use SYS$SCRATCH instead of
$      !   DSN$SCRATCH for the location of this temporary file, we're going
$      !   to set both SYS$SCRATCH and DSN$SCRATCH just to be sure.
$      !
$      !   Note that this means the DSN ITS commands (which we're about to 
$      !   execute) cannot contain references to the logicals SYS$SCRATCH or 
$      !   DSN$SCRATCH, as we're only temporarily re-defining them to point
$      !   to SYS$LOGIN (or else they'll look in the wrong places!)
$      !
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE DSN$SCRATCH ", F$PARSE("SYS$LOGIN") - ".;"
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE SYS$SCRATCH ", F$PARSE("SYS$LOGIN") - ".;"
$    ENDIF
$    IF GET_STYLE .EQS. "EXTRACT"
$    THEN
$      WRITE DSNLINK_NEW_OUTFILE "$! GET_STYLE = EXTRACT"
$      IF DSNLINK_STYLE .NE. DSNLINK_STYLE_22
$      THEN
$        WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE SYS$ERROR ", TMP_ERRORFILE
$        WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE SYS$OUTPUT ", TMP_OUTFILE
$      ELSE
$        WRITE DSNLINK_NEW_OUTFILE "$! DSNlink 2.2 doesn't handle SYS$ERROR/SYS$OUTPUT redirected with GET Processing"
$        WRITE DSNLINK_NEW_OUTFILE "$ CREATE ", TMP_ERRORFILE
$        WRITE DSNLINK_NEW_OUTFILE "<empty>"
$        WRITE DSNLINK_NEW_OUTFILE "$ CREATE ", TMP_OUTFILE
$        WRITE DSNLINK_NEW_OUTFILE "<empty>"
$        WRITE DSNLINK_NEW_OUTFILE "$"
$      ENDIF
$    ENDIF
$  ELSE
$    IF .NOT. CHECKING_DATABASES
$    THEN
$      WRITE DSNLINK_NEW_OUTFILE "$! normal DSNlink_NEW processing"
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE SYS$ERROR ", TMP_ERRORFILE
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE SYS$OUTPUT ", TMP_OUTFILE
$    ELSE
$      WRITE DSNLINK_NEW_OUTFILE "$! check database DSNlink_NEW processing"
$      WRITE DSNLINK_NEW_OUTFILE "$! (cannot use /USER_MODE on the DEFINEs)"
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE SYS$ERROR ", TMP_ERRORFILE
$      WRITE DSNLINK_NEW_OUTFILE "$ DEFINE SYS$OUTPUT ", TMP_OUTFILE
$    ENDIF
$  ENDIF
$
$  WRITE DSNLINK_NEW_OUTFILE "$"
$  IF DSNLINK_STYLE .NE. DSNLINK_STYLE_22
$  THEN
$    WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE DSN$ITS_INIT NLA0:"
$  ELSE
$    WRITE DSNLINK_NEW_OUTFILE "$ DEFINE/USER_MODE DSN$ITS_INIT NLA0:"  !&& does 2.2 provide a way to disable the init file?
$  ENDIF
$
$  WRITE DSNLINK_NEW_OUTFILE "$"
$  WRITE DSNLINK_NEW_OUTFILE "$ SET noON"
$  WRITE DSNLINK_NEW_OUTFILE "$ DSN ITS"
$  WRITE DSNLINK_NEW_OUTFILE "$ deck"
$  CLOSE DSNLINK_NEW_OUTFILE
$
$  APPEND 'TMP_DSNFILE';-1 'TMP_DSNFILE'
$
$  OPEN/APPEND DSNLINK_NEW_OUTFILE 'TMP_DSNFILE'
$  IF F$EXTRACT(1,-1,DSN$VERSION) .LTS. "1.2" .OR. -
      DSNLINK_STYLE .EQ. DSNLINK_STYLE_22 THEN -
      WRITE DSNLINK_NEW_OUTFILE "no"  ! in case DSN ITS asks for a survey (V1.2 is the only version which doesn't ask for survey if we're non-interactive)
$  WRITE DSNLINK_NEW_OUTFILE "$ eod"
$  WRITE DSNLINK_NEW_OUTFILE "$ SAVESTATUS = $STATUS"
$  IF CHECKING_DATABASES
$  THEN
$    WRITE DSNLINK_NEW_OUTFILE "$ DEASSIGN SYS$OUTPUT"
$    WRITE DSNLINK_NEW_OUTFILE "$ DEASSIGN SYS$ERROR"
$  ENDIF
$  WRITE DSNLINK_NEW_OUTFILE "$ EXIT ''SAVESTATUS'"
$  CLOSE DSNLINK_NEW_OUTFILE
$
$  IF DEBUG
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, .COM file to connect to DSNlink host:"
$    MERGE 'TMP_DSNFILE' SYS$OUTPUT/SPECIFICATION=SYS$INPUT/noCHECK_SEQUENCE
/FIELD=(NAME=TEXT,POSITION:1,SIZE=150)
/DATA="## "
/DATA=TEXT
$  ENDIF   ! DEBUG
$
$!
$!!!
$!
$
$_DSN_RETRY:
$  WRITE SYS$OUTPUT ""
$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-ATTCONN, Attempting connection at ", F$TIME()
$  IF BATCH THEN SET OUTPUT_RATE  ! flush stuff to LOG file
$
$  ATTEMPT_CONNECT_TIME = F$CVTIME(,,"TIME")
$  ATTEMPT_CONNECT_DATE = F$CVTIME(,,"DATE")
$  DSN_ATTEMPT_COUNT = DSN_ATTEMPT_COUNT + 1
$
$  IF F$SEARCH(TMP_ERRORFILE) .NES. "" THEN DELETE 'TMP_ERRORFILE';*
$  IF F$SEARCH(TMP_OPENCALLSFILE) .NES. "" THEN DELETE 'TMP_OPENCALLSFILE';*
$  IF F$SEARCH(TMP_CLOSEDCALLSFILE) .NES. "" THEN DELETE 'TMP_CLOSEDCALLSFILE';*
$  IF F$SEARCH(TMP_OUTFILE) .NES. "" THEN DELETE 'TMP_OUTFILE';*
$
$! we're currently running at LOW_PRIORITY; if this is lower than what
$! we started with, go back up...  Note some *old* versions of VMS can't
$! handle this properly, so we ignore any possible errors just to be sure
$! (I believe it was after VMS V5.0, but not sure if V5.1 or V5.2 had this
$! behavior, too...).
$  SET noON
$  IF LOW_PRIORITY .LT. PRIB THEN SET PROCESS/PRIORITY='PRIB''DEBUG_LOG'
$  SET ON
$
$  SET noON
$  @'TMP_DSNFILE'
$  DSNLINK_ITS_EXIT_STATUS = $STATUS
$  SET ON
$
$  IF LOW_PRIORITY .NE. PRIB THEN SET PROCESS/PRIORITY='LOW_PRIORITY''DEBUG_LOG'
$  
$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-RETURNED, Attempt completed at ", F$TIME()
$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-EXITSTATUS, DSN ITS exit $STATUS value: ", DSNLINK_ITS_EXIT_STATUS
$! calculate elapsed connection time (only works for the same day)
$! (from James Thompson, DECUServe VMS conference, note 1270.20)
$  SET noON
$  DEFINE SYS$OUTPUT NLA0:
$  DEFINE SYS$ERROR NLA0:
$  ELAPSED_CONN_TIME = F$CVTIME("-''ATTEMPT_CONNECT_TIME'",,"TIME")
$  STATUS = $STATUS
$  DEASSIGN SYS$OUTPUT
$  DEASSIGN SYS$ERROR
$  SET ON
$  IF (.NOT. STATUS) .OR. (TOTAL_ELAPSED_CONN_TIME .EQS. "--:--:--") .OR. (ATTEMPT_CONNECT_DATE .NES. F$CVTIME(,,"DATE"))
$  THEN    ! The day changed, 
$    TOTAL_ELAPSED_CONN_TIME = "--:--:--"
$    WRITE SYS$OUTPUT "%DSNlink_NEW-W-ELAPSED, Unable to calculate elapsed time (", STATUS, ")"
$    WRITE SYS$OUTPUT "-DSNlink_NEW-W-2DAYS, Can only calculate elapsed time for same day."
$    WRITE SYS$OUTPUT "-DSNlink_NEW-W-2DAYS, Connect time will be set to ", TOTAL_ELAPSED_CONN_TIME
$  ELSE
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-ELAPSED, Connect time to DSNlink host: ", ELAPSED_CONN_TIME
$    TOTAL_ELAPSED_CONN_TIME = F$CVTIME("''TOTAL_ELAPSED_CONN_TIME'+''ELAPSED_CONN_TIME'",,"TIME")
$  ENDIF ! .not. status
$  WRITE SYS$OUTPUT ""
$
$  IF DEBUG 
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, DSNlink connection attempt finished (''F$TIME()')"
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Error file from DSN ITS (SYS$ERROR):"
$    IF F$SEARCH(TMP_ERRORFILE) .NES. "" 
$    THEN 
$      TYPE 'TMP_ERRORFILE' 
$    ELSE
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, <no SYS$ERROR file was created>"
$    ENDIF
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Output file from DSN ITS (SYS$OUTPUT):"
$    IF F$SEARCH(TMP_OUTFILE) .NES. "" 
$    THEN 
$      TYPE 'TMP_OUTFILE'
$    ELSE
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, <No SYS$OUTPUT file was created>"
$    ENDIF
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Contents of DSN ITS directory extract files:"
$    IF F$SEARCH("''TMP_EXTRACTFILE'*",1) .NES. ""
$    THEN
$      TYPE 'TMP_EXTRACTFILE'* 
$    ELSE
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, <no DSN ITS directory extract files were created>"
$    ENDIF ! not found
$  ENDIF  ! debug
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!  Determine if encountered an error
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$  RETRY_OPERATION = FALSE
$
$! if we got an UNKDBNAM (unknown database) error, inform $ERROR$ group now
$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
$  SEARCH/EXACT 'TMP_ERRORFILE' "%DSN-E-UNKDBNAM" 
$  IF $STATUS .EQ. SS$_NORMAL 
$  THEN 
$    ! if we had an UNKDBNAM (unknown database) error, assume (cough cough)
$    ! that it was the only error, and we don't need to retry the operation.
$    RETRY_OPERATION = FALSE
$    SET FILE/VERSION_LIMIT=2 'TMP_ERRORFILE';
$    SEARCH/EXACT 'F$SEARCH(TMP_ERRORFILE)' "%DSN-","-DSN-"/OUTPUT='TMP_ERRORFILE'
$    CALL _SEND_MAIL_MESSAGE 'TMP_ERRORFILE' "DSNlink UNKDBNAM error" $ERROR$
$  ELSE
$    ! Check to see if we got any errors and if we need to retry the connection
$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
$    SEARCH/EXACT 'TMP_ERRORFILE' "%DSN-F-","%DSN-E-","-DSN-E-","-DSN-F-"
$    IF $STATUS .EQ. SS$_NORMAL THEN RETRY_OPERATION = TRUE   ! found an error
$  ENDIF
$
$  IF RETRY_OPERATION   ! we got an error other than UNKDBNAM
$  THEN       ! some sort of DSNlink failure
$    SEARCH/EXACT/noHIGHLIGHT 'TMP_ERRORFILE' -
          "-F-","-E-"    ! show error in .LOG file (%DSN and %SYSTEM errors)
$    WRITE SYS$OUTPUT ""
$
$    ! see if the error was "CSCERRACC, CSC call tracking database is unavailable"
$    ! if so, get out now -- retrying isn't going to make it available
$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
$    SEARCH/EXACT 'TMP_ERRORFILE' "CSCERRACC"
$    IF $STATUS .EQ. SS$_NORMAL
$    THEN
$      ! we found it!  That must have been why we failed
$      EXCEEDED_RETRY_LIMIT = TRUE
$      RECEIVED_FATAL_ERROR = TRUE
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-QUITTING, Detected CSCERRACC error -- not attempting to retry."
$      GOTO _NO_DSNLINK_ARTICLES
$    ENDIF
$
$    ! see if the error was "NOTREGSTRD, application is not registered for this access number"
$    ! if so, get out now -- retrying isn't going to make it available
$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
$    SEARCH/EXACT 'TMP_ERRORFILE' "NOTREGSTRD"
$    IF $STATUS .EQ. SS$_NORMAL
$    THEN
$      ! we found it!  That must have been why we failed
$      EXCEEDED_RETRY_LIMIT = TRUE
$      RECEIVED_FATAL_ERROR = TRUE
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-QUITTING, Detected NOTREGSTRD error -- not attempting to retry."
$      GOTO _NO_DSNLINK_ARTICLES
$    ENDIF
$
$    IF (DSN_RETRY_COUNT + 1) .LE. DSN_RETRY_MAX
$    THEN 
$      DSN_RETRY_COUNT = DSN_RETRY_COUNT + 1
$      IF BATCH THEN SET RESTART_VALUE="Call failed; connection retry count = ''DSN_RETRY_COUNT'"
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-RETRY, Waiting ", DSN_RETRY_WAIT_TIME, " to retry connection"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-RETRY, Retry number ", DSN_RETRY_COUNT
$      WAIT 'DSN_RETRY_WAIT_TIME'
$      GOTO _DSN_RETRY
$    ELSE
$      EXCEEDED_RETRY_LIMIT = TRUE
$      GOTO _NO_DSNLINK_ARTICLES
$    ENDIF    ! retry limit exceeded?
$  ENDIF ! retry_operation
$  IF BATCH THEN SET RESTART_VALUE
$  RETURN  ! Leave _CALL_DSN_HOST ...
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_BUILD_MESSAGE:
$! put FLASH articles together for all (selected) databases
$  COUNT = 0
$  TOTAL_NORMAL_ARTICLES = 0
$  TOTAL_FLASH_ARTICLES = 0
$  TOTAL_DATABASES = 0
$  DSNLINK_FLASH = TRUE
$
$  _FILE_LOOP_2:
$  DB_NAME = DB_NAME_'COUNT'
$  MAIL_CHECK = "," + DB_MAIL_'COUNT' + ","
$  IF F$LOCATE(",''SELECT_MAIL_GROUP_NAME',",MAIL_CHECK) .EQ. F$LENGTH(MAIL_CHECK) THEN GOTO _SKIP_FILE_LOOP_2
$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT) + "_FLASH"
$  IF F$SEARCH(DB_DIRFILE) .NES. "" 
$  THEN
$    WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-BUILDMSG, Writing !UL new FLASH article!%S from !AS database", -
     DB_ARTICLES_FLASH_'COUNT', DB_NAME)
$    APPEND/NEW_VERSION 'DB_DIRFILE' 'TMP_MAILFILE'
$    TOTAL_FLASH_ARTICLES = TOTAL_FLASH_ARTICLES + DB_ARTICLES_FLASH_'COUNT'
$    TOTAL_DATABASES = TOTAL_DATABASES + 1
$  ENDIF
$  _SKIP_FILE_LOOP_2:
$  COUNT = COUNT + 1
$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _FILE_LOOP_2
$
$! put normal (non-FLASH) articles together for all (selected) databases
$  COUNT = 0
$  DSNLINK_FLASH = FALSE
$  _FILE_LOOP_3:
$  DB_NAME = DB_NAME_'COUNT'
$  MAIL_CHECK = "," + DB_MAIL_'COUNT' + ","
$  IF F$LOCATE(",''SELECT_MAIL_GROUP_NAME',",MAIL_CHECK) .EQ. F$LENGTH(MAIL_CHECK) THEN GOTO _SKIP_FILE_LOOP_3
$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT)
$  IF F$SEARCH(DB_DIRFILE) .NES. "" 
$  THEN
$    WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-BUILDMSG, Writing !UL new article!%S from !AS database", -
     DB_ARTICLES_NORMAL_'COUNT', DB_NAME)
$    APPEND/NEW_VERSION 'DB_DIRFILE' 'TMP_MAILFILE'
$    TOTAL_NORMAL_ARTICLES = TOTAL_NORMAL_ARTICLES + DB_ARTICLES_NORMAL_'COUNT'
$    TOTAL_DATABASES = TOTAL_DATABASES + 1
$  ENDIF
$  _SKIP_FILE_LOOP_3:
$  COUNT = COUNT + 1
$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _FILE_LOOP_3
$
$  RETURN   ! from _BUILD_MESSAGE
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_ELIMINATE_HISTORY_RECORDS:
$  ACCEPTED_ARTICLE_COUNT = 0
$  REJECTED_ARTICLE_COUNT = 0
$  NEW_ARTICLE_FOUND = FALSE
$  DEBUG_HISTORY_REJECT = FALSE
$
$! determine if these should be accepted or rejected based on history
$  SET FILE/VERSION_LIMIT=3 'DB_DIRFILE'
$  OPEN/READ DSNLINK_NEW_INFILE 'DB_DIRFILE'
$
$  CREATE 'DB_DIRFILE';               ! create a new version, which contains 'real' stuff
$  OPEN/APPEND DSNLINK_NEW_OUTFILE 'DB_DIRFILE'
$
$  IF DSN_HISTORY_FILE .EQS. "NONE" 
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTDIS, History processing disabled."
$    GOTO _HISTORY_DISABLED
$  ENDIF
$
$  WRITE SYS$OUTPUT -
     F$FAO("%DSNlink_NEW-I-CHECKHIST, Checking history for !0UL!1%CFLASH !%E!%Fdatabase !AS", -
     DSNLINK_FLASH, DB_NAME)
$
$  OPEN/READ/WRITE/SHARE=WRITE HISTORY_FILE 'DSN_HISTORY_FILE'
$
$!
$!
$!
$
$_CHK_HIST_LOOP:
$  READ/END_OF_FILE=_EXIT_CHK_HIST_LOOP DSNLINK_NEW_INFILE ART_TITLE
$  IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_22
$  THEN
$    ARTICLE_TITLE = F$EDIT(ART_TITLE,"COMPRESS")     ! no article numbers with DSNlink 2.2
$  ELSE
$    ARTICLE_TITLE = F$EDIT(F$EXTRACT(8,78,ART_TITLE),"COMPRESS")     ! strip off article numbers
$  ENDIF
$  GOSUB _CHECK_EXCLUDE
$  IF ARTICLE_TITLE .EQS. "" THEN GOTO _CHK_HIST_LOOP  ! excluded
$  ACCEPTED_ARTICLE_COUNT = ACCEPTED_ARTICLE_COUNT + 1
$
$! "flash" history records have database name in lowercase
$! "normal" history records have database name in uppercase
$  IF DSNLINK_FLASH
$  THEN
$    HISTORY_KEY = F$FAO("!32AS!78AS", F$EDIT(DB_NAME,"LOWERCASE"), ARTICLE_TITLE)
$  ELSE
$    HISTORY_KEY = F$FAO("!32AS!78AS", F$EDIT(DB_NAME,"UPCASE"), ARTICLE_TITLE)
$  ENDIF
$
$  _HIS_KEY_LOOP:   ! remove all quotes from string
$  IF F$LOCATE(QUOTE,HISTORY_KEY) .NE. F$LENGTH(HISTORY_KEY)
$  THEN
$    HISTORY_KEY[F$LOCATE(QUOTE,HISTORY_KEY),1] := "\"
$    GOTO _HIS_KEY_LOOP
$  ENDIF
$
$  IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTORYKEY, \", HISTORY_KEY, "\"
$  READ/KEY=&HISTORY_KEY/ERROR=_NOT_IN_HIST/noLOCK HISTORY_FILE HIST_RECORD
$
$!
$!!! we found the article in history
$!
$
$!!+ 
$!! backed out so history date reflects the first day we saw the article, not
$!! the most recent day we saw the article.
$!!
$!!  ! update history file record with today's date
$!!  READ/KEY=&HISTORY_KEY/ERROR=_ERROR HISTORY_FILE HIST_RECORD
$!!  WRITE/UPDATE HISTORY_FILE F$CVTIME(,"COMPARISON","DATE") + HISTORY_KEY
$!!-
$
$  IF F$EXTRACT(0,10,HIST_RECORD) .GTS. ALLOW_IF_OLDER 
$  THEN 
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTORY, Rejected article \", ARTICLE_TITLE, "\"
$    WRITE SYS$OUTPUT "-DSNlink_NEW-I-HISTORY, because it was last retrieved at ", F$EXTRACT(0,10,HIST_RECORD)
$    IF DEBUG 
$    THEN 
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, Rejected article will be tagged and included in mail message"
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("!80AS", "Hist/DBG:" + F$EXTRACT(0,10,HIST_RECORD) + ":" + ARTICLE_TITLE)
$      DEBUG_HISTORY_REJECT = TRUE
$    ENDIF  ! debug
$    REJECTED_ARTICLE_COUNT = REJECTED_ARTICLE_COUNT + 1
$    GOTO _CHK_HIST_LOOP
$  ENDIF  ! already seen this article
$
$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-AGAIN, Article re-appeared from history: ", ARTICLE_TITLE
$  ! update history file record with today's date
$  READ/KEY=&HISTORY_KEY/ERROR=_ERROR HISTORY_FILE HIST_RECORD
$  WRITE/UPDATE HISTORY_FILE F$CVTIME(,"COMPARISON","DATE") + HISTORY_KEY
$  GOTO _INCLUDE_TITLE
$
$_NOT_IN_HIST:      ! we've never seen it.
$  IF DEBUG THEN WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-DEBUG, Writing: !/!5* !AS!AS", F$CVTIME(,"COMPARISON","DATE"), HISTORY_KEY)
$  WRITE HISTORY_FILE F$CVTIME(,"COMPARISON","DATE") + HISTORY_KEY
$
$_INCLUDE_TITLE:
$  WRITE DSNLINK_NEW_OUTFILE F$FAO("  !78AS", ARTICLE_TITLE)
$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTORY, New article: ", ARTICLE_TITLE
$  NEW_ARTICLE_FOUND = TRUE
$  GOTO _CHK_HIST_LOOP
$
$_EXIT_CHK_HIST_LOOP:
$  CLOSE DSNLINK_NEW_INFILE
$  CLOSE DSNLINK_NEW_OUTFILE
$  IF F$TRNLNM("HISTORY_FILE") .NES. "" THEN CLOSE HISTORY_FILE
$
$  IF NEW_ARTICLE_FOUND .OR. DEBUG_HISTORY_REJECT
$  THEN
$    WRITE SYS$OUTPUT F$FAO( -
     "%DSNlink_NEW-I-SUMMARY, Total of !UL new article!%S in !0UL!1%CFLASH !%E!%F!AS, !UL rejected, !UL accepted.", -
     ACCEPTED_ARTICLE_COUNT, DSNLINK_FLASH, DB_NAME, REJECTED_ARTICLE_COUNT, ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT)
$    CREATE 'DB_DIRFILE';     ! create new version that has headers
$    OPEN/APPEND DSNLINK_NEW_OUTFILE 'DB_DIRFILE'
$    IF DSNLINK_FLASH
$    THEN
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("FLASH !AS database, !UL article!%S:", DB_NAME, ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT)
$      DB_ARTICLES_FLASH_'COUNT' = ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT
$    ELSE
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("!AS database, !UL article!%S:", DB_NAME, ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT)
$      DB_ARTICLES_NORMAL_'COUNT' = ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT
$    ENDIF ! flash
$    CLOSE DSNLINK_NEW_OUTFILE
$    APPEND 'DB_DIRFILE';-1 'DB_DIRFILE'
$  ELSE
$    WRITE SYS$OUTPUT F$FAO( -
     "%DSNlink_NEW-I-SUMMARY, Total of !UL new article!%S in !0UL!1%CFLASH !%E!%F!AS, all rejected.", -
     ACCEPTED_ARTICLE_COUNT, DSNLINK_FLASH, DB_NAME)
$    DELETE 'DB_DIRFILE';*
$  ENDIF ! new article found
$
$  RETURN  ! from _CHECK_DSNLINK_HISTORY
$
$_HISTORY_DISABLED:
$  NEW_ARTICLE_FOUND = TRUE
$_HISTORY_DISABLED_LOOP:
$  READ/END_OF_FILE=_EXIT_CHK_HIST_LOOP DSNLINK_NEW_INFILE ART_TITLE
$  IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_22
$  THEN
$    ARTICLE_TITLE = F$EDIT(ART_TITLE,"COMPRESS")   ! no article numbers with DSNlink 2.2
$  ELSE
$    ARTICLE_TITLE = F$EDIT(F$EXTRACT(8,78,ART_TITLE),"COMPRESS")   ! strip off article numbers
$  ENDIF
$  GOSUB _CHECK_EXCLUDE
$  IF ARTICLE_TITLE .EQS. "" THEN GOTO _HISTORY_DISABLED_LOOP  ! excluded
$  WRITE DSNLINK_NEW_OUTFILE F$FAO("  !78AS",ARTICLE_TITLE)
$  ACCEPTED_ARTICLE_COUNT = ACCEPTED_ARTICLE_COUNT + 1
$  GOTO _HISTORY_DISABLED_LOOP
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_CHECK_EXCLUDE:
$! Determine if the article is one of the articles we're supposed to
$! exclude from checking.
$  EXC_COUNT = 0
$  IF F$TYPE(EXCLUDE_ARTICLE_0) .EQS. "" THEN RETURN  ! if we don't have anything to exclude
$_CHECK_EXCLUDE_LOOP:
$  IF (F$EDIT(EXCLUDE_ARTICLE_'EXC_COUNT',"UPCASE,COLLAPSE") .EQS. F$EDIT(ARTICLE_TITLE,"UPCASE,COLLAPSE")) .AND. -
      (F$EDIT(DB_NAME,"UPCASE") .EQS. F$EDIT(EXCLUDE_DB_'EXC_COUNT',"UPCASE") .OR. EXCLUDE_DB_'EXC_COUNT' .EQS. "*")
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-EXCLUDED, Article \", ARTICLE_TITLE, "\ from database \", DB_NAME, "\"
$    TOTAL_EXCLUDED_ARTICLES = TOTAL_EXCLUDED_ARTICLES + 1
$    ARTICLE_TITLE = ""
$  ENDIF
$  EXC_COUNT = EXC_COUNT + 1
$  IF EXC_COUNT .LE. MAX_EXC_COUNT THEN GOTO _CHECK_EXCLUDE_LOOP
$  RETURN  ! from _CHECK_EXCLUDE
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_NO_DSNLINK_ARTICLES:
$! There were no articles retrieved, and no errors (some other code has
$!  already checked for errors)
$
$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
$  SEARCH/EXACT 'TMP_ERRORFILE' "Connection established"
$  IF ($STATUS .EQ. SS$_NORMAL) .AND. (.NOT. EXCEEDED_RETRY_LIMIT)
$  THEN   ! good connection, just no articles...
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-NONEW, No new DSNlink articles were found."
$    IF (DSN_MAIL_EMPTY) 
$    THEN 
$      GOTO _GENERATE_MAIL_MESSAGES
$    ENDIF
$    FINISHED_OK = TRUE
$  ELSE   ! never established a connection, or we exceeded retry limit (errors after the connection)
$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-PROBLEM, Problem connecting to DSNlink host (", F$TIME(), ")"
$    CALL _SEND_MAIL_MESSAGE 'TMP_ERRORFILE' "Problem connecting to DSNlink host" $ERROR$
$  ENDIF
$
$  GOTO _POST_PROCESSING
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_POST_PROCESSING:
$!  check to see if history file should be auto-compressed, then exit
$
$  IF PERFORM_AUTO_COMPRESS 
$  THEN
$    GOSUB _COMPRESS_HISTORY
$  ENDIF
$
$  GOTO _EXIT
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_EXIT:
$  IF FINISHED_OK
$  THEN 
$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-S-OK, DSNlink_NEW completed successfully at !%T",0)
$    EXIT_STATUS = (SS$_NORMAL .OR. STS$M_INHIB_MSG)
$  ELSE
$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-E-NOTOK, DSNlink_NEW encountered errors.")
$    WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOTOK, No new DSNlink articles were retrieved."
$    EXIT_STATUS = (SS$_ABORT .OR. STS$M_INHIB_MSG)
$  ENDIF
$  GOSUB _DELETE_TMP_FILES
$_EXIT_NOCLEANUP:
$  VERIFY = F$VERIFY(VERIFY)
$  EXIT 'EXIT_STATUS'   ! Leaving DSNlink_NEW ...
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_EXIT_ERROR:
$  IF F$TYPE(ERROR_STATUS) .EQS. "" THEN ERROR_STATUS = $STATUS
$  IF ERROR_MESSAGE .EQS. "" THEN -
     ERROR_MESSAGE = "Unexpected condition; exact error unknown."
$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-ERROR, error occurred; DSNlink_NEW error was:"
$  WRITE SYS$OUTPUT "  ", ERROR_MESSAGE
$  WRITE SYS$OUTPUT ""
$  WRITE SYS$OUTPUT F$FAO("-DSNlink_NEW-E-ERROR, DCL error was: %X!XL", ERROR_STATUS)
$  DISPLAY_MESSAGES_OUTPUT = "SYS$OUTPUT"
$  GOSUB _DISPLAY_MESSAGES 
$  WRITE SYS$OUTPUT ""
$
$  IF (.NOT. INTERACTIVE) .AND. ("''MAIL_GROUP_$ERROR$'" .NES. "")
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-MAILING, Informing $ERROR$ group of error"
$    SET noON       ! in case error-handler dies
$    CLOSE/noLOG DSNLINK_NEW_OUTFILE
$    OPEN/WRITE DSNLINK_NEW_OUTFILE 'TMP_MAILFILE'
$    WRITE DSNLINK_NEW_OUTFILE "*** An error caused DSNlink_NEW to fail ***"
$    WRITE DSNLINK_NEW_OUTFILE ""
$    WRITE DSNLINK_NEW_OUTFILE "The error detected by DSNlink_NEW was:"
$    WRITE DSNLINK_NEW_OUTFILE "  ", ERROR_MESSAGE
$    WRITE DSNLINK_NEW_OUTFILE ""
$    DISPLAY_MESSAGES_OUTPUT = "DSNLINK_NEW_OUTFILE"
$    GOSUB _DISPLAY_MESSAGES 
$    CLOSE DSNLINK_NEW_OUTFILE
$    CALL _SEND_MAIL_MESSAGE 'TMP_MAILFILE' "Error in DSNlink_NEW" $ERROR$
$  ENDIF  ! error group exists, not interactive
$  GOSUB _DELETE_TMP_FILES
$  VERIFY = F$VERIFY(VERIFY)
$  EXIT SS$_ABORT      ! Leaving DSNlink_NEW (on error status) ...
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_DISPLAY_MESSAGES: 
$  WRITE 'DISPLAY_MESSAGES_OUTPUT' F$FAO("$STATUS = %X!XL", ERROR_STATUS)
$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
$  IF F$SEARCH("SYS$MESSAGE:CLIUTLMSG.EXE") .NES. "" THEN SET MESSAGE SYS$MESSAGE:CLIUTLMSG.EXE
$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
$  IF F$SEARCH("SYS$MESSAGE:SYSMGTMSG.EXE") .NES. "" THEN SET MESSAGE SYS$MESSAGE:SYSMGTMSG.EXE
$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
$  IF F$SEARCH("SYS$MESSAGE:DSN$MESSAGES.EXE") .NES. "" THEN SET MESSAGE SYS$MESSAGE:DSN$MESSAGES.EXE
$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
$  SET MESSAGE/DELETE
$  WRITE 'DISPLAY_MESSAGES_OUTPUT' ""
$  RETURN ! Leaving _DISPLAY_MESSAGES
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_DELETE_TMP_FILES:
$  IF F$TRNLNM("DSNLINK_NEW_REFFILE") .NES. "" THEN CLOSE DSNLINK_NEW_REFFILE
$  IF F$TRNLNM("DSNLINK_NEW_SEARCHFILE") .NES. "" THEN CLOSE DSNLINK_NEW_SEARCHFILE
$  IF F$TRNLNM("DSNLINK_NEW_FILE") .NES. "" THEN CLOSE DSNLINK_NEW_FILE
$  IF F$TRNLNM("DSNLINK_NEW_OUTFILE") .NES. "" THEN CLOSE DSNLINK_NEW_OUTFILE
$  IF F$TRNLNM("DSNLINK_NEW_INFILE") .NES. "" THEN CLOSE DSNLINK_NEW_INFILE
$  IF F$TRNLNM("DSNLINK_NEW_CONFIGFILE") .NES. "" THEN CLOSE DSNLINK_NEW_CONFIGFILE
$  IF (F$SEARCH("''TMP_EXTRACTFILE'*",3) .NES. "") .AND. -
     ("''TMP_EXTRACTFILE'" .NES. "") THEN DELETE 'TMP_EXTRACTFILE'*;*
$  IF F$SEARCH(TMP_ERRORFILE) .NES. "" THEN DELETE 'TMP_ERRORFILE';*
$  IF F$SEARCH(TMP_DSNFILE) .NES. "" THEN DELETE 'TMP_DSNFILE';*
$  IF F$SEARCH(TMP_CHECKFILE) .NES. "" THEN DELETE 'TMP_CHECKFILE';*
$  IF F$SEARCH(TMP_OPENCALLSFILE) .NES. "" THEN DELETE 'TMP_OPENCALLSFILE';*
$  IF F$SEARCH(TMP_CLOSEDCALLSFILE) .NES. "" THEN DELETE 'TMP_CLOSEDCALLSFILE';*
$  IF F$SEARCH(TMP_OUTFILE) .NES. "" THEN DELETE 'TMP_OUTFILE';*
$  IF F$SEARCH(TMP_MAILFILE) .NES. "" THEN DELETE 'TMP_MAILFILE';*
$  IF F$SEARCH(TMP_YESFILE) .NES. "" THEN DELETE 'TMP_YESFILE';*
$  IF F$TRNLNM("DSNLINK_NEW_RETRY_GET") .NES. "" THEN CLOSE DSNLINK_NEW_RETRY_GET
$  IF F$SEARCH(TMP_RETRYFILE) .NES. "" THEN DELETE 'TMP_RETRYFILE';*
$  IF F$SEARCH(P3) .NES. "" .AND. PERFORMING_GET THEN DELETE 'F$SEARCH(P3)'
$  RETURN  ! from _DELETE_TMP_FILES
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_ERROR:
$  ERROR_STATUS = $STATUS
$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-ERROR, Unexpected error occurred in DSNlink_NEW"
$  GOTO _EXIT_ERROR
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_ABORT:
$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-ABORT, User aborted DSNlink_NEW"
$  FINISHED_OK = TRUE
$  GOTO _EXIT
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_NO_DSNLINK:
$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-NOTPRESENT, DSNlink software is not installed on this node"
$  WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOTPRESENT, Contact your local Digital office to obtain DSNlink software"
$  FINISHED_OK = TRUE
$  GOTO _EXIT
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_NO_CONFIG_FILE:
$  ERROR_MESSAGE = "%DSNlink_NEW-E-CONFIG, Missing file \''DSN_CONFIG_FILENAME'\"
$  ERROR_STATUS = SS$_NOSUCHFILE
$  GOTO _EXIT_ERROR
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_BAD_P2:
$  ERROR_MESSAGE = "%DSNlink_NEW-E-INVP2, Bad parameter P2 \''P2'\"
$  ERROR_STATUS = SS$_BADPARAM
$  GOTO _EXIT_ERROR
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_DISPLAY_DATABASES: SUBROUTINE
$! P1 = output file (file must already be opened)
$! P2 = number spaces at beginning of each line
$! P3 = String to put at front of output text [optional]
$! P4 = mail group name
$! P5 = additional indentation after first line [optional]
$
$  FIRST_LINE = TRUE
$  IF P2 .EQS. "" THEN P2 = 0
$  P2 = F$INTEGER(P2)
$  P5 = F$INTEGER(P5)
$  AVAILABLE_WIDTH = 80 - P2 - 2          ! 2 = comma + extra space
$  DB_NUMBER = 0
$  OUTPUT_STRING = P3
$  NEED_COMMA = FALSE
$
$! first make sure prefix string isn't too long
$_CHECK_STR_LENGTH:
$  IF F$LENGTH(OUTPUT_STRING) .GT. AVAILABLE_WIDTH
$  THEN
$    START = 0
$    END = AVAILABLE_WIDTH
$    _DISPLAY_PREFIX_LOOP:
$    CHAR = F$EXTRACT(END,1,OUTPUT_STRING)
$    IF CHAR .NES. "," .AND. CHAR .NES. " " .AND. CHAR .NES. "." 
$    THEN
$      END = END - 1
$      IF END .GT. 0 THEN GOTO _DISPLAY_PREFIX_LOOP
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-DISPdb, Display database error."
$      GOTO _ABORT_DISP_LOOP
$    ENDIF
$    WRITE SYS$OUTPUT F$EDIT(F$EXTRACT(START,END+1,OUTPUT_STRING),"TRIM")
$    OUTPUT_STRING = F$EXTRACT(END+1,-1,OUTPUT_STRING)
$    GOTO _CHECK_STR_LENGTH
$  ENDIF
$
$  _ABORT_DISP_LOOP:
$
$_DISPLAY_DB_LOOP:
$  IF DB_NUMBER .GT. DB_NAME_COUNT THEN GOTO _DISPLAY_DB_EXIT
$  MAIL_CHECK = "," + DB_MAIL_'DB_NUMBER' + ","
$  IF F$LOCATE(",''P4',",MAIL_CHECK) .EQ. F$LENGTH(MAIL_CHECK)
$  THEN
$!!!    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DISPLAY, Not displaying ", DB_NAME_'DB_NUMBER', " for ", P4
$    GOTO _DISPLAY_DB_NEXT
$  ENDIF  ! not for this group
$  ADDITIONAL_STRING = DB_NAME_'DB_NUMBER'
$  IF F$LENGTH(OUTPUT_STRING + "," + ADDITIONAL_STRING) .GE. AVAILABLE_WIDTH
$  THEN  ! not enough room to all fit on this line
$    IF NEED_COMMA 
$    THEN
$      WRITE 'P1' F$FAO("!#* !AS,", P2, OUTPUT_STRING)
$    ELSE
$      WRITE 'P1' F$FAO("!#* !AS", P2, OUTPUT_STRING)
$    ENDIF
$    IF FIRST_LINE 
$    THEN
$      FIRST_LINE = FALSE
$      P2 = P2 + P5
$      AVAILABLE_WIDTH = AVAILABLE_WIDTH - P5
$    ENDIF
$    OUTPUT_STRING = ADDITIONAL_STRING
$    NEED_COMMA = TRUE
$  ELSE ! yes, enough room to fit on this line
$    IF OUTPUT_STRING .EQS. ""
$    THEN
$      OUTPUT_STRING = ADDITIONAL_STRING
$      NEED_COMMA = TRUE
$    ELSE
$      IF NEED_COMMA 
$      THEN 
$        OUTPUT_STRING = OUTPUT_STRING + "," + ADDITIONAL_STRING
$      ELSE
$        OUTPUT_STRING = OUTPUT_STRING + ADDITIONAL_STRING
$        NEED_COMMA = TRUE
$      ENDIF ! need_comma
$    ENDIF ! output_string .eqs. ""
$  ENDIF ! enough room to fit on this line?
$_DISPLAY_DB_NEXT:
$  DB_NUMBER = DB_NUMBER + 1
$  GOTO _DISPLAY_DB_LOOP
$
$_DISPLAY_DB_EXIT:
$  IF OUTPUT_STRING .NES. P3
$  THEN
$    WRITE 'P1' F$FAO("!#* !AS", P2, OUTPUT_STRING)
$  ELSE
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Display_databases: No DBs for group \", P4, "\"
$  ENDIF
$  EXIT           ! _DISPLAY_DATABASES
$  ENDSUBROUTINE  ! _DISPLAY_DATABASES
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_SEND_MAIL_MESSAGE: SUBROUTINE
$! P1 = append information to this file, then Email it
$! P2 = Subject to use for Mail message
$! P3 = name of mail group we're sending to... 
$!      if there is an equal sign with text afterwards, the text after
$!      the equal sign is the username(s) we're going to send to.
$! P4 = Personal name (if empty, P2 is used)
$
$  IF P4 .EQS. "" THEN P4 = P2
$
$  GRPNAME = F$ELEMENT(0,"=",P3)
$
$  MAIL_USERS = "<none>"
$  MAIL_GROUPS = "<unknown>"
$
$  IF GRPNAME .EQS. "Retain" 
$  THEN
$    COUNT = 0
$    _RETAIN_LOOP:
$    IF F$EDIT(DATABASE,"UPCASE") .EQS. F$EDIT(DB_NAME_'COUNT',"UPCASE")
$    THEN                               ! found the database
$      MAIL_GROUPS = DB_MAIL_'COUNT'    ! list of mail groups (Programmers, Systems)
$      OFFSET = 0
$      _RETAIN_LOOP_2:                  ! now get list of users for the group
$      ELEMENT = F$ELEMENT(OFFSET,",",MAIL_GROUPS) 
$      IF ELEMENT .EQS. "," THEN GOTO _DONE_RETAIN
$      GROUP_NUMBER = MAIL_GROUP_'ELEMENT'
$      IF MAIL_USERS .EQS. "<none>" 
$      THEN 
$         MAIL_USERS = MAIL_GROUP_USERS_'GROUP_NUMBER'
$      ELSE
$         MAIL_USERS = MAIL_USERS + "," + MAIL_GROUP_USERS_'GROUP_NUMBER'
$      ENDIF
$      OFFSET = OFFSET + 1
$      GOTO _RETAIN_LOOP_2
$    ENDIF ! if database = db_name_'count'
$    COUNT = COUNT + 1
$    IF COUNT .LE. DB_NAME_COUNT THEN GOTO _RETAIN_LOOP  ! check next database
$  ELSE ! retain
$    MAIL_NUMBER = MAIL_GROUP_'GRPNAME'
$    MAIL_USERS  = MAIL_GROUP_USERS_'MAIL_NUMBER'
$  ENDIF ! retain
$
$  IF F$ELEMENT(1,"=",P3) .NES. "="
$  THEN
$    MAIL_USERS = F$ELEMENT(1,"=",P3)
$  ENDIF
$
$_DONE_RETAIN:
$
$
$  IF F$TRNLNM("DSNLINK_NEW_OUTFILE") .NES. "" THEN CLOSE DSNLINK_NEW_OUTFILE
$  OPEN/APPEND DSNLINK_NEW_OUTFILE 'P1'
$  WRITE DSNLINK_NEW_OUTFILE "_____"
$  WRITE DSNLINK_NEW_OUTFILE ""
$
$  WRITE DSNLINK_NEW_OUTFILE F$FAO("Message sent by DSNlink_NEW version !AS on node !AS, using:", DSNLINK_NEW_VERSION, F$GETSYI("NODENAME"))
$  WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Config file:!>!AS", DSN_CONFIG_FILENAME)
$  IF PERFORMING_GET 
$  THEN 
$    WRITE DSNLINK_NEW_OUTFILE "  Performing GET processing"
$    IF F$TYPE(GET_FILENAME) .NES. ""
$    THEN
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Get file:!>!AS", GET_FILENAME)
$    ELSE
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Get file:!><unknown>")
$    ENDIF
$  ENDIF ! performing_get
$  TMP_GROUP_COUNT = MAIL_GROUP_'GRPNAME'
$  WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Mail group:!>!AS", MAIL_GROUP_NAME_'TMP_GROUP_COUNT')
$  IF F$TYPE(ORIGINALLY_FROM) .NES. "" THEN -
     IF ORIGINALLY_FROM .NES. USERNAME THEN -
     WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Sent by:!>!AS", ORIGINALLY_FROM)
$  IF PERFORMING_GET .AND. MAIL_GROUPS .NES. "<unknown>" THEN WRITE DSNLINK_NEW_OUTFILE F$FAO("!15* = !AS", MAIL_GROUPS)  ! for RETAIN
$
$  IF MAIL_USERS .EQS. "<none>"
$  THEN
$    MAIL_USERS = USERNAME
$    WRITE DSNLINK_NEW_OUTFILE "%DSNlink_NEW-E-MAILUSERS, Undefined mail users for this database; using SELF."
$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-MAILUSERS, Undefined mail users for this database."
$  ENDIF
$    
$!
$
$  IF .NOT. PERFORMING_GET
$  THEN
$    IF GRPNAME .NES. "$ERROR$" 
$    THEN
$      CALL _DISPLAY_DATABASES DSNLINK_NEW_OUTFILE 2 "Databases:    " "''GRPNAME'" 2
$      IF F$TYPE(MAIL_LOGICAL_NAME) .EQS. "STRING" 
$      THEN 
$        DEFINE 'MAIL_LOGICAL_NAME' &MAIL_LOGICAL_EQUIV
$        IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Defined \''MAIL_LOGICAL_NAME'\ to be \''F$TRNLNM("MAIL_LOGICAL_NAME")'\"
$        SET_MAIL_LOGICAL_NAME = TRUE
$      ENDIF
$    ENDIF
$  
$    WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Since:!>!AS (!AS, !AS)", -
       DSN_SINCE, -
       F$CVTIME(DSN_SINCE,,"WEEKDAY"), -
       F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE"))
$    IF DSN_HISTORY_FILE .NES. "NONE"
$    THEN
$      WRITE DSNLINK_NEW_OUTFILE F$EXTRACT(0,79,F$FAO("  !14<History:!>!AS  (!UL blocks)", -
          DSN_HISTORY_FILE, F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF")))
$    ELSE
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<History:!>NONE")
$    ENDIF ! history enabled/disabled
$  ENDIF ! performing GET
$
$  IF BATCH .AND. GRPNAME .EQS. "$ERROR$" 
$  THEN 
$    WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Log file:!>!AS", F$PARSE(F$GETQUI("DISPLAY_JOB","LOG_SPECIFICATION",,"THIS_JOB"),LOG_FILE))
$    IF "''ORIGINAL_GET_FILE'" .NES. "" THEN -
     WRITE DSNLINK_NEW_OUTFILE F$FAO("  !14<Orig GET file:!>!AS", ORIGINAL_GET_FILE)
$  ENDIF
$
$  IF EXCEEDED_RETRY_LIMIT .AND. (.NOT. RECEIVED_FATAL_ERROR)
$  THEN 
$    WRITE DSNLINK_NEW_OUTFILE ""
$    WRITE DSNLINK_NEW_OUTFILE "%DSNlink_NEW-E-EXRETRY, Exceeded retry limit"
$    WRITE DSNLINK_NEW_OUTFILE ""
$  ENDIF
$  IF RECEIVED_FATAL_ERROR
$  THEN
$    WRITE DSNLINK_NEW_OUTFILE ""
$    WRITE DSNLINK_NEW_OUTFILE "%DSNlink_NEW-E-NORETRY, Received fatal error - aborting retry attempts."
$    WRITE DSNLINK_NEW_OUTFILE ""
$  ENDIF
$
$  IF DSN_ATTEMPT_COUNT .GT. 0 
$  THEN
$    IF MAX_EXC_COUNT .GT. 0
$    THEN
$      IF .NOT. PERFORMING_GET
$      THEN
$        WRITE DSNLINK_NEW_OUTFILE F$FAO("Total connect time: !8AS, !UL reconnection!%S.  !UL total articles excluded.", -
           TOTAL_ELAPSED_CONN_TIME, DSN_RETRY_COUNT, TOTAL_EXCLUDED_ARTICLES)
$      ELSE
$        WRITE DSNLINK_NEW_OUTFILE F$FAO("Total connect time: !8AS, !UL reconnection!%S.", -
           TOTAL_ELAPSED_CONN_TIME, DSN_RETRY_COUNT)
$      ENDIF
$    ELSE
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("Total connect time: !8AS, !UL reconnection!%S.", -
       TOTAL_ELAPSED_CONN_TIME, DSN_RETRY_COUNT)
$    ENDIF
$  ENDIF
$  IF PERFORM_AUTO_COMPRESS THEN WRITE DSNLINK_NEW_OUTFILE "History file auto-compression was triggered (history file > ", DSN_HISTORY_AUTO_COMPRESS, " blocks)."
$
$  CLOSE DSNLINK_NEW_OUTFILE
$
$  IF DEBUG 
$  THEN 
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Mail message about to be sent:"
$    MERGE 'P1' SYS$OUTPUT/SPECIFICATION=SYS$INPUT/noCHECK_SEQUENCE
/FIELD=(NAME=TEXT,POSITION:1,SIZE=150)
/DATA="## "
/DATA=TEXT
$  ENDIF
$
$  IF F$SEARCH(TMP_YESFILE) .EQS. ""
$  THEN
$    ! create file to 'feed' Mail in case a DECnet link is down -- this will
$    ! answer Mail's question "
$    OPEN/WRITE DSNLINK_NEW_FILE 'TMP_YESFILE'
$    WRITE DSNLINK_NEW_FILE "Y"
$    CLOSE DSNLINK_NEW_FILE
$  ENDIF
$  
$  SET noON
$  DEFINE/USER_MODE SYS$INPUT 'TMP_YESFILE'
$  MAIL -
     'P1' -
     "''MAIL_USERS'" -
     /SUBJECT="''P2'" -
     /PERSONAL_NAME="''P4'" -
     /noSELF 
$  STATUS = $STATUS
$  SET ON
$  IF F$TYPE(SET_MAIL_LOGICAL_NAME) .NES. "" THEN DEASSIGN 'MAIL_LOGICAL_NAME'
$
$  IF STATUS 
$  THEN 
$    WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-SENTMAIL, Message sent to user!0UL!%S !AS (!%T)", -
     (F$LENGTH(MAIL_USERS) - F$LOCATE(",",MAIL_USERS) + 1), -
     MAIL_USERS, 0)
$    WRITE SYS$OUTPUT F$FAO("-DSNlink_NEW-I-SENTMAIL, Mail subject: \!AS\", P2)
$  ENDIF
$  
$  
$  EXIT           ! _SEND_MAIL_MESSAGE
$  ENDSUBROUTINE  ! _SEND_MAIL_MESSAGE
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_COMPRESS_HISTORY:
$  IF DSN_HISTORY_FILE .EQS. "NONE"
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-W-COMPRESS, History is disabled in your configuration file, so"
$    WRITE SYS$OUTPUT "-DSNlink_NEW-W-COMPRESS, no compression will be performed."
$    RETURN  ! from _COMPRESS_HISTORY
$  ENDIF
$
$  IF F$EXTRACT(0,1,P2) .EQS. "$"
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-NOCOMPRESS, Not compressing due to P2 (", P2, ")"
$    WRITE SYS$OUTPUT "-DSNlink_NEW-I-TWICE, bypassing to prevent two processes from compressing"
$    RETURN  ! from _COMPRESS_HISTORY
$  ENDIF
$  
$! if old format file, convert it to new format
$  IF F$FILE(DSN_HISTORY_FILE,"MRS") .EQ. 108 THEN GOSUB _NEW_HISTORY_FORMAT
$
$  WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-COMPRESS, Beginning to compress history file (size=!UL)", -
     F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF"))
$  WRITE SYS$OUTPUT "-DSNlink_NEW-I-HISTORY, File ", F$SEARCH(DSN_HISTORY_FILE)
$
$  TMP_SPEC_FILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_SPEC_" + F$GETJPI(0,"PID") + ".TMP"
$  TMP_HIST_FILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_HISTORY_" + F$GETJPI(0,"PID") + ".TMP"
$  TMP_FDL_FILE  = DSN_SCRATCH_DIR + "DSNLINK_NEW_FDL_" + F$GETJPI(0,"PID") + ".TMP"
$
$  OPEN/WRITE DSNLINK_NEW_FILE 'TMP_SPEC_FILE'
$  WRITE DSNLINK_NEW_FILE "/FIELD=(NAME=HISTORY_DATE,POSITION:1,SIZE:10)"
$  WRITE DSNLINK_NEW_FILE "/FIELD=(NAME=DATABASE,POSITION:10,SIZE:32)"
$  WRITE DSNLINK_NEW_FILE "/FIELD=(NAME=TITLE,POSITION:42,SIZE:78)"
$  WRITE DSNLINK_NEW_FILE "/FIELD=(NAME=DATABASE_KEY,POSITION:10,SIZE:110)"
$  WRITE DSNLINK_NEW_FILE "/KEY=(DATABASE_KEY)"
$  WRITE DSNLINK_NEW_FILE "/CONDITION=(NAME=KEEP_RECORD,TEST=(HISTORY_DATE GE ", QUOTE, ALLOW_IF_OLDER, QUOTE, "))"
$  WRITE DSNLINK_NEW_FILE "/INCLUDE=(CONDITION=KEEP_RECORD)"
$  CLOSE DSNLINK_NEW_FILE
$
$! generate an FDL, and 'tune' it (using EDIT/FDL/noINTERACTIVE) best
$! we can for repopulation
$  ANALYZE/RMS/FDL/OUTPUT='TMP_FDL_FILE' 'DSN_HISTORY_FILE'
$  SET FILE/VERSION_LIMIT=2 'TMP_FDL_FILE'
$  EDIT/FDL/noINTERACTIVE/ANALYZE='TMP_FDL_FILE' 'TMP_FDL_FILE'
$  CREATE/FDL='TMP_FDL_FILE' 'TMP_HIST_FILE'
$
$  MERGE/SPECIFICATION='TMP_SPEC_FILE'/STATISTICS/STABLE/noCHECK_SEQUENCE -
     'DSN_HISTORY_FILE' -
     'TMP_HIST_FILE'/OVERLAY
$
$! a lot of records have been removed by the MERGE, so re-tune the 
$! file and use Convert to move it to its normal location
$  ANALYZE/RMS/FDL/OUTPUT='TMP_FDL_FILE' 'TMP_HIST_FILE'
$  EDIT/FDL/noINTERACTIVE/ANALYZE='TMP_FDL_FILE' 'TMP_FDL_FILE'
$  CONVERT/FDL='TMP_FDL_FILE' 'TMP_HIST_FILE' 'DSN_HISTORY_FILE'
$
$  DELETE 'TMP_SPEC_FILE';,'TMP_FDL_FILE';*,'TMP_HIST_FILE';
$  PURGE 'DSN_HISTORY_FILE'
$  RENAME 'DSN_HISTORY_FILE' 'DSN_HISTORY_FILE';1
$
$  WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-S-COMPRESS, Compression has completed normally (size=!UL)", -
     F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF"))
$
$  FINISHED_OK = TRUE
$  RETURN  ! from _COMPRESS_HISTORY
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_INIT_DEFAULTS:
$
$  TRUE  = 1
$  FALSE = 0
$
$  FINISHED_OK = FALSE  ! indicate we failed in case we error out prematurely
$
$  USERNAME = F$EDIT(F$GETJPI(0,"USERNAME"),"TRIM")
$
$!!!
$
$  PERFORMING_GET = FALSE
$  CHECKING_DATABASES = FALSE
$  PRIB = F$GETJPI(0,"PRIB")
$
$  IF F$TRNLNM("DSNLINK_NEW_DEBUG") 
$  THEN
$    DEBUG = TRUE
$    DEBUG_LOG = "/LOG"
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, DSNlink_NEW debugging enabled."
$  ELSE
$    DEBUG = FALSE
$    DEBUG_LOG = ""
$  ENDIF
$
$  DSNLINK_DIRECTORY = F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,"DEVICE") + -
     F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,"DIRECTORY")
$  IF F$TRNLNM("DSNLINK_NEW_DIR") .NES. "" THEN -
     DSNLINK_DIRECTORY = F$TRNLNM("DSNLINK_NEW_DIR")
$
$  DSN_RETRY_MAX = 5
$  IF F$TRNLNM("DSNLINK_NEW_RETRY") .NES. "" THEN -
     DSN_RETRY_MAX = F$TRNLNM("DSNLINK_NEW_RETRY")
$
$  DSN_SINCE = "TODAY-10-00:00"
$  IF F$TRNLNM("DSNLINK_NEW_SINCE") .NES. "" THEN -
     DSN_SINCE = F$TRNLNM("DSNLINK_NEW_SINCE")
$
$  DSN_MAIL_EMPTY = FALSE
$  IF F$TRNLNM("DSNLINK_NEW_MAIL_EMPTY") .NES. "" THEN -
     DSN_MAIL_EMPTY = F$TRNLNM("DSNLINK_NEW_MAIL_EMPTY")
$
$  DSN_AFTER = "TOMORROW+01:00:00"
$  IF F$TRNLNM("DSNLINK_NEW_AFTER") .NES. "" THEN -
     DSN_AFTER = F$TRNLNM("DSNLINK_NEW_AFTER")
$
$  DSN_RETRY_WAIT_TIME = "00:00:30"  ! 30 seconds
$  IF F$TRNLNM("DSNLINK_NEW_WAIT_TIME") .NES. "" THEN -
     DSN_RETRY_WAIT_TIME = F$TRNLNM("DSNLINK_NEW_WAIT_TIME")
$
$  DSN_HISTORY_FILE = "''DSNLINK_DIRECTORY'DSNLINK_NEW_HISTORY.DAT"
$  IF F$TRNLNM("DSNLINK_NEW_HISTORY") .NES. "" THEN -
     DSN_HISTORY_FILE = F$PARSE(F$TRNLNM("DSNLINK_NEW_HISTORY"),DSN_HISTORY_FILE)
$
$  DSN_HISTORY_AUTO_COMPRESS = 300
$  IF F$TRNLNM("DSNLINK_NEW_AUTO_COMPRESS") .NES. "" THEN -
     DSN_HISTORY_AUTO_COMPRESS = F$INTEGER(F$TRNLNM("DSNLINK_NEW_AUTO_COMPRESS"))
$
$  DSN_CHECK_OPENCALLS = "Never"
$  IF F$TRNLNM("DSNLINK_NEW_CHECK_OPENCALLS") .NES. "" THEN -
     DSN_CHECK_OPENCALLS = F$TRNLNM("DSNLINK_NEW_CHECK_OPENCALLS")
$  DSN_CHECK_OPENCALLS_USER = USERNAME
$
$  DSN_CHECK_CLOSEDCALLS = "Never"
$  IF F$TRNLNM("DSNLINK_NEW_CHECK_CLOSEDCALLS") .NES. "" THEN -
     DSN_CHECK_CLOSEDCALLS = F$TRNLNM("DSNLINK_NEW_CHECK_CLOSEDCALLS")
$  DSN_CHECK_CLOSEDCALLS_USER = USERNAME
$
$  DSN_CHECK_DATABASES = "Never"
$  IF F$TRNLNM("DSNLINK_NEW_CHECK_DATABASES") .NES. "" THEN -
     DSN_CHECK_DATABASES = F$TRNLNM("DSNLINK_NEW_CHECK_DATABASES")
$  DSN_CHECK_DATABASES_USER = USERNAME
$  DSN_CHECK_DATABASES_FILE_DEFAULT = F$PARSE(DSNLINK_DIRECTORY + "DSNLINK_NEW_DATABASES.DAT")
$  DSN_CHECK_DATABASES_FILE = DSN_CHECK_DATABASES_FILE_DEFAULT  ! until we're told otherwise
$  
$  DSN_CONFIG_FILENAME = F$PARSE(P1,F$TRNLNM("DSNLINK_NEW_CONFIG"), -
     "''DSNLINK_DIRECTORY'DSNLINK_NEW_CONFIG.DAT")
$
$  LOG_FILE = "''DSNLINK_DIRECTORY'DSNLINK_NEW.LOG"
$
$  GET_FILE = "SYS$LOGIN:DSNLINK_GET.DAT"
$  IF F$TRNLNM("DSNLINK_NEW_GET_FILE") .NES. "" THEN -
     GET_FILE = F$PARSE(F$TRNLNM("DSNLINK_NEW_GET_FILE"),GET_FILE)
$
$  GET_LOG_FILE = "SYS$LOGIN:DSNLINK_NEW_GET.LOG"
$  IF F$TRNLNM("DSNLINK_NEW_GET_LOG_FILE") .NES. "" THEN -
     GET_LOG_FILE = F$PARSE(F$TRNLNM("DSNLINK_NEW_GET_LOG_FILE"),GET_LOG_FILE)
$
$  DSNLINK_GET_ARTICLE_COUNT = 10
$
$  LOW_PRIORITY = PRIB
$  IF F$TRNLNM("DSNLINK_NEW_BATCH_PRIORITY") .NES. "" THEN -
     LOW_PRIORITY = F$INTEGER(F$TRNLNM("DSNLINK_NEW_BATCH_PRIORITY"))
$
$  DSN_SCRATCH_DIR = F$PARSE("DSN$SCRATCH:") - ".;"
$  IF DSN_SCRATCH_DIR .EQS. "" THEN DSN_SCRATCH_DIR = F$PARSE("SYS$SCRATCH:") - ".;"
$  IF DSN_SCRATCH_DIR .EQS. "" THEN DSN_SCRATCH_DIR = F$PARSE("SYS$LOGIN:") - ".;"
$  IF DSN_SCRATCH_DIR .EQS. "" THEN DSN_SCRATCH_DIR = DSNLINK_DIRECTORY
$  IF F$TRNLNM("DSNLINK_NEW_SCRATCH_DIR") .NES. "" THEN -
     DSN_SCRATCH_DIR = F$PARSE(F$TRNLNM("DSNLINK_NEW_SCRATCH_DIR")) - ".;"
$
$! undocumented symbol DSN$VERSION (only works with DSNlink V1.2)
$  DSN$VERSION = "V1.1"       ! assume we're running V1.1 unless told otherwise
$  IF F$TRNLNM("DSN_VERSION","LNM$SYSTEM",,"EXECUTIVE") .EQS. "" .AND. -
      F$TRNLNM("DSN$VERSION","LNM$SYSTEM",,"EXECUTIVE") .EQS. ""
$  THEN
$    ! disable error handling in case the DSN command doesn't exist (we haven't 
$    ! tested for SYS$SYSTEM:DSN$MAIN.EXE yet), and also disable error handling
$    ! in case DSN SHOW VERSION doesn't work (it is undocumented and might
$    ! go away in a future version).
$    SET noON         
$    DEFINE/USER_MODE SYS$OUTPUT NLA0:
$    DEFINE/USER_MODE SYS$ERROR NLA0:
$    DSN SHOW VERSION
$    SET ON
$  ! deassign logicals, as they will still be around if we didn't activate the DSN image
$    IF F$TRNLNM("SYS$OUTPUT","LNM$PROCESS_TABLE",,,,"ACCESS_MODE") .EQS. "USER" THEN DEASSIGN/USER_MODE SYS$OUTPUT
$    IF F$TRNLNM("SYS$ERROR","LNM$PROCESS_TABLE",,,,"ACCESS_MODE") .EQS. "USER" THEN DEASSIGN/USER_MODE SYS$ERROR
$  ELSE
$    DSN$VERSION = F$TRNLNM("DSN_VERSION","LNM$SYSTEM",,"EXECUTIVE")
$    IF DSN$VERSION .EQS. "" THEN DSN$VERSION = F$TRNLNM("DSN$VERSION","LNM$SYSTEM",,"EXECUTIVE")
$    DSN$VERSION = F$EXTRACT(4,-1,DSN$VERSION)
$  ENDIF
$
$! determine the get-style we'll use.  If it is an old version of DSNlink
$! (less than V1.2) we have to use EXTRACT:  DSNlink V1.0 (which isn't used
$! anywhere as far as I know) doesn't allow READ/EDIT, and DSNlink V1.1 and
$! V1.1-1 have a bug preventing READ/EDIT from working with EDT as the
$! editor (which is how we're doing GET Processing with DSNlink_NEW).
$! V1.2A has a different bug which prevents READ/EDIT from working with
$! EDT.  X2.2 only uses TPU which doesn't enter command mode but instead
$! enters screen mode
$
$  GET_STYLE = "EXTRACT"
$  IF F$EXTRACT(1,-1,DSN$VERSION) .EQS. "1.2B" THEN GET_STYLE = "EDT"
$  IF F$EXTRACT(1,-1,DSN$VERSION) .EQS. "1.2C" THEN GET_STYLE = "EDT"
$  WRITE_TITLES = ""  ! null filename = don't write titles to file
$
$! DSNLINK_STYLE values refer to the commands and how their output appears
$!    1X = DSNlink version 1.x
$!         Uses DIR/SINCE
$!         EXTRACT/DIRECTORY includes article numbers
$!         SHOW DATABASES has one space in front of database name
$!    22 = DSNlink version 2.2
$!         Uses DIR/BEGINNING
$!         EXTRACT/DIRECTORY doesn't include article numbers
$!         SHOW DATABASES has no spaces in front of database name
$!
$  DSNLINK_STYLE_1X = 1
$  DSNLINK_STYLE_22 = 2
$  DSNLINK_STYLE_30 = 2
$  DSNLINK_STYLE = DSNLINK_STYLE_1X
$  IF F$EXTRACT(1,3,DSN$VERSION) .EQS. "2.2" THEN DSNLINK_STYLE = DSNLINK_STYLE_22
$  IF F$EXTRACT(1,3,DSN$VERSION) .EQS. "3.0" THEN DSNLINK_STYLE = DSNLINK_STYLE_30
$
$  IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, style code=", DSNLINK_STYLE, ", get_style=", GET_STYLE
$
$  FAO_SUBJECT = "!+!+!UL new !AS DSNlink article!%S in !UL database!%S"
$  FAO_PERSONAL = "!+!+DSNlink article!0UL!%S, !AS"
$  FAO_GET_SUBJECT = "!AS: !AS"
$  FAO_GET_PERSONAL = "!AS database article"
$
$  QUOTE[0,8] = 34
$  EXCEEDED_RETRY_LIMIT = FALSE
$  RECEIVED_FATAL_ERROR = FALSE
$  ELAPSED_CONN_TIME = "00:00:00"
$  TOTAL_ELAPSED_CONN_TIME = "00:00:00"
$  PERFORM_AUTO_COMPRESS = FALSE
$  DSN_RETRY_COUNT = 0
$  DSN_ATTEMPT_COUNT = 0
$
$  ERROR_MESSAGE = "%DSNlink_NEW-?-UNK, Unknown/unexpected error.  Please investigate"
$  EXCLUDE_WARNING = TRUE
$  TOTAL_EXCLUDED_ARTICLES = 0
$
$  TMP_MAILFILE    = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_1.TMP"
$  TMP_ERRORFILE   = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_2.TMP"
$  TMP_DSNFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_3.TMP"
$  TMP_CHECKFILE   = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_4.TMP"
$  TMP_OUTFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_5.TMP"
$  TMP_EXTRACTFILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_6.TMP"
$  TMP_YESFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_7.TMP"
$  TMP_RETRYFILE   = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_8.TMP"
$  TMP_OPENCALLSFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_9.TMP"
$  TMP_CLOSEDCALLSFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_10.TMP"
$
$  INTERACTIVE = F$MODE() .EQS. "INTERACTIVE"
$  BATCH = F$MODE() .EQS. "BATCH"
$  ASK := READ SYS$COMMAND/END_OF_FILE=_ABORT/PROMPT=
$
$  SS$_NORMAL      = %X00000001
$  SS$_ABORT       = %X0000002C
$  SS$_NOSUCHFILE  = %X00000912
$  SS$_BADPARAM    = %X00000012
$  STS$M_INHIB_MSG = %X10000000
$
$  TOO_LONG_CHAR  = "~"       ! used by GET Processing on lines that are too long for VMSmail Subject and Personal lines.
$
$  DSN_NOTIFY = "/NOTIFY"
$
$  RETURN      ! leaving _INIT_DEFAULTS
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_INIT_PARSE_CONFIG_FILE:
$
$  IF BATCH THEN SET RESTART_VALUE="Reading configuration file"
$  DSN_CONFIG_FILENAME = F$SEARCH(DSN_CONFIG_FILENAME)
$  WRITE SYS$OUTPUT F$FAO("  !24<Configuration file:!>!AS", DSN_CONFIG_FILENAME)
$  OPEN/READ/ERROR=_ERROR DSNLINK_NEW_CONFIGFILE 'DSN_CONFIG_FILENAME'
$  DB_NAME_COUNT = 0
$  MAIL_GROUP_COUNT = 0
$  EXC_COUNT = 0
$  CONFIG_ERROR = FALSE
$  LINE_NUMBER = 0
$  
$_PARSE_CONFIG_LOOP:
$  LINE_NUMBER = LINE_NUMBER + 1
$  READ/ERROR=_ERROR/END_OF_FILE=_END_PARSE_CONFIG DSNLINK_NEW_CONFIGFILE ORIG_RECORD
$  RECORD = F$EDIT(ORIG_RECORD,"COMPRESS,TRIM,UNCOMMENT")
$  IF RECORD .EQS. "" THEN GOTO _PARSE_CONFIG_LOOP
$  VALID_RECORD = FALSE
$
$  PARM1 = F$EDIT(F$ELEMENT(0," ",RECORD),"TRIM")
$  PARM2 = F$EDIT(F$ELEMENT(1," ",RECORD),"TRIM")
$  PARM3 = F$EDIT(F$ELEMENT(2," ",RECORD),"TRIM")
$  PARM4 = F$EDIT(F$ELEMENT(3," ",RECORD),"TRIM")
$  PARM1_UP = F$EDIT(PARM1,"UPCASE")
$  PARM2_UP = F$EDIT(PARM2,"UPCASE")
$
$  IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-READ, Record \", RECORD, "\"
$
$!
$!!!
$!
$
$  IF F$EXTRACT(0,1,PARM1) .NES. "."
$  THEN  ! it's a database
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Database: \", -
     PARM1, "\", ", Group: \", PARM3, "\, Flash: ", PARM2 .EQS. "FLASH"
$    PARM2 = F$EDIT(PARM2,"UPCASE")
$    IF PARM2 .NES. "FLASH" .AND. PARM2 .NES. "NOFLASH"
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Database ", PARM1, " missing FLASH/NOFLASH setting"
$      CONFIG_ERROR = TRUE
$    ENDIF ! if parm2 <> flash and parm2 <> noflash
$    DB_NAME_'DB_NAME_COUNT' = PARM1
$    DB_FLASH_'DB_NAME_COUNT' = PARM2 .EQS. "FLASH"
$    DB_MAIL_'DB_NAME_COUNT' = PARM3
$    COUNT = 0
$    _CONFIG_DB_LOOP:
$    CHECK_MAIL = F$ELEMENT(COUNT,",",PARM3)
$    IF CHECK_MAIL .EQS. "," .OR. CHECK_MAIL .EQS. " " THEN GOTO _END_CONFIG_DB_LOOP
$    IF F$TYPE(MAIL_GROUP_'CHECK_MAIL') .EQS. ""
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Database ", PARM1, " uses undefined mail group \", CHECK_MAIL, "\"
$      CONFIG_ERROR = TRUE
$    ELSE
$      TMP_COUNT = MAIL_GROUP_'CHECK_MAIL'
$      IF MAIL_GROUP_NAME_'TMP_COUNT' .NES. CHECK_MAIL
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Database ", PARM1, " has incorrect case for mail group ", CHECK_MAIL
$        WRITE SYS$OUTPUT "-DSNlink_NEW-E-CONFIG, Case of \", CHECK_MAIL, "\ is different from \", MAIL_GROUP_NAME_'TMP_COUNT', "\"
$        CONFIG_ERROR = TRUE
$      ENDIF
$    ENDIF
$    COUNT = COUNT + 1
$    GOTO _CONFIG_DB_LOOP
$    _END_CONFIG_DB_LOOP:
$    DB_NAME_COUNT = DB_NAME_COUNT + 1
$    GOTO _PARSE_CONFIG_LOOP                 ! to speed up parsing
$  ENDIF  ! extract <> "."
$
$  IF PARM1_UP .EQS. ".MAIL_LOGICAL"
$  THEN
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, mail logical \", PARM2, "\ going to be set to \", PARM3, "\"
$    MAIL_LOGICAL_NAME = PARM2
$    MAIL_LOGICAL_EQUIV = PARM3
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".MAIL"
$  THEN
$    IF DEBUG 
$    THEN 
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Mail group name: \", PARM2, "\, Users: \", PARM3, "\"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, assigned to mail group number ", MAIL_GROUP_COUNT
$    ENDIF ! debug
$    IF F$TYPE(MAIL_GROUP_'PARM2') .NES. ""
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Multiple definition of mail group \", PARM2, "\"
$      CONFIG_ERROR = TRUE
$    ENDIF
$    IF PARM2_UP .EQS. "SELF" .OR. PARM2_UP .EQS. "RETAIN" .OR. PARM2_UP .EQS. "SPECIFIC"
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, SELF, RETAIN, and SPECIFIC are illegal mail groups."
$      CONFIG_ERROR = TRUE
$    ENDIF
$    MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = PARM2
$    MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = PARM3
$    MAIL_GROUP_'PARM2' = MAIL_GROUP_COUNT
$    MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1
$    GOTO _PARSE_CONFIG_LOOP                 ! to speed up parsing
$  ENDIF
$
$  IF PARM1_UP .EQS. ".RETRY"
$  THEN
$    DSN_RETRY_MAX = F$INTEGER(PARM2)
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Retry set to ", DSN_RETRY_MAX
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".LOG_FILE"
$  THEN
$    LOG_FILE = F$PARSE(PARM2,LOG_FILE)
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, LOG file set to ", LOG_FILE
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".GET_FILE"
$  THEN
$    GET_FILE = F$PARSE(PARM2,GET_FILE)
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET file set to ", GET_FILE
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".GET_LOG_FILE"
$  THEN
$    GET_LOG_FILE = F$PARSE(PARM2,GET_LOG_FILE)
$    IF GET_LOG_FILE .EQS. ""
$    THEN
$      VALID_RECORD = FALSE
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Invalid get log file \", PARM2, "\"
$    ELSE
$      IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET log file set to ", GET_LOG_FILE
$      VALID_RECORD = TRUE
$    ENDIF
$  ENDIF
$
$  IF PARM1_UP .EQS. ".GET_DELETE_TYPE"
$  THEN
$    IF PARM2_UP .NES. "IMMEDIATE" .AND. PARM2_UP .NES. "NEVER" .AND. PARM2_UP .NES. "SUCCESS"
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Invalid GET_DELETE_TYPE"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-E-CONFIG, Valid types are IMMEDIATE, NEVER, SUCCESS"
$      VALID_RECORD = FALSE
$    ELSE
$      GET_DELETE_TYPE = PARM2
$      VALID_RECORD = TRUE
$      IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET Delete Type set to ", PARM2
$    ENDIF
$  ENDIF
$
$  IF PARM1_UP .EQS. ".GET_TIME"
$  THEN
$    IF PARM2_UP .NES. "EVENING" .AND. PARM2_UP .NES. "NOW" .AND. PARM2_UP .NES. "TOMORROW"
$    THEN
$      VALID_RECORD = FALSE
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Invalid GET_TIME"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-E-CONFIG, Valid times are EVENING, NOW, and TOMORROW"
$    ELSE
$      VALID_RECORD = TRUE
$      GET_TIME = PARM2
$      IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET TIME set to ", GET_TIME
$    ENDIF
$  ENDIF
$
$  IF PARM1_UP .EQS. ".SCRATCH_DIR"
$  THEN
$    DSN_SCRATCH_DIR = PARM2_UP
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, SCRATCH directory set to ", DSN_SCRATCH_DIR
$    VALID_RECORD = TRUE  ! until one of the tests fail
$    IF F$PARSE(DSN_SCRATCH_DIR) .EQS. ""
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, .SCRATCH_DIR isn't a valid directory"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-E-CONFIG, .SCRATCH_DIR value is: \''PARM2_UP'\"
$      VALID_RECORD = FALSE
$    ENDIF
$  ENDIF
$
$  IF PARM1_UP .EQS. ".SINCE"
$  THEN
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Since set to \", PARM2, "\"
$    DSN_SINCE = PARM2
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".MAIL_EMPTY"
$  THEN
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Mail empty set to \", PARM2, "\"
$    DSN_MAIL_EMPTY = PARM2
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".NOTIFY"
$  THEN
$    IF .NOT. "''PARM2'" THEN DSN_NOTIFY = "/noNOTIFY"
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".WRITE_TITLES"
$  THEN
$    WRITE_TITLES = F$PARSE(PARM2,"SYS$LOGIN:DSNLINK_GET.DAT")
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Titles will be written to \", WRITE_TITLES, "\"
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".NEXT_EXECUTION"
$  THEN
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Next execution set to \", PARM2, "\"
$    DSN_AFTER = PARM2
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".HISTORY_FILE"
$  THEN
$    DSN_HISTORY_FILE = F$PARSE(PARM2,"''DSNLINK_DIRECTORY'DSNLINK_NEW_HISTORY.DAT") - ";"
$    IF F$EDIT(PARM2,"UPCASE") .EQS. "NONE" THEN DSN_HISTORY_FILE = "NONE"
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, History file set to \", DSN_HISTORY_FILE, "\"
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".RETRY_WAIT_TIME"
$  THEN
$    DSN_RETRY_WAIT_TIME = F$EXTRACT(11,-1,F$CVTIME(PARM2,"COMPARISON"))
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Retry wait time set to \", DSN_RETRY_WAIT_TIME, "\"
$    VALID_RECORD = TRUE
$  ENDIF
$  
$  IF PARM1_UP .EQS. ".HISTORY_AUTO_COMPRESS"
$  THEN
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, History auto compress set to \", PARM2, "\"
$    DSN_HISTORY_AUTO_COMPRESS = F$INTEGER(PARM2)
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".CHECK_OPENCALLS"
$  THEN
$    IF PARM3 .EQS. "" THEN PARM3 = USERNAME
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Check opencalls: \", PARM2, "\, user: \", PARM3, "\"
$    DSN_CHECK_OPENCALLS = PARM2
$    ! now validate it
$    COUNT = 0
$    VALID_RECORD = TRUE    ! assume the best unless proven otherwise
$    _CONFIG_OPENCALLS_LOOP:
$    CHECK = F$ELEMENT(COUNT,",",DSN_CHECK_OPENCALLS)
$    IF CHECK .EQS. "," THEN GOTO _EXIT_CFG_OPENCALLS
$    IF F$TYPE(CHECK) .EQS. "INTEGER"
$    THEN
$    ! day of month.  Don't allow anything > 28 because of February
$      CHECK = F$INTEGER(CHECK)
$      IF CHECK .LT. 1 .OR. CHECK .GT. 28
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Value for Check_OpenCalls must be 1-28."
$        WRITE SYS$OUTPUT "\", CHECK, "\"
$        VALID_RECORD = FALSE
$      ENDIF
$    ELSE
$    ! day of week (Monday, Tuesday, etc.)
$      VALID_DAYS = "Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,Sunday,Always,Never"
$      IF F$LOCATE("," + F$EDIT(CHECK,"UPCASE") + ",", "," + F$EDIT(VALID_DAYS,"UPCASE") + ",") .EQ. F$LENGTH(VALID_DAYS) + 2
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Valid days for Check_OpenCalls are: ", VALID_DAYS
$        WRITE SYS$OUTPUT "\", CHECK, "\"
$        VALID_RECORD = FALSE
$      ENDIF
$    ENDIF ! type is integer
$    COUNT = COUNT + 1
$    GOTO _CONFIG_OPENCALLS_LOOP
$    _EXIT_CFG_OPENCALLS:
$    DSN_CHECK_OPENCALLS_USER = PARM3
$    IF DSN_CHECK_OPENCALLS_USER .NES. USERNAME
$    THEN      ! specified a different username
$      AUTHPRIV = "," + F$GETJPI(0,"AUTHPRIV") + ","
$      IF F$LOCATE(",CMKRNL,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV) .AND. -
          F$LOCATE(",SETPRV,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV)
$      THEN  ! don't have the necessary privilege
$        DSN_CHECK_OPENCALLS_USER = USERNAME
$        IF (DEBUG) .OR. (.NOT. INTERACTIVE)
$        THEN 
$          WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, CHECK_OPENCALLS needs CMKRNL or SETPRV to specify a different user"
$          WRITE SYS$OUTPUT "-DSNlink_NEW-W-CONFIG, CHECK_OPENCALLS output will be sent to ", DSN_CHECK_OPENCALLS_USER
$        ENDIF ! debug
$      ENDIF ! no priv
$    ENDIF ! different username
$  ENDIF ! check_opencalls
$
$  IF PARM1_UP .EQS. ".CHECK_CLOSEDCALLS"
$  THEN
$    IF PARM3 .EQS. "" THEN PARM3 = USERNAME
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Check closedcalls: \", PARM2, "\, user: \", PARM3, "\"
$    DSN_CHECK_CLOSEDCALLS = PARM2
$    ! now validate it
$    COUNT = 0
$    VALID_RECORD = TRUE    ! assume the best unless proven otherwise
$    _CONFIG_CLOSEDCALLS_LOOP:
$    CHECK = F$ELEMENT(COUNT,",",DSN_CHECK_CLOSEDCALLS)
$    IF CHECK .EQS. "," THEN GOTO _EXIT_CFG_CLOSEDCALLS
$    IF F$TYPE(CHECK) .EQS. "INTEGER"
$    THEN
$    ! day of month.  Don't allow anything > 28 because of February
$      CHECK = F$INTEGER(CHECK)
$      IF CHECK .LT. 1 .OR. CHECK .GT. 28
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Value for Check_ClosedCalls must be 1-28."
$        WRITE SYS$OUTPUT "\", CHECK, "\"
$        VALID_RECORD = FALSE
$      ENDIF
$    ELSE
$    ! day of week (Monday, Tuesday, wec.)
$      VALID_DAYS = "Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,Sunday,Always,Never"
$      IF F$LOCATE("," + F$EDIT(CHECK,"UPCASE") + ",", "," + F$EDIT(VALID_DAYS,"UPCASE") + ",") .EQ. F$LENGTH(VALID_DAYS) + 2
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Valid days for Check_ClosedCalls are: ", VALID_DAYS
$        WRITE SYS$OUTPUT "\", CHECK, "\"
$        VALID_RECORD = FALSE
$      ENDIF
$    ENDIF ! type is integer
$    COUNT = COUNT + 1
$    GOTO _CONFIG_CLOSEDCALLS_LOOP
$    _EXIT_CFG_CLOSEDCALLS:
$    DSN_CHECK_CLOSEDCALLS_USER = PARM3
$    IF DSN_CHECK_CLOSEDCALLS_USER .NES. USERNAME
$    THEN      ! specified a different username
$      AUTHPRIV = "," + F$GETJPI(0,"AUTHPRIV") + ","
$      IF F$LOCATE(",CMKRNL,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV) .AND. -
          F$LOCATE(",SETPRV,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV)
$      THEN  ! don't have the necessary privilege
$        DSN_CHECK_CLOSEDCALLS_USER = USERNAME
$        IF (DEBUG) .OR. (.NOT. INTERACTIVE)
$        THEN 
$          WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, CHECK_CLOSEDCALLS needs CMKRNL or SETPRV to specify a different user"
$          WRITE SYS$OUTPUT "-DSNlink_NEW-W-CONFIG, CHECK_CLOSEDCALLS output will be sent to ", DSN_CHECK_CLOSEDCALLS_USER
$        ENDIF ! debug
$      ENDIF ! no priv
$    ENDIF ! different username
$  ENDIF ! check_closedcalls
$
$  IF PARM1_UP .EQS. ".CHECK_DATABASES"
$  THEN
$    IF PARM3 .EQS. "" THEN PARM3 = "$ERROR$"
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Check new databases: \", PARM2, "\"
$    DSN_CHECK_DATABASES = PARM2
$    DSN_CHECK_DATABASES_FILE = F$PARSE(PARM4,DSN_CHECK_DATABASES_FILE_DEFAULT)
$    ! now validate it
$    COUNT = 0
$    VALID_RECORD = TRUE    ! assume the best unless proven otherwise
$    _CONFIG_DATABASES_LOOP:
$    CHECK = F$ELEMENT(COUNT,",",DSN_CHECK_DATABASES)
$    IF CHECK .EQS. "," THEN GOTO _EXIT_CFG_DATABASES
$    IF F$TYPE(CHECK) .EQS. "INTEGER"
$    THEN
$    ! day of month.  Don't allow anything > 28 because of February
$      CHECK = F$INTEGER(CHECK)
$      IF CHECK .LT. 1 .OR. CHECK .GT. 28
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Value for Check_databases must be 1-28."
$        WRITE SYS$OUTPUT "\", CHECK, "\"
$        VALID_RECORD = FALSE
$      ENDIF
$    ELSE
$    ! day of week (Monday, Tuesday, wec.)
$      VALID_DAYS = "Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,Sunday,Always,Never"
$      IF F$LOCATE("," + F$EDIT(CHECK,"UPCASE") + ",", "," + F$EDIT(VALID_DAYS,"UPCASE") + ",") .EQ. F$LENGTH(VALID_DAYS) + 2
$      THEN
$        WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Valid days for Check_databases are: ", VALID_DAYS
$        WRITE SYS$OUTPUT "\", CHECK, "\"
$        VALID_RECORD = FALSE
$      ENDIF
$    ENDIF ! type is integer
$    COUNT = COUNT + 1
$    GOTO _CONFIG_DATABASES_LOOP
$    _EXIT_CFG_DATABASES:
$    DSN_CHECK_DATABASES_USER = PARM3
$    IF F$TYPE(MAIL_GROUP_'DSN_CHECK_DATABASES_USER') .EQS. ""
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, No such mail group \", DSN_CHECK_DATABASES_USER, "\"
$      VALID_RECORD = FALSE
$    ENDIF
$  ENDIF ! check_databases
$
$  IF PARM1_UP .EQS. ".FAO_SUBJECT"
$  THEN
$    FAO_SUBJECT = F$ELEMENT(1,QUOTE,ORIG_RECORD)
$    IF DEBUG 
$    THEN 
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO Subject set to \", FAO_SUBJECT, "\"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (11,22,33,SYSTEMS,88): \", -
     F$FAO(FAO_SUBJECT, 11, 22, 33, "SYSTEMS", 88), "\"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (1,1,1,SYSTEMS,1)    : \", -
     F$FAO(FAO_SUBJECT, 1, 1, 1, "SYSTEMS", 1), "\"
$    ENDIF
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".FAO_PERSONAL"
$  THEN
$    FAO_PERSONAL = F$ELEMENT(1,QUOTE,ORIG_RECORD)
$    IF DEBUG 
$    THEN 
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO Personal set to \", FAO_PERSONAL, "\"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (11,22,33,SYSTEMS,88): \", -
     F$FAO(FAO_PERSONAL, 11, 22, 33, "SYSTEMS", 88), "\"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (1,1,1,SYSTEMS,1)     : \", -
     F$FAO(FAO_PERSONAL, 1, 1, 1, "SYSTEMS", 1), "\"
$    ENDIF
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".FAO_GET_SUBJECT"
$  THEN
$    FAO_GET_SUBJECT = F$ELEMENT(1,QUOTE,ORIG_RECORD)
$    IF DEBUG
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO GET Subject set to \", FAO_GET_SUBJECT, "\"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (Database,Title): \", -
     F$FAO(FAO_GET_SUBJECT,"Database","Title")
$    ENDIF
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".FAO_GET_PERSONAL"
$  THEN
$    FAO_GET_PERSONAL = F$ELEMENT(1,QUOTE,ORIG_RECORD)
$    IF DEBUG
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO GET Personal set to \", FAO_GET_PERSONAL, "\"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (Database,Title): \", -
     F$FAO(FAO_GET_PERSONAL,"Database","Title")
$    ENDIF
$    VALID_RECORD = TRUE
$  ENDIF
$
$! allow forced override of GET_STYLE (mostly for testing purposes)
$  IF PARM1_UP .EQS. ".GET_STYLE"
$  THEN
$    GET_STYLE = PARM2
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Get style set to \", GET_STYLE, "\"
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".BATCH_PRIORITY"
$  THEN
$    LOW_PRIORITY = F$INTEGER(PARM2)
$    IF (F$INTEGER(LOW_PRIORITY) .GT. PRIB) .AND. (.NOT. F$PRIVILEGE("ALTPRI"))
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-PRIORITY, Priority setting of \", PARM2, "\ ignored."
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-PRIORITY, ", PARM2, " is larger than base priority (", PRIB, ")"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-PRIORITY, ALTPRI is required to set higher priority."
$      LOW_PRIORITY = PRIB
$    ENDIF
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Batch priority set to \", LOW_PRIORITY, "\ (current PRIB=", PRIB, ")"
$    VALID_RECORD = TRUE
$  ENDIF
$
$  IF PARM1_UP .EQS. ".EXCLUDE"
$  THEN
$    VALID_RECORD = TRUE
$    EXCLUDE_ARTICLE_'EXC_COUNT' = F$EDIT(RECORD - PARM1 - PARM2,"TRIM")
$    EXCLUDE_DB_'EXC_COUNT' = PARM2
$    IF PARM2 .EQS. "*" THEN GOTO _END_EXC_DB_CHECK_LOOP
$
$    CC = 0
$    _EXC_DB_CHECK_LOOP:
$    IF F$TYPE(DB_NAME_'CC') .NES. "" THEN IF F$EDIT(PARM2,"UPCASE") .EQS. F$EDIT(DB_NAME_'CC',"UPCASE") THEN GOTO _END_EXC_DB_CHECK_LOOP
$    CC = CC + 1
$    IF CC .LT. DB_NAME_COUNT THEN GOTO _EXC_DB_CHECK_LOOP
$    WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, .EXCLUDE database /", PARM2, "/ is not defined."
$    VALID_RECORD = FALSE
$
$    _END_EXC_DB_CHECK_LOOP:
$    IF F$LOCATE(QUOTE,EXCLUDE_ARTICLE_'EXC_COUNT') .NE. F$LENGTH(EXCLUDE_ARTICLE_'EXC_COUNT') .AND. EXCLUDE_WARNING
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, Exclude on line ", LINE_NUMBER, " may unnecessarily include quote character."
$      WRITE SYS$OUTPUT "-DSNlink-NEW-W-CONFIG, Use .EXCLUDE_WARNING FALSE to disable this warning."
$    ENDIF
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Excluding article \", EXCLUDE_ARTICLE_'EXC_COUNT', "\"
$    EXC_COUNT = EXC_COUNT + 1
$  ENDIF
$
$  IF PARM1_UP .EQS. ".EXCLUDE_WARNING"
$  THEN
$    EXCLUDE_WARNING = 'PARM2_UP'       ! should be TRUE or FALSE
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, EXCLUDE_WARNING set to ", EXCLUDE_WARNING
$    VALID_RECORD = TRUE
$  ENDIF
$
$
$
$!
$!!!
$!
$
$  IF .NOT. VALID_RECORD 
$  THEN 
$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Invalid record \", ORIG_RECORD, "\, line ", LINE_NUMBER
$    CONFIG_ERROR = TRUE
$  ENDIF
$
$  GOTO _PARSE_CONFIG_LOOP
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$_END_PARSE_CONFIG:
$  CLOSE DSNLINK_NEW_CONFIGFILE
$  DB_NAME_COUNT = DB_NAME_COUNT - 1
$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT - 1
$  EXC_COUNT = EXC_COUNT - 1
$  MAX_EXC_COUNT = EXC_COUNT
$
$  IF F$TYPE(MAIL_GROUP_$ERROR$) .EQS. "" 
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-MISGRP, Missing $ERROR$ mail group."
$    CONFIG_ERROR = TRUE
$  ELSE
$    IF MAIL_GROUP_NAME_'MAIL_GROUP_$ERROR$' .NES. "$ERROR$"
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-MISGRP, $ERROR$ mail group must be uppercase."
$      CONFIG_ERROR = TRUE
$    ENDIF
$  ENDIF
$
$! the MX SITE agent's subprocess doesn't have SYS$LOGIN defined, which
$! causes problems when using SUBMIT/LOG_FILE -- so, we point it at
$! the DSNLINK_DIR location; then we make a test to see if that'll work....
$  IF F$TRNLNM("SYS$LOGIN") .EQS. "" 
$  THEN 
$    DEFINE SYS$LOGIN 'DSNLINK_DIRECTORY'
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, SYS$LOGIN was undefined; it has been set to ", F$TRNLNM("SYS$LOGIN")
$  ENDIF
$
$! in case the above trick didn't do it, we force the log file to the specific filename and location....
$  IF F$PARSE(GET_LOG_FILE) .EQS. "" .OR. GET_LOG_FILE .EQS. ""
$  THEN  
$    GET_LOG_FILE = DSNLINK_DIRECTORY + "DSNLINK_NEW_GET.LOG"
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET_LOG_FILE is undefined; attempting to"
$    WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, use DSNLINK_NEW_DIR:DSNLINK_NEW_GET.LOG (", F$PARSE(GET_LOG_FILE), ")"
$  ENDIF
$
$! accept "NO" and "0" to mean 'disable history file'
$  IF F$PARSE(DSN_HISTORY_FILE,,,"NAME") .EQS. "NO" .OR. F$PARSE(DSN_HISTORY_FILE,,,"NAME") .EQS. "0" 
$  THEN
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, History file disabled.  \", DSN_HISTORY_FILE, "\"
$    DSN_HISTORY_FILE = "NONE"
$  ENDIF
$
$  IF GET_STYLE .NES. "EDT" .AND. GET_STYLE .NES. "EXTRACT"
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, GET_STYLE must be EXTRACT or EDT."
$    CONFIG_ERROR = TRUE
$  ENDIF
$
$  IF F$TYPE(DB_NAME_0) .EQS. ""
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, No databases have been defined in config file."
$    CONFIG_ERROR = TRUE
$  ENDIF
$
$! accept "NONE" and "0" to mean 'don't schedule another execution.'
$  IF DSN_AFTER .EQS. "NONE" .OR. DSN_AFTER .EQS. "0"
$  THEN
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, DSN_AFTER set to ONE.  \", DSN_AFTER, "\"
$    DSN_AFTER = "ONE"
$  ENDIF
$
$  IF CONFIG_ERROR 
$  THEN 
$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-E-CONFIGERR, Configuration error(s) prevent running DSNlink_NEW.")
$    ERROR_STATUS = SS$_ABORT
$    ERROR_MESSAGE = "%DSNlink_NEW-E-CONFIGERR, Configuration error(s) in ''DSN_CONFIG_FILENAME'"
$    ! we will send a message to the user that submitted this DSNlink_NEW, 
$    ! and (when this configuration routine exits) also try to send a mail 
$    ! message to the $ERROR$ group about it.
$    IF (.NOT. INTERACTIVE) 
$    THEN
$      SEND_TO = USERNAME
$      IF F$TYPE(ORIGINALLY_FROM) .NES. "" THEN SEND_TO = ORIGINALLY_FROM
$      SET noON
$      MAIL/noSELF -
     SYS$INPUT: 'SEND_TO' -
     /SUBJECT="DSNlink_NEW configuration error in ''DSN_CONFIG_FILENAME'" -
     /PERSONAL_NAME="DSNlink_NEW configuration error"
DSNlink_NEW encountered a configuration error while executing in non-
interactive mode (Batch or Detached).  

For further information, check .LOG file and the DSNlink_NEW
configuration file; if using DELIVER or MX SITE agents, check those logs
as well.
 
... attempting to also inform user(s) in group $ERROR$ about the problem.

This message sent automatically by DSNlink_NEW.
$      SET ON
$    ENDIF  ! interactive
$  ENDIF ! configuration error
$
$  IF BATCH THEN SET RESTART_VALUE
$  RETURN       !  leaving _INIT_PARSE_CONFIG
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_DSNLINK_GET:
$  PERFORMING_GET = TRUE
$  IF .NOT. BATCH THEN WRITE SYS$OUTPUT F$FAO("  !24<Batch log file:!>!AS", GET_LOG_FILE)
$  IF BATCH THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-GET, Performing article GET processing."
$  IF DEBUG .OR. BATCH THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-GET, Get style: ", GET_STYLE
$  WRITE SYS$OUTPUT F$FAO("  !24<Connection retries:!>!AS", F$STRING(DSN_RETRY_MAX))
$
$! modify mail groups a bit....
$
$  COUNT = 0
$_REWRITE_LOOP:
$  IF MAIL_GROUP_NAME_'COUNT' .EQS. "$ERROR$" 
$  THEN
$    IF P5 .EQS. ""
$    THEN
$      MAIL_GROUP_USERS_'COUNT' = USERNAME
$      ORIGINALLY_FROM = USERNAME
$    ELSE
$      MAIL_GROUP_USERS_'COUNT' = P5
$      ORIGINALLY_FROM = P5
$    ENDIF
$    IF (DEBUG) .OR. (.NOT. INTERACTIVE) THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-REWROTE, Rewrote $ERROR$ group to ", MAIL_GROUP_USERS_'COUNT'
$  ENDIF  ! error group
$  COUNT = COUNT + 1
$  IF COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _REWRITE_LOOP
$  IF ORIGINALLY_FROM .NES. USERNAME THEN WRITE SYS$OUTPUT F$FAO("  !24<Originally from:!>!AS", ORIGINALLY_FROM)
$
$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1        ! create another group
$  MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = "Self"
$  MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = USERNAME
$  MAIL_GROUP_Self = MAIL_GROUP_COUNT
$  PREFERRED = MAIL_GROUP_COUNT
$
$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1        ! even another group
$  MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = "Retain"
$  MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = "<send to assigned mail groups>"
$  MAIL_GROUP_Retain = MAIL_GROUP_COUNT
$
$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1        ! even another group
$  MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = "Specific"
$  MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = "<send to specific user(s)>"
$  MAIL_GROUP_Specific = MAIL_GROUP_COUNT
$
$  IF P3 .NES. "" 
$  THEN 
$    GET_FILENAME = P3
$  ELSE
$    GET_FILENAME = GET_FILE
$  ENDIF
$  IF F$SEARCH(GET_FILENAME) .EQS. ""
$  THEN
$    ERROR_MESSAGE = "%DSNlink_NEW-E-GETFILE, Missing file \''GET_FILENAME'\"
$    ERROR_STATUS = SS$_NOSUCHFILE
$    GOTO _EXIT_ERROR
$  ENDIF
$  GET_FILENAME = F$SEARCH(GET_FILENAME)
$  WRITE SYS$OUTPUT F$FAO("  !24<Get filename:!>!AS", GET_FILENAME)
$
$  IF (BATCH) .AND. (P6 .NES. "")  ! if P6 is non-null, then we should delete file after we're done (if all works out)
$  THEN
$    DELETE_ORIGINAL_GET_FILE = TRUE
$    ORIGINAL_GET_FILE = P6
$    WRITE SYS$OUTPUT F$FAO("  !24<Delete orig GET file:!>Yes, !AS",ORIGINAL_GET_FILE)
$  ELSE
$    DELETE_ORIGINAL_GET_FILE = FALSE
$    WRITE SYS$OUTPUT F$FAO("  !24<Delete orig GET file:!>No")
$  ENDIF
$
$  IF P7 .EQS. ""
$  THEN 
$    RETRYING_MAX_COUNT = 1
$  ELSE 
$    RETRYING_MAX_COUNT = P7
$  ENDIF
$
$  CREATE_CMD = FALSE
$  IF BATCH THEN CREATE_CMD = TRUE
$
$  GOSUB _PROCESS_GET_FILE
$
$  IF TOTAL_ARTICLE_COUNT .EQ. 0
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-NOGET, No articles to get - invalid GET file."
$    WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOGET, Enable DSNlink_NEW debugging or delete"
$    WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOGET, ", GET_FILE, " and start over."
$    ERROR_MESSAGE = "%DSNlink_NEW-E-GETFILE, No articles to get - invalid GET file."
$    ERROR_STATUS = (SS$_ABORT .OR. STS$M_INHIB_MSG)
$    GOTO _EXIT_ERROR
$  ENDIF
$
$  IF BATCH THEN GOTO _GET_BATCH
$
$  IF .NOT. INTERACTIVE 
$  THEN ! validate group name
$    GRPNAME = F$ELEMENT(0,"=",P4)
$    USRNAME = F$ELEMENT(1,"=",P4)
$    IF F$TYPE(MAIL_GROUP_'GRPNAME') .NES. ""
$    THEN
$      WHICH_GROUP = MAIL_GROUP_'GRPNAME'
$      IF USRNAME .NES. "=" THEN SPECIFIC_USER = USRNAME
$      AFTER_QUAL = ""
$      GOTO _GET_SKIP_PROMPTS
$    ENDIF  ! found it
$    ERROR_MESSAGE = "%DSNlink_NEW-E-GROUP, Invalid or missing group name P4: \''P4'\"
$    ERROR_STATUS = SS$_BADPARAM
$    GOTO _EXIT_ERROR
$  ENDIF ! .not. interactive
$
$  WRITE SYS$OUTPUT F$FAO("!/  !24<Mail groups!>Users")
$  COUNT = 0
$  LOW = 99
$  HIGH = -1
$_DB_DISPLAY_LOOP_2:
$  IF MAIL_GROUP_NAME_'COUNT' .NES. "$ERROR$" 
$  THEN
$    TMP = F$FAO(" !2UL. !15AS      !50AS", COUNT, MAIL_GROUP_NAME_'COUNT', MAIL_GROUP_USERS_'COUNT')
$    IF F$LENGTH(MAIL_GROUP_USERS_'COUNT') .GT. 50 THEN TMP = TMP + TOO_LONG_CHAR
$    WRITE SYS$OUTPUT TMP
$    IF COUNT .GT. HIGH THEN HIGH = COUNT
$    IF COUNT .LT. LOW THEN LOW = COUNT
$  ENDIF
$  COUNT = COUNT + 1
$  IF COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _DB_DISPLAY_LOOP_2
$
$  WRITE SYS$OUTPUT F$FAO("!/Ready to retrieve articles and Email to users.")
$_GET_GRP_NUMBER:
$  ASK "* Send mail to which group? (''LOW'-''HIGH') [''PREFERRED'] " WHICH_GROUP
$  WHICH_GROUP = F$EDIT(WHICH_GROUP,"TRIM")
$  IF WHICH_GROUP .EQS. "" THEN WHICH_GROUP = PREFERRED
$  IF F$TYPE(WHICH_GROUP) .NES. "INTEGER" 
$  THEN
$    WRITE SYS$OUTPUT "Invalid - must be a number"
$    GOTO _GET_GRP_NUMBER
$  ENDIF
$  WHICH_GROUP = F$INTEGER(WHICH_GROUP)
$  IF F$TYPE(MAIL_GROUP_NAME_'WHICH_GROUP') .EQS. "" 
$  THEN
$    WRITE SYS$OUTPUT "Invalid mail group number"
$    GOTO _GET_GRP_NUMBER
$  ENDIF ! valid number
$  IF MAIL_GROUP_NAME_'WHICH_GROUP' .EQS. "$ERROR$"
$  THEN
$    WRITE SYS$OUTPUT "Invalid mail group number - cannot use $ERROR$ group."
$    GOTO _GET_GRP_NUMBER
$  ENDIF ! $error$
$  
$  IF MAIL_GROUP_NAME_'WHICH_GROUP' .EQS. "Retain"
$  THEN        ! check to make sure the config file has user mappings for the Databases specified in the GET file
$    RETAIN_OKAY = TRUE
$    OLD_DB = ""
$    COUNT1 = 0
$    _CHECK_RETAIN_LOOP_1:
$    COUNT2 = 0
$    _CHECK_RETAIN_LOOP_2:
$    IF F$EDIT(GET_DATABASE_'COUNT1',"UPCASE") .EQS. F$EDIT(DB_NAME_'COUNT2',"UPCASE")
$    THEN                               ! found the database
$      IF OLD_DB .NES. GET_DATABASE_'COUNT1' THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-USERS, Text from ", GET_DATABASE_'COUNT1', " will be mailed to ", DB_MAIL_'COUNT2'
$      OLD_DB = GET_DATABASE_'COUNT1'
$      COUNT1 = COUNT1 + 1
$      IF COUNT1 .LT. TOTAL_ARTICLE_COUNT THEN GOTO _CHECK_RETAIN_LOOP_1
$      GOTO _EXIT_CHECK_RETAIN
$    ENDIF ! database not found
$    COUNT2 = COUNT2 + 1
$    IF COUNT2 .LT. DB_NAME_COUNT THEN GOTO _CHECK_RETAIN_LOOP_2  ! check next DB name
$    IF OLD_DB .NES. GET_DATABASE_'COUNT1' THEN WRITE SYS$OUTPUT "%DSNlink_NEW-E-NOUSER, Can't use RETAIN: Config doesn't have ", GET_DATABASE_'COUNT1'
$    OLD_DB = GET_DATABASE_'COUNT1'
$    RETAIN_OKAY = FALSE
$    COUNT1 = COUNT1 + 1
$    IF COUNT1 .LT. TOTAL_ARTICLE_COUNT THEN GOTO _CHECK_RETAIN_LOOP_1
$  _EXIT_CHECK_RETAIN:
$    IF .NOT. RETAIN_OKAY THEN GOTO _GET_GRP_NUMBER
$  ENDIF ! retain
$
$  SPECIFIC_USER = ""
$  IF MAIL_GROUP_NAME_'WHICH_GROUP' .EQS. "Specific"
$  THEN
$    _GET_SPECIFIC_USERNAME:
$    ASK "* Enter specific usernames: " SPECIFIC_USER
$    IF SPECIFIC_USER .EQS. "" .OR. F$LOCATE(QUOTE,SPECIFIC_USER) .NE. F$LENGTH(SPECIFIC_USER) -
     .OR. SPECIFIC_USER .EQS. "?"
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-E-INVUSER, Invalid: cannot be blank, cannot contain quotes"
$      GOTO _GET_SPECIFIC_USERNAME
$    ENDIF
$    SPECIFIC_USER = F$EDIT(SPECIFIC_USER,"COLLAPSE")
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-USERS, Sending to: ", SPECIFIC_USER
$  ENDIF ! specific
$
$!
$!!!
$!
$
$  AFTER_QUAL = "\"
$  IF DSN_AFTER .EQS. "ONE" THEN DSN_AFTER = "TOMORROW+01:00:00"
$  PREFERRED = "T"
$  IF F$TYPE(GET_TIME) .NES. ""  THEN PREFERRED = F$EXTRACT(0,1,F$EDIT(GET_TIME,"UPCASE"))
$_REPROMPT_WHEN:
$  WRITE SYS$OUTPUT ""
$  ASK "* Submit Now, this Evening, or Tomorrow? (N/E/T) [''PREFERRED'] " SUBMIT_TIME
$  SUBMIT_TIME = F$EXTRACT(0,1,F$EDIT(SUBMIT_TIME,"UPCASE,COLLAPSE"))
$  IF SUBMIT_TIME .EQS. "" THEN SUBMIT_TIME = PREFERRED
$  IF SUBMIT_TIME .EQS. "N" THEN AFTER_QUAL = ""
$  IF SUBMIT_TIME .EQS. "E" THEN AFTER_QUAL = "/AFTER=18:00"
$  IF SUBMIT_TIME .EQS. "T" THEN AFTER_QUAL = "/AFTER=" + QUOTE + F$CVTIME(DSN_AFTER,"ABSOLUTE") + QUOTE
$  IF AFTER_QUAL .EQS. "\"
$  THEN
$    WRITE SYS$OUTPUT "Invalid - must be N, E, or T"
$    GOTO _REPROMPT_WHEN
$  ENDIF
$  IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, AFTER_QUAL is \", AFTER_QUAL, "\"
$
$_GET_SKIP_PROMPTS:
$  GET_TEMPFILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_GET_" + F$GETJPI(0,"PID") + "_" + -
     F$CVTIME(,,"HOUR") + F$CVTIME(,,"MINUTE") + F$CVTIME(,,"SECOND") + ".DAT"
$  CREATE 'GET_TEMPFILE'
$  OPEN/APPEND DSNLINK_NEW_OUTFILE 'GET_TEMPFILE'
$  WRITE DSNLINK_NEW_OUTFILE "! ", F$SEARCH(GET_TEMPFILE)
$  WRITE DSNLINK_NEW_OUTFILE "! Created by DSNlink_NEW, version ", DSNLINK_NEW_VERSION
$  WRITE DSNLINK_NEW_OUTFILE "! file used for GET processing."
$  WRITE DSNLINK_NEW_OUTFILE F$FAO("!! Created at !%D by user !AS::!AS", 0, F$GETSYI("NODENAME"), USERNAME)
$  WRITE DSNLINK_NEW_OUTFILE "!"
$  CLOSE DSNLINK_NEW_OUTFILE
$  APPEND 'GET_FILENAME' 'GET_TEMPFILE'
$
$  IF SPECIFIC_USER .EQS. ""
$  THEN
$    TMP = MAIL_GROUP_NAME_'WHICH_GROUP'
$  ELSE
$    TMP = MAIL_GROUP_NAME_'WHICH_GROUP' + "=" + SPECIFIC_USER
$  ENDIF
$
$  IF ORIGINALLY_FROM .EQS. USERNAME
$  THEN
$    NOTIFY_QUAL = "/NOTIFY"
$  ELSE
$    NOTIFY_QUAL = "/noNOTIFY"     ! we're doing this 100% on someone else's behalf (MX SITE or DELIVER)
$  ENDIF
$
$  ORIGINAL_GET_FILE = ""
$
$! see if there's a new default
$  PREFERRED = "S"
$  IF F$TYPE(GET_DELETE_TYPE) .EQS. "STRING"    ! use default from configuration file
$  THEN
$    IF F$EDIT(GET_DELETE_TYPE,"UPCASE") .EQS. "IMMEDIATE" THEN PREFERRED = "Y"
$    IF F$EDIT(GET_DELETE_TYPE,"UPCASE") .EQS. "NEVER"     THEN PREFERRED = "N"
$    IF F$EDIT(GET_DELETE_TYPE,"UPCASE") .EQS. "SUCCESS"   THEN PREFERRED = "S"
$  ENDIF
$
$  IF INTERACTIVE
$  THEN
$    _ASK_DEL_GET_FILE:
$    WRITE SYS$OUTPUT ""
$    ASK "* Delete all versions of your GET file? (Y/N/S/?) [''PREFERRED'] " DELETE_GET
$    DELETE_GET = F$EXTRACT(0,1,F$EDIT(DELETE_GET,"UPCASE,COLLAPSE"))
$    IF DELETE_GET .EQS. "" THEN DELETE_GET = PREFERRED
$    IF DELETE_GET .EQS. "?" 
$    THEN
$      WRITE SYS$OUTPUT "Your GET file (", F$PARSE(";*",GET_FILENAME), ") is no longer needed."
$      WRITE SYS$OUTPUT "DSNlink_NEW has made a copy of your get file for its use in the batch job."
$      WRITE SYS$OUTPUT "You can safely delete this file."
$      WRITE SYS$OUTPUT "  Y - Delete GET file immediately"
$      WRITE SYS$OUTPUT "  N - Don't delete GET file"
$      WRITE SYS$OUTPUT "  S - Only delete GET file if GET Processing is Successful"
$      GOTO _ASK_DEL_GET_FILE
$    ENDIF
$    IF DELETE_GET .EQS. "Y" THEN DELETE/LOG 'F$PARSE(";*",GET_FILENAME)'
$    IF DELETE_GET .EQS. "S" THEN ORIGINAL_GET_FILE = F$PARSE(";*",GET_FILENAME)
$    IF DELETE_GET .NES. "Y" .AND. DELETE_GET .NES. "N" .AND. DELETE_GET .NES. "S"
$    THEN
$      WRITE SYS$OUTPUT "Invalid - must be Y, N, or ?"
$      GOTO _ASK_DEL_GET_FILE
$    ENDIF
$  ENDIF ! interactive
$
$  SUBMIT -
     'NOTIFY_QUAL' -
     /QUEUE=DSN$BATCH -
     /noPRINTER -
     'AFTER_QUAL' -
     /KEEP -
     /PARAMETERS=( -
          'DSN_CONFIG_FILENAME', -      ! P1 (keep the version number)
          "$GET$", -                    ! P2
          'F$SEARCH(GET_TEMPFILE)', -   ! P3 - remove logicals -- another node may have different ideas about where SYS$SCRATCH is located
          "''TMP'",   -                 ! P4
          "''ORIGINALLY_FROM'", -       ! P5
          "''ORIGINAL_GET_FILE'") -     ! P6
     /NAME="DSNlink_NEW:get" -
     /LOG_FILE='GET_LOG_FILE' -
     'F$PARSE(";",F$ENVIRONMENT("PROCEDURE"))'
$
$  FINISHED_OK = TRUE
$  GOTO _EXIT
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_GET_BATCH:
$  
$  GOSUB _PHONE_DSN_HOST
$
$  LAST_OPEN_DB = ""
$
$  CURRENT_COUNT = 0
$_GET_SENDMAIL_LOOP:
$  DATABASE = GET_DATABASE_'CURRENT_COUNT'
$  TITLE = GET_TITLE_'CURRENT_COUNT'
$  TITLE_NOQUOTES = TITLE
$  _REMOVE_QUOTES:
$  IF F$LOCATE(QUOTE,TITLE_NOQUOTES) .NE. F$LENGTH(TITLE_NOQUOTES)
$  THEN
$    TITLE_NOQUOTES[F$LOCATE(QUOTE,TITLE_NOQUOTES),1] := " "
$    GOTO _REMOVE_QUOTES
$  ENDIF
$
$  FILENAME_ZERO = TMP_EXTRACTFILE + "_" + F$STRING(CURRENT_COUNT)
$  TRYING = 0
$_RETRYING_ARTICLE_FETCH:
$  TRYING = TRYING + 1
$  FILENAME = FILENAME_ZERO + "_" + F$STRING(TRYING)
$  IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, ", TRYING, "-th retrieved file is: ", FILENAME
$
$  IF F$SEARCH(FILENAME) .EQS. ""
$  THEN
$    IF RETRYING_MAX_COUNT .GT. 1
$    THEN
$      CREATE 'FILENAME'
$      OPEN/APPEND FILE 'FILENAME'
$      WRITE FILE "Couldn't find the article:"
$      WRITE FILE "  Database:  ", DATABASE
$      WRITE FILE F$EXTRACT(0,79,"  Title:     " + TITLE)
$      WRITE FILE "  For group: ", P4
$      CLOSE FILE
$      CALL _SEND_MAIL_MESSAGE -
     'FILENAME' -
     "DSNlink article not found; ''DATABASE' database" -
     $ERROR$ -
     "''DATABASE' DSNlink article not found"
$    ELSE
$      IF F$TRNLNM("DSNLINK_NEW_RETRY_GET") .EQS. "" THEN OPEN/WRITE DSNLINK_NEW_RETRY_GET 'TMP_RETRYFILE'
$      IF LAST_OPEN_DB .NES. DATABASE
$      THEN
$        WRITE DSNLINK_NEW_RETRY_GET DATABASE, " database, articles: unknown"  ! we might parse this later
$        LAST_OPEN_DB = DATABASE
$      ENDIF
$      WRITE DSNLINK_NEW_RETRY_GET "  ", TITLE
$    ENDIF
$  ELSE
$    TMP1 = F$FAO(FAO_GET_SUBJECT,DATABASE,TITLE_NOQUOTES)
$    IF F$LENGTH(TMP1) .GT. 71 THEN TMP1[71,1] := "''TOO_LONG_CHAR'"
$    TMP1 = F$EXTRACT(0,72,TMP1)
$
$    TMP2 = F$FAO(FAO_GET_PERSONAL,DATABASE,TITLE_NOQUOTES)
$
$    SET FILE/VERSION_LIMIT=2 'FILENAME'
$
$! get title (which is the second record) 
$    OPEN/READ FILE 'FILENAME'
$    LINE_COUNT = 2
$    IF DSNLINK_STYLE .NE. DSNLINK_STYLE_22 THEN -
          READ FILE JUNK/END_OF_FILE=_LINE_COUNT_EXIT
$    READ FILE ACTUAL_TITLE/END_OF_FILE=_LINE_COUNT_EXIT
$
$! verify title is what we want, and if not we may try another
$    IF DEBUG
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, title of retrieved article: \" + ACTUAL_TITLE + "\"
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, title of wanted article: \" + TITLE + "\"
$    ENDIF
$    ! try another if titles don't match
$    IF F$EDIT(ACTUAL_TITLE,"UPCASE,COLLAPSE") .NES. -
        F$EDIT(TITLE,"UPCASE,COLLAPSE")
$    THEN
$      CLOSE FILE
$      GOTO _RETRYING_ARTICLE_FETCH
$    ENDIF ! title mismatch
$
$! count lines
$    _LINE_COUNT_LOOP:
$    READ FILE RECORD/END_OF_FILE=_LINE_COUNT_EXIT
$    LINE_COUNT = LINE_COUNT + 1
$    GOTO _LINE_COUNT_LOOP
$    _LINE_COUNT_EXIT:
$
$    CLOSE FILE
$    CREATE 'FILENAME';            ! create new version
$    OPEN/APPEND FILE 'FILENAME'
$    WRITE FILE "  Database:      " + DATABASE
$    TMP = "  Title:         " + ACTUAL_TITLE
$    IF F$LENGTH(TMP) .GT. 79 THEN TMP[79,1] := "''TOO_LONG_CHAR'"
$    WRITE FILE F$EXTRACT(0,80,TMP)
$
$    ! display warning if they mismatch
$    IF F$EDIT(ACTUAL_TITLE,"UPCASE,COLLAPSE") .NES. F$EDIT(TITLE,"UPCASE,COLLAPSE") 
$    THEN 
$      TMP =  "  Desired title: " + TITLE
$      IF F$LENGTH(TMP) .GT. 79 THEN TMP[79,1] := "''TOO_LONG_CHAR'"
$      WRITE FILE F$EXTRACT(0,80,TMP)
$      WRITE FILE "  Warning:       Titles don't match"
$      WRITE FILE "                 Article may need to be retrieved from DSNlink manually"
$      TMP1 = F$EXTRACT(0,72,"(title mismatch) " + TMP1)
$    ENDIF ! title mismatch
$
$    WRITE FILE "  Lines:         ", LINE_COUNT
$    IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_22 THEN WRITE FILE ""
$    CLOSE FILE
$    SET NOON  ! ignore 'INCOMPAT' warning with DSNlink V2.2 (DSNLINK_STYLE_22)
$    APPEND 'FILENAME';-1 'FILENAME'
$    SET ON
$    CALL _SEND_MAIL_MESSAGE -
     'FILENAME' -
     "''TMP1'" -
     "''P4'" -
     "''TMP2'"
$  ENDIF ! extract file exists
$
$  CURRENT_COUNT = CURRENT_COUNT + 1
$  IF CURRENT_COUNT .LT. TOTAL_ARTICLE_COUNT THEN GOTO _GET_SENDMAIL_LOOP
$
$  FINISHED_OK = TRUE
$  IF F$TRNLNM("DSNLINK_NEW_RETRY_GET") .NES. ""
$  THEN
$    CLOSE DSNLINK_NEW_RETRY_GET
$    PERM_RETRYFILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_GET_RETRY.TMP"
$    COPY 'TMP_RETRYFILE' 'PERM_RETRYFILE'
$    SUBMIT -
     'DSN_NOTIFY' -
     /QUEUE=dsn$batch -
     /noPRINTER -
     /KEEP -
     /NAME="DSNlink_NEW:get10" -
     /LOG_FILE='GET_LOG_FILE' -
     /PARAMETERS=( -
          'DSN_CONFIG_FILENAME', -       ! P1 (keep version number)
          "$GET$", -                     ! P2
          'PERM_RETRYFILE', -            ! P3 (get file)
          "''P4'", -                     ! P4
          "''P5'", -                     ! P5
          "''P6'", -                     ! P6
          'DSNLINK_GET_ARTICLE_COUNT') - ! P7 (# of articles to retrieve)
     /NAME="DSNlink_NEW:get_''DSNLINK_GET_ARTICLE_COUNT'" -
     'F$PARSE(";",F$ENVIRONMENT("PROCEDURE"))'     
$  ELSE
$    IF DELETE_ORIGINAL_GET_FILE THEN DELETE'DEBUG_LOG' 'ORIGINAL_GET_FILE'
$  ENDIF
$  GOTO _EXIT
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$_PROCESS_GET_FILE:
$! process contents of GET file and create ITS command file
$
$  IF DEBUG
$  THEN
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Contents of GET file:"
$    TYPE 'GET_FILENAME' 
$  ENDIF  ! debug
$
$  OPEN/READ DSNLINK_NEW_INFILE 'GET_FILENAME'
$
$  IF CREATE_CMD 
$  THEN 
$    OPEN/WRITE DSNLINK_NEW_OUTFILE 'TMP_DSNFILE'
$    WRITE DSNLINK_NEW_OUTFILE ""
$  ENDIF
$
$  ARTICLE_COUNT = 0
$  TOTAL_ARTICLE_COUNT = 0
$  LAST_CLOSED_DB = ""
$  LAST_OPENED_DB = ""
$  DB = ""
$
$_GET_VALID_LOOP:
$  READ/END_OF_FILE=_DONE_GET_VALID_LOOP DSNLINK_NEW_INFILE RECORD
$  IF F$EXTRACT(0,1,RECORD) .EQS. ">" .OR. F$EXTRACT(0,1,RECORD) .EQS. "#" THEN -
     RECORD = F$EXTRACT(1,-1,RECORD)
$  IF RECORD .EQS. "_____" THEN GOTO _DONE_GET_VALID_LOOP
$  IF ((F$ELEMENT(1," ",RECORD) .EQS. "database," .OR. F$ELEMENT(2," ",RECORD) .EQS. "database,")) .AND. -
      ((F$LOCATE(" article:",RECORD) .NE. F$LENGTH(RECORD) .OR. F$LOCATE(" articles:",RECORD) .NE. F$LENGTH(RECORD))) .AND. -
      (F$EXTRACT(0,1,RECORD) .NES. " ")
$  THEN
$    IF F$ELEMENT(1," ",RECORD) .EQS. "database," THEN DB = F$ELEMENT(0," ",RECORD)  ! "xxx database, NN articles:"
$    IF F$ELEMENT(2," ",RECORD) .EQS. "database," THEN DB = F$ELEMENT(1," ",RECORD)  ! "FLASH xxx database, NN articles:"
$  ELSE
$    IF F$EXTRACT(0,2,RECORD) .EQS. "  " 
$    THEN      ! first two characters of this record are spaces
$      RECORD = F$EDIT(RECORD,"COMPRESS,TRIM")  ! remove junk
$
$
$      ! new database opened yet? 
$      ! (we hold off on doing the OPEN until we've got an article title, 
$      !  to prevent the overhead of OPENing a database and then not 
$      !  retrieving any articles)
$      IF DB .NES. LAST_OPENED_DB
$      THEN 
$
$        ! check to see if we have to close the last-opened database
$        IF LAST_OPENED_DB .NES. DB .AND. LAST_OPENED_DB .NES. ""  ! If we're on to a new database
$        THEN 
$          IF CREATE_CMD THEN WRITE DSNLINK_NEW_OUTFILE "close ", LAST_OPENED_DB
$          LAST_CLOSED_DB = LAST_OPENED_DB
$          WRITE SYS$OUTPUT F$FAO("!28* !20AS get !UL article!%S", LAST_OPENED_DB, ARTICLE_COUNT)
$        ENDIF 
$
$        ! now ready to open the new database
$        LAST_OPENED_DB = DB
$        ARTICLE_COUNT = 0
$        IF CREATE_CMD 
$        THEN
$          WRITE DSNLINK_NEW_OUTFILE ""
$          WRITE DSNLINK_NEW_OUTFILE "open ", DB
$        ENDIF ! create_cmd
$      ENDIF ! db <> last_opened_db
$
$      IF LAST_OPENED_DB .NES. ""
$      THEN
$        ! if we've opened a database ...
$        IF CREATE_CMD
$        THEN
$          ! perform the search
$          WRITE DSNLINK_NEW_OUTFILE "search/english ", RECORD
$          IF DEBUG THEN WRITE DSNLINK_NEW_OUTFILE "directory"
$          TRYING = 0
$          _RETRYING_ARTICLE_GET:
$          TRYING = TRYING + 1
$          IF GET_STYLE .EQS. "EDT"
$          THEN
$            WRITE DSNLINK_NEW_OUTFILE "next/edit"
$            ! now we're talking to EDT
$            WRITE DSNLINK_NEW_OUTFILE "EXIT " + TMP_EXTRACTFILE + "_" + F$STRING(TOTAL_ARTICLE_COUNT) + "_" + F$STRING(TRYING)
$            ! now we're back talking to DSN ITS
$          ENDIF ! get_style
$          IF GET_STYLE .EQS. "EXTRACT"
$          THEN
$            WRITE DSNLINK_NEW_OUTFILE "next"
$            WRITE DSNLINK_NEW_OUTFILE "extract " + TMP_EXTRACTFILE + "_" + F$STRING(TOTAL_ARTICLE_COUNT) + "_" + F$STRING(TRYING)
$          ENDIF ! get_style
$          IF TRYING .LT. RETRYING_MAX_COUNT THEN GOTO _RETRYING_ARTICLE_GET
$        ENDIF ! create_cmd 
$        GET_TITLE_'TOTAL_ARTICLE_COUNT' = RECORD
$        GET_DATABASE_'TOTAL_ARTICLE_COUNT' = DB
$        ARTICLE_COUNT = ARTICLE_COUNT + 1
$        TOTAL_ARTICLE_COUNT = TOTAL_ARTICLE_COUNT + 1
$      ENDIF ! LAST_OPENED_DB <> ""
$    ELSE  ! there was some text in the first two spaces, but it wasn't a new database
$      LAST_OPENED_DB = ""
$      DB = ""
$    ENDIF ! F$EXTRACT(0,2,RECORD) = "  " 
$  ENDIF  ! record type (database, title, or 'other' [ignore 'other' records])
$  GOTO _GET_VALID_LOOP
$
$_DONE_GET_VALID_LOOP:
$  IF ARTICLE_COUNT .GT. 0 THEN WRITE SYS$OUTPUT F$FAO("!28* !20AS get !UL article!%S", DB, ARTICLE_COUNT)
$  CLOSE DSNLINK_NEW_INFILE
$  IF CREATE_CMD 
$  THEN 
$    WRITE DSNLINK_NEW_OUTFILE ""
$    WRITE DSNLINK_NEW_OUTFILE "exit"
$    CLOSE DSNLINK_NEW_OUTFILE
$  ENDIF
$
$  RETURN  ! from _PROCESS_GET_FILE
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$
$_DO_CHECK_OPENCALLS:
$  if DSNLINK_STYLE.EQ.DSNLINK_STYLE_22 
$  then
$	set noon
$	DSN FETCH OPEN/output='tmp_opencallsfile'
$	if .not. $status then $ goto _ERROR
$! for V2.2C .nes. "%X100002D4" 
$	set on
$	MAIL 'tmp_opencallsfile' -
	/NOSELF -
	'DSN_CHECK_OPENCALLS_USER' -
	/SUBJECT="DSNlink_NEW-initiated check of DSNlink open calls" 
$  else
$  IF DSN_CHECK_OPENCALLS_USER .NES. USERNAME
$  THEN
$    ! need to submit under a different username
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-OPENCALLS, Submitting DSNlink_NEW with CHECKOPEN with username: ", DSN_CHECK_OPENCALLS_USER 
$    IF DEBUG
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Will attempt to create log file in .SCRATCH_DIR"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, this may fail if the user can't write to .SCRATCH_DIR"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, .SCRATCH_DIR is set in DSNLINK_NEW_CONFIG.DAT"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, Value is: \", DSN_SCRATCH_DIR, "\"
$      CHECKOPEN_LOG = "/LOG_FILE=" + DSN_SCRATCH_DIR + "DSNLINK_NEW_CHECK.LOG" 
$    ELSE
$      CHECKOPEN_LOG = "/noLOG_FILE"
$    ENDIF
$    OLDPRIV = F$SETPRV("CMKRNL")  ! save current privilege setting in OLDPRIV
$    SUBMIT -
     'DSN_NOTIFY' -
     /NAME="DSNlink_NEW_checkopen" -
     'CHECKOPEN_LOG' -
     /noPRINTER -
     /QUEUE=DSN$BATCH -
     /USER='DSN_CHECK_OPENCALLS_USER' -
     /PARAMETERS=( -
          'DSN_CONFIG_FILENAME', -      ! P1 (keep the version number)
          "$CHECKOPEN$") -              ! P2 
     'F$ENVIRONMENT("PROCEDURE")'
$    OLDPRIV = F$SETPRV(OLDPRIV)  ! restore previous setting of CMKRNL
$    GOTO _DONE_CHECK_OPENCALLS
$  ELSE
$    ! this username
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-OPENCALLS, Sending mail to DSN%OPENCALLS. (", F$TIME(), ")"
$    MAIL -
     /noSELF -
     SYS$INPUT: -
     DSN%OPENCALLS -
     /SUBJECT="DSNlink_NEW-initiated check of DSNlink open calls" 
This check of DSNlink open calls was initiated by code within DSNlink_NEW.
$    WAIT 00:00:01
$    DSN SHOW BATCH      ! see if we see our job
$  ENDIF ! separate user
$
$  ENDIF  ! V2.2 Style
$_DONE_CHECK_OPENCALLS:
$  RETURN ! from _DO_CHECK_OPENCALLS
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$_DO_CHECK_CLOSEDCALLS:
$  if DSNLINK_STYLE.EQ.DSNLINK_STYLE_22 
$  then
$	set noon
$	DSN FETCH CLOSED/output='tmp_closedcallsfile'
$       if .not. $status then $ goto _ERROR
$! .nes. "%X100002D4"
$       set on
$	MAIL 'tmp_closedcallsfile' -
	/NOSELF -
	'DSN_CHECK_CLOSEDCALLS_USER' -
/SUBJECT="DSNlink_NEW-initiated check of DSNlink closed calls" 
$  else
$  IF DSN_CHECK_CLOSEDCALLS_USER .NES. USERNAME
$  THEN
$    ! need to submit under a different username
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CLOSEDCALLS, Submitting DSNlink_NEW with CHECKCLOSED with username: ", DSN_CHECK_CLOSEDCALLS_USER 
$    IF DEBUG
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Will attempt to create log file in .SCRATCH_DIR"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, this may fail if the user can't write to .SCRATCH_DIR"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, .SCRATCH_DIR is set in DSNLINK_NEW_CONFIG.DAT"
$      WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, Value is: \", DSN_SCRATCH_DIR, "\"
$      CHECKCLOSED_LOG = "/LOG_FILE=" + DSN_SCRATCH_DIR + "DSNLINK_NEW_CHECK.LOG" 
$    ELSE
$      CHECKCLOSED_LOG = "/noLOG_FILE"
$    ENDIF
$    OLDPRIV = F$SETPRV("CMKRNL")  ! save current privilege setting in OLDPRIV
$    SUBMIT -
     'DSN_NOTIFY' -
     /NAME="DSNlink_NEW_checkclosed" -
     'CHECKCLOSED_LOG' -
     /noPRINTER -
     /QUEUE=DSN$BATCH -
     /USER='DSN_CHECK_CLOSEDCALLS_USER' -
     /PARAMETERS=( -
          'DSN_CONFIG_FILENAME', -      ! P1 (keep the version number)
          "$CHECKCLOSED$") -              ! P2 
     'F$ENVIRONMENT("PROCEDURE")'
$    OLDPRIV = F$SETPRV(OLDPRIV)  ! restore previous setting of CMKRNL
$    GOTO _DONE_CHECK_CLOSEDCALLS
$  ELSE
$    ! this username
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CLOSEDCALLS, Sending mail to DSN%CLOSEDCALLS. (", F$TIME(), ")"
$    MAIL -
     /noSELF -
     SYS$INPUT: -
     DSN%CLOSEDCALLS -
     /SUBJECT="DSNlink_NEW-initiated check of DSNlink closed calls" 
This check of DSNlink closed calls was initiated by code within DSNlink_NEW.
$    WAIT 00:00:01
$    DSN SHOW BATCH      ! see if we see our job
$  ENDIF ! separate user
$
$  ENDIF ! V2.2 Style
$_DONE_CHECK_CLOSEDCALLS:
$  RETURN ! from _DO_CHECK_CLOSEDCALLS
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!
$_DO_CHECK_DATABASES:
$  CHECKING_DATABASES = TRUE
$  IF P2 .EQS. "$CHECKDATABASES$"
$  THEN
$    ! we really have to do it (there's an ELSE way down there..)
$    OPEN/WRITE DSNLINK_NEW_FILE 'TMP_DSNFILE'
$    WRITE DSNLINK_NEW_FILE "SHOW DATABASE"
$    WRITE DSNLINK_NEW_FILE "EXIT"
$    CLOSE DSNLINK_NEW_FILE
$
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CHECKDB, Performing database check"
$    GOSUB _PHONE_DSN_HOST
$
$    DEFINE/USER_MODE SYS$OUTPUT NLA0:
$    DEFINE/USER_MODE SYS$ERROR NLA0:
$    SEARCH/EXACT 'TMP_OUTFILE' "-E-","*** Failed to establish DsnIts session!"
$    SEARCH_STATUS = $STATUS
$    IF SEARCH_STATUS .EQ. SS$_NORMAL .OR. .NOT. DSNLINK_ITS_EXIT_STATUS
$    THEN
$      WRITE SYS$OUTPUT "%DSNlink_NEW-W-CANTCHECK, Failed connecting to DSNlink host - aborting database check."
$      WRITE SYS$OUTPUT "-DSNlink_NEW-W-ERRCODES, DSNLINK_ITS_EXIT_STATUS=''DSNLINK_ITS_EXIT_STATUS', SEARCH_STATUS=''SEARCH_STATUS'"
$      RETURN
$    ENDIF
$    
$    OPEN/READ DSNLINK_NEW_FILE 'TMP_OUTFILE'
$    CREATE 'TMP_CHECKFILE'/FDL=SYS$INPUT:
FILE
  BEST_TRY_CONTIGUOUS  yes
  ORGANIZATION         INDEXED
RECORD
  FORMAT  FIXED
  SIZE    43                  ! database + date (DD-MMM-YYYY) = 32 + 11
KEY 0
  INDEX_COMPRESSION  yes      ! lots of whitespace in database names
  DUPLICATES         no
  NULL_KEY           no
  SEG0_LENGTH        32
  SEG0_POSITION      0
$    OPEN/READ/WRITE DSNLINK_NEW_OUTFILE 'TMP_CHECKFILE'
$
$!
$! populate indexed file with data from the output of the record 
$! SHOW DATABASE command
$!
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Looking for beginning of list of databases..."
$    _CHK_DB_LOOP_1:
$    READ/END_OF_FILE=_CHK_DB_LOOP_1_END/ERROR=_ERROR DSNLINK_NEW_FILE RECORD
$    IF DEBUG THEN WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, Record: \", RECORD, "\"
$    IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_1X .AND. -
        RECORD .EQS. "The currently available databases are:" THEN GOTO _CHK_DB_LOOP_2
$    IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_22 .AND. -
        RECORD .EQS. "-------------                   --------------------" THEN GOTO _CHK_DB_LOOP_2
$    GOTO _CHK_DB_LOOP_1
$    _CHK_DB_LOOP_2:
$    READ/END_OF_FILE=_CHK_DB_LOOP_2_END/ERROR=_ERROR DSNLINK_NEW_FILE RECORD
$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Record: \", RECORD, "\"
$! all databases start with database name in first position
$! end of list is string "No databases are currently open" (style 1.x) or
$! "its> " (style 2.2)
$    IF RECORD .EQS. "its> " .OR. -
        RECORD .EQS. "No databases are currently open" -
     THEN GOTO _CHK_DB_LOOP_2_END
$    IF DSNLINK_STYLE .EQ. DSNLINK_STYLE_1X 
$    THEN 
$      DB_NAME = F$EDIT(F$ELEMENT(1," ",RECORD),"COLLAPSE")
$    ELSE
$      DB_NAME = F$ELEMENT(0," ",RECORD)
$    ENDIF
$    IF F$EXTRACT(1,1,RECORD) .NES. " " .AND. -
        DB_NAME .NES. ""
$    THEN
$      IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Found database \", DB_NAME, "\"
$      !+-+
$      ! some broken DSNlink database listings have duplicate listings of
$      ! database names for some reason.  As our indexed file doesn't allow
$      ! duplicates, we delete the key if it already exists
$      TMPKEY = F$FAO("!32AS", DB_NAME)
$      READ/DELETE/KEY=&TMPKEY/ERROR=_CHK_WRITE DSNLINK_NEW_OUTFILE JUNK
$      _CHK_WRITE:
$      !-+-
$      WRITE DSNLINK_NEW_OUTFILE F$FAO("!32AS!11%D", DB_NAME, 0)
$    ENDIF
$    GOTO _CHK_DB_LOOP_2
$
$    _CHK_DB_LOOP_1_END:
$    _CHK_DB_LOOP_2_END:
$    CLOSE DSNLINK_NEW_OUTFILE
$    CLOSE DSNLINK_NEW_FILE
$    
$    IF DEBUG 
$    THEN 
$      WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Database file:"
$      TYPE 'TMP_CHECKFILE' 
$    ENDIF
$
$    OPEN/WRITE DSNLINK_NEW_OUTFILE 'TMP_MAILFILE'       ! for the generated mail message
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!  LOOK FOR MISSING DATABASES ....
$
$    MISSING_COUNT = 0
$    IF F$SEARCH(DSN_CHECK_DATABASES_FILE) .EQS. "" 
$    THEN 
$      CREATE 'DSN_CHECK_DATABASES_FILE'/FDL=SYS$INPUT:
FILE
  BEST_TRY_CONTIGUOUS  yes
  ORGANIZATION         INDEXED
RECORD
  FORMAT  FIXED
  SIZE    43             ! database + date (DD-MMM-YYYY) = 32 + 11
KEY 0
  INDEX_COMPRESSION  yes  ! lots of whitespace in database names
  DUPLICATES         no
  NULL_KEY           no
  SEG0_LENGTH        32
  SEG0_POSITION      0
$
$    ENDIF
$
$!
$!   Database file records look like:
$!   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxDD-MMM-YYYY
$!     Where "xxx..." is the database name (32 characters)
$!     The date is used to indicate when the database was last seen
$!
$    ! open the reference ("old") database
$    OPEN/READ DSNLINK_NEW_REFFILE 'DSN_CHECK_DATABASES_FILE'
$    ! open the new database
$    OPEN/READ DSNLINK_NEW_SEARCHFILE 'TMP_CHECKFILE'
$
$    _CHK_DB_LOOP_3:
$    ! read record from reference ("old") file
$    READ/END_OF_FILE=_CHK_DB_LOOP_3_END/ERROR=_ERROR DSNLINK_NEW_REFFILE RECORD
$    REF_DATE   = F$EXTRACT(32,11,RECORD)
$    REF_RECORD = F$EXTRACT(0,32,RECORD)
$
$    ! now try to find the record in the new file
$    READ/KEY=&REF_RECORD/ERROR=_CHK_DB_MISSING_NF DSNLINK_NEW_SEARCHFILE JUNK
$    GOTO _CHK_DB_LOOP_3
$
$    _CHK_DB_MISSING_NF:
$    ! we didn't find it
$    IF MISSING_COUNT .EQ. 0 THEN WRITE DSNLINK_NEW_OUTFILE "Missing database(s):"
$    MISSING_COUNT = 1
$    WRITE DSNLINK_NEW_OUTFILE F$FAO("  Database !32AS last seen !AS, !AS", REF_RECORD, -
     F$CVTIME(REF_DATE,,"WEEKDAY"), REF_DATE)
$    GOTO _CHK_DB_LOOP_3
$    _CHK_DB_LOOP_3_END:
$    CLOSE DSNLINK_NEW_SEARCHFILE
$    CLOSE DSNLINK_NEW_REFFILE
$
$
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$!!!!!  LOOK FOR NEW DATABASES ....
$
$    NEW_COUNT = 0
$    ! open the reference ("old") file
$    OPEN/READ DSNLINK_NEW_REFFILE 'DSN_CHECK_DATABASES_FILE'
$    ! open the new file
$    OPEN/READ DSNLINK_NEW_SEARCHFILE 'TMP_CHECKFILE'
$
$    _CHK_DB_LOOP_4:
$    ! read record from new file
$    READ/END_OF_FILE=_CHK_DB_LOOP_4_END/ERROR=_ERROR DSNLINK_NEW_SEARCHFILE RECORD
$    NEW_RECORD = F$FAO("!32AS",F$EXTRACT(0,32,RECORD))
$
$    ! see if record is in reference ("old") file
$    READ/KEY=&NEW_RECORD/ERROR=_CHK_DB_NEW_NF DSNLINK_NEW_REFFILE JUNK
$    GOTO _CHK_DB_LOOP_4
$
$    _CHK_DB_NEW_NF:
$    ! we didn't find it
$    IF NEW_COUNT .EQ. 0 THEN WRITE DSNLINK_NEW_OUTFILE "New database(s):"
$    NEW_COUNT = NEW_COUNT + 1
$    WRITE DSNLINK_NEW_OUTFILE "  ", NEW_RECORD
$    GOTO _CHK_DB_LOOP_4
$
$    _CHK_DB_LOOP_4_END:
$    CLOSE DSNLINK_NEW_REFFILE
$    CLOSE DSNLINK_NEW_SEARCHFILE
$    CLOSE DSNLINK_NEW_OUTFILE
$
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-RESULTS, Database check results:  new=", NEW_COUNT, ", missing=", MISSING_COUNT
$    IF NEW_COUNT .GT. 0 .OR. MISSING_COUNT .GT. 0 THEN -
          CALL _SEND_MAIL_MESSAGE 'TMP_MAILFILE' "DSNlink_NEW - new/missing ITS databases" 'DSN_CHECK_DATABASES_USER'
$
$! delete old reference file, make new file the reference file
$    DELETE 'DSN_CHECK_DATABASES_FILE'*
$    COPY 'TMP_CHECKFILE' 'DSN_CHECK_DATABASES_FILE'
$    
$  ELSE
$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DATABASES, Submitting DSNlink_NEW (again) to check databases."
$    WRITE SYS$OUTPUT "-DSNlink_NEW-I-TWICE, Attempting to get two connections to DSNlink host."
$    MY_QUEUE = F$GETQUI("DISPLAY_JOB","QUEUE_NAME",,"THIS_JOB")
$    IF MY_QUEUE .EQS. "" THEN MY_QUEUE = "DSN$BATCH"
$    DATABASE_LOG_FILE = F$PARSE("DSNLINK_NEW_DATABASES.LOG",LOG_FILE)
$    SUBMIT -
     'DSN_NOTIFY' -
     /NAME="DSNlink_NEW:chk_db" -
     /LOG='DATABASE_LOG_FILE' -
     /noPRINTER -
     /QUEUE='MY_QUEUE' -
     /PARAMETERS=( -
          'DSN_CONFIG_FILENAME', -        ! P1 (keep the version number)
          "$CHECKDATABASES$") -           ! P2 
     'F$ENVIRONMENT("PROCEDURE")'
$  ENDIF
$
$_DONE_CHECK_DATABASES:
$  RETURN ! from _DO_CHECK_DATABASES
$
$!
$!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
$
$! Convert history file to the new format (old format has 20-character 
$! database names, new format has 32 character database names).
$!
$
$_NEW_HISTORY_FORMAT:
$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTCONV, Converting history file to new format ..."
$  WRITE SYS$OUTPUT "-DSNlink_NEW-I-HISTORY, File ", F$SEARCH(DSN_HISTORY_FILE)
$
$  OPEN/READ  DSNLINK_NEW_FILE    'DSN_HISTORY_FILE'
$  CREATE/FDL=SYS$INPUT: 'TMP_DSNFILE'
FILE
  BEST_TRY_CONTIGUOUS  yes
  ORGANIZATION         INDEXED
  EXTENSION            9           ! in case it never gets COMPRESSed
RECORD
  FORMAT  FIXED
  SIZE    120        ! date (YYYY-MM-DD) + database + title = 10 + 32 + 78
KEY 0
  INDEX_COMPRESSION  no
  DUPLICATES         no
  NULL_KEY           no
  SEG0_LENGTH        110
  SEG0_POSITION      10
$
$  OPEN/READ/WRITE DSNLINK_NEW_OUTFILE 'TMP_DSNFILE'
$  JUNK = 0
$
$  _NEW_HIST_LOOP:
$  READ/END_OF_FILE=_END_NEW_HIST_LOOP DSNLINK_NEW_FILE RECORD
$  RECORD = F$EXTRACT(0,30,RECORD) + F$FAO("!#* ",12) + F$EXTRACT(30,-1,RECORD)
$  WRITE/SYMBOL DSNLINK_NEW_OUTFILE RECORD
$  JUNK = JUNK + 1
$  IF (JUNK / 200) * 200 .EQ. JUNK THEN WRITE SYS$OUTPUT "-DSNlink_NEW-I-HISTCONV, Converting history file, record ", JUNK
$  GOTO _NEW_HIST_LOOP
$  _END_NEW_HIST_LOOP:
$
$  CLOSE DSNLINK_NEW_FILE
$  CLOSE DSNLINK_NEW_OUTFILE
$  COPY 'TMP_DSNFILE' 'DSN_HISTORY_FILE'
$  PURGE 'DSN_HISTORY_FILE'
$  DELETE 'TMP_DSNFILE';
$  WRITE SYS$OUTPUT "%DSNlink_NEW-S-HISTCONV, Done converting history file to new format"
$  WRITE SYS$OUTPUT ""
$  RETURN
