!This procedure uses the System Dump Analyzer (SDA) to search the lock
!database for all processes that hold locks to a selected file.  This
!example was written as a DCL procedure to show what is being searched.
!
!To search the lock database in a timely fashion would require
!execution of Kernel-mode code and would require that the lock
!database be locked while it was searched.
!
!
!*** CAUTION ***
!
!This sample command procedure has been tested on OpenVMS VAX V6.0.
!However, we cannot guarantee its effectiveness because of the
!possibility of error in transmitting or implementing it.  It is meant
!to be used as a template for writing your own procedure, and it may
!require modification for use on your system.
!
!
!PROCEDURE NOTES:
!
!RMS record locks are stored in the lock database, in a resource tree
!with the file lock at the root.  By searching for the file lock, it is
!possible to determine if the tree exists on the node being searched.
!If the root exists on that node, the entire resource tree resides on
!that node.  All record locks for that resource tree can be found by
!comparing the parent lock ID to the lock ID for the file.
!
!The following command procedure demonstrates a method that can be used
!to find these record locks.  Please note that the execution time of
!this procedure can be fairly long.  Also, dynamic lock remastering may
!move  a resource tree to another node in the cluster, in response to
!lock activity.  This means that the procedure may find the file lock
!on one node of the cluster, but the entire resource tree may move to
!another node before all data is gathered for analysis.  This may
!result in the generation of a partial list of record locks.
!
!This procedure uses numerous temporary files.  Although they are
!deleted when the procedure completes, they may use significant disk
!space.  If the procedure terminates abnormally it will leave those
!files on your disk. The names of those files are listed as temporary
!files at the beginning of the procedure.  The size of the files and
!the runtime of the procedure are directly related to the number of
!locks on your system.
!
!
!PROCEDURE:
!
$!*********************** RECORD_LOCK.COM *********************************
$!
$! This procedure is presented as an example.  Runtime is variable, but
$! invariably lengthy.  Please see the warnings in the article if you
$! actually plan to invoke this procedure.  Your process must hold CMKRNL
$! privilege to run ANALYZE/SYSTEM.
$!
$       RES_NAME = " "
$       TEMPFILE1 = "FILE_RSB.LIS"
$       TEMPFILE2 = "CLUSTR.LIS"
$       TEMPFILE3 = "SUB_LOCK.LIS"
$       TEMPFILE4 = "RECORD_LOCK.LIS"
$       TEMPFILE5 = "GRANTED_QUE.LIS"
$       TEMPFILE6 = "PID_RECORD.LIS"
$       TEMPFILE7 = "RSB.TXT"
$       TEMPFILE8 = "LOCK.TXT"
$       TEMPFILE9 = "NODE_LIST.LIS"
$!
$! These commands close any open channels, in case a previous run aborted.
$!
$       SET MESSAGE/NOFACILITY/NOTEXT/NOSEVERITY/NOIDENTIFICATION
$       CLOSE DATA
$       CLOSE LOCK
$       CLOSE FIND_LOCK
$       CLOSE PID_RECORD
$       CLOSE FIND_PID
$       CLOSE NODE
$!
$       SAY :== WRITE SYS$OUTPUT
$       SAY "Enter name of file containing locked records (including path)"
$       SAY "(ie. [SYS10.SYSCOMMON.SYSMGR]AUDIT_SERVER.DAT)"
$       INQUIRE FILENAME
$       SAY "Enter disk that holds this file (including :)"
$       SAY "(ie.  $1$DUA0:)"
$       INQUIRE DISK
$       FILENAME = DISK + FILENAME
$!
$! Execute BUILD_RESOURCE_NAME to build a resource name that is based on
$! the file ID.
$!
$       GOSUB BUILD_RESOURCE_NAME
$!
$! Explicit filenames must be used in ANALYZE/SYSTEM
$!
$       ANALYZE/SYSTEM

SET OUTPUT FILE_RSB.LIS
SHOW RESOURCE/NAME="RMS$"
SET OUTPUT CLUSTR.LIS
SHOW CLUSTER
EXIT
$!
$! Find the resource blocks, and extract the address of the Resource Block
$! (RSB) and the cluster system ID (CSID) to determine what node the
$! resource is on.  If on this node, we get all the info we need.  If not
$! on this node, delete the temp files and exit.
$!
$       SAY "Building resource name."
$       OPEN/READ DATA 'TEMPFILE1'
$LOOP1:
$       READ/END=NOT_FOUND DATA RECORD
$!
$! Next line tests if we're at beginning of a resource block
$!
$       IF F$EXTRACT (0,7,RECORD) .NES. "Address" THEN GOTO LOOP1
$!
$! save the RSB address
$!
$       LOCKID = F$EXTRACT (17,8,RECORD)
$       COUNT = 3
$!
$! skip the next 3 records
$!
$       GOSUB RECORD_READ
$!
$! save the CSID
$!
$       CSID = F$EXTRACT (35,8,RECORD)
$       COUNT = 3
$       GOSUB RECORD_READ
$!
$! Build a hex representation of the resource name from data in FILE_RSB.LIS
$! and compare it to the prebuilt resource name for the file you are looking
$! for.  If the two names match, delete FILE_RSB.LIS to save disk space.
$!
$       RES_NAME = F$EXTRACT (15,18,RECORD)
$       READ/END = NOT_FOUND DATA RECORD
$       RES_NAME = RES_NAME + F$EXTRACT (15,18,RECORD)
$       READ/END=NOT_FOUND DATA RECORD
$       RES_NAME = RES_NAME + F$EXTRACT (15,18,RECORD)
$
$       IF RES_NAME .NES. RESOURCE_NAME THEN GOTO LOOP1
$       CLOSE DATA
$       DELETE 'TEMPFILE1';*
$       FILE_LOCK = LOCKID
$!
$! if the CSID is 00000000, then the resource is local to this node.
$!
$       IF CSID .EQS. "00000000" .OR. F$SEARCH (TEMPFILE4) .EQS. "" -
                THEN GOTO SKIP_CHECK_CSID
$!
$! execute CHECK_CSID, to determine if file lock is mastered on this
$! local node.  If it is not, CHECK_CSID displays a message and exit.
$!
$       GOSUB CHECK_CSID
$       IF LOCAL_NODE .EQS. "FALSE" THEN GOTO CLOSEOUT
$
$SKIP_CHECK_CSID:
$       SAY "Resource tree was found on this node."
$       SAY "Gathering data.  Please wait..."
$!
$! Get all the rest of the info we need
$!
$       ANALYZE/SYSTEM
SET OUTPUT RSB.TXT
SHOW RESOURCE/ALL
SET OUTPUT LOCK.TXT
SHOW LOCK/ALL
EXIT
$
$       SAY "Analyzing the data.  May take a while..."
$!
$! Search RSB.TXT (TEMPFILE7) for all sub_locks; and save to SUB_LOCK.LIS
$! (TEMPFILE3)
$!
$       SEARCH/WINDOW=30/OUTPUT='TEMPFILE3' 'TEMPFILE7' "''FILE_LOCK'"
$!
$! Search for all the record locks from that file and save them to
$! RECORD_LOCK.LIS (TEMPFILE4).  NOTE:  Record locks have a length of 4.
$!
$       SEARCH /WINDOW=20/OUTPUT='TEMPFILE4' 'TEMPFILE3' "Length    4"
$       DELETE 'TEMPFILE3';*
$       OPEN/READ DATA 'TEMPFILE4'
$       OPEN/WRITE LOCK 'TEMPFILE5'
$!
$! Look in the "granted" queue for the lock ID of each granted lock ; save
$! each one to GRANTED_QUE.LIS (TEMPFILE5).
$!
$LOCK_LOOP:
$       READ/END=END1 DATA RECORD
$       IF F$EXTRACT (0,7,RECORD) .EQS. "Granted"
$       THEN
$ID_LOOP:
$!
$! read the next record.  If it's not blank, the record holds at least
$! one lock ID.  Write each lock ID to TEMPFILE5.
$!
$               READ DATA RECORD
$               IF F$EXTRACT (1,1,RECORD) .NES. ""
$               THEN
$                       WRITE LOCK F$EXTRACT (1,8,RECORD)
$                       IF F$EXTRACT (33,1,RECORD) .NES. ""
$                       THEN
$                               WRITE LOCK F$EXTRACT (33,8,RECORD)
$                       ENDIF
$!
$! loop back till last record read for this granted queue
$!
$                       GOTO ID_LOOP
$               ENDIF
$       ENDIF
$!
$! read till we hit end_of_file
$!
$       GOTO LOCK_LOOP
$END1:
$       CLOSE DATA
$       CLOSE LOCK
$       DELETE 'TEMPFILE4';*
$       GOSUB GET_PID
$       GOSUB DISPLAY_PID
$       GOTO EXIT_MAIN
$!
$CLOSEOUT:
$       GOTO EXIT_MAIN
$NOT_FOUND:
$       SAY "No resource block found for file ''FILENAME'"
$       SAY "found on this node.  Either this node has no interest in"
$       SAY "this resource or the filename is incorrect."
$EXIT_MAIN:
$       IF F$SEARCH (TEMPFILE1) .NES. "" THEN DELETE 'TEMPFILE1';*
$       IF F$SEARCH (TEMPFILE2) .NES. "" THEN DELETE 'TEMPFILE2';*
$       IF F$SEARCH (TEMPFILE3) .NES. "" THEN DELETE 'TEMPFILE3';*
$       IF F$SEARCH (TEMPFILE4) .NES. "" THEN DELETE 'TEMPFILE4';*
$       IF F$SEARCH (TEMPFILE5) .NES. "" THEN DELETE 'TEMPFILE5';*
$       IF F$SEARCH (TEMPFILE6) .NES. "" THEN DELETE 'TEMPFILE6';*
$       IF F$SEARCH (TEMPFILE7) .NES. "" THEN DELETE 'TEMPFILE7';*
$       IF F$SEARCH (TEMPFILE8) .NES. "" THEN DELETE 'TEMPFILE8';*
$       IF F$SEARCH (TEMPFILE9) .NES. "" THEN DELETE 'TEMPFILE9';*
$
$       SET MESSAGE/FACILITY/TEXT/SEVERITY/IDENTIFICATION
$       SAY "Exiting..."
$       EXIT
$!
$!********************** BUILD_RESOURCE_NAME subroutine ********************
$!
$BUILD_RESOURCE_NAME:
$!
$! PURPOSE:  builds the hex representation of the file resource name and
$! places it in the variable RESOURCE_NAME.  The format of the resource
$! name is:  "RMS$" + File_ID + disk_lock_name.
$!
$       FID = F$FAO ("!XW",'F$EXT (1,f$len (F$FILE (FILENAME,"FID"))-
                ,F$ELEM (0,",",F$FILE (FILENAME,"fid")))')
$       SEQ = F$FAO ("!XW",'F$EXT (0,f$len (F$FILE (FILENAME,"FID"))-
                ,F$ELEM (1,",",F$FILE (FILENAME,"fid")))')
$       RVN = F$FAO ("!XW",'F$EXT (0,f$len (F$FILE (FILENAME,"FID"))-
                ,F$ELEM (2,",",F$FILE (FILENAME,"fid")))')
$       DISK_NAME = F$GETDVI (DISK,"DEVLOCKNAM")
$       LINE1 = SEQ + FID + " " + "24534D52"
$       LINE2 = " " + F$EXTRACT (20,8,DISK_NAME) + " " + -
                F$EXTRACT (28,4,DISK_NAME) + RVN
$       LINE3 = " " + F$EXTRACT (4,8,DISK_NAME) + " " + -
                F$EXTRACT (12,8,DISK_NAME) + " "
$       RESOURCE_NAME = LINE1 + LINE2 + LINE3
$       RETURN
$!
$!********************** GET_PID subroutine ********************************
$!
$! PURPOSE:  This subroutine recursively searches the lock database for
$! each lock ID in GRANTED_QUE.LIS (TEMPFILE5), to find the process ID
$! (PID) of the process holding the lock and the mode of the granted lock.
$! This is the most time consuming routine -- the larger the lock data base,
$! and the more record locks there are to search, the longer this will take.
$! We use RECORD_LOCK.LIS (TEMPFILE4) to hold info on each record lock
$! resource, and delete TEMPFILE4 after extracting the PID and mode
$! information from it.
$!
$GET_PID:
$       OPEN/READ FIND_LOCK 'TEMPFILE5'
$       OPEN/WRITE PID_RECORD 'TEMPFILE6'
$READ_LOCK:
$       READ/END=END2 FIND_LOCK LOCK
$       SEARCH /WINDOW=10/OUTPUT='TEMPFILE4' LOCK.TXT "''LOCK'"
$       OPEN/READ FIND_PID 'TEMPFILE4'
$READ_PID:
$       READ FIND_PID RECORD
$       IF F$EXTRACT (0,8,RECORD) .NES. "Lock id:" THEN GOTO READ_PID
$       WRITE PID_RECORD F$EXTRACT (10,8,RECORD)
$       WRITE PID_RECORD F$EXTRACT (30,8,RECORD)
$       COUNT = 5
$MODE_LOOP:
$       READ FIND_PID RECORD
$       COUNT = COUNT - 1
$       IF COUNT .NE. 0 THEN GOTO MODE_LOOP
$       WRITE PID_RECORD F$EXTRACT (16,2,RECORD)
$       CLOSE FIND_PID
$       DELETE 'TEMPFILE4';*
$       GOTO READ_LOCK
$END2:
$       CLOSE FIND_LOCK
$       DELETE 'TEMPFILE5';*
$       CLOSE FIND_PID
$       IF F$SEARCH (TEMPFILE4) .NES. "" DELETE 'TEMPFILE4';*
$       CLOSE PID_RECORD
$       RETURN
$!
$!**************** DISPLAY_PID subroutine ****************************
$!
$! PURPOSE:  This subroutine displays all pertinent information about
$! the PIDs that hold locks, and the mode in which each lock was granted.
$! The information is stored in PID_RECORD.LIS (TEMPFILE6).
$!
$DISPLAY_PID:
$       COUNT = 0
$       SAY "The following records locks were found for file ''filename'"
$       SAY ""
$       SAY "LOCKID                     Process ID              GRANTED AT"
$       SAY ""
$DISPLAY_LOOP:
$       OPEN/READ PID_RECORD 'TEMPFILE6'
$       READ/END=END3 PID_RECORD RECORD
$       COUNT = COUNT + 1
$       LOCKA = RECORD
$       READ PID_RECORD RECORD
$       PIDA = RECORD
$       READ PID_RECORD RECORD
$       MODE = RECORD
$       LOCK_RECORD = LOCKA + "         " + PIDA + "            " + MODE
$       SAY "''LOCK_RECORD'"
$       GOTO DISPLAY_LOOP
$END3:
$       CLOSE PID_RECORD
$       DELETE 'TEMPFILE6';*
$       SAY "There are ''COUNT' record locks on file: ''FILENAME'"
$       RETURN
$!
$!************************ RECORD_READ subroutine *************************
$!
$! PURPOSE:  This little subroutine to skip over some records in file
$! FILE_RSB.LIS (TEMPFILE1).
$!
$RECORD_READ:
$       READ/END=NOT_FOUND DATA RECORD
$       COUNT = COUNT - 1
$       IF COUNT .NE. 0 THEN GOTO RECORD_READ
$       RETURN
$!
$!******************* CHECK_CSID subroutine *******************************
$!
$! PURPOSE:  This subroutine searches CLUSTR.LIS (TEMPFILE2) and reads
$! NODE_LIST.LIS (TEMPFILE9) to determine whether the resource is held on
$! the local node.  If it is, CHECK_CSID RETURNs LOCAL_NODE=TRUE.
$!
$CHECK_CSID:
$       LOCAL_NODE = "FALSE"
$       SEARCH/WINDOW=30/OUTPUT='TEMPFILE9' 'TEMPFILE2' ''CSID'
$       DELETE 'TEMPFILE2';*
$       OPEN/READ NODE 'TEMPFILE9'
$NODE_LOOP:
$       READ/END=EXIT_LOCAL NODE RECORD
$       IF F$EXTRACT (18,8,RECORD) .NES. CSID THEN GOTO NODE_LOOP
$       IF F$LOCATE ("local", RECORD) .NE. F$LENGTH (RECORD)
$       THEN
$             SAY "The resource tree was located on node ",
$             SAY F$ELEMENT (2, " ", RECORD)
$             SAY "Please run this command procedure on that node."
$       ELSE
$             LOCAL_NODE = "TRUE"
$       ENDIF
$EXIT_LOCAL:
$       CLOSE NODE
$       DELETE 'TEMPFILE9';*
$       RETURN
