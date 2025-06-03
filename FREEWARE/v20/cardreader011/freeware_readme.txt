CARDREADER011, MISCELLANEOUS, VMS Card Reader and mail back print symbiont

 				 CARDREADER_SMB
 			A VMS CARDREADER/MAILBACK symbiont

Version 1.0 6/2/93

Author:  Robert Eden                robert@cpvax.tu.com
 	 Comanche Peak S.E.S        eden@fallout.lonestar.org 
         Glen Rose Tx, 76043        
         (817) 897-0491

        This program is a VAX/VMS server symbiont to provide a
        queue on the VAX that acts as a cardreader.  A second queue is
        also available to mail output back to the original user.

 	Any file sent to a queue assigned to device CARDIN gets run 
 	through DEC's INPSMB image. Login errors are sent to
 	the CLUSTER operator(s)' console by the symbiont and logged
 	normally by IMPSMB.

 	Any file sent to a queue assigned to device MAILOUT gets sent
 	as a mail message to the username owning the job.  This 
 	is used so the output from the batch job submitted to CARDREADER
 	can be returned as a mail message.

        This symbiont can connect to up to 16 queues, but only 1
        is active at any one time.  (Hey, I'm lazy.) (Why would
 	anyone want more than 1 cardreader and 1 mailback queue
 	anyway).

 	Login failure notification is sent to the CLUSTER operator console(s).

 	To Build:
 		$ FORTRAN 	CARDSMB.FOR
 		$ LINK/NOTRACE  CARDSMB.OBJ
 		$ copy cardsmb.exe sys$system

 		(notrace prevents a lot of errors from JOB CONTROL should
 		 the program terminate abnormally)

 	To setup:
 	 	$ INIT/QUEUE/PROC=CARDSMB/ON=CARDIN  CARDREADER
 	 	$ INIT/QUEUE/PROC=CARDSMB/ON=MAILOUT MAILBACK


 	To Test PRINT the following file to the CARDREADER queue:
 	 	$ JOB username /QUEUE=SYS$BATCH/PRINT=MAILBACK
 	 	$ PASSWORD password
 	 	$
 	 	$ dir
 	 	$
 	 	$ eoj

	The file will be processed by CARDREADER, and be SUBMITed to
	SYS$BATCH.  After it executes, the log file will be mailed
	back to "username".

 	If you end up using this program, please drop a note to me at
        the above addresses so I get a warm fuzzy and can make you aware
	of updates/changes.

