XPOOL, GAMES,  XWindow Pool Game 

        XPOOL is a DECwindows  version  of  a  graphical  pool  game.  It
        allows 2 player mode locally  and  over  the  network,  player vs
        computer, computer against itself and computer  vs  computer in a
        VMScluster.  
        
        Computer vs  computer in a VMScluster was used to demo an OpenVMS
        VAX system playing against an OpenVMS AXP system.  This mode uses
        cluster wide resource locks  to communicate and syncronize shots.
        The computer against itself mode  is  also  useful for standalone
        demos on OpenVMS AXP and OpenVMS VAX.
                
        ** NOTE **

        You must have the logical name  DEMOS defined to point to the top
        level directory  that  contains the XPOOL directory.  All command
        procedures rely on  this  being  defined.  For example if all the
        XPOOL files are in  WORK:[SMITH.XPOOL]  then  you  need a logical
        name defined as follows:
        
        $ DEFINE/SYSTEM/EXECUTIVE DEMOS WORK:[SMITH.] 

        **  Ensure you include the "." after the last directory name  **
        

        To build...
        -----------------------------------------------------------------
                        
        $ @DEMOS:[XPOOL]BUILD_XPOOL.COM    
        
        This will build either a OpenVMS VAX or  OpenVMS  AXP image based
        on what architecture it is run on.
        
        
        To run...
        -----------------------------------------------------------------
        
        $ @DEMOS:[XPOOL]START_XPOOL.COM
        
        This will start up the appropiate architecture version of XPOOL.
                
        
        XPOOL options...
        -----------------------------------------------------------------
        After  starting  up  XPOOL check out the HELP pulldown  for  more
        information on game and startup options.        

