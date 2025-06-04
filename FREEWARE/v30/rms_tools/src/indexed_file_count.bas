 1	OPTION TYPE = EXPLICIT
 !
 !	This program will gather some prologue-3 indexed file statistics for
 !	the data buckets that ANALYZE/RMS omits and generaly it counts faster.
 !	The file integrety is assumed to be checked beforehand with ANALYZE/RMS.
 !	The program will verify the record compression (which old ANA/RMS 
 !	verions do not check) To get to the databuckets it starts by reading
 !	the first data bucket or a user entered value and then follows the 
 !	next vbn pointers. For the time being there are several assumptions 
 !	made in the program. Disk-IO is somewhat optimized by reading in as 
 !	many buckets as a 32Kb buffer will hold under the assumption that there 
 !	is a good chance that the next bucket is adjacent to the current.
 !	The maximum bucket size the program will handle is 32 blocks.
 !	
 !	Have fun, Hein van den Heuvel, Valbonne Xmas 1986
	On error go to hell!

	EXTERNAL LONG FUNCTION	SYS$OPEN(FABDEF), SYS$CONNECT(RABDEF), &
				SYS$READ(RABDEF), SYS$CLOSE(FABDEF)
	EXTERNAL LONG CONSTANT	RMS$_NORMAL
       %INCLUDE "$FABDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"
       %INCLUDE "$RABDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"
       %INCLUDE "$XABDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"

	DECLARE STRING	FILE_NAME, FRMT1, FRMT2, FRMT3, FRMT4
	DECLARE DOUBLE	ALL_BYTES, ALL_RECS
	DECLARE LONG	RMS_STATUS, I, NEXT_DATA, KEY_NCMPR, DAT_NCMPR,	&
		AREA0_BYTES, USED_BYTES, DEL_BYTES, DAT_BYTES, CMP_BYTES, &
		VBN_IN_BUF, VBN_PER_BUF, BKT_PER_STATUS, DAT_END, LRL,	&
		CNT_RRV, CNT_DEL, CNT_REC, CNT_BKT, CNT_UNKNOWN, CNT_IO, &
		CUR_VBN, TKS

	MAP (RMS_BLOCKS) RABDEF RAB, FABDEF FAB, XABDEF XAB,	&
			 STRING NAME_BUFFER = 80	
	MAP (RMS_BUFFER) STRING USER_BUFFER=32767
	!
	! Two ways to look at the data in the buffer, to be used with REMAP
	! - to get to the BUCKET HEADER and/or RECORD HEADER.
	!
	MAP DYNAMIC (RMS_BUFFER) BYTE CHECK, AREA, WORD VBN_SAMPLE, 	&
		NEXT_FREE, ID, LONG NEXT_VBN ,BYTE LEVEL, FLAGS
	MAP DYNAMIC (RMS_BUFFER) BYTE REC_FLAGS, WORD REC_ID, RRV_ID,	&
		LONG RRV_VBN, WORD REC_LEN, BYTE KEY_LEN, KEY_CNT
	MAP DYNAMIC (RMS_BUFFER) BYTE REPEAT_CNT, WORD DAT_LEN
	REMAP	    (RMS_BUFFER) NEXT_VBN, FLAGS

	GOSUB OPEN_FILE
	PRINT "Buckets", "Records", "RRVs", "Deleted", "Unknown", "Reads."
	PRINT
	WHILE (FLAGS and 1%) = 0%	!Last bucket?
		GOSUB READ_NEXT_BUCKET
		GOSUB ANALYZE_BUCKET
		PRINT CNT_BKT, CNT_REC, CNT_RRV, CNT_DEL, CNT_UNKNOWN, CNT_IO&
		  IF CNT_BKT=(BKT_PER_STATUS*(CNT_BKT/BKT_PER_STATUS))
	NEXT
	GOSUB PRINT_COUNTERS
	RMS_STATUS = SYS$CLOSE(FAB)		!Done.
	GO TO 2

 ANALYZE_BUCKET:
	WHILE NEXT_DATA < NEXT_FREE			!Anything left?
	    REMAP (RMS_BUFFER) STRING FILL = NEXT_DATA, &
			REC_FLAGS, REC_ID, RRV_ID, RRV_VBN, REC_LEN, KEY_LEN
	    SELECT REC_FLAGS				!What is it?
	      CASE 2%					!Normal valid record
		CNT_REC = CNT_REC + 1%
		NEXT_DATA = NEXT_DATA + 11%
		IF DAT_NCMPR AND FAB::FAB$B_RFM = FAB$C_FIX THEN
		    CMP_BYTES = CMP_BYTES + FAB::FAB$W_MRS + 9%
		ELSE
		    CMP_BYTES = CMP_BYTES + REC_LEN + 11%
		END IF
		GOSUB CHECK_RECORD			!This is the big one
	      CASE 6%					!Deleted record
		CNT_DEL = CNT_DEL + 1%
		NEXT_DATA = NEXT_DATA + REC_LEN + 11%
		DEL_BYTES = DEL_BYTES + REC_LEN + 11%
	      CASE 10%					!Retrieval vector
		CNT_RRV = CNT_RRV + 1%
		NEXT_DATA = NEXT_DATA + 9%
	      CASE ELSE
		CNT_UNKNOWN = CNT_UNKNOWN + 1%	!?
		NEXT_DATA = NEXT_FREE			!Terminate bucket scan
	    END SELECT
	NEXT
	RETURN

 READ_NEXT_BUCKET:
	CUR_VBN = NEXT_VBN			!Only for error messages
	I = NEXT_VBN - VBN_IN_BUF		!Last VBN of bucket in buffer?
	IF (I > 0) AND (I =< (VBN_PER_BUF - XAB::XAB$B_DBS) ) THEN
		NEXT_DATA = I * 512%
	ELSE				
		NEXT_DATA = 0%
		RAB::RAB$L_BKT = NEXT_VBN	!Stuff the VBN into the RAB
		RMS_STATUS = SYS$READ(RAB)	!Read next bucket in buffer
	    		CALL sys$exit(RMS_STATUS BY VALUE) IF RMS_STATUS <> RMS$_NORMAL
		CNT_IO = CNT_IO + 1%		!Maintain READ count.
		VBN_IN_BUF = CUR_VBN		!Save
	END IF

	REMAP (RMS_BUFFER) STRING FILL = NEXT_DATA, CHECK, AREA, &
		VBN_SAMPLE, NEXT_FREE, ID, NEXT_VBN, LEVEL, FLAGS

	NEXT_FREE = NEXT_DATA + NEXT_FREE 	!Add Offset in buffer
	NEXT_DATA = NEXT_DATA + 14%		!Add Bucket overhead
	CNT_BKT = CNT_BKT + 1%			!Count this bucket
	RETURN 

 CHECK_RECORD:
    DAT_END = NEXT_DATA + REC_LEN			!Look at the data now.
    IF DAT_END =< NEXT_FREE THEN			!Free space?
	IF KEY_NCMPR THEN				!Deal with the key first
	    NEXT_DATA = NEXT_DATA + TKS
	    I = TKS
	ELSE
	    IF KEY_LEN > TKS THEN
		PRINT "Key chunk spills over key boundary. Rec = ";  &
			;CNT_REC; ", VBN ="; CUR_VBN; &
			", ID ="; REC_ID; " ( "; RRV_VBN; ","; RRV_ID; ")"
		NEXT_DATA = NEXT_FREE 			!End of this bucket.
	    ELSE
		NEXT_DATA = NEXT_DATA + KEY_LEN + 2%
		I = TKS - KEY_LEN - 2%
	    END IF
	END IF


	REMAP (RMS_BUFFER) STRING FILL = NEXT_DATA, DAT_LEN !First chunk

	IF DAT_NCMPR THEN				!Deal with data next
	    IF FAB::FAB$B_RFM = FAB$C_FIX THEN
		NEXT_DATA = NEXT_DATA + FAB::FAB$W_MRS	!Add in fix size
		DAT_BYTES = DAT_BYTES + FAB::FAB$W_MRS	!Count the user data.
	    ELSE
		NEXT_DATA = NEXT_DATA + DAT_LEN		!Add in Variable size
		DAT_BYTES = DAT_BYTES + DAT_LEN		!Count the user data.
	    END IF
	ELSE						!Uh Oh, decompress.
	    WHILE NEXT_DATA < DAT_END			!Still in record?
		I = I + DAT_LEN				!Count chunk bytes
		NEXT_DATA = NEXT_DATA + DAT_LEN + 2%!Skip chunk & its length
		REMAP (RMS_BUFFER) STRING FILL = NEXT_DATA, &
			REPEAT_CNT, DAT_LEN		!This repeat, next len
		I = I + REPEAT_CNT			!Count the repeat bytes
		I = I + 256% IF REPEAT_CNT < 0		!Unsigned byte.
		NEXT_DATA = NEXT_DATA + 1%		!Adjust for REPEAT_CNT
	    NEXT					!Go around in circles
	    
	    IF (NEXT_DATA > DAT_END) THEN
		PRINT "Data chunk spills over record boundary. Rec = "; &
		     CNT_REC; ", VBN ="; CUR_VBN; &
		     ", ID ="; REC_ID; " ( "; RRV_VBN; ","; RRV_ID; ")"
		NEXT_DATA = NEXT_FREE		!End of this bucket.
	    ELSE
		! Everything OK.
		DAT_BYTES = DAT_BYTES + I		!Count the user data.
		LRL = I IF I > LRL			!Maintain longest record
	    END IF !Overflow
	END IF ! Data compression
    ELSE ! Spill into Free Space?
	PRINT "Compressed data spills into free space. VBN =";CUR_VBN; &
	    ", ID ="; REC_ID; " ( "; RRV_VBN; ","; RRV_ID; ")"
    END IF !Free Space
    RETURN

 OPEN_FILE:
	LRL = 0
	KEY_NCMPR = 0
	DAT_NCMPR = 0
	USED_BYTES = 0
	DEL_BYTES = 0
	DAT_BYTES = 0
	CMP_BYTES = 0
	CNT_RRV = 0
	CNT_DEL = 0
	CNT_REC = 0
	CNT_BKT = 0
	CNT_UNKNOWN = 0
	CNT_IO = 0
	INPUT 'File name'; FILE_NAME
	NAME_BUFFER = FILE_NAME		!Move into static string for LOC().
	FAB::FAB$B_BID = FAB$C_BID	!Make this a real FAB
	FAB::FAB$B_BLN = FAB$C_BLN	!Make this a real FAB
	FAB::FAB$L_FNA = LOC(NAME_BUFFER) !Put Address of name_buf in Fab
	FAB::FAB$B_FNS = LEN(FILE_NAME)	!Put Lenght of file_name in Fab
	FAB::FAB$B_FAC = FAB$M_GET + FAB$M_BRO 	!READ access in BLOCK I/O mode
	FAB::FAB$B_SHR = FAB$M_UPI		!No locking what so ever needed
	RAB::RAB$B_BID = RAB$C_BID	!Make this a real RAB
	RAB::RAB$B_BLN = RAB$C_BLN	!Make this a real RAB
	RAB::RAB$L_ROP = RAB$M_BIO
	RAB::RAB$L_FAB = LOC(FAB)	!Put Address of Fab in Rab
	FAB::FAB$L_XAB = LOC(XAB)	!Put Address of Xab in Fab
	XAB::XAB$B_COD = XAB$C_KEY	!Make this a KEY XAB
	XAB::XAB$B_BLN = XAB$C_KEYLEN	!Make this a KEY XAB

	RMS_STATUS = SYS$OPEN(FAB)	!Open the file
    		CALL sys$exit(RMS_STATUS BY VALUE) IF RMS_STATUS <> RMS$_NORMAL
	IF FAB::FAB$B_ORG <> FAB$C_IDX THEN 
		PRINT "Organization is NOT indexed. Code found: "; FAB::FAB$B_ORG
		CALL sys$exit(2% BY VALUE)
	END IF
	IF XAB::XAB$B_PROLOG <> XAB$C_PRG3 THEN 
		PRINT "Prologue version is not 3. Code found: "; XAB::XAB$B_PROLOG
		CALL sys$exit(2% BY VALUE)
	END IF

	PRINT "Start VBN (Default "; XAB::XAB$L_DVB; " )";
	INPUT NEXT_VBN
	NEXT_VBN = XAB::XAB$L_DVB UNLESS NEXT_VBN 	!Use first data VBN?
	INPUT 'Number of buckets between status lines'; BKT_PER_STATUS
	BKT_PER_STATUS = 1000000% UNLESS BKT_PER_STATUS !No status?

    	RMS_STATUS = SYS$CONNECT(RAB)	!Connect a buffer
    		CALL sys$exit(RMS_STATUS BY VALUE) IF RMS_STATUS <> RMS$_NORMAL
	RAB::RAB$L_UBF = LOC(USER_BUFFER)	!Put Address of user_buf in Rab
	VBN_PER_BUF = XAB::XAB$B_DBS * ( 63% / XAB::XAB$B_DBS )
	RAB::RAB$W_USZ = VBN_PER_BUF * 512%    !User buffer size based in bytes

	DAT_NCMPR = -1% IF (XAB::XABKEYDEF::XAB$B_FLG AND XAB$M_DAT_NCMPR)
	KEY_NCMPR = -1% IF (XAB::XABKEYDEF::XAB$B_FLG AND XAB$M_KEY_NCMPR)
	TKS = XAB::XAB$B_TKS				!Total key size
	VBN_IN_BUF = - VBN_PER_BUF 
	RETURN

 PRINT_COUNTERS:
	AREA0_BYTES = CNT_BKT * XAB::XAB$B_DBS * 512%
	ALL_RECS = (CNT_REC + CNT_RRV + CNT_DEL ) / 100
	ALL_BYTES = AREA0_BYTES/100			!Used for percentages
	I = AREA0_BYTES - CMP_BYTES - DEL_BYTES - (CNT_BKT*15%) - (CNT_RRV*9%)
	PRINT
	FRMT1 = "'RRRRRRRRRRRRRRRRRRR <%>######## <%>##.## <%>###### <%>##.##"
	FRMT2 = "'RRRRRRRRRRRRRRRRRRR <%>######## <%>##.##"
	FRMT3 = "####### 'LLLLLLLLLLLLLLLLLLLL"
	FRMT4 = "####### 'LLLLLLLLLLLLLLLLLLLL"
	PRINT           "   Databucket Totals     Bytes    %   Records    %"
	PRINT           "----------------------------------------------------"
	PRINT USING FRMT1; "User data";		 			&
		CMP_BYTES; CMP_BYTES/ALL_BYTES; CNT_REC; CNT_REC/ALL_RECS
	PRINT USING FRMT1; "RRV data"; 					&
		CNT_RRV*9%; (CNT_RRV*9%)/ALL_BYTES; CNT_RRV, CNT_RRV/ALL_RECS
	PRINT USING FRMT1; "Deleted data";				&
		DEL_BYTES; DEL_BYTES/ALL_BYTES; CNT_DEL, CNT_DEL/ALL_RECS
	PRINT USING FRMT2; "Bucket Overhead"; CNT_BKT*15%; 15*CNT_BKT/ALL_BYTES
	PRINT USING FRMT2; "Free room"; I; I/ALL_BYTES
	PRINT USING FRMT1; "TOTAL size"; AREA0_BYTES; 100%; 
	PRINT USING FRMT3; CNT_BKT; "Data Buckets."
	PRINT USING FRMT2; "Real User data"; DAT_BYTES; 0%;
	PRINT USING FRMT4; 100 * (1 - (REAL(CMP_BYTES,DOUBLE) /		&
	    REAL(DAT_BYTES,DOUBLE))); "% Compression."
	PRINT USING FRMT1; "Longest record seen"; LRL; 0%
	PRINT USING FRMT1; "Ave. record length"; DAT_BYTES/CNT_REC; 0%
	RETURN 

 HELL:	PRINT ERT$(ERR) UNLESS ERR = 11
	RESUME 2
 2	END
