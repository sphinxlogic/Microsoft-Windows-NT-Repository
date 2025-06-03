C This file contains common setup information for the IBMPRINT symbiont.
C
C An ARRAY of STRUCTURE is used to keep information on the various 
C streams and jobs.  The array offset is ONE PLUS the JOB CONTROLLER's
C stream id.  This is done so we can use the JOB CONTROLLER's value for
C our array offset.
C
C
C
        PARAMETER LF              = CHAR(10)
        PARAMETER CR              = CHAR(13)

 	PARAMETER NUM_STREAMS = 15

 	PARAMETER STRM_DOWN   = 0
 	PARAMETER STRM_AVAIL  = 1   
 	PARAMETER STRM_START  = 2 ! START PENDING (OPEN FILES)
 	PARAMETER STRM_RUN    = 3 ! JOB in process 
 	PARAMETER STRM_ABORT  = 4 ! ABORT in process 
        PARAMETER DBG_LU     = 100
        PARAMETER INP_LU     = 103
        PARAMETER MBX_LU     = 104

 	STRUCTURE / STREAM_PARAMETERS / 
C STREAM INFORMATION
                INTEGER*4 ID              ! STREAM ID TO SYMBIONT
 		INTEGER*2 STATE		  ! STREAM STATE (STRM_*)
 		INTEGER*2 DEV_LEN         ! DEVICE NAME LENGTH
 		CHARACTER DEV_NAM*50	  ! DEVICE NAME
 		INTEGER*4 DEV_STS         ! STATUS OF THE DEVICE
 		LOGICAL   ABORT,PAUSED
C TASK CHARACTERISTICS
 		INTEGER*4 ACCOUNTNG(4)    ! ACCOUTING VALUES
 		INTEGER*4 ERROR(4)	  ! LAST 4 ERROR CODES 	
 		INTEGER*4 UIC             ! UIC OF THE CURRENT USER
                UNION 
                MAP
                    CHARACTER*28 FID_INFO
                END MAP
                MAP
                    CHARACTER NAM_DVI*16 
                    INTEGER*2 NAM_FID(3)
                    INTEGER*2 NAM_DID(3)
                END MAP
                END UNION
		INTEGER*4 ENTRY
                CHARACTER*80 FILE
		                    INTEGER*2    FILE_LEN
		CHARACTER*20 USER
				    INTEGER*2    USER_LEN
		CHARACTER*40 JOB
				    INTEGER*2    JOB_LEN

		END STRUCTURE

 	RECORD / STREAM_PARAMETERS / STREAM(NUM_STREAMS)


       STRUCTURE /oper_msg_type/
           UNION 
           MAP
              integer*4 type
              integer*4 rqstid
              character*12 title/'CARDREADER: '/
              character*80 text 
           END MAP
           MAP
               character*(80+12+8) string
           END MAP
           END UNION
          END STRUCTURE    !message
      RECORD /oper_msg_type/ inpsmb_msg

      STRUCTURE /iosb_type/
 		INTEGER*2 status,length
 		INTEGER*4 junk
          END STRUCTURE    !iosb_type
      RECORD /iosb_type/ inpsmb_iosb

        LOGICAL DEBUG		     	 ! DEBUG STATE
        INTEGER  CUR_STREAM,CUR_ID
 	INTEGER  ACTIVE,TASKS            ! NUMBER OF ACTIVE STREAMS/TASKS
        CHARACTER*1024 MESSAGE           ! MESSAGE BUFFER

      INTEGER inpsmb_inp_chan,inpsmb_err_chan

      COMMON /misc/ debug,message 
      COMMON /streams/ STREAM,ACTIVE,CUR_ID 
      COMMON /inpsmb/ inpsmb_inp_chan,inpsmb_err_chan,
     +                inpsmb_msg,inpsmb_iosb

