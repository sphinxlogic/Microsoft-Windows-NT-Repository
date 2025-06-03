$ v='f$verify(f$trnlnm("SHARE_UNPACK_VERIFY"))'
$!
$! This archive created:
$!  Name : DSNlink_NEW, version V5.3
$!  By   : Dan Wing <wing@tgv.com>
$!  Date : 11-OCT-1995 15:59:51.37
$!  Using: VMS_SHARE 8.5-1, (C) 1993 Andy Harper, Kings College London UK
$!
$! Credit is due to these people for their original ideas:
$!    James Gray, Michael Bednarek 
$!
$! To unpack this archive:
$!    Minimum of VMS 5.4 (VAX) / OpenVMS 1.0 (Alpha) is required.
$!    Remove the headers of the first part, up to `cut here' line.
$!    Execute file as a command procedure.
$!
$! The following file(s) will be created after unpacking:
$!       1. DSNLINK_NEW.NEW_FEATURES;12
$!       2. DSNLINK_NEW.DOC;11
$!       3. DSNLINK_NEW_CONFIG.DAT_TEMPLATE;12
$!       4. DSNLINK_NEW.COM;106
$!       5. MX_SITE_DELIVER.COM_TEMPLATE;3
$!       6. PMDF_DELIVER.COM_TEMPLATE;3
$!
$ set="set"
$ set symbol/scope=(nolocal,noglobal)
$ f="SYS$SCRATCH:."+f$getjpi("","PID")+";"
$ if f$trnlnm("SHARE_UNPACK") .nes. "" then $ -
 f=f$parse("SHARE_UNPACK_TEMP",f)
$ e="write sys$error  ""%UNPACK"", "
$ w="write sys$output ""%UNPACK"", "
$ if .not. f$trnlnm("SHARE_UNPACK_LOG") then $ w = "!"
$ if f$getsyi("CPU") .gt. 127 then $ goto start
$ ve=f$getsyi("version")
$ if ve-f$extract(0,1,ve) .ges. "5.4" then $ goto start
$ e "-E-OLDVER, Must run at least VMS 5.4"
$ v=f$verify(v)
$ exit 44
$unpack:subroutine!P1=file,P2=chksum,P3=attrib,P4=size,P5=fileno,P6=filetotal
$ if f$parse(P1) .nes. "" then $ goto dirok
$ dn=f$parse(P1,,,"DIRECTORY")
$ w "-I-CREDIR, Creating directory ''dn'"
$ create/dir 'dn'
$ if $status then $ goto dirok
$ e "-E-CREDIRFAIL, Unable to create ''dn' File skipped"
$ delete 'f'*
$ exit
$dirok:
$ x=f$search(P1)
$ if x .eqs. "" then $ goto file_absent
$ e "-W-EXISTS, File ''P1' exists. Skipped"
$ delete 'f'*
$ exit
$file_absent:
$ w "-I-UNPACK, Unpacking ", P5, " of ", P6, " - ", P1, " - ", P4, " Blocks"
$ n=P1
$ if P3 .nes. "" then $ n=f
$ if .not. f$verify() then $ define/user sys$output nl:
$ EDIT/TPU/NOSEC/NODIS/COM=SYS$INPUT/NOJOURNAL 'f'/OUT='n'
PROCEDURE GetHex(s,p)LOCAL x1,x2;x1:=INDEX(t,SUBSTR(s,p,1))-1;x2:=INDEX(t,
SUBSTR(s,p+1,1))-1;RETURN 16*x1+x2;ENDPROCEDURE;PROCEDURE SkipPartsep LOCAL m;
LOOP m:=MARK(NONE);EXITIF m=END_OF(CURRENT_BUFFER);DELETE(m);EXITIF INDEX(
ERASE_LINE,"-+-+-+-+-+-+-+-+")=1;ENDLOOP;ENDPROCEDURE;
PROCEDURE ProcessLine LOCAL c,s,l,b,n,p;s := ERASE_LINE;EDIT(s,TRIM);c :=
 SUBSTR(s,1,1);s := s-c;IF c = "X" THEN SPLIT_LINE; ENDIF;MOVE_HORIZONTAL(-1);
l := LENGTH(s);p := 1;LOOP EXITIF p > l;c := SUBSTR(s,p,1);p := p+1;
CASE c FROM ' ' TO '`' ['&']: b:=GetHex(s,p); n:=GetHex(s,p+2); p:=p+4;
 COPY_TEXT(ASCII(n)*b);['`']: COPY_TEXT(ASCII(GetHex(s,p))); p:=p+2;[INRANGE,
OUTRANGE]: COPY_TEXT(c);ENDCASE;ENDLOOP;ENDPROCEDURE;PROCEDURE Decode(b)
LOCAL m;POSITION(BEGINNING_OF(b));LOOP m:=MARK(NONE);EXITIF m=END_OF(b);DELETE(
m);IF INDEX(CURRENT_LINE,"+-+-+-+-+-+-+-+-")=1 THEN SkipPartSep;
ELSE ProcessLine;MOVE_HORIZONTAL(1);ENDIF;ENDLOOP;ENDPROCEDURE;SET(
FACILITY_NAME,"UNPACK");SET(SUCCESS,OFF);SET(INFORMATIONAL,OFF);t:=
"0123456789ABCDEF";f:=GET_INFO(COMMAND_LINE,"file_name");o:=CREATE_BUFFER(f,f);
Decode(o);WRITE_FILE(o,GET_INFO(COMMAND_LINE,"output_file"));QUIT;
$ if p3 .eqs. "" then $ goto dl
$ open/write fdl &f
$ write fdl "RECORD"
$ write fdl P3
$ close fdl
$ w "-I-CONVRFM, Converting record format to ", P3
$ convert/fdl='f' 'f'-1 'f'
$ fa=f$getdvi(f$parse(f),"ALLDEVNAM")
$ Pa=f$getdvi(f$parse(P1),"ALLDEVNAM")
$ if fa .eqs. Pa then $ rename &f 'f$parse(P1)'
$ if fa .nes. Pa then $ copy &f 'f$parse(P1)'
$dl: delete 'f'*
$ checksum 'P1'
$ if checksum$checksum .nes. P2 then $ -
  e "-E-CHKSMFAIL, Checksum of ''P1' failed."
$ exit
$ endsubroutine
$start:
$!
$ create 'f'
X*`20
X* DSNlink_NEW, version V5.3, released October, 1995
X*`20
X*   DSNlink_NEW will automatically retrieve titles of new DSNlink ITS`20
X*   articles from all the ITS databases you'd like to search.  The new
X*   article titles are mailed to a user or group of users.  DSNlink_NEW`20
X*   version 5 (and up) includes the ability to automatically retreive the
X*   full article text (not just the titles), and integrates with MX or PMDF's
V
X*   DELIVER (or standalone DELIVER).
X*  `20
X*   Please contact the author at wing@tgv.com if you encounter any bugs, or`20
X*   have questions, problems, or enhancement requests.
X*  `20
X*
X* NEW FEATURES AND BUG FIXES:
X*  `20
X* V5.3
X*&0620o Allow user to exclude specific article titles (which are updated
X*&0820often by Digital, causing them to re-appear in the DSNlink_NEW mail
X*&0820messages).
X*
X* V5.2-1
X*&0620o For Get Processing, allow user to not have DSNLINK_GET.DAT file
X*&0820deleted if the GET Processing fails (due to DSNlink host being
X*&0820down, or other reason).  Requested by <friedberg@comets.com>.
X* V5.2
X*&0620o Allow PMDF's DELIVER and standalone DELIVER to work with`20
X*&0820DSNlink_NEW's GET Processing.  Previously, only functionality with`20
X*&0820MX's SITE agent was supplied.
X*&0620o SITE_DELIVER.COM_TEMPLATE is now named MX_SITE_DELIVER.COM_TEMPLATE
X*&0620o Don't attempt to reconnect to DSNlink host if we receive any of
X*&0820the following errors (as they won't be corrected by retrying):
X*&0A20CSCERRAC, CSC call tracking database is unavailable
X*&0A20NOTREGSTRD, application is not registered for this access number
X*&0620o V1.2A apparently needs the same handling for GET that was
X*&0820introduced in DSNlink_NEW V5.1-1, so make the "slow" GET processing`20
X*&0820happen if V1.2A is detected (reported by rdickens@datastream.co.uk).
X*
X* V5.1-3
X*&0620o  Allow GET file to have ">" or "#" as first character (to handle
X*&0920cases where these 'quote' characters are automatically inserted`20
X*&0920by a mailer when Forwarding mail).
X*&0620o  Allow specifying priority of DSNlink_NEW when it is doing batch
X*&0920job work with new .BATCH_PRIORITY configuration file setting.
X*
X* V5.1-2`20
X*&0620o  Fix bug with handling two tick marks ('') in the title of a`20
X*&0920retreived article.
X*
X* V5.1-1
X*&0620o  Allow GET Processing to work with old (V1.1 and V1.1-1) versions
X*&0920of DSNlink.
X*&0620o  Minor enhancement to SITE_DELIVER.COM_TEMPLATE for sites that don't
X*&0920have SYS$SCRATCH defined in the system logical name table.
X*&0620o  Eliminate final "no" from DSN ITS commands if running DSNlink V1.2
X*&0920or higher (see section 4.17 of DSNlink V1.2 release notes).
X*&0620o  Fixed obscure problem that could cause deletion of files without
X*&0920file extentions.
X*&0920(Note:  Version number determination uses undocumented DSN$VERSION
X*&1120symbol which is created by the $ DSN VERSION command,
X*&1120which only exists in DSNlink V1.2 and higher;
X*&1120DSNlink_NEW assumes V1.1 if the DSN$VERSION symbol isn't
X*&1120changed by the $ DSN VERSION command.)
X*
X* V5.1
X*&0620o  VMSmail subject and personal-name are now site-configurable.
X*&0620o  Enhancements to GET processing to work with MX SITE agent, DELIVER,
X*&0920and PMDF.
X*&0620o  Scratch directory now configurable (.SCRATCH_DIR).
X*&0620o  Check closedcalls added to complement V5.0's check opencalls.
X*&0620o  Fix bug when .CHECK_OPENCALLS wasn't in the configuration file.
X*&0620o  Don't set protection of temporary GET file because of some site's
X*&0920handling of SYS$SCRATCH file ownership and privileges.
X*&0620o  New .WRITE_TITLES configuration file setting.
X*
X* V5.0
X*&0620o  Ability to retrieve article text (not just titles) by parsing a
X*&0920mail message sent by a 'normal' execution of DSNlink_NEW.
X*&0620o  Show number of articles retrieved from each database.
X*&0620o  Inform $ERROR$ group when we get UNKDBNAM error from DSNlink
X*&0920(instead of informing all DSNlink_NEW groups).
X*&0620o  Configuration file parser no longer uppercases all input, and will
X*&0920respect your use of lowercase and uppercase without modification.
X*&0920New template configuration file (DSNLINK_NEW_CONFIG.DAT_TEMPLATE)`20
X*&0920shows changes.
X*&0620o  Send Email if there is a configuration file error when running in`20
X*&0920batch (it used to just quietly die if configuration file couldn't be
X*&0920parsed).
X*&0620o  Ability to check DSNlink open calls on a certain schedule (mail is`20
X*&0920only sent to user running DSNlink_NEW and to DSN$MAIL_CC_RECIPIENT).
X*&0620o  Don't update history date each time we re-retrieve an article;
X*&0920just update if the article is new, or if last retrieved was
X*&0920ALLOW_IF_OLDER (or more) days ago.
X*&0620o  Minor changes with debugger output during configuration file`20
X*&0920parsing.
X*&0620o  Fixed other problems and inconsistencies with case-sensitivity and
X*&0920error message displays.
X*&0620o  Default auto-purge of history file increased from 300 to 600 blocks.
V
X*&0620o  Rewritten documentation.
X*
X* V4.3-1
X*&0620o  Ensure history file exists before trying to display its size.
X*&0620o  Add Digital-recommended runtimes to documentation.
X*
X* V4.3
X*&0620o  Add ability to automatically compress history file.
X*
X* V4.2-2
X*&0620o  Fixed bug so database name is now displayed when history is`20
X*&0920disabled.
X*
X* V4.2-1
X*&0620o  Display translations of unexpected error messages by using
X*&0920SET MESSAGE with several different message files.
X*
X* V4.2
X*&0620o  Separate history database for flash/normal articles by using
X*&0920upper-/lower-case for database name within history database.
X*
X* V4.1-8
X*&0620o  Change error handling so we always retry if there were any errors
X*&0920during the DSNlink connection.
X*
X* V4.1-6
X*&0620o  Fixed bugs when history is disabled
X*&0620o  Updated DSNLINK_NEW_CONFIG.DAT_TEMPLATE
X*
X* V4.1-5
X*&0620o  Disregard upper-/lower-case in config file.
X*
X* V4.1-4
X*&0620o  All messages now use %DSNlink_NEW as facility code, and other
X*&0920cleanups.
X*&0620o  Set DSNLINK_NEW_DIR to location of DSNLINK_NEW.COM instead of
X*&0920SYS$LOGIN.
X*
X* V4.1-3
X*&0620o  DSNLINK_NEW_DIR logical, and .LOG_FILE in configuration file
X*
X* V4.1-2
X*&0620o  More verbose output to the .LOG file.
X*
X* V4.1-1
X*&0620o  A configuration file controls all major settings, eliminating all
X*&0920prompting for information.  A template file is provided to assist`20
X*&0920with the initial setup.
X*&0620o  FLASH articles are now automatically retrieved, and are put at the
X*&0920top of DSNlink_NEW mail messages.
X*&0620o  A history file prevents mailing article titles that were previously
X*&0920retrieved by DSNlink_NEW.
X*&0620o  Support is provided for using one DSNlink_NEW batch job to send
X*&0920new articles to groups of different users.  For example, the systems
X*&0920group can receive new articles from databases X, Y, and Z, and`20
X*&0920programmers can receive new articles from databases X, Y, and W.
X*&0620o  Documentation is in a separate file instead of imbedded as comments
X*&0920in the first few hundred lines of code.
X*&0620o  Errors with DSNlink_NEW or with connecting to the DSNlink host are
X*&0920mailed to a special group, instead of all recipients as in previous
X*&0920versions.
X*&0620o  DSNlink_NEW mail message indicates connect time with DSNlink host.
X*&0620o  Improved error handling if DSNlink session is dropped.
X* `20
$ call unpack DSNLINK_NEW.NEW_FEATURES;12 273305661 "" 16 1 6
$!
$ create 'f'
X
X
X&1F20DSNlink_NEW
X
X&1620Installation and Users Guide
X
X
X&1E20Version  V5.3
X&1E20October, 1995
X
X
X&0820DSNlink_NEW will automatically retrieve titles of new DSN
X&0820ITS articles from all the ITS databases you'd like to
X&0820search.  The new article titles are mailed to a user or
X&0820group of users.  The text of the articles can also be`20
X&0820retreived for you automatically (called "Get Processing").
X
XDSNlink_NEW has been tested with DSNlink V1.2b, but is expected to work
Xwith all versions of DSNlink, and all versions of OpenVMS that are
Xsupported by DSNlink.  "DSNlink" is also called "AES" in some regions.
X
XContents:
X
X  1.  Setting up DSNlink_NEW
X  2.  Configuration file
X  3.  GET Processing
X  3a. Automating GET Processing using MX, PMDF, or DELIVER
X  4.  DSNlink_NEW's handling of SINCE
X  5.  Compressing history manually
X  6.  DSNlink_NEW wish list
X  7.  Old, boring DSNlink_NEW modification history
X
X
X&4E3D
X
XIf you have questions, comments, bug reports, or enhancement requests for
XDSNlink_NEW, please contact the author at:
X
X&0820Dan Wing
X&0820TGV, Inc.
X&0820101 Cooper St.
X&0820Santa Cruz, CA  95060
X
X&0820Phone:     408-461-8701 / 408-457-5200
X&0820Internet:  wing@tgv.com
X&1320wing@eisner.decus.org
X
XDSNlink_NEW is copyright `A9 1991-1995 by Dan Wing.  This code may be freely`20
V
Xdistributed and modified for no commercial gain as long as this copyright`20
Xnotice is retained.  This program is supplied 'as-is', and with no warranty.
X
XThe DECUServe notes conference "VMS", note 1889.*, contains discussions
Xabout DSNlink_NEW.
X
XThe original idea for this code taken from DSNlink article "`5BDSNlink`5D
XV1.n How To Get Daily List of New Database Articles" in the DSNLINK
Xdatabase.  An early version of DSNlink_NEW can also be found there. `20
X
X
X
X&4E3D
X&17201. Setting up DSNlink_NEW`20
X&4E3D
X
XTo get DSNlink_NEW up and running quickly, simply copy all the files to
Xa directory on your system, edit the file DSNLINK_NEW_CONFIG.DAT_TEMPLATE
Xas necessary for your site, and save the modified version as`20
XDSNLINK_NEW_CONFIG.DAT. `20
X
XThen execute DSNLINK_NEW.COM, and it will parse the configuration file,
Xdisplay information from the configuration file, and ask you if the
Xinformation is correct.  When you are satisfied with the configuration
Xsettings, answer "yes" to its question, and DSNlink_NEW will submit itself
Xto your DSN$BATCH queue and will execute on the schedule specified in the
Xconfiguration file.
X
XNothing else needs to be done - DSNlink_NEW will notify users in the
X$ERROR$ group if something goes wrong, and users will be notified, per the
Xconfiguration file settings, of new database article titles that are
Xfound by DSNlink_NEW.
X
XTo have DSNlink_NEW retrieve article text for you, see the section on GET`20
Xprocessing.
X
X
X
X&4E3D
X&1A202. Configuration file
X&4E3D
X
XAll configuration file settings are prefixed with a single period (with
Xthe exception of databases).  Most settings can also be specified with a
Xlogical name.  Settings are used in this order:
X
X&08201.  Configuration file
X&08202.  Logical name
X&08203.  Hard-coded default value
X
XThe following chart lists the configuration file settings and their
Xcorresponding logical names.  Detailed information about the
Xconfiguration file settings can be found in the configuration file
Xtemplate (DSNLINK_NEW_CONFIG.DAT_TEMPLATE), or in the text below.
X
X&0820Configuration file&0620Logical&1220
X&0820&122D&0620&082D&1120
X&0820.RETRY&1220DSNLINK_NEW_RETRY&0820
X&0820.MAIL&1320(none)&1320
X&0820.SINCE&1220DSNLINK_NEW_SINCE&0820
X&0820.MAIL_EMPTY&0D20DSNLINK_NEW_MAIL_EMPTY  `20
X&0820.NEXT_EXECUTION&0920DSNLINK_NEW_AFTER&0820
X&0820(none)&1220DSNLINK_NEW_DIR&0A20
X&0820.HISTORY_FILE&0B20DSNLINK_NEW_HISTORY&0620
X&3920
X&0820.HISTORY_AUTO_COMPRESS  DSNLINK_NEW_AUTO_COMPRESS
X&3920
X&0820(none)&1220DSNLINK_NEW_CONFIG&0720
X&0820.RETRY_WAIT_TIME&0820DSNLINK_NEW_WAIT_TIME   `20
X&0820.LOG_FILE&0F20(none)&1320
X&0820.CHECK_OPENCALLS&0820DSNLINK_NEW_CHECK_OPENCALLS
X&0820.CHECK_CLOSEDCALLS&0620DSNLINK_NEW_CHECK_CLOSEDCALLS
X&0820.GET_FILE&0F20DSNLINK_NEW_GET_FILE
X&0820.GET_LOG_FILE&0B20DSNLINK_NEW_GET_LOG_FILE
X&0820.SCRATCH_DIR&0C20DSNLINK_NEW_SCRATCH_DIR
X&0820.BATCH_PRIORITY&0920DSNLINK_NEW_BATCH_PRIORITY
X&0820.WRITE_TITLES&0B20(none)
X&0820.FAO_SUBJECT&0C20(none)
X&0820.FAO_PERSONAL&0B20(none)
X&0820.FAO_GET_SUBJECT&0820(none)
X&0820.FAO_GET_PERSONAL&0720(none)
X&0820.EXCLUDE&1020(none)
X&0820
X&0820
XIf the logical DSNLINK_NEW_DIR is undefined, the directory location is
Xassumed to be the location of DSNLINK_NEW.COM. `20
X
X
X&0F20Description of configuration file settings
X
X.RETRY number
X&0820Default:  5
X&0820Number of attempts to connect to DSNlink host system.  Default is
X&08205 attempts.  DSNlink_NEW waits 30 seconds between attempts to
X&0820allow the modem and DSNlink to "settle."
X
X.MAIL mailgroup user`5B,user,...`5D
X&0820Default:  none
X&0820The first parameter is the name of the group, the second
X&0820parameter is the users belonging to the group (each username
X&0820should be separated by commas, and distribution lists ("@") are
X&0820allowed).  The name of the group can only contain characters that
X&0820are legal as a DCL symbol.  If you are using a foreign mail
X&0820protocol (MX, PMDF, or some other mailer), you will need to use a
X&0820distribution list, logical, or setup a forwarding address for a
X&0820pseudo-user within VMSmail, as the quotes necessary for a foreign
X&0820protocol aren't handled by DSNlink_NEW.  To setup a forwarding
X&0820address (requires SYSNAM privilege): `20
X
X&0A20MAIL> SET FORWARD/USER=DSNLINK_NEW_user  "MX%""user@host"""`20
X&0820example:
X&0A20MAIL> SET FORWARD/USER=DSNLINK_NEW_DAN`20
X&0A20_Address:  "MX%""dwing@uh01.colorado.edu"""
X
X&0820DSNlink_NEW does not perform any validation to ensure the mail
X&0820addresses exist or are functional.  In fact, DSNlink_NEW's mail
X&0820routine specifically disregards any errors during mail delivery
X&0820(some sites prefer to have mail delivered to all recipients that
X&0820can receive mail instead of failing delivery because one user
X&0820couldn't receive mail).
X
X&0820The special mail group $ERROR$ is used when errors are
X&0820encountered by DSNlink -- either internal errors or problems
X&0820connecting to the DSNlnk host system, accessing specific
X&0820databases, etc.  If you are using GET processing, the $ERROR$
X&0820group is always re-written to be the user running DSNlink_NEW (or`20
X&0820the user specified in the parameters when using MX SITE or
X&0820DELIVER -- see below for more information); the setting with the
X&0820configuration file is ignored (further information on GET
X&0820processing is located elsewhere in this documentation).
X
X&0820Upper- and lowercase are permitted in the mail group name and the
X&0820usernames.  No spaces are allowed between the usernames. `20
X
X&0820NOTE:  You must use the same case in subsequent references to
X&0F20mailgroup names.  That is, if you define a mail group
X&0F20"pRoGrAmMeRs", you must use "pRoGrAmMeRs" in all
X&0F20subsequent database settings:  you cannot intermix
X&0F20"pRoGrAmMeRs" or "Programmers" or "PROGRAMMERS", but you
X&0F20must use the same upper/lowercase combination you used
X&0F20when defining the mail group with the .MAIL setting.
X&0F20DSNlink_NEW will display an error message if the
X&0F20configuration file contains different upper/lowercase for
X&0F20mail group names.
X
Xdatabase `5BFLASH `7C NOFLASH`5D mailgroup`5B,mailgroup,...`5D
X&0820Default:  none
X&0820This is the only configuration setting that doesn't have a period
X&0820as the first character.  This setting is used to define a DSNlink
X&0820database you'd like DSNlink_NEW to check for new articles.  The
X&0820first parameter is the database name, second parameter is the
X&0820word FLASH or the word NOFLASH, and the third parameter is a
X&0820previously-defined mail group (see .MAIL).
X
X&0820NOTE:  Please don't abuse the FLASH setting; using FLASH causes
X&0F20the database to be checked twice (once for flash articles,`20
X&0F20another time for non-flash articles).  Some databases will
X&0F20never have flash articles, and it is best to not check
X&0F20them for flash articles (VMS-RN-NF-DOC for example).
X
X&0820DSNlink_NEW does not, and cannot, validate the database name (the
X&0820list of databases is country-specific, and changes often).
X&0820However, if a database doesn't exist when DSNlink_NEW connects to
X&0820the DSNlink host system, or the database isn't accessible to your
X&0820CSC access number, a message will be sent to the $ERROR$ group
X&0820showing the database that couldn't be opened.
X&0820
X&0820Specifying a database more than once may cause unpredictable`20
X&0820behaviour -- specifically, history functions may not work as
X&0820expected.
X&0820
X&0820Upper- and lowercase are permitted in the database name,
X&0820FLASH/NOFLASH setting, and mailgroup name.  No spaces are allowed
X&0820in the list of mailgroups.  See the note (in .MAIL, above) which
X&0820describes restrictions on upper/lowercase with mail-group names.
X
X.SINCE delta-time
X&0820Default:  TODAY-10-00:00
X&0820This specifies how far back the search should be performed.
X&0820Because there is a lag (on the DSNlink host system) between the
X&0820date in the article (searched for by /SINCE) and the date the
X&0820article is available through DSN ITS, you need to search farther
X&0820back than /SINCE=YESTERDAY.  However, searching farther back than
X&0820necessary wastes resources (on the DSNlink host system and your
X&0820system).  Ten days seems to catch all articles.  DEC's current
X&0820(April 1993) recommendation is two days (TODAY-2-00:00). `20
X&0820
X&0820You should specify a delta time (such as TODAY-7-00:00:00 for a
X&0820week ago).
X
X.MAIL_EMPTY `5BTRUE `7C FALSE`5D
X&0820Default:  FALSE
X&0820If TRUE, send Email if there are no new articles.  Some sites may
X&0820prefer this setting for its "comfort factor".  The default value
X&0820(Falose) causes no mail to be sent to users if there aren't new
X&0820articles in the databases they have been assigned to.
X
X.NEXT_EXECUTION delta-time
X&0820Default:  TOMORROW+01:00 (1:00am tomorrow morning)
X&0820Specifies when you'd like DSNlink_NEW to resubmit itself.  Use
X&0820"ONE" if you only want it to execute once and not resubmit
X&0820itself; this can be useful if you are having DECscheduler, CRON,
X&0820or some other self-submitting job execute DSNlink_NEW.
X&0820
X&0820NOTE:  Digital recommends that North American users`20
X&0F20execute DSNlink_NEW between 9pm and 3am Mountain`20
X&0F20Time to lessen the load on the DSNlink host system.
X
X.HISTORY_FILE `5Bfilename `7C NONE`5D
X&0820Default:  DSNLINK_NEW_DIR:DSNLINK_NEW_HISTORY.DAT
X&0820DSNlink_NEW history filename.  This file is used to store all
X&0820article titles that were retrieved with DSNlink_NEW.  When
X&0820DSNlink_NEW is executed, new articles are compared to the history
X&0820file to prevent mailing articles you've already seen.  However,
X&0820if an article is retrieved from DSNlink, and the last time it was
X&0820seen (according to the history file) was SINCE-2 (two days before
X&0820your SINCE date), the article will be re-displayed -- this is
X&0820based on the assumption that DEC actually changed the text of the
X&0820article. `20
X
X&0820To disable the history feature, specify NONE as the filename.
X
X&0820If debugging is enabled, records found in history are still
X&0820mailed to users, but are flagged as being found in history.
X
X.HISTORY_AUTO_COMPRESS blocks
X&0820Default:  600 blocks
X&0820This controls history file auto compression.  Auto compression is
X&0820triggered when the history file's size (in used blocks, not allocated`20
X&0820blocks) exceeds this value.  When the compression is performed, a`20
X&0820single line message is affixed to any outgoing DSNlink_NEW mail`20
X&0820messages; if no DSNlink_NEW mail message is generated, the`20
X&0820compression is still performed, but the only indication will be in`20
X&0820the log file.
X
X&0820History file auto compression can be disabled by specifying 0.
X
X&0820You can also manually compress the history file by specifying
X&0820COMPRESS as P2 when you run DSNlink_NEW, and the file will be
X&0820compressed no matter what its relationship to the value specified
X&0820with .HISTORY_AUTO_COMPRESS.  See section titled "Compressing
X&0820history."
X
X&0820The code has been written to be fairly tight (to prevent problems
X&0820if another program is accessing the history file), but it is
X&0820desirable to prevent any other processes (including DSNlink_NEW
X&0820executing in other process) from accessing the history file
X&0820during compression. `20
X
X.RETRY_WAIT_TIME delta-time
X&0820Default:  30 seconds (00:00:30)
X&0820If a connection (or connection attempt) to the DSNlink host
X&0820system fails, this value specifies the time to wait before the
X&0820next connection attempt.  If you notice your modem and/or
X&0820DECserver don't 'settle' after the default retry wait time, you
X&0820may need to increase this value at your site.  30 seconds seems
X&0820to be sufficient for most sites.
X
X.CHECK_OPENCALLS schedule `5Busername`5D
X&0820Default: Never
X&0820Specifies how often to check for open DSNlink calls.  This works
X&0820by sending a mail message to DSN%OPENCALLS -- note that only one
X&0820user can get the list of open calls from the DSNlink host system.
X&0820You can specify an integer value (which implies the day of the
X&0820month), or a weekday value, or a comma-separated list of any
X&0820combination of either.  Use the value "Never" to disable checking.
X
X&0820The schedule cannot contain spaces; some validation is performed
X&0820on the values entered in the schedule.
X
X&0820If the username is specified and it is different from the username
X&0820running DSNlink_NEW.com, CMKRNL or SETPRV privilege is required for
X&0820it to function -- this is because DSNLINK_NEW.COM uses SUBMIT/USER
X&0820to perform the submit using another username.  DSNlink_NEW will
X&0820enable CMKRNL to perform this function.  If you don't have CMKRNL
X&0820or SETPRV, DSNlink_NEW will ignore your username setting (if you
X&0820are running interactively, no warning will be displayed `5Bthis is
X&0820to allow non-privileged users, who might receive DSNlink_NEW
X&0820messages, to use the same configuration file when performing GET
X&0820processing).  Unfortunately, DSNlink_NEW can't safely assume the
X&0820user has write access to either DSNLINK_NEW_DIR or the SYS$LOGIN
X&0820of the current user, and, because OpenVMS VAX V5.5 translates
X&0820SYS$LOGIN when the SUBMIT command is executed, by default
X&0820DSNlink_NEW will not create a log file for a job that is
X&0820submitted under another username;  however, if debugging is
X&0820enabled, an *attempt* will be made to create a log file in the
X&0820directory specified by .DSN_SCRATCH in the configuration file.
X
X.CHECK_CLOSEDCALLS schedule `5Busername`5D
X&0820Default: Never
X&0820Functions exactly like .CHECK_OPENCALLS, except this sends mail
X&0820to the username DSN%CLOSEDCALLS. `20
X&0820
X&0820DSNlink in some geographic areas may not have a DSN%CLOSEDCALLS
X&0820mailing address.
X
X&0820NOTE:  Some sites can have a very large closed call list, thus
X&0F20using .CHECK_CLOSEDCALLS isn't highly recommended.
X
X.LOG_FILE file-specification
X&0820Default:  DSNLINK_NEW_DIR:DSNLINK_NEW.LOG    `20
X&0820File specification for your log file.  Normally you'd only
X&0820specify the directory, and DSNlink_NEW would use DSNLINK_NEW.LOG
X&0820as the filename.  .LOG_FILE is used for normal processing
X&0820(article title retrieval), and .GET_LOG_FILE is used for GET`20
X&0820processing (article text retrieval).
X
X.GET_FILE file-specification
X&0820Default:  SYS$LOGIN:DSNLINK_GET.DAT
X&0820Filename containing titles of articles that should be retrieved
X&0820from the DSNlink host system.  This is called "GET processing".
X&0820If this file exists when DSNlink_NEW is executed interactively,
X&0820DSNlink_NEW will ask a few questions about where to mail the
X&0820article text, and when to begin retrieving articles, and then GET
X&0820processing will be performed in batch.
X
X&0820It is recommended to use SYS$LOGIN as the location of the
X&0820GET file to prevent multiple users from interfering with each
X&0820other if they use a public area such as DSNLINK_NEW_DIR
X&0820(depending on how you've installed DSNlink_NEW, the location of
X&0820the GET file may not make any difference).
X
X.GET_LOG_FILE file-specification
X&0820Default:  SYS$LOGIN:DSNLINK_NEW_GET.LOG
X&0820Log file for GET processing batch job.  .GET_LOG_FILE is used for
X&0820GET processing (article text retrieval), and .LOG_FILE is used
X&0820for normal processing (article title retrieval).
X
X.BATCH_PRIORITY priority
X&0820Specifies the process base priority to be used when DSNlink_NEW
X&0820is running in batch.  This is useful on slower systems that need
X&0820all their CPU horsepower in the middle of the night, especially
X&0820if you've got a lot of DSNlink_NEW history data or a lot of
X&0820DSNlink_NEW groups that must be processed.  Note that during the
X&0820DSN ITS conversation (when DSNlink_NEW is communicating with the
X&0820DSNlink host), the base priority is always set the greater of the
X&0820original base priority or the value specified in BATCH_PRIORITY
X&0820-- this is to help ensure the connection to the remote DSNlink
X&0820host is completed as fast as possible.  ALTPRI is required to
X&0820set a priority higher than authorized base priority.
X
X.WRITE_TITLES file-specification
X&0820Default:  <no action - titles aren't appended to any file>
X&0820If specified, all article titles retrieved (by normal processing)
X&0820will be appended to this file.  This can be used to simplify
X&0820GET processing if you desire, but isn't commonly used.
X
X.SCRATCH_DIR directory
X&0820Default:  DSN$SCRATCH:&0A20if defined
X&1220SYS$SCRATCH:&0A20if defined
X&1220SYS$LOGIN:&0C20if defined
X&1220DSNLINK_NEW_DIR:&0620
X&0820Specifies location for DSNlink_NEW scratch files.
X&0820
X.FAO_SUBJECT "fao-controlstring"
X&0820Default:  "!+!+!UL new !AS DSNlink article!%S in !UL database!%S"
X&0820This controls the subject-line used for DSNlink_NEW mail
X&0820messages, and allows custom tailoring of the information
X&0820presented in the subject line.  The FAO control string *must* be
X&0820enclosed in quotes, and there cannot be quotation marks within
X&0820the FAO string.  No matter what you specify for the FAO string,
X&0820the following five fields need to be handled in this order:
X&0A20numeric&0820number of flash articles`20
X&0A20numeric&0820number of normal (non-flash) articles`20
X&0A20numeric&0820total number of articles (flash + normal)
X&0A20string&0920mailgroup name
X&0A20numeric&0820number of databases
X
X&0820You can invent any FAO control string that will work with the
X&0820above arguments.  Sample FAO control strings:
X&0A20a. "!+!+!0ULNew DSNlink article!%S"
X&0A20b. "!+!+!UL new DSNlink article!%S"
X&0A20c. "!+!+!+!AS - !-!-!UL new DSNlink article!%S"
X&0A20d. "!+!+!+!AS - !-!-!UL new DSNlink article!%S in !+!UL database!%S"
X&0A20e. "!+!+!+!AS - !-!-!2UL new DSNlink article!%S in !+!UL database!%S"
X&0A20f. "!+!+!UL new !AS DSNlink article!%S in !UL database!%S"
X&0A20g. "!+!+!UL new DSNlink article!%S (!AS)"
X&0A20h. "!+!+!UL hit!%S in !+!UL DSNlink database!%S"
X&0A20i. "!UL flash, !UL normal DSNlink articles in !+!+!UL database!%S"
X&0A20j. "!+!+!UL !AS articles (!-!-!-!-!UL flash, !UL normal in !+!+!UL`20
X&0E20database!%S)"
X
X&0820Output from above FAO strings:
X&0A20a.  New DSNlink articles
X&0A20b.  5 new DSNlink articles
X&0A20c.  Programmer - 3 new DSNlink articles
X&0A20d.  Programmer - 7 new DSNlink articles in 3 databases
X&0A20e.  Programmer -  7 new DSNlink articles in 4 databases
X&0A20f.  9 new Programmer DSNlink articles in 2 databases
X&0A20g.  6 new DSNlink articles (Programmer)
X&0A20h.  7 hits in 5 DSNlink databases
X&0A20i.  3 flash, 8 normal DSNlink articles in 4 databases
X&0A20j.  23 Programmer articles (2 flash, 21 normal in 7 databases)
X
X&0820Sample f. is the default FAO control string.  Note that this FAO
X&0820string only controls normal DSNlink_NEW processing -- it doesn't
X&0820affect GET processing, error handling, or exception processing.
X&0820Enabling debugging will display sample output.  Note that`20
X&0820the fao control string cannot wrap to another line, and it is only
X&0820wrapped in this documentation to fit on the page.
X
X.FAO_PERSONAL "fao-controlstring"
X&0820Default:  "!+!+DSNlink article!0UL!%S, !AS"
X&0820Has the same arguments as .FAO_SUBJECT.  This setting controls
X&0820the VMSmail personal name of normal DSNlink_NEW mail messages.
X&0820Enabling debugging will display sample output.  The FAO control
X&0820string must be enclosed in quotes.
X&0820
X&0820NOTE:  The VMSmail personal name cannot begin with a number, and
X&0F20certain other combinations of strings are invalid (such as
X&0F20multiple spaces).
X&0820
X.FAO_GET_SUBJECT "fao-controlstring"
X&0820Default:  "!AS: !AS"
X&0820Functions similar to .FAO_SUBJECT, except .FAO_GET_SUBJECT
X&0820is used during GET processing, when article text is mailed to
X&0820a user.  There are always exactly two arguments:
X&0A20string&0820database name
X&0A20string&0820title
X
X&0820sample FAO control strings:
X&0A201.  "!AS database, !AS"
X&0A202.  "!+!AS"
X&0A203.  "(!AS) !AS"
X&1420
X&0820sample output from above FAO strings:
X&0A201.  VMS database, FASTLANE for OA - A DSIN/DSNlink Database Tool
X&0A202.  FASTLANE for OA - A DSIN/DSNlink Database Search Tool
X&0A203.  (VMS) FASTLANE for OA - A DSIN/DSNlink Database Search Tool
X&0A20
X&0820If the total length of the subject line exceeds 71 characters, it
X&0820is truncated at 71 characters and a tilde (`7E) is placed on the 71st
X&0820character; this prevents line wrapping caused by a long VMSmail
X&0820subject line.  Note that .FAO_GET_PERSONAL is not handled in this
X&0820manner (no truncation is performed).  Enabling debugging will
X&0820display sample output.  The FAO control string must be enclosed in
X&0820quotes.  You can acheive simple truncation by using "!71<...>" as
X&0820the FAO control string, where "..." is the contents of the control
X&0820string.
X&1020
X.FAO_GET_PERSONAL "fao-controlstring"
X&0820Default:  "!AS database article"
X&0820Takes the same arguments as .FAO_GET_SUBJECT, but controls the
X&0820VMSmail personal name of GET processing mail messages.  The
X&0820FAO control string must be enclosed in quotes.
X&0820
X&0820NOTE:  The VMSmail personal name cannot begin with a number, and
X&0F20certain other combinations of strings are invalid (such as
X&0F20multiple spaces).  This is a restriction of VMSmail.
X
X.EXCLUDE database-name article-title
X&0820Never display <article-title> in DSNlink_NEW mail messages.
X&0820This is used no matter if History is enabled or disabled.  The
X&0820database-name can be "*" (without quotes) to indicate all
X&0820databases.  Quotes are allowed (as some article titles contain
X&0820quotes), but quotes will earn a warning message.  To suppress the
X&0820warning message, use .EXCLUDE_WARNING FALSE (below). `20
X
X.EXCLUDE_WARNING TRUE `7C FALSE
X&0820Default:  FALSE
X&0820Set to TRUE to disable the warning if quotes are used in the
X&0820article title in an .EXCLUDE directive.
X
X
XThe following settings can be specified *ONLY* as logical names:
X&403D
X
XDSNLINK_NEW_VERIFY
X&0820Default:  FALSE
X&0820If the evaluates to a TRUE value, then DCL verification is turned on.
X
XDSNLINK_NEW_DEBUG    `20
X&0820Default:  FALSE
X&0820If TRUE, debugging is enabled.  This displays lots of extra output
X&0820useful when debugging DSNlink_NEW.  This also changes the actual
X&0820operation of the history function by displaying "Hist:yyyy-mm-dd"
X&0820in front of any articles that were retrieved from the DSN ITS
X&0820query, and were already in DSNlink_NEW's history database.  There
X&0820are also a few other differences in behavior that happen when
X&0820debugging is enabled.
X
X
X
X
X&4E3D
X&1C203. GET Processing
X&4E3D
X
X
XDSNlink_NEW's ability to get article text (instead of just titles) is
Xtermed "Get Processing" for lack of a better name.  This feature was
Xintroduced with version 5.0 of DSNlink_NEW.  Thanks to Joel "M-for-VNEWS"
XSnyder, jms@Opus1.COM, for the original concept.
X
XTo utilize this feature:
X
X  1. Extract a normal mail message from DSNlink_NEW (containing database
X     names and article titles) to a file called SYS$LOGIN:DSNLINK_GET.DAT
X  2. Edit the file SYS$LOGIN:DSNLINK_GET.DAT, using any editor, so only
X     the "interesting" article titles and database names are in the file
X     (see below for examples).
X  3. Execute DSNlink_NEW.COM:
X&0720$ @DSNLINK_NEW
X     (DSNlink_NEW will see the file SYS$LOGIN:DSNLINK_GET.DAT and will
X&0620automatically initiate Get Processing.  You can change the name`20
X&0620of the file it looks for with .GET_FILE in your configuration
X&0620file).
X  4. Answer the prompts.
X  5. DSNlink_NEW will submit a batch job that will retreive the articles. `20
X
X  Notes on formatting the GET file (or mail message if you're using
X  automatic GET processing (see another section for details)):
X`20
X    o  These examples are set to spaces from the left margin for clarity;
X&0720the actual text should be directly on the left margin, just as you
X&0720receive the message.
X&0820
X    o  If you choose, you can leave the VMSmail "From:", "To:", "CC:", and
X&0720"Subj:" headers in the file -- they will be ignored.
X&0820
X    o  If you choose, you can leave the DSNlink_NEW "trailer" (the stuff
X&0720after the line of five dashes) in the file -- it will be ignored.
X
XFor example, SYS$LOGIN:DSNLINK_GET.DAT could be edited to contain:
X
X(example #1):
X `20
X  VMS database, 4 articles:
X    `5BOpenVMS`5D AUTOGEN Returns %AUTOGEN-W-SVAEXCEED Error And Refers To BAL
VSETCNT
X  LAT-DECserver database, 1 article:
X    *DECserver 250`5D CSCPAT_0521: DS250 V2.0 BL2905 ECO Summary
X
X(example #2:)
X
X  ECO-Summary database, 6 articles:
X    *OpenVMS`5D CSCPAT_0180 RMS (V5.2-1-V5.5-2) ECO Summary
X  VMS database, 43 articles:
X    `5BOpenVMS`5D R5 Value Is Ignored In The Console Command ">>> B/R5:n"
X    *OpenVMS`5D CSCPAT_0180 RMS (V5.2-1-V5.5-2) ECO Summary
X    `5BOpenVMS`5D V6.0 AUTOGEN Miscalculates LOCKDIRWT and SHADOW_MAX_COPY
X    `5BOpenVMS`5D Escape Sequences Ignored By Printer on Terminal Queue
X    `5BOpenVMS`5D MOUNT Returns NOOBJSRV Error If AUDIT_SERVER Is Not Running
V
X    `5BOpenVMS`5D Job Controller Using Unmodified LATSYM Logs %QMAN-I-INVSMBMS
VG
X    `5BOpenVMS`5D List Of VMS RTL And System Service Routines, Updated For V6.
V0
X  LAT-DECserver database, 3 articles:
X    `5BDECserver`5D RS-232-C Line Signals Supported by DECserver 200 V1.0
X  Shadow database, 8 articles:
X    `5BSHADOW`5D Phase II Shadow Copy Runs Extremely Slow on RF36 Disk Drives
V
X  DECnet-VMS database, 1 article:
X    `5BDECnet/OSI`5D OpenVMS VAX: X.25 `26 X.29 File Transfers Methods
X  DECW-Motif database, 1 article:
X    `5BMotif`5D Print From DECterm Hangs Or Prints Garbage
X  Cobol database, 1 article:
X    `5BOpenVMS`5D List Of VMS RTL And System Service Routines, Updated For V6.
V0
X
X
X
XNOTES ON GET PROCESSING:
X
Xo  Don't use messages from versions of DSNlink_NEW prior to V5.0.
X     Don't extract a mail message from a version of DSNLINK_NEW prior to
X     DSNlink_NEW version V5.0 and expect DSNlink_NEW's GET Processing to
X     work.  Versions of DSNlink_NEW prior to V5.0 use a different
X     database name format.  (Well, you CAN use an old mail message, but
X     you must edit it to match the format of the new mail message from
X     DSNLINK_NEW.)
X&1420
Xo  Database count isn't used:
X     The database count in the DSNLINK_GET.DAT file ("4 articles", "14
X     articles") does *not* need to be accurate, as the value isn't used
X     when DSNlink_NEW parses the GET file.  The GET processing code looks
X     for the strings "database," and "article:" on the same line to
X     indicate that a new database should be used.
X
Xo  $ERROR$ group is modified:
X     When performing GET processing, the $ERROR$ group (defined in the
X     configuration file) is modified so it contains only the user running
X     DSNlink_NEW -- this is to prevent the system manager or whoever is
X     "managing" DSNlink_NEW from being bothered by errors generated when`20
X     someone else is using DSNlink_NEW's GET processing facility.
X    `20
Xo  Pseudo-mail groups "Retain" and "Self":
X     When using GET processing, a list of mail groups is displayed.  The
X     $ERROR$ group is never displayed in this list, and two additional
X     groups are always shown:  "Self" and "Retain".  The "Self" group is
X     used to mail the articles to yourself, and the "Retain" group is used
X     to mail the articles to all users that would normally receive the
X     article titles per the configuration file.
X
X     For example, if your configuration file contains:
X    `20
X&0720.MAIL Programmers DR_DOBBS,NWIRTH
X&0720.MAIL Analysts    CSTOLL
X&0720.MAIL Systems     SYSTEM
X&0720VMS     NOFLASH  Programmers,Systems
X&0720RdbVMS  NOFLASH  Analysts
X&0720
X     and you selected group "Retain", then Programmers and Systems
X     (DR_DOBBS, NWIRTH, and SYSTEM) would receive articles retrieved from
X     the VMS database, and Analysts (CSTOLL) would receive any articles
X     retrieved from the RdbVMS database.
X
Xo  Retrieving incorrect article:
X     Because DSNlink's Interactive Text Search (ITS) lacks a method to
X     retrieve a specific article title, there is no guarantee that
X     DSNlink_NEW will retrieve the correct article title.  DSNlink_NEW
X     attempts to warn you when it detects that it retrieved the incorrect
X     article title by displaying information similar to:
X&0A20
X&0720Title:&0A20`5BOpenVMS,Motif`5D Motif V1.1 Doesn't Start After OpenV
X&0720Desired title:  `5BOpenVMS,Motif`5D "RMS-E-DNR, Device Not Ready" After
X&0720Warning:&0820Titles do not match
X&0720Warning:&0820Article may need to be retrieved from DSNlink manually
X
X     DSNlink_NEW also makes the (sometimes incorrect) assumption that the
X     desired article title will always be article number 1 after the DSN
X     ITS "SEARCH" command is issued.  While this assumption is usually
X     correct, the desired article is not article number 1 about 5% of the
X     time.  In these cases, manual retrieval is necessary.  For example:
X   `20
X&0820ITS> close/all
X&0820ITS> open lat-decserver
X&0820ITS> search/boolean "`5BLATmaster`5D Common Questions About LATmaster`20
X&0820Software"
X&08204 matching documents found
X&0820ITS> dir
X&0A201.  *LAT/VMS`5D CSCPAT #511 V3.3 Patch Kit Release Notes
X&0A202.  `5BLATmaster`5D NOTFCBFCB Bugchecks and Other Pool Memory Corruption
V
X&0A203.  `5BLATmaster`5D Common Questions About LATmaster Software
X&0A204.  `5BLATmaster`5D Optional Software Available with VMS V5.4-1
X
X&0820Note the desired article is article number 3, instead of article`20
X&0820number 1.
X
X&0820(I tried various forms of SEARCH, with and without quotes, with and`20
X&0820without the /BOOLEAN, with the same results). `20
X
X     As this problem happens relatively infrequently, and the only way to
X     prevent this problem would be _two_ connections to the remote DSN
X     host system, no attempt is made to retreive any article other than
X     article number 1.
X
Xo  Retrieval Method (for DSNlink V1.2 and above):
X     If you are running DSNlink V1.2 or higher, DSNlink_NEW uses a
X     technique which takes advantage of line-mode EDT so the file is only
X     transferred over the modem one time.  This technique uses the
X     DSN$ITS_EDIT logical, and requires DSNlink V1.2 or higher
X     (unfortunately, a bug in DSNlink V1.1 and V1.1-1 doesn't properly
X     implement this feature - see Alternate Retrieval Method, below).
X     This technique uses DSN ITS commands like:
X    `20
X&0720OPEN database&1920! DSN ITS command
X&0720SEARCH/ENGLIGH article-title&0A20! DSN ITS command
X&0720READ/EDIT 1&1B20! DSN ITS command
X&0720EXIT temporary-filename&0F20! EDT command`20
X&0720...&2320! DSN ITS commands ...
X
X     to retrieve the articles.
X
X     This retrieval method is indicated by the message:
X&0820%DSNlink_NEW-I-GET, Get style: EDT
X     which is displayed in the DSNLINK_NEW_GET.LOG file when DSNlink_NEW is`20
X     executing in batch during GET Processing.
X
Xo  Alternate Retrieval Method (for older versions of DSNlink)
X     Due to a bug in DSNlink V1.1 and V1.1-1, these versions of DSNlink
X     do not work with EDT, and hence don't work with the above method of
X     article retrieval (see the article titled "`5BDSNlink/VMS`5D V1.1-n ITS
X     Edit Fails with EDT-F-OPENIN, RMS-E-FLK" in the DSNLINK database for
X     a full description of this problem).  DSNlink_NEW V5.1-1 and above
X     will automatically determine the version of DSNlink that is being
X     used (using the undocumented DSN$VERSION symbol which is created
X     after performing a "$ DSN SHOW VERSION" command), and, if the
X     version of DSNlink is prior to V1.2, a different mechanism to
X     retrieve articles is used:
X    `20
X&0720OPEN database
X&0720SEARCH/ENGLISH article-title
X&0720READ 1
X&0720EXTRACT temporary-filename
X&0720...
X&0720
X     Note that this method causes the article text to be transmitted over
X     the DSNlink modem two times -- once for the "READ 1" command, and
X     another time for the "EXTRACT temporary-filename" command. `20
X
X     This retrieval method is indicated by the message:
X&0820%DSNlink_NEW-I-GET, Get style: EXTRACT
X     which is displayed in the DSNLINK_NEW_GET.LOG file when DSNlink_NEW is`20
X     executing in batch.
X&0A20
X     The preferred solution to this problem is to upgrade to DSNlink V1.2
X     at your earliest convenience, as the built-in workaround increases the
X     connect time to the DSNlink host system.
X
X
X&4E3D
X&13203a. Automating GET Processing
X&4E3D
X
XIf you have MX, PMDF, or DELIVER installed on your system, you can automate
Xthe GET Processing so you never need to exit Mail to cause DSNlink articles
Xto be automatically retrieved.  When you are reading a message sent by
XDSNlink_NEW, you only need to type FORWARD/EDIT, edit your message to
Xremove uninteresting titles (you need to leave the "xxx database, nnn
Xtitles:" line above the titles you want -- see examples above), and exit
Xyour editor -- DSNlink_NEW will automatically be activated and will
Xretrieve articles on your behalf.
X
XNote that setting up the MX SITE agent or standalone DELIVER will require
Xprivileges.  However, you will not need special VMS privileges to use it
Xafter it has been setup.
X
X
Xo  Using MX's SITE agent:
X&0820(If you have PMDF or DELIVER already installed on your
X&0920system, please read the section below on using DELIVER.)
X    `20
X     A sample MX_SITE_DELIVER.COM_TEMPLATE is included with the
X     DSNlink_NEW distribution, and this can be used as a starting point.
X     First, ensure that you installed the SITE agent when you installed
X     MX (you may need to re-install MX if the site agent isn't
X     installed).
X
X     After the SITE agent is installed, you need to add a SITE path.  The
X     MCP command would be something like:
X    `20
X&0720DEFINE PATH DSNLINK_NEW_GET SITE
X&0720
X     this path should be before any non-local paths; specifying it as your
X     first path works well.  You should modify your MX configuration file
X     and then RESET all agents.  Basically, the SITE agent is activated
X     whenever an outgoing mail address has a "host" portion which matches
X     the path DSNLINK_NEW_GET (in the above example #1 and example #2).
X
X     After this point, any mail sent through MX that has a host-part of
X     DSNLINK_NEW_GET will get routed to the SITE agent.  To make using
X     this easier (to reduce typing), you can setup mail forwarding using:
X    `20
X&0720MAIL> SET FORWARD/USER=GET MX%"""USER@DSNLINK_NEW_GET"""
X
X     or, if you prefer, setup an alias within MX (MCP> DEFINE ALIAS) --
X     anything that causes the message to be processed by MX and routed
X     through to the SITE agent will work.
X
X     The file MX_SITE_DELIVER.COM_TEMPLATE should be copied to MX_EXE:
X     and named SITE_DELIVER.COM.  This file shouldn't require too much
X     modification to work at your site other than changing the list of
X     authorized users.  The file includes documentation showing where the
X     changes need to be made.
X
X     After you've got all of this setup, when you receive a mail message
X     sent by DSNlink_NEW, you only need to FORWARD/EDIT the message to
X     the username GET, put anything in the subject line (or leave it
X     blank), edit the file to remove uninteresting database titles, and
X     exit your editor.  You will receive a REPLY message (from
X     SITE_DELIVER.COM) when the batch job is submitted.  All articles and
X     errors are mailed to your account.  Note that the batch job isn't`20
X     owned by your username, but by the user specified in the
X     SITE_DELIVER.COM file with the /USER=username qualifier (which is
X     DSN$SERVER with the included template file).
X    `20
X     If errors are mailed to your account, you'll have to check the log
X     file (the log file name is indicated at the end of the message sent
X     by DSNlink_NEW -- non privileged users may not have access to that
X     area, depending on how it was setup in SITE_DELIVER.COM -- the
X     template SITE_DELIVER places the log file into SYS$SCRATCH).  If you
X     get a bounce from Postmaster, you (or your system manager) probably
X     need to enable MX SITE debugging to diagnose the problem; see the MX
X     manuals for information on enabling MX debugging.
X
Xo  Using DELIVER (part of PMDF, and also available standalone)
X     DELIVER allows you to execute any arbitrary .COM file when you
X     receive an Email message.  By passing the correct values to
X     DSNLINK_NEW.COM, you can use DELIVER in conjunction with
X     DSNlink_NEW. `20
X    `20
X     PMDF_DELIVER.COM_TEMPLATE provides sufficient pieces to get GET
X     Processing working with PMDF's DELIVER.  Simply follow the
X     directions in the file for editing your MAIL.DELIVERY file.  If you
X     enhance DELIVER.COM, please let me know so that I may include the
X     enhancements in a future version of DSNlink_NEW.
X    `20
XNotes:
X `20
X  o  If your mail user agent automatically prefixes lines with a ">" or
X     "#" character when you forward mail for GET processing, these`20
X     characters in column 1 will be automatically ignored by DSNlink_NEW's`20
X     GET Processing code.
X
X    `20
X&4E3D
X&13204. DSNlink_NEW's handling of SINCE
X&4E3D
X
XDue to DSNlink's implementation of DIR/SINCE, DSNlink_NEW will, by default,
Xattempt to retrieve articles that are up to ten days old.  Unfortunately,
X/SINCE=YESTERDAY doesn't work as expected, and using /SINCE=TODAY-2 (the
Xday before yesterday) causes some new DSNlink articles to be missed.
X
XThe following articles in the "DSNLINK" database help describe how /SINCE
Xfunctions.  DSNlink_NEW is written to attempt to get around these side
Xeffects of the DSNlink host and client software.
X
X  `5BDSNlink/VMS`5D FLASH Articles Not Included In LIST/SINCE Title List
X  `5BDSNlink/VMS`5D ITS /SINCE Now Produces Shorter Article Title Lists `20
X
XTo prevent sending the same article titles over and over, DSNlink_NEW
Xuses a history datafile, which contains all article titles that have been
Xretrieved by DSNlink_NEW.  However, to ensure that DSNlink_NEW displays
Xarticles that have actually been modified by DEC, DSNlink_NEW will
Xre-send an article if the last time it appeared was two days prior to
Xyour SINCE time.
X
XThe history function is basically a set-it-and-forget-it feature:
XDSNlink_NEW will automatically compress the history file (purging old
Xrecords and performing some simple-minded tuning) after it reaches a
Xcertain size (600 blocks by default).
X
X
X&4E3D
X&15205. Compressing history manually
X&4E3D
X
XYou can manually compress the history file, which will eliminate old
Xrecords and perform optimization, by entering COMPRESS as P2 when you
Xexecute DSNlink_NEW.COM:
X
X&0620$ @DSNLINK_NEW_DIR:DSNLINK_NEW configuration-filename COMPRESS
X  or  $ @DSNLINK_NEW_DIR:DSNLINK_NEW "" COMPRESS
X
XThis goes through a small routine which uses EDIT/FDL to automatically
Xdetermine a better size for your history file, and uses MERGE to
Xeliminate old history records from the file (those older than two days
Xprior to your SINCE date).
X
XThis compression is also automatically done during normal processing when
Xthe history file grows larger than the value specified by
X.HISTORY_AUTO_COMPRESS in your configuration file (see section 4, above).
X
X
X
X&4E3D
X&18206. DSNlink_NEW wish list
X&4E3D
X
XAlways get full article text, each night:
X     Due to the excessive load this would place on the DSNlink host
X     system, and the likelihood that large volume of retrieved text
X     wouldn't be read, this will not be implemented in DSNlink_NEW.  This
X     functionality would be similar to printing EVERY message that comes
X     across INFO-VAX/comp.os.vms.
X
X     The code that currently exists in DSNlink_NEW allows you to edit the
X     list of article titles down to the "interesting" titles, and have
X     DSNlink_NEW retrieve the text of these articles for you.
X
X
XInform users of new DSNlink databases:
X     DSNlink_NEW currently informs the $ERROR$ group if a database (which
X     is in DSNlink_NEW's list of databases to search) is missing.  The
X     desired enhancement would add functionality so DSNlink_NEW would
X     inform users if a database was added or removed.  Implementation
X     would probably be by sending output of SHOW DATABASES to a file and
X     performing a DIFFERENCES between each execution of DSNlink_NEW.
X     There is no expected timeframe for implementing this feature (and so
X     far, only one request for this feature).
X
X     Update:  As of DSNlink V1.2, SHOW DATABASES/OUTPUT isn't a command;`20
X&0E20however, it might be possible to do a SHOW DATABASES and
X&0E20then parse the information from DSN ITS's SYS$OUTPUT file.
X&1120
X
XCheck VTX patch 9999 for new patch:
X     Until/if DEC gets the PATCHES database going, could DSNlink_NEW have
X     an option to get the description of CSCPATCH_9999 out of the VTX
X     function and mail new patch descriptions? There seems to be a pretty
X     consistent format, w/2 lines per patch, which should fit nicely into
X     the existing DSNlink_NEW history, etc. `20
X
X     Update:  This could be implemented using DSNlink V1.2's INVOKE_FUNCTION`20
V
X&0E20capability, and would base most of its operation on DEC not
X&0E20modifying the prompts sent by the DSNlink host system in
X&0E20response to INVOKE_FUNCTION.
X&1020
X&0E20DEC has also gently hinted that DSN VTX would disappear at
X&0E20some future date.
X
X&0E20DEC has created a PATCHES database (ECO-SUMMARY).
X
XNotify of changes to DSNlink mail addresses:
X     When mail is sent to DSN%HELP, the reply (from the DSNlink host
X     system) contains instructions for using DSNlink mail and a list of
X     DSNlink (DSN%) addresses that are valid for your access number.
X
X     Because of the difficulty in intercepting this information (as it
X     is only obtainable via mail), it would be very difficult to
X     implement an automatic check to determine if the list of valid
X     addresses has changed.
X
X
X&4E3D
X&0D207. Old, boring DSNlink_NEW modification history
X&4E3D
X
XNewer history information is in the DSNlink_NEW.COM file itself.
X
X  4.1-8   17-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     Use symbol DSNLINK_DIRECTORY for location of DSNLINK_NEW default`20
X     directory.  Change handling of errors so we always retry if there
X     were any errors during DSNlink connection.  Update documentation and
X     add comments to code.
X  4.1-6  15-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     Corrected bugs when history was disabled. `20
X  4.1-5  12-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     Disregard upper-/lower-case in config file where appropriate.
X  4.1-4  10-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     All messages now use %DSNlink_NEW facility code, other cleanups.  Set
X     DSNLINK_DIRECTORY to location of DSNLINK_NEW.COM
X  4.1-3   8-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     Show flash/normal in HISTSUMMARY.  Support for DSNLINK_NEW_DIR logical
X     and for .LOG_FILE setting in configuration file.  Corrected`20
X     misspellings in documentation. `20
X  4.1-2   5-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     More information in .LOG file about 'accepted' articles, removed "r "
X     as title prefix in mail (was used for previously-retrieved articles)
X  4.1-1   1-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     Fixed missing ";" within compress routine.
X  4.1     1-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     History COMPRESS now deletes old history records older than two
X     days before SINCE date.
X
X  4.0-10  30-MAR-1993  Dan Wing, dwing@uh01.colorado.edu
X     Configuration file for all configuration information, which eliminates
X     all prompting.  Support for mailing to groups of users based on`20
X     interest in certain ITS databases.  Fix bug with error handling if`20
X     ITS session is interrupted before it completes.
X
X  3.0     20-JAN-1993  Dan Wing, dwing@uh01.colorado.edu
X     Added history function to eliminate topics we've already seen.  This
X     allows using /SINCE=TODAY-5 as the default, which will let fewer
X     articles 'slip through'.  Also added COMPRESS for use with the new
X     history function.  Search for FLASH articles, and put them at the top
X     of the mail message.  (This version was never 'released.')
X
X  2.22b   19-JAN-1993  Dan Wing, dwing@uh01.colorado.edu
X     Change default to /SINCE=TODAY-3.  Some articles aren't getting
X     selected using /SINCE=TODAY-2.  (DEC still recommends TODAY-2).
X     Also specify today's date on this system, as it may be different
X     than "today" on the DSNlink host system.
X
X  2.22     8-JAN-1993  Dan Wing, dwing@uh01.colorado.edu
X     Fix bug in mail suppress code.  Use different scheme for extracting
X     article titles to prevent problems with P.S.I. and WPS-Plus/DOS
X     databases.  Don't exceed WIDTH when displaying database names (was
X     causing %RMS-F-RSZ errors).  Inform user of 'Unknown Database'
X     errors (for when the CSC drops one of your DSNlink databases).
X
X  2.20   25-NOV-1992  Dan Wing
X   Store database names in separate symbols (to avoid problems when list of`20
X   databases exceeds 255 characters).  Allow list of databases to be in a`20
X   file, and removed ability to add to a list of data-bases using "+". `20
X   Don't allow output of DSNlink DIR command to exceed 80 characters (to`20
X   avoid wrapping in Mail).  Changed logicals to the form DSNLINK_NEW_xxx`20
X   instead of DSNlinkNEW_xxx.
X `20
X  2.10   24-NOV-1992  Dan Wing
X   Remove all checking to see if DSNlink is running on the DCN - too much`20
X   code, and too many scenerios. `20
X
X  2.02   23-NOV-1992  Dan Wing
X   Include converted time in mail message for SINCE.  Default to`20
X   /SINCE=TODAY-2.  See DSNlink article "`5BDSNlink/VMS`5D ITS /SINCE Now`20
X   Produces Shorter Article Title Lists" in the DSNLINK database.  Use TRUE`20
X   and FALSE for clarity.
X `20
X  2.01   18-NOV-1992  Dan Wing
X   Use F$SEARCH(file,number) when using wildcards, and delete all of the`20
X   little extracted directory files as part of cleanup.
X `20
X  2.00b  17-NOV-1992  Dan Wing
X   Handle auto-resubmit time <> default.  Quote list of usernames which`20
X   allows "@" in distribution lists.
X  2.00   13-NOV-1992  Dan Wing
X    List articles broken out by database name.  Support for DSNlink V1.1-2`20
X    (EDSNlink) which uses the logical DSN$EADDR_DIGITAL to indicate the`20
X    connection type (M=modem, D=DECnet, X=X.25).  Add DEBUG logical and`20
X    some debugging output.
X
X  13-NOV-92  136a  DGW  Look for DSN$SCRATCH logical instead of`20
X&1820DSN$COPY_DIRECTORY (only DCN has the second logical)
X&1820Also fix syntax error on error-message creation.
X   6-NOV-92  136   DGW  Provide more information when running as to`20
X&1820success/failure.
X   4-NOV-92  135   DGW  Correct automatic-resubmit code to resubmit after
X&1820the first time.
X   3-NOV-92  134   DGW  Send Email if DSNlink isn't available when the batch
X&1820job runs.
X  31-OCT-92  133   DGW  Allow DETACHED execution (for DECscheduler)
X   6-OCT-92  132   DGW  Only prompt user if necessary.
X   5-OCT-92  131   DGW  Allow auto-submit if logical DSNlinkNEW_AFTER is
X&1820defined, or if user specifies a time when prompted.
X   1-OCT-92  1302b DGW  Invalid syntax on SUBMIT corrected.  Handle case`20
X&1820where DAN isn't clustered with the DCN.
X  26-SEP-92  130   DGW  Fixed to operate correctly if we're running on a DAN
X&1820instead of the DCN, and still sense if DSNlink is`20
X&1820operating on the DCN.  Handle DSNlink error 'unknown
X&1820database' as a special case.  Logical names added.
X&1820Don't Email if there's no new articles.  (From Andy
X&1820Harper, <udaa055@elm.cc.kcl.ac.uk>).  Also cosmetic`20
X&1820changes, error handling, parameter validation, and
X&1820documentation.
X  14-SEP-92  120   DGW  Break list of databases at 80 columns (symbol WIDTH)
X&1820Some cosmetic changes.
X   6-JUL-92  111   DGW  Only need edit 110 if INTERACTIVE.
X  30-JUN-92  110   DGW  Added SET SCREEN/noPAGE, as DIR would loop forever
X&1820if more than a screenful of subjects was displayed
X&1820(from Rand Hall, <rand@merrimack.edu>)
X   2-FEB-92  006   DGW  Use RESTART_VALUE to indicate how many`20
X&1820retries have been done.
X  27-JAN-92  005   DGW  If interactive, prompt for all parameters, and`20
X&1820allow user to submit to batch.
X  17-JAN-92  004   DGW  Retry limit = 5
X   5-JAN-92  003   DGW  The "NO" after the "DSN ITS" must be there in case`20
X&1820DSNlink enters its question-mode.  DSNlink was`20
X&1820returning "Your response is ambiguous. Please`20
X&1820re-enter" if the "NO" was in the DSN$ITS_INIT file.
X   4-JAN-92  002   DGW  Add check for "%DSN-E-" errors as well as "%DSN-F-"`20
X&1820errors to decide to retry.
X  31-DEC-91  001   DGW  Created.
$ call unpack DSNLINK_NEW.DOC;11 1531716020 "" 109 2 6
$!
$ create 'f'
X!! DSNLINK_NEW_CONFIG.DAT_TEMPLATE
X!!   DSNlink_NEW version V5.3
X!
X!+++
X!+
X!+   This file should be modified as necessary, and placed in
X!+&0720DSNLINK_NEW_DIR:DSNLINK_NEW_CONFIG.DAT
X!+&0920(DSNLINK_NEW_DIR defaults to the location of DSNLINK_NEW.COM
X!+&0A20unless changed by the logical DSNLINK_NEW_DIR)
X!+
X!+++
X!
X! One or more spaces separate elements within each line.  Line-continuation`20
X! is not allowed.  Anything after ! on a line is ignored as a comment.
X!
X! DSNlink_NEW assumes that any record in the configuration file that`20
X! doesn't start with a period is a database name.
X!
X!!!
X!
X! Configuration file settings: `20
X!    (see DSNLINK_NEW.DOC for detailed descriptions)
X!
X!    .RETRY count
X!&0920Maximum number of retries. This controls how many times DSNlink_NEW
X!&0920will try to connect to the DSNlink host system.  Default is 5, which
X!&0920should be sufficient for any site (if we haven't connected after
X!&09205 times, we probably aren't going to ever connect!).
X!
X!    .RETRY_WAIT_TIME time  `20
X!&0920Time to wait between retry attempts to the DSNlink host system. `20
X!&0920Default is 00:00:30, which is 30 seconds.
X!
X!    .SINCE delta-time&0720
X!&0920/SINCE value, which Specifies how far back to check DSNlink`20
X!&0920databases for new articles.  DEC recommends TODAY-2, but this seems`20
X!&0920to cause a few articles to slip through; the HISTORY function will`20
X!&0920prevent you from seeing the same article again if you search farther
X!&0920back, so there is little harm in being a little more generous in the
X!&0920time.  Default is TODAY-10-00:00, which is 10 days ago.
X!
X!    .MAIL_EMPTY `5BTRUE `7C FALSE`5D
X!&0920Send mail even if no new articles were selected to be mailed. `20
X!&0920Default is NO, don't sent messages indicating no articles were
X!&0920retrieved.
X!
X!    .NEXT_EXECUTION delta-time   `20
X!&0920next scheduled execution (in delta time) if you only want one run,`20
X!&0920specify ONE (this is useful if something else schedules and runs
X!&0920DSNlink_NEW, such as DECscheduler, CRON, or some other nightly-
X!&0920batch processing you perform on your system.) `20
X!&0920(default=TOMORROW+01:00:00, which is 1:00am tomorrow morning).
X!
X!&0E20Digital recommends that North American customers run`20
X!&0E20DSNlink_NEW between 9pm and 3am Mountain Time.
X!
X!    .HISTORY_FILE file
X!&0920filename of history file.  Specify NONE to disable history. `20
X!&0920Defaults to DSNLINK_NEW_DIR:DSNLINK_NEW_HISTORY.DAT
X!&0920(DSNLINK_NEW_DIR defaults to location of DSNLINK_NEW.COM)
X!
X!    .HISTORY_AUTO_COMPRESS blocks
X!&0920Sets size of history file to trigger history file auto-compression.
X!&0920Default is 600.  Value of 0 disables auto-compression.  You can`20
X!&0920manually run compression with COMPRESS as P2.
X!
X!    .MAIL mailgroupname username`5B,username,...`5D
X!&0920setup a mail group.  "name" is an arbitrary name assigned to the`20
X!&0920group, which must be a legal DCL symbol name.  "users" is the list
X!&0920of users to associate with the mail group.  This can be a comma-
X!&0920separated list, or you can use an "@" to use a VMSmail`20
X!&0920distribution list.  No default.  See DSNLINK_NEW.DOC for more`20
X!&0920information, including restrictions on use of upper/lower case.
X!
X!    database `5BFLASH `7C NOFLASH`5D mailgroupname`5B,mailgroupname,...`5D
X!&0920Database name, followed by "FLASH" or "NOFLASH" to indicate if you`20
X!&0920want FLASH articles searched, and a comma-separated list of mail-
X!&0920group names, which are previously-defined .MAIL group names.  No`20
X!&0920default.  Specifying a database more than once may cause`20
X!&0920unpredictable results.
X!
X!    .LOG_FILE file&0A20
X!&0920location of batch .LOG file.  Defaults to`20
X!&0920DSNLINK_NEW_DIR:DSNLINK_NEW.LOG.  (DSNLINK_NEW_DIR defaults to
X!&0920location of DSNLINK_NEW.COM)
X!
X!    .CHECK_OPENCALLS schedule `5Busername`5D
X!&0920Check DSNlink open calls using schedule.  This can be a day of the`20
X!&0920week (Monday, Tuesday, etc.), the keyword ALL, or a day of the`20
X!&0920month (1, 2, etc.) to check on the first day of each month.  Use
X!&0920Never to disable.  Default is Never.  If "username" is specified,`20
X!&0920and it is different than your username, you must have CMKRNL; if`20
X!&0920you don't have CMKRNL your username will be used.
X!   `20
X!    .CHECK_CLOSEDCALLS schedule `5Busername`5D
X!&0920Same as CHECK_OPENCALLS, except checks closed DSNlink calls.
X!
X!    .CHECK_DATABASES schedule `5Busername`5D
X!&0920Checks for new or missing DSNlink databases.  List of databases is`20
X!&0920stored in file DSNLINK_NEW_DATABASES.DAT, and is automatically`20
X!&0920updated by DSNlink_NEW.  Optional username is list of users to`20
X!&0920inform of new DSNlink databases. `20
X!
X!    .GET_FILE filename&0620
X!&0920Filename to use for get-file routine.  Default is`20
X!&0920SYS$LOGIN:DSNLINK_GET.DAT.  To allow everyone that receives`20
X!&0920'normal' DSNlink_NEW mail to use the 'get' facility, the`20
X!&0920configuration file should specify a get file located in a`20
X!&0920directory that everyone can write to -- SYS$LOGIN works well
X!&0920for this function.
X!
X!    .GET_LOG_FILE filename
X!&0920Log-file name used for the batchjob .LOG file during GET`20
X!&0920processing.  Default is SYS$LOGIN:DSNLINK_NEW_GET.LOG
X!
X!    .WRITE_TITLES filename
X!&0920Default:  no action.  If a filename is specified, all article
X!&0920titles retrieved (by normal processing) will be appended to this
X!&0920file.  This can be used to simplify GET processing, but isn't
X!&0920commonly used.
X!
X!    .SCRATCH_DIR directory
X!&0920Scratch directory for DSNlink_NEW temporary files.  Default is
X!&0920DSN$SCRATCH, SYS$SCRATCH, SYS$LOGIN, or DSNLINK_NEW_DIR.
X!
X!    .FAO_SUBJECT "fao-controlstring"
X!    .FAO_PERSONAL "fao-controlstring"
X!    .FAO_GET_SUBJECT "fao-controlstring"
X!    .FAO_GET_PERSONAL "fao-controlstring"
X!&0920These four configuration settings allow control over the VMSmail
X!&0920subject and personal-name fields.  See DSNLINK_NEW.DOC for
X!&0920information on these settings.  When changing these settings,
X!&0920enabling DSNlink_NEW debugging (with the logical DSNLINK_NEW_DEBUG)
X!&0920will display sample output using your new FAO control strings.
X!
X!     .EXCLUDE database article-title
X!&0920Exclude <article-title> whenever it appears in the specified
X!&0920database.  The database name can be just "*" (without quotes).
X!&0920Quotes aren't needed for database name or for article-title.
X!&0920.EXCLUDE must appear after database in the configuration file.
X!
X!!!
X!
X!  If a dot-parameter is specified more than once, the last definition is
X!  used.  Specifying a database more than once may cause unpredictable
X!  results.
X!
X&4621
X
X!
X!!  GENERAL SETTINGS.
X!
X
X.Retry&12205
X.Since&1220Today-10-00:00
X.Mail_Empty&0D20NO
X.Next_Execution&0920Tomorrow+01:00&0820! "ONE" will disable re-Submits
X.History_File&0B20DSNLINK_NEW_HISTORY.DAT
X.History_Auto_Compress  600
X.Log_File&0F20DSNLINK_NEW.LOG
X.Check_OpenCalls&0820Never DWING
X.Check_ClosedCalls&0620Never DWING
X.Get_File&0F20SYS$LOGIN:DSNLINK_GET.DAT
X.Get_Log_File&0B20SYS$LOGIN:DSNLINK_NEW_GET.LOG
X.Scratch_Dir&0C20SYS$SCRATCH:
X
X!
X!!  DEFINE MAIL GROUPS.
X!!
X!!  The group "$ERROR$" is required.  All DSNlink connection errors and
X!!  unexpected errors are mailed to this user group (except during GET`20
X!!  processing, when it is rewritten to be the user specified in the`20
X!!  parameters or the user running DSNlink_NEW).
X!!
X!!  If you need to use a foreign mail protocol (such as MX% or IN%):
X!!&0620*  use a logical (the logical must be defined when running in batch)
X!!   or *  put the user's address into a .DIS file and use "@"
X!!   or *  use VMSmail forwarding for a pseudo-user, like:
X!!&0920MAIL> SET FORWARD/USER=D.WING "MX%""DWING@UH01.COLORADO.EDU"""
X!!&0920and then use D.WING as a mail address.
X!!
X!!  Upper- and lowercase is permitted in the mail group names and usernames.
X!!  (Upper/lowercase must be consistant between the .MAIL setting and
X!!   the database line -- see DSNLINK_NEW.DOC for details).
X!!
X!
X
X.Mail     $ERROR$&0620System
X.Mail     Systems&0620System
X.Mail     Programmers  Dr_Dobbs,NWirth
X
X!
X!!
X!!  DATABASES TO SEARCH.
X!!
X!!  Use "FLASH" and "NOFLASH" to indicate if you want to check for flash
X!!  articles.  See note above about unnecessary checking for flash articles.
X!!
X!!  Put the 'important' databases towards the top, as the databases are
X!!  included in mail messages in the order listed -- flash articles are`20
X!!  always placed above all other articles.
X!!
X!! * NOTE:&4320*
X!! *  Do not specify FLASH for databases that don't need it, or will never   *
V
X!! *  have FLASH articles -- it is a waste of resources on the DSNlink host  *
V
X!! *  system.  Examples of some databases that never need FLASH support are  *
V
X!! *  the databases SPD, VMS-SYSMGR-DOC, and VMS-RN-NF-DOC.  Please use&0620*
V
X!! *  your professional judgement.&2B20*
X!!
X!!
X!! **
X!! ** SPECIFYING A DATABASE MORE THAN ONCE MAY CAUSE UNPREDICTABLE RESULTS.
X!! **
X!
X
X
XECO-Summary&0820FLASH&0620Systems,Programmers
XVMS&1020FLASH&0620Systems,Programmers
XDECnet-VMS&0720NOFLASH&0620Systems
XShadow&0B20NOFLASH&0620Systems
XCMS&0E20NOFLASH&0620Systems,Programmers
XInfoServer&0720NOFLASH&0620Systems
XCobol&0C20NOFLASH&0620Programmers
XRdbVMS&0B20NOFLASH&0620Programmers
X
X
X.Exclude ECO-Summary  `5BOpenVMS`5D Which ECOs are Incorporated in VAX V6.0 /
V Need to Install
X.Exclude *&0C20`5BOpenVMS`5D Which ECOs are Incorporated in VAX V6.1 / Need to
V Install
X.Exclude ECO-Summary  `5BOpenVMS`5D Which ECOs Are Incorporated in Alpha V6.2
V / Need to Install
$ call unpack DSNLINK_NEW_CONFIG.DAT_TEMPLATE;12 894092009 "" 21 3 6
$!
$ create 'f'
X$ VERIFY = 'F$VERIFY(F$TRNLNM("DSNLINK_NEW_VERIFY"))'
X$!
X$!  Enhancements:  Verify (in .EXCLUDE) that the database the user
X$!&1120indicated in P2 is a valid database.
X$!&1120
X$!
X$ SET = "SET"
X$ SET SYMBOL/SCOPE=(noGLOBAL,noLOCAL)
X$ GOTO _START_DSNLINK_NEW
X
XABSTRACT:
X     This program automates checking the DSNlink ITS databases for new
X     articles.
X  `20
X     It will get list of new DSNlink article titles from the DSN host
X     system, optionally compare the list to a history database of
X     articles retreived by DSNlink_NEW, and mail the article titles that
X     weren't in the history database to a user or group of users.
X
X     DSNlink_NEW can also automatically retrieve the text of specific DSNlink
V
X     articles and mail them to a user or group of users (called "GET`20
X     Processing").
X   `20
X     DSNlink is a free service offered by Digital for software support.  It
X     is available in the US and Canada, as well as most other countries
X     (DSNlink is called "AES" in some regions).  Contact your Digital
X     salesperson, or account representative, for information on obtaining
X     DSNlink for your site.
X
XPARAMETERS:
X     P1 - `5Boptional`5D Configuration filename.  If not specified,
X&0A20DSNLINK_NEW_DIR:DSNLINK_NEW_CONFIG.DAT is used.  If the logical
X&0A20DSNLINK_NEW_DIR isn't defined, the location of DSNLINK_NEW.COM is`20
X&0A20used.
X
X `7B** Note -- parameters P2 through P6 are used by DSNlink_NEW.COM internally
V
X     when it submits itself to batch.  They aren't necessary to use any
X     of the features of DSNlink_NEW.  To do Get processing, simply extract
X     the mail from DSNlink_NEW into a a file called SYS$LOGIN:DSNLINK_GET.DAT
V`20
X     and then type "@DSNLINK_NEW" and it'll see the file and get the articles
V
X     for you automatically.  Also see DSNLINK_NEW.DOC for more information.`7D
V
X
X     P2 - `5Boptional`5D COMPRESS, which will remove all History records older
V`20
X&0A20than SINCE-2 days (SINCE is specified in your configuration file)
X&0620or  `5Boptional`5D $GET$, which causes GET processing to be performed (to
V`20
X&0A20get article text). `20
X&0C20NOTE:  Using this parameter is NOT normally required, as
X&1320DSNlink_NEW will automatically invoke GET Processing
X&1320if the Get file is found (the name of the get file is`20
X&1320specified with .GET_FILE in the configuration file, and
X&1320defaults to SYS$LOGIN:DSNLINK_GET.DAT).  See
X&1320DSNLINK_NEW.DOC for more information on how to use GET
X&1320Processing.
X&0620or  `5Boptional`5D $CHECKOPEN$, which, if run in batch, causes mail to be
V`20
X&0A20sent to DSN%OPENCALLS and then DSNlink_NEW is exited (used`20
X&0A20internally when a different user is specified with .CHECK_OPENCALLS)
X
X     P3 - get file - This is the file containing database names and article`20
X&0A20titles we're trying to retrieve; only used if P2 is $GET$
X
X     P4 - mail group name - only used if P2 is $GET$.
X
X     P5 - `5Boptional`5D $ERROR$ rewrite username, used only if P2 is $GET$.
X&0A20Also used in the "sent by:" line (in mail messages) if this`20
X&0A20username is different from the username running DSNlink_NEW.
X
X     P6 - Name of original GET file, used only if P2 is $GET$.  If this
X&0A20parameter is non-null, and GET Processing is successful, we'll`20
X&0A20delete the file in P6.  If GET Processing is unsuccessful,
X&0A20we'll include the filename in P6 to our mail sent to the
X&0A20$ERROR$ group, allowing the user to resubmit the GET Processing
X&0A20job after the DSNlink problem has been corrected.
X
XPRIVILEGES REQUIRED:
X     No special VMS privileges are required.  Access to DSN ITS, DSN$BATCH`20
X     queue, and ability to send DSNlink mail are necessary to use all`20
X     features.
X
XENVIRONMENT:
X     DCL.  Expected to work on all versions of VMS supported by DSNlink
X     software, on both VAX and AXP platforms.
X
XREQUIRES:
X     "DSNlink for OpenVMS VAX" or "DSNlink for OpenVMS AXP". `20
X    `20
X =&462D=
X
XOriginal idea taken from DSNlink article "`5BDSNlink`5D V1.n How To Get Daily
V`20
XList of New Database Articles" in the DSNLINK database.  An early version of`20
V
XDSNlink_NEW can also be found there.
X
XCopyright `A9 1991-1995 by Dan Wing.  This code may be freely distributed
Xand modified for no commercial gain as long as this copyright notice is
Xretained.  This program is supplied 'as-is', and with no warranty.
X
XCreated December 31, 1991, by Dan Wing, University Hospital, Denver.
XInternet:  wing@tgv.com or wing@eisner.decus.org
X
X =&462D=
X
X  REVERSE CHRONOLOGICAL MODIFICATION HISTORY
X  &2A2D
X
X  5.3&06202-OCT-1995  Dan Wing, wing@tgv.com
X     Allow user to exclude specific article titles (which are updated
X     often by Digital, causing them to re-appear in the DSNlink_NEW mail
X     messages).
X&0720
X  5.2-1   11-APR-1995  Dan Wing, wing@tgv.com
X     Allow user to choose when to delete Get file, so that we don't delete`20
X     GET file if GET Processing fails.
X  5.2     31-OCT-1994  Dan Wing, wing@tgv.com
X     Don't attempt to reconnect to DSNlink host if we receive "CSC call
X     tracking database is unavailable" (CSCERRACC), or "application is
X     not registered for this access number" (NOTREGSTRD) errors.  V1.2A
X     apparently needs the same handling for GET that was introduced in
X     V5.1-1, so make the "slow" GET processing happen if V1.2A is
X     detected (reported by rdickens@datastream.co.uk).  Include support for
X     PMDF/DELIVER for Get Processing in file PMDF_DELIVER.COM_TEMPLATE.
X
X  5.1-3    3-AUG-1994  Dan Wing, wing@tgv.com
X     Allow GET file to have ">" or "#" as first character (in case Forwarding
V
X     automatically inserts 'quote' characters).  Allow specifying priority of
V
X     DSNlink_NEW when it is doing batch job work with .BATCH_PRIORITY.
X  5.1-2   10-MAY-1994  Dan Wing, wing@tgv.com
X     Use /KEY=`26HISTORY_KEY to eliminate problem with single quotes in the
X     title retreived from the DSNlink host.  Thanks to Larry Stone.`20
X  5.1-1    4-NOV-1993  Dan Wing, dwing@uh01.colorado.edu
X     Automatically sense if we're using a version of DSNlink older than V1.2
X     -- old versions (V1.1 and V1.1-1) cannot use EDT as their callable editor
V
X     and don't work with GET processing's use of EDT -- so we automatically
X     switch to a less efficient method of retrieving articles (pulling them
X     over the modem twice).
X  5.1     22-SEP-1993  Dan Wing, dwing@uh01.colorado.edu
X     Enhancements to GET processing to work with the MX SITE agent, DELIVER,
X     and PMDF.  Fix bug when .CHECK_OPENCALLS wasn't in the configuration
X     file, and stopped playing with protection of the temporary file created
X     for GET processing.  New .SCRATCH_DIR configuration file setting for
X     DSNlink_NEW temporary files.  Added .CHECK_CLOSEDCALLS to complement
X     .CHECK_OPENCALLS.
X
X  5.0     11-SEP-1993  Dan Wing, dwing@uh01.colorado.edu
X     Ability to automatically retrieve article text (not just titles).  Show
X     number of articles retrieved from each database.  Inform $ERROR$ when we
V
X     get UNKDBNAM error (instead of everyone).  Ability to send mail to
X     DSN%OPENCALLS on a schedule.  Don't update history date every time we
X     re-retrieve an article -- only update the date when it is new, or if it
X     was last retrieved ALLOW_IF_OLDER (or more) days ago.  Configuration file
V
X     parser no longer uppercases everything.  Fixed other problems and
X     inconsistencies with case-sensitivity and error message display.  Send
X     Email for configuration file errors when running in batch.  Deliver mail
V
X     to all recipients even if DECnet mail can't be sent to certain nodes.
X
X  4.3-1   27-JUN-1993  Dan Wing, dwing@uh01.colorado.edu
X     Ensure history file exists before trying to display its size.
X  4.3     24-JUN-1993  Dan Wing, dwing@uh01.colorado.edu
X     Add ability to automatically compress history file.
X
X  4.2-2   13-JUN-1993  Dan Wing, dwing@uh01.colorado.edu
X     Fixed bug so database name is now displayed when history is disabled.
X  4.2-1    5-MAY-1993  Dan Wing, dwing@uh01.colorado.edu
X     When Emailing about errors, use several common message files to get
X     more information.  Changed default SINCE time to TODAY-10.
X  4.2     19-APR-1993  Dan Wing, dwing@uh01.colorado.edu
X     Separate history for flash and normal databases by using uppercase/
X     lowercase for database name within the history database.
X `20
X  Older modification history is available in the DSNlink_NEW documentation.
X
X =&462D=
X
X$!`0C
X$&4A21
X$
X$_START_DSNLINK_NEW:
X$
X$  ON WARNING THEN GOTO _ERROR
X$  ON CONTROL_Y THEN GOTO _ABORT
X$
X$!
X$
X$  DSNLINK_NEW_VERSION = "V5.3"
X$
X$  IF P1 .EQS. "$VERSION$"
X$  THEN
X$    SET SYMBOL/SCOPE=GLOBAL
X$    DSNLINK_NEW_VERSION == DSNLINK_NEW_VERSION
X$    VERIFY = F$VERIFY(VERIFY)
X$    EXIT   ! Leaving DSNlink_NEW ...
X$  ENDIF
X$
X$  WRITE SYS$OUTPUT F$FAO("!/DSNlink_NEW, version !AS.!/Started by !AS on node
V !AS on !AS, !AS !8AS.", -
X     DSNLINK_NEW_VERSION, F$EDIT(F$GETJPI(0,"USERNAME"),"TRIM"), -
X     F$GETSYI("NODENAME"), F$CVTIME(,,"WEEKDAY"), F$CVTIME(,"ABSOLUTE","DATE")
V, F$CVTIME(,,"TIME"))
X$
X$  GOSUB _INIT_DEFAULTS
X$
X$  IF F$SEARCH("SYS$SYSTEM:DSN$MAIN.EXE") .EQS. "" THEN GOTO _NO_DSNLINK
X$
X$  WRITE SYS$OUTPUT F$FAO("DSNlink version !AS.!/", DSN$VERSION)
X$
X$  IF F$SEARCH(DSN_CONFIG_FILENAME) .EQS. "" THEN GOTO _NO_CONFIG_FILE
X$  GOSUB _INIT_PARSE_CONFIG_FILE
X$  IF CONFIG_ERROR THEN GOTO _EXIT_ERROR
X$  IF BATCH .AND. (LOW_PRIORITY .NE. PRIB) THEN SET PROCESS/PRIORITY='LOW_PRIO
VRITY''DEBUG_LOG'
X$
X$  ALLOW_IF_OLDER = F$CVTIME("''F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE")'-2-00:00
V","COMPARISON","DATE")
X$
X$  IF (INTERACTIVE .AND. F$SEARCH(GET_FILE) .NES. "") .OR. -
X&0620(P2 .EQS. "$GET$") THEN GOTO _DSNLINK_GET
X$
X$  IF (BATCH .AND. P2 .EQS. "$CHECKOPEN$")`20
X$  THEN
X$    GOSUB _DO_CHECK_OPENCALLS
X$    FINISHED_OK = TRUE
X$    GOTO _EXIT
X$  ENDIF
X$
X$  IF (BATCH .AND. P2 .EQS. "$CHECKCLOSED$")
X$  THEN
X$    GOSUB _DO_CHECK_CLOSEDCALLS
X$    FINISHED_OK = TRUE
X$    GOTO _EXIT
X$  ENDIF
X$
X$  IF P2 .EQS. "COMPRESS"`20
X$  THEN`20
X$    GOSUB _COMPRESS_HISTORY
X$    GOTO _EXIT
X$  ENDIF
X$  IF P2 .NES. "" THEN GOTO _BAD_P2
X$
X$&4621
X$
X$  COUNT = 0
X$  WRITE SYS$OUTPUT F$FAO("  !24<Connection retries:!>!UL", DSN_RETRY_MAX)
X$  IF F$EDIT(DSN_AFTER,"UPCASE") .NES. "ONE"`20
X$  THEN`20
X$    WRITE SYS$OUTPUT F$FAO("  !24<Next run at:!>!AS (!AS, !AS)", -
X     DSN_AFTER, -
X     F$CVTIME(DSN_AFTER,,"WEEKDAY"), -
X     F$CVTIME(DSN_AFTER,"ABSOLUTE") - ":00.00" - ".00")
X$  ELSE
X$    WRITE SYS$OUTPUT F$FAO("  !24<Next run at:!>(only one run)")
X$  ENDIF
X$  WRITE SYS$OUTPUT F$FAO("  !24<History file:!>!AS", DSN_HISTORY_FILE)
X$  IF DSN_HISTORY_FILE .NES. "NONE" .AND. F$SEARCH(DSN_HISTORY_FILE) .NES. ""
V
X$  THEN`20
X$    IF DSN_HISTORY_AUTO_COMPRESS .GT. 0`20
X$    THEN
X$&0620WRITE SYS$OUTPUT F$FAO("  !24<History auto compress:!>at !UL blocks (cu
Vrrent size !UL blocks)", -
X     DSN_HISTORY_AUTO_COMPRESS, F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF"))
X$&0620IF F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF") .GT. DSN_HISTORY_AUTO_COMPR
VESS
X$&0620THEN
X$&0820WRITE SYS$OUTPUT F$FAO("!26* compression will be performed")
X$&0820PERFORM_AUTO_COMPRESS = TRUE
X$&0620ENDIF ! history auto compress will be performed
X$    ELSE
X$&0620WRITE SYS$OUTPUT F$FAO("  !24<History auto compress:!>Disabled")
X$    ENDIF  ! history auto compress enabled
X$  ENDIF ! dsn_history_file <> none
X$  IF WRITE_TITLES .NES. "" THEN WRITE SYS$OUTPUT F$FAO("  !24<Write all title
Vs to:!>!AS", WRITE_TITLES)
X$
X$  IF F$TYPE(EXCLUDE_ARTICLE_0) .NES. ""
X$  THEN
X$    WRITE SYS$OUTPUT F$FAO("  Excluded article!0UL!%S:", MAX_EXC_COUNT + 1)
X$    EXC_COUNT = 0
X$    _DIS_EXC_LOOP:
X$    IF F$TYPE(EXCLUDE_ARTICLE_'EXC_COUNT') .NES. ""`20
X$    THEN`20
X$&0620IF F$LENGTH(EXCLUDE_ARTICLE_'EXC_COUNT') .GT. 52
X$&0620THEN
X$&0820WRITE SYS$OUTPUT F$FAO("    !20<!AS!>  !52<!AS!>`7E", EXCLUDE_DB_'EXC_CO
VUNT', EXCLUDE_ARTICLE_'EXC_COUNT')
X$&0620ELSE
X$&0820WRITE SYS$OUTPUT F$FAO("    !20<!AS!>  !52<!AS!>", EXCLUDE_DB_'EXC_COUNT
V', EXCLUDE_ARTICLE_'EXC_COUNT')
X$&0620ENDIF
X$    ENDIF
X$    EXC_COUNT = EXC_COUNT + 1
X$    IF EXC_COUNT .LE. MAX_EXC_COUNT THEN GOTO _DIS_EXC_LOOP
X$  ENDIF
X$
X$  IF F$EDIT(DSN_CHECK_OPENCALLS,"UPCASE") .NES. "NEVER"`20
X$  THEN
X$    WRITE SYS$OUTPUT F$FAO("  !24<Check opencalls:!>!AS, user: !AS", DSN_CHEC
VK_OPENCALLS, DSN_CHECK_OPENCALLS_USER)
X$  ELSE
X$    WRITE SYS$OUTPUT F$FAO("  !24<Check opencalls:!>Never")
X$  ENDIF
X$
X$  IF F$EDIT(DSN_CHECK_CLOSEDCALLS,"UPCASE") .NES. "NEVER"
X$  THEN`20
X$    WRITE SYS$OUTPUT F$FAO("  !24<Check closedcalls:!>!AS, user: !AS", DSN_CH
VECK_CLOSEDCALLS, DSN_CHECK_CLOSEDCALLS_USER)
X$  ELSE
X$    WRITE SYS$OUTPUT F$FAO("  !24<Check closedcalls:!>Never")
X$  ENDIF
X$
X$  WRITE SYS$OUTPUT F$FAO("  !24<Since:!>!AS (!AS, !AS)", DSN_SINCE, -
X     F$CVTIME(DSN_SINCE,,"WEEKDAY"), F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE"))
X$  IF DSN_MAIL_EMPTY
X$  THEN
X$    WRITE SYS$OUTPUT F$FAO("  !24<Send mail if empty:!>Yes")
X$  ELSE
X$    WRITE SYS$OUTPUT F$FAO("  !24<Send mail if empty:!>No")
X$  ENDIF
X$  WRITE SYS$OUTPUT F$FAO("  !24<Batch log file:!>!AS", F$PARSE(LOG_FILE) - ";
V")
X$  IF LOW_PRIORITY .NES. PRIB THEN WRITE SYS$OUTPUT F$FAO("  !24<Batch priorit
Vy:!>!UL", LOW_PRIORITY)
X$  WRITE SYS$OUTPUT F$FAO("  !24<Get file:!>!AS", F$PARSE(GET_FILE) - ";")
X$
X$_DB_DISPLAY_LOOP:
X$  IF MAIL_GROUP_NAME_'COUNT' .NES. "$ERROR$"`20
X$  THEN
X$    TMP = F$FAO("Will send !AS new articles from databases: ", -
X&0720MAIL_GROUP_USERS_'COUNT')`20
X$    GROUP_NAME = MAIL_GROUP_NAME_'COUNT'
X$    WRITE SYS$OUTPUT ""
X$    CALL _DISPLAY_DATABASES SYS$OUTPUT 0 "''TMP'" "''GROUP_NAME'" 2
X$  ELSE
X$    WRITE SYS$OUTPUT F$FAO("!/Will send !AS any errors.", MAIL_GROUP_USERS_'C
VOUNT')
X$  ENDIF  ! not error group
X$  COUNT = COUNT + 1
X$  IF COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _DB_DISPLAY_LOOP
X$
X$  WRITE SYS$OUTPUT ""
X$  IF BATCH THEN SET OUTPUT_RATE&0820! flush stuff to LOG file`20
X$
X$  IF INTERACTIVE
X$  THEN
X$    ASK "* Is this correct? `5BY`5D " OK_TO_CONTINUE
X$    IF (.NOT. OK_TO_CONTINUE) .AND. (OK_TO_CONTINUE .NES. "") THEN GOTO _ABOR
VT
X$  ENDIF
X$
X$&4621
X$
X$  IF DSN_HISTORY_FILE .NES. "NONE"
X$  THEN
X$    IF F$SEARCH(DSN_HISTORY_FILE) .EQS. ""`20
X$    THEN
X$&0620CREATE/FDL=SYS$INPUT:/LOG 'DSN_HISTORY_FILE'
XFILE
X  BEST_TRY_CONTIGUOUS  yes
X  ORGANIZATION&0920INDEXED
X  EXTENSION&0C2027&0A20! in case it never gets COMPRESSed
XRECORD
X  FORMAT  FIXED
X  SIZE    108&0820! date (YYYY-MM-DD) + database + title = 10 + 20 + 78
XKEY 0
X  INDEX_COMPRESSION  no
X  DUPLICATES&0920no
X  NULL_KEY&0B20no
X  SEG0_LENGTH&082098
X  SEG0_POSITION&062010
X$    ELSE
X$&0620! test to ensure we can R+W the history file
X$&0620OPEN/READ/WRITE/SHARE=WRITE DSN_OUTFILE 'DSN_HISTORY_FILE'
X$&0620CLOSE DSN_OUTFILE
X$    ENDIF  ! history file exists
X$  ENDIF ! history disabled
X$
X$&4621
X$
X$  IF INTERACTIVE .OR. F$EDIT(DSN_AFTER,"UPCASE") .NES. "ONE"
X$  THEN
X$    IF F$EDIT(DSN_AFTER,"UPCASE") .EQS. "ONE"
X$    THEN
X$&0620AFTER_QUAL = ""&0820! right now
X$    ELSE
X$&0620AFTER_QUAL = "/AFTER=" + QUOTE + F$CVTIME(DSN_AFTER,"ABSOLUTE") + QUOTE
V
X$    ENDIF
X$
X$    SUBMIT -
X     /NOTIFY -
X     /QUEUE=DSN$BATCH -
X     /noPRINTER -
X     /KEEP -
X     'AFTER_QUAL' -
X     /PARAMETERS=('F$PARSE(";",DSN_CONFIG_FILENAME)')  -
X     /NAME="DSNlink_NEW" -
X     /LOG_FILE='LOG_FILE' -
X     'F$PARSE(";",F$ENVIRONMENT("PROCEDURE"))'
X$
X$    IF F$EDIT(DSN_AFTER,"UPCASE") .EQS. "ONE" .OR. INTERACTIVE
X$    THEN
X$&0620FINISHED_OK = TRUE
X$&0620GOTO _EXIT
X$    ENDIF  ! dsn_after = "one" or interactive
X$  ENDIF ! interactive or dsn_after = "one"
X$
X$!`0C
X$&4621
X$! See if we need to send mail to DSN%OPENCALLS and DSN%CLOSEDCALLS.
X$! we delay sending the actual mail to immediately before we connect to the
X$! host system to try to get two 'streams' going to the host system at`20
X$! once to shorten the total time we're using the modem.
X$&4621
X$
X$  DO_CHECK_OPENCALLS   = FALSE
X$
X$  IF F$EDIT(DSN_CHECK_OPENCALLS,"UPCASE") .NES. "NEVER"`20
X$  THEN
X$    COUNT = 0
X$    _OPENCALLS_CHECK:
X$    CHECK_VALUE = F$ELEMENT(COUNT,",",DSN_CHECK_OPENCALLS)
X$    IF CHECK_VALUE .EQS. "," THEN GOTO _STOP_OPENCALLS_CHECK
X$    IF F$TYPE(CHECK_VALUE) .EQS. "INTEGER"
X$    THEN
X$    ! it is a day of the month (1, 2, 3)
X$&0620IF F$CVTIME(,,"DAY") .EQ. CHECK_VALUE
X$&0620THEN
X$&0820DO_CHECK_OPENCALLS = TRUE
X$&0820IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, OPENCALLS triggere
Vd by `5C", CHECK_VALUE, "`5C"
X$&0620ENDIF
X$    ELSE
X$    ! it is the day of a week (Monday, Tuesday, ...)
X$&0620IF F$EDIT(F$CVTIME(,,"WEEKDAY"),"UPCASE") .EQS. F$EDIT(CHECK_VALUE,"UPCA
VSE")`20
X$&0620THEN
X$&0820DO_CHECK_OPENCALLS = TRUE
X$&0820IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, OPENCALLS triggere
Vd by `5C", CHECK_VALUE, "`5C"
X$&0620ENDIF
X$    ENDIF
X$    COUNT = COUNT + 1
X$    IF .NOT. DO_CHECK_OPENCALLS THEN GOTO _OPENCALLS_CHECK
X$  ENDIF ! dont check
X$
X$_STOP_OPENCALLS_CHECK:
X$  IF DO_CHECK_OPENCALLS THEN GOSUB _DO_CHECK_OPENCALLS
X$
X$!
X$!!!
X$!
X$
X$  DO_CHECK_CLOSEDCALLS = FALSE
X$
X$  IF F$EDIT(DSN_CHECK_CLOSEDCALLS,"UPCASE") .NES. "NEVER"`20
X$  THEN
X$    COUNT = 0
X$    _CLOSEDCALLS_CHECK:
X$    CHECK_VALUE = F$ELEMENT(COUNT,",",DSN_CHECK_CLOSEDCALLS)
X$    IF CHECK_VALUE .EQS. "," THEN GOTO _STOP_CLOSEDCALLS_CHECK
X$    IF F$TYPE(CHECK_VALUE) .EQS. "INTEGER"
X$    THEN
X$    ! it is a day of the month (1, 2, 3)
X$&0620IF F$CVTIME(,,"DAY") .EQ. CHECK_VALUE
X$&0620THEN
X$&0820DO_CHECK_CLOSEDCALLS = TRUE
X$&0820IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CLOSEDCALLS trigge
Vred by `5C", CHECK_VALUE, "`5C"
X$&0620ENDIF
X$    ELSE
X$    ! it is the day of a week (Monday, Tuesday, ...)
X$&0620IF F$EDIT(F$CVTIME(,,"WEEKDAY"),"UPCASE") .EQS. F$EDIT(CHECK_VALUE,"UPCA
VSE")`20
X$&0620THEN
X$&0820DO_CHECK_CLOSEDCALLS = TRUE
X$&0820IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, CLOSEDCALLS trigge
Vred by `5C", CHECK_VALUE, "`5C"
X$&0620ENDIF
X$    ENDIF
X$    COUNT = COUNT + 1
X$    IF .NOT. DO_CHECK_CLOSEDCALLS THEN GOTO _CLOSEDCALLS_CHECK
X$  ENDIF ! dont check
X$
X$_STOP_CLOSEDCALLS_CHECK:
X$  IF DO_CHECK_CLOSEDCALLS THEN GOSUB _DO_CHECK_CLOSEDCALLS
X$
X$!`0C
X$&4621
X$!!!  Build input file for "DSN ITS" command
X$&4621
X$
X$  GOSUB _DELETE_TMP_FILES
X$ `20
X$  OPEN/WRITE DSN_OUTFILE 'TMP_DSNFILE'
X$
X$  COUNT = 0
X$  DB_NAME = ""
X$_FILE_LOOP_1:
X$  IF DB_NAME .NES. ""`20
X$  THEN`20
X$    WRITE DSN_OUTFILE "close ", DB_NAME
X$    WRITE DSN_OUTFILE ""
X$  ENDIF
X$  DB_NAME = DB_NAME_'COUNT'
X$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT)
X$  DB_DIRFILE_FLASH = DB_DIRFILE + "_FLASH"
X$  WRITE DSN_OUTFILE "open ", DB_NAME
X$  WRITE DSN_OUTFILE "directory/all/since=", QUOTE, F$CVTIME(DSN_SINCE,"ABSOLU
VTE"), QUOTE
X$  WRITE DSN_OUTFILE "extract/directory ", DB_DIRFILE
X$  IF DB_FLASH_'COUNT'
X$  THEN
X$    WRITE DSN_OUTFILE "directory/all/flash/since=", QUOTE, F$CVTIME(DSN_SINCE
V,"ABSOLUTE"), QUOTE
X$    WRITE DSN_OUTFILE "extract/directory ", DB_DIRFILE_FLASH
X$  ENDIF ! symbol is TRUE
X$  COUNT = COUNT + 1
X$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _FILE_LOOP_1
X$
X$  WRITE DSN_OUTFILE "exit"
X$  CLOSE DSN_OUTFILE
X$
X$  WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-I-COUNT, !UL database!%S will be che
Vcked for new articles.", COUNT)
X$
X$!
X$!!! connect to DSNlink host system
X$!
X$
X$  GOSUB _CALL_DSN_HOST
X$
X$  IF F$SEARCH("''TMP_EXTRACTFILE'*",2) .EQS. "" THEN GOTO _NO_DSNLINK_ARTICLE
VS
X$
X$!`0C
X$&4621
X$!!!  Found DSNlink articles.
X$!!!  We need to remove articles we've already seen (history records), and
X$!!!  then generate the appropriate mail messages
X$&4621
X$
X$  IF BATCH THEN SET RESTART_VALUE="Processing retreived articles"
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-S-NEWART, DSNlink articles were retrieved fr
Vom DSNlink host."
X$
X$! Remove history records`20
X$  COUNT = 0
X$_PROCESS_HIST_LOOP:
X$  DB_NAME = DB_NAME_'COUNT'
X$
X$  DSNLINK_FLASH = TRUE
X$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT) + "_FLASH"
X$  IF F$SEARCH(DB_DIRFILE) .NES. "" THEN GOSUB _ELIMINATE_HISTORY_RECORDS
X$
X$  DSNLINK_FLASH = FALSE
X$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT)`20
X$  IF F$SEARCH(DB_DIRFILE) .NES. "" THEN GOSUB _ELIMINATE_HISTORY_RECORDS
X$
X$  COUNT = COUNT + 1
X$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _PROCESS_HIST_LOOP
X$ `20
X$_GENERATE_MAIL_MESSAGES:
X$!
X$! generate mail messages for each group, as defined in config file
X$  MAIL_COUNT = 0
X$  _GEN_ART_LOOP:
X$  SELECT_MAIL_GROUP_NAME = MAIL_GROUP_NAME_'MAIL_COUNT'
X$  IF SELECT_MAIL_GROUP_NAME .NES. "$ERROR$"
X$  THEN
X$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-S-GENART, Generating mail for grou
Vp !AS", SELECT_MAIL_GROUP_NAME)
X$    GOSUB _BUILD_MESSAGE
X$    IF F$SEARCH(TMP_MAILFILE) .NES. ""
X$    THEN
X$&0620TMP1 = F$FAO(FAO_SUBJECT, TOTAL_FLASH_ARTICLES, TOTAL_NORMAL_ARTICLES, T
VOTAL_FLASH_ARTICLES + TOTAL_NORMAL_ARTICLES, SELECT_MAIL_GROUP_NAME, TOTAL_DAT
VABASES)
X$&0620TMP2 = F$FAO(FAO_PERSONAL, TOTAL_FLASH_ARTICLES, TOTAL_NORMAL_ARTICLES,
V TOTAL_FLASH_ARTICLES + TOTAL_NORMAL_ARTICLES, SELECT_MAIL_GROUP_NAME, TOTAL_D
VATABASES)
X$&0620IF WRITE_TITLES .NES. ""`20
X$&0620THEN
X$&0820WRITE SYS$OUTPUT "%DSNlink_NEW-I-WRITETITLES, Writing titles to: ", WRIT
VE_TITLES
X$&0820APPEND 'TMP_MAILFILE' 'WRITE_TITLES'/NEW_VERSION/LOG
X$&0620ENDIF
X$&0620CALL _SEND_MAIL_MESSAGE -
X     'TMP_MAILFILE' -
X     "''TMP1'" -
X     "''SELECT_MAIL_GROUP_NAME'" -
X     "''TMP2'"
X$&0620DELETE 'TMP_MAILFILE';*
X$    ELSE   ! everything was rejected because of history and/or database wasn'
Vt selected
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-NOSELECTED, No articles for group ", SE
VLECT_MAIL_GROUP_NAME
X$&0620IF DSN_MAIL_EMPTY`20
X$&0620THEN`20
X$&0820CREATE 'TMP_MAILFILE'
XNo new DSNlink articles were found that matched configuration criteria.
X$&0820CALL _SEND_MAIL_MESSAGE 'TMP_MAILFILE' "No new ''SELECT_MAIL_GROUP_NAME'
V DSNlink articles" "''SELECT_MAIL_GROUP_NAME'"
X$&0820DELETE 'TMP_MAILFILE';*
X$&0620ENDIF ! dsn_mail_empty
X$    ENDIF ! no file to mail
X$  ENDIF ! error group
X$  MAIL_COUNT = MAIL_COUNT + 1
X$  IF MAIL_COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _GEN_ART_LOOP
X$  FINISHED_OK = TRUE
X$  GOTO _POST_PROCESSING
X$
X$!`0C
X$&4621
X$!  Call DSNlink host, and retry as necessary
X$!  This routine is used by "normal" DSNlink_NEW as well as DSNlink_NEW's
X$!  "get" routines.
X$&4621
X$_CALL_DSN_HOST:
X$  IF BATCH THEN SET RESTART_VALUE="Calling DSNlink host"
X$! build DCL jacket around the DSN ITS commands
X$! (we have to use this method instead of the old DSN$ITS_INIT method
X$!  because EDT was rewinding the input file every time it was invoked, but
X$!  it doesn't rewind stuff fed to it in this manner).
X$  SET FILE/VERSION_LIMIT=2 'TMP_DSNFILE'
X$  OPEN/WRITE DSN_OUTFILE 'TMP_DSNFILE';&0720! new version
X$
X$  IF PERFORMING_GET
X$  THEN
X$    IF GET_STYLE .EQS. "EDT"
X$    THEN
X$&0620WRITE DSN_OUTFILE "$! GET_STYLE = EDT"
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE DSN$ITS_EDIT CALLABLE_EDT"
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE EDTSYS NLA0:"
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE EDTINI NLA0:"
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE SYS$ERROR ", TMP_ERRORFILE
X$&0620! defining SYS$OUTPUT seems to mess up EDT (gives %SYSTEM-F-IVDEVNAM)
X$
X$&0620! Note on SYS$SCRATCH (DSNlink V1.2b):
X$&0620!   when run from user DSN$SERVER, DSN$MAIN.EXE seems to leave on SYSPRV
V
X$&0620!   which causes problems when trying to delete edit-journal file owned
V`20
X$&0620!   by the owner of the scratch directory (`5BSYSTEM`5D) -- so, as a wor
Vk-
X$&0620!   around, we set DSN$SCRATCH to point to a directory that is owned
X$&0620!   by our UIC no matter what (this controls the creation of the`20
X$&0620!   DSN$ITS_pid.TMP file which contains the file we're editing).
X$&0620!   This problem doesn't seem to exist with other usernames....
X$&0620!   In case future versions of DSNlink use SYS$SCRATCH instead of
X$&0620!   DSN$SCRATCH for the location of this temporary file, we're going
X$&0620!   to set both SYS$SCRATCH and DSN$SCRATCH just to be sure.
X$&0620!
X$&0620!   Note that this means the DSN ITS commands (which we're about to`20
X$&0620!   execute) cannot contain references to the logicals SYS$SCRATCH or`20
X$&0620!   DSN$SCRATCH, as we're only temporarily re-defining them to point
X$&0620!   to SYS$LOGIN.
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE DSN$SCRATCH ", F$PARSE("SYS$LOGIN"
V) - ".;"
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE SYS$SCRATCH ", F$PARSE("SYS$LOGIN"
V) - ".;"
X$    ENDIF
X$    IF GET_STYLE .EQS. "EXTRACT"
X$    THEN
X$&0620WRITE DSN_OUTFILE "$! GET_STYLE = EXTRACT"
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE SYS$ERROR ", TMP_ERRORFILE
X$&0620WRITE DSN_OUTFILE "$ DEFINE/USER_MODE SYS$OUTPUT ", TMP_OUTFILE
X$    ENDIF
X$  ELSE
X$    WRITE DSN_OUTFILE "$! normal DSNlink_NEW processing"
X$    WRITE DSN_OUTFILE "$ DEFINE/USER_MODE SYS$ERROR ", TMP_ERRORFILE
X$    WRITE DSN_OUTFILE "$ DEFINE/USER_MODE SYS$OUTPUT ", TMP_OUTFILE
X$  ENDIF
X$
X$  WRITE DSN_OUTFILE "$"
X$  WRITE DSN_OUTFILE "$ DEFINE/USER_MODE DSN$ITS_INIT NLA0:"
X$  WRITE DSN_OUTFILE "$"
X$  WRITE DSN_OUTFILE "$ SET noON"
X$  WRITE DSN_OUTFILE "$ DSN ITS"
X$  CLOSE DSN_OUTFILE
X$
X$  APPEND 'TMP_DSNFILE';-1 'TMP_DSNFILE'
X$
X$  OPEN/APPEND DSN_OUTFILE 'TMP_DSNFILE'
X$  IF F$EXTRACT(1,-1,DSN$VERSION) .LTS. "1.2" THEN WRITE DSN_OUTFILE "no"  ! i
Vn case DSN ITS asks for a survey (V1.2 and up don't ask for surveys if we're r
Vunning in batch mode)
X$  WRITE DSN_OUTFILE "$ EXIT '$STATUS'"
X$  CLOSE DSN_OUTFILE
X$
X$  IF DEBUG
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, .COM file to connect to DSNlink h
Vost:"
X$    TYPE 'TMP_DSNFILE'
X$  ENDIF   ! DEBUG
X$
X$!
X$!!!
X$!
X$
X$_DSN_RETRY:
X$  WRITE SYS$OUTPUT ""
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-ATTCONN, Attempting connection at ", F$TIM
VE()
X$  IF BATCH THEN SET OUTPUT_RATE  ! flush stuff to LOG file
X$
X$  ATTEMPT_CONNECT_TIME = F$CVTIME(,,"TIME")
X$  ATTEMPT_CONNECT_DATE = F$CVTIME(,,"DATE")
X$  DSN_ATTEMPT_COUNT = DSN_ATTEMPT_COUNT + 1
X$
X$  IF F$SEARCH(TMP_ERRORFILE) .NES. "" THEN DELETE 'TMP_ERRORFILE';*
X$  IF F$SEARCH(TMP_OUTFILE) .NES. "" THEN DELETE 'TMP_OUTFILE';*
X$
X$! we're currently running at LOW_PRIORITY; if this is lower than what
X$! we started with, go back up...  Note some *old* versions of VMS can't
X$! handle this properly, so we ignore any possible errors just to be sure
X$! (I believe it was after VMS V5.0, but not sure if V5.1 or V5.2 had this
X$! behavior, too...).
X$  SET noON
X$  IF LOW_PRIORITY .LT. PRIB THEN SET PROCESS/PRIORITY='PRIB''DEBUG_LOG'
X$  SET ON
X$
X$  SET noON
X$  @'TMP_DSNFILE'
X$  DSNLINK_ITS_EXIT_STATUS = $STATUS
X$  SET ON
X$
X$  IF LOW_PRIORITY .NE. PRIB THEN SET PROCESS/PRIORITY='LOW_PRIORITY''DEBUG_LO
VG'
X$ `20
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-RETURNED, Attempt completed at ", F$TIME()
V
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-EXITSTATUS, DSN ITS exit $STATUS value: ",
V DSNLINK_ITS_EXIT_STATUS
X$! calculate elapsed connection time (only works for the same day)
X$! (from James Thompson, DECUServe VMS conference, note 1270.20)
X$  SET noON
X$  DEFINE SYS$OUTPUT NLA0:
X$  DEFINE SYS$ERROR NLA0:
X$  ELAPSED_CONN_TIME = F$CVTIME("-''ATTEMPT_CONNECT_TIME'",,"TIME")
X$  STATUS = $STATUS
X$  DEASSIGN SYS$OUTPUT
X$  DEASSIGN SYS$ERROR
X$  SET ON
X$  IF (.NOT. STATUS) .OR. (TOTAL_ELAPSED_CONN_TIME .EQS. "--:--:--") .OR. (ATT
VEMPT_CONNECT_DATE .NES. F$CVTIME(,,"DATE"))
X$  THEN    ! The day changed,`20
X$    TOTAL_ELAPSED_CONN_TIME = "--:--:--"
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-W-ELAPSED, Unable to calculate elapsed tim
Ve (", STATUS, ")"
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-W-2DAYS, Can only calculate elapsed time f
Vor same day."
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-W-2DAYS, Connect time will be set to ", TO
VTAL_ELAPSED_CONN_TIME
X$  ELSE
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-ELAPSED, Connect time to DSNlink host: "
V, ELAPSED_CONN_TIME
X$    TOTAL_ELAPSED_CONN_TIME = F$CVTIME("''TOTAL_ELAPSED_CONN_TIME'+''ELAPSED_
VCONN_TIME'",,"TIME")
X$  ENDIF ! .not. status
X$  WRITE SYS$OUTPUT ""
X$
X$  IF DEBUG`20
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, DSNlink connection attempt finish
Ved (''F$TIME()')"
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Error file from DSN ITS (SYS$ERRO
VR):"
X$    IF F$SEARCH(TMP_ERRORFILE) .NES. ""`20
X$    THEN
X$&0620TYPE 'TMP_ERRORFILE'
X$    ELSE
X$&0620WRITE SYS$OUTPUT "DSNlink_NEW-I-DEBUG, <No SYS$ERROR file was created>"
V
X$    ENDIF
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Output file from DSN ITS (SYS$OUT
VPUT):"
X$    IF F$SEARCH(TMP_OUTFILE) .NES. ""
X$    THEN
X$&0620TYPE 'TMP_OUTFILE'
X$    ELSE
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, <No SYS$OUTPUT file was created>
V"
X$    ENDIF
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Contents of DSN ITS directory ext
Vract files:"
X$    IF F$SEARCH("''TMP_EXTRACTFILE'*",1) .NES. ""
X$    THEN
X$&0620TYPE 'TMP_EXTRACTFILE'*
X$    ELSE
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, <no DSN ITS directory extract fi
Vles were created>"
X$    ENDIF ! not found
X$  ENDIF  ! debug
X$
X$!`0C
X$&4621
X$!!!  Determine if encountered an error
X$&4621
X$
X$  RETRY_OPERATION = FALSE
X$
X$! if we got an UNKDBNAM (unknown database) error, inform $ERROR$ group now
X$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
X$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
X$  SEARCH/EXACT 'TMP_ERRORFILE' "%DSN-E-UNKDBNAM"`20
X$  IF $STATUS .EQ. SS$_NORMAL`20
X$  THEN`20
X$    ! if we had an UNKDBNAM (unknown database) error, assume (cough cough)
X$    ! that it was the only error, and we don't need to retry the operation.
X$    RETRY_OPERATION = FALSE
X$    SET FILE/VERSION_LIMIT=2 'TMP_ERRORFILE';
X$    SEARCH/EXACT 'F$SEARCH(TMP_ERRORFILE)' "%DSN-","-DSN-"/OUTPUT='TMP_ERRORF
VILE'
X$    CALL _SEND_MAIL_MESSAGE 'TMP_ERRORFILE' "DSNlink UNKDBNAM error" $ERROR$
V
X$  ELSE
X$    ! Check to see if we got any errors and if we need to retry the connectio
Vn
X$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
X$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
X$    SEARCH/EXACT 'TMP_ERRORFILE' "%DSN-F-","%DSN-E-","-DSN-E-","-DSN-F-"
X$    IF $STATUS .EQ. SS$_NORMAL THEN RETRY_OPERATION = TRUE   ! found an error
V
X$  ENDIF
X$
X$  IF RETRY_OPERATION   ! we got an error other than UNKDBNAM
X$  THEN&0720! some sort of DSNlink failure
X$    SEARCH/EXACT/noHIGHLIGHT 'TMP_ERRORFILE' -
X&0A20"-F-","-E-"    ! show error in .LOG file (%DSN and %SYSTEM errors)
X$    WRITE SYS$OUTPUT ""
X$
X$    ! see if the error was "CSCERRACC, CSC call tracking database is unavaila
Vble"
X$    ! if so, get out now -- retrying isn't going to make it available
X$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
X$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
X$    SEARCH/EXACT 'TMP_ERRORFILE' "CSCERRACC"
X$    IF $STATUS .EQ. SS$_NORMAL
X$    THEN
X$&0620! we found it!  That must have been why we failed
X$&0620EXCEEDED_RETRY_LIMIT = TRUE
X$&0620RECEIVED_FATAL_ERROR = TRUE
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-QUITTING, Detected CSCERRACC error -- n
Vot attempting to retry."
X$&0620GOTO _NO_DSNLINK_ARTICLES
X$    ENDIF
X$
X$    ! see if the error was "NOTREGSTRD, application is not registered for thi
Vs access number"
X$    ! if so, get out now -- retrying isn't going to make it available
X$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
X$    IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
X$    SEARCH/EXACT 'TMP_ERRORFILE' "NOTREGSTRD"
X$    IF $STATUS .EQ. SS$_NORMAL
X$    THEN
X$&0620! we found it!  That must have been why we failed
X$&0620EXCEEDED_RETRY_LIMIT = TRUE
X$&0620RECEIVED_FATAL_ERROR = TRUE
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-QUITTING, Detected NOTREGSTRD error --
V not attempting to retry."
X$&0620GOTO _NO_DSNLINK_ARTICLES
X$    ENDIF
X$
X$    IF (DSN_RETRY_COUNT + 1) .LE. DSN_RETRY_MAX
X$    THEN`20
X$&0620DSN_RETRY_COUNT = DSN_RETRY_COUNT + 1
X$&0620IF BATCH THEN SET RESTART_VALUE="Connection retry count = ''DSN_RETRY_CO
VUNT'"
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-RETRY, Waiting ", DSN_RETRY_WAIT_TIME,
V " to retry connection"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-RETRY, Retry number ", DSN_RETRY_COUNT
V
X$&0620WAIT 'DSN_RETRY_WAIT_TIME'
X$&0620GOTO _DSN_RETRY
X$    ELSE
X$&0620EXCEEDED_RETRY_LIMIT = TRUE
X$&0620GOTO _NO_DSNLINK_ARTICLES
X$    ENDIF    ! retry limit exceeded?
X$  ENDIF ! retry_operation
X$  IF BATCH THEN SET RESTART_VALUE
X$  RETURN  ! Leave CALL_DSN_HOST ...
X$
X$!`0C
X$&4621
X$&4621
X$&4621
X$_BUILD_MESSAGE:
X$! put FLASH articles together for all (selected) databases
X$  COUNT = 0
X$  TOTAL_NORMAL_ARTICLES = 0
X$  TOTAL_FLASH_ARTICLES = 0
X$  TOTAL_DATABASES = 0
X$  DSNLINK_FLASH = TRUE
X$
X$  _FILE_LOOP_2:
X$  DB_NAME = DB_NAME_'COUNT'
X$  MAIL_CHECK = "," + DB_MAIL_'COUNT' + ","
X$  IF F$LOCATE(",''SELECT_MAIL_GROUP_NAME',",MAIL_CHECK) .EQ. F$LENGTH(MAIL_CH
VECK) THEN GOTO _SKIP_FILE_LOOP_2
X$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT) + "_FLASH"
X$  IF F$SEARCH(DB_DIRFILE) .NES. ""`20
X$  THEN
X$    WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-BUILDMSG, Writing !UL new FLASH ar
Vticle!%S from !AS database", -
X     DB_ARTICLES_FLASH_'COUNT', DB_NAME)
X$    APPEND/NEW_VERSION 'DB_DIRFILE' 'TMP_MAILFILE'
X$    TOTAL_FLASH_ARTICLES = TOTAL_FLASH_ARTICLES + DB_ARTICLES_FLASH_'COUNT'
X$    TOTAL_DATABASES = TOTAL_DATABASES + 1
X$  ENDIF
X$  _SKIP_FILE_LOOP_2:
X$  COUNT = COUNT + 1
X$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _FILE_LOOP_2
X$
X$! put normal (non-FLASH) articles together for all (selected) databases
X$  COUNT = 0
X$  DSNLINK_FLASH = FALSE
X$  _FILE_LOOP_3:
X$  DB_NAME = DB_NAME_'COUNT'
X$  MAIL_CHECK = "," + DB_MAIL_'COUNT' + ","
X$  IF F$LOCATE(",''SELECT_MAIL_GROUP_NAME',",MAIL_CHECK) .EQ. F$LENGTH(MAIL_CH
VECK) THEN GOTO _SKIP_FILE_LOOP_3
X$  DB_DIRFILE = TMP_EXTRACTFILE + "_" + F$STRING(COUNT)
X$  IF F$SEARCH(DB_DIRFILE) .NES. ""`20
X$  THEN
X$    WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-BUILDMSG, Writing !UL new article!
V%S from !AS database", -
X     DB_ARTICLES_NORMAL_'COUNT', DB_NAME)
X$    APPEND/NEW_VERSION 'DB_DIRFILE' 'TMP_MAILFILE'
X$    TOTAL_NORMAL_ARTICLES = TOTAL_NORMAL_ARTICLES + DB_ARTICLES_NORMAL_'COUNT
V'
X$    TOTAL_DATABASES = TOTAL_DATABASES + 1
X$  ENDIF
X$  _SKIP_FILE_LOOP_3:
X$  COUNT = COUNT + 1
X$  IF COUNT .LE. DB_NAME_COUNT THEN GOTO _FILE_LOOP_3
X$
X$  RETURN   ! from _BUILD_MESSAGE
X$
X$!`0C
X$&4621
X$_ELIMINATE_HISTORY_RECORDS:
X$  ACCEPTED_ARTICLE_COUNT = 0
X$  REJECTED_ARTICLE_COUNT = 0
X$  NEW_ARTICLE_FOUND = FALSE
X$  DEBUG_HISTORY_REJECT = FALSE
X$
X$! determine if these should be accepted or rejected based on history
X$  SET FILE/VERSION_LIMIT=3 'DB_DIRFILE'
X$  OPEN/READ DSN_INFILE 'DB_DIRFILE'
X$
X$  CREATE 'DB_DIRFILE';&0F20! create a new version, which contains 'real' stu
Vff
X$  OPEN/APPEND DSN_OUTFILE 'DB_DIRFILE'
X$
X$  IF DSN_HISTORY_FILE .EQS. "NONE"`20
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTDIS, History processing disabled."
X$    GOTO _HISTORY_DISABLED
X$  ENDIF
X$
X$  WRITE SYS$OUTPUT -
X     F$FAO("%DSNlink_NEW-I-CHECKHIST, Checking history for !0UL!1%CFLASH !%E!%
VFdatabase !AS", -
X     DSNLINK_FLASH, DB_NAME)
X$
X$  OPEN/READ/WRITE/SHARE=WRITE HISTORY_FILE 'DSN_HISTORY_FILE'
X$
X$!
X$!
X$!
X$
X$_CHK_HIST_LOOP:
X$  READ/END_OF_FILE=_EXIT_CHK_HIST_LOOP DSN_INFILE ART_TITLE
X$  ARTICLE_TITLE = F$EXTRACT(8,78,ART_TITLE)     ! strip off article numbers
X$  GOSUB _CHECK_EXCLUDE
X$  IF ARTICLE_TITLE .EQS. "" THEN GOTO _CHK_HIST_LOOP  ! excluded
X$  ACCEPTED_ARTICLE_COUNT = ACCEPTED_ARTICLE_COUNT + 1
X$
X$! "flash" history records have database name in lowercase
X$! "normal" history records have database name in uppercase
X$  IF DSNLINK_FLASH
X$  THEN
X$    HISTORY_KEY = F$FAO("!20AS!78AS", F$EDIT(DB_NAME,"LOWERCASE"), ARTICLE_TI
VTLE)
X$  ELSE
X$    HISTORY_KEY = F$FAO("!20AS!78AS", F$EDIT(DB_NAME,"UPCASE"), ARTICLE_TITLE
V)
X$  ENDIF
X$
X$  _HIS_KEY_LOOP:   ! remove all quotes from string
X$  IF F$LOCATE(QUOTE,HISTORY_KEY) .NE. F$LENGTH(HISTORY_KEY)
X$  THEN
X$    HISTORY_KEY`5BF$LOCATE(QUOTE,HISTORY_KEY),1`5D := "`5C"
X$    GOTO _HIS_KEY_LOOP
X$  ENDIF
X$
X$  IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTORYKEY, `5C", HISTORY_KE
VY, "`5C"
X$  READ/KEY=`26HISTORY_KEY/ERROR=_NOT_IN_HIST/noLOCK HISTORY_FILE HIST_RECORD
V
X$
X$!
X$!!! we found the article in history
X$!
X$
X$!!+`20
X$!! backed out so history date reflects the first day we saw the article, not
V
X$!! the most recent day we saw the article.
X$!!
X$!!  ! update history file record with today's date
X$!!  READ/KEY=`26HISTORY_KEY/ERROR=_ERROR HISTORY_FILE HIST_RECORD
X$!!  WRITE/UPDATE HISTORY_FILE F$CVTIME(,"COMPARISON","DATE") + HISTORY_KEY
X$!!-
X$
X$  IF F$EXTRACT(0,10,HIST_RECORD) .GTS. ALLOW_IF_OLDER`20
X$  THEN`20
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTORY, Rejected article `5C", ARTICLE_
VTITLE, "`5C"
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-I-HISTORY, because it was last retrieved a
Vt ", F$EXTRACT(0,10,HIST_RECORD)
X$    IF DEBUG`20
X$    THEN`20
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, Rejected article will be tagged
V and included in mail message"
X$&0620WRITE DSN_OUTFILE F$FAO("!80AS", "Hist/DBG:" + F$EXTRACT(0,10,HIST_RECOR
VD) + ":" + ARTICLE_TITLE)
X$&0620DEBUG_HISTORY_REJECT = TRUE
X$    ENDIF  ! debug
X$    REJECTED_ARTICLE_COUNT = REJECTED_ARTICLE_COUNT + 1
X$    GOTO _CHK_HIST_LOOP
X$  ENDIF  ! already seen this article
X$
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-AGAIN, Article re-appeared from history: "
V, ARTICLE_TITLE
X$  ! update history file record with today's date
X$  READ/KEY=`26HISTORY_KEY/ERROR=_ERROR HISTORY_FILE HIST_RECORD
X$  WRITE/UPDATE HISTORY_FILE F$CVTIME(,"COMPARISON","DATE") + HISTORY_KEY
X$  GOTO _INCLUDE_TITLE
X$
X$_NOT_IN_HIST:&0620! we've never seen it.
X$  IF DEBUG THEN WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-DEBUG, Writing: !/!5*
V !AS!AS", F$CVTIME(,"COMPARISON","DATE"), HISTORY_KEY)
X$  WRITE HISTORY_FILE F$CVTIME(,"COMPARISON","DATE") + HISTORY_KEY
X$
X$_INCLUDE_TITLE:
X$  WRITE DSN_OUTFILE "  ", ARTICLE_TITLE
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTORY, New article: ", ARTICLE_TITLE
X$  NEW_ARTICLE_FOUND = TRUE
X$  GOTO _CHK_HIST_LOOP
X$
X$_EXIT_CHK_HIST_LOOP:
X$  CLOSE DSN_INFILE
X$  CLOSE DSN_OUTFILE
X$  IF F$TRNLNM("HISTORY_FILE") .NES. "" THEN CLOSE HISTORY_FILE
X$
X$  IF NEW_ARTICLE_FOUND .OR. DEBUG_HISTORY_REJECT
X$  THEN
X$    WRITE SYS$OUTPUT F$FAO( -
X     "%DSNlink_NEW-I-SUMMARY, Total of !UL new article!%S in !0UL!1%CFLASH !%E
V!%F!AS, !UL rejected, !UL accepted.", -
X     ACCEPTED_ARTICLE_COUNT, DSNLINK_FLASH, DB_NAME, REJECTED_ARTICLE_COUNT, A
VCCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT)
X$    CREATE 'DB_DIRFILE';     ! create new version that has headers
X$    OPEN/APPEND DSN_OUTFILE 'DB_DIRFILE'
X$    IF DSNLINK_FLASH
X$    THEN
X$&0620WRITE DSN_OUTFILE F$FAO("FLASH !AS database, !UL article!%S:", DB_NAME,
V ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT)
X$&0620DB_ARTICLES_FLASH_'COUNT' = ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_CO
VUNT
X$    ELSE
X$&0620WRITE DSN_OUTFILE F$FAO("!AS database, !UL article!%S:", DB_NAME, ACCEPT
VED_ARTICLE_COUNT - REJECTED_ARTICLE_COUNT)
X$&0620DB_ARTICLES_NORMAL_'COUNT' = ACCEPTED_ARTICLE_COUNT - REJECTED_ARTICLE_C
VOUNT
X$    ENDIF ! flash
X$    CLOSE DSN_OUTFILE
X$    APPEND 'DB_DIRFILE';-1 'DB_DIRFILE'
X$  ELSE
X$    WRITE SYS$OUTPUT F$FAO( -
X     "%DSNlink_NEW-I-SUMMARY, Total of !UL new article!%S in !0UL!1%CFLASH !%E
V!%F!AS, all rejected.", -
X     ACCEPTED_ARTICLE_COUNT, DSNLINK_FLASH, DB_NAME)
X$    DELETE 'DB_DIRFILE';*
X$  ENDIF ! new article found
X$
X$  RETURN  ! from CHECK_DSNLINK_HISTORY
X$
X$_HISTORY_DISABLED:
X$  NEW_ARTICLE_FOUND = TRUE
X$_HISTORY_DISABLED_LOOP:
X$  READ/END_OF_FILE=_EXIT_CHK_HIST_LOOP DSN_INFILE ART_TITLE
X$  ARTICLE_TITLE = F$EXTRACT(8,78,ART_TITLE)   ! strip off article numbers
X$  GOSUB _CHECK_EXCLUDE
X$  IF ARTICLE_TITLE .EQS. "" THEN GOTO _HISTORY_DISABLED_LOOP  ! excluded
X$  WRITE DSN_OUTFILE "  ", ARTICLE_TITLE
X$  ACCEPTED_ARTICLE_COUNT = ACCEPTED_ARTICLE_COUNT + 1
X$  GOTO _HISTORY_DISABLED_LOOP
X$
X$!`0C
X$&4621
X$_CHECK_EXCLUDE:
X$! Determine if the article is one of the articles we're supposed to
X$! exclude from checking.
X$  EXC_COUNT = 0
X$  IF F$TYPE(EXCLUDE_ARTICLE_0) .EQS. "" THEN RETURN  ! if we don't do exclusi
Vons at all....
X$_CHECK_EXCLUDE_LOOP:
X$  IF (EXCLUDE_ARTICLE_'EXC_COUNT' .EQS. F$EDIT(ARTICLE_TITLE,"COMPRESS,TRIM"
V)) .AND. -
X&0620(F$EDIT(DB_NAME,"UPCASE") .EQS. EXCLUDE_DB_'EXC_COUNT' .OR. EXCLUDE_DB_'E
VXC_COUNT' .EQS. "*")
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-EXCLUDED, Article `5C", ARTICLE_TITLE, "
V`5C from database `5C", DB_NAME, "`5C"
X$    TOTAL_EXCLUDED_ARTICLES = TOTAL_EXCLUDED_ARTICLES + 1
X$    ARTICLE_TITLE = ""
X$  ENDIF
X$  EXC_COUNT = EXC_COUNT + 1
X$  IF EXC_COUNT .LE. MAX_EXC_COUNT THEN GOTO _CHECK_EXCLUDE_LOOP
X$  RETURN
X$
X$!`0C
X$&4621
X$_NO_DSNLINK_ARTICLES:
X$! There were no articles retrieved, and no errors (some other code has
X$!  already checked for errors)
X$
X$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$OUTPUT NLA0:
X$  IF .NOT. DEBUG THEN DEFINE/USER_MODE SYS$ERROR NLA0:
X$  SEARCH/EXACT 'TMP_ERRORFILE' "Connection established"
X$  IF ($STATUS .EQ. SS$_NORMAL) .AND. (.NOT. EXCEEDED_RETRY_LIMIT)
X$  THEN   ! good connection, just no articles...
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-NONEW, No new DSNlink articles were foun
Vd."
X$    IF (DSN_MAIL_EMPTY)`20
X$    THEN`20
X$&0620GOTO _GENERATE_MAIL_MESSAGES
X$    ENDIF
X$    FINISHED_OK = TRUE
X$  ELSE   ! never established a connection, or we exceeded retry limit (errors
V after the connection)
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-PROBLEM, Problem connecting to DSNlink h
Vost (", F$TIME(), ")"
X$    CALL _SEND_MAIL_MESSAGE 'TMP_ERRORFILE' "Problem connecting to DSNlink ho
Vst" $ERROR$
X$  ENDIF
X$
X$  GOTO _POST_PROCESSING
X$
X$!`0C
X$&4621
X$_POST_PROCESSING:
X$!  check to see if history file should be auto-compressed, then exit
X$
X$  IF PERFORM_AUTO_COMPRESS`20
X$  THEN
X$    GOSUB _COMPRESS_HISTORY
X$  ENDIF
X$
X$  GOTO _EXIT
X$
X$!`0C
X$&4621
X$_EXIT:
X$  IF FINISHED_OK
X$  THEN`20
X$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-S-OK, DSNlink_NEW completed succe
Vssfully at !%T",0)
X$    EXIT_STATUS = (SS$_NORMAL .OR. STS$M_INHIB_MSG)
X$  ELSE
X$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-E-NOTOK, DSNlink_NEW encountered e
Vrrors.")
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOTOK, No new DSNlink articles were retr
Vieved."
X$    EXIT_STATUS = (SS$_ABORT .OR. STS$M_INHIB_MSG)
X$  ENDIF
X$  GOSUB _DELETE_TMP_FILES
X$_EXIT_NOCLEANUP:
X$  VERIFY = F$VERIFY(VERIFY)
X$  EXIT 'EXIT_STATUS'   ! Leaving DSNlink_NEW ...
X$
X$&4621
X$_EXIT_ERROR:
X$  IF F$TYPE(ERROR_STATUS) .EQS. "" THEN ERROR_STATUS = $STATUS
X$  IF ERROR_MESSAGE .EQS. "" THEN -
X     ERROR_MESSAGE = "Unexpected condition; exact error unknown."
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-ERROR, error occurred; DSNlink_NEW error w
Vas:"
X$  WRITE SYS$OUTPUT "  ", ERROR_MESSAGE
X$  WRITE SYS$OUTPUT ""
X$  WRITE SYS$OUTPUT F$FAO("-DSNlink_NEW-E-ERROR, DCL error was: %X!XL", ERROR_
VSTATUS)
X$  DISPLAY_MESSAGES_OUTPUT = "SYS$OUTPUT"
X$  GOSUB _DISPLAY_MESSAGES`20
X$  WRITE SYS$OUTPUT ""
X$
X$  IF (.NOT. INTERACTIVE) .AND. ("''MAIL_GROUP_$ERROR$'" .NES. "")
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-MAILING, Informing $ERROR$ group of erro
Vr"
X$    SET noON&0720! in case error-handler dies
X$    CLOSE/noLOG DSN_OUTFILE
X$    OPEN/WRITE DSN_OUTFILE 'TMP_MAILFILE'
X$    WRITE DSN_OUTFILE "*** An error caused DSNlink_NEW to fail ***"
X$    WRITE DSN_OUTFILE ""
X$    WRITE DSN_OUTFILE "The error detected by DSNlink_NEW was:"
X$    WRITE DSN_OUTFILE "  ", ERROR_MESSAGE
X$    WRITE DSN_OUTFILE ""
X$    DISPLAY_MESSAGES_OUTPUT = "DSN_OUTFILE"
X$    GOSUB _DISPLAY_MESSAGES`20
X$    CLOSE DSN_OUTFILE
X$    CALL _SEND_MAIL_MESSAGE 'TMP_MAILFILE' "Error in DSNlink_NEW" $ERROR$
X$  ENDIF  ! error group exists, not interactive
X$  GOSUB _DELETE_TMP_FILES
X$  VERIFY = F$VERIFY(VERIFY)
X$  EXIT SS$_ABORT&0620! Leaving DSNlink_NEW (on error status) ...
X$
X$&4621
X$_DISPLAY_MESSAGES:`20
X$  WRITE 'DISPLAY_MESSAGES_OUTPUT' F$FAO("$STATUS = %X!XL", ERROR_STATUS)
X$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
X$  IF F$SEARCH("SYS$MESSAGE:CLIUTLMSG.EXE") .NES. "" THEN SET MESSAGE SYS$MESS
VAGE:CLIUTLMSG.EXE
X$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
X$  IF F$SEARCH("SYS$MESSAGE:SYSMGTMSG.EXE") .NES. "" THEN SET MESSAGE SYS$MESS
VAGE:SYSMGTMSG.EXE
X$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
X$  IF F$SEARCH("SYS$MESSAGE:DSN$MESSAGES.EXE") .NES. "" THEN SET MESSAGE SYS$M
VESSAGE:DSN$MESSAGES.EXE
X$  WRITE 'DISPLAY_MESSAGES_OUTPUT' " = ", F$MESSAGE(ERROR_STATUS)
X$  SET MESSAGE/DELETE
X$  WRITE 'DISPLAY_MESSAGES_OUTPUT' ""
X$  RETURN ! Leaving DISPLAY_MESSAGES
X$
X$&4521
X$_DELETE_TMP_FILES:
X$  IF F$TRNLNM("DSN_OUTFILE") .NES. "" THEN CLOSE DSN_OUTFILE
X$  IF F$TRNLNM("DSN_INFILE") .NES. "" THEN CLOSE DSN_INFILE
X$  IF F$TRNLNM("DSN_CONFIGFILE") .NES. "" THEN CLOSE DSN_CONFIGFILE
X$  IF (F$SEARCH("''TMP_EXTRACTFILE'*",3) .NES. "") .AND. -
X     ("''TMP_EXTRACTFILE'" .NES. "") THEN DELETE 'TMP_EXTRACTFILE'*;*
X$  IF F$SEARCH(TMP_ERRORFILE) .NES. "" THEN DELETE 'TMP_ERRORFILE';*
X$  IF F$SEARCH(TMP_DSNFILE) .NES. "" THEN DELETE 'TMP_DSNFILE';*
X$  IF F$SEARCH(TMP_CHECKFILE) .NES. "" THEN DELETE 'TMP_CHECKFILE';*
X$  IF F$SEARCH(TMP_OUTFILE) .NES. "" THEN DELETE 'TMP_OUTFILE';*
X$  IF F$SEARCH(TMP_MAILFILE) .NES. "" THEN DELETE 'TMP_MAILFILE;*
X$  IF F$SEARCH(TMP_YESFILE) .NES. "" THEN DELETE 'TMP_YESFILE';*
X$  IF F$SEARCH(P3) .NES. "" .AND. PERFORMING_GET THEN DELETE 'F$SEARCH(P3)'
X$  RETURN  ! from DELETE_TMP_FILES
X$
X$&4621
X$_ERROR:
X$  ERROR_STATUS = $STATUS
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-ERROR, Unexpected error occurred in DSNlin
Vk_NEW"
X$  GOTO _EXIT_ERROR
X$
X$&4621
X$_ABORT:
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-ABORT, User aborted DSNlink_NEW"
X$  FINISHED_OK = TRUE
X$  GOTO _EXIT
X$
X$&4621
X$_NO_DSNLINK:
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-E-NOTPRESENT, You do not have DSNlink softwa
Vre on this node"
X$  WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOTPRESENT, Contact your local Digital off
Vice to obtain DSNlink"
X$  FINISHED_OK = TRUE
X$  GOTO _EXIT
X$
X$&4621
X$_NO_CONFIG_FILE:
X$  ERROR_MESSAGE = "%DSNlink_NEW-E-CONFIG, Missing file `5C''DSN_CONFIG_FILENA
VME'`5C"
X$  ERROR_STATUS = SS$_NOSUCHFILE
X$  GOTO _EXIT_ERROR
X$
X$&4621
X$_BAD_P2:
X$  ERROR_MESSAGE = "%DSNlink_NEW-E-INVP2, Bad parameter P2 `5C''P2'`5C"
X$  ERROR_STATUS = SS$_BADPARAM
X$  GOTO _EXIT_ERROR
X$
X$!`0C
X$&4621
X$_DISPLAY_DATABASES: SUBROUTINE
X$! P1 = output file (file must be opened)
X$! P2 = number spaces at beginning of each line
X$! P3 = String to put at front of output text `5Boptional`5D
X$! P4 = mail group name
X$! P5 = additional indentation after first line `5Boptional`5D
X$
X$  FIRST_LINE = TRUE
X$  IF P2 .EQS. "" THEN P2 = 0
X$  P2 = F$INTEGER(P2)
X$  P5 = F$INTEGER(P5)
X$  AVAILABLE_WIDTH = 80 - P2 - 2&0A20! 2 = comma + extra space
X$  DB_NUMBER = 0
X$  OUTPUT_STRING = P3
X$  NEED_COMMA = FALSE
X$
X$! first make sure prefix string isn't too long
X$_CHECK_STR_LENGTH:
X$  IF F$LENGTH(OUTPUT_STRING) .GT. AVAILABLE_WIDTH
X$  THEN
X$    START = 0
X$    END = AVAILABLE_WIDTH
X$    _DISPLAY_PREFIX_LOOP:
X$    CHAR = F$EXTRACT(END,1,OUTPUT_STRING)
X$    IF CHAR .NES. "," .AND. CHAR .NES. " " .AND. CHAR .NES. "."`20
X$    THEN
X$&0620END = END - 1
X$&0620IF END .GT. 0 THEN GOTO _DISPLAY_PREFIX_LOOP
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-DISPdb, Display database error."
X$&0620GOTO _ABORT_DISP_LOOP
X$    ENDIF
X$    WRITE SYS$OUTPUT F$EDIT(F$EXTRACT(START,END+1,OUTPUT_STRING),"TRIM")
X$    OUTPUT_STRING = F$EXTRACT(END+1,-1,OUTPUT_STRING)
X$    GOTO _CHECK_STR_LENGTH
X$  ENDIF
X$
X$  _ABORT_DISP_LOOP:
X$
X$_DISPLAY_DB_LOOP:
X$  IF DB_NUMBER .GT. DB_NAME_COUNT THEN GOTO _DISPLAY_DB_EXIT
X$  MAIL_CHECK = "," + DB_MAIL_'DB_NUMBER' + ","
X$  IF F$LOCATE(",''P4',",MAIL_CHECK) .EQ. F$LENGTH(MAIL_CHECK)
X$  THEN
X$!!!    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-DISPLAY, Not displaying
V ", DB_NAME_'DB_NUMBER', " for ", P4
X$    GOTO _DISPLAY_DB_NEXT
X$  ENDIF  ! not for this group
X$  ADDITIONAL_STRING = DB_NAME_'DB_NUMBER'
X$  IF F$LENGTH(OUTPUT_STRING + "," + ADDITIONAL_STRING) .GE. AVAILABLE_WIDTH
X$  THEN  ! not enough room to all fit on this line
X$    IF NEED_COMMA`20
X$    THEN
X$&0620WRITE 'P1' F$FAO("!#* !AS,", P2, OUTPUT_STRING)
X$    ELSE
X$&0620WRITE 'P1' F$FAO("!#* !AS", P2, OUTPUT_STRING)
X$    ENDIF
X$    IF FIRST_LINE`20
X$    THEN
X$&0620FIRST_LINE = FALSE
X$&0620P2 = P2 + P5
X$&0620AVAILABLE_WIDTH = AVAILABLE_WIDTH - P5
X$    ENDIF
X$    OUTPUT_STRING = ADDITIONAL_STRING
X$    NEED_COMMA = TRUE
X$  ELSE ! yes, enough room to fit on this line
X$    IF OUTPUT_STRING .EQS. ""
X$    THEN
X$&0620OUTPUT_STRING = ADDITIONAL_STRING
X$&0620NEED_COMMA = TRUE
X$    ELSE
X$&0620IF NEED_COMMA`20
X$&0620THEN`20
X$&0820OUTPUT_STRING = OUTPUT_STRING + "," + ADDITIONAL_STRING
X$&0620ELSE
X$&0820OUTPUT_STRING = OUTPUT_STRING + ADDITIONAL_STRING
X$&0820NEED_COMMA = TRUE
X$&0620ENDIF ! need_comma
X$    ENDIF ! output_string .eqs. ""
X$  ENDIF ! enough room to fit on this line?
X$_DISPLAY_DB_NEXT:
X$  DB_NUMBER = DB_NUMBER + 1
X$  GOTO _DISPLAY_DB_LOOP
X$
X$_DISPLAY_DB_EXIT:
X$  WRITE 'P1' F$FAO("!#* !AS", P2, OUTPUT_STRING)
X$  EXIT&0B20! display_databases
X$  ENDSUBROUTINE  ! display_databases
X$
X$!`0C
X$&4621
X$_SEND_MAIL_MESSAGE: SUBROUTINE
X$! P1 = append information to this file, then Email it
X$! P2 = Subject to use for Mail message
X$! P3 = name of mail group we're sending to...`20
X$!&0620if there is an equal sign with text afterwards, the text after
X$!&0620the equal sign is the username(s) we're going to send to.
X$! P4 = Personal name (if empty, P2 is used)
X$
X$  IF P4 .EQS. "" THEN P4 = P2
X$
X$  GRPNAME = F$ELEMENT(0,"=",P3)
X$
X$  MAIL_USERS = "<none>"
X$  MAIL_GROUPS = "<unknown>"
X$
X$  IF GRPNAME .EQS. "Retain"`20
X$  THEN
X$    COUNT = 0
X$    _RETAIN_LOOP:
X$    IF F$EDIT(DATABASE,"UPCASE") .EQS. F$EDIT(DB_NAME_'COUNT',"UPCASE")
X$    THEN&1F20! found the database
X$&0620MAIL_GROUPS = DB_MAIL_'COUNT'    ! list of mail groups (Programmers, Sys
Vtems)
X$&0620OFFSET = 0
X$&0620_RETAIN_LOOP_2:&1220! now get list of users for the group
X$&0620ELEMENT = F$ELEMENT(OFFSET,",",MAIL_GROUPS)`20
X$&0620IF ELEMENT .EQS. "," THEN GOTO _DONE_RETAIN
X$&0620GROUP_NUMBER = MAIL_GROUP_'ELEMENT'
X$&0620IF MAIL_USERS .EQS. "<none>"`20
X$&0620THEN`20
X$&0920MAIL_USERS = MAIL_GROUP_USERS_'GROUP_NUMBER'
X$&0620ELSE
X$&0920MAIL_USERS = MAIL_USERS + "," + MAIL_GROUP_USERS_'GROUP_NUMBER'
X$&0620ENDIF
X$&0620OFFSET = OFFSET + 1
X$&0620GOTO _RETAIN_LOOP_2
X$    ENDIF ! if database = db_name_'count'
X$    COUNT = COUNT + 1
X$    IF COUNT .LE. DB_NAME_COUNT THEN GOTO _RETAIN_LOOP  ! check next database
V
X$  ELSE ! retain
X$    MAIL_NUMBER = MAIL_GROUP_'GRPNAME'
X$    MAIL_USERS  = MAIL_GROUP_USERS_'MAIL_NUMBER'
X$  ENDIF ! retain
X$
X$  IF F$ELEMENT(1,"=",P3) .NES. "="
X$  THEN
X$    MAIL_USERS = F$ELEMENT(1,"=",P3)
X$  ENDIF
X$
X$_DONE_RETAIN:
X$
X$
X$  IF F$TRNLNM("DSN_OUTFILE") .NES. "" THEN CLOSE DSN_OUTFILE
X$  OPEN/APPEND DSN_OUTFILE 'P1'
X$  WRITE DSN_OUTFILE "_____"
X$  WRITE DSN_OUTFILE ""
X$
X$  WRITE DSN_OUTFILE F$FAO("Message sent by DSNlink_NEW version !AS on node !A
VS, using:", DSNLINK_NEW_VERSION, F$GETSYI("NODENAME"))
X$  WRITE DSN_OUTFILE F$FAO("  !14<Config file:!>!AS", DSN_CONFIG_FILENAME)
X$  IF PERFORMING_GET`20
X$  THEN`20
X$    WRITE DSN_OUTFILE "  Performing GET processing"
X$    IF F$TYPE(GET_FILENAME) .NES. ""
X$    THEN
X$&0620WRITE DSN_OUTFILE F$FAO("  !14<Get file:!>!AS", GET_FILENAME)
X$    ELSE
X$&0620WRITE DSN_OUTFILE F$FAO("  !14<Get file:!><unknown>"
X$    ENDIF
X$  ENDIF ! performing_get
X$  TMP_GROUP_COUNT = MAIL_GROUP_'GRPNAME'
X$  WRITE DSN_OUTFILE F$FAO("  !14<Mail group:!>!AS", MAIL_GROUP_NAME_'TMP_GROU
VP_COUNT')
X$  IF F$TYPE(ORIGINALLY_FROM) .NES. "" THEN -
X     IF ORIGINALLY_FROM .NES. USERNAME THEN -
X     WRITE DSN_OUTFILE F$FAO("  !14<Sent by:!>!AS", ORIGINALLY_FROM)
X$  IF PERFORMING_GET .AND. MAIL_GROUPS .NES. "<unknown>" THEN WRITE DSN_OUTFIL
VE F$FAO("!15* = !AS", MAIL_GROUPS)  ! for RETAIN
X$
X$  IF MAIL_USERS .EQS. "<none>"
X$  THEN
X$    MAIL_USERS = USERNAME
X$    WRITE DSN_OUTFILE "%DSNlink_NEW-E-MAILUSERS, Undefined mail users for thi
Vs database; using SELF."
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-MAILUSERS, Undefined mail users for this
V database."
X$  ENDIF
X$   `20
X$!
X$
X$  IF .NOT. PERFORMING_GET
X$  THEN
X$    IF GRPNAME .NES. "$ERROR$" THEN -
X&0720CALL _DISPLAY_DATABASES DSN_OUTFILE 2 "Databases:    " "''GRPNAME'" 2
X$ `20
X$    WRITE DSN_OUTFILE F$FAO("  !14<Since:!>!AS (!AS, !AS)", -
X&0720DSN_SINCE, -
X&0720F$CVTIME(DSN_SINCE,,"WEEKDAY"), -
X&0720F$CVTIME(DSN_SINCE,"ABSOLUTE","DATE"))
X$    IF DSN_HISTORY_FILE .NES. "NONE"
X$    THEN
X$&0620WRITE DSN_OUTFILE F$EXTRACT(0,79,F$FAO("  !14<History:!>!AS  (!UL blocks
V)", -
X&0A20DSN_HISTORY_FILE, F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF")))
X$    ELSE
X$&0620WRITE DSN_OUTFILE F$FAO("  !14<History:!>NONE")
X$    ENDIF ! history enabled/disabled
X$  ENDIF ! performing GET
X$
X$  IF BATCH .AND. GRPNAME .EQS. "$ERROR$"`20
X$  THEN`20
X$    WRITE DSN_OUTFILE F$FAO("  !14<Log file:!>!AS", F$PARSE(F$GETQUI("DISPLAY
V_JOB","LOG_SPECIFICATION",,"THIS_JOB"),LOG_FILE))
X$    IF "''ORIGINAL_GET_FILE'" .NES. "" THEN -
X     WRITE DSN_OUTFILE F$FAO("  !14<Orig GET file:!>!AS", ORIGINAL_GET_FILE)
X$  ENDIF
X$
X$  IF EXCEEDED_RETRY_LIMIT .AND. (.NOT. RECEIVED_FATAL_ERROR)
X$  THEN`20
X$    WRITE DSN_OUTFILE ""
X$    WRITE DSN_OUTFILE "%DSNlink_NEW-E-EXRETRY, Exceeded retry limit"
X$    WRITE DSN_OUTFILE ""
X$  ENDIF
X$  IF RECEIVED_FATAL_ERROR
X$  THEN
X$    WRITE DSN_OUTFILE ""
X$    WRITE DSN_OUTFILE "%DSNlink_NEW-E-NORETRY, Received fatal error - abortin
Vg retry attempts."
X$    WRITE DSN_OUTFILE ""
X$  ENDIF
X$
X$  IF DSN_ATTEMPT_COUNT .GT. 0`20
X$  THEN
X$    IF MAX_EXC_COUNT .GT. 0
X$    THEN
X$&0620WRITE DSN_OUTFILE F$FAO("Total connect time: !8AS, !UL reconnection!%S.
V  !UL total articles excluded.", -
X&0720TOTAL_ELAPSED_CONN_TIME, DSN_RETRY_COUNT, TOTAL_EXCLUDED_ARTICLES)
X$    ELSE
X$&0620WRITE DSN_OUTFILE F$FAO("Total connect time: !8AS, !UL reconnection!%S."
V, -
X&0720TOTAL_ELAPSED_CONN_TIME, DSN_RETRY_COUNT)
X$    ENDIF
X$  ENDIF
X$  IF PERFORM_AUTO_COMPRESS THEN WRITE DSN_OUTFILE "History file auto-compress
Vion was triggered (history file > ", DSN_HISTORY_AUTO_COMPRESS, " blocks)."
X$
X$  CLOSE DSN_OUTFILE
X$
X$  IF DEBUG`20
X$  THEN`20
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Mail message about to be sent:"
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, +-+ begin mail message +-+"
X$    TYPE 'P1'
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, -+- end mail message -+-"
X$  ENDIF
X$
X$  IF F$SEARCH(TMP_YESFILE) .EQS. ""
X$  THEN
X$    ! create file to 'feed' Mail in case a DECnet link is down -- this will
X$    ! answer Mail's question "
X$    OPEN/WRITE FILE 'TMP_YESFILE'
X$    WRITE FILE "Y"
X$    CLOSE FILE
X$  ENDIF
X$ `20
X$  SET noON
X$  DEFINE/USER_MODE SYS$INPUT 'TMP_YESFILE'
X$  MAIL -
X     'P1' -
X     "''MAIL_USERS'" -
X     /SUBJECT="''P2'" -
X     /PERSONAL_NAME="''P4'" -
X     /noSELF`20
X$  STATUS = $STATUS
X$  SET ON
X$
X$  IF STATUS`20
X$  THEN`20
X$    WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-SENTMAIL, Message sent to user!0UL
V!%S !AS (!%T)", -
X     (F$LENGTH(MAIL_USERS) - F$LOCATE(",",MAIL_USERS) + 1), -
X     MAIL_USERS, 0)
X$    WRITE SYS$OUTPUT F$FAO("-DSNlink_NEW-I-SENTMAIL, Mail subject: `5C!AS`5C"
V, P2)
X$  ENDIF
X$ `20
X$ `20
X$  EXIT&0B20! SEND_MAIL_MESSAGE
X$  ENDSUBROUTINE  ! SEND_MAIL_MESSAGE
X$
X$!`0C
X$&4621
X$_COMPRESS_HISTORY:
X$  IF DSN_HISTORY_FILE .EQS. "NONE"
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-W-COMPRESS, History is disabled in your co
Vnfiguration file, so"
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-W-COMPRESS, no compression will be perform
Ved."
X$    RETURN  ! from COMPRESS_HISTORY
X$  ENDIF
X$
X$  WRITE SYS$OUTPUT F$FAO("%DSNlink_NEW-I-COMPRESS, Beginning to compress hist
Vory file (size=!UL)", -
X     F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF"))
X$  WRITE SYS$OUTPUT "%DSNlink_NEW-I-HISTORY, File ", F$SEARCH(DSN_HISTORY_FILE
V)
X$
X$  TMP_SPEC_FILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_SPEC_" + F$GETJPI(0,"PID") +
V ".TMP"
X$  TMP_HIST_FILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_HISTORY_" + F$GETJPI(0,"PID"
V) + ".TMP"
X$  TMP_FDL_FILE  = DSN_SCRATCH_DIR + "DSNLINK_NEW_FDL_" + F$GETJPI(0,"PID") +
V ".TMP"
X$
X$  OPEN/WRITE FILE 'TMP_SPEC_FILE'
X$  WRITE FILE "/FIELD=(NAME=HISTORY_DATE,POSITION:1,SIZE:10)"
X$  WRITE FILE "/FIELD=(NAME=DATABASE,POSITION:10,SIZE:20)"
X$  WRITE FILE "/FIELD=(NAME=TITLE,POSITION:30,SIZE:78)"
X$  WRITE FILE "/FIELD=(NAME=DATABASE_KEY,POSITION:10,SIZE:98)"
X$  WRITE FILE "/KEY=(DATABASE_KEY)"
X$  WRITE FILE "/CONDITION=(NAME=KEEP_RECORD,TEST=(HISTORY_DATE GE ", QUOTE, A
VLLOW_IF_OLDER, QUOTE, "))"
X$  WRITE FILE "/INCLUDE=(CONDITION=KEEP_RECORD)"
X$  CLOSE FILE
X$
X$  ANALYZE/RMS/FDL/OUTPUT='TMP_FDL_FILE' 'DSN_HISTORY_FILE'
X$  SET FILE/VERSION_LIMIT=2 'TMP_FDL_FILE'
X$  EDIT/FDL/noINTERACTIVE/ANALYZE='TMP_FDL_FILE' 'TMP_FDL_FILE'
X$  CREATE/FDL='TMP_FDL_FILE' 'TMP_HIST_FILE'
X$
X$  MERGE/SPECIFICATION='TMP_SPEC_FILE'/STATISTICS/STABLE/noCHECK_SEQUENCE -
X     'DSN_HISTORY_FILE' -
X     'TMP_HIST_FILE'/OVERLAY
X$
X$  COPY 'TMP_HIST_FILE' 'DSN_HISTORY_FILE'
X$
X$  DELETE 'TMP_SPEC_FILE';,'TMP_FDL_FILE';*,'TMP_HIST_FILE';
X$  PURGE 'DSN_HISTORY_FILE'
X$  RENAME 'DSN_HISTORY_FILE' 'DSN_HISTORY_FILE';1
X$
X$  WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-S-COMPRESS, Compression has complete
Vd normally (size=!UL)", -
X     F$FILE_ATTRIBUTES(DSN_HISTORY_FILE,"EOF"))
X$
X$  FINISHED_OK = TRUE
X$  RETURN  ! from COMPRESS_HISTORY
X$
X$!`0C
X$&4621
X$_INIT_DEFAULTS:
X$
X$  TRUE  = 1
X$  FALSE = 0
X$
X$  FINISHED_OK = FALSE  ! indicate we failed in case we error out prematurely
V
X$
X$  USERNAME = F$EDIT(F$GETJPI(0,"USERNAME"),"TRIM")
X$
X$!!!
X$
X$  PERFORMING_GET = FALSE
X$  PRIB = F$GETJPI(0,"PRIB")
X$
X$  IF F$TRNLNM("DSNLINK_NEW_DEBUG")`20
X$  THEN
X$    DEBUG = TRUE
X$    DEBUG_LOG = "/LOG"
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, DSNlink_NEW debugging enabled."
X$  ELSE
X$    DEBUG = FALSE
X$    DEBUG_LOG = ""
X$  ENDIF
X$
X$  DSNLINK_DIRECTORY = F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,"DEVICE") + -
X     F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,"DIRECTORY")
X$  IF F$TRNLNM("DSNLINK_NEW_DIR") .NES. "" THEN -
X     DSNLINK_DIRECTORY = F$TRNLNM("DSNLINK_NEW_DIR")
X$
X$  DSN_RETRY_MAX = 5
X$  IF F$TRNLNM("DSNLINK_NEW_RETRY") .NES. "" THEN -
X     DSN_RETRY_MAX = F$TRNLNM("DSNLINK_NEW_RETRY")
X$
X$  DSN_SINCE = "TODAY-10-00:00"
X$  IF F$TRNLNM("DSNLINK_NEW_SINCE") .NES. "" THEN -
X     DSN_SINCE = F$TRNLNM("DSNLINK_NEW_SINCE")
X$
X$  DSN_MAIL_EMPTY = FALSE
X$  IF F$TRNLNM("DSNLINK_NEW_MAIL_EMPTY") .NES. "" THEN -
X     DSN_MAIL_EMPTY = F$TRNLNM("DSNLINK_NEW_MAIL_EMPTY")
X$
X$  DSN_AFTER = "TOMORROW+01:00:00"
X$  IF F$TRNLNM("DSNLINK_NEW_AFTER") .NES. "" THEN -
X     DSN_AFTER = F$TRNLNM("DSNLINK_NEW_AFTER")
X$
X$  DSN_RETRY_WAIT_TIME = "00:00:30"  ! 30 seconds
X$  IF F$TRNLNM("DSNLINK_NEW_WAIT_TIME") .NES. "" THEN -
X     DSN_RETRY_WAIT_TIME = F$TRNLNM("DSNLINK_NEW_WAIT_TIME")
X$
X$  DSN_HISTORY_FILE = "''DSNLINK_DIRECTORY'DSNLINK_NEW_HISTORY.DAT"
X$  IF F$TRNLNM("DSNLINK_NEW_HISTORY") .NES. "" THEN -
X     DSN_HISTORY_FILE = F$PARSE(F$TRNLNM("DSNLINK_NEW_HISTORY"),DSN_HISTORY_FI
VLE)
X$
X$  DSN_HISTORY_AUTO_COMPRESS = 600
X$  IF F$TRNLNM("DSNLINK_NEW_AUTO_COMPRESS") .NES. "" THEN -
X     DSN_HISTORY_AUTO_COMPRESS = F$INTEGER(F$TRNLNM("DSNLINK_NEW_AUTO_COMPRESS
V"))
X$
X$  DSN_CHECK_OPENCALLS = "Never"
X$  IF F$TRNLNM("DSNLINK_NEW_CHECK_OPENCALLS") .NES. "" THEN -
X     DSN_CHECK_OPENCALLS = F$TRNLNM("DSNLINK_NEW_CHECK_OPENCALLS")
X$  DSN_CHECK_OPENCALLS_USER = USERNAME
X$
X$  DSN_CHECK_CLOSEDCALLS = "Never"
X$  IF F$TRNLNM("DSNLINK_NEW_CHECK_CLOSEDCALLS") .NES. "" THEN -
X     DSN_CHECK_CLOSEDCALLS = F$TRNLNM("DSNLINK_NEW_CHECK_CLOSEDCALLS")
X$  DSN_CHECK_CLOSEDCALLS_USER = USERNAME
X$
X$  DSN_CONFIG_FILENAME = F$PARSE(P1,F$TRNLNM("DSNLINK_NEW_CONFIG"), -
X     "''DSNLINK_DIRECTORY'DSNLINK_NEW_CONFIG.DAT")
X$
X$  LOG_FILE = "''DSNLINK_DIRECTORY'DSNLINK_NEW.LOG"
X$
X$  GET_FILE = "SYS$LOGIN:DSNLINK_GET.DAT"
X$  IF F$TRNLNM("DSNLINK_NEW_GET_FILE") .NES. "" THEN -
X     GET_FILE = F$PARSE(F$TRNLNM("DSNLINK_NEW_GET_FILE"),GET_FILE)
X$
X$  GET_LOG_FILE = "SYS$LOGIN:DSNLINK_NEW_GET.LOG"
X$  IF F$TRNLNM("DSNLINK_NEW_GET_LOG_FILE") .NES. "" THEN -
X     GET_LOG_FILE = F$PARSE(F$TRNLNM("DSNLINK_NEW_GET_LOG_FILE"),GET_LOG_FILE)
V
X$
X$  LOW_PRIORITY = PRIB
X$  IF F$TRNLNM("DSNLINK_NEW_BATCH_PRIORITY") .NES. "" THEN -
X     LOW_PRIORITY = F$INTEGER(F$TRNLNM("DSNLINK_NEW_BATCH_PRIORITY"))
X$
X$  DSN_SCRATCH_DIR = F$PARSE("DSN$SCRATCH:") - ".;"
X$  IF DSN_SCRATCH_DIR .EQS. "" THEN DSN_SCRATCH_DIR = F$PARSE("SYS$SCRATCH:")
V - ".;"
X$  IF DSN_SCRATCH_DIR .EQS. "" THEN DSN_SCRATCH_DIR = F$PARSE("SYS$LOGIN:") -
V ".;"
X$  IF DSN_SCRATCH_DIR .EQS. "" THEN DSN_SCRATCH_DIR = DSNLINK_DIRECTORY
X$  IF F$TRNLNM("DSNLINK_NEW_SCRATCH_DIR") .NES. "" THEN -
X     DSN_SCRATCH_DIR = F$PARSE(F$TRNLNM("DSNLINK_NEW_SCRATCH_DIR")) - ".;"
X$
X$! undocumented symbol DSN$VERSION (only works with DSNlink V1.2 and above)
X$  DSN$VERSION = "V1.1"&0720! assume we're running V1.1 unless told otherwise
V
X$
X$! disable error handling in case the DSN command doesn't exist (we haven't`20
X$! tested for SYS$SYSTEM:DSN$MAIN.EXE yet), and also disable error handling
X$! in case DSN SHOW VERSION doesn't work (it is apparently undocumented)
X$  SET noON&0920
X$  DEFINE/USER_MODE SYS$OUTPUT NLA0:
X$  DEFINE/USER_MODE SYS$ERROR NLA0:
X$  DSN SHOW VERSION
X$  SET ON
X$  IF F$TRNLNM("SYS$OUTPUT","LNM$PROCESS_TABLE",,,,"ACCESS_MODE") .EQS. "USER"
V THEN DEASSIGN/USER_MODE SYS$OUTPUT
X$  IF F$TRNLNM("SYS$ERROR","LNM$PROCESS_TABLE",,,,"ACCESS_MODE") .EQS. "USER"
V THEN DEASSIGN/USER_MODE SYS$ERROR
X$
X$! determine the get-style we'll use.  If it is an old version of DSNlink
X$! (less than V1.2) we have to use EXTRACT:  DSNlink V1.0 (which isn't used
X$! anywhere as far as I know) doesn't allow READ/EDIT, and DSNlink V1.1 and
X$! V1.1-1 have a bug preventing READ/EDIT from working with EDT as the
X$! editor (which is how we're doing GET Processing with DSNlink_NEW).
X$! V1.2A also has the problem
X$  GET_STYLE = "EDT"
X$  IF F$EXTRACT(1,-1,DSN$VERSION) .LTS. "1.2" THEN GET_STYLE = "EXTRACT"
X$  IF F$EXTRACT(1,-1,DSN$VERSION) .EQS. "1.2A" THEN GET_STYLE = "EXTRACT"
X$  WRITE_TITLES = ""  ! null filename = don't write titles to file
X$
X$  FAO_SUBJECT = "!+!+!UL new !AS DSNlink article!%S in !UL database!%S"
X$  FAO_PERSONAL = "!+!+DSNlink article!0UL!%S, !AS"
X$  FAO_GET_SUBJECT = "!AS: !AS"
X$  FAO_GET_PERSONAL = "!AS database article"
X$
X$  QUOTE`5B0,8`5D = 34
X$  EXCEEDED_RETRY_LIMIT = FALSE
X$  RECEIVED_FATAL_ERROR = FALSE
X$  ELAPSED_CONN_TIME = "00:00:00"
X$  TOTAL_ELAPSED_CONN_TIME = "00:00:00"
X$  PERFORM_AUTO_COMPRESS = FALSE
X$  DSN_RETRY_COUNT = 0
X$  DSN_ATTEMPT_COUNT = 0
X$
X$  ERROR_MESSAGE = "%DSNlink_NEW-?-UNK, Unknown/unexpected error.  Please inve
Vstigate"
X$  EXCLUDE_WARNING = TRUE
X$  TOTAL_EXCLUDED_ARTICLES = 0
X$
X$  TMP_MAILFILE    = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_
V1.TMP"
X$  TMP_ERRORFILE   = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_
V2.TMP"
X$  TMP_DSNFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_
V3.TMP"
X$  TMP_CHECKFILE   = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_
V4.TMP"
X$  TMP_OUTFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_
V5.TMP"
X$  TMP_EXTRACTFILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_
V6.TMP"
X$  TMP_YESFILE     = DSN_SCRATCH_DIR + "DSNLINK_NEW_" + F$GETJPI(0,"PID") + "_
V7.TMP"
X$
X$  INTERACTIVE = F$MODE() .EQS. "INTERACTIVE"
X$  BATCH = F$MODE() .EQS. "BATCH"
X$  ASK := READ SYS$COMMAND/END_OF_FILE=_ABORT/PROMPT=
X$
X$  SS$_NORMAL&0620= %X&07301
X$  SS$_ABORT&0720= %X&06302C
X$  SS$_NOSUCHFILE  = %X00000912
X$  SS$_BADPARAM    = %X&063012
X$  STS$M_INHIB_MSG = %X1&0730
X$
X$  TOO_LONG_CHAR  = "`7E"&0720! used by GET Processing on lines that are too l
Vong for VMSmail Subject and Personal lines.
X$
X$  RETURN&0620! leaving INIT_DEFAULTS
X$
X$!`0C
X$&4621
X$_INIT_PARSE_CONFIG_FILE:
X$
X$  IF BATCH THEN SET RESTART_VALUE="Reading configuration file"
X$  DSN_CONFIG_FILENAME = F$SEARCH(DSN_CONFIG_FILENAME)
X$  WRITE SYS$OUTPUT F$FAO("  !24<Configuration file:!>!AS", DSN_CONFIG_FILENAM
VE)
X$  OPEN/READ/ERROR=_ERROR DSN_CONFIGFILE 'DSN_CONFIG_FILENAME'
X$  DB_NAME_COUNT = 0
X$  MAIL_GROUP_COUNT = 0
X$  EXC_COUNT = 0
X$  CONFIG_ERROR = FALSE
X$  LINE_NUMBER = 0
X$ `20
X$_PARSE_CONFIG_LOOP:
X$  LINE_NUMBER = LINE_NUMBER + 1
X$  READ/ERROR=_ERROR/END_OF_FILE=_END_PARSE_CONFIG DSN_CONFIGFILE ORIG_RECORD
V
X$  RECORD = F$EDIT(ORIG_RECORD,"COMPRESS,TRIM,UNCOMMENT")
X$  IF RECORD .EQS. "" THEN GOTO _PARSE_CONFIG_LOOP
X$  VALID_RECORD = FALSE
X$
X$  PARM1 = F$EDIT(F$ELEMENT(0," ",RECORD),"TRIM")
X$  PARM2 = F$EDIT(F$ELEMENT(1," ",RECORD),"TRIM")
X$  PARM3 = F$EDIT(F$ELEMENT(2," ",RECORD),"TRIM")
X$  PARM1_UP = F$EDIT(PARM1,"UPCASE")
X$  PARM2_UP = F$EDIT(PARM2,"UPCASE")
X$
X$  IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-READ, Record `5C", RECORD, "
V`5C"
X$
X$!
X$!!!
X$!
X$
X$  IF F$EXTRACT(0,1,PARM1) .NES. "."
X$  THEN  ! it's a database
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Database: `5C", -
V
X     PARM1, "`5C", ", Group: `5C", PARM3, "`5C, Flash: ", PARM2 .EQS. "FLASH"
V
X$    PARM2 = F$EDIT(PARM2,"UPCASE")
X$    IF PARM2 .NES. "FLASH" .AND. PARM2 .NES. "NOFLASH"
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Database ", PARM1, " missing FL
VASH/NOFLASH setting"
X$&0620CONFIG_ERROR = TRUE
X$    ENDIF ! if parm2 <> flash and parm2 <> noflash
X$    DB_NAME_'DB_NAME_COUNT' = PARM1
X$    DB_FLASH_'DB_NAME_COUNT' = PARM2 .EQS. "FLASH"
X$    DB_MAIL_'DB_NAME_COUNT' = PARM3
X$    COUNT = 0
X$    _CONFIG_DB_LOOP:
X$    CHECK_MAIL = F$ELEMENT(COUNT,",",PARM3)
X$    IF CHECK_MAIL .EQS. "," .OR. CHECK_MAIL .EQS. " " THEN GOTO _END_CONFIG_D
VB_LOOP
X$    IF F$TYPE(MAIL_GROUP_'CHECK_MAIL') .EQS. ""
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Database ", PARM1, " uses undef
Vined mail group `5C", CHECK_MAIL, "`5C"
X$&0620CONFIG_ERROR = TRUE
X$    ELSE
X$&0620TMP_COUNT = MAIL_GROUP_'CHECK_MAIL'
X$&0620IF MAIL_GROUP_NAME_'TMP_COUNT' .NES. CHECK_MAIL
X$&0620THEN
X$&0820WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Database ", PARM1, " has incorr
Vect case for mail group ", CHECK_MAIL
X$&0820WRITE SYS$OUTPUT "-DSNlink_NEW-E-CONFIG, Case of `5C", CHECK_MAIL, "`5C
V is different from `5C", MAIL_GROUP_NAME_'TMP_COUNT', "`5C"
X$&0820CONFIG_ERROR = TRUE
X$&0620ENDIF
X$    ENDIF
X$    COUNT = COUNT + 1
X$    GOTO _CONFIG_DB_LOOP
X$    _END_CONFIG_DB_LOOP:
X$    DB_NAME_COUNT = DB_NAME_COUNT + 1
X$    GOTO _PARSE_CONFIG_LOOP&1120! to speed up parsing
X$  ENDIF  ! extract <> "."
X$
X$  IF PARM1_UP .EQS. ".MAIL"
X$  THEN
X$    IF DEBUG`20
X$    THEN`20
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Mail group name: `5C", PARM2, "
V`5C, Users: `5C", PARM3, "`5C"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, assigned to mail group number "
V, MAIL_GROUP_COUNT
X$    ENDIF ! debug
X$    IF F$TYPE(MAIL_GROUP_'PARM2') .NES. ""
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Multiple definition of mail gro
Vup `5C", PARM2, "`5C"
X$&0620CONFIG_ERROR = TRUE
X$    ENDIF
X$    IF PARM2_UP .EQS. "SELF" .OR. PARM2_UP .EQS. "RETAIN" .OR. PARM2_UP .EQS.
V "SPECIFIC"
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, SELF, RETAIN, and SPECIFIC are
V illegal mail groups."
X$&0620CONFIG_ERROR = TRUE
X$    ENDIF
X$    MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = PARM2
X$    MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = PARM3
X$    MAIL_GROUP_'PARM2' = MAIL_GROUP_COUNT
X$    MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1
X$    GOTO _PARSE_CONFIG_LOOP&1120! to speed up parsing
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".RETRY"
X$  THEN
X$    DSN_RETRY_MAX = F$INTEGER(PARM2)
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Retry set to ", DS
VN_RETRY_MAX
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".LOG_FILE"
X$  THEN
X$    LOG_FILE = F$PARSE(PARM2,LOG_FILE)
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, LOG file set to ",
V LOG_FILE
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".GET_FILE"
X$  THEN
X$    GET_FILE = F$PARSE(PARM2,GET_FILE)
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET file set to ",
V GET_FILE
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".GET_LOG_FILE"
X$  THEN
X$    GET_LOG_FILE = F$PARSE(PARM2,GET_LOG_FILE)
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET log file set t
Vo ", GET_LOG_FILE
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".SCRATCH_DIR"
X$  THEN
X$    DSN_SCRATCH_DIR = PARM2_UP
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, SCRATCH directory
V set to ", DSN_SCRATCH_DIR
X$    VALID_RECORD = TRUE  ! until one of the tests fail
X$    IF F$PARSE(DSN_SCRATCH_DIR) .EQS. ""
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, .SCRATCH_DIR isn't a valid dire
Vctory"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-E-CONFIG, .SCRATCH_DIR value is: `5C''PAR
VM2_UP'`5C"
X$&0620VALID_RECORD = FALSE
X$    ENDIF
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".SINCE"
X$  THEN
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Since set to `5C",
V PARM2, "`5C"
X$    DSN_SINCE = PARM2
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".MAIL_EMPTY"
X$  THEN
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Mail emptry set to
V `5C", PARM2, "`5C"
X$    DSN_MAIL_EMPTY = PARM2
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".WRITE_TITLES"
X$  THEN
X$    WRITE_TITLES = F$PARSE(PARM2,"SYS$LOGIN:DSNLINK_GET.DAT")
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Titles will be wri
Vtten to `5C", WRITE_TITLES, "`5C"
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".NEXT_EXECUTION"
X$  THEN
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Next execution set
V to `5C", PARM2, "`5C"
X$    DSN_AFTER = PARM2
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".HISTORY_FILE"
X$  THEN
X$    DSN_HISTORY_FILE = F$PARSE(PARM2,"''DSNLINK_DIRECTORY'DSNLINK_NEW_HISTORY
V.DAT") - ";"
X$    IF F$EDIT(PARM2,"UPCASE") .EQS. "NONE" THEN DSN_HISTORY_FILE = "NONE"
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, History file set t
Vo `5C", DSN_HISTORY_FILE, "`5C"
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".RETRY_WAIT_TIME"
X$  THEN
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Retry wait time se
Vt to `5C", PARM2, "`5C"
X$    DSN_RETRY_WAIT_TIME = PARM2
X$    VALID_RECORD = TRUE
X$  ENDIF
X$ `20
X$  IF PARM1_UP .EQS. ".HISTORY_AUTO_COMPRESS"
X$  THEN
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, History auto compr
Vess set to `5C", PARM2, "`5C"
X$    DSN_HISTORY_AUTO_COMPRESS = F$INTEGER(PARM2)
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".CHECK_OPENCALLS"
X$  THEN
X$    IF PARM3 .EQS. "" THEN PARM3 = USERNAME
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Check opencalls:`20
V`5C", PARM2, "`5C, user: `5C", PARM3, "`5C"
X$    DSN_CHECK_OPENCALLS = PARM2
X$    ! now validate it
X$    COUNT = 0
X$    VALID_RECORD = TRUE    ! assume the best unless proven otherwise
X$    _CONFIG_OPENCALLS_LOOP:
X$    CHECK = F$ELEMENT(COUNT,",",DSN_CHECK_OPENCALLS)
X$    IF CHECK .EQS. "," THEN GOTO _EXIT_CFG_OPENCALLS
X$    IF F$TYPE(CHECK) .EQS. "INTEGER"
X$    THEN
X$    ! day of month.  Don't allow anything > 28 because of February
X$&0620CHECK = F$INTEGER(CHECK)
X$&0620IF CHECK .LT. 1 .OR. CHECK .GT. 28
X$&0620THEN
X$&0820WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Value for Check_OpenCalls must
V be 1-28."
X$&0820WRITE SYS$OUTPUT "`5C", CHECK, "`5C"
X$&0820VALID_RECORD = FALSE
X$&0620ENDIF
X$    ELSE
X$    ! day of week (Monday, Tuesday, wec.)
X$&0620VALID_DAYS = "Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,Sunday,A
VLL"
X$&0620IF F$LOCATE("," + F$EDIT(CHECK,"UPCASE") + ",", "," + F$EDIT(VALID_DAYS,
V"UPCASE") + ",") .EQ. F$LENGTH(VALID_DAYS)
X$&0620THEN
X$&0820WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Valid days for Check_OpenCalls
V are: ", VALID_DAYS
X$&0820WRITE SYS$OUTPUT "`5C", CHECK, "`5C"
X$&0820VALID_RECORD = FALSE
X$&0620ENDIF
X$    ENDIF ! type is integer
X$    COUNT = COUNT + 1
X$    GOTO _CONFIG_OPENCALLS_LOOP
X$    _EXIT_CFG_OPENCALLS:
X$    DSN_CHECK_OPENCALLS_USER = PARM3
X$    IF DSN_CHECK_OPENCALLS_USER .NES. USERNAME
X$    THEN&0620! specified a different username
X$&0620AUTHPRIV = "," + F$GETJPI(0,"AUTHPRIV") + ","
X$&0620IF F$LOCATE(",CMKRNL,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV) .AND. -
X&0A20F$LOCATE(",SETPRV,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV)
X$&0620THEN  ! don't have the necessary privilege
X$&0820DSN_CHECK_OPENCALLS_USER = USERNAME
X$&0820IF (DEBUG) .OR. (.NOT. INTERACTIVE)
X$&0820THEN`20
X$&0A20WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, CHECK_OPENCALLS needs CMKRNL or
V SETPRV to specify a different user"
X$&0A20WRITE SYS$OUTPUT "-DSNlink_NEW-W-CONFIG, CHECK_OPENCALLS output will be
V sent to ", DSN_CHECK_OPENCALLS_USER
X$&0820ENDIF ! debug
X$&0620ENDIF ! no priv
X$    ENDIF ! different username
X$  ENDIF ! check_opencalls
X$
X$  IF PARM1_UP .EQS. ".CHECK_CLOSEDCALLS"
X$  THEN
X$    IF PARM3 .EQS. "" THEN PARM3 = USERNAME
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Check closedcalls:
V `5C", PARM2, "`5C, user: `5C", PARM3, "`5C"
X$    DSN_CHECK_CLOSEDCALLS = PARM2
X$    ! now validate it
X$    COUNT = 0
X$    VALID_RECORD = TRUE    ! assume the best unless proven otherwise
X$    _CONFIG_CLOSEDCALLS_LOOP:
X$    CHECK = F$ELEMENT(COUNT,",",DSN_CHECK_CLOSEDCALLS)
X$    IF CHECK .EQS. "," THEN GOTO _EXIT_CFG_CLOSEDCALLS
X$    IF F$TYPE(CHECK) .EQS. "INTEGER"
X$    THEN
X$    ! day of month.  Don't allow anything > 28 because of February
X$&0620CHECK = F$INTEGER(CHECK)
X$&0620IF CHECK .LT. 1 .OR. CHECK .GT. 28
X$&0620THEN
X$&0820WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Value for Check_ClosedCalls mus
Vt be 1-28."
X$&0820WRITE SYS$OUTPUT "`5C", CHECK, "`5C"
X$&0820VALID_RECORD = FALSE
X$&0620ENDIF
X$    ELSE
X$    ! day of week (Monday, Tuesday, wec.)
X$&0620VALID_DAYS = "Monday,Tuesday,Wednesday,Thursday,Friday,Saturday,Sunday,A
VLL"
X$&0620IF F$LOCATE("," + F$EDIT(CHECK,"UPCASE") + ",", "," + F$EDIT(VALID_DAYS,
V"UPCASE") + ",") .EQ. F$LENGTH(VALID_DAYS)
X$&0620THEN
X$&0820WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Valid days for Check_ClosedCall
Vs are: ", VALID_DAYS
X$&0820WRITE SYS$OUTPUT "`5C", CHECK, "`5C"
X$&0820VALID_RECORD = FALSE
X$&0620ENDIF
X$    ENDIF ! type is integer
X$    COUNT = COUNT + 1
X$    GOTO _CONFIG_CLOSEDCALLS_LOOP
X$    _EXIT_CFG_CLOSEDCALLS:
X$    DSN_CHECK_CLOSEDCALLS_USER = PARM3
X$    IF DSN_CHECK_CLOSEDCALLS_USER .NES. USERNAME
X$    THEN&0620! specified a different username
X$&0620AUTHPRIV = "," + F$GETJPI(0,"AUTHPRIV") + ","
X$&0620IF F$LOCATE(",CMKRNL,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV) .AND. -
X&0A20F$LOCATE(",SETPRV,",AUTHPRIV) .EQ. F$LENGTH(AUTHPRIV)
X$&0620THEN  ! don't have the necessary privilege
X$&0820DSN_CHECK_CLOSEDCALLS_USER = USERNAME
X$&0820IF (DEBUG) .OR. (.NOT. INTERACTIVE)
X$&0820THEN`20
X$&0A20WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, CHECK_CLOSEDCALLS needs CMKRNL
V or SETPRV to specify a different user"
X$&0A20WRITE SYS$OUTPUT "-DSNlink_NEW-W-CONFIG, CHECK_CLOSEDCALLS output will b
Ve sent to ", DSN_CHECK_CLOSEDCALLS_USER
X$&0820ENDIF ! debug
X$&0620ENDIF ! no priv
X$    ENDIF ! different username
X$  ENDIF ! check_closedcalls
X$
X$  IF PARM1_UP .EQS. ".FAO_SUBJECT"
X$  THEN
X$    FAO_SUBJECT = F$ELEMENT(1,QUOTE,ORIG_RECORD)
X$    IF DEBUG`20
X$    THEN`20
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO Subject set to `5C", FAO_SU
VBJECT, "`5C"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (11,22,33,SYSTEMS,88):
V `5C", -
X     F$FAO(FAO_SUBJECT, 11, 22, 33, "SYSTEMS", 88), "`5C"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (1,1,1,SYSTEMS,1)    :
V `5C", -
X     F$FAO(FAO_SUBJECT, 1, 1, 1, "SYSTEMS", 1), "`5C"
X$    ENDIF
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".FAO_PERSONAL"
X$  THEN
X$    FAO_PERSONAL = F$ELEMENT(1,QUOTE,ORIG_RECORD)
X$    IF DEBUG`20
X$    THEN`20
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO Personal set to `5C", FAO_P
VERSONAL, "`5C"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (11,22,33,SYSTEMS,88):
V `5C", -
X     F$FAO(FAO_PERSONAL, 11, 22, 33, "SYSTEMS", 88), "`5C"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (1,1,1,SYSTEMS,1)  `20
V  : `5C", -
X     F$FAO(FAO_PERSONAL, 1, 1, 1, "SYSTEMS", 1), "`5C"
X$    ENDIF
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".FAO_GET_SUBJECT"
X$  THEN
X$    FAO_GET_SUBJECT = F$ELEMENT(1,QUOTE,ORIG_RECORD)
X$    IF DEBUG
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO GET Subject set to `5C", FA
VO_GET_SUBJECT, "`5C"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (Database,Title): `5C"
V, -
X     F$FAO(FAO_GET_SUBJECT,"Database","Title")
X$    ENDIF
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".FAO_GET_PERSONAL"
X$  THEN
X$    FAO_GET_PERSONAL = F$ELEMENT(1,QUOTE,ORIG_RECORD)
X$    IF DEBUG
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, FAO GET Personal set to `5C", F
VAO_GET_PERSONAL, "`5C"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, FAO test (Database,Title): `5C"
V, -
X     F$FAO(FAO_GET_PERSONAL,"Database","Title")
X$    ENDIF
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$! allow forced override of GET_STYLE (mostly for testing purposes)
X$  IF PARM1_UP .EQS. ".GET_STYLE"
X$  THEN
X$    GET_STYLE = PARM2
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Get style set to`20
V`5C", GET_STYLE, "`5C"
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".BATCH_PRIORITY"
X$  THEN
X$    LOW_PRIORITY = F$INTEGER(PARM2)
X$    IF (F$INTEGER(LOW_PRIORITY) .GT. PRIB) .AND. (.NOT. F$PRIVILEGE("ALTPRI"
V))
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-PRIORITY, Priority setting of `5C", PAR
VM2, "`5C ignored."
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-PRIORITY, ", PARM2, " is larger than ba
Vse priority (", PRIB, ")"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-PRIORITY, ALTPRI is required to set hig
Vher priority."
X$&0620LOW_PRIORITY = PRIB
X$    ENDIF
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, Batch priority set
V to `5C", LOW_PRIORITY, "`5C (current PRIB=", PRIB, ")"
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".EXCLUDE"
X$  THEN
X$    VALID_RECORD = TRUE
X$    EXCLUDE_ARTICLE_'EXC_COUNT' = F$EDIT(RECORD - PARM1 - PARM2,"TRIM")
X$    EXCLUDE_DB_'EXC_COUNT' = PARM2
X$    IF PARM2 .EQS. "*" THEN GOTO _END_EXC_DB_CHECK_LOOP
X$
X$    CC = 0
X$    _EXC_DB_CHECK_LOOP:
X$    IF F$TYPE(DB_NAME_'CC') .NES. "" THEN IF F$EDIT(PARM2,"UPCASE") .EQS. F$E
VDIT(DB_NAME_'CC',"UPCASE") THEN GOTO _END_EXC_DB_CHECK_LOOP
X$    CC = CC + 1
X$    IF CC .LT. DB_NAME_COUNT THEN GOTO _EXC_DB_CHECK_LOOP
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, .EXCLUDE database /", PARM2, "/
V is not defined."
X$    VALID_RECORD = FALSE
X$
X$    _END_EXC_DB_CHECK_LOOP:
X$    IF F$LOCATE(QUOTE,EXCLUDE_ARTICLE_'EXC_COUNT') .NE. F$LENGTH(EXCLUDE_ARTI
VCLE_'EXC_COUNT') .AND. EXCLUDE_WARNING
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-W-CONFIG, Exclude on line ", LINE_NUMBER,
V " may unnecessarily include quote character."
X$&0620WRITE SYS$OUTPUT "-DSNlink-NEW-W-CONFIG, Use .EXCLUDE_WARNING FALSE to d
Visable this warning."
X$    ENDIF
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Excluding article
V `5C", EXCLUDE_ARTICLE_'EXC_COUNT', "`5C"
X$    EXC_COUNT = EXC_COUNT + 1
X$  ENDIF
X$
X$  IF PARM1_UP .EQS. ".EXCLUDE_WARNING"
X$  THEN
X$    EXCLUDE_WARNING = 'PARM2_UP'&0720! should be TRUE or FALSE
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, EXCLUDE_WARNING se
Vt to ", EXCLUDE_WARNING
X$    VALID_RECORD = TRUE
X$  ENDIF
X$
X$
X$
X$!
X$!!!
X$!
X$
X$  IF .NOT. VALID_RECORD`20
X$  THEN`20
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, Invalid record `5C", ORIG_RECORD
V, "`5C, line ", LINE_NUMBER
X$    CONFIG_ERROR = TRUE
X$  ENDIF
X$
X$  GOTO _PARSE_CONFIG_LOOP
X$
X$&4621
X$
X$_END_PARSE_CONFIG:
X$  CLOSE DSN_CONFIGFILE
X$  DB_NAME_COUNT = DB_NAME_COUNT - 1
X$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT - 1
X$  EXC_COUNT = EXC_COUNT - 1
X$  MAX_EXC_COUNT = EXC_COUNT
X$
X$  IF F$TYPE(MAIL_GROUP_$ERROR$) .EQS. ""`20
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-MISGRP, Missing $ERROR$ mail group."
X$    CONFIG_ERROR = TRUE
X$  ELSE
X$    IF MAIL_GROUP_NAME_'MAIL_GROUP_$ERROR$' .NES. "$ERROR$"
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-E-MISGRP, $ERROR$ mail group must be uppe
Vrcase."
X$&0620CONFIG_ERROR = TRUE
X$    ENDIF
X$  ENDIF
X$
X$! the MX SITE agent's subprocess doesn't have SYS$LOGIN defined, whcih
X$! causes problems when using SUBMIT/LOG_FILE -- so, we point it at
X$! the DSNLINK_DIR location; then we make a test to see if that'll work....
X$  IF F$TRNLNM("SYS$LOGIN") .EQS. ""`20
X$  THEN`20
X$    DEFINE SYS$LOGIN 'DSNLINK_DIRECTORY'
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, SYS$LOGIN was undefined; it has
V been set to ", F$TRNLNM("SYS$LOGIN")
X$  ENDIF
X$
X$! in case the above trick didn't do it, we force the log file to the specific
V filename and location....
X$  IF F$PARSE(GET_LOG_FILE) .EQS. "" .OR. GET_LOG_FILE .EQS. ""
X$  THEN `20
X$    GET_LOG_FILE = DSNLINK_DIRECTORY + "DSNLINK_NEW_GET.LOG"
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, GET_LOG_FILE is undefined; attem
Vpting to"
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-I-CONFIG, use DSNLINK_NEW_DIR:DSNLINK_NEW_
VGET.LOG (", F$PARSE(GET_LOG_FILE), ")"
X$  ENDIF
X$
X$! accept "NO" and "0" to mean 'disable history file'
X$  IF F$PARSE(DSN_HISTORY_FILE,,,"NAME") .EQS. "NO" .OR. F$PARSE(DSN_HISTORY_F
VILE,,,"NAME") .EQS. "0"`20
X$  THEN
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, History file disab
Vled.  `5C", DSN_HISTORY_FILE, "`5C"
X$    DSN_HISTORY_FILE = "NONE"
X$  ENDIF
X$
X$  IF GET_STYLE .NES. "EDT" .AND. GET_STYLE .NES. "EXTRACT"
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-CONFIG, GET_STYLE must be EXTRACT or EDT
V."
X$    CONFIG_ERROR = TRUE
X$  ENDIF
X$
X$! accept "NONE" and "0" to mean 'don't schedule another execution.'
X$  IF DSN_AFTER .EQS. "NONE" .OR. DSN_AFTER .EQS. "0"
X$  THEN
X$    IF DEBUG THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-CONFIG, DSN_AFTER set to O
VNE.  `5C", DSN_AFTER, "`5C"
X$    DSN_AFTER = "ONE"
X$  ENDIF
X$
X$  IF CONFIG_ERROR`20
X$  THEN`20
X$    WRITE SYS$OUTPUT F$FAO("!/%DSNlink_NEW-E-CONFIGERR, Configuration error(s
V) prevent running DSNlink_NEW.")
X$    ERROR_STATUS = SS$_ABORT
X$    ERROR_MESSAGE = "%DSNlink_NEW-E-CONFIGERR, Configuration error(s) in ''DS
VN_CONFIG_FILENAME'"
X$    ! we will send a message to the user that submitted this DSNlink_NEW,`20
X$    ! and (when this configuration routine exits) also try to send a mail`20
X$    ! message to the $ERROR$ group about it.
X$    IF (.NOT. INTERACTIVE)`20
X$    THEN
X$&0620SEND_TO = USERNAME
X$&0620IF F$TYPE(ORIGINALLY_FROM) .NES. "" THEN SEND_TO = ORIGINALLY_FROM
X$&0620SET noON
X$&0620MAIL/noSELF -
X     SYS$INPUT: 'SEND_TO' -
X     /SUBJECT="DSNlink_NEW configuration error in ''DSN_CONFIG_FILENAME'" -
X     /PERSONAL_NAME="DSNlink_NEW configuration error"
XDSNlink_NEW encountered a configuration error while executing in non-
Xinteractive mode (Batch or Detached). `20
X
XFor further information, check .LOG file and the DSNlink_NEW
Xconfiguration file; if using DELIVER or MX SITE agents, check those logs
Xas well.
X`20
X... attempting to also inform user(s) in group $ERROR$ about the problem.
X
XThis message sent automatically by DSNlink_NEW.
X$&0620SET ON
X$    ENDIF  ! interactive
X$  ENDIF ! configuration error
X$
X$  IF BATCH THEN SET RESTART_VALUE
X$  RETURN&0720!  leaving INIT_PARSE_CONFIG
X$
X$!`0C
X$&4621
X$&4621
X$&4621
X$_DSNLINK_GET:
X$  PERFORMING_GET = TRUE
X$  IF .NOT. BATCH THEN WRITE SYS$OUTPUT F$FAO("  !24<Batch log file:!>!AS", GE
VT_LOG_FILE)
X$  IF BATCH THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-GET, Performing article GET
V processing."
X$  IF DEBUG .OR. BATCH THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I-GET, Get style: "
V, GET_STYLE
X$  WRITE SYS$OUTPUT F$FAO("  !24<Connection retries:!>!AS", F$STRING(DSN_RETRY
V_MAX))
X$
X$! modify mail groups a bit....
X$
X$  COUNT = 0
X$_REWRITE_LOOP:
X$  IF MAIL_GROUP_NAME_'COUNT' .EQS. "$ERROR$"`20
X$  THEN
X$    IF P5 .EQS. ""
X$    THEN
X$&0620MAIL_GROUP_USERS_'COUNT' = USERNAME
X$&0620ORIGINALLY_FROM = USERNAME
X$    ELSE
X$&0620MAIL_GROUP_USERS_'COUNT' = P5
X$&0620ORIGINALLY_FROM = P5
X$    ENDIF
X$    IF (DEBUG) .OR. (.NOT. INTERACTIVE) THEN WRITE SYS$OUTPUT "%DSNlink_NEW-I
V-REWROTE, Rewrote $ERROR$ group to ", MAIL_GROUP_USERS_'COUNT'
X$  ENDIF  ! error group
X$  COUNT = COUNT + 1
X$  IF COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _REWRITE_LOOP
X$  IF ORIGINALLY_FROM .NES. USERNAME THEN WRITE SYS$OUTPUT F$FAO("  !24<Origin
Vally from:!>!AS", ORIGINALLY_FROM)
X$
X$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1&0820! create another group
X$  MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = "Self"
X$  MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = USERNAME
X$  MAIL_GROUP_Self = MAIL_GROUP_COUNT
X$
X$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1&0820! even another group
X$  MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = "Retain"
X$  MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = "<send to assigned mail groups>"
X$  MAIL_GROUP_Retain = MAIL_GROUP_COUNT
X$  PREFERRED = MAIL_GROUP_COUNT
X$
X$  MAIL_GROUP_COUNT = MAIL_GROUP_COUNT + 1&0820! even another group
X$  MAIL_GROUP_NAME_'MAIL_GROUP_COUNT' = "Specific"
X$  MAIL_GROUP_USERS_'MAIL_GROUP_COUNT' = "<send to specific user(s)>"
X$  MAIL_GROUP_Specific = MAIL_GROUP_COUNT
X$
X$  IF P3 .NES. ""`20
X$  THEN`20
X$    GET_FILENAME = P3
X$  ELSE
X$    GET_FILENAME = GET_FILE
X$  ENDIF
X$  IF F$SEARCH(GET_FILENAME) .EQS. ""
X$  THEN
X$    ERROR_MESSAGE = "%DSNlink_NEW-E-GETFILE, Missing file `5C''GET_FILENAME'
V`5C"
X$    ERROR_STATUS = SS$_NOSUCHFILE
X$    GOTO _EXIT_ERROR
X$  ENDIF
X$  GET_FILENAME = F$SEARCH(GET_FILENAME)
X$  WRITE SYS$OUTPUT F$FAO("  !24<Get filename:!>!AS", GET_FILENAME)
X$
X$  IF (BATCH) .AND. (P6 .NES. "")  ! if P6 is non-null, then we should delete
V file after we're done (if all works out)
X$  THEN
X$    DELETE_ORIGINAL_GET_FILE = TRUE
X$    ORIGINAL_GET_FILE = P6
X$    WRITE SYS$OUTPUT F$FAO("  !24<Delete orig GET file:!>Yes, !AS",ORIGINAL_G
VET_FILE)
X$  ELSE
X$    DELETE_ORIGINAL_GET_FILE = FALSE
X$    WRITE SYS$OUTPUT F$FAO("  !24<Delete orig GET file:!>No")
X$  ENDIF
X$
X$  CREATE_CMD = FALSE
X$  IF BATCH THEN CREATE_CMD = TRUE
X$
X$  GOSUB _PROCESS_GET_FILE
X$
X$  IF TOTAL_ARTICLE_COUNT .EQ. 0
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-E-NOGET, No articles to get - invalid GET
V file."
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOGET, Enable DSNlink_NEW debugging or d
Velete"
X$    WRITE SYS$OUTPUT "-DSNlink_NEW-E-NOGET, ", GET_FILE, " and start over."
X$    ERROR_MESSAGE = "%DSNlink_NEW-E-GETFILE, No articles to get - invalid GET
V file."
X$    ERROR_STATUS = (SS$_ABORT .OR. STS$M_INHIB_MSG)
X$    GOTO _EXIT_ERROR
X$  ENDIF
X$
X$  IF BATCH THEN GOTO _GET_BATCH
X$
X$  IF .NOT. INTERACTIVE`20
X$  THEN ! validate group name
X$    GRPNAME = F$ELEMENT(0,"=",P4)
X$    USRNAME = F$ELEMENT(1,"=",P4)
X$    IF F$TYPE(MAIL_GROUP_'GRPNAME') .NES. ""
X$    THEN
X$&0620WHICH_GROUP = MAIL_GROUP_'GRPNAME'
X$&0620IF USRNAME .NES. "=" THEN SPECIFIC_USER = USRNAME
X$&0620AFTER_QUAL = ""
X$&0620GOTO _GET_SKIP_PROMPTS
X$    ENDIF  ! found it
X$    ERROR_MESSAGE = "%DSNlink_NEW-E-GROUP, Invalid or missing group name P4:
V `5C''P4'`5C"
X$    ERROR_STATUS = SS$_BADPARAM
X$    GOTO _EXIT_ERROR
X$  ENDIF ! .not. interactive
X$
X$  WRITE SYS$OUTPUT F$FAO("!/  !24<Mail groups!>Users")
X$  COUNT = 0
X$  LOW = 99
X$  HIGH = -1
X$_DB_DISPLAY_LOOP_2:
X$  IF MAIL_GROUP_NAME_'COUNT' .NES. "$ERROR$"`20
X$  THEN
X$    TMP = F$FAO(" !2UL. !15AS&0620!50AS", COUNT, MAIL_GROUP_NAME_'COUNT', MAI
VL_GROUP_USERS_'COUNT')
X$    IF F$LENGTH(MAIL_GROUP_USERS_'COUNT') .GT. 50 THEN TMP = TMP + TOO_LONG_C
VHAR
X$    WRITE SYS$OUTPUT TMP
X$    IF COUNT .GT. HIGH THEN HIGH = COUNT
X$    IF COUNT .LT. LOW THEN LOW = COUNT
X$  ENDIF
X$  COUNT = COUNT + 1
X$  IF COUNT .LE. MAIL_GROUP_COUNT THEN GOTO _DB_DISPLAY_LOOP_2
X$
X$  WRITE SYS$OUTPUT F$FAO("!/Ready to retrieve articles and Email to users.")
V
X$_GET_GRP_NUMBER:
X$  ASK "* Send mail to which group? (''LOW'-''HIGH') `5B''PREFERRED'`5D " WHIC
VH_GROUP
X$  WHICH_GROUP = F$EDIT(WHICH_GROUP,"TRIM")
X$  IF WHICH_GROUP .EQS. "" THEN WHICH_GROUP = PREFERRED
X$  IF F$TYPE(WHICH_GROUP) .NES. "INTEGER"`20
X$  THEN
X$    WRITE SYS$OUTPUT "Invalid - must be a number"
X$    GOTO _GET_GRP_NUMBER
X$  ENDIF
X$  WHICH_GROUP = F$INTEGER(WHICH_GROUP)
X$  IF F$TYPE(MAIL_GROUP_NAME_'WHICH_GROUP') .EQS. ""`20
X$  THEN
X$    WRITE SYS$OUTPUT "Invalid mail group number"
X$    GOTO _GET_GRP_NUMBER
X$  ENDIF ! valid number
X$  IF MAIL_GROUP_NAME_'WHICH_GROUP' .EQS. "$ERROR$"
X$  THEN
X$    WRITE SYS$OUTPUT "Invalid mail group number - cannot use $ERROR$ group."
V
X$    GOTO _GET_GRP_NUMBER
X$  ENDIF ! $error$
X$ `20
X$  IF MAIL_GROUP_NAME_'WHICH_GROUP' .EQS. "Retain"
X$  THEN&0820! check to make sure the config file has user mappings for the Dat
Vabases specified in the GET file
X$    RETAIN_OKAY = TRUE
X$    OLD_DB = ""
X$    COUNT1 = 0
X$    _CHECK_RETAIN_LOOP_1:
X$    COUNT2 = 0
X$    _CHECK_RETAIN_LOOP_2:
X$    IF F$EDIT(GET_DATABASE_'COUNT1',"UPCASE") .EQS. F$EDIT(DB_NAME_'COUNT2',"
VUPCASE")
X$    THEN&1F20! found the database
X$&0620IF OLD_DB .NES. GET_DATABASE_'COUNT1' THEN WRITE SYS$OUTPUT "%DSNlink_NE
VW-I-USERS, Text from ", GET_DATABASE_'COUNT1', " will be mailed to ", DB_MAIL_
V'COUNT2'
X$&0620OLD_DB = GET_DATABASE_'COUNT1'
X$&0620COUNT1 = COUNT1 + 1
X$&0620IF COUNT1 .LT. TOTAL_ARTICLE_COUNT THEN GOTO _CHECK_RETAIN_LOOP_1
X$&0620GOTO _EXIT_CHECK_RETAIN
X$    ENDIF ! database nout found
X$    COUNT2 = COUNT2 + 1
X$    IF COUNT2 .LT. DB_NAME_COUNT THEN GOTO _CHECK_RETAIN_LOOP_2  ! check next
V DB name
X$    IF OLD_DB .NES. GET_DATABASE_'COUNT1' THEN WRITE SYS$OUTPUT "%DSNlink_NEW
V-E-NOUSER, Can't use RETAIN: Config doesn't have ", GET_DATABASE_'COUNT1'
X$    OLD_DB = GET_DATABASE_'COUNT1'
X$    RETAIN_OKAY = FALSE
X$    COUNT1 = COUNT1 + 1
X$    IF COUNT1 .LT. TOTAL_ARTICLE_COUNT THEN GOTO _CHECK_RETAIN_LOOP_1
X$  _EXIT_CHECK_RETAIN:
X$    IF .NOT. RETAIN_OKAY THEN GOTO _GET_GRP_NUMBER
X$  ENDIF ! retain
X$
X$  SPECIFIC_USER = ""
X$  IF MAIL_GROUP_NAME_'WHICH_GROUP' .EQS. "Specific"
X$  THEN
X$    _GET_SPECIFIC_USERNAME:
X$    ASK "* Enter specific username: " SPECIFIC_USER
X$    IF SPECIFIC_USER .EQS. "" .OR. F$LOCATE(QUOTE,SPECIFIC_USER) .NE. F$LENGT
VH(SPECIFIC_USER)
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-E-INVUSER, Invalid: cannot be blank, cann
Vot contain quotes"
X$&0620GOTO _GET_SPECIFIC_USERNAME
X$    ENDIF
X$    SPECIFIC_USER = F$EDIT(SPECIFIC_USER,"COLLAPSE")
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-USERS, Sending to: ", SPECIFIC_USER
X$  ENDIF ! specific
X$
X$!
X$!!!
X$!
X$
X$  AFTER_QUAL = "`5C"
X$  IF DSN_AFTER .EQS. "ONE" THEN DSN_AFTER = "TOMORROW+01:00:00"
X$_REPROMPT_WHEN:
X$  WRITE SYS$OUTPUT ""
X$  ASK "* Submit Now, this Evening, or Tomorrow? (N/E/T) `5BT`5D " SUBMIT_TIME
V
X$  SUBMIT_TIME = F$EXTRACT(0,1,F$EDIT(SUBMIT_TIME,"UPCASE,COLLAPSE"))
X$  IF SUBMIT_TIME .EQS. "N" THEN AFTER_QUAL = ""
X$  IF SUBMIT_TIME .EQS. "E" THEN AFTER_QUAL = "/AFTER=18:00"
X$  IF SUBMIT_TIME .EQS. "T" .OR. SUBMIT_TIME .EQS. "" THEN AFTER_QUAL = "/AFTE
VR=" + QUOTE + F$CVTIME(DSN_AFTER,"ABSOLUTE") + QUOTE
X$  IF AFTER_QUAL .EQS. "`5C"
X$  THEN
X$    WRITE SYS$OUTPUT "Invalid - must be N, E, or T"
X$    GOTO _REPROMPT_WHEN
X$  ENDIF
X$
X$_GET_SKIP_PROMPTS:
X$  GET_TEMPFILE = DSN_SCRATCH_DIR + "DSNLINK_NEW_GET_" + F$GETJPI(0,"PID") + "
V_" + -
X     F$CVTIME(,,"HOUR") + F$CVTIME(,,"MINUTE") + F$CVTIME(,,"SECOND") + ".DAT"
V
X$  CREATE 'GET_TEMPFILE'
X$  OPEN/APPEND DSN_OUTFILE 'GET_TEMPFILE'
X$  WRITE DSN_OUTFILE "! ", F$SEARCH(GET_TEMPFILE)
X$  WRITE DSN_OUTFILE "! Created by DSNlink_NEW, version ", DSNLINK_NEW_VERSION
V
X$  WRITE DSN_OUTFILE "! file used for GET processing."
X$  WRITE DSN_OUTFILE F$FAO("!! Created at !%D by user !AS::!AS", 0, F$GETSYI("
VNODENAME"), USERNAME)
X$  WRITE DSN_OUTFILE "!"
X$  CLOSE DSN_OUTFILE
X$  APPEND 'GET_FILENAME' 'GET_TEMPFILE'
X$
X$  IF SPECIFIC_USER .EQS. ""
X$  THEN
X$    TMP = MAIL_GROUP_NAME_'WHICH_GROUP'
X$  ELSE
X$    TMP = MAIL_GROUP_NAME_'WHICH_GROUP' + "=" + SPECIFIC_USER
X$  ENDIF
X$
X$  IF ORIGINALLY_FROM .EQS. USERNAME
X$  THEN
X$    NOTIFY_QUAL = "/NOTIFY"
X$  ELSE
X$    NOTIFY_QUAL = "/noNOTIFY"     ! we're doing this 100% on someone else's b
Vehalf (MX SITE or DELIVER)
X$  ENDIF
X$
X$  ORIGINAL_GET_FILE = ""
X$  IF INTERACTIVE`20
X$  THEN
X$    _ASK_DEL_GET_FILE:
X$    WRITE SYS$OUTPUT ""
X$    ASK "* Delete all versions of your GET file? (Y/N/S/?) `5BY`5D " DELETE_G
VET
X$    DELETE_GET = F$EXTRACT(0,1,F$EDIT(DELETE_GET,"UPCASE,COLLAPSE"))
X$    IF DELETE_GET .EQS. "" THEN DELETE_GET = "Y"
X$    IF DELETE_GET .EQS. "?"`20
X$    THEN
X$&0620WRITE SYS$OUTPUT "Your GET file (", F$PARSE(";*",GET_FILENAME), ") is no
V longer needed."
X$&0620WRITE SYS$OUTPUT "DSNlink_NEW has made a copy of your get file for its u
Vse in the batch job."
X$&0620WRITE SYS$OUTPUT "You can safely delete this file."
X$&0620WRITE SYS$OUTPUT "  Y - Delete GET file immediately"
X$&0620WRITE SYS$OUTPUT "  N - Don't delete GET file"
X$&0620WRITE SYS$OUTPUT "  S - Only delete GET file if GET Processing is Succe
Vssful"
X$&0620GOTO _ASK_DEL_GET_FILE
X$    ENDIF
X$    IF DELETE_GET .EQS. "Y" THEN DELETE/LOG 'F$PARSE(";*",GET_FILENAME)'
X$    IF DELETE_GET .EQS. "S" THEN ORIGINAL_GET_FILE = F$PARSE(";*",GET_FILENAM
VE)
X$    IF DELETE_GET .NES. "Y" .AND. DELETE_GET .NES. "N" .AND. DELETE_GET .NES.
V "S"
X$    THEN
X$&0620WRITE SYS$OUTPUT "Invalid - must be Y, N, or ?"
X$&0620GOTO _ASK_DEL_GET_FILE
X$    ENDIF
X$  ENDIF ! interactive
X$
X$  SUBMIT -
X     'NOTIFY_QUAL' -
X     /QUEUE=DSN$BATCH -
X     /noPRINTER -
X     'AFTER_QUAL' -
X     /KEEP -
X     /PARAMETERS=( -
X&0A20'DSN_CONFIG_FILENAME', -&0620! P1 (keep the version number)
X&0A20"$GET$", -&1420! P2
X&0A20'F$SEARCH(GET_TEMPFILE)', -   ! P3 - remove logicals -- another node may
V have different ideas about where SYS$SCRATCH is located
X&0A20"''TMP'",   -&1120! P4
X&0A20"''ORIGINALLY_FROM'", -&0720! P5
X&0A20"''ORIGINAL_GET_FILE'") -     ! P6
X     /NAME="DSNlink_NEW:get" -
X     /LOG_FILE='GET_LOG_FILE' -
X     'F$PARSE(";",F$ENVIRONMENT("PROCEDURE"))'
X$
X$  FINISHED_OK = TRUE
X$  GOTO _EXIT
X$
X$!`0C
X$&4621
X$_GET_BATCH:
X$ `20
X$  GOSUB _CALL_DSN_HOST
X$
X$  CURRENT_COUNT = 0
X$_GET_SENDMAIL_LOOP:
X$  DATABASE = GET_DATABASE_'CURRENT_COUNT'
X$  TITLE = GET_TITLE_'CURRENT_COUNT'
X$  TITLE_NOQUOTES = TITLE
X$  _REMOVE_QUOTES:
X$  IF F$LOCATE(QUOTE,TITLE_NOQUOTES) .NE. F$LENGTH(TITLE_NOQUOTES)
X$  THEN
X$    TITLE_NOQUOTES`5BF$LOCATE(QUOTE,TITLE_NOQUOTES),1`5D := " "
X$    GOTO _REMOVE_QUOTES
X$  ENDIF
X$
X$  FILENAME = TMP_EXTRACTFILE + "_" + F$STRING(CURRENT_COUNT)
X$  IF F$SEARCH(FILENAME) .EQS. ""
X$  THEN
X$    CREATE 'FILENAME'
X$    OPEN/APPEND FILE 'FILENAME'
X$    WRITE FILE "Couldn't find the article:"
X$    WRITE FILE "  Database:  ", DATABASE
X$    WRITE FILE F$EXTRACT(0,79,"  Title:     " + TITLE)
X$    WRITE FILE "  For group: ", P4
X$    CLOSE FILE
X$    CALL _SEND_MAIL_MESSAGE -
X     'FILENAME' -
X     "DSNlink article not found; ''DATABASE' database" -
X     $ERROR$ -
X     "''DATABASE' DSNlink article not found"
X$  ELSE
X$    TMP1 = F$FAO(FAO_GET_SUBJECT,DATABASE,TITLE_NOQUOTES)
X$    IF F$LENGTH(TMP1) .GT. 71 THEN TMP1`5B71,1`5D := "''TOO_LONG_CHAR'"
X$    TMP1 = F$EXTRACT(0,72,TMP1)
X$
X$    TMP2 = F$FAO(FAO_GET_PERSONAL,DATABASE,TITLE_NOQUOTES)
X$
X$    SET FILE/VERSION_LIMIT=2 'FILENAME'
X$
X$! get title (second record) and count the lines
X$    OPEN/READ FILE 'FILENAME'
X$    LINE_COUNT = 2
X$    READ FILE JUNK/END_OF_FILE=_LINE_COUNT_EXIT
X$    READ FILE SECOND_RECORD/END_OF_FILE=_LINE_COUNT_EXIT
X$
X$    _LINE_COUNT_LOOP:
X$    READ FILE RECORD/END_OF_FILE=_LINE_COUNT_EXIT
X$    LINE_COUNT = LINE_COUNT + 1
X$    GOTO _LINE_COUNT_LOOP
X$    _LINE_COUNT_EXIT:
X$
X$    CLOSE FILE
X$    CREATE 'FILENAME';&0C20! create new version
X$    OPEN/APPEND FILE 'FILENAME'
X$    WRITE FILE "  Database:&0620" + DATABASE
X$    TMP = "  Title:&0920" + SECOND_RECORD
X$    IF F$LENGTH(TMP) .GT. 79 THEN TMP`5B79,1`5D := "''TOO_LONG_CHAR'"
X$    WRITE FILE F$EXTRACT(0,80,TMP)
X$
X$    ! display warning if they mismatch
X$    IF F$EDIT(SECOND_RECORD,"UPCASE,COLLAPSE") .NES. F$EDIT(TITLE,"UPCASE,CO
VLLAPSE")`20
X$    THEN`20
X$&0620TMP =  "  Desired title: " + TITLE
X$&0620IF F$LENGTH(TMP) .GT. 79 THEN TMP`5B79,1`5D := "''TOO_LONG_CHAR'"
X$&0620WRITE FILE F$EXTRACT(0,80,TMP)
X$&0620WRITE FILE "  Warning:&0720Titles don't match"
X$&0620WRITE FILE "&1120Article may need to be retrieved from DSNlink manually"
V
X$&0620TMP1 = F$EXTRACT(0,72,"(title mismatch) " + TMP1)
X$    ENDIF ! title mismatch
X$
X$    WRITE FILE "  Lines:&0920", LINE_COUNT
X$    CLOSE FILE
X$    APPEND 'FILENAME';-1 'FILENAME' `20
X$    CALL _SEND_MAIL_MESSAGE -
X     'FILENAME' -
X     "''TMP1'" -
X     "''P4'" -
X     "''TMP2'"
X$  ENDIF ! extract file exists
X$
X$  CURRENT_COUNT = CURRENT_COUNT + 1
X$  IF CURRENT_COUNT .LT. TOTAL_ARTICLE_COUNT THEN GOTO _GET_SENDMAIL_LOOP
X$
X$  FINISHED_OK = TRUE
X$  IF DELETE_ORIGINAL_GET_FILE THEN DELETE'DEBUG_LOG' 'ORIGINAL_GET_FILE'
X$  GOTO _EXIT
X$
X$!`0C
X$&4621
X$_PROCESS_GET_FILE:
X$! process contents of GET file and (optionally) create ITS command file
X$
X$  IF DEBUG
X$  THEN
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Contents of GET file:"
X$    TYPE 'GET_FILENAME'
X$  ENDIF  ! debug
X$
X$  OPEN/READ DSN_INFILE 'GET_FILENAME'
X$
X$  IF CREATE_CMD`20
X$  THEN`20
X$    OPEN/WRITE DSN_OUTFILE 'TMP_DSNFILE'
X$    WRITE DSN_OUTFILE ""
X$  ENDIF
X$
X$  ARTICLE_COUNT = 0
X$  TOTAL_ARTICLE_COUNT = 0
X$  LAST_CLOSED_DB = ""
X$  LAST_OPENED_DB = ""
X$  DB = ""
X$
X$_GET_VALID_LOOP:
X$  READ/END_OF_FILE=_DONE_GET_VALID_LOOP DSN_INFILE RECORD
X$  IF F$EXTRACT(0,1,RECORD) .EQS. ">" .OR. F$EXTRACT(0,1,RECORD) .EQS. "#" THE
VN -
X     RECORD = F$EXTRACT(1,-1,RECORD)
X$  IF RECORD .EQS. "_____" THEN GOTO _DONE_GET_VALID_LOOP
X$  IF ((F$ELEMENT(1," ",RECORD) .EQS. "database," .OR. F$ELEMENT(2," ",RECORD)
V .EQS. "database,")) .AND. -
X&0620((F$LOCATE(" article:",RECORD) .NE. F$LENGTH(RECORD) .OR. F$LOCATE(" arti
Vcles:",RECORD) .NE. F$LENGTH(RECORD))) .AND. -
X&0620(F$EXTRACT(0,1,RECORD) .NES. " ")
X$  THEN
X$    IF F$ELEMENT(1," ",RECORD) .EQS. "database," THEN DB = F$ELEMENT(0," ",RE
VCORD)  ! "xxx database, NN articles:"
X$    IF F$ELEMENT(2," ",RECORD) .EQS. "database," THEN DB = F$ELEMENT(1," ",RE
VCORD)  ! "FLASH xxx database, NN articles:"
X$  ELSE
X$    IF F$EXTRACT(0,2,RECORD) .EQS. "  "`20
X$    THEN&0620! first two characters of this record are spaces
X$&0620RECORD = F$EDIT(RECORD,"COMPRESS,TRIM")  ! remove junk
X$
X$
X$&0620! new database opened yet?`20
X$&0620! (we hold off on doing the OPEN until we've got an artilce title,`20
X$&0620!  to prevent OPENing a database and retrieving 0 articles)
X$&0620IF DB .NES. LAST_OPENED_DB
X$&0620THEN`20
X$
X$&0820! check to see if we have to close the last-opened database
X$&0820IF LAST_OPENED_DB .NES. DB .AND. LAST_OPENED_DB .NES. ""  ! If we're on
V to a new database
X$&0820THEN`20
X$&0A20IF CREATE_CMD THEN WRITE DSN_OUTFILE "close ", LAST_OPENED_DB
X$&0A20LAST_CLOSED_DB = LAST_OPENED_DB
X$&0A20WRITE SYS$OUTPUT F$FAO("!28* !20AS get !UL article!%S", LAST_OPENED_DB,
V ARTICLE_COUNT)
X$&0820ENDIF`20
X$
X$&0820! now ready to open the new database
X$&0820LAST_OPENED_DB = DB
X$&0820ARTICLE_COUNT = 0
X$&0820IF CREATE_CMD`20
X$&0820THEN
X$&0A20WRITE DSN_OUTFILE ""
X$&0A20WRITE DSN_OUTFILE "open ", DB
X$&0820ENDIF ! create_cmd
X$&0620ENDIF ! db <> last_opened_db
X$
X$&0620IF LAST_OPENED_DB .NES. ""
X$&0620THEN
X$&0820! if we've opened a database ...
X$&0820IF CREATE_CMD
X$&0820THEN
X$&0A20! perform the search
X$&0A20WRITE DSN_OUTFILE "search/english ", RECORD
X$&0A20IF DEBUG THEN WRITE DSN_OUTFILE "directory  ! this line only present dur
Ving debugging"
X$&0A20IF GET_STYLE .EQS. "EDT"
X$&0A20THEN
X$&0C20WRITE DSN_OUTFILE "read/edit 1"
X$&0C20! now we're talking to EDT
X$&0C20WRITE DSN_OUTFILE "EXIT " + TMP_EXTRACTFILE + "_" + F$STRING(TOTAL_ARTIC
VLE_COUNT)
X$&0C20! now we're back talking to DSN ITS
X$&0A20ENDIF ! get_style
X$&0A20IF GET_STYLE .EQS. "EXTRACT"
X$&0A20THEN
X$&0C20WRITE DSN_OUTFILE "read 1"
X$&0C20WRITE DSN_OUTFILE "extract " + TMP_EXTRACTFILE + "_" + F$STRING(TOTAL_AR
VTICLE_COUNT)
X$&0A20ENDIF ! get_style
X$&0820ENDIF ! create_cmd`20
X$&0820GET_TITLE_'TOTAL_ARTICLE_COUNT' = RECORD
X$&0820GET_DATABASE_'TOTAL_ARTICLE_COUNT' = DB
X$&0820ARTICLE_COUNT = ARTICLE_COUNT + 1
X$&0820TOTAL_ARTICLE_COUNT = TOTAL_ARTICLE_COUNT + 1
X$&0620ENDIF ! LAST_OPENED_DB <> ""
X$    ELSE  ! there was some text in the first two spaces, but it wasn't a new
V database
X$&0620LAST_OPENED_DB = ""
X$&0620DB = ""
X$    ENDIF ! F$EXTRACT(0,2,RECORD) = "  "`20
X$  ENDIF  ! record type (database, title, or 'other' `5Bignore 'other' records
V`5D)
X$  GOTO _GET_VALID_LOOP
X$
X$_DONE_GET_VALID_LOOP:
X$  IF ARTICLE_COUNT .GT. 0 THEN WRITE SYS$OUTPUT F$FAO("!28* !20AS get !UL art
Vicle!%S", DB, ARTICLE_COUNT)
X$  CLOSE DSN_INFILE
X$  IF CREATE_CMD`20
X$  THEN`20
X$    WRITE DSN_OUTFILE ""
X$    WRITE DSN_OUTFILE "exit"
X$    CLOSE DSN_OUTFILE
X$  ENDIF
X$
X$  RETURN  ! from PROCESS_GET_FILE
X$
X$&4621
X$!`0C
X$
X$_DO_CHECK_OPENCALLS:
X$  IF DSN_CHECK_OPENCALLS_USER .NES. USERNAME
X$  THEN
X$    ! need to submit under a different username
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-OPENCALLS, Submitting DSNlink_NEW with C
VHECKOPEN with username: ", DSN_CHECK_OPENCALLS_USER`20
X$    IF DEBUG
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Will attempt to create log file
V in .SCRATCH_DIR"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, this may fail if the user can't
V write to .SCRATCH_DIR"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, .SCRATCH_DIR is set in DSNLINK_N
VEW_CONFIG.DAT"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, Value is: `5C", DSN_SCRATCH_DIR,
V "`5C"
X$&0620CHECKOPEN_LOG = "/LOG_FILE=" + DSN_SCRATCH_DIR + "DSNLINK_NEW_CHECK.LOG"
V`20
X$    ELSE
X$&0620CHECKOPEN_LOG = "/noLOG_FILE"
X$    ENDIF
X$    OLDPRIV = F$SETPRV("CMKRNL")  ! save current privilege setting in OLDPRIV
V
X$    SUBMIT -
X     /NOTIFY -
X     /NAME="DSNlink_NEW_checkopen" -
X     'CHECKOPEN_LOG' -
X     /noPRINTER -
X     /QUEUE=DSN$BATCH -
X     /USER='DSN_CHECK_OPENCALLS_USER' -
X     /PARAMETERS=( -
X&0A20'DSN_CONFIG_FILENAME', -&0620! P1 (keep the version number)
X&0A20"$CHECKOPEN$") -&0E20! P2`20
X     'F$ENVIRONMENT("PROCEDURE")'
X$    OLDPRIV = F$SETPRV(OLDPRIV)  ! restore previous setting of CMKRNL
X$    GOTO _DONE_CHECK_OPENCALLS
X$  ELSE
X$    ! this username
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-OPENCALLS, Sending mail to DSN%OPENCALLS
V. (", F$TIME(), ")"
X$    MAIL -
X     /noSELF -
X     SYS$INPUT: -
X     DSN%OPENCALLS -
X     /SUBJECT="DSNlink_NEW-initiated check of DSNlink open calls"`20
XThis check of DSNlink open calls was initiated by code within DSNlink_NEW.
X$    WAIT 00:00:01
X$    DSN SHOW BATCH&0620! see if we see our job
X$  ENDIF ! separate user
X$
X$_DONE_CHECK_OPENCALLS:
X$  RETURN ! from _DO_CHECK_OPENCALLS
X$
X$&4621
X$!`0C
X$_DO_CHECK_CLOSEDCALLS:
X$  IF DSN_CHECK_CLOSEDCALLS_USER .NES. USERNAME
X$  THEN
X$    ! need to submit under a different username
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CLOSEDCALLS, Submitting DSNlink_NEW with
V CHECKCLOSED with username: ", DSN_CHECK_CLOSEDCALLS_USER`20
X$    IF DEBUG
X$    THEN
X$&0620WRITE SYS$OUTPUT "%DSNlink_NEW-I-DEBUG, Will attempt to create log file
V in .SCRATCH_DIR"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, this may fail if the user can't
V write to .SCRATCH_DIR"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, .SCRATCH_DIR is set in DSNLINK_N
VEW_CONFIG.DAT"
X$&0620WRITE SYS$OUTPUT "-DSNlink_NEW-I-DEBUG, Value is: `5C", DSN_SCRATCH_DIR,
V "`5C"
X$&0620CHECKCLOSED_LOG = "/LOG_FILE=" + DSN_SCRATCH_DIR + "DSNLINK_NEW_CHECK.LO
VG"`20
X$    ELSE
X$&0620CHECKCLOSED_LOG = "/noLOG_FILE"
X$    ENDIF
X$    OLDPRIV = F$SETPRV("CMKRNL")  ! save current privilege setting in OLDPRIV
V
X$    SUBMIT -
X     /NOTIFY -
X     /NAME="DSNlink_NEW_checkclosed" -
X     'CHECKCLOSED_LOG' -
X     /noPRINTER -
X     /QUEUE=DSN$BATCH -
X     /USER='DSN_CHECK_CLOSEDCALLS_USER' -
X     /PARAMETERS=( -
X&0A20'DSN_CONFIG_FILENAME', -&0620! P1 (keep the version number)
X&0A20"$CHECKCLOSED$") -&0E20! P2`20
X     'F$ENVIRONMENT("PROCEDURE")'
X$    OLDPRIV = F$SETPRV(OLDPRIV)  ! restore previous setting of CMKRNL
X$    GOTO _DONE_CHECK_CLOSEDCALLS
X$  ELSE
X$    ! this username
X$    WRITE SYS$OUTPUT "%DSNlink_NEW-I-CLOSEDCALLS, Sending mail to DSN%CLOSEDC
VALLS. (", F$TIME(), ")"
X$    MAIL -
X     /noSELF -
X     SYS$INPUT: -
X     DSN%CLOSEDCALLS -
X     /SUBJECT="DSNlink_NEW-initiated check of DSNlink closed calls"`20
XThis check of DSNlink closed calls was initiated by code within DSNlink_NEW.
X$    WAIT 00:00:01
X$    DSN SHOW BATCH&0620! see if we see our job
X$  ENDIF ! separate user
X$
X$_DONE_CHECK_CLOSEDCALLS:
X$  RETURN ! from _DO_CHECK_CLOSEDCALLS
$ call unpack DSNLINK_NEW.COM;106 884620164 "" 218 4 6
$!
$ create 'f'
X$! MX_SITE_DELIVER.COM_TEMPLATE
X$!
X$! &4B2B
X$! +  DSNlink_NEW version V5.1-1&2E20+
X$! +&4920+
X$! +  This is a sample file to automate DSNlink_NEW's GET Processing with    +
V
X$! +  MX's SITE Agent.  See DSNlink_NEW documentation for installation&0720+
X$! +  instructions.&3A20+
X$! +&4920+
X$! +  The string "*%*" marks sections you may want to modify&1120+
X$! &4B2B
X$!
X$!
X$! ABSTRACT:
X$!   Provides for automatic fetching of DSNlink article text by using the`20
X$!   MX SITE Agent.
X$!
X$! PARAMETERS:
X$!   P1 = ROUTE (host name or value of /ROUTE qualifier from DEFINE PATH)
X$!   P2 = filename containing entire message (including RFC822 headers)
X$!   P3 = filename containing recipient's address
X$!   P4 = originator's address
X$!
X$! PRIVILEGES REQUIRED:
X$!   This .COM file runs from the MX SITE Agent, which is fully privileged.
X$!   As written, this .COM file expects CMKRNL, OPER, and SYSPRV to be
X$!   enabled when the .COM file is started.
X$!
X$! ENVIRONMENT:
X$!   DCL, executed from MX SITE Agent.  The MX SITE Agent starts the
X$!   process without going through LOGINOUT.EXE, so SYS$LOGIN and SYS$SCRATCH
V
X$!   are undefined in the job table.
X$!
X$! REQUIRES:
X$!   MX mailer
X$!
X$!   The logical SYS$SCRATCH must be defined in the system or group logical
X$!   name tables, or this code needs to be modified to use a specific
X$!   scratch area for its temporary file.
X$!
X$!   This file would be named SITE_DELIVER.COM and placed in the MX_EXE:
X$!   directory.
X$!
X$!
X$&4D21
X$!`20
X$! Copyright `A9 1991-1995 by Dan Wing.  This code may be freely distributed
X$! and modified for no commercial gain as long as this copyright notice is
X$! retained.  This program is supplied 'as-is', and with no warranty.
X$!
X$&4D21
X$
X$  ON WARNING THEN GOTO ERROR
X$
X$  ! strip off stuff so we only have the local username
X$  USER = F$EDIT(P4 - ">" - "<" - F$TRNLNM("MX_VMSMAIL_LOCALHOST"),"UPCASE")
X$
X$  ! allow only certain users to activate the SITE agent
X$!*%* modify following line for your site - be sure to keep a comma in front
X$!*%* and behind the comma-separated list of authorized users.
X$  AUTHORIZED_USERS = ",SYSTEM,DWING,"
X$  IF F$LOCATE(",''USER',",AUTHORIZED_USERS) .EQ. F$LENGTH(AUTHORIZED_USERS) -
V
X     THEN GOTO INVALID_USER
X$
X$  ! copy the Email message to another location; we will be deleting the
X$  ! original file shortly (MX's SITE agent would delete it for us in any
X$  ! case).
X$  ! Set the file protection so the user specified in /USER=username can`20
X$  ! delete the file.
X$!*%* You may need to modify SYS$SCRATCH -- see REQUIRES section, above
X$  TEMPFILE = F$PARSE("SYS$SCRATCH:DSNLINK_NEW_GET_" + F$GETJPI(0,"PID") + -
X     "_" + F$CVTIME(,,"HOUR") + F$CVTIME(,,"MINUTE") + -
X     F$CVTIME(,,"SECOND") + ".TMP",,,,"NO_CONCEAL")
X$  COPY 'P2' 'TEMPFILE'/PROTECTION=W:RD
X$
X$  ! find a good place for the batch job's .LOG file; try a few places in`20
X$  ! case SYS$SCRATCH isn't defined.  Hopefully one of DSN$SCRATCH or`20
X$  ! DSN$LOGS is defined (we're running DSNlink, so they should be!)!
X$  LOG_FILE = "SYS$SCRATCH:DSNLINK_NEW_GET.LOG"
X$  IF F$PARSE(LOG_FILE) .EQS. "" THEN LOG_FILE = "DSN$SCRATCH:DSNLINK_NEW_GET.
VLOG"
X$  IF F$PARSE(LOG_FILE) .EQS. "" THEN LOG_FILE = "DSN$LOGS:DSNLINK_NEW_GET.LOG
V"
X$
X$
X$  ! kick off the GET processing.  Any errors are sent to the user specified
X$  ! with P5, and a "Sent By:" line is put on all mail messages.
X$  SUBMIT -
X     /USER=DSN$SERVER -&1B20! any non-privileged username will work (user has
V to be able to use DSN ITS)
X     /LOG_FILE='LOG_FILE' -&1720! the user needs WRITE access to this director
Vy
X     /KEEP -
X     /noPRINTER -
X     /QUEUE=DSN$BATCH -
X     /NAME="DSNlink_NEW:get" -
X     /PARAMETERS=( -
X&0A20"DSNLINK_NEW_DIR:DSNLINK_NEW_CONFIG.DAT", -  ! P1, config filename
X&0A20"$GET$", -&2320! P2, $GET
X&0A20'TEMPFILE', -&2020! P3, articles to retrieve
X&0A20"Specific=''USER'", -&1820! P4, groupname
X&0A20'USER') -&2420! P5, $ERROR$ username
X     DSNLINK_NEW_DIR:DSNLINK_NEW.COM
X$
X$  ENTRY = $ENTRY
X$
X$  REPLY/USERNAME='USER'/noNOTIFY -
X     "%DSNlink_NEW-I-SITEDONE, DSNlink_NEW GET Processing initiated (entry ''E
VNTRY')"
X$
X$  SET noON
X$  PURGE/LOG/KEEP=10 SYS$SCRATCH:DSNLINK_NEW_GET.LOG
X$  SET ON
X$
X$  DELETE 'P2'
X$  EXIT %X&07301
X$
X$&4621
X$  ! An invalid user activated the SITE agent -- inform the local node's
X$  ! Postmaster (you may want to modify this to inform the user that`20
X$  ! manages DSNlink_NEW, or inform multiple people, etc.)
X$INVALID_USER:
X$  ADDRFILE = "MX_SITE_DIR:DSNLINK_NEW_1_" + F$GETJPI(0,"PID") + ".TMP"
X$  MAILFILE = "MX_SITE_DIR:DSNLINK_NEW_2_" + F$GETJPI(0,"PID") + ".TMP"
X$  LOCAL_POSTMASTER = "<Postmaster" + F$TRNLNM("MX_VMSMAIL_LOCALHOST") + ">"
X$  RETURN_ADDRESS = "<DSNlink_NEW-SiteAgent" + F$TRNLNM("MX_VMSMAIL_LOCALHOST"
V) + ">"
X$
X$  CREATE 'MAILFILE'
X$  OPEN/APPEND FILE 'MAILFILE'
X$  ! first we write the RFC822 headers
X$  WRITE FILE "From: ", RETURN_ADDRESS
X$  WRITE FILE "Subject: DSNlink_NEW SITE AGENT warning"
X$  WRITE FILE "To: ", LOCAL_POSTMASTER
X$  ! and a blank line
X$  WRITE FILE ""
X$  ! and now the text of the message....
X$  WRITE FILE "     DSNlink_NEW SITE AGENT warning"
X$  WRITE FILE "     Invalid user ", P4, " activated DSNlink_NEW SITE agent."
X$  WRITE FILE "     File MX_EXE:SITE_DELIVER.COM contains list of valid users.
V"
X$  WRITE FILE ""
X$  WRITE FILE "-- begin attached file --"
X$  CLOSE FILE
X$  APPEND 'P2',SYS$INPUT: 'MAILFILE'
X-- end attached file --
X$
X$  ! create file containing the address we're sending to (RCPT TO:)
X$  OPEN/WRITE FILE 'ADDRFILE'
X$  WRITE FILE LOCAL_POSTMASTER
X$  CLOSE FILE
X$
X$  ! And get all of this into MX
X$  MX_ENTER = "$MX_EXE:MX_SITE_IN"
X$  MX_ENTER 'MAILFILE' 'ADDRFILE' "''RETURN_ADDRESS'"
X$
X$  ! delete work files
X$  DELETE 'ADDRFILE';,'MAILFILE';
X$
X$  ! and bounce the original message back to the sender
X$  EXIT %X&06302C&0720! %X02C is "%SYSTEM-F-ABORT"
X$
X$&4621
X$  ! Generic error handler:  forces the message to bounce to back to sender
X$ERROR:
X$  EXIT_STATUS = ($STATUS .AND. %X&07468) .OR. %X&07304  ! make it fatal
X$  EXIT 'EXIT_STATUS'
$ call unpack MX_SITE_DELIVER.COM_TEMPLATE;3 22475230 "" 14 5 6
$!
$ create 'f'
X$&4621
X$! PMDF_DELIVER.COM
X$!
X$! &4B2B
X$! +  DSNlink_NEW version V5.2&2F20+
X$! +&4920+
X$! +  This is a sample file to automate DSNlink_NEW's GET Processing with    +
V
X$! +  PMDF's DELIVER, or with standalong DELIVER.  PMDF DELIVER is&0B20+
X$! +  incorporated with PMDF, and standalong DELIVER is available via&0820+
X$! +  Anonymous FTP from Innosoft.COM.  See DSNlink_NEW documentation for    +
V
X$! +  installation instructions.&2D20+
X$! &4B2B
X$!
X$!   Your MAIL.DELIVERY file should include the lines:
X$!
X$!+++
X$!&0620* "*username+GET*" * T L SYS$LOGIN:DSNLINK_NEW_GET.LOG-DELIVER
X$!&0620* "*username+GET*" * T C SYS$LOGIN:DSNLINK_GET.DAT
X$!&0620* "*username+GET*" * T E @device:`5Bdirectory`5DDELIVER.COM
X$!---
X$!
X$!  (Replace "username" with your username, and replace "device:`5Bdirectory
V`5D"
X$!   with the actual device and directory that you've placed DELIVER.COM
X$!   into.)
X$!
X$!  (Note the first line is necessary for debugging only, and can be removed
X$!   after everything is working.)
X$!
X$&4721
X$
X$  RENAME/LOG SYS$LOGIN:DSNLINK_GET.DAT -
X     SYS$LOGIN:DSNLINK_GET_'F$GETJPI(0,"PID")'.TMP
X$
X$  IF F$SEARCH("SYS$LOGIN:DSNLINK_NEW_GET.LOG-DELIVER;-5") .NES. "" THEN -
X     PURGE/KEEP=3 SYS$LOGIN:DSNLINK_NEW_GET.LOG-DELIVER
X$
X$!!!
X$
X$! strip off the IN% and domain stuff -- we only handle local users
X$  USERNAME = F$EXTRACT(1,-1,F$ELEMENT(1,"%",F$ELEMENT(0,"@",FROM)))
X$
X$  IF F$TRNLNM("DSNLINK_NEW_DIR") .EQS. ""
X$  THEN
X$    DEVICE = F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,"DEVICE")
X$    DIRECTORY = F$PARSE(F$ENVIRONMENT("PROCEDURE"),,,"DIRECTORY")
X$    DEFINE DSNLINK_NEW_DIR 'DEVICE''DIRECTORY'
X$  ENDIF
X$
X$! you may want to do a SUBMIT here so the MAIL* queues aren't kept busy
X$! during the (potentially long) connection to the DSNlink host system. `20
X$! See MX_SITE_DELIVER.COM_TEMPLATE for ideas.
X$
X$  @DSNLINK_NEW_DIR:DSNLINK_NEW -
X     "DSNLINK_NEW_DIR:DSNLINK_NEW_CONFIG.DAT" -
X     "$GET$" -
X     "SYS$LOGIN:DSNLINK_GET_''F$GETJPI(0,"PID")'.TMP" -
X     "Specific=''USERNAME'" -
X     'USERNAME'
X$ `20
X$  EXIT
$ call unpack PMDF_DELIVER.COM_TEMPLATE;3 360666536 "" 6 6 6
$ v=f$verify(v)
$ exit

% ====== Internet headers and postmarks (see DECWRL::GATEWAY.DOC) ======
% Received: from mail1.digital.com by us2rmc.zko.dec.com (5.65/rmc-22feb94) id AA22745; Wed, 18 Oct 95 13:50:45 -040
% Received: from Cone-Of-Silence.TGV.COM by mail1.digital.com; (5.65 EXP 4/12/95 for V3.2/1.0/WV) id AA06353; Wed, 18 Oct 1995 10:17:51 -070
% Date: Wed, 18 Oct 1995 10:17:48 -0700 (PDT)
% From: Dan Wing <WING@tgv.com>
% To: star::zalewski
% Message-Id: <951018101748.26806c9e@tgv.com>
% Subject: Freeware CD submission: DSNlink_NEW
