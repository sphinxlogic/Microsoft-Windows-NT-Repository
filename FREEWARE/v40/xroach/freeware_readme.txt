XROACH, MOTIF_TOYS, XWindow Screen Hack
        
        XROACH creates Roaches that  run  around  on  the root window and
        hide under any open windows or icons.  
        
        ** NOTE **

        You must  have the logical name DEMOS defined to point to the top
        level directory that  contains the XROACH directory.  All command
        procedures rely on this  being  defined.   For example if all the
        XROACH files are in WORK:[SMITH.XROACH]  then  you need a logical
        name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **

        
        To build...
        -----------------------------------------------------------------
        
        $ @DEMOS:[XROACH]BUILD_XROACH.COM
        
        This will build either a OpenVMS VAX or OpenVMS  AXP  image based
        on what architecture it is run on.    
        
        
        To run...
        -----------------------------------------------------------------
        
        $ @DEMOS:[XROACH]START_XROACH.COM
        
        This  will  start  up  the  appropiate  architecture  version  by
        defining the symbol XROACH to point to the correct executable and
        then running it.  
        
        
        XROACH options:
        -----------------------------------------------------------------
        $ XROACH :== "$ DEMOS:[XROACH]XROACH_AXP.EXE"
        $ XROACH -h
        
        Usage: xroach [options]
        
        Options:
               -display displayname
               -rc      roachcolor
               -roaches numroaches
               -speed   roachspeed
