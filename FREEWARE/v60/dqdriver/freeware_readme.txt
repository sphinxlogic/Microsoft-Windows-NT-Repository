DQDRIVER, SOFTWARE, ATA/ATAPI (IDE) Disk Driver for OpenVMS Alpha

    Beware: This version is newer than OpenVMS Alpha V7.3-2.

    SYS$DQDRIVER is an ATA/ATAPI (IDE) disk driver for OpenVMS Alpha. 
    The driver was originally written and tested on an add-in ATA
    controller board as well as the on-board IDE controller found
    on early members of the AlphaStation series.

    The driver has been substantially revised in subsequent releases
    and includes ATAPI CD-ROM device support.  In addition, the driver
    has been incorporated in OpenVMS Alpha and is supported on various
    recent platforms.

    This version of SYS$DQDRIVER continues the inclusion of IO$_DIAGNOSE
    for use with CD-ROM audio and also for use with ATAPI devices such
    as CD-RW and DVD+RW.

    This version is newer than that included with OpenVMS Alpha V7.3-2, 
    and contains changes to IO$_DIAGNOSE not found in V7.3-2 and prior.
    This version particularly corrects cases where the SS$_NORMAL was
    erroneously returned when the requested IO$_DIAGNOSE operation had
    failed.  There may be (will be?) changes  made to this version prior
    to its release either as an ECO kit and/or with an OpenVMS Alpha
    and/or OpenVMS I64 release after V7.3-2.

    The driver is provided as an example, and as with any hardware-based
    tools, you should use at your own risk.  Failures within DQDRIVER can
    lead to OpenVMS system crashes, and errors may or may not result in
    disk and memory data inconsistencies or corruptions.

    A version of DECW$CDPLAYER been included in the kit.  This version
    contains enhancements for ATAPI CD-ROM audio support via the new
    IO$_DIAGNOSE interface in DQDRIVER.  The new version of the CD player
    can be used to play CD-ROM audio from either ATA/ATAPI drives or from
    SCSI CD-ROM drives.  

    This package has the following code modules:

	DQDRIVER.C		- ATA/ATAPI device driver source file.
        DQDRIVER.COM		- Build file for SYS$DQDRIVER
	ENABLE-IDE.C 		- tool to enable the built-in ATA for the
			  	  AlphaStation 400 4/233 and load the ATA
    				  driver.  (Beware: DQDRIVER is presently
                                  autoconfigured on all Alpha systems with
                                  support for ATA/ATAPI devices.)
	ENABLE-IDE.COM 		- Build file for ENABLE-IDE.C
	IDE-INFO 		- tool to dump the identity page information 
    				  from an ATA drive.
	IDE-INFO 		- Build file for IDE-INFO.C
	DECW$CDPLAYER.C		- New version of DECwindows CD-ROM audio 
    				  player with support for ATAPI CD-ROMs.
    	DECW$CDPALYER.UIL	- Interface definition file for CD player
	DECW$CDPLAYER.EXE	- Excutable for V7.1-2 
    	DECW$CDPALYER.UIL	- Compiled Interface definition file.  Must 
                                  be co-located with the .EXE file.

    Also included in this directory is the procedure LOAD_DQ_INTEL_SIO.COM,
    which will connect and load DQDRIVER for operation with the Intel SIO
    peripheral controller found on some members of the Personal Workstation
    -au series Alpha systems.  (DQDRIVER is supported with and autoconfigures
    on and can boot via the Cypress ATA/ATAPI PCI Peripheral Controller found
    on various Personal Workstation -au series systems.  OpenVMS cannot boot
    from ATA/ATAPI (IDE) devices connected via the Intel SIO PCI adapter. 
    If your system has the Intel SIO, you must boot from a SCSI device.)

    To determine which ATA/ATAPI controller you have, use the SRM console
    command SHOW CONFIGURATION.   If your controller is shown as:

             Bus 00 Slot 07: Cypress PCI Peripheral Controller

    You can directly use DQDRIVER, can boot from supported ATA/ATAPI devices,
    and do not require LOAD_DQ_INTEL_SIO.COM.  If your controller is:

             Bus 00 Slot 07: Intel SIO 82378

    You cannot bootstrap from any ATA/ATAPI devices, and DQDRIVER will not 
    configure automatically.  If you wish to use DQDRIVER with ATA/ATAPI
    devices connected via the Intel SIO, you will need LOAD_DQ_INTEL_SIO.COM.

