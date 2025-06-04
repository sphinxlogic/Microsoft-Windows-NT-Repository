
  DIGITAL InfoServer Client for Windows NT Version V1.1

  File: README.TXT

  CONTENTS
  ========

    Introduction
    Hardware and Software Requirements
    Preparing for Installation
    Installation
    Using InfoServer Client for Windows NT
    Removing InfoServer Client for Windows NT
    Installing on systems that have Pathworks 32 installed
  
  Introduction
  ============

    InfoServer Client for Windows NT is software for browsing,
    connecting, and accessing InfoServer CD-ROM and Disk services 
    through the Microsoft Windows NT Explorer or INFOCON program. Once
    connected to an InfoServer service, the drive may be shared, thus 
    providing access from Windows 95 clients.

    Read this document for information on installing, removing and
    using the InfoServer Client for Windows NT.
 
    After installation, refer to RELNOTES.TXT which contains
    problem descriptions, restrictions and notes on using the
    product.

  Hardware and Software Requirements
  ==================================

    Make sure your system meets or exceeds the minimum hardware
    and software requirements.

    On Microsoft Windows NT version 4.0 systems, the requirements
    are:

     > An Intel(R) or Alpha system referenced in the Microsoft
       Hardware Compatibility List (HCL) for Microsoft Windows
       NT version 4.0

     > The minimum memory required for Microsoft Windows NT
       version 4.0 as specified in the Microsoft HCL

     > The minimum disk space specified for Microsoft Windows NT
       version 4.0 by the Microsoft HCL, plus 1MB disk space for
       InfoServer NT Client for Windows NT

     > Microsoft Windows NT version 4.0 final release software

     > Microsoft Windows NT version 4.0 supported Ethernet, Token
       Ring or FDDI adapter and driver

    InfoServer requirements:

     > A Digital Equipment Corporation InfoServer 100, 150 or 1000
       with at least 1 CD-ROM drive or 1 Hard disk

     > The minimum version of InfoServer software is version 3.1

  Preparing for Installation
  ==========================

    Please note the following:

    * The installation will create the following folder on your
      Microsoft Windows NT system disk:
	
	INFOSRV

    If your system disk is drive C, for example, then you will have a
    new folder named "C:\INFOSRV". This folder will contain the
    release notes, README.TXT and help files.

    InfoServer Client for Windows NT is distributed only on CD-ROM.
    If your system does not have a CD-ROM drive, but does have a
    diskette drive, you will have to go to a system that has a
    CD-ROM drive and diskette drive and copy the entire contents of
    the CD-ROM onto a diskette. You can then use the diskette at
    your system for installation. Or, you can copy the contents of
    the CD-ROM to a shared network drive that your system can access.

    Before installing, remove any previous version of InfoServer 
    Client for Windows NT by following the removal directions in the
    README.TXT for the version you are removing. You should also
    remove all files in the \INFOSRV folder.


    Installation
    ============

	Note: If you have Digital's Pathworks 32 installed on
	your system, please use the installation procedures in
	"Installing on systems that have Pathworks 32 installed."
    
    1.  Insert the DIGITAL InfoServer Client for Windows NT V1.1
	CD-ROM (or diskette that you created) into a drive
	(which is the "installation drive") on your system. 
	If you are installing from the network, connect to the
	network drive (which is the "installation drive") that
	contains the InfoServer Client for Windows NT.
    
    2.  Click on "My Computer", "Control Panel", "Network", and
	"Services".
	
    3.  Click on "Add" and "Have Disk". In the drive letter box,
	enter the correct drive letter and folder (e.g. F:\X86 if
	your installation drive is F:), then choose "OK".
	
	The system will display: "DIGITAL InfoServer Client for
	Windows NT". Choose "OK". 
	
	When the installation procedure completes, choose "Close"
	and "Yes" to restart your system.
    
    To test the installation, run the Microsoft Windows NT Explorer, 
    click on "Network Neighborhood" and "Entire Network". You should
    see an entry for "DIGITAL InfoServer Network".

    
    Using InfoServer Client for Windows NT
    ======================================

    To use the InfoServer Client for Windows NT to find
    InfoServers and services on your Local Area Network, run the
    Microsoft Windows NT Explorer, click on "Network Neighborhood",
    "Entire Network", "DIGITAL InfoServer Network". Click on "CD-ROM
    Services" to display all CD-ROM Services, or "Disk Services" to
    see all Disk Services. Only ISO 9660 CD-ROMs or MS-DOS hard disk
    services can be displayed. If none are available, the display
    will be blank. Refer to the release notes for information on 
    creating an MS-DOS service.

			    NOTE
			    ----

      Once the services are displayed, no further browsing
      is available. That is, clicking on a service to
      display its contents will result in an error message. You
      must connect a drive letter to the service to see its'
      contents or to perform other operations such as running
      applications or copying files.

    To use the InfoServer Client for Windows NT to connect
    a drive letter to an InfoServer service, run the Microsoft Windows
    NT Explorer, click on "Tools" and "Map Network Drive". Select a
    drive and enter the InfoServer name and service in the path box as
    \\SERVER_NAME\SERVICE where SERVER_NAME is the name of the
    InfoServer and SERVICE is the name of the service to which you
    want a connection. Choose "OK". 

    You may also browse from the "Map Network Drive" dialog box
    by clicking on "DIGITAL InfoServer Network", "CD-ROM Services" or
    "Disk Services", and click on the service to make a connection.

    Once you have connected to a service, you access the drive that
    is assigned to the service as if it is a locally connected drive.
    
    To share the drive so Windows 95 clients can access services
    on InfoServers, from Explorer, right-click on the drive, then
    click "Sharing" and complete the required information.

    To disconnect from an InfoServer service using the Microsoft
    Windows NT Explorer, click on "Tools" and "Disconnect Network
    Drive". From the display list, select the service you wish to 
    disconnect and choose "OK".

    From the MS-DOS command prompt, you may use the INFOCON
    program for browsing or connecting to InfoServers. For more
    details, type "INFOCON /?" or "INFOCON /HELP", or, from Explorer, 
    click on "INFOSERV.HLP" in the \INFOSRV folder.

 
  Removing InfoServer Client for Windows NT
  =========================================

    To remove the InfoServer Client for Windows NT, take the
    following steps:


	1. Click "My Computer", "Control Panel", "Network" and
	   "Services".

	2. Click "InfoServer Block Device Driver", "Remove" and
	   choose "OK" (This step will also remove the InfoServer
	   entry).

	3. Click "InfoServer LAST Transport", "Remove" and choose
	   "OK".

	4. Click "InfoServer Protocol Module", "Remove" and choose
	   "OK".

	Note: Do not perform steps 5 and 6 if you have Digital's
	Pathworks 32 installed on your system.

	5. Click "CORE Service Module", "Remove" and choose "OK".

	6. Click "Protocols", "NDIS Data Link Module", "Remove"
	   and choose "OK".

	7. Choose "Close" to restart the system. You must restart the
	   system to complete the removal.


  Installing on systems that have Pathworks 32 installed
  ======================================================

  If your Windows NT system is a server, and you have Pathworks 
  32 installed, use the following procedure to install:

    1.  Follow the instructions in "Preparing for Installation"
	above.

    2.  Insert the DIGITAL InfoServer Client for Windows NT V1.1
	CD-ROM (or diskette that you created) into a drive
	(which is the "installation drive") on your system. 
	If you are installing from the network, connect to the
	network drive (which is the "installation drive") that
	contains the InfoServer Client for Windows NT.
    
    3.  Click on "My Computer", "Control Panel", "Network", and
	"Services".

    4.  If you see a "DEC Block Device Driver" entry under
	Services, click on it followed by "Remove" and "Yes". Then
	click on "Close" followed by "Yes" to restart your system.
	After the system restarts, repeat steps 1, 2 and 3 and 
	continue with step 5.
	  
    
    5.  Click on "Add" and "Have Disk". In the drive letter box,
	enter the correct drive letter and folder (e.g. F:\X86 if
	your installation drive is F:), then choose "OK". 
	
	The system will display: "DIGITAL InfoServer Client for
	Windows NT". Choose "OK". You may see messages during 
	installation that one or more modules are already present 
	on your system. Choose "OK" and disregard the message. Do not 
	select "Update" since this function is not supported.
	
	When the installation procedure completes, choose "Close". 
    
    6.  You are now prompted with the option to restart your system.
	Select "No", then copy DECBLK.SYS, DECLAD.SYS and DECLAST.SYS 
	to your systemroot drivers folder. For example, from an
	MS-DOS prompt:

	    C:\> CD F:\X86
	    F:\X86> COPY DECBLK.SYS %SYSTEMROOT%\SYSTEM32\DRIVERS\
	    F:\X86> COPY DECLAD.SYS %SYSTEMROOT%\SYSTEM32\DRIVERS\
	    F:\X86> COPY DECLAST.SYS %SYSTEMROOT%\SYSTEM32\DRIVERS\

	You can now restart your system (select "Start" and 
	"Shutdown").

    To test the installation, run the Microsoft Windows NT Explorer, 
    click on "Network Neighborhood" and "Entire Network". You should
    see an entry for "DIGITAL InfoServer Network".


