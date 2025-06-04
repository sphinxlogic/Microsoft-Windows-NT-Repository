ColorWheel, MOTIF_TOYS, displays a multicolor wheel rotating 

        This demo  displays  a  multicolor  wheel rotating in a resizable
        window.  The  various  colormap  and  window options are detailed
        below.
        
       
        ** NOTE **

        You must have the logical  name DEMOS defined to point to the top
        level  directory  that contains the COLORWHEEL  directory.    All
        command procedures rely on this being defined.    For  example if
        all the COLORWHEEL files are in WORK:[SMITH.COLORWHEEL] then  you
        need a logical name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **

        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[COLORWHEEL]BUILD_COLORWHEEL.COM

        This will  build  either a OpenVMS VAX or OpenVMS AXP image based
        on what architecture it is run on.

                
        To run...
        -----------------------------------------------------------------

        $ @DEMOS:[COLORWHEEL]START_COLORWHEEL.COM
        
        This  will  start  up  the  appropiate  architecture  version  of
        COLORWHEEL

                                
        ColorWheel options...
        -----------------------------------------------------------------

        $ COLORWHEEL :== "$ DEMOS:[COLORWHEEL]COLORWHEEL_AXP"
        $ COLORWHEEL -h
        
        
        Usage: ColorWheel [options] server::0
        
        -g WxH+X+Y     :Window size and location
        -nochroma      :Do this with grey shades
        -cmap          :Use an alternate colormap (else default)
        -colors n      :How many colors in the wheel
        -flush         :Flush buffer after color update
        -hop n         :How much to rotate wheel each step
        -install       :Install a new colormap
        -psize n       :How many colors per StoreColor request
        -primary n     :The doColor mask for storecolors
        -setcmap       :Set cmap in window after is is mapped
        -sync          :Wait for server after each op
        -allocall      :Allocall on new colormap
        -v             :Verbose info on colormaps
                                                               

