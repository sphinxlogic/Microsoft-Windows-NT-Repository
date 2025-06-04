XGLOBE, MOTIF_TOYS, XWindow spinning earth
        
        This version of  XGLOBE  came  from the original source posted in
        the  SPEZKO::DECW_EXAMPLES notes conference  and  was  ported  to
        OpenVMS AXP.  Included in  this saveset are the various sizes and
        orientations of the globe pixmap.   There  is  the original BLISS
        code included to both shrink and rotate  the globe pixmap.  These
        were used on OpenVMS VAX to create the various pixmap datafiles.

        
        ** NOTE **

        You must have the logical name  DEMOS defined to point to the top
        level directory that contains the XGLOBE directory.   All command
        procedures rely on this being defined.  For  example  if  all the
        XGLOBE  files are in WORK:[SMITH.XGLOBE] then you need a  logical
        name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **

        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[XGLOBE]BUILD_XGLOBE.COM p1    
        
        Where p1 = 64, 128 or 256 for globe size (eg: 128x128 pixels)
        
        This will build either a OpenVMS VAX or  OpenVMS  AXP image based
        on what architecture it is run on.
                                                                     
        
        To run...
        -----------------------------------------------------------------
        
        $ @DEMOS:[XGLOBE]START_XGLOBE.COM p1 p2
        
        ;where p1 = 64, 128 or 256 for globe size (eg: 128x128 pixels)
        ;where p2 = R for rotated or leave blank for vertical orientation
        
        This will  start up the appropiate architecture version of XGLOBE
        of either 64x64,  128x128  or  256x256  pixels in size and either
        vertically oriented or rotated to the left.
                
        
        XGLOBE options...
        -----------------------------------------------------------------
        Besides the p1, p2 parameters,  XGLOBE also allows you to specify
        the window  positioning  (-x  -y),  delay (-d) between frames and
        (-t) title of  the  window.    You  can  edit  the values used in
        START_XGLOBE.COM.
        
        $ XGLOBE :== "$ DEMOS:[XGLOBE]XGLOBE_256_AXP"
        $ XGLOBE -h

        Usage: demos:[xglobe]xglobe_256_axp.exe;8 [ options ]

             where options are one or more of:
             [-x #] [-y #] [-d delay] [-t WindowTitle]
                                                  

        Revision history...
        -----------------------------------------------------------------

        11-22-92 - previous  version  would  consume 100% of the CPU in a
        dead loop, waiting to rotate the  pixmap.    Added  a lib$wait in
        XGLOBE.C's  main  loop  to reduce the CPU  to  about  3%.    This
        required adjusting the -d value used in START_XGLOBE.COM as well.
