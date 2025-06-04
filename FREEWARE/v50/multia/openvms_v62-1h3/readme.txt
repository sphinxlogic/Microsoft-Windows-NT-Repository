Unsupported OpenVMS V6.2-1H3 files for Multia Systems

  This procedure describes how to update OpenVMS Alpha V6.2-1H3 to
  bootstrap on a Multia Multi-Client Desktop System. 

  The Multia system is not supported by OpenVMS.

  These files should be used entirely at your own risk.  These
  modifications are not supported by OpenVMS.  These images render 
  your OpenVMS system disk unbootable, and this firmware may render 
  your Multia system unusable.

  0) Make a BACKUP of any files you wish to preserve, and make
     a copy of your OpenVMS Alpha V6.2-1H3 system disk.

  1) You need to load new firmware.  Place the following files:

		QSBYPASS.SCR
		SRM.EXE

     onto a DOS (FAT) format floppy.  Put the floppy into the Multia 
     and power it on.  It will automatically invoke the script and 
     install the SRM console.  When it reinitializes, remove the floppy.

  2) You need to configure the Multia system to bootstrap without 
     requiring a new set of OpenVMS system files.  To do this, you will 
     create a NVRAM script and change certain settings to those required 
     for OpenVMS.

     >>> edit nvram
     * 10 d -p 2050 15
     * 20 d -b eport:4d0 0
     * boot dka0
     * exit
     >>> nvram

     The NVRAM script allows you to store console commands that can be 
     executed by typing NVRAM at the console.  The editor is a BASIC-like 
     editor that takes a line number, and a text string.  The LIST command 
     will show the current line numbers and current contents.  From the
     SRM (>>>) prompt, the command CAT NVRAM will display the current NVRAM 
     script.  The example above also boots the system when invoked, though
     you do not have to do that if you don't want to.

  3) You need to load various new OpenVMS images onto your system disk.

	$ COPY SYS$GYBDRIVER.EXE SYS$COMMON:[SYS$LDR]
	$ COPY SYS$CPU_ROUTINES_1504.EXE SYS$COMMON:[SYS$LDR]
	$ COPY APB.EXE SYS$COMMON:[SYS$LDR]
	$ COPY SYS$EWBTDRIVER.EXE SYS$COMMON:[SYS$LDR]

     The GYBDRIVER removes the need for TGA interrupts.  The CPU routines
     image causes the system to operate as an AlphaBook 1, an Alpha system 
     very similar to the Multia system.  The Ethernet and APB images allow 
     satellite bootstraps.

  4) You will need to create a CD-R or separate disk containing the
     V6.2-1H3 bootable environment, and modify it to contain these
     files.

