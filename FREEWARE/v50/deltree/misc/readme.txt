
Before Starting, an agressive note
----------------------------------
 !!! Your are NOT helping the VMS community if you do not report bugs !!!

 If the procedure described herein does not work, please report by sending 
 an Email to <jlauret@mail.chem.sunysb.edu> with the following information :
 - Your Operating system version (including the flavor AXP or VAX)
 - A copy of you DELETE.CLD file


Description
-----------
 This directory contains files to generate a replacement CLD file for the DELETE
 DCL command. 
 The steps are as follow

 1) Esure that you do have a SYS$COMMON:[SYSUPD]DELETE.CLD file
    If not, you can use the VREB utility (if installed) to extract the DELETE
    command to a CLD file.
 
 2) Execute GENCLD.COM
    What it does :
   
    o Patches DELETE.CLD as follow
	define verb DELETE
	   image DELETE
	   ...
	   qualifier TREE, syntax=DELETE_TREE, nonnegatable
	   ...

	...
	define syntax DELETE_TREE
  	   image VMSAPPS:DELTREE.EXE
  	   cliflags(foreign)

   o Creates a new DELETE.HLP for your help library.
   o Ask you if you want to "proceed". If you answer "Y", the steps
     described in (3) and above are executed.


  ******************************************************************
  THE FOLLOWING STEPS CAN BE EXECUTED BY SYSTEM MANAGERS OR PRIVILEGED
  USERS ONLY. BEFORE DOING ANY OF THOSE STEPS, BE SURE YOU HAVE READ
  AND UNDERSTOOD THE INSTRUCTIONS AND THE WARNINGS ABOVE.
  ******************************************************************

 3) Assuming you have a modified DELETE.CLD file produced by GENALL.COM, use
	$ SET COMMAND/REPLACE DELETE.CLD -
	/TABLE=SYS$COMMON:[SYSLIB]DCLTABLES.EXE -
	/OUTPUT=SYS$COMMON:[SYSLIB]DCLTABLES.EXE

    to replace the DELETE command in your command table.
 
 4) On all node within a cluster, installing it by doing
	$ INSTALL REPLACE SYS$COMMON:[SYSLIB]DCLTABLES.EXE /HEADER/OPEN/SHARE

 5) A replacement help file will be created. You may want to insert it after
    review
	$ LIBRARY/REPLACE SYS$COMMON:[SYSHLP]HELPLIB.HLB DELETE.HLP

    
 You can now use DELETE/TREE as part of the delete command.


WARNING
-------
 If  you  have  a  mixed  architecture  OR OpenVMS version cluster, you
 __MUST__ follow the steps (1) to  (3)  +  (optionally)  (5)  described
 above on __ALL__ bootnodes the step (4) on all node within a cluster.

 The  patched DELETE.CLD file assumes that DELTREE will be installed in
 a directory named  VMSAPPS:.   If  your  plan  is  to  install  it  in
 SYS$COMMON:[SYSEXE],  edit  the patched DELETE.CLD file and remove the
 path information.  Otherwise, adjust the path description to fit  your
 site  specific  installation or define, once for all, VMSAPPS to point
 to that area where you have all of your freeware installed.


