$ Type SYS$INPUT

This procedure will compile, link and install this VMS Password Policy 
module.   (Objects provided if you don't have the FORTRAN Compiler.)


It will create the module and copy it to SYS$LIBRARY.  It will install the
module.  You will need to add a line to SYS$MANAGER:SYSTARTUP_VMS.COM to
reinstall the module after a reboot.  The line needed is:

	$ INSTALL ADD SYS$LIBRARY:VMS$PASSWORD_POLICY.EXE

The module will also create a supplementary dictionary file in SYS$LIBRARY
called VMS$PASSWORD-POLICY-WORDS.DATA.  You can add words to this dictionary
by using the program ADD_DICTIONARY which is included in this package.

$ inquire junk "Press return to continue"
$ Type SYS$INPUT

After this has been completed, the program will change the SYSGEN parameter
LOAD_PWD_POLICY to a value of 1.  The Commands executed are:

	$ MCR SYSGEN
	SYSGEN> USE CURRENT
	SYSGEN> SET LOAD_PWD_POLICY 1
	SYSGEN> WRITE CURRENT
	SYSGEN> USE ACTIVE
	SYSGEN> SET LOAD_PWD_POLICY 1
	SYSGEN> WRITE ACTIVE


$ Type SYS$INPUT
  Do you wish to compile the program?  This requires the FORTRAN compiler.
  Objects are provided if you do not have the compiler.  
$Ask1:
$ inquire ANS "Compile?  (Y or N) "
$ IF ANS .EQS. "N" THEN GOTO COPY_OBJ
$ IF ANS .EQS. "Y" THEN GOTO compile
$ goto Ask1
$COMPILE:
$ Type SYS$INPUT

  You can ignore any compiler warning messages

$ FORTRAN/EXTEND VMS$PASSWORD_POLICY
$ FORTRAN/EXTEND ADD_DICTIONARY
$ goto Check_link
$Copy_OBJ:
$ IF F$GETSYI ("ARCH_NAME") .NES. "Alpha"
$   THEN
$    ! VAX OBJ
$    COPY *.OBJ_VAX *.OBJ
$   ELSE
$    ! ALPHA OBJ
$    COPY *.OBJ_ALPHA *.OBJ
$ ENDIF
$CHECK_LINK:
$ Type SYS$INPUT

  Do you wish to Link the program?  This is required to use this command 
  procedure. 

$ASK2:
$ inquire ANS "Link?  (Y or N) "
$ IF ANS .EQS. "N" THEN GOTO Activate
$ IF ANS .EQS. "Y" THEN GOTO Link
$ goto Ask2
$Link:
$ LINK ADD_DICTIONARY
$ IF F$GETSYI ("ARCH_NAME") .NES. "Alpha"
$   THEN
$    ! VAX LINK COMMAND
$    LINK/SHAREABLE VMS$PASSWORD_POLICY+SYS$SYSTEM:SYS.STB/SEL+SYS$INPUT:/OPTIONS
     UNIVERSAL=POLICY_PLAINTEXT
     UNIVERSAL=POLICY_HASH
$   ELSE
$    ! ALPHA LINK COMMAND
$    LINK/SHAREABLE/SYSEXE VMS$PASSWORD_POLICY,SYS$INPUT:/OPTIONS
     SYMBOL_VECTOR = -
       ( -
       POLICY_PLAINTEXT = PROCEDURE,-
       POLICY_HASH      = PROCEDURE -
       )
$ ENDIF
$! 
$Activate:
$ Type SYS$INPUT
  Do you wish to Activate the module?  
$ASK3:
$ inquire junk "Activate?  (Y or N) "
$ IF ANS .EQS. "N" THEN GOTO Instruct
$ IF ANS .EQS. "Y" THEN GOTO GO_ACT
$ goto Ask3
$Go_ACT:
$ COPY VMS$PASSWORD_POLICY.EXE SYS$COMMON:[SYSLIB]*.*/LOG
$ CONVERT/FDL=VMS$PASSWORD-POLICY-WORDS.FDL passwords.dat -
 sys$common:[syslib]VMS$PASSWORD-POLICY-WORDS.DATA
$ type SYS$INPUT
VMS$PASSWORD-POLICY-WORDS.DATA has been put in SYS$LIBRARY

Now installing the module and changing the SYSGEN parameter LOAD_PWD_POLICY
$ INSTALL ADD SYS$LIBRARY:VMS$PASSWORD_POLICY.EXE
$ MCR SYSGEN
USE CURRENT
SET LOAD_PWD_POLICY 1
WRITE CURRENT
USE ACTIVE
SET LOAD_PWD_POLICY 1
WRITE ACTIVE
$ type SYS$INPUT

The system has now been installed and is currently active.
Remember to add the line to SYS$MANAGER:SYSTARTUP_VMS.COM to reinstall the
module.
$ EXIT
$INSTRUCT:
$ TYPE SYS$INPUT
  TO activate the program, you need to copy the executable to SYS$LIBRARY: and 
install it using the command:

     $ INSTALL ADD SYS$LIBRARY:VMS$PASSWORD_POLICY.EXE

and then issue the SYSGEN Commands:

	$ MCR SYSGEN
	SYSGEN> USE CURRENT
	SYSGEN> SET LOAD_PWD_POLICY 1
	SYSGEN> WRITE CURRENT
	SYSGEN> USE ACTIVE
	SYSGEN> SET LOAD_PWD_POLICY 1
	SYSGEN> WRITE ACTIVE

Don't forget to add the INSTALL command SYS$MANAGER:SYSTARTUP_VMS.COM.

$ EXIT
