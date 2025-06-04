
 *** EXTREMELY IMPORTANT ***
 In order to help Yves Noyelle in developping DCC and make this utility
 stronger and better, we urge you to  report  bugs,  hints/suggestions,
 problems,  questions, comments, flame/blame or any reaction whatsoever
 you may have while using dcc.  This is YOUR contribution to  dcc.   So
 please, let us know what you think about it by leaving a message at

	http://nucwww.chem.sunysb.edu/dccboard/wwwboard.html

 Help can be found at
	http://nucwww.chem.sunysb.edu/info/dcc.htmlx

 Most recent version downloaded from
	http://nucwww.chem.sunysb.edu/htbin/software_list.cgi?sort=date
 

	This being said ...



 To install DCC, I used the following steps :


0) Compile 

  $ MM{K|S} 

 You must have either MMS or MMK as a make utility in order to assemble
 this software.  MMK is a freeware Copyright © MadGoat Software and  is
 available  at  http://www2.wku.edu/www/fileserv/.   MMS  is  © Digital
 Equipment Corporation.


1) Pre-Installation

  $ define DCCSYSHDRDIR SYS$COMMON:[VMSAPPS.DCC.H]

 DCC needs the header files to  function.  

 You   will   need   to   extract   them   from   the   text  libraries
 DECC$RTLDEF.TLB,SYS$STARLET_C.TLB,SYS$LIB_C.TLB.   To accomplish that,
 I have added EXTRACTHDRFILESVMS.COM file to extract the C header files
 from  the  text  library in case this is not done on your system.  For
 DECC6.0 on Alpha, you have to be sure  that  you  have  provision  for
 21788 blocks.

 You  may also try to define it to point on your GNU_CC_INCLUDE path in
 case you use GCC includes. This was NOT tested.

 You  may  also  use SYS$COMMON:[DECC$LIB.REFERENCE.DECC$RTLDEF] as the
 header file path.  This assumes that you have chosen  to  install  the
 header  files  while  installing DEC-C compiler.  The files present in
 this directory will NOT be sufficient (only the RTLDEF are there)  for
 DCC  to  check code containing VMS specific includes.  Therefore, I DO
 prefer the first solution and extracted all header files.


2) Installation

  $ define DCCEXECFILESDIR SYS$COMMON:[VMSAPPS.DCC]

 Or  any  directory suitable to your site.  This directory will contain
 files used by DCC at run time.

 The   DCC   executable   itself   may   be   moved   elsewhere.    The
 INSTALLDCCVMS.COM  ask you for its location.


3) Note : 
 For  a  global/cluster-like installation, you MUST define the logicals
 DCCEXECFILESDIR and DCCSYSHDRDIR at boot time.  You  can  place  those
 command  in  your  system startup file or any file executed at startup
 and suitable for your site.  No need to say that they do  have  to  be
 defined /SYSTEM .


4) Documentation is provided as 
	DCCARTICLE.PS		PostScript Documentation.
	DCCARTICLE.ASCII	Ascii version
	DCC.HLP			VMS help file (partial).


5) Quick Installation ?

 Here is what INSTALLDCCVMS.COM did :

$ copy *.adj        dccexecfilesdir:/prot=(w:r)
$ copy starter.dcc  dccexecfilesdir:/prot=(w:r)
$ copy dynarray.h   dccexecfilesdir:/prot=(w:r)
$ purge/keep=1      dccexecfilesdir:*.dcc
$ purge/keep=1      dccexecfilesdir:*.adj
$ purge/keep=1      dccexecfilesdir:dynarray.h
$ copy/prot=(w=e) dc.exe VMSAPPS:dcc.exe ! VMSAPPS is for me where the
$					 !freeware executables are located
$ purge/keep=1 VMSAPPS:dcc.exe 
$ 

	The executable provided in this distribution has been build using
DEC C V6.0-001 on OpenVMS Alpha V6.2-1H3.


 
 Finally     
 ---------
 
 I  have  tested this software on an Alpha OpenVMS 6.2 using CC version
 6.0 and it's running great.

 I have added to this distribution dcc.pod (and DCCHELP.MMS) which were
 used to format the help in this distribution.  Note that  DCC.POD  was
 build  from  DCCREADME.TXT now removed from the VMS distribution.  You
 will NOT be able to build the  DCC  help  if  you  do  not  have  perl
 installed.



---------------------------------------------------------------------------
J. Lauret <jlauret@mail.chem.sunysb.edu>
---------------------------------------------------------------------------

