FINDLOCKS, System_Mgmt, Reports all RMS record locks on cluster.

This program scans the Distributed Lock Manager searching for RMS record
locks. For each lock found it displays the process ID, and name, the user
name, terminal name and the name of the image being executed. It also 
displays the name of the file being locked, the VBN and ID of the locked
record and the first 50 bytes of the record. If the file is indexed, all
the keys are displayed. Unprintable characters within displayed data
are output as dots.


Example:

SP1>run findlocks
Process 20E0F209, "SP_EDITOR", User: SP_EDITOR on node: SP1
running DSA1:[REL_SP_117_1_3.EXE.UTIL]CDB_BATCH_ADD.EXE;1 at _LTA5310:
Has a lock on DSA1:[SUB_COMPANIES]GENERAL_SUBSIDIARY.IDX;4
VBN = 15997, ID = 26
TELEWEST COMMUNICATIONS GROUP LTD
Key  1, "TELEWEST COMMUNICATIONS GROUP LTD                 .                                                "
Key  2, "TELWST  "
Key  3, "1999-12-07"
