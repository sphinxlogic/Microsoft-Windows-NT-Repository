XShuffle, MOTIF TOYS, Turns screen into a multi square sliding puzzle.

        This demo turns your screen into a multi square sliding puzzle.
                
       
        ** NOTE **

        You must have the logical name  DEMOS defined to point to the top
        level  directory  that  contains  the  XSHUFFLE directory.   All
        command  procedures rely on this being defined.  For  example  if
        all the XSHUFFLE files are in WORK:[SMITH.XSHUFFLE] then you need
        a logical name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **
        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[XSHUFFLE]BUILD_XSHUFFLE.COM

        This will  build  either a OpenVMS VAX or OpenVMS AXP image based
        on what architecture it is run on.
                
        To run...
        -----------------------------------------------------------------

        $ @DEMOS:[XSHUFFLE]START_XSHUFFLE.COM
        
        This  will  start  up  the  appropiate  architecture  version  of
        XSHUFFLE.  

                                
        XSHUFFLE options...
        -----------------------------------------------------------------

        $ XSHUFFLE :== "$ DEMOS:[XSHUFFLE]XSHUFFLE_AXP"
        $ XSHUFFLE -h
        
        Unknown option -h
        
        XShuffle [-options]
        
                 [-numsquares n]       : Number of squares both ways
                 [-xnumsquares n]      : Number of squares across
                 [-ynumsquares n]      : Number of squares down
                 [-display disp]       : Display to use
                 [-border]             : Draw a border
                 [-solve]              : Undo moves at end
                 [-max n]              : Size of square area
                 [-xmax n]             : Width of area
                 [-ymax n]             : Height of area
                 [-moves n]            : Maximum moves to make
                                                                     
        

