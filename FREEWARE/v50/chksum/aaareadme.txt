Computes the BSD checksum of a file or files.

Requirements
-------------
 The  chksum  help  is  generated  using  TEXT2C, but in principle (and
 unless you modify it) it is already generated.  TEXT2C is available at
        http://nucwww.chem.sunysb.edu/htbin/software_list.cgi

 -- VMS  --
 CHKSUM requires either MMK or MMS for compilation.
        MMS is © Digital Equipment Corporation, All right reserved.
        MMK is © MadGoat Software.  All Rights Reserved.
          
 -- Unix --                                                         
 Nothing.   But  the  provide makefile does not take care of formatting
 help and/or the quick help produced by the -h option.

Platforms
---------
	
 It  has  been  tested  on  VAX OpenVMS 6.1 and Alpha OpenVMS 6.2 using
 respectively VAXC V3.2-044  and  DECC  V5.3-006.   GCC  2.8.0  on  AXP
 OpenVMS7.2  and GCC 2.1.0 on VAX (OpenVMS 6.2), Digital Unix V4.0D and
 Linux RedHat 6.1 have also been tested.

 VMS  Provided  executables  and  objects  produced by DECC and VAXC on
 OpenVMS6.2. chksum executable is a Digital Unix 4.0D executable.


Compilation
-----------
 -- VMS  --
  $ @MAKE
 or
  $ @MAKE {DECC|VAXC|GCC}

 -- Unix --
 try % make

	

VMS Installation
----------------
 You can either
 - Define a global symbol CHKSUM :== $dev:[dir]CHKSUM
 - Place the executable in DCL$PATH (VMS 6.2 and up)
 - Insert the following in DCLTABLES
        define verb CHKSUM
                image dev:[dir]CHKSUM.EXE
                cliflags(foreign)

 (no need to specify that you should copy the proper .exe* suitable for
 your platform i.e.  AXP or VAX)

 An  help  file  is  also  provided.   Insert it in your preferred help
 library.

Unix Installation
-----------------
 Try
  % make install 
 but read it first (simple and stupid but does the job) 

KnownProblem
------------
 VMS only :

 GNU  C  compiler  under  VMS  returns $STATUS=%X00000002 on a returned
 value of 0. If you know how to fix that, I'll be glad to hear  it  but
 this  does  not  appear  to  be a serious problem and GCC VMS is a low
 priority maintainance right now.


Author
------
 Original author unknown.
 Current program written by Jérôme LAURET ; jlauret@mail.chem.sunysb.edu

 Please,  send comments/suggestions/flame/blame/curses to me.  They are
 all welcomed and will help other  people  in  future  by  making  this
 software better.


History
-------
 23-Mar-2000	Added Wildcard under VMS.
 13-Apr-2000	Added MD5 checksum. Supressed GNU C executables and support.
 23-Oct-2000    Return value on VAX was wrong. Corrected.
 14-Jan-2001    GCC compilation fix for Digital Unix.

