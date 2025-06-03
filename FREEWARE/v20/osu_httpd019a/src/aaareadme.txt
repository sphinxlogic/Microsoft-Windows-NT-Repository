This directory contains the source files for a DECthreads-based HTTP server.

Building images:
    First build the www_server executable, either by MMS with the descrip.mms 
    file or the build_xxxx.com command procedure in the [.BASE_CODE]
    sub-directory.  The commands to do this are:

	(MMS)	$ MMS/MACRO=TCP=xxxx
	(DCL)	$ @BUILD_xxxx.COM

    where 'xxxx' is a code for the TCP package: 
	UCXTCP, MULTINET, CMUTCP, TWGTCP, TCPWARE

    Next set default to the [.SCRIPT_CODE] sub-directory and compile/link the
    script images using either one of the the commands:

	(MMS)	$ MMS/MACRO=TCP=xxxx
	(DCL)	$ @BUILD_SCRIPTS_xxxx.COM

    Finally, set default to the [.FORK_CODE] sub-directory and compile/link the
    script images using either one of the the commands:

	(MMS)	$ MMS
	(DCL)	$ @BUILD_FORKSCRIPTS.COM

Testing server:
    Verify that the server operates by invoking the [.SYSTEM]HTTP_SERVER.COM 
    with the following parameters:

	 @[.SYSTEM]HTTP_SERVER.COM RUN SYS$OUTPUT/3 HTTP_SAMPLE_HOME.CONF 8000

    and then attempt to connect to the server using a WWW client (use url
    "http://your-host-name:8000/").  The "/3" appended to the second parameter
    sets the logging level to 3, increasing the logging level makes the server
    report more information (useful for debugging problems with the configura-
    tion).

    The demonstrations (vmshelp and nagel23a) will not work until the
    DECnet proxy access is set up properly, view the HTML file
    documents/script_setup.html for hints on setting this up.

    Comments at the top of HTTP_SERVER.COM give detailed descriptions of
    the parameters it expects.

Logical names:

    WWW_ROOT		Logical root of web server files.  If not defined
			when invoked, http_server.com defines it as parent
			directory of directory containing http_server.com.
			
    WWW_SYSTEM		Contains server executables and configuration files.
			Defaults to same directory as http_server.com.

    WWW_SERVER_PORTS	Exec mode logical that lists privileged TCP ports
			http_server.exe is permitted to listen on.

    WWW_DIRSERV_ACCESS	Used by http_dirserv.exe to limit connections to
			the DECnet object is creates.

    WWW_DIRSERV_OBJECT	Translates to name of DECnet object declared by
			http_dirserv.exe (WWWDIR).

Distribution directories:

    [] (WWW_ROOT:[000000])	If login directory, holds WWWEXEC.COM and
				WWWDIR.COM (DECnet task objects).  Default
				location for log files such as access.log.

    [.BASE_CODE] 		Source code for main server and support
				programs such as privrequest.c.

    [.BIN]			Default location for scripts invoked via
				URL paths starting with /htbin.

    [.DOCUMENTS]		Holds HTML documents used in sample
				configuration.  These documents describe
				the server's features as they demonstrate
				them.

    [.SCRIPT_CODE]		Source code for scripts run from the WWWEXEC
				object (i.e. any code that uses scriptlib.c,
				or cgilib.c).

    [.SYSTEM] (WWW_SYSTEM)	Holds procedures, executables, and
				configuration files for running server.

Installing server:
    The best way and safest way to install the server is to create a dedicated
    non-privileged account, duplicating the distribution file tree in this
    account's login directory.

    Define DECnet proxy access to the server account from both the server
    account and the SYSTEM account for the current node, making the 
    proxy from the server account a default proxy (/default).  The comments
    at the top of HTTP_STARTUP.COM give more details on creating the
    proxy records via AUTHORIZE.

    Copy HTTP_STARTUP.COM to sys$manager and modify your system startup 
    procedures to invoke it at system startup, specifying the parameters as 
    directed by the comments at the top of HTTP_STARTUP.COM.  HTTP_STARTUP.COM
    will defines the logicals listed above as system-wide logical names.

    Create a rules file for your site, using http_sample_home.conf as a 
    starting point.  The rules file format is similar to the CERN http server 
    format, see file [.DOCUMENTS]CONFIGFILE.HTML for descriptions of the
    individual rules.

Shutting down/Restarting server:
    Management commands are sent to the running server using the privrequest
    program.  Privrequest must be invoked as a foriegn command with the 
    following arguments:

	$ privrequest local-port "command" remport

    where:
	local-port	Local port number server recognizes as management port
			(HTTP_SERVER.COM configures server to use port 931).
	"command"	Managment command: HELP, SHUTDOWN, RESTART, ...
	remport   	Option port if server not running on port 80.

Mailing list:
    Hunter Goatley has created a mailing list for discussion of issues related
    to DECthread HTTP server. To subscribe send a command like the following 
    in the body of a mail message to MXserver@WKUVX1.WKU.EDU: 

	SUBSCRIBE VMS-WEB-daemon "Your real name here" 

Release notes:
    See the file aaareadme.release_notes for a summary of fixes and
    enhancments made to the various releases.

-----------------------------------------------------------------------------
David L. Jones               |      Phone:    (614) 292-6929
Ohio State Unviversity       |      Internet:
2070 Neil Ave. Rm. 122       |               jonesd@kcgl1.eng.ohio-state.edu
Columbus, OH 43210           |               vman+@osu.edu
