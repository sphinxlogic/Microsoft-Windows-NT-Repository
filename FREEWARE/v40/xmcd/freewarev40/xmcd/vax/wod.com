$!	File            : WOD.COM (Weed Out Duplicates command procedure),
$!		          may be resident in any directory on any disk
$!	Purpose         : Find duplicate file names in XMCD(TM) CD database on
$!	                  OpenVMS(TM) systems.
$!	Author          : Michael Monscheuer <Monscheuer@T-Online.de>
$!	                  
$!	History         : 24-JAN-1997, V0.9
$!	                  01-FEB-1997, V1.0
$!	                     added function for replacing empty "link" files
$!	                     with a copy of the file the "link" is pointing to.
$!	                  05-FEB-1997, V1.1
$!	                     help functions added
$!
$!	OpenVMS versions: 5.4 and later, tested with 5.5-2 and 6.2 
$!	                  on VAX(TM) machines
$!	
$!	Parameters : Optional, see label HELP_PARAMS
$!
$!	First off, some definitions
$
$	WOD      = "1.1"
$	ENVI     = F$ENVIRONMENT("DEFAULT")
$	LF[0,8]  = 10
$	CR[0,8]  = 13
$	ESC[0,8] = 27
$	EXMSG    = ""
$	DBLOC    = ""
$	KEEPLIS  = 0
$	DIFFCMD  = "DIFFERENCE/PAGE"
$	IF F$GETSYI("VERSION") .LTS. "V6.0" THEN DIFFCMD = "DIFFERENCE"
$
$
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " WOD V''WOD'"
$	IF P1 .EQS. "?" .OR. F$EDIT(F$EXTRACT(0,1,P1),"COLLAPSE,UPCASE") .EQS. "H" THEN GOTO HELP_PARAMS
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " Enter a question mark (?) at any time for help."
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " To obtain help for optional command line parameters press ^Z now and"
$	WRITE SYS$OUTPUT " restart this procedure specifying a question mark (?) as P1."
$	WRITE SYS$OUTPUT ""
$
$	OPEN/READ CHN TT:
$	READ/END=FORCED_EXIT/PROMPT="                    ''ESC'[7mPress RETURN to continue or ^Z to exit''ESC'[0m" CHN CTRL
$	CLOSE CHN
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " Starting XMCD(TM) CD database cleanup at ''F$TIME()'"
$	WRITE SYS$OUTPUT ""
$
$	
$!	Find XMCD(TM) cddb location (say it supports multiple CDDBs on a given 
$!	system as well as in a cluster).
$
$	IF P1 .NES. ""
$	 THEN
$	  DBLOC = "''P1'"
$	  GOTO LOCA
$	ENDIF
$
$ DISK:
$	DDEV = F$DEVICE("*","DISK")
$	IF DDEV .EQS. ""
$	 THEN 
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT " No XMCD(TM) CD database found on this system."
$	  WRITE SYS$OUTPUT ""
$	  EXMSG = "Exiting due to previous error at ''F$TIME()'."
$	  GOTO EXIT
$	ENDIF
$	IF .NOT. F$GETDVI("''DDEV'","MNT") 
$	 THEN 
$	  WRITE SYS$OUTPUT " Found disk ''DDEV' not being mounted."
$	  GOTO DISK
$	ENDIF
$	WRITE SYS$OUTPUT " Scanning disk ''DDEV' for CD database..."
$	DBLOC  = F$SEARCH("''DDEV'[*...XMCD]CDDB.DIR")
$	IF DBLOC .EQS. "" THEN GOTO DISK
$ INQLOC:
$	WRITE SYS$OUTPUT "
$	WRITE SYS$OUTPUT " A XMCD(TM) database was found in "
$	WRITE SYS$OUTPUT " ''DBLOC'."
$	WRITE SYS$OUTPUT "
$	INQUIRE OK " Clean up ''DBLOC' ([Y]/N)"
$	IF OK .EQS. "" THEN OK = 1
$	IF OK .EQS. "?" THEN GOTO HELP_DISK
$	IF .NOT. OK THEN GOTO DISK
$
$!	cddb found... open temp file and start scanning cddb subdirs
$
$	DBLOC = DBLOC - F$PARSE("''DBLOC'",,,"VERSION") - "]CDDB.DIR" + ".CDDB]"
$ LOCA:
$	SET DEFAULT 'DBLOC'
$	DIR/OUT=NLA0:
$	IF $SEVERITY .NE. 1 
$	 THEN 
$	  SET DEF 'ENVI'
$	  EXIT
$	ENDIF
$	IF P2 .EQS. "" 
$	 THEN
$	  IF F$SEARCH("DUPLICATES.LIS") .NES. ""  
$	   THEN
$	    LISDATE = F$FILE("DUPLICATES.LIS","CDT")
$	    WRITE SYS$OUTPUT ""
$	    WRITE SYS$OUTPUT " A list with duplicate file names dated as of ''LISDATE'"
$	    WRITE SYS$OUTPUT " already exists."
$	    WRITE SYS$OUTPUT ""
$ INQUSE:
$	    USE_OLD = 0
$	    INQUIRE USE_OLD " Do you want to use the existing list to clean up the database ([N]/Y)"
$	    USE_OLD = F$EDIT(USE_OLD,"COLLAPSE,UPCASE")
$	    IF USE_OLD .EQS. "" THEN USE_OLD = 0
$	    IF USE_OLD .EQS. "?" THEN GOSUB HELP_USE
$	    IF USE_OLD .EQS. "?" THEN GOTO INQUSE
$	    IF .NOT. USE_OLD 
$	     THEN 
$	      DELETE/NOCONFIRM DUPLICATES.LIS;*
$	     ELSE
$	      GOTO USE_INFO
$	    ENDIF
$	  ENDIF
$	ENDIF
$	IF P2 .EQS. "SEARCH"
$	 THEN
$	  IF F$SEARCH("DUPLICATES.LIS") .NES. "" THEN DELETE/NOCONFIRM DUPLICATES.LIS;*
$	ENDIF
$	IF P2 .EQS. "NOSEARCH"
$	 THEN
$	  IF F$SEARCH("DUPLICATES.LIS") .EQS. "" 
$	   THEN
$	    WRITE SYS$OUTPUT ""
$	    NOSACT = 1
$ INQSEA:
$	    INQUIRE NOSACT " No list containing duplicates found. Search for duplicates now ? ([Y]/N)"
$	    NOSACT = F$EDIT(NOSACT,"COLLAPSE,UPCASE")
$	    IF NOSACT .EQS. "?" THEN GOTO HELP_SEARCH
$	    IF NOSACT .EQS. "" THEN NOSACT = 1
$	    IF NOSACT THEN GOTO FIND0
$	    EXMSG = "Exiting WOD due to user command at ''F$TIME()'"
$	    GOTO EXIT1
$	   ELSE
$	    GOTO USE_INFO
$	  ENDIF
$	ENDIF
$ FIND0:
$	OPEN/WRITE 0 TEMP_SEQ.TMP
$ FIND:
$	FILE = F$SEARCH("[...]*.;*")
$	IF FILE .EQS. "" THEN GOTO SORT
$	NAME = -
	F$PARSE(FILE,,,"NAME") + "_" + F$PARSE(FILE,,,"TYPE") -"."-"-"-"-"-"
$	WRITE 0 F$FAO("!40AS",NAME)
$	GOTO FIND
$
$!	Scanning cddb subdirs completed. Now determine dups by converting
$!      file list to a RMS indexed file (TEMP_IDX.TMP) and write out 
$!	duplicates to an exception file.
$
$ SORT:
$	CLOSE 0
$	CONVERT/STATISTIC/FAST_LOAD/SORT/EXCEP=DUPS.DAT -
	/FDL=SYS$INPUT TEMP_SEQ.TMP TEMP_IDX.TMP
FILE; ORG indexed; RECORD;	FORMAT variable;
AREA 0; BEST_TRY_CONTIGUOUS yes; BUCKET 12; ALLOCATION   60; EXTENSION  600;
KEY 0; DUPLICATES no; SEG0_LENGTH 40;
$	
$!	TEMP_IDX.TMP might come in handy for other operations. In case
$!	you want to use it, exclude it from the next $DEL command.
$	
$	DELETE/NOCONFIRM TEMP_SEQ.TMP;*,TEMP_IDX.TMP;*
$	IF F$FILE("DUPS.DAT","EOF") .EQ. 0
$	 THEN
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT " No duplicate entries found in ''DBLOC'"
$	  WRITE SYS$OUTPUT ""
$	  IF P4 .EQS. "NOLINK" THEN GOTO EXIT
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT " Searching for link files now. This may take some time..."
$	  WRITE SYS$OUTPUT ""
$	  GOTO RESOLVE_0
$	ENDIF
$
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " All duplicate CD IDs found." 
$	WRITE SYS$OUTPUT " Continuing to determine files and their locations ..."
$	WRITE SYS$OUTPUT ""
$	
$	SORT/NODUPLICATES/KEY=(POS:1,SIZE:40,DESCENDING) DUPS.DAT DUPS.DAT
$	OPEN/READ 1 DUPS.DAT
$	OPEN/WRITE 2 EVAL.DAT
$	
$!	Figure out all locations of IDs found 
$	
$ READ:
$	READ/END=DONE 1 LINE
$	FILE = F$EXTRACT(0,8,LINE) + ".;*"
$ LOCATE:
$	LOC = F$SEARCH("[...]''FILE'")
$	IF LOC .NES. "" 
$	 THEN 
$	  NAME = -
	  F$PARSE(LOC,,,"NAME") + F$PARSE(LOC,,,"TYPE") + F$PARSE(LOC,,,"VERSION")
$	  WRITE 2 F$FAO("!40AS",NAME), F$PARSE(LOC,,,"DIRECTORY")
$	  GOTO LOCATE
$	ENDIF
$	WRITE 2 ""
$	GOTO READ
$ DONE:
$	WRITE 2 ""
$	CLOSE 1
$	CLOSE 2
$	RENAME EVAL.DAT DUPLICATES.LIS
$	WRITE SYS$OUTPUT " Locating duplicate files completed."
$ USE_INFO:
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " A list of all duplicate entries in your cddb follows:"
$	WRITE SYS$OUTPUT ""
$	INQUIRE/NOPUNCTATION DUM "                            ''ESC'[7m Press RETURN to continue ''ESC'[0m''ESC'[0K"
$	TYPE/PAGE DUPLICATES.LIS
$
$!	Let user decide how to proceed now - in case there was no related param
$!	on the command line
$
$	IF P3 .NES. ""
$	 THEN
$	  CHOICE = F$EXTRACT(0,1,P3)
$	  GOTO SKIP_ACTION
$	ENDIF
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " Do you want to review the above files manually or do you want to continue''CR'''LF' this procedure to evaluate these files automatically ?"
$ ACTION:
$	INQUIRE CHOICE " (M)anually or (A)utomatically ? [A] "
$	CHOICE = F$EDIT(CHOICE,"COLLAPSE,UPCASE")
$	IF CHOICE .EQS. "" THEN CHOICE = "A"
$	IF CHOICE .EQS. "?" THEN GOTO HELP_ACTION
$	IF CHOICE .NES. "A" .AND. CHOICE .NES. "M" THEN GOTO ACTION
$ SKIP_ACTION:
$	WRITE SYS$OUTPUT ""
$	IF CHOICE .EQS. "M"
$	 THEN
$	  KEEPLIS  = 1
$	  GOTO EXIT
$	ENDIF
$	
$!	As it is more or less meaningless to compare the files' creation
$!	date and the files' number of bytes one will need to compare the 
$!	files' contence which needs more than an algorythm. So throw the 
$!	user at the output of $DIFFERENCE for all duplicates found to
$!	let the user compare the files' contence and decide which to keep.
$	
$	OPEN/READ  3 DUPLICATES.LIS
	OPEN/WRITE 4 DEL_FILES.LIS
$	DFLAG = 0
$	SET TERM/WIDTH=132
$ EVAL0:
$	CNT = 0
$ EVAL:
$	CNT = CNT + 1
$	READ/END=FINISH 3 LINE'CNT'
$	IF LINE'CNT' .EQS. "" THEN GOTO DECIDE
$	FILE'CNT' = F$EDIT(F$EXTRACT(40,255,LINE'CNT'),"COLLAPSE") + F$EDIT(F$EXTRACT(0,40,LINE'CNT'),"COLLAPSE") 
$	CHECK = FILE'CNT'
$	IF F$SEARCH("''CHECK'") .EQS. "" THEN CNT = CNT - 1
$	GOTO EVAL
$ DECIDE:
$	CNT = CNT - 1
$	CNTN = CNT - 1
$	IF CNTN .EQ. 0 THEN GOTO EVAL0
$	IF CNT .EQ. 0 THEN GOTO EVAL0
$	FILE_X = FILE'CNT'
$	FILE_Y = FILE'CNTN'
$	'DIFFCMD'/PARALLEL/WIDTH=132 'FILE_X' 'FILE_Y'
$ INQDECI:
$	WRITE SYS$OUTPUT ""
$	INQUIRE WIPE " Delete file being displayed on the (L)eft or the (R)ight or (N)one ? (L/R/N)"
$	WIPE = F$EDIT(WIPE,"UPCASE,COLLAPSE")
$	IF WIPE .EQS. "?" THEN GOTO HELP_DECIDE
$	IF WIPE .NES. "R" .AND. WIPE .NES. "L" .AND. WIPE .NES. "N" THEN GOTO INQDECI
$	IF WIPE .EQS. "N" THEN GOTO DECIDE
$	DFLAG = 1
$	IF WIPE .EQS. "L" THEN WRITE 4 FILE_X
$	IF WIPE .EQS. "R" THEN WRITE 4 FILE_Y
$	GOTO DECIDE
$ FINISH:
$	IF F$TRNLNM("3") .NES. "" THEN CLOSE 3
$	IF F$TRNLNM("4") .NES. "" THEN CLOSE 4
$	SORT/NODUPLICATES DEL_FILES.LIS DEL_FILES.LIS
$	PURGE/NOLOG DEL_FILES.LIS
$	SET TERM/WIDTH=80
$	IF .NOT. DFLAG
$	 THEN
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT " You decided to NOT delete any of the duplicates found."
$	  WRITE SYS$OUTPUT ""
$ INQKEEP:
$	  INQUIRE KEEPLIS " Do you want to keep the file containing the duplicate file names [N]"
$	  KEEPLIS = F$EDIT(KEEPLIS,"COLLAPSE,UPCASE")
$	  IF KEEPLIS .EQS. "?" THEN GOTO HELP_KEEP
$	  IF KEEPLIS .EQS. "" THEN KEEPLIS = 0
$	  GOTO EXIT
$	  WRITE SYS$OUTPUT ""
$	ENDIF
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " You decided to delete the following files:"
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT ""
$	TYPE DEL_FILES.LIS
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " You now will be asked to confirm the deletion of each file"
$	WRITE SYS$OUTPUT " listed above."
$	WRITE SYS$OUTPUT ""
$	OPEN/READ 5 DEL_FILES.LIS
$ CONFIRM:
$	READ/END=COMPLETE 5 LINE
$	DELETE/CONFIRM 'LINE'
$	GOTO CONFIRM
$ COMPLETE:
$	CLOSE 5
$	
$! Only replace files containing a link information if the CD db was cleaned up.
$! Otherwise one may end up with multiple disc IDs again. 
$! Specify NOLINK as P4 if you do not want to resolve these links.
$		
$	IF P4 .EQS. "NOLINK" THEN GOTO EXIT
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " Replacing link files now. This may take some time..."
$	WRITE SYS$OUTPUT ""
$ RESOLVE_0:
$
$! This one's a real disk test. Just listen to the drive's noises now...
$
$	SEARCH/STATISTICS [...]*.;* "IS A LINK TO"/OUT=LINKS.LIS
$	OPEN/READ 6 LINKS.LIS
$	FLF = 0
$ RESOLVE:
$	READ/END=RESOLVED 6 DUM
$	READ/ERROR=RESOLVED 6 DUM
$	READ/ERROR=RESOLVED 6 TARGET
$	READ/ERROR=RESOLVED 6 DUM
$	READ/ERROR=RESOLVED 6 SOURCE
$	FLF = 1
$	SOURCE = SOURCE - "*** This file is a link to "
$	SUBDIR = F$ELEMENT(0,"/",SOURCE)
$	FILE = F$ELEMENT(1,"/",SOURCE)
$	SOURCE = F$ENVIRONMENT("DEFAULT") - "]" + ".''SUBDIR']''FILE'."
$	TARGET = TARGET - F$PARSE("''TARGET'",,,"VERSION")
$	IF F$SEARCH("''TARGET';*") .NES. "" THEN DELETE/NOCONFIRM 'TARGET';*
$	IF F$SEARCH("''SOURCE';*") .EQS. "" THEN GOTO RESOLVE
$	COPY 'SOURCE' 'TARGET'
$	GOTO RESOLVE
$ RESOLVED:
$	CLOSE 6
$	DELETE/NOCONFIRM LINKS.LIS;*
$	WRITE SYS$OUTPUT ""
$	IF .NOT. FLF THEN WRITE SYS$OUTPUT " No links found.''CR''LF'"
$
$ EXIT:
$	IF EXMSG .EQS. "" THEN EXMSG = "Cleaning up the CD database completed at ''F$TIME()'"
$	IF .NOT. KEEPLIS 
$	 THEN 
$	  IF F$SEARCH("DUPLICATES.LIS;*") .NES. "" THEN DELETE/NOCONFIRM DUPLICATES.LIS;*
$	 ELSE
$	  WRITE SYS$OUTPUT ""
$	  WRITE SYS$OUTPUT " The file containing duplicate file names is "
$	  WRITE SYS$OUTPUT " ''F$ENVIRONMENT("DEFAULT")'DUPLICATES.LIS"
$	  WRITE SYS$OUTPUT ""
$	  EXMSG = "Exiting WOD due to user command at ''F$TIME()'"
$	ENDIF
$ EXIT1:
$	IF F$SEARCH("DUPS.DAT") .NES. "" THEN DELETE/NOCONFIRM DUPS.DAT;*
$	IF F$SEARCH("DEL_FILES.LIS") .NES. "" THEN DELETE/NOCONFIRM DEL_FILES.LIS;*
$	SET DEFAULT 'ENVI'
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT " ''EXMSG'"
$	WRITE SYS$OUTPUT ""
$	EXIT
$ HELP_DISK:
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    If you have one CD database only enter YES (or Y or 1)." 
$	WRITE SYS$OUTPUT "    If you have more than one CD database and want a different one to be"
$	WRITE SYS$OUTPUT "    cleaned UP enter NO (or N or 0)."    
$	WRITE SYS$OUTPUT ""
$	GOTO INQLOC
$ HELP_DECIDE:
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Enter L if you want to delete the file being displayed on the left. "
$	WRITE SYS$OUTPUT "    Enter R if you want to delete the file being displayed on the right. "
$	WRITE SYS$OUTPUT "    Enter N if you want to keep both of the files."
$	WRITE SYS$OUTPUT "    Press RETURN then to compare the next two files."
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    The files are not deleted immediately. You will be asked again later"
$	WRITE SYS$OUTPUT "    whether you really want to delete files which you are selecting now."    
$	WRITE SYS$OUTPUT ""
$	GOTO INQDECI
$ HELP_ACTION:
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    If you want to continue and compare the contence on the files found press"
$	WRITE SYS$OUTPUT "    RETURN or enter A and press RETURN. This procedure will display the"
$	WRITE SYS$OUTPUT "    results of a $DIFFERENCE command on all duplicates found."
$	WRITE SYS$OUTPUT "    You can decide which of these files to keep then.
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    If you want to review the duplicates by manually browsing through the database"
$	WRITE SYS$OUTPUT "    directories and files enter N and press RETURN."
$	WRITE SYS$OUTPUT "    The procedure will terminate then, leaving you with a file containing all"
$	WRITE SYS$OUTPUT "    duplicate file names."
$	WRITE SYS$OUTPUT ""
$	GOTO ACTION
$ HELP_USE:
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    A list with duplicate file names resulting from a previous clean up was"
$	WRITE SYS$OUTPUT "    found on this system."
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    If you prefer to use the existing list for cleaning up over creating a"
$	WRITE SYS$OUTPUT "    new one now, enter YES and press RETURN. Otherwise a new list will be"
$	WRITE SYS$OUTPUT "    created."
$	WRITE SYS$OUTPUT ""
$	RETURN
$ HELP_KEEP:
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Enter YES and press RETURN if you wish to not delete the file containing"
$	WRITE SYS$OUTPUT "    the list of duplicates in order to review these files manually again."
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Enter NO and press RETURN if you do not intend to review the duplicates"
$	WRITE SYS$OUTPUT "    manually again.
$	WRITE SYS$OUTPUT "    
$	WRITE SYS$OUTPUT ""
$	GOTO INQKEEP
$ HELP_SEARCH:
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    You advised this procedure to not search for duplicate disc IDs now and"
$	WRITE SYS$OUTPUT "    to use an existing list of duplicate disc IDs instead."
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    As no such list does exist, you need to decide whether so search for"
$	WRITE SYS$OUTPUT "    duplicate disc IDs now or whether to exit this procedure"
$	WRITE SYS$OUTPUT ""
$	WRITE SYS$OUTPUT "    Enter YES and press the Return key to start the search or enter NO and"
$	WRITE SYS$OUTPUT "    press the Return key to exit this procedure."
$	WRITE SYS$OUTPUT ""
$	GOTO INQSEA
$ HELP_PARAMS:
$	
$	TYPE SYS$INPUT

 The following optional parameters may be passed to this procedure:


   P1 - Exact location of CD database to be cleaned up
           
          Standard VMS notation - e.g. disk:[dir.sub]

          If omitted this procedure will search for a CD db on the entire
          system. 

   P2 - "SEARCH" or "NOSEARCH"

          SEARCH   

            explicitely advises this procedure to scan the database for 
            duplicate disc IDs regardless whether a file containing the 
            duplicate IDs already exists or not.

          NOSEARCH 

            explicitely advises this procedure to use an existing file 
            containing the duplicate IDs. If there is no such file, you 
            will be prompted for how to continue.

          If omitted, the procedure will either start the search or prompt 
          you whether to use the file (if any) containing the duplicate IDs.


   P3 - "Manually" or "Automatically"

          MANUALLY 

            advises this procedure to terminate after finding duplicate 
            IDs and writing them to a file named DUPLICATES.LIS
            Note that finding and replacing "link" files with files 
            containing real information won't take place if P3 is "Manually"

          AUTOMATICALLY 

            advises this procedure to continue the clean up after finding 
            the duplicates. You'll be facing the output of multiple $DIFF 
            commands to determine which duplicates to weed out.
            The files you choose won't be deleted immediately, you'll be 
            prompted for each file's deletion afterwards.

         If omitted, you'll be prompted for how to continue at this 
         particular step.


   P4 - "NOLINK"

          NOLINK

            advises this procedure to not search for files containing a 
            "link" information and replace them with a copy of a file the 
            "link" is pointing to.

          If omitted _AND_ the previous clean up is complete, the procedure
          will replace "link" files with a copy of the file the "link" is 
          pointing to.


  In case you want to pass only one of the above parameters to the procedure
  remember to specify the preceeding parameters as "", f.i. like with

          @WOD "" "" "" NOLINK
     or
          @WOD "" SEARCH

$	
$ FORCED_EXIT:
$	IF F$TRNLNM("CHN") .NES. "" THEN CLOSE CHN
