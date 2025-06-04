	    --------------- 1ST_README.TXT -------------------------------
	This file explains how to set up and run the DECtalk health
	care demonstration programs.

	CONNECTING THE DECTALK TO THE MicroVAX II 
	-------------------------------------------------------------------
	1.  Log into the account on the system containing the demo.

	2.  Modify the configuration files (health_care.cfg) and
	    (results.cfg) to contain the terminal lines that the 
	    RS232 cables for each of the DECtalk demos is connected
	    to on the MicroVAX.  That is if the DECtalk module running
	    the "health_care" demo is connected to terminal line txb0:,
	    then the configuration file, health_care.cfg, should read:

	    TXB0:

	    Likewise, if the DECtalk module running the "laboratory
	    results reporting" demo is connected to terminal line
	    txb1:, then the configuration file, results.cfg, should read:

	    TXB1:

	3.  Set the device protection of each of the terminal lines 
	    specified in the configuration file to WORLD READ/WRITE
	    access.

	    For example, for the above configuration, type

	    $ set protection=(w:rw)/device txa0:
	    $ set protection=(w:rw)/device txa1:

	    (consult the DCL manual for more detailed information)

	4.  Set up an operator terminal to receive the "OPER11" type
	    of message.  To have error messages logged to the console
	    terminal, or any other terminal, type the REPLY/ENABLE
	    command at the DCL ($) prompt. 

	    $ reply/enable=oper11


	    Note, OPCOM must be running on your system for error messages
	    to be logged to an operator terminal.	

	5.  To start up the demos, enter the following two DCL commands:

	    $ set proc/priv=all
	    $ @health_care health_care.cfg
	    $ @results results.cfg

	6.  The following will come back after these commands are entered:

	    %RUN-S-PROC-ID, identification of created process is 00000xxy
	    %RUN-S-PROC-ID, identification of created process is 00000xxz

	    which indicates that the DECtalk demos have been started.

	7.  If you enter the DCL command, "show system" you should see one
	    process with the names "DEMO_txxy" and "DEMO_txxz" in the 
	    listing.  Note "txxy" and "txxz" are the terminal lines that 
	    the DECtalk modules are connected to on the MicroVAX.  If these
	    process names are missing, then the demo(s) have stopped running
            because some error has occured.  To see which errors have 
	    occured, type the error file "txxy.err" where "txxy" is the 
	    terminal line the DECtalk is connected to.

	8.  If you want to stop running the demos enter the following
	    DCL command:

	    $ stop demo_txxy
	    $ stop demo_txxz

	    where "txxy" and "txxz" are the terminal lines the
	    DECtalk modules are connected to.

	9. After the process has been stopped, type the following DCL
	    command:

	    $ copy TXyy: nla0:

	    where again, "TXyy:" is the name of the terminal line
	    that the DECtalk is connected to.  After about 5 seconds,
	    press either CTRL/C or CTRL/Y.  (The copy command hangs
	    until either of these commands is entered).  Pressing the
	    CTRL/C or CTRL/Y should cause the DCL ($) prompt to be
	    displayed again.

