XNEKO, MOTIF TOYS, XWindow Cat and Mouse Hack

        The XNEKO demo turns the pointer into a  mouse pixmap and opens a
        window with a cat in it that chases the  mouse pointer around the
        screen.
        
        This version of XNEKO came from the original source posted in the
        SPEZKO::DECW_EXAMPLES  notes conference and was ported to OpenVMS
        AXP.

                                       
        ** NOTE **

        You must have the logical name  DEMOS defined to point to the top
        level directory  that  contains the XNEKO directory.  All command
        procedures rely on  this  being  defined.  For example if all the
        XNEKO files are in  WORK:[SMITH.XNEKO]  then  you  need a logical
        name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **

        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[XNEKO]BUILD_XNEKO.COM
        
        This will  build  either a OpenVMS VAX or OpenVMS AXP image based
        on what architecture it is run on.            

        
        To run...
        -----------------------------------------------------------------
        
        $ @DEMOS:[XNEKO]START_XNEKO.COM        
        
        This will start up the appropiate architecture version of XNEKO.
        START_XNEKO_PAUSE.COM uses the pause window instead.
                
        
        XNEKO options...
        -----------------------------------------------------------------

        $ XNEKO :== "$ DEMOS:[XNEKO]XNEKO_AXP"
        $ XNEKO -h
        
        Usage: demos:[xneko]xneko_axp.exe;8

        [-display <display>] [-geometry <geometry>] [-help]
        [-title <title>] [-name <title>] [-iconic] [-pause]
        [-speed <speed>] [-time <time>] [-root] [-demo]
