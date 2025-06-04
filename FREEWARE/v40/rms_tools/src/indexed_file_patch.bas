 1	OPTION TYPE = EXPLICIT, CONSTANT TYPE = INTEGER
 !
 !      18-Oct-1991, Fixed usage of MARGIN statement.
 !
 !	This program verifies data level bucket headers and reports any
 !	possible corrupted buckets. It does NOT check the records, just
 !	the general bucket layout info. The scan is done by following the
 !	next_bucket_vbn pointer in the header starting at the first
 !	data bucket, or a user specified VBN (can specify index bucket
 !	here). In case of any corruption the program ATTEMPTS to find
 !	the next valid bucket by assuming all data buckets are consequtive.
 !
 !	IFF a corruption is recognised, then a TEMPLATE PATCH command
 !	file is generated which could be used to ATTEMPT to patch up
 !	(a copy of) the file. Such patch must be followed by a normal
 !	convert before the file is reused.
 !
 !	Disk-IO is somewhat optimized by reading in as many buckets as
 !	a 32Kb buffer will hold under the assumption that there is a
 !	good chance that the next bucket is adjacent to the current.
 !	The maximum bucket size the program will handle is 32 blocks.
 !
 !	The advantage of this program over ANA/RMS that it tends to 
 !	need far less I/Os and can find multiple corruptions in one run.
 !	
 !	Have fun, Hein van den Heuvel, Valbonne March 1987.
 !	  - 3-Feb-1991: Modified to use library definitions for RMS.
 !	  - 3-Sep-1991: Add bitmap for seen buckets.
 !
	On error go to hell!
 
        EXTERNAL LONG FUNCTION  SYS$OPEN(FABDEF), SYS$CONNECT(RABDEF), &
                                SYS$READ(RABDEF), SYS$CLOSE(FABDEF)
	EXTERNAL LONG CONSTANT	RMS$_NORMAL
       %INCLUDE "$FABDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"
       %INCLUDE "$RABDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"
       %INCLUDE "$XABDEF" %FROM %LIBRARY "SYS$LIBRARY:BASIC$STARLET.TLB"

	DECLARE STRING	FILE_NAME, TXT
	DECLARE LONG	RMS_STATUS, I, NEXT_DATA, HEADER_INVALID, TRY,	&
			VBN_IN_BUF, VBN_PER_BUF, LAST_VBN, PATCH_OPEN,	&
			BKT_PER_STATUS, CNT_BKT, CNT_IO, MAX_TRY, SEEN,	&
			BYTES_PER_BUCKET, FIRST_VBN, DBS, BIT_NUMBER, BITMASK
	DECLARE BYTE	CHECK_LEVEL
	I = 0
	DIMENSION INTEGER BUCKET_SEEN (I)
	EXTERNAL LONG FUNCTION LIB$EXTV

	MAP (RMS_BLOCKS) LONG CUR_VBN, STRING NAME_BUFFER = 80,		&
			 RABDEF RAB, FABDEF FAB, XABDEF XAB
	MAP (RMS_BLOCKS) WORD CUR_VBN_SAMPLE

	!
	! Three ways to look at the data in the buffer, 
	! to be used with REMAP to get to the BUCKET HEADER
	!
	MAP         (RMS_BUFFER) STRING USER_BUFFER=32767
	MAP DYNAMIC (RMS_BUFFER) BYTE CHECK, AREA, WORD VBN_SAMPLE, 	&
		NEXT_FREE, ID, LONG NEXT_VBN ,BYTE LEVEL, FLAGS, OTHER_CHECK
	MAP DYNAMIC (RMS_BUFFER) BYTE FIRST_CHECK, BYTE LAST_CHECK
	REMAP	    (RMS_BUFFER) NEXT_VBN, FLAGS

	INPUT 'File name'; FILE_NAME
	INPUT 'Start VBN'; FIRST_VBN				
	INPUT 'Number of buckets between status lines'; BKT_PER_STATUS
	GOSUB OPEN_FILE
	FIRST_VBN = XAB::XAB$L_DVB UNLESS FIRST_VBN 	!Use first data VBN?
	NEXT_VBN = FIRST_VBN
	BKT_PER_STATUS = 1000% UNLESS BKT_PER_STATUS	!Default 1000
	WHILE (FLAGS and 1%) = 0%	!Last bucket?
		LAST_VBN = CUR_VBN
		GOSUB READ_NEXT_BUCKET
		GOSUB ANALYZE_BUCKET_HEADER
		IF SEEN THEN
		    PRINT "LOOP! @ VBN:"; CUR_VBN; "! Last valid VBN"; LAST_VBN
		    GO TO 2
		END IF
		IF HEADER_INVALID THEN
		    PRINT "*** VBN:"; CUR_VBN; TXT; "! Last valid VBN"; LAST_VBN
		    WHILE HEADER_INVALID AND (TRY < MAX_TRY)
			TRY = TRY + 1%
!			NEXT_VBN = CUR_VBN + DBS Asume consequtive buckets
			NEXT_VBN = CUR_VBN + 1  !Asume consequtive buckets
			GOSUB READ_NEXT_BUCKET
			GOSUB ANALYZE_BUCKET_HEADER
			HEADER_INVALID = -1 IF SEEN	!Don't create loop.
		    NEXT
		    IF HEADER_INVALID THEN
			PRINT "Can not find valid bucket after"; TRY; "tries"
			INPUT "How many more tries? "; MAX_TRY
			FLAGS = 1% UNLESS MAX_TRY	!Finish outer loop
		    ELSE
			GOSUB WRITE_PATCH_RECORD
		    END IF
		END IF
	NEXT
	PRINT "*** There were"; CNT_BKT; "Data buckets in the file."
	PRINT "   "; CNT_IO; "Reads done. Last bucket VBNs are:"; LAST_VBN; CUR_VBN
	GO TO 2

 Analyze_bucket_header:
	!
	! BKT$B_BKTCB not checked beyond last bucket flag.
	!

	HEADER_INVALID = -1%		!Assume the worst, hope for the best.
	IF VBN_SAMPLE <> CUR_VBN_SAMPLE THEN
	   TXT = " Address sample" + NUM$(VBN_SAMPLE)
	ELSE
	    IF (NEXT_FREE < 0%) OR (NEXT_FREE > (BYTES_PER_BUCKET + 14%)) THEN
		TXT = " Next free:" + NUM$(NEXT_FREE)
	    ELSE
		IF AREA <> '0'B THEN
		    TXT = " Area:" + NUM$(AREA)
		ELSE
		    IF (NEXT_VBN > FAB::FAB$L_ALQ) OR (NEXT_VBN < 0) THEN
			TXT = " Next bucket:" + NUM$(NEXT_VBN)
		    ELSE
			IF (LEVEL <> CHECK_LEVEL) THEN
			    TXT = " Level:" + NUM$(LEVEL)
			ELSE
			    IF CHECK <> OTHER_CHECK THEN
			    	TXT = " Check bytes:" + NUM$(CHECK) + &
				    "," + NUM$(OTHER_CHECK) 
			    ELSE
			    	    HEADER_INVALID = 0%	!Header seems OK.
			    END IF
			END IF
		    END IF
		END IF
	    END IF
	END IF
	RETURN

 Read_next_bucket:

	CUR_VBN = NEXT_VBN			! Next VBN will become CURRENT.
	
	!
	! The longword array BUCKET_SEEN holds a bit per data bucket
	! Make sure a bucket is no already seen before processin to avoid loops.
	! 
	I = CUR_VBN / ( DBS * 32 )		! Bitmask Offset in Array
	BIT_NUMBER = MOD ( CUR_VBN / DBS, 32)	! Bit Offest in Mask
	BITMASK = BUCKET_SEEN (I)		! Current mask
	IF LIB$EXTV( BIT_NUMBER, 1, BITMASK) THEN ! NON Zero?
	    SEEN = -1				! Seen this bucket before.
	ELSE
	    SEEN = 0				! Not seen.
	END IF	    

	IF NOT HEADER_INVALID THEN		! Hunting for valid bucket?
	    CALL LIB$INSV ( 1, BIT_NUMBER, 1, BITMASK) 
	    BUCKET_SEEN (I) = BITMASK		! Mark bit for bucket in array
	END IF 	

	I = NEXT_VBN - VBN_IN_BUF		! Last VBN of bucket in buffer?
	IF (I > 0) AND (I =< (VBN_PER_BUF - DBS) ) THEN
		NEXT_DATA = I * 512%
	ELSE				
		NEXT_DATA = 0%
		RAB::RAB$L_BKT = NEXT_VBN	!Stuff the VBN into the RAB
		RMS_STATUS = SYS$READ(RAB)	!Read next bucket in buffer
	    		CALL sys$exit(RMS_STATUS BY VALUE) IF RMS_STATUS <> RMS$_NORMAL
		CNT_IO = CNT_IO + 1%		!Maintain READ count.
		VBN_IN_BUF = CUR_VBN		!Save
	END IF
	CNT_BKT = CNT_BKT + 1%			!Count this bucket
	REMAP (RMS_BUFFER) STRING FILL = NEXT_DATA, CHECK, AREA, &
		VBN_SAMPLE, NEXT_FREE, ID, NEXT_VBN, LEVEL, FLAGS,  &
		STRING FILL = BYTES_PER_BUCKET, OTHER_CHECK
	CHECK_LEVEL = LEVEL IF CUR_VBN = FIRST_VBN
	PRINT "   "; CNT_BKT; "Buckets, "; CNT_IO; "Reads. Current VBN = "; &
		CUR_VBN IF CNT_BKT=(BKT_PER_STATUS*(CNT_BKT/BKT_PER_STATUS))
	RETURN 

 Open_file:
	NAME_BUFFER = FILE_NAME		!Move into static string for LOC().
        FAB::FAB$B_BID = FAB$C_BID      !Make this a real FAB
        FAB::FAB$B_BLN = FAB$C_BLN      !Make this a real FAB
	FAB::FAB$L_FNA = LOC(NAME_BUFFER)	!Put Address of name_buf in Fab
	FAB::FAB$B_FNS = LEN(FILE_NAME)	!Put Lenght of file_name in Fab
        FAB::FAB$B_FAC = FAB$M_GET + FAB$M_BRO  !READ access in BLOCK I/O mode
        FAB::FAB$B_SHR = FAB$M_UPI      !No locking what so ever needed
        RAB::RAB$B_BID = RAB$C_BID      !Make this a real RAB
        RAB::RAB$B_BLN = RAB$C_BLN      !Make this a real RAB
	RAB::RAB$L_ROP = RAB$M_BIO
	RAB::RAB$L_FAB = LOC(FAB)	!Put Address of Fab in Rab
	FAB::FAB$L_XAB = LOC(XAB)	!Put Address of Xab in Fab
        XAB::XAB$B_COD = XAB$C_KEY      !Make this a KEY XAB
        XAB::XAB$B_BLN = XAB$C_KEYLEN   !Make this a KEY XAB

	RMS_STATUS = SYS$OPEN(FAB)	!Open the file
    		CALL sys$exit(RMS_STATUS BY VALUE) IF RMS_STATUS <> RMS$_NORMAL
    	RMS_STATUS = SYS$CONNECT(RAB)	!Connect a buffer
    		CALL sys$exit(RMS_STATUS BY VALUE) IF RMS_STATUS <> RMS$_NORMAL
	RAB::RAB$L_UBF = LOC(USER_BUFFER)	!Put Address of user_buf in Rab
	DBS = XAB::XAB$B_DBS		!Pick up Data Bucket size.
	VBN_PER_BUF = DBS * ( 63 / DBS )
	BYTES_PER_BUCKET = ( DBS * 512 ) - 15
	RAB::RAB$W_USZ = VBN_PER_BUF * 512	!User buffer size based in bytes
	VBN_IN_BUF = - VBN_PER_BUF 
	MAX_TRY = FAB::FAB$L_ALQ / 20
	MAX_TRY = 100 IF MAX_TRY < 100
	I = FAB::FAB$L_ALQ / ( 32 * DBS )	! Just to please BASIC parser.
	DIMENSION INTEGER BUCKET_SEEN ( I )	! Bit per bucket
	RETURN

 Write_patch_record:
	IF NOT PATCH_OPEN THEN
		OPEN "PATCH.COM" FOR OUTPUT AS FILE #1%
		MARGIN #1, 80
		PRINT #1%, "$PATCH/NONEW/ABSOLUTE " + FILE_NAME
	END IF	
	PRINT "    Valid bucket"; CUR_VBN; " after"; TRY; "tries."
	PRINT #1%, "DEPOSIT ^D" + NUM1$(LAST_VBN) + " - 1 * 200 + 8 = ^D" + &
		NUM1$(CUR_VBN) + " ! Last valid -> Next valid VBN"
	PATCH_OPEN = -1%	!Mark as open
	TRY = 0%		!Reset
	RETURN

 HELL:	PRINT ERT$(ERR) UNLESS ERR = 11
	RESUME 3
 2	IF PATCH_OPEN THEN
		PRINT "*** Template patch command file PATCH.COM generated."
		PRINT #1%, "UPDATE"
		CLOSE #1%
	END IF
	RMS_STATUS = SYS$CLOSE(FAB)		!Done.
 3	END
