YADMAN, MOTIF_TOYS, MandelBrot Demo

        This  demo,  Yadman  (Yet Another Damn  MANdelbrot  program)  can
        generate  and  display  images  of  the  Mandelbrot   set.    The
        Mandelbrot   set  is  produces  images  of  fractals,  fractional
        dimensions,  that  are  pleasing  to  the  eye.    Look   at  the
        OVERVIEW.TXT for complete details.
        
       
        ** NOTE **

        You must  have the logical name DEMOS defined to point to the top
        level directory that  contains the YADMAN directory.  All command
        procedures rely on this  being  defined.   For example if all the
        YADMAN files are in WORK:[SMITH.YADMAN]  then  you need a logical
        name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **
        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[YADMAN]BUILD_YADMAN.COM

        This will  build  either a OpenVMS VAX or OpenVMS AXP image based
        on what architecture it is run on.
                
        To run...
        -----------------------------------------------------------------

        $ @DEMOS:[YADMAN]START_YADMAN.COM
        
        This  will  start  up  the  appropiate  architecture  version  of
        YADMAN.  

                                
        YADMAN options...
        -----------------------------------------------------------------

        $ YADMAN :== "$ DEMOS:[YADMAN]YADMAN_AXP"
        $ YADMAN
        
        Usage: demos:[yadman]yadman_axp.exe;1 [ options ]
        
             Where options are one or more of:
        
             [-x #] [-y #]                  x and y window position
             [-title WindowTitle]           Title of YADMAN window
