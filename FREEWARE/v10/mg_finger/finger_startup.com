$ sv = 'f$verify(0)'
$! FINGER_STARTUP.COM
$!
$!  Copyright © 1994, MadGoat Software.  All rights reserved.
$!
$!  Starts up the Finger client and server.
$!
$! You can restrict access to the finger server by uncommenting the
$! following logical name definitions, and modifying them as needed.
$!
$!$! DEFINE/SYSTEM FINGER_NETMASK "255.255.0.0"				!LOCAL
$!$! DEFINE/SYSTEM FINGER_LOCAL   "128.113.0.0","128.213.0.0"		!LOCAL
$!
$! FINGER will use MX_VMSMAIL_LOCALHOST, MULTINET_HOST_NAME, and
$! UCX$INET_HOST, in that order, to determine the node name for the
$! mailing address.  If you don't have MX, MultiNet, or UCX, then
$! define the following:
$!
$!$ DEFINE/SYSTEM MX_VMSMAIL_LOCALHOST "@WKUVX1.WKU.EDU"		!LOCAL
$!
$!
$! By defining the following logical name, "FINGER username"
$! with no host name will be equivalent to "FINGER username@default_host".
$!
$!$ DEFINE/SYSTEM FINGER_DEFAULT_HOST  "WKUVX1.WKU.EDU"			!LOCAL
$!
$! You may want to INSTALL the FINGER utility with WORLD so users can
$! get information about the local system:
$!
$! ICMD := CREATE
$! IF F$FILE_ATTR ("MADGOAT_EXE:FINGER.EXE","KNOWN") THEN ICMD := REPLACE
$! INSTALL 'ICMD MADGOAT_EXE:FINGER.EXE/PRIV=WORLD			!LOCAL
$!
$! The value of FINGER_NETMASK is ANDed with the address of the requesting
$! host.  The result of the AND operation must equal one of the masks
$! specified in the FINGER_LOCAL search list in order for the request
$! to be honored; otherwise the message "Access denied" is returned
$! to the requester.
$!
$! FINGER_SERVER requires SYSPRV to be able to open the listener on
$! TCP port 79, and requires WORLD privilege to collect information
$! about other users.
$!
$! You may substitute another UIC for [200,200] below -- just make sure
$! it is not a system UIC (any UIC with group less than or equal to
$! the SYSGEN parameter MAXSYSGROUP).  We do this to ensure that only
$! those PLAN files that have at least W:R access can be read by the
$! server.
$!
$ START = "RUN/AST_LIMIT=100/BUFFER=32768/ENQUE=300/EXTENT=1024"+-
    "/FILE_LIM=40/IO_BUF=50/IO_DIR=50/JOB_TABLE=0/MAXIMUM=3000"+-
    "/PAGE_FILE=10240/QUEUE=50/TIME_LIMIT=0/DETACH/UIC=[200,200]"+-
    "/PRIV=(NOSAME,NETMBX,WORLD,SYSPRV,TMPMBX)/PRIO=4"
$ START/INPUT=NL:/OUTPUT=NL:/PROC="MadGoat FingerD" -
	MADGOAT_EXE:FINGER_SERVER   	    	    	    	    	!LOCAL
$ exit
