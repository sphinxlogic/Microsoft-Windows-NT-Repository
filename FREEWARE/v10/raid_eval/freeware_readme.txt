RAID Evaluator, SYSTEM MANAGEMENT, StorageWorks(TM) RAID Evaluator program

This directory contains a copy of the StorageWorks(TM) 
RAID Evaluator program, Version 1.9.  V1.9 is an experimental 
version of the program. It contains new features, but has not been 
thoroughly tested. It is being released primarily for fall, 1993 
DECUS attendees, although it may be made available to others.

----------------------------
New Features Of Version V1.9
----------------------------

1. Each child window now contains a "Print" button which enables 
printing of its contents to a laser printer. This feature has been 
tested with the Apple LaserWriter Plus, the Digital LN03R, and 
the Digital LPS20 and LPS40 laser printers. Whether it works properly 
with other laser printers is not known. The File menu contains a 
"Printer Setup..." command to provide printer setup local to the 
RAID Evaluator program.

2. Data about Digital's latest disk and controller products has been 
added to the built-in disk and controller types in the RAID Evaluator 
program. This enables you to construct hypothetical configurations 
using these products. Cautions: like previous RAID Evaluator versions, 
this one does not restrict modeled configurations to valid ones. 
Moreover, the variable maximum data transfer rates of the newer 
'banded' disk drives have been approximated by averages. 

*********************************************************************

			Most Important:

The RAID Evaluator model is based on disk mechanical characteristics.
It DOES NOT MODEL cache. This is especially important to note in the
case of cached controllers (HSC70, HSC9x, HSJ40), where performance
can be seriously understated. This will be fixed in a future release
of the model.

*********************************************************************

3. A new "Parameters" menu has been added to the RAID Evaluator menu 
bar. This menu has two commands, "Controllers..." and "Disks...". 
These allow you to add user-defined controller and disk types 
respectively. User-defined controller and disk types do not become 
available until the session following the one in which they are added. 
The "Parameters" menu applies only to user-defined controller and 
disk types. It is not possible to alter the parameters of the 
built-in disks and controllers. Approximately 20 User-defined 
controller types and 40 User-defined disk types may be added. 
The program is supplied with two hypothetical controller types called 
"UserControllerA" and "UserControllerB" and six disk types called 
"UserDisk1A" through "UserDisk3B" to give you an idea of how to use
the user-specified controller and disk types. 

-------------------------------------------------
General Description of the RAID Evaluator Program
-------------------------------------------------

The RAID Evaluator is a simple application for the Microsoft(TM) 
Windows V3.1 or later environment that can help you understand the 
tradeoffs among various RAID configurations and appreciate the 
performance, data reliability, and cost characteristics of various 
RAID alternatives using your own I/O workload and hardware 
configuration definitions.

-------------------------
Installation Instructions
-------------------------

To install and use the RAID Evaluator program, do the following:

1. Create a directory on your PC hard drive, for example:

   C:\RAIDEVAL\

2. Copy the following files from the distribution diskette to 
the directory:

    QREV19WN.EXE    -- program image (windows version)
    RAIDEVAL.INI    -- file of drive/controller parameters
    *.WLD           -- sample workload files (This is optional)
    QRE_HELP.HLP    -- help file

3. Use any of the standard Windows mechanisms for running the 
program. If you make a Windows association between file type .WLD 
and QREV19WN.EXE, the RAID Evaluator will start executing and 
automatically open any .WLD file that you double-click from 
File Manager.

4. When the RAID Evaluator starts executing, you first see a 
panel in its main window that describes the limits of its modeling 
capability. Be sure you read this panel to understand the program's 
capabilities and limitations.

5. When you have read the introductory panel, use the <ENTER> key 
or double-click the <OK> button to start. You may either enter 
your own workload data into the input dialog panel that appears, 
or you may use the <FILE> menu to open an existing .WLD file. 
A single .WLD file generally contains several workloads. The
names of the available workloads can be listed at any time by 
pulling down the Workload Name list box in the input dialog panel.

6. At any time, you may press the F1 key for help. When you do this, 
the cursor changes to the words "QRE HELP" with an upward left 
pointing arrow. If you move the cursor to the RAID Evaluator window 
object you want help on (the cursor will change back to a text 
cursor or arrow) and click, you will get a help panel describing 
the object you have clicked. This works with menu commands as well. 
After each invocation of HELP, RAID Evaluator program returns to 
normal mode.

Note: I have encountered problems with the HELP cursor not 
appearing on some monitor/video board combinations. The circumvention 
seems to be to minimize the RAID Evaluator main window and then 
restore it. The next time you press "F1", the HELP cursor should appear.

You may also use the Index command on the Help menu to invoke the 
Help utility for browsing, or Search to go directly to the help search 
list. The updating of Help to include the new features is not complete.
What is there is generally valid, but you may encounter some missing
context-sensitive help items.
