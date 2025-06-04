KPRINT, UTILITIES, Japanese printer formatter utility

KPRINT allows to specify various printer parameters such as line pitch, 
character pitch, page orientation, etc. when printing to Japanese printers.

KPRINT used to be shipped with OpenVMS/Japanese VAX and Alpha Operating System 
until V6.1. As of OpenVMS/Japanese V6.2, KPRINT utility has officially retired 
and no longer ships with OpenVMS/Japanese V6.2 kit. KPRINT is now provided as 
an unsupported freeware to customers who still want to use this facility on 
OpenVMS/Japanese V6.2 or higher.

Please read KPRINT.PS or KPRINT.PS. They are KPRINT command reference manuals
written in Japanese.

To use KPRINT utility, execute the following command.

   $ @freeware_cd:[KPRINT]KPRINT_SETUP

Above command procedure defines KPRINT image location and KPRINT command 
definition for a user. After executing this, user can invoke the utility
by entering KPRINT. You must be running OpenVMS/Japanese because KPRINT
image requires OpenVMS/Japanese runtime libraries.

System manager may want to define KPRINT as a system wide DCL command verb.
JSY_KPRINT_VAX.CLD and JSY_KPRINT_ALPHA.CLD are provided so that system 
manager can add the verb KPRINT into SYS$LIBRARY:DCLTABLES.EXE. 

System manager may also want to provide DCL help. To do this, enter the 
following command.

   $ LIBRARY/INSERT SYS$COMMON:[SYSHLP.JA_JP]HELPLIB.HLB -
     JSY_KPRINT_[VAXorALPHA].HLP

Source files are available under [KPRINT.SRC] directory on the Freeware 
CD-ROM. Source files are common to VAX and Alpha. They are conditionally 
compiled depending on the CPU architecture you are running.

To build KPRINT utility, execute the following command.

   $ @freeware_cd:[KPRINT.SRC]KPRINT_BLD

You need VAX C (for VAX) or DEC C (for Alpha) compiler to build KPRINT.
You must also have OpenVMS/Japanese Operating System installed for building.

