This is the distribution of CRINOID --- a multi-threaded Perl CGI
Server for the OSU HTTPd.

TABLE OF CONTENTS:
     1. What you'll need first
     2. The CRINOID processes
     3. DOING THE INSTALLATION
     4. EDITING CRINOID.CONFIG
     5. STARTING IT UP
     6. RUNNING MULTIPLE CRINOIDs
     7. Changing Configurations
     8. A Typical Configuration
     9. Setting up CGI scripts
    10. Licensing
    11. Acknowledgements
    12. Change History
    13. Needs Doing



1. What you'll need first:
==========================


0. A VMS system, preferably an Alpha...the code *should* compile on
    VAXen, but it's mostly untested.   VMS >= 6.2 is preferred. Again,
    it's untested on earlier VMS versions, and anything <6.0 will
    likely have problems with missing features (such as the
    READERCHECK flag on MBX reads).

    You'll need the DEC C compiler. I'm pretty sure VAXC won't do the
    trick, and I've never gotten GNUC to work for me on VMS.

    Patches to extend workability to earlier VMS versions, VAXen, and
    other compilers are welcome!

1. The OSU HTTP server: http://kcgl1.eng.ohio-state.edu/www/doc/serverinfo.html

    While nearly all versions of the OSU HTTP server should work fine
    with CRINOID, if you're going to do "tricky" HTTP requests via
    CGI...such as implementing Netscape Roaming, a patch to the file
    [.base_code]script_cgiexecute.c may be needed.  Patch included
    with this distribution (SCRIPT_CGIEXECUTE-19990611.PATCH)

    The patch is for OSU HTTP server version 3.3b, but almost
    certainly applies to previous 3.x releases...and perhaps later
    releases as well.  You'll mostly need it if you're doing tricky
    stuff with methods other than the "standard" ones (GET,HEAD,POST).

2. Perl, preferably a recent version: http://www.cpan.org/src/stable.tar.gz

    CRINOID requires Perl 5.003 or newer, because of the use of Embed.
    It's known to work with Perl 5.6.0, and intermediate versions.

    A patch to Opcode.xs used by Safe:: is required for CRINOID.
    This patch does two things: it makes scripts running in a Safe::
    partition think that they are in "main::" (important for module
    use in scripts) and fixes a problem with _ use in scripts.  If
    you have already patched Opcode.xs with an older patch that doesn't
    take care of the _ problem, don't worry...you'd likely only run into
    it while trying to upload files using CGI.pm.

    The CONFIGURE procedure will try and figure out the patchlevel of your
    Opcode.xs and tell you if you need a patch.  If you've already patched
    and need the _ fix, apply the new patch to a virgin (unpatched) version
    of Opcode.xs.   And if you've "installed" your Perl so that you don't
    have a copy of Opcode.xs sitting around, grab a copy from CPAN and patch
    it.

    Just make sure that after patching you do the whole "build/install" thing
    to get OPCODE.EXE into the right place in PERL_ROOT:[LIB...].  It's
    good to do a directory before and after to make sure that you've got
    a new OPCODE.EXE in the right place.

    If you're building a fresh copy of Perl, apply the patch before the
    main Perl build.

    Opcode.xs patches provided with this distribution:
        OPCODE_500557_*.PATCH    patch for Perl v 5.005.57 and earlier
        OPCODE_500559_*.PATCH    patch for Perl v 5.005.59 and later
    (one of these *should* work for 5.005.58!)

    To get the maximum benefit from Perl CGIs, you'll probably want
    some or all of the following Perl modules:

    CGI.PM
    MIME::Base64
    Digest::MD5
    libwww
    GD
    PGPLOT

Please check the CRINOID web page (http://www.crinoid.com/) for
module compatibility status, pointers to useful utilities, etc.

2. The CRINOID processes:
========================

    CRINOID   ---> persistent DECnet server, multithreaded, that processes
                  requests from the OSU server, divvies up the work
                  among a number of TENTACLES and manages the various
                  processes that actually do the work.

    TENTACLE ---> a process running in a 'user' account with Perl
                  (plus modules) pre-loaded.  Gets the request data from
                  CRINOID, passes it into Perl and manages the various
              +-- startup/shutdown and i/o needed for the CGI.
              |
              +-> the Perl code in TENTACLE is 'OYSTER'; it sets up
                  Safe:: partitions for the CGI scripts, locates them,
                  sets up their $ENV, preloaded modules, etc.
                  Acts as a hard-shelled "wrapper" for a Perl.

Helper programs:

    STUB     ---> temp program used to start a TENTACLE in user context
                  as a detached process.

    LOGGER ---> records logging information from CRINOID and TENTACLE,
                  started automatically by CRINOID if not already present.

There's probably a workaround for STUB, but the difficulty is that you
want user 'A' to start a detached process for user 'B', where the new
process does NOT have any extra privs.  The CREPRC system service call
lets you start processes with a different UIC, but not username; setting
the username is typically a CMKRNL call, and if you don't have privs it's
no-go.

The way around this is for CRINOID to start STUB; a very simple
detached process that has enough privs that it can "IMPERSONATE" the
target user and start a detached (and unpriv'ed) TENTACLE process for
that user.  You don't want to IMPERSONATE from CRINOID, since *all* of
the threads get the same impersonation (n.b.: perhaps changed in VMS 7.2).
You also don't want TENTACLE to have the privs to IMPERSONATE, ergo
the STUB workaround.  A kernel-mode workaround for pre-6.2 systems is
included, the configuration script will check if your system can IMPERSONATE
or not.

LOGGER was created as a separate process so that we can keep logging
even if TENTACLE or CRINOID crashes.

TENTACLE's that are started for a user will execute their LOGIN.COM,
so any special setup (stuff like PGPLOT setup, etc) should be done there;
it'll be a 'network' login so should be absolutely minimal for speed
and robustness.  You can also do any "all-TENTACLE" setup in the
TENTACLE.COM file in CRINOID_HOME.

A user with CGI's should also consider having an "OYSTER.CONFIG" file
in their home directory.  The OYSTER.CONFIG can set the logging file
for script STDERR output, logging level,  preload Perl modules, and
tell Oyster what logicals to import into script %ENV.

An example OYSTER.CONFIG is provided in the [.MISC] directory.  Having
your script send stuff to STDERR, which then gets logged in a file is
currently one of the better ways of debugging CGIs...at least until we
get TkPerl working on VMS.

3. DOING THE INSTALLATION
=========================

The following steps are required:
        @configure          & answer the questions (creates WWWPERL.COM
                                and CONFIGURE.MMS, tests C-compiler and
                                system capabilities)
        @build              to build CRINOID,TENTACLE,STUB and LOGGER
                                executables
        @install            to move files to CRINOID directory,
                                WWWPERL.COM to WWW_ROOT:[000000] (optionally)

You can then
        @build tidy     (or 'clean' or 'realclean')
to clean up the build area.

Note that @configure and @build only create files in the CRINOID build
directory (where CRINOID was unpacked), and @install only creates files
in the CRINOID install directory selected during @configure,  plus
optionally adding WWWPERL.COM to WWW_ROOT:[000000]

After installation:
    1.  Move WWWPERL.COM to the HTTP server `home' directory
        (WWW_ROOT:[000000]) if not already done by @INSTALL.

    2.  Add one or more `EXEC' rules to the HTTP server config files to
        direct appropriate URLs to CRINOID.  This is usually to be found
        in WWW_ROOT:[SYSTEM]HTTP_SCRIPTS.CONF, something like:

.NEXT       exec /cgi/*            0::"0=WWWPERL"nl:

        if you're using the "metaconfig" stuff...otherwise just a

exec /cgi/*            0::"0=WWWPERL"nl:

        You need an `EXEC' entry for each set of URLs that you want to
        have CRINOID handle, for example:
            exec /cgi/*       0::"0=WWWPERL"nl:
            exec /~joe/cgi/*  0::"0=WWWPERL"nl:

    3.  Edit CRINOID.CONFIG in the CRINOID_HOME directory to set the
        mapping between URLs handled by CRINOID and the location of the
        Perl scripts, and how many processes owned by whom, etc. will
        be running the scripts.  See below (and the supplied CRINOID.CONFIG)
        for more information.

    4.  Add calls to @CRINOID_HOME:STARTUP RUN to your code for starting the
        HTTP server on system startup, and @CRINOID_HOME:SHUTDOWN FULL
        when the HTTP server is shut down.

    5.  Do a @STARTUP of the CRINOID server...this will define logicals
        and install the CRINOID processes with the privs they need.

    6.  Restart the OSU HTTP server to reload the config files.

The CRINOID processes will be started when an HTTP request for one of the
CGI's that it handles is received.

Uninstallation:
---------------
A clean uninstall is quite easy:

    1.  remove "exec" lines in OSU HTTP config that reference WWWPERL,
        and restart OSU HTTP server.

    2.  remove WWWPERL.COM from WWW_ROOT:[000000]

    3.  do a @CRINOID_HOME:SHUTDOWN FULL  if CRINOID is running

    4.  delete files in CRINOID_HOME:

A note about privileges needed:
--------------------------------
A serious attempt has been made to minimize the priv's required to
run CRINOID, and to leave those privs OFF unless needed for specific
operations.  This, of course, means that changes to VMS may result in
needing more (or different) privs than what I anticipated.  NOTE that
I assume that all processes also have the usual NETMBX and TMPMBX privs.

CRINOID:    SYSNAM                      DECNET ACPCONTROL, commands to LOGGER
            LOG_IO,PHY_IO               DECNET I/O
            DETACH                      starting LOGGER, STUBs
            WORLD                       querying/shutting down TENTACLES
            SYSLCK                      for locks used with LOGGER

LOGGER:     SYSNAM                      MBX name, etc. in system table
            SYSLCK                      coordinate with CRINOID

STUB:       SYSPRV                      "username" impersonation
            +  IMPERSONATE [v7.2]
            +  CMKRNL  [pre v6.2]       if sys$impersonate not available
            DETACH                      starting detached TENTACLES


Some of these (The *_IO privs) were found by trial and error, since
the VMS documentation was a bit vague.  They didn't need to be explicitly
*set*, just available as an "authorized" priv.

It is strongly suggested that you run CRINOID from an unpriv'ed
account (and your HTTP server too) with just NETMBX and TMPMBX, and
have the images installed with the minimal set of privs needed for
operation.

The one case where you can not operate like this is if you are
doing program debugging, since images that are linked /TRACEBACK
can't be installed with privs.  See the files in [.debug_support]
of the distribution for more information.

4. EDITING CRINOID.CONFIG
=========================

CRINOID.CONFIG is located in the CRINOID installation directory.  It
tells CRINOID information about which accounts to use for starting
TENTACLE processes, and to direct URLs to the processes will handle
requests (NOTE: reinstallation will NOT overwrite an existing CRINOID.CONFIG)

A simple example, one would like
        http://example.com/cgi/~joe/script1.pl      to run in JOE's account
and
        http://example.com/cgi/~fred/script2.pl    to run in FRED's account

A set of TENTACLE processes running under a single account, with a
common set of privs and quotas, I call a "process group".  The
"process groups" are set up in CRINOID.CONFIG, and their mapping to
URLs is also defined.

When a CGI request comes in, CRINOID determines the process group from
the URL, then tries to find an idle process in that group, starting a
new TENTACLE process if necessary.  The request is handed off to the
idle TENTACLE, which calls its embedded Perl interpreter to invoke the
OYSTER script that actually runs the CGI.

You can set parameters for the process groups to determine the max/min
number of processes in that group; how many of them (max/min) can be
idle, etc.

This allows one to tune resource use for particular CGIs: a "high
availability" CGI might have min_idle = 2 so that there are always a
couple of idle processes ready to process requests.   A typical
"user" CGI process group (used infrequently, longer response time
acceptable) would have max_idle = 0  and max_processes = 1

In addition to specifically designated process groups one can also
have a "user" process group, where the username is filled in
dynamically from the URL...the CGI equivalent of /~user/stuff.html
processing.  Note that the individual processes that handle these CGI
requests are kept strictly separate (JOE's processes handle requests
to ~joe, FRED's to ~fred, etc.)  but they are handled with a common
set of configuration parameters.  This is implemented with the
USERGROUP statement in the CRINOID.CONFIG file; you use it to
designate a subdirectory name to look for the CGI scripts, and then
use a ".SCRIPT" command to designate the URLs that are to be run in
the USERGROUP.

For more details, look at the CRINOID.CONFIG with this distribution.

5. STARTING IT UP
==================
After installing CRINOID, doing the necessary configuration of the
OSU_HTTP server, CRINOID, etc. ...then do @STARTUP in the CRINOID_HOME
directory.  This will just define logicals and install images.

You can also do a @STARTUP RUN  which will generate a faked HTTP
request to get CRINOID running.

Do an HTTP request for a simple CGI with a browser...something like:

TEST.PL:

    #! perl
    print "Content-type: text/plain\n\n";
    print "I am.\n";

If you do a $SHOW SYSTEM you should see the following:

    CRINOID
    LOGGER
    user/CGI01       -> this is a tentacle

If you see:
    STUBxxxxxx
or no ".../CGI.." processes, then there's some problem starting TENTACLEs.
Take a look at the README.TXT in [.DEBUG_SUPPORT].

As CGI requests are made, the number of TENTACLE processes will vary
according to the load. CRINOID will automatically shut down TENTACLE
processes if they are no longer needed, or if they are consuming too
much pagefile quota.

6. RUNNING MULTIPLE CRINOIDs:
=============================
I've only used this for debugging purposes (so one CRINOID can be
messed with without causing problems for normal CGI requests), but it
may be more generally useful.

Here's how to do it: @CONFIGURE, @BUILD, and @INSTALL CRINOID as normal
in the "primary" directory.  Then do another @CONFIGURE, specifying a
different ("secondary") directory.  Also select a different CRINOID
server process name ("CRINOID2") and Decnet service name ("WWWPERL2").
Instead of WWWPERL.COM being created in by CONFIGURE, WWWPERL2.COM
will be created.

@BUILD and @INSTALL to the secondary directory.

Copy the WWWPERL2.COM that is generated in the "secondary" @CONFIGURE
to your OSU HTTP sys$login directory.  Then add "exec" rules in your
OSU HTTP script configuration to invoke WWWPERL2.

Example:
    exec /cgi/*            0::"0=WWWPERL"nl:        ( primary CRINOID)
    exec /alt-cgi/*        0::"0=WWWPERL2"nl:       ( secondary CRINOID)

Make sure that the CRINOID.CONFIG for the secondary server does not
have any "SERVICE ..." lines.  The Decnet service name will be set from
a logical name defined in WWWPERL2.COM.

You will need to adjust urls, etc.  (to handle /alt-cgi/* rather than
/cgi, for example) in CRINOID.CONFIG also.

Then @STARTUP the new CRINOID server.

NOTE: the first CRINOID server to start (in response to a HTTP
request) will start its own LOGGER.  All of the subsequent
CRINOID servers will use the same LOGGER process, so you may want
to edit the LOGGER.COM procedures in all of the CRINOID server
directories to point them to a common log file. (See the definition of
CRINOIDLOG_FILE in LOGGER.COM).

The @SHUTDOWN procedure will only stop the associated CRINOID But
@SHUTDOWN FULL will also stop LOGGER, even if other CRINOID servers
are still running.  As of V0.5-2, one of those other CRINOIDs will
then start a new LOGGER process.  So a complete system-wide shutdown
should do

   [CRINOID_1]@SHUTDOWN
   [CRINOID_2]@SHUTDOWN
          ...
   [CRINOID_N]@SHUTDOWN
   [CRINOID_1]@SHUTDOWN LOGGER

If you need to use different OYSTER.CONFIG's for the two servers
(probably a good idea), simply edit the OYSTER. script in the CRINOID
home directory and change the line (near the beginning) to something like:
    $ConfigFile = 'Oyster2.config';


7. Changing Configurations:
===========================
While scripts and OYSTER.CONFIG files are automatically reloaded if
they are changed, this is NOT the case for changes to system-wide
programs and configuration files.

If OYSTER. or SCRIPT.PM are changed all TENTACLES should be killed,
but it does not require a restart of CRINOID.  See KILL_TENTACLES.COM
in the [.misc] subdirectory of the distribution for a handy-dandy
TENTACLE killer.

If CRINOID.CONFIG or the executables are changed, then the CRINOID
server should be shut down (@SHUTDOWN, use @SHUTDOWN FULL if
executables or logging options were changed).  If the executables are
changed, you need to do a @STARTUP to get the new executables
installed in place of the old ones.

8. A Typical Configuration
==========================

In WWW_ROOT:[SYSTEM]HTTP_SCRIPTS.CONF:
    ...
    exec /cgi/*            0::"0=WWWPERL"nl:
    ...

WWW_ROOT:[SYSTEM]START_SERVER.COM:

    $ here = f$parse("Z.Z;",f$env("procedure")) - "Z.Z;"
    $ x = f$setprv("all")
    $ set def 'here'
    $ set def [-]
    $ base = f$env("default")
    $ set def 'here'
    $!
    $ F = F$SEARCH("DISK$SYSTEM3:[CRINOID]STARTUP.COM")
    $ IF F .NES. "" THEN @'F  RUN
    $ @'here'HTTP_Startup HTTP 'base'http_server.log/9 'here'HTTP_MAIN.CONF "80" "443"
    $!
    $ x = f$setprv(x)
    $ exit

CRINOID.CONFIG:
    #
    # very simple, only user-scripts like   /cgi/~fred/tryit.cgi
    #                          mapping to   disk:[fred.www-cgi]tryit.cgi
    #
    LOGLEVEL          4
    USERGROUP         /www-cgi
    .SCRIPT           /cgi/~*
    .processes        0 3
    .PAD              0 1


9. Setting up CGI scripts:
==========================

You can use "standard" (if there is such a thing!) CGI scripts with
CRINOID...two examples:

    #! perl
    print "Content-type: text/plain\n\n";
    print "hello, world!\n";

    #! perl
    use CGI;
    $q = new CGI;
    print $q->start_html('hey there');
    print $q->h1('This is a big title');
    print $q->end_html;


Script reusability
------------------
To improve the performance of your scripts, you should write them
so that they are "reusable"...typically that is when you don't use
variables before they are initialized, since the value might be left
over from a previous run. (And don't count on them being there, either...
two sucessive requests might go to two different TENTACLE processes)

You should be able to run your script interactively with the "-w" flag
set:
    $ perl -w myscript.cgi
and not have any warnings generated.

When your script is well behaved, then put in a line near the beginning:

    $CRINOID::Reuse = 1;

This will tell the server that the script can be reused, at a great savings
in memory use and with much faster startup.

If your script is edited, the server will detect the fact and reload
the newest version.

File extensions
---------------
You can specify which file-extensions to use for CGI scripts in the
OYSTER.CONFIG file:
    Suffixes    .pl .cgi .  # only allow NAME.pl NAME.cgi  NAME. scripts
The URL still has to have the appropriate extension, but requests for
files without an acceptable suffix will be rejected.

         /cgi/mystuff/testit.cgi
         /cgi/mystuff/testit2           -> for "testit2."

Directories and PATH_INFO
-------------------------

If you have subdirectories of your "cgi directory" you can put CGIs in
those subdirectories, and the URLs will need the subdirectory name:

    /cgi/mystuff/testit.cgi         -maps to->  disk:[user.cgis]testit.cgi
    /cgi/mystuff/test/test2.cgi     -maps to->  disk:[user.cgis.test]test2.cgi

The server normally searches for CGI scripts "depth first", so if you have

URL:  /cgi/mystuff/test/xyz/test1

        [user.cgis.test.xyz]test1.        <-- this one gets run
        [user.cgis.test]xyz.                this isn't run
        [user.cgis]test.                       "

You can alter this behavior by setting the "NoDescend" flag in OYSTER.CONFIG;
if "NoDescend" is set, subdirectories will *not* be searched for scripts.

When a script is found, all of the URL to the right of the script name is
available as $ENV{'PATH_INFO'}:

URL:       /cgi/mystuff/test/xyz/something_else/here
script:    [user.cgis.test]xyz.
path_info: something_else/here


Script %ENV
-----------
The CGI scripts normally don't have access to the "real, true" %ENV
(i.e., logicals).  Instead, they get an %ENV that is populated with
the standard CGI variables.  This means that there's no "255-byte"
limitations on CGI variables, but sometimes it's desirable to let
scripts see or change a limited set of logicals.

There's several commands you can put in OYSTER.CONFIG to control this:
Static_ENV, Volatile_ENV, Persistant_ENV, each of which is follwed by
a whitespace list of logical names.

    Static_ENV XYZ
Imports the value of logical XYZ to $ENV{'XYZ'} at each script execution.
Scripts can change $ENV{'XYZ'}, but it does not affect the logical.

    Volatile_ENV XYZ
Imports the value of logical XYZ to $ENV{'XYZ'} at each script execution.
Scripts can change $ENV{'XYZ'}, and the logical XYZ is changed also.  When
the script finishes, the logical XYZ is restored to its original value.

    Persistant_ENV XYZ
Imports the value of logical XYZ to $ENV{'XYZ'} at each script execution.
Scripts can change $ENV{'XYZ'}, and the logical XYZ is changed also.  The
logical XYZ is NOT reset at script termination and will keep the changes
made by the script.

One default setting:  if the logical TZ is not defined, Oyster will
define it by copying UCX$TZ (if present).  TZ is set as a "Volatile_ENV".
This can be overridden with OYSTER.CONFIG.

Modules and @INC
----------------
Modules that are frequently used, especially if used by multiple scripts,
can be pre-loaded to reduce script startup time.   Some script pre-loading
is done in OYSTER, but the scripts preloaded in OYSTER are really a minimal
set that (because of the way Perl loads them) *must* be preloaded if a
script is going to use them.  An example is modules that are linked to Perl
statically, or that OYSTER uses internally.

In OYSTER.CONFIG you can add "Preload" lines that will preload modules
for your scripts:
    Preload     Digest::MD5
    Preload     PGPLOT
    ...

Note that there are some modules (such as CGI) that you should NOT preload.
The CGI module has such tricky symbol export logic (and tends to keep memory
of what it was doing) that preloading just won't work properly.  Modules
that are preloaded are shared among ALL the scripts being run in a given
TENTACLE process, so modules that remember stuff from one run to the next
should be preloaded with caution.

If you preload a module you should also "use" it in your script...it doesn't
hurt in terms of memory or startup time, and it may help initialize the
module properly....as well as making the script so that it can be tested
interactively.

When you try to "use" modules, sometimes it will be necessary to set @INC
appropriately.  The directories on the @INC list are:

        CRINOID_ROOT:[LIB]
        ..the usual Perl directories for @INC...
        the 'home' directory for the CGI's
        directories specified by OYSTER.CONFIG  "AddINC" commands
        directories specified by  #! perl -I ...  line in the script

The script "home" directory is either the base directory for the CGI
(i.e., something like disk:[user.cgis]  ) or the directory specified
in the "HomeDir" command in OYSTER.CONFIG.

If you need to add directories to @INC for multiple scripts, use the
AddINC command.  If it is just one or two scripts, then on the first
line of the script you can put something like:
    #! perl -Idisk:[myincdir.stuff]
    ...rest of script...
Just like you would do from a command line (except you don't need double
quotes around it).

Debugging, Errors, etc.
------------------------
Several OYSTER.CONFIG commands are useful for debugging and keeping
tabs on your scripts:

    Errlog  oyster.log       # log file (in sys$login dir, by default)
    LogLevel 3               # log level, 1=critical...5=babble
    MaxLogfileSize    50000  # create new logfile if old one is too big (size in bytes)
    MaxLogfileVersions  5    # purge logfile versions /keep=...
    MaxRunTime 100           # max time a script can run, in seconds
    Debug m                  # Perl debug flags  (e.g.,  $Perl -Dm )

Use the "debug" command cautiously, some debug options are extremely verbose.

In addition you can specify the "-w" option on the #! line to turn on warning
messages.  This isn't terribly useful for _running_ CGIs, but is good for
testing.

The script STDERR output will be sent to the Errlog file that you set in
OYSTER.CONFIG.   If you don't set an Errlog file, the STDERR output winds
up in CRINOID.LOG where it's much more difficult to decipher.

Sometimes there'll be a nasty, BUG in your CGI.  When that happens,
CRINOID will send back an error message...which may or may not display
properly depending on what stuff has been sent back previously.

For example, having a text error message:

    Error in ... at line ...

doesn't do much good when the script first writes out:

    Content-type: image/gif

At that point it's best to look in CRINOID.LOG for guidance.

If you want to customize the pages generated for errors, there's a
OYSTER.CONFIG command that might help:

ErrorHTML   disk:[dir]errorpage.html    # use this for error messages from OYSTER.

If an error occurs, OYSTER will grab the file specified and substitute:

#   <!--#error-->   ->  text of error message
#   <!--#script-->  ->  name of script

before sending it out as a "text/html" document.


GET, POST, PUT
--------------

When you use a CGI to handle an HTTP request, you really have a lot of
control over what happens.   For example, it's easy to implement your
own authentication system....just check $ENV{'HTTP_AUTHORIZATION'},
maybe redirect to a "https:" protocol, check cookies, etc. ...and send
a "401" if you need the user to enter a password.

You can also implement (using $ENV{'REQUEST_METHOD'}) the POST or PUT
methods...POST gives you form data that you can read from STDIN, PUT
does the same for file uploading.

There's some "nonstandard" methods that Netscape uses to implement
roaming access:  MOVE (renames a file) and DELETE.  I've managed to
do roaming access with a CRINOID CGI...it works fine (even moderately
fast) but your Netscape config will be messed up good if your network
connection goes down in the middle of an up- or download!

If you're interested in applications like this, please let me know and
I'll send you some example code.


10. Licensing:
===============

This program is Open Source, under the general terms of the Perl
Artistic License (http://language.perl.com/misc/Artistic.html) with
one exception: Part #5: "You may charge a reasonable copying fee..."
first two sentences replaced with "You may sell this program as long
as you make available full source code and properly attribute
authorship."

Needless to say, this program is provided "AS-IS" and with no
warrantee of fitness, usefulness, being non-destructive, etc.  Use at
your own risk.

Please inform me of any fixes or modifications you make; and if
you want to be alerted to upgrades or bugfixes let me know that
you're using CRINOID.

11. Acknowledgements
====================

This program owes a tremendous amount to Dave Jones and his work on
the OSU web server (http://kcgl1.eng.ohio-state.edu/www/doc/serverinfo.html).
Astute readers of the code will notice a number of places where
"Dave's code" was the obvious starting point.

Larry Wall's Perl, of course, is a simple necessity of life
(http://www.perl.org), as well of CGI applications.

Charles Bailey (bailey@newman.upenn.edu) provided a much-needed
kickstart to the development of this program, with his hints on what
would work best for good Perl CGI performance on VMS.

The bugs of course, are mine, all mine.  But you're welcome to as many
as you want to take away!

12. Change History:
===================
    0.1  early  1998?   initial version (named `SQUID')
    0.2  fall   1998    major rewrite of thread/piping code
    0.25 spring 1999    logging removed from main program
                        many thread/network timing changes
                        config file capability added
                        OYSTER config files added
    0.3     9 Dec 2000  first public release
    0.3-1  10 Dec 2000  from Emanuele Ruffaldi <pit@sssup1.sssup.it>
                        need _PTHREAD_USE_D4 flag for VMS 7.2
                        destroy_STRING needs to return a value, all cases
           16 Dec 2000  Fix to STUB.C so that it can compile on AXP for
                        VMS 7.2 (extra unused params in IMPERSONATE
                        system service call). STARTUP.COM modified to
                        check for and set IMPERSONATE priv if available.
    0.3-2  13 Jan 2000  from Dr. Martin P.J. Zinser <ZINSER@sysdev.exchange.de>
                        VMS::Filespec needed preload, in OYSTER
                        also copy TZ logical into OYSTER Safe:: partitions
                        SHUTDOWN.COM get the single quotes right
                        INSTALL.COM  location of WWWPERL.COM
                        CONFIGURE.COM ask about 'installlib' directory
    0.3-3  20 Jan 2000  from Dr. Martin P.J. Zinser <ZINSER@sysdev.exchange.de>
                        SQUID, ERRLOG, LOGGER found some
                        picky, picky DECC warnings, all about my abuse of
                        signed vs. unsigned ints.
    0.4     4 Feb 2000  rename to CRINOID; $SQUID::Reuse still works, added
                        $CRINOID::Reuse also.
                        EXC_HANDLING.H/DECC bug check functionality
                        during config;  Various config and com file fixes
                        Define licensing to be applied to CRINOID
            8 Feb 2000  Mods to OYSTER: oyster.config can set
                        "suffixes" to allow only some file suffixes,
                        set HomeDir directory to chdir to when running
                        scripts, set NoDescend to NOT search in subdirectories
                        for a script URL with PATH_INFO
                        Generate fake PID in $$ for scripts

    0.4-1  10 Feb 2000  CRINOID.COM, SHUTDOWN.COM modified to make it
                        easier to run multiple servers.
                        Added KILL_TENTACLES.COM to shut down TENTACLE
                        processes easily.

                        SCRIPT.PM improved script error-handling,
                        implement -I -D  and -w flags on CGI scipt's
                        leading `#!'  line
                        (options not fully parsed, needs work).
                        Extensive change in script compilation
                        compilation (eliminated the 'do $file' stuff),
                        and module sharing.  It's faster now.

                        Script timeout capabilities added in OYSTER
                        and SCRIPT.PM, controlled by MaxRunTime in
                        OYSTER.CONFIG

                        mod to CRINOID.C to remove trailing slash from
                        CRINOID.CONFIG bindir's. It was making probems
                        for scripts, giving double slashes.

                        Added OYSTER script-lookup caching, so we don't
                        have to walk the CGI directory tree for a script
                        we already found.


    0.4-2  14 Feb 2000  Modifications to CRINOID.C, SCRIPT.C, SCRIPT.H
                        and CRINOID_TYPES.H to support multihoming.
                        After a ".SCRIPT" directive in CRINOID.CONFIG,
                        one can add a "..LOCALHOST hostname" to restrict
                        the script URLs to only those from a particular
                        localhost name (as specified in the SERVER_NAME
                        parameter generated by the OSU server).  Since
                        the search for a matching URL in CRINOID.CONFIG
                        proceeds from the "top down", one should have
                        "more specific" URLs first.

                        If no ..LOCALHOST directive is given, any localhost
                        name will match.

                        Added ErrorHTML parameter to OYSTER.CONFIG so that
                        one can use a user-generated "CGI error" page;
                        substitutes on the fly for the error message and
                        script name.  See [.MISC]OYSTER.CONFIG-EXAMPLE

                        Various OYSTER script/path/cache lookup fixes.

    0.4-3  15 Feb 2000  mods to CONFIGURE.COM, INSTALL.COM, and STARTUP.COM
                        to make multi-server operation easier.  In CRINOID.C
                        get DECNET_SERVICE_NAME from logical if not
                        defined in CRINOID.CONFIG.  The "WWWPERL.COM"
                        (or whatever, name defined by
                        DECNET_SERVICE_NAME) sets the logical.

                        Added ..BINDIR command for CRINOID.CONFIG as
                        an option for the .SCRIPT command, so that
                        script URLs in the same process group don't
                        have to share a bindir.

                        Note that the "USERGROUP" *does* have to have
                        a common BINDIR, in the sense that the same
                        subdirectory of a user's SYS$LOGIN is common
                        for all.  Can add process groups for specific
                        users before the USERGROUP command.

                        Mods to CRINOID to separate out the part of the URL
                        that doesn't map to directory structure and
                        make it available to OYSTER (SCRIPT_PREFIX).
                        Mods to OYSTER to strip the prefix when doing
                        tree-search for a script.

    0.4-4  16 Feb 2000  fixed an annoying bug with bindir finding for
                        usergroups in CRINOID.C.  And another small
                        (non-harmful) bug in OYSTER's url parsing is
                        fixed.

                        First attempt at "non-PERSONA" capabilities for
                        STUB.  Checks for PERSONA system services in
                        CONFIGURE.COM. If there's no PERSONA, we have to
                        change the USERNAME in the PCB and JIB system
                        data structures with a little CMKRNL routine.

                        We get some MACRO->C definitions converted for
                        $JIBDEF and $PCBDEF and set the "username" and
                        uic for the detached process started by STUB.

                        Modified STARTUP to look for CMKRNL priv needed
                        for non-Persona STUB.

                        Tried the non-Persona version of STUB and it seems
                        to work okay.

           17 Feb 2000  Minor doc changes

           15 May 2000  Martin Zinzer <zinser@sysdev.exchange.de> fixed
                        typo in STARTUP.COM

    0.4-5   6 Jul 2000  move creation of WWWPERL.COM to build phase

                        change to SCRIPT.PM to fix package/sub name finding
                        problem reported by Mike Marmor <mike.marmor@reuters.com>

                        Prompted by Mike Marmor's suggestions, OYSTER.CONFIG
                        can now have "addinc DIR1 [DIR2 [DIR3...]]" lines,
                        that will append to the @INC used to look up modules.

    0.4-6   20 Jul 2000 Added AddENV and ExportENV to OYSTER.CONFIG to
                        import and export from logicals to script %ENV
                        This can also be used to export TZ to the process
                        logicals table.

                        Change to CONFIGURE.COM to look for .h files in
                        Perl_root:[000000...], and to print more informative
                        message when PERL_SETUP.COM is not found.  Change to
                        INSTALL.COM to set up Perl before installation.

                        Note: should use CGI.PM v2.68+, fixes some CGI::Carp
                        and file uploading problems, but CGI::Carp still calls
                        exit, resulting in an error.  Submitted a minor
                        patch to CGI.PM 2.68 to fix file upload: it seems
                        that file tests on _ don't work right in CGI
                        environment...

                        Added support for __DATA__ and __END__ in scripts;
                        note that this is NOT parsed internal to Perl, so
                        you have to treat it gently:  put __DATA__ on a line
                        by itself if you want it picked up.
                        (E.g. /^\s*__DATA__\s*$/)  Which is good style anyway.

                        The "read position" in the file is maintained between
                        script invocations (if you're using reusable scripts)
                        so you can read the data the first time and cache it
                        in a persistant variable...

                        Added MaxLogfileSize and MaxLogfileVersions to
                        OYSTER.CONFIG to control OYSTER log generation.
                        Added dump of module namespace to logfile to aid
                        in preload debugging.

                        Minor change to OYSTER and SCRIPT.PM to prevent
                        variables persisting when a script is modified and
                        reloaded.

                        Tracked down at least one timing-related ACCVIO:
                        process idling, disconnecting, then DELPROC was
                        triggering it.  A trivial fix, but hard to find!

                        Added an overload to "exit" for scripts and cleaned
                        up messages.   Note that to make CGI::Carp work
                        properly an overload to CGI::Carp::exit was added,
                        and a tricky "init_stash()" call so that the stash
                        for (root)::CGI:: and (root)::CGI::Carp:: are
                        initialized into the Safe partition.

                        Don't try to preload CGI (or any CGI::* modules),
                        there's too much stuff that shouldn't be shared
                        between different scripts.

    0.5     25 Jul 2000 update docs, bump version #
                        Fixed use of filetests on _ in scripts...
                        change in Opcode.xs was needed, so will update
                        patches.

                        Check Perl_root:[lib...] permissions and
                        check for Opcode.xs patch during CONFIGURE.
                        Generated new Opcode patches based on Perl
                        version #'s.

                        created [.debug_support]test_global.pl to get
                        a list of global symbols, added save/restore in
                        SCRIPT.PM, also sets $^T and $0 for scripts.

    0.5-1   28 Jul 2000 minor changes for VAX compatibility
    0.5-2   10 Aug 2000 changes to localization in OYSTER and SCRIPT.PM
                        (particularly $^O)
    0.5-3   14 Sep 2000 Add 'Exporter' to list of always-preloaded modules
                        (needed for module symbol import)

                        Suppress "exit(0)" messages.

                        $In_CRINOID = 1 added in script symbol table, so
                        scripts can tell they're being run by CRINOID.

                        Thomas Pfau (pfau@maherterminals.com) caught
                        misordered parameters in call to $CREMBX in
                        LOGGER.C.  Fixed, and buffer quota defaulted to
                        ~ 10 logger messages.

                        Added exit_handler in LOGGER to delete
                        logicals; was causing CRINOID restart to not
                        start logger when needed.  Also modified
                        SHUTDOWN.COM to shut down logger gently, and
                        deassign leftover logicals as needed.

                        Mods to DESCRIP.MMS to install
                        [.debug_support] stuff when building a debug
                        version of CRINOID

                        Generate nfbdef.h dynamically during build.

                        Handling of tentacle idling changed to remove
                        deadlock when too many concurrent idle/connect
                        requests are being processed.

                        Thomas Pfau (pfau@maherterminals.com) patched
                        CRINOID.COM to send info in /subject and avoid
                        use of tempfile...also patched LOGGER.C to allow
                        viewing log while still open.

                        Added TieENV module so that scripts can selectively
                        (controlled by oyster.config) the "true" $ENV.
                        See [.misc]oyster.config-example for keyword info.

    0.51   19 Sep 2000  rewritten ERRLOG_CLIENT to avoid use of MbxQ,
                        hold/restart logging if LOGGER goes away and comes
                        back later.

                        added "max active connections" so we don't run
                        out of BYTCNT quota.  Mailbox quotas defined.

                        Proc_mgr process startup "throttled" so we don't
                        start N proc's all at once.   Tentacle startup
                        reordered to flag readiness when closer to ready.

                        connection requests go directly from AST to thread,
                        bypassing main event loop in CRINOID

                        CRINOID.COM dies if duplicate process, avoiding
                        startup overhead from dying later.

    0.51-1  20 Sep 2000 added memory-leak diagnostics, fixed two small
                        leaks. To turn on diagnostics, edit CONFIGURE.MMS
                        and uncomment the XCD= ... line before compiling.
                        Setting the log level to L_MEM+L_INFO gives a log
                        entry for each malloc/free/realloc.  Routines in
                        ERRLOG_CLIENT.C excluded from diagnostics for
                        obvious reasons.

                        [.debug_support]mem.pl added to process CRINOID.LOG
                        for matching malloc's with free's etc.

    0.51-2  21 Sep 2000
                        got STARTUP executable working, so now
                        @STARTUP RUN will cause the CRINOID server to
                        start.

                        revision of LOGGER for speed: reads MBX's with
                        ASTs, queuing the messages for main program
                        loop.  Flushing reduced, and added
                        "write-behind" with multibuffers.

                        now using locks to coordinate LOGGER startup; if
                        logger exits, CRINOID will restart. LOGGER will try
                        and take over MBX's left over from a previous
                        logger to prevent hanging tentacles.  Only one
                        LOGGER can run, and (if multiple CRINOIDs) only
                        one CRINOID will attempt restart.

                        Revised .com files to deal with rooted logicals,
                        was causing problems with installed images.

    0.51-3  22 Sep 2000 Tom Pfau <Pfau@MaherTerminals.com> provided
                        changes to error-handling, including a message
                        definition file and new macros VMS_SIGNAL,
                        RMS_SIGNAL and revised VMS_ABORT and UNIX_ABORT.

                        Changes to DESCRIP.MMS, MANIFEST, [.misc]*.com
                        to improve error reporting.

    0.51-4  26 Sep 2000 thanks to Tom Pfau <Pfau@MaherTerminals.com> for
                        changes to open of OYSTER.LOG so that multiple
                        processes can share the logfile; changed open of
                        OYSTER.CONFIG and of script files to allow shared
                        read access.  Change in SCRIPT.PM for __DATA__
                        handling: closes file on eof or if script not
                        reusable.

                        Minor bugfix in locking for multiple-CRINOID
                        logger startup.

    0.51-5  27 Sep 2000 Added [.EXAMPLE_SCRIPTS]

13. Needs Doing:
================
Bug Fixes:
    o Lynx request sometimes causes CRINOID crash?
    o status code after crash messed up by pthreads

Upgrades:
    o Documentation!
    o A better syntax for CRINOID.CONFIG? -or-
      a Perl script to help generate CRINOID.CONFIG?
    o get TKperl working, so can use for debugging scripts
    o an intelligent model of #procs/#idle vs. hit rates?
    o Add process quotas etc, that haven't been implemented
    o MAJOR PROJECT: implement CRINOID as MST extension to OSU HTTPd
    o Taint check CGI scripts with -T on #! line?
    o IMPERSONATE per-thread and eliminate STUB for VMS 7.1? 7.2?
--Chuck Lane  lane@duphy4.physics.drexel.edu
