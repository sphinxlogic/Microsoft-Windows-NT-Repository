Abstract
--------

This directory contains a modification of the SHOW command which allows one to
display 1, 5, and 15 minute load averages of the current node.  Load averages
for the number of computables processes and disk queue lengths are displayed.
The top 5 CPU users can also be displayed with the /HOGS qualifier.  If a user
is running at a rate that is consuming more than 30 CPU minutes per hour of
connect time (MPH), the user is flagged with an asterisk.  


Syntax
------

$ SHOW LOAD_AVERAGE
$ SHOW LOAD_AVERAGE /OUTPUT=filename


How to install it
-----------------

Note: CMKRNL is required for the installation.

1. Execute the BUILD.COM file to build it.  You may optionally install it using:

	$ INSTALL ADD SHOW_LOAD_AVERAGE /OPEN /HEADER_RES /SHARED

   The /HOGS qualifier requires WORLD privilege.  If you wish to make this
   qualifier available to non-privileged users, add /PRIVILEGED=WORLD to
   the above INSTALL command.

2. Extract the SHOW verb from SYS$COMMON:[SYSLIB]DCLTABLES using the DECUS
   "VERB" utility or obtain the SHOW.CLD file from the tape used to install
   the current version of VMS.  Modify the SHOW command according to the
   instructions in VERB-MODS.TXT and then put it back in the tables using:

	$ SET COMMAND /TABLE=SYS$COMMON:[SYSLIB]DCLTABLES -
		     /OUTPUT=SYS$COMMON:[SYSLIB]DCLTABLES SHOW

	where "SHOW" is the SHOW.CLD file with the changes.

4. INSTALL /REPLACE DCLTABLES on each node (global pages permitting), and
   log in again to get the new tables (or use SET COMMAND).

5. Include the contents of SYCONFIG.COM in SYS$COMMON:[SYSMGR]SYCONFIG.COM.


But wait, there's more!
-----------------------

Also included in this distribution is a utility which allows you to dynamically
update SYS$ANNOUNCE with system load and login information.  To use it, insert
the following line in SYS$MANAGER:SYLOGICALS.COM (or the node specific logicals
command procedure):

$ @SYS$MANAGER:ANNOUNCE_DAEMON

This procedure will create a detached process which automatically updates the
SYS$ANNOUNCE logical anytime a user attempts to log in.  It consumes virtually
no resources because it hibernates until awakened by somebody attempting to
log in.  You are allowed to insert a message before and after the system load 
information display using two logical names: ANNOUNCE_MSG1 and ANNOUNCE_MSG2.

For example, you might want to define these two logicals as follows:

$ DEFINE /SYSTEM ANNOUNCE_MSG1 "DAFFY - Computing Centre VAX 9000-440"
$ DEFINE /SYSTEM ANNOUNCE_MSG2 "Unauthorized use of this machine is prohibited."

You would then get a display similar to this when attempting to log in:

DAFFY - Computing Centre VAX 9000-440

                  26-JUN-1991 09:49:18

Load averages:   1 min   5 min  15 min

          CPU:    1.20    1.17    1.19   
          I/O:    2.07    1.04    0.07   

Interactive jobs : 618  
Batch jobs       :  14

Unauthorized use of this machine is prohibited.

Username:


*** NOTE: Both logical names are optional.


Author
------

Comments, suggestions, and questions about this software can be directed to the
author at vmsguy@home.com or curtis.rempel@shaw.ca

NOTE: LAVDRIVER.MAR was written by David Kashtan at SRI.COM and is available
      in OpenVMS VAX format in the public domain and on several DECUS 
      collections.  An OpenVMS Alpha version is available with the MultiNet
      TCP/IP package from Process Software and possibly other vendors.
