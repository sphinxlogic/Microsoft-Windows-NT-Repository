XMAGnifier, MOTIF_TOYS, Magnifying glass attached to the mouse pointer

        This demo opens a magnifying glass attached to the mouse pointer.
        It will enlarge any area it  covers  when  a  MB  is  pressed  by
        opening  a  magnified  window.    Once in  it's  enlarged  window
        pressing  MB1  causes pixel information for the pixel  under  the
        pointer  to be displayed.  Pressing MB2 or MB3  in  this  window,
        closes  it  and returns you to the magnifying glass.   It  has  a
        various command line options to control the size of the magnifer,
        the level of magnification, etc, listed below.
                        
       
        ** NOTE **

        You must have the logical  name DEMOS defined to point to the top
        level directory that contains the XMAG  directory.    All command
        procedures rely on this being defined.   For  example  if all the
        XMAG files are in WORK:[SMITH.XMAG] then you need  a logical name
        defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **
        
        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[XMAG]BUILD_XMAG.COM

        This will  build  either a OpenVMS VAX or OpenVMS AXP image based
        on what architecture it is run on.
        
                
        To run...
        -----------------------------------------------------------------

        $ @DEMOS:[XMAG]START_XMAG.COM
        
        This  will  start  up  the  appropiate  architecture  version  of
        XMAG.  

                                
        XMAG options...
        -----------------------------------------------------------------

        $ XMAG :== "$ DEMOS:[XMAG]XMAG_AXP"
        $ XMAG -h

        Usage: demos:[xmag]xmag_axp.exe;5 [-options ...]

        Where options include:

        -display displaystring        X server to contact
        -geometry geomstring          size and location of enlargement
        -source geomstring            size and location of area to be enlarged
        -mag magfactor                integral amount by which area is enlarged
        -bw pixels                    border width in pixels
        -bd color                     border color
        -bg colororpixelvalue         background color or %pixel number
        -fn fontname                  name of font for examining enlarged pixels
        -z                            grab server during dynamics and XGetImage

        Options may be abbreviated to the shortest unambiguous prefix.
