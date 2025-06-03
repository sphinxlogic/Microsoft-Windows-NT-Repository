PCM, System Management, Scan Profiles for POLYCENTER Console Manager

POLYCENTER Console Manager allows you to monitor console activity and manage
any device that sends and receives data via RS323 link in ASCII format.

If you have Console Manager installed, you may utilize any of the following
scan profiles in your PCM environment.  Follow the instructions to import the
events you want PCM to monitor.

Scan File Name			Description
--------------			-----------

PCM_SCAN_DCPS.PORT        	DECprint Supervisor 
PCM_SCAN_LPS40.PORT		Print Server 40
PCM_SCAN_DCSC.PORT              StorageTek Silo Software
PCM_SCAN_DECNSR.PORT            Network Save and Restore
PCM_SCAN_LANBRIDGE.PORT 	Vitalink Lan Bridge
PCM_SCAN_RAID5.PORT        	Host Based Raid 5
PCM_SCAN_RSTSV9.PORT       	RSTS/E V9.0
PCM_SCAN_WANROUTER_90.PORT      WANRouter 90
PCM_SCAN_PLY_PERF_ADV.PORT	POLYCENTER Performance Advisor
PCM_SCAN_POLY_NETWORKER.PORT    POLYCENTER Networker
PCM_SCAN_SCHEDULER.PORT         POLYCENTER Scheduler
PCM_SCAN_SLS.PORT         	POLYCENTER Storage Library System
PCM_SCAN_PCM_INTERNALS.PORT	POLYCENTER Console Manager
PCM_SCAN_HSC50.PORT        	Hierarchical Storage Controller 50
PCM_SCAN_HSC70.PORT       	Hierarchical Storage Controller 70
PCM_SCAN_HSJ40.PORT        
PCM_SCAN_HSZ10.PORT        
PCM_SCAN_KL10.PORT         
PCM_SCAN_KS10.PORT         

Operating Systems:

PCM_SCAN_VMSV5.PORT             OpenVMS V5
PCM_SCAN_VMSV6.PORT             OpenVMS V6
PCM_SCAN_ULTRIX.PORT      	Ultrix Risc
PCM_SCAN_AIX.PORT               IBM AIX

Hardware:

PCM_SCAN_MICROVAX2000.PORT
PCM_SCAN_MICROVAX3000.PORT
PCM_SCAN_MICROVAXII.PORT
PCM_SCAN_VAX4000.PORT		
PCM_SCAN_VAX6000.PORT	
PCM_SCAN_VAX6200.PORT
PCM_SCAN_VAX6300.PORT		
PCM_SCAN_VAX750.PORT       	
PCM_SCAN_VAX7600.PORT
PCM_SCAN_VAX780.PORT       		
PCM_SCAN_VAX8200.PORT	
PCM_SCAN_VAX8300.PORT
PCM_SCAN_VAX8500.PORT		
PCM_SCAN_VAX8600.PORT	
PCM_SCAN_VAX8700.PORT
PCM_SCAN_VAX8800.PORT		
PCM_SCAN_VAX9000.PORT

                  
Importing Scan Profiles:

- Digital recommends that you backup your existing database configuration file
  stored as:
	OpenVMS:	CONSOLE$DATA:CONSOLE_CFG.DAT.
        Digital UNIX:	/var/opt/console/data/console_cfg.dat

- Copy the scan profiles off the CD-ROM to your PCM local directory.

- Import scan profiles using the configuration editor.

	OpenVMS:  	$ Console Edit/Interface=DECWindows
        UNIX:     	# console -e

- Import new scan profiles to the PCM database from the DECWindows interface or
  from the command line, by typing the following.

	PCM Edit>import pcm_scan_'profilename'.port   

- Reconfigure PCM to activate changes made to the database.  Do this from the
  DECWindows interface or from the operating system of your choice.

	OpenVMS:	$ Console Reconfigure
        UNIX:           # console -r
