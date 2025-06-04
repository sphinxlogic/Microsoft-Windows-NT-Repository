KDUMP, UTILITIES, Kanji DUMP utility

KDUMP is a Japanese version of OpenVMS DUMP utility.
It is suitable to dump text files containing Japanese Kanji or 
Hankaku Katakana characters. KDUMP used to be shipped with OpenVMS/Japanese 
VAX Operating System until V6.1. As of OpenVMS/Japanese V6.2, KDUMP utility 
has officially retired and no longer ships with OpenVMS/Japanese VAX V6.2 
kit. KDUMP is now provided as an unsupported freeware to customers who still
want to use this facility on OpenVMS/Japanese V6.2 or higher.

Please read KDUMP.PS or KDUMP.PS. They are KDUMP command reference manuals
written in Japanese.

To use KDUMP utility, execute the following command.

   $ @freeware_cd:[KDUMP]KDUMP_SETUP

Above command procedure defines KDUMP image location and KDUMP command 
definition for a user. After executing this, user can invoke the utility
by entering KDUMP.

System manager may want to define KDUMP as a system wide DCL command verb.
JSY_KDUMP.CLD is provided so that system manager can add the verb KDUMP
into SYS$LIBRARY:DCLTABLES.EXE. 

System manager may also want to provide DCL help. To do this, enter the 
following command.

   $ LIBRARY/INSERT SYS$COMMON:[SYSHLP.JA_JP]HELPLIB.HLB JSY_KDUMP.HLP

Source files are available under [KDUMP.SRC] directory on the Freeware 
CD-ROM. To build KDUMP utility, execute the following command.

   $ @freeware_cd:[KDUMP.SRC]KDUMP_BLD

You need VAX BLISS-32 and MACRO compiler to build KDUMP.
VAX BLISS-32 is also available from OpenVMS Freeware CD-ROM.

