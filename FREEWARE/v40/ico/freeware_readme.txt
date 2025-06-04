ICO, MOTIF_TOYS, A Rotating Icosahedron Demo

        This demo  is  included  in  the DECW$EXAMPLES directory when you
        install DECWindows.   I  add  one  switch  to  the  demo to allow
        setting of the window's title.  The default title is "Ico".
                        
       
        ** NOTE **

        You must have the logical name  DEMOS defined to point to the top
        level directory that contains  the  ICO  directory.   All command
        procedures rely on this being  defined.    For example if all the
        ICO files are in WORK:[SMITH.ICO] then you  need  a  logical name
        defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **

        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[ICO]BUILD_ICO.COM

        This will  build  either a OpenVMS VAX or OpenVMS AXP image based
        on what architecture it is run on.

                
        To run...
        -----------------------------------------------------------------

        $ @DEMOS:[ICO]START_ICO.COM
        
        This  will  start  up  the  appropiate  architecture  version  of
        ICO.  
            
                                    
        ICO options...
        -----------------------------------------------------------------

        $ ICO :== "$ DEMOS:[ICO]ICO_AXP"
        $ ICO -h

        Usage: demos:[ico]ico_axp.exe;7 [-options]
        
        Where options include:
            -display host:dpy     X server to use
            -geometry geom        Geometry of window to use WxH+X+Y
            -r                    Draw in the root window
            -title windowtitle    Title for Ico's window
            -d number             Dashed line pattern for wire frames
            -colors color ...     Colors to use on sides
            -dbl                  Use double buffering
            -noedges              Don't draw wire frame edges
            -faces                Draw faces
            -i                    Invert
            -sleep number         Seconds to sleep in between draws
            -obj objname          Type of polyhedral object to draw
            -objhelp              List polyhedral objects available
               
        $ ICO -objhelp
        
        Name            ShortName     #Vert.  #Edges  #Faces  Dual
        cube            cube             8      12       6    tetrahedron
        icosahedron     ico             12      30      20    dodecahedron
        tetrahedron     tetra            6      12       8    cube                                       
