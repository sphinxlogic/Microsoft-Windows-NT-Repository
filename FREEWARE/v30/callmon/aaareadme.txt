CALLMON, UTILITIES, Procedure Call Monitor for OpenVMS Alpha

CallMon is a low-level debug utility for developers.

With CallMon, an application can monitor all calls to a selected set
of procedures, taken from any shareable image (including the various
OpenVMS run-time libraries).

The calls are intercepted whether they are come from the application
or  from  the  OpenVMS  run-time libraries. For instance, if CallMon
intercepts all calls to LIB$GET_VM, the application will be notified
each  time  LIB$GET_VM  is called either from the some module of the
application, from an OpenVMS library routine in some shareable image
or even from another routine in LIBRTL.EXE.

CallMon  is  especially useful to implement higher level tools which
need  to  monitor  the  activity of some library routines. A typical
usage  of  CallMon  may  be  the  monitoring  of  the virtual memory
allocation  (intercepting  calls  to  routines  like  LIB$GET_VM and
LIB$FREE_VM).

To intercept a routine, the application must provide:

    - the name of the routine to intercept (a character string)
    - the address of a user-written pre-processing routine
    - the address of a user-written post-processing routine

Later,  whenever  the  intercepted  routine is called, the following
steps are taken:

    - the user-written pre-processing routine is called,
    - the original intercepted routine is called,
    - the user-written post-processing routine is called.

The  user-written  procedures  have  access to the original argument
list and returned value. The interception can be activated, modified
and  deactivated  at any time. All hooks into the code are performed
"on the fly" in the virtual address space of the process.

Interface definition files are provided for the C and Ada languages.
However, CallMon can be used from any other language.

Note that CallMon is designed for OpenVMS Alpha only. CallMon uses a
lot  of features which are specific to the Alpha architecture and to
the  structure  of OpenVMS on Alpha platforms. There is currently no
version of CallMon for VAX platforms.

This  OpenVMS  Freeware  CD contains the version 1.0 of CallMon. You
will find the following items in the [CALLMON] directory:

    - CALLMON.PS : CallMon documentation in PostScript form
    - CALLMON.OLB : CallMon object library
    - CALLMON.H and CALLMONMSG.H : Interface files for C
    - CALLMON_.ADA and CALLMONMSG_.ADA : Interface files for Ada
    - [.SRC]*.* : CallMon source files (written in C and Macro-64)
    - CALLMON$RESIDENT.COM : Utility procedure (see documentation)

Several  example  programs  written in C and Ada are provided. Their
source  files are located in the subdirectory [.SRC]. The correspon-
ding executable images are provided in the main directory.


                        RELEASE FORM
                        ------------

OpenVMS Freeware CD
--------------------

DEVELOPER RELEASE FORM:


        As the owner/developer of this software, I release it to be
        included on the OpenVMS Freeware CD-ROM.  I expect no renumerances
        or payments for the use of this software from Digital or Digital's
        customers.

                                        _______________________
                                        Thierry Lelegard
