This directory contains the source files for a DECthreads-based HTTP server.

Building images:
    First build the www_server executable, either by MMS with the descrip.mms 
    file or the build_xxxx.com command procedure.  The commands to do this
    are:

	(MMS)	$ MMS/MACRO=TCP=xxxx
	(DCL)	$ @BUILD_xxxx.COM

    where 'xxxx' is a code for the TCP package: 
	UCXTCP, MULTINET, CMUTCP, TWGTCP, TCPWARE

    Next set default to the [.SCRIPT_CODE] sub-directory and compile/link the
    script images using either one of the the commands:

	(MMS)	$ MMS/MACRO=TCP=xxxx
	(DCL)	$ @BUILD_SCRIPTS_xxxx.COM

Testing server:
    Verify that the server operates by invoking the HTTP_SERVER.COM with
    the following parameters:

	 @HTTP_SERVER.COM RUN SYS$OUTPUT/3 HTTP_SAMPLE_HOME.CONF 8000

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
    directed by the comments at the top of HTTP_STARTUP.COM.

    Create a rules file for your site, using http_sample_home.conf as a 
    starting point.  The rules file format is similar to the CERN http server 
    format.  A summary of the rule file directives is included as comments 
    in the file http_sample_home.conf.

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
