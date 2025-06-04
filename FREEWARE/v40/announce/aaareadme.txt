SYS$ANNOUNCE Surgery

(This article was published in the Canadian DECUS magazine, DECUScope)


Abstract

Are you tired of looking at the same old boring message that OpenVMS displays
prior to the "Username:" prompt when you log in?  You know the one, "Welcome to
OpenVMS VAX V6.2" (or something similar).  

DIGITAL gives you the capability to change this message via the SYS$ANNOUNCE
logical name.  At first glance, it would appear that this logical name only
allows for a static message.

This article describes how to write a program that will automatically update
SYS$ANNOUNCE every time a user attempts to log in.  A sample program is
provided from which your own program can be constructed.


About the Author

Curtis Rempel, an OpenVMS systems programmer, has managed OpenVMS systems for
over 12 years and specializes in OpenVMS internals and system performance
issues.  


                                     ---

Are you tired of looking at the same old boring message that OpenVMS displays
prior to the "Username:" prompt when you log in?  You know the one, "Welcome to
OpenVMS VAX V6.2" (or something similar).  

DIGITAL gives you the capability to change this message via the SYS$ANNOUNCE
logical name.  At first glance, it would appear that this logical name only
allows for a static message.

However, as we shall see shortly, it is rather simple to write a program which
can be used to provide dynamic information to users prior to logging in.


From Static to Dynamic - Method #1:

One way of displaying dynamic information prior to login is to define
SYS$ANNOUNCE to point to a text file using the "@" symbol (input redirection)
as follows:

   $ DEFINE /SYSTEM /EXECUTIVE_MODE SYS$ANNOUNCE "@SYS$MANAGER:ANNOUNCE.TXT"

When a login is attempted, LOGINOUT.EXE will read the contents of the file
SYS$MANAGER:ANNOUNCE.TXT and display it prior to the "Username:" prompt.  Thus,
by updating this file periodically, you can provide important information to
users attempting to log in without being restricted to a single line of text.  

For example, if the accounting system is currently down for backup and the
system manager has disabled interactive logins, a user attempting to log in
would experience the following:

   Username: JSMITH
   Password: 
   %LOGIN-F-LOGDISABL,  logins are currently disabled - try again later

Although this message is somewhat informative, it is still a generic message
that could be made more useful.

If SYS$ANNOUNCE is defined to point to a text file via the "@" symbol as above,
the user would see the following and thus know not to bother trying to log in
until after 10:00 p.m. :

   XYZ Corporation - Accounting Division
   
   *** ACCOUNTING SYSTEM IS CURRENTLY DOWN UNTIL 2200 FOR BACKUP ***

   Username:

NOTE: If you forget the "@" symbol in the definition of SYS$ANNOUNCE, users
attempting to log in will receive the following message:

   SYS$MANAGER:ANNOUNCE.TXT

   Username:


A more flexible approach - Method #2:

The dynamic SYS$ANNOUNCE program is implemented by extending the concept of
input redirection (the "@" symbol) via an inter-process communication device
instead of a text file. Instead of defining SYS$ANNOUNCE to point to a text
file, we define the logical name to point to a mailbox.  Thus, when a user
attempts to log in, instead of reading from a text file, input will be read
from the mailbox.

So how does this mailbox method give us information that is more dynamic that
the text file method?  Each method requires that information be stored
somewhere so it can be retrieved when a user logs in.  With the text file
method, you simply use your favorite editor to update a text file.  But how do
you update information in a mailbox?  By queueing a "read attention"
asynchronous system trap (AST) on the mailbox, a piece of code can be defined
to execute whenever a process attempts to read data from the mailbox.  This
piece of code formats a message (with whatever dynamic information you want)
and writes it to the mailbox.  This data is then immediately transferred  and
displayed on the users terminal prior to the "Username:" prompt.

The sample program illustrates the set of steps necessary to implement the
mailbox method.  The main routine sets up the environment by creating a
permanent mailbox, queueing a "read attention" AST on the mailbox, and then
hibernating.  At this point, SYS$ANNOUNCE is redefined by the DCL command
procedure (see below) to point to the mailbox just created.  When a read
operation occurs on the mailbox (i.e. somebody is attempting to log in), the
AST fires and executes the  ast_handler() routine.  This routine reads a set of
system load average values from the LAV0: device and formats a string
consisting of the system time, the 1, 5, and 15 minute CPU queue load averages,
and the number of interactive and batch jobs.  The string is then written to
the mailbox to satisfy the read operation.  Finally, another "read attention"
AST is queued to handle the next login attempt.

NOTE: If the program ever terminates abnormally for any reason, users will not
be able to log in until either you restart the program or redefine SYS$ANNOUNCE
to point to a text file or a plain old string instead of the mailbox device. 
The reason for this is that because the program has terminated, the read
request to the mailbox cannot be satisfied and the users session will appear to
have hung without ever having received the "Username:" prompt.  Thus, some
error recovery code is necessary  as shown in the file SYS$MANAGER:ANNOUNCE.COM
below.

If you have the MultiNet TCP/IP networking product for OpenVMS VAX or Alpha 
installed, the load average device driver will already be operational on your
system.  If you don't use MultiNet, you can obtain a VAX version of the driver
as a ZIP archive called LAVDRIVER.ZIP via anonymous FTP from ftp.spc.edu in the
[.MACRO32.SAVESETS] directory.  To my knowledge, an OpenVMS Alpha version is
not available via FTP.  The ZIP archive contains information about the load
average data provided and how to obtain the data from a user program. 


Caveats:

When making modifications to your system, you should always keep your
company security policy in mind.  If you don't have a security policy, there are plenty
of good resources available on the Internet and right in your own OpenVMS
documentation set: OpenVMS Guide to System Security.

Be careful about what type of information you provide prior to login via
SYS$ANNOUNCE.  Consider whether this information could be used by a hacker,
either externally or internally, to gain access to your system.  For example,
displaying the operating system version may not be acceptable as it may give
clues as to what, if any, vulnerabilities your system may be susceptible to.


Installation:

Installing the dynamic SYS$ANNOUNCE program is accomplished using the
following four simple steps:

1.  Compile and link the image.  

    $ CC /STANDARD=VAXC ANNOUNCE


    Linking on VAX systems:

    $ LINK ANNOUNCE, -
           SYS$SYSTEM:SYS.STB /SELECTIVE_SEARCH, -
           SYS$INPUT /OPTIONS
    SYS$SHARE:VAXCRTL /SHARE


    Linking on Alpha systems:

    $ LINK /SYSEXE ANNOUNCE

2.  Create a file called SYS$MANAGER:START_ANNOUNCE.COM as follows:

    $! File: SYS$MANAGER:START_ANNOUNCE.COM
    $ RUN /DETACHED -
          /PROCESS_NAME=SYS$ANNOUNCE -
          /PRIVILEGES=(PRMMBX) -
          /PRIORITY=15 -
          /NOSWAPPING -
          /UIC=[SYSTEM] -
          /INPUT=SYS$MANAGER:ANNOUNCE.COM -
          /OUTPUT=SYS$MANAGER:ANNOUNCE.LOG -
          SYS$SYSTEM:LOGINOUT
    $ WAIT 00:00:10
    $ DEFINE /SYSTEM SYS$ANNOUNCE "@''F$TRNLNM(""SYS$ANNOUNCE_MBX"")'"
    $ EXIT

3.  Create a file called SYS$MANAGER:ANNOUNCE.COM as follows:

    $! File: SYS$MANAGER:ANNOUNCE.COM
    $ SET NOON
    $LOOP:
    $ RUN SYS$MANAGER:ANNOUNCE
    $ REQUEST "SYS$ANNOUNCE terminated, $STATUS=''$status', restarting..."
    $ GOTO LOOP

4.  Modify the OpenVMS system startup procedure to include the following:

    $ @SYS$MANAGER:START_ANNOUNCE

    (SYS$STARTUP:SYSTARTUP_V5.COM or SYS$STARTUP:SYSTARTUP_VMS.COM)


Sample Session

The following session illustrates the use of the sample dynamic SYS$ANNOUNCE
program:

31-JUL-1997 13:05   Load:   1.05 0.95 0.32    Jobs: 318+5

Username:


Enhancements:

Further enhancements can be made to this sample program such as changing
the information provided depending on time of day, terminal identification,
etc.  The program could also translate site specific logical names and
display them as part of the message.


Conclusion

A dynamic SYS$ANNOUNCE program can provide useful information to users prior to
logging on to the system.  Careful consideration with respect to security and
continuity of operation will ensure trouble free operation.

                                     ---

Questions?

Email: vmsguy@home.com
       curtis.rempel@shaw.ca
