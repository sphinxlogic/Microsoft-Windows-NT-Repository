SunClock, MOTIF_TOYS, XWindow SunClock Display

        This demo is  a  clock  that  shows  which portion of the Earth's
        surface is illuminated by the Sun. 

        This version of SunClock  has  evolved  from  the original source
        posted in the  SPEZKO::DECW_EXAMPLES  notes conference.  I ported
        the code to OpenVMS AXP and  added several command line switches.
        For more information on the original code  see  the  SUNCLOCK.DOC
        file.  The command line switches are documented below.
                
       
        ** NOTE **

        You must have the logical name  DEMOS defined to point to the top
        level  directory  that  contains  the  SunClock  directory.   All
        command  procedures rely on this being defined.  For  example  if
        all the SunClock files are in WORK:[SMITH.SUNCLOCK] then you need
        a logical name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **

        
        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[SUNCLOCK]BUILD_SUNCLOCK.COM

        This will  build  either a OpenVMS VAX or OpenVMS AXP image based
        on what architecture it is run on.

                
        To run...
        -----------------------------------------------------------------

        $ @DEMOS:[SUNCLOCK]START_SUNCLOCK.COM
        
        This  will  start  up  the  appropiate  architecture  version  of
        SunClock.  You will  need  to  edit  this startup file to set the
        appropiate timezone you are in and  your  offset  from  GMT.  The
        offset from GMT is required to correctly  position  the sun.  The
        zone may  be  any  3 characters to represent the timezone you are
        in.  For  example,  in the Eastern USA the -zone is "EST" and the
        -offset is 5.   To  retain  the  case  for  -zone,  quote  the  3
        characters.
                                
        SunClock options...
        -----------------------------------------------------------------

        $ SUNCLOCK :== "$ DEMOS:[SUNCLOCK]SUNCLOCK_AXP"
        $ SUNCLOCK

        Usage: sunclock_axp -offset # [options]
        
         -offset #             : Required - # of hours from GMT
                                 eg: EST is 5, PST is 8
        [-zone aaa]            : Timezone, eg: EST, maximum of 3 characters
        [-display dispname]    : Target display
        [-wtitle windowtitle]  : SunClock's window title
        [-geometry +x+y]       : Window geometry
        [-version]             : Displays SunClock's version
        [-iconic]              : Start up as an icon
        [-ititle icontitle]    : Icon's title
        [-icongeometry +x+y]   : Icon geometry
        
        
