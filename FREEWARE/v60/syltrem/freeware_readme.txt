Syltrem, UTILITIES, Determine Global Pages and Sections Requirements

I have written a DCL procedure to know how much free global
pages/sections are required for installing a program in 
shared memory.

It supports both Alpha and VAX.

I believe it can be useful to others as it is to me from time to time.

Maybe it could be included in the next release of the freeware CD.

Regards,

Syltrem
http://pages.infinit.net/syltrem <http://pages.infinit.net/syltrem>
(OpenVMS related web site)
 
 
Sample execution on VAX
$ @[.mis6]gbl prod_exe:pfcp0100inf
%ANALYZE-I-ERRORS, IVA$EXE:[IMAGES]PFCP0100INF.EXE;72            0
errors
Section  1:    41 pages
Section  2:   305 pages
IVA$EXE:[IMAGES]PFCP0100INF.EXE; requires 346 global pages in 2 =
sections
to be installed/open/share/header

Verified by:
$ install list/glo prod_exe:pfcp0100inf
 
DISK$DISQUE35:<IMAGES>.EXE
   PFCP0100INF;72   Open Hdr Shar
 
        System Global Sections
 
DSA4:<IMAGES>PFCP0100INF.EXE
 INS$873B2E10_002(6AA6F911)              PRM SYS
Pagcnt/Refcnt=3D305/0
 INS$873B2E10_001(6AA6F911)              PRM SYS
Pagcnt/Refcnt=3D41/0
 
 
Sample execution on Alpha
$ @gbl prod_exe:pfcp0100inf

%ANALYZE-I-ERRORS, IVA$EXE_ALPHA:[IMAGES_ALPHA]PFCP0100INF.EXE;73
0 errors
Section  1:   706 pages
IVA$EXE_ALPHA:[IMAGES_ALPHA]PFCP0100INF.EXE; requires 706 global pages
in 1 section to be installed/open/share/header

Result verified with:
$ install list/glo prod_exe:PFCP0100INF.EXE
 
DISK$DISQUE35:<IMAGES_ALPHA>.EXE
   PFCP0100INF;73   Open Hdr Shared
 
        System Global Sections
 
DSA4:<IMAGES_ALPHA>PFCP0100INF.EXE
 INS$ACB6C9B0_003(6AA84EFE)              PRM SYS
Pgltcnt/Refcnt=3D706/
405
 
