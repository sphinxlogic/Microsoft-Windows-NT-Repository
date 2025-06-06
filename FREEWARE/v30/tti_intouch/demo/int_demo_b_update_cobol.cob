IDENTIFICATION DIVISION.
PROGRAM-ID. prog6.
*
*  Updates a sequential file
*
ENVIRONMENT DIVISION.
INPUT-OUTPUT SECTION.
FILE-CONTROL.

        SELECT TEST-FILE
                ASSIGN TO "SYS$SCRATCH:TTI_VENDOR.TMP"
                ORGANIZATION IS SEQUENTIAL.

DATA DIVISION.
FILE SECTION.

FD  TEST-FILE.
01  TEST-FILE-RECORD.
    05 NAME        PIC X(30).
    05 SEX         PIC X.
    05 B-DATE      PIC X(6).
    05 STREET      PIC X(30).
    05 CITY        PIC X(12).
    05 ZIP         PIC X(9).
    05 ETHNIC      PIC X.
    05 PARENT      PIC X(24).
    05 PHONE       PIC X(10).
    05 BPHONE      PIC X(10).
    05 EPHONE      PIC X(10).
    05 BALANCE     PIC 9(6)v99.
    05 BALANCE-CHAR REDEFINES BALANCE PIC X(8).
    05 FILLER      PIC X(49).

WORKING-STORAGE SECTION.

01  FILE-EOF-SW    PIC X     VALUE '0'.
    88 FILE-EOF              VALUE '1'.
01  RECORD-CNT     PIC 9(6)  VALUE 0.
01  TMP-CNT        PIC 99    VALUE 0.

PROCEDURE DIVISION.

MAINLINE SECTION.
START-UP.

	OPEN I-O TEST-FILE ALLOWING ALL.
	PERFORM 1-READ-FILE THRU 1-EXIT UNTIL FILE-EOF.
	DISPLAY "Records updated: " RECORD-CNT WITH CONVERSION.
	CLOSE TEST-FILE.
	STOP RUN.

START-UP-EXIT.
	EXIT.

1-READ-FILE.

	READ TEST-FILE
	  AT END SET FILE-EOF TO TRUE
	         GO TO 1-EXIT.

	ADD 1 TO RECORD-CNT.
	ADD 1 TO TMP-CNT.
	IF  TMP-CNT = 25 THEN 
	  DISPLAY "On record: " RECORD-CNT WITH CONVERSION
	  MOVE 0 TO TMP-CNT
	END-IF.
	INSPECT BALANCE-CHAR REPLACING ALL SPACE BY ZERO.	
	ADD 50 TO BALANCE.
	REWRITE TEST-FILE-RECORD.

1-EXIT.
	EXIT.
