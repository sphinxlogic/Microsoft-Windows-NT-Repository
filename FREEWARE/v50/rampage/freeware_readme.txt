WATCHDOG, SYSTEM_MGMT, Events monitor with system manager alerts

        This is WATCHDOG V2.0-2.

        WATCHDOG is a utility for OpenVMS that performs selected
        functions at repeated intervals to monitor system events and
        activities that require the attention of system managers. 
        Problem alerts that are triggered by WATCHDOG may be delivered
        to one or more system managers via any combination of pagers,
        MAIL, REPLY, and OPCOM.

        WATCHDOG was developed by Ergonomic Solutions as a freeware
        system management utility for customers of their RamPage paging
        software product.  However, this utility does not require the
        installation of RamPage for problem alerts to be delivered via
        the OpenVMS MAIL, REPLY, and OPCOM facilities.

        For RamPage customers, this utility is better known as the
        "RamPage Watchdog Dispatcher", and therefore, this document
        uses that name in the sections that follow.  They are:

		1. RamPage Watchdog Dispatcher Operation
		2. Designating Events to Watch
		3. Sample Configurations
		4. Real-Time Watchdog Monitor for Video Terminals
		5. Contacting the Author


	------------------------------------------------------------------
	Section 1 - RamPage Watchdog Dispatcher Operation
	------------------------------------------------------------------

        The RamPage Watchdog Dispatcher is provided in the form of an
        extensive Digital Command Language (DCL) command procedure
        called WATCHDOG.COM.  This file is a template which can be
        customized for your own use by selecting certain automated
        functions.

        These functions may be used in any combination to perform
        lights-out operations, system management, and housekeeping
        operations. You must customize your watchdog setup to enable
        the desired functions, described in Table 1:

        Table 1: RamPage Watchdog Dispatcher Functions
        ------------------------------------------------------------------
        Function   Parameter(s)        Description
	------------------------------------------------------------------
	INTERVAL   period [refresh]    Sets the interval period between
                                       successive problem checks. The
                                       period must be specified as hours,
                                       minutes, and seconds in the format
                                       HH:MM:SS. The default interval
                                       period is 10 minutes.

                                       An optional refresh period may also
 				       be specified in the format
                                       HH:MM:SS. This value determines how
                                       frequently the countdown status is
                                       refreshed for viewing by RamPage
                                       Watchdog Monitor utility. The
                                       default refresh period is 5
                                       seconds. New values must not be
                                       greater than the interval period,
                                       and should be evenly divisible by
                                       it.
                                       -----------------------------------
        
        NOTIFY     username_list       Sets the usernames in the given
                    delivery           list as the receivers of problem
                                       reports, with delivery method as
 			               RAMPAGE, MAIL, REPLY, and/or OPCOM.

                                       (NOTE: RAMPAGE requires Ergonomic
                                       Solutions' RamPage paging
                                       software).
                                       -----------------------------------
        
        SYSBOOT    (none)              Checks and reports the system boot
                                       time upon startup of the watchdog.
                                       Useful in detecting unexpected
                                       reboots.
                                       -----------------------------------

        DISKS      fill_percentage     Checks the fill percentage of
                    [device]           mounted disk drives. Any which
                                       exceed the given value are
                                       reported. A full or partial device
 				       name may be provided to override
                                       the default of checking all disks.

                                       If a partial device name is given,
                                       all devices whose device names
                                       begin with what you entered.
                                       -----------------------------------

        NODES      node_list [method]  Checks all nodes in the given list
                                       and reports any that aren't
                                       reachable. If a method is provided,
				       it indicates how the node(s) are
                                       queried for detection, using the
                                       keywords DECNET (default), CLUSTER,
                                       or NCP.
                                       -----------------------------------

        INTRUSIONS type                Checks the intrusion database and
				       reports on any new entries. You
                                       must specify an intrusion type of
                                       ALL, SUSPECT, or INTRUDER.
                                       -----------------------------------

        DEVICE     (none)              Checks all known devices and
                                       reports changes in error counts.
                                       -----------------------------------

        LICENSE    (none)              Checks the license database and
                                       reports any products due to expire.
                                       -----------------------------------

        USERNAME   username_list       Checks all usernames in the given
 				       list and reports any that are
                                       currently on the system.
                                       -----------------------------------

        PROCESS    process_list action Checks all processes in the given
        			       list for actions REPORT_MISSING or
                                       REPORT_PRESENT.
                                       -----------------------------------
                                       
        QUEUE      status queue        Checks the given queue and reports
                                       if the status becomes one of
				       MISSING, STOPPED, STALLED,
                                       RESETTING, CLOSED, PAUSED,
                                       STARTING, STOPPING, or IDLE.
                                       -----------------------------------

        PING       address_list        Checks all remote TCP/IP addresses
				       in the given list and reports any
                                       that fail to respond.
	------------------------------------------------------------------


	------------------------------------------------------------------
	Section 2 - Designating Events to Watch
	------------------------------------------------------------------

	1. To setup the watchdog, use a standard text editor to edit the
	   template file WATCHDOG.COM. Specify one or more desired
	   functions via the PERFORM keyword using the following syntax:

     	   $ PERFORM function-1 [parameter1] [parameter2] [parameter-n]
     	   $ PERFORM function-2 [parameter1] [parameter2] [parameter-n]
               .
               .
               .
     	   $ PERFORM function-n [parameter1] [parameter2] [parameter-n]

           All PERFORM's must be created in the appropriate section of
           the template, indicated by the text "- Start Marker -" and
	   "- End Marker -".  For more information, refer to the sample
	   configurations in the next section.

        2. To start a watchdog process, execute a command similar to the
           following at the DCL level, or somewhere within the system
           startup procedures:

           $ RUN/DETACHED/UIC=[1,4]                                -
               /INPUT=device:[directory]WATCHDOG.COM               -
               /OUTPUT=NL:                                         -
               /ERROR=device:[directory]WATCHDOG.ERR               -
               /PROCESS_NAME="Watchdog"                            -
              SYS$SYSTEM:LOGINOUT.EXE

        3. The watchdog owner process must have the following privileges
           to perform all functions: SECURITY, CMKRNL (intrusions),
           SYSPRV, OPER (queue queries).

        4. The interval period can't vary between functions, since the
           interval period pause occurs once at the end of the function
           list. However, to perform functions at different interval
           periods, simply run multiple watchdog processes with separate
           function lists and unique process names.

        5. A valid RamPage license is not required unless you specify
           RAMPAGE as the delivery method for the NOTIFY function.


	------------------------------------------------------------------
	Section 3 - Sample Configurations
	------------------------------------------------------------------

	Sample 1:

	$ ! - Start Marker -
	$ !
	$ PERFORM INTERVAL     00:05:00
	$ PERFORM NOTIFY       "FOX" "MAIL"
	$ PERFORM DISKS        80
	$ !
	$ ! - End Marker -

        The watchdog enabled by this sequence sets the interval to five
        minutes, and username FOX as the recipient of problem notices
        delivered via OpenVMS mail. The only function performed is a
        check for disks which exceed 80% of their capacity.


	Sample 2:

	$ ! - Start Marker -
	$ !
	$ PERFORM INTERVAL     00:10:00
	$ PERFORM NOTIFY       "TYLER,GARRISON" "MAIL,REPLY"
	$ PERFORM LICENSE
	$ PERFORM NODES        "STAR,SATURN,PISCES,VENUS"
	$ PERFORM NODES        "JUPITER,MARS,PLUTO,MERCURY"
	$ PERFORM NODES        "URANUS,EARTH"
	$ PERFORM DEVICE
	$ !
	$ ! - End Marker -

        The watchdog enabled by this sequence sets the interval to ten
        minutes, and usernames TYLER and GARRISON as the recipients of
        problem notices delivered via MAIL and REPLY. Functions checked
        include licensing, inactive nodes, and device errors.


	Sample 3:

	$ ! - Start Marker -
	$ !
	$ PERFORM INTERVAL     00:10:00
	$ !
	$ PERFORM NOTIFY       "WALKER" "MAIL"
	$ PERFORM QUEUE        "STOPPED" "INVOICE_PRINTER"
	$ PERFORM QUEUE        "STOPPED" "CHECK_PRINTER"
	$ PERFORM QUEUE        "STOPPED" "PO_PRINTER"
	$ PERFORM PROCESS      "Payroll_Update" "REPORT_PRESENT"
	$ !
	$ PERFORM NOTIFY       "GARRISON" "RAMPAGE"
	$ PERFORM SYSBOOT
	$ !
	$ PERFORM NOTIFY       "GARRISON" "RAMPAGE,OPCOM"
	$ PERFORM INTRUSIONS   "ALL"
	$ PERFORM DEVICE
	$ PERFORM DISKS        95
	$ PERFORM USERNAME     "FIELD,SYSTEM"
	$ PERFORM PROCESS      "OPCOM" "REPORT_MISSING"
	$ !
	$ PERFORM NOTIFY       "GARRISON" "MAIL,REPLY"
	$ PERFORM DISKS        75
	$ PERFORM LICENSE
	$ PERFORM NODES        "MICKY,MINNIE,PLUTO,DAFFY"
	$ !
	$ ! - End Marker -

        The watchdog enabled by this sequence sets the interval to 10
        minutes, and demonstrates how to monitor several functions, with
        changing recipients and delivery methods.


	------------------------------------------------------------------
	Section 4 - Real-Time Watchdog Monitor for Video Terminals
	------------------------------------------------------------------

        A Watchdog Monitor is provided to view the real-time operation
        of the Watchdog Dispatcher.  If you are licensed to use RamPage,
        you can invoke the monitor with following DCL command:

	$ RAMPAGE/MONITOR=WATCHDOG [Return]

        If you are using the freeware version of the Watchdog
        Dispatcher, you can invoke the monitor by executing the image
        RAMPAGEWDM.EXE directly:

	$ RUN device:[directory]RAMPAGEWDM.EXE [Return]

        ...or from a DCL symbol:
        
	$ WATCHDOGMONITOR :== device:[directory]RAMPAGEWDM.EXE [Return]
	$ WATCHDOGMONITOR [Return]

        The Watchdog Monitor requires OpenVMS/Vax V5.5-2H4 or higher,
        or OpenVMS/Alpha V6.1 or higher.


	------------------------------------------------------------------
	Section 5 - Contacting the Author
	------------------------------------------------------------------

        WATCHDOG was written by Ergonomic Solutions.  It may be freely
        distributed and copied, provided no charge is made for use or
        for installation of such distribution, and the embedded
        copyright notice is retained.

        Enhanced versions and other derivative works may also be freely
        distributed and copied under the same restrictions, provided full
        documentation of such changes is included with the authors' names
        and contact information.

	Under no circumstances may revenue be collected as a result of
        the usage of WATCHDOG or its derivative works without the prior
        written consent of Ergonomic Solutions.

        To report bugs, please send e-mail to support@ergosol.com.

        For WATCHDOG updates, visit web site:

        	www.ergosol.com/www/watchdog.html

        For information about RamPage, send e-mail to info@ergosol.com,
        visit web site www.ergosol.com, or telephone (800)ERGO-SOL.
        
