TELNETSYM, UTILITIES, Example of modifying OpenVMS Print Symbiont

   This program is an example of how the standard VMS print symbiont can be 
customized to support printers over TCP/IP networks.  The program
replaces two of the VMS symbiont routines, "output" and "job setup", you
may wish to replace other routines based on your specific requirements.
This code is designed for use with DEC TCP/IP Services for OpenVMS (UCX),
though it should also work with the third-party products providing a
BGDRIVER emulation facility.

   This code has been evaluated and tested for its intended purpose, however 
all possible scenarios have not been exercised.  I would suggest complete 
and thorough testing appropriate for your application if you plan to use
this in any type of "production" environment.

     About this Symbiont:

	o PostScript support is limited - PostScript banner pages
          are not generated, and no special functionality to
          recognize PostScript files and place the printer in
          PostScript mode is included
	
        o Error/Log messages are written to the file pointed to by 
	  the logical TELNETSYM_LOGFILE.  If this logical is not 
	  defined then messages are written to SYS$SYSTEM:TELNETSYM.LOG

        o If a host is unreachable then connection attempts will
          be performed every 3 minutes.  (This is presently hardcoded)

 	o links to the remote host are disconnected if idle for more
	  than 2 minutes.

	o The symbiont has been tested with the CISCO STSX10 and 
	  DECSERVER 300 terminal servers.
	

 Using the Print Symbiont:

     Build the symbiont and copy the image to sys$system:

       $ CC TELNETSYM        (use /STANDARD=VAXC if compiling with DEC C)
       $ LINK TELNETSYM, SYS$SHARE:UCX$IPC/LIB, SYS$SHARE:VAXCRTL/LIB
       $ COPY TELNETSYM.EXE SYS$SYSTEM:

     To start a queue using the symbiont do the following:

       $ INIT/QUE/PROCESS=TELNETSYM/START/ON="server_ip_nodename:port_number" 
  

     For example, the command to start the queue "TCP_PRINTER" which should
     send output to a printer connected to a DECserver 300 with 
     an IP node name of TCPSERVER listening on port 2007 would be:  

       $ INIT/QUE/PROCESS=TELNETSYM/START/ON="TCPSERVER:2007" TCP_PRINTER
