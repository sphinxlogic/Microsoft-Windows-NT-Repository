Directory deletion program.

Requirements
-------------
 The  DELTREE  help  is  generated  using TEXT2C, but in principle (and
 unless you modify it) it is already generated.  TEXT2C is available at
	http://nucwww.chem.sunysb.edu/htbin/software_list.cgi

 This software requires either MMK or MMS for compilation.
        MMS is © Digital Equipment Corporation, All right reserved.
        MMK is © MadGoat Software.  All Rights Reserved.


Platform
--------

 It has been tested on VAX OpenVMS 6.1, Alpha OpenVMS 6.2 and 7.2 using
 respectively VAXC V3.2-044, DECC V6.0-001 and DECC V6.2-003.

 I  have  also  compiled and tested this package using GCC 2.8.0 on AXP
 but the  messaging  utility  was  behaving  chaotically  (i.e.   shows
 messages  but  not  related  to  what  is  declared in .MSG).  For the
 moment, we include a GCC hack and print  messages to std-error instead
 of  using  VMS  message facility __BUT__ If someone figures out how to
 get it to work properly with GCC, I'll be delighted to know about it.

 *** GCC VAX UN-SUPPORTED due to rather poor support and implementation.
 You may try it and succeed (and I'd be glad to know about it) but for
 the moment, I've added a #error in the code so the compilation will
 abort and warn you about it.


Compilation & Test
------------------
o Use MAKE.COM, a short DCL script interface to MM{S|K}.

  Compilation : 
	$ @MAKE {DECC|VAXC|GCC}

  Test	      :
	$ @MAKE "" TEST




Installation
------------

(1) Installing the EXEcutable

 First,  copy  the  appropriate  DELTREE.EXE*  into  your software path
 directory (it can be SYS$COMMON:[SYSEXE]).   Let's  suppose  that  you
 copied it in dev:[dir].  Then, you can either

 - Define a foreign symbol DELTREE :== $dev:[dir]DELTREE.EXE
 OR
 - Be sure that the executable is somewhere in DCL$PATH (VMS 6.2 and up)
 OR
 - Insert the following in DCLTABLES
	define verb DELTREE
		image dev:[dir]DELTREE.EXE
		cliflags(foreign)

   WARNING : This will render any symbol such as rm :== del/confirm
             ambiguous i.e. any symbols using "del" instead of DELETE
	     will be ambiguous. You may have to reshape your already
	     existing command files ...

 OR
 - Finally, I also used a modification of the DELETE verb as described in 
   the [.MISC] directory.  This is a a less-traditional way of installing
   DELTREE but will allow you to use /TREE as a  new  qualifier  for  the
   DELETE command :  DELETE/TREE will activate DELTREE.


(2) Installing the Help

 Remember to also install the help file where it should be, for example
	$ LIBRARY/REPLACE SYS$COMMON:[SYSHLP]HELPLIB.HLB DELTREE.HLP
	
 Personally,  I  have  a  different help library (named VMSAPPS.HLB) in
 which I insert all freeware help.  In any cases, if you don't want to,
 help is available on-line at

	http://nucwww.chem.sunysb.edu/helplib/@hvmsapps/DELTREE




Author
------
 Jérôme LAURET ; jlauret@mail.chem.sunysb.edu


History :
---------
 ??-???-1998	Added the .*;* files. Remains the deep tree problem		
 20-JAN-1999 	Made it GCC compatible. Now tested with DECC & GCC (Alpha), 
		VAXC.
 29-JAN-1999	Added Cli interface and options.
 09-APR-1999	Added deep tree mechanism.                 
 28-JUN-1999 	GCC small cleanup.				
 19-JUL-1999    Oups ! I found a bug (a strcpy() to an unallocated pointer.
 05-AUG-1999 	Some message changed from -W- to -I- from -I- to -S-      
 13-AUG-1999    Added extended path syntax + multiple argument list.
 20-AUG-1999	Added /VERSION and /DETACH			
 27-AUG-1999    Added /PRIV                                
 11-SEP-1999    Changed .OUT in .LOG 		
 28-OCT-1999	Added some fixes to get DELTREE to smoothly compile on 7.2 . 
		Thx to Martin.
 06-JAN-2000    /NOTIFY added. Fixed the problem with /PRIO and 
		priority > base-prio.	 
 11-Feb-2000	Addded 255 levels support for OpenVMS7.2 and up.
 16-Feb-2000    Problem with OpenVMS7.2 fixed (returned file name are 
                lowercased now).
 19-Feb-2000	Fixed DECC problem on VAX. MSG interface detach routine.
		Some bug fixes and useless variable cleaned.
 05-Mar-2000	Clean up messages + added a more robust chdir("[.XXX]") in case
		logicals XXX is defined (lead to catastrophe) + lib$ return	
		value check.
 02-Apr-2000    Gained 10% speed. Misc bug fixes one of those being the rather
		poor choice of lowercase logical impossible to undefine ...


Acknowledgement
---------------
 Thanks to 

 - zinser@decus.de   for   reporting   some  compilation  problems  under
   OpenVMS7.2 and providing a template for MAKE.COM .
 - R.A. Byer for providing an OpenVMS 7.2 (VAX and Alpha) account which 
   made possible the discovery of a  major  missfeature  related  to  RTL  
   behavior  change somewhere between 6.2 and 7.2 as well as a problem with
   VAX compilation using DECC.

