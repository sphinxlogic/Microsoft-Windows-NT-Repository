	    --------------- 1ST_README.TXT -------------------------------
	This file explains how to set up and run the DECtalk-Telecomm
	demonstration program.

	CONNECTING THE DECTALK TO THE MicroVAX II 
	-------------------------------------------------------------------
	1.  Log into the account on the system containing the demo.

	2.  If the Archer 4 line tap telephone adaptor is being used,
	    the "free ends" of the two RS232 cables connected to
	    the MicroVAX II or the VAX should be connected to 
	    DECtalk module 1 (connector J1) and module 4 (connector J4).
    
	3.  The Archer 4 line tap telephone adaptor (from Radio Shack)
	    only runs 2 telephone lines.  LINE 1 on the Archer Line Tap
	    is for the demo running on DECtalk module 1 and LINE 2 on 
	    the Archer Line Tap is for the demo running on DECtalk module 4.

	4.  Modify the configuration file (telecomm.cfg) to contain
	    the terminal lines that the RS232 cables for each of the
	    DECtalk demos is connected to on the MicroVAX.  That is,
	    if the DECtalk module running the "telecomm" demo is 
	    connected to terminal line "txb0:", then the configuration
	    file (telecomm.cfg), should read:

	    TXB0:

	5.  Set the device protection of each of the terminal lines 
	    specified in the configuration file to WORLD READ/WRITE
	    access.

	    For example, for the above configuration, type

	    $ set protection=(w:rw)/device txb0:

	    (consult the DCL manual for more detailed information)

	6.  To start up the demos, enter the following two DCL commands:

	    $ set proc/priv=all
	    $ @telecomm telecomm.cfg

	8.  The following will come back after these commands are entered:

	    %RUN-S-PROC-ID, identification of created process is 00000xxy

	    which indicates that the DECtalk demo has been started.

	9.  If you enter the DCL command, "show system" you should see a
	    process with the name "DEMO_txxy" in the listing.  Note, 
	    "txxy" is the terminal line that the DECtalk module is connected
	    to on the MicroVAX.  If this process name is missing, then the
	    demo has stopped running because some error has occured.  To
	    see which errors have occured, type the error file "txxy.err"
	    where "txxy" is the terminal line the DECtalk module is 
	    connected to.

	10. If you want to stop running the demo enter the following
	    DCL command:

	    $ stop demo_txxy

	    where "txxy" is the terminal line the DECtalk module is 
	    connected to.

	11. After the process has been stopped, type the following DCL
	    command:

	    $ copy TXyy: nla0:

	    where again, "TXyy:" is the name of the terminal line
	    that the DECtalk is connected to.  After about 5 seconds,
	    press either CTRL/C or CTRL/Y.  (The copy command hangs
	    until either of these commands is entered).  Pressing the
	    CTRL/C or CTRL/Y should cause the DCL ($) prompt to be
	    displayed again.

