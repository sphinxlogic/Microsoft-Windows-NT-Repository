        .TITLE  EMUMSG
!
!       This is the message file for the EMU System
! System Wide
        .FACILITY       EMUSYS, 1
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        BADERR  <!%D Bad error structure !AS Name = !AS, Paramcnt = !UL>/FAO=4
        LOCBOX  <!%D !AS Error from LOCATE_BOXID !XL>/FAO=3
        LOCPSR  <!%D !AS Error from LOCATE_PSR !XL, PID= !UL>/FAO=4
        LOCREC  <!%D !AS Error from LOCREC !XL>/FAO=3
        UNLREC  <!%D !AS Error from UNLOCREC !XL>/FAO=3
        LOCKDB  <!%D !AS Error locking ($ENQ) !AS, !XL >/FAO=4
        EXPREG  <!%D !AS Error Getting memeory ($EXPREG)  !XL >/FAO=3
        CREMAP  <!%D !AS Error mapping ($CRMPSC) !AS, !XL >/FAO=4
        MKNMFM  <!%D !AS Error from MAKE_NAMER_FRAME !XL >/FAO=3
        DELETE  <!%D !AS Deleted !UL Records, Status = !XL>/FAO=4
        .SEVERITY       SEVERE
        .SEVERITY       FATAL
! CREATE_RELATER_FRAME
        .FACILITY       CRERLT, 2
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        NOIPC  <!%D Buffer Unavailable !AS >/FAO=2  
        .SEVERITY       ERROR
        BADBOX  <!%D BOXIDs do not match !AS >/FAO=2
        BADACT  <!%D Actions do not match !AS >/FAO=2
        BADSDR  <!%D Senders do not match !AS >/FAO=2
        BADONR  <!%D Sender is not owner !AS >/FAO=2
        BUFOVF  <!%D Could not add to frame !AS >/FAO=2  
        BADSND  <!%D Frame Owner and Sender do not match !AS>/FAO=2  
! GETDN4DATA
        .FACILITY       GETDN4, 3
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        UNRECG  <!%D Unregocnised func or opt !AS !UW.!UW Func = !UL >/FAO=5  
        INVMSG  <!%D Invalid Message format !AS !UW.!UW Func = !UL >/FAO=5 
        NOPRIV  <!%D Privilege Violation !AS !UW.!UW Func = !UL >/FAO=5  
        TOLONG  <!%D Management Message Too Big !AS !UW.!UW Func = !UL >/FAO=5    
        REMFAIL <!%D Remote software failure !AS !UW.!UW Func = !UL >/FAO=5 
        UNRPARM <!%D Unrecognised Paramater Type !AS !UW.!UW Func = !UL >/FAO=5 
        INCVER  <!%D Incompatible Versions !AS !UW.!UW Func = !UL >/FAO=5 
        UNRCOM  <!%D Unregocnised Component !AS !UW.!UW Func = !UL >/FAO=5 
        INVID   <!%D Invalid Identification !AS !UW.!UW Func = !UL >/FAO=5 
        LINERR  <!%D Communications error !AS !UW.!UW Func = !UL >/FAO=5 
        WROSTA  <!%D Component in wrong state !AS !UW.!UW Func = !UL >/FAO=5 
        BADMSG  <!%D Undefined error message !AS !UW.!UW Func = !UL >/FAO=5 
        FILERR  <!%D File open error !AS !UW.!UW Func = !UL >/FAO=5 
        INVFIL  <!%D Invalid File contents !AS !UW.!UW Func = !UL >/FAO=5 
        RESERR  <!%D Resource Error !AS !UW.!UW Func = !UL >/FAO=5 
        INVPAR  <!%D Invalid Paramater Value !AS !UW.!UW Func = !UL >/FAO=5 
        PROERR  <!%D Protocol Error !AS !UW.!UW Func = !UL >/FAO=5 
        FILIO   <!%D File IO Error !AS !UW.!UW Func = !UL >/FAO=5 
        MIRERR  <!%D Mirror Connected then failed !AS !UW.!UW Func = !UL >/FAO=5 
        INTBSP  <!%D Insufficient Table Space !AS !UW.!UW Func = !UL >/FAO=5 
        MIRFAL  <!%D Connection to Mirror Failed !AS !UW.!UW Func = !UL >/FAO=5 
        NOTAPL  <!%D Paramater Not applicable !AS !UW.!UW Func = !UL >/FAO=5 
        TOOBIG  <!%D Paramater too Long !AS !UW.!UW Func = !UL >/FAO=5 
        HARDWAR <!%D Hardware Failure !AS !UW.!UW Func = !UL >/FAO=5 
        OPFAIL  <!%D Operation Failure !AS !UW.!UW Func = !UL >/FAO=5 
        NOTSUP  <!%D Specific Function Not Supported !AS !UW.!UW Func = !UL >/FAO=5 
        INVGRP  <!%D Invalid Paramater Grouping !AS !UW.!UW Func = !UL >/FAO=5 
        BADLOOP <!%D Bad Loop Response !AS !UW.!UW Func = !UL >/FAO=5 
        PARMISS <!%D Paramater Missing !AS !UW.!UW Func = !UL >/FAO=5 

! RELATER
        .FACILITY       RELATR, 4
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        ENADB <!%D !AS Enabled PSR !XL with !UL Records>/FAO=4 
        .SEVERITY       WARNING
        GENVMS  <!%D !AS VMS Error !XL From !AS>/FAO=4 
        VERADDR <!%D !AS Invalid Target addr !XL For PID !XL From !XL>/FAO=5 
        GETDB <!%D !AS Error getting DB !UL, Error = !XL>/FAO=4 
        CHGBOX <!%D !AS Error from CHANGE_BOXID_DB !XL>/FAO=3 
	
        .SEVERITY       ERROR
! SCANNER
        .FACILITY       SCANER, 5
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        INPTFIL  <!%D !AS RMS Error !XL >/FAO=3
        DBREAD   <!%D !AS Database Read error !XL >/FAO=3   
        DBWRITE  <!%D !AS Database Write error !XL >/FAO=3   
        BADHDR   <!%D !AS Bad DB header !XL !XL !XL !XL !XL >/FAO=7   
        BADACT   <!%D !AS Bad Action Directive !UL>/FAO=3
        SNDNAM   <!%D !AS Error from Send Name !UL>/FAO=3   
        SNDRLT   <!%D !AS Error from Send Relater !UL>/FAO=3   
        XLTNAM   <!%D !AS Error from XLATE_FULLNAME !UL>/FAO=3   
        DBACCES  <!%D !AS Error accessing DB !UL, !XL>/FAO=4   
! Config Monitor
        .FACILITY       CFGMON, 6
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        WRITDB   <!%D !AS RMS Error !XL >/FAO=3
        SNDALT   <!%D !AS SEND_ALERT Error !XL >/FAO=3
        SETMIB   <!%D !AS Error opening !AD !XL >/FAO=4
        GENVMS   <!%D !AS Error !XL >/FAO=3
! Alert
        .FACILITY       ALERT, 7
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        NORLT    <!%D !AS Map Relater Error !XL >/FAO=3
        MSGFMT   <!%D !AS Formatting Error !XL >/FAO=3
        XLTPID   <!%D !AS BAD PID = !UL >/FAO=3
        NOIPC    <!%D !AS Map IPC Error !XL >/FAO=3
        NOCTRL   <!%D !AS Map Control Error !XL >/FAO=3
        NOCNTR   <!%D !AS Map Counters Error !XL >/FAO=3
        NOMBX    <!%D !AS $CREMBX Error !XL >/FAO=3
        READMBX  <!%D !AS Read MBX Error !XL >/FAO=3
        WRITEMBX <!%D !AS Write MBX Error !XL >/FAO=3
        READMBXI <!%D !AS Read IOSB Error !XL.!XL >/FAO=4
        NODB     <!%D !AS Could not map to DB !UL, Error = !XL >/FAO=4
        LOCREL   <!%D !AS Could not locate PID.BOXID !UL.!UL in Relater>/FAO=4
        NOALTID  <!%D !AS Could not locate ALTID !UL in PSRdb !UL>/FAO=4
        NOLOG    <!%D !AS Could Log message. Error = !XL>/FAO=3
        SETNAME  <!%D !AS SET_NAME error !XL >/FAO=3
        SETDEV   <!%D !AS SETDEV error !XL >/FAO=3
! PSR
        .FACILITY       GENPSR, 8
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        GENERR   <!%D !AS Detected error = !XL >/FAO=3
        .SEVERITY       ERROR
        UNRPTY   <!%D !AS Unrecognised protocol type received !XW >/FAO=3
        SNDRLT   <!%D !AS Error from SEND_RELATER !XL >/FAO=3
        SNDNAM   <!%D !AS Error from SEND_NAME !XL >/FAO=3
        DELNAM   <!%D !AS Error from DELETE_NAME !XL >/FAO=3
        EXITERR  <!%D !AS Exiting with error = !XL >/FAO=3
        MAPDB    <!%D !AS Error in MAPDB = !XL >/FAO=3
        NOCEF    <!%D !AS Error from ASCEFC error = !XL>/FAO=3
        NOHNDLR  <!%D !AS Error from DCLEXH error = !XL>/FAO=3
        CTLLCK   <!%D !AS GET_CONTROL_LOCK error = !XL>/FAO=3
        ASSMBX   <!%D !AS ASN_TEMMBX_SYSTEM error = !XL>/FAO=3
        MAPCTL   <!%D !AS MAP_CONTROL_SECTION error = !XL>/FAO=3
        MAPCNT   <!%D !AS MAP_COUNTER_SECTION error = !XL>/FAO=3
        MAPNAM   <!%D !AS MAP_NAMER_SECTION error = !XL>/FAO=3
        MAPIPC   <!%D !AS MAP_COMIPC_SECTION error = !XL>/FAO=3
        MAPEBUF  <!%D !AS MAP_EBUFFS error = !XL>/FAO=3
        MAPPSRT  <!%D !AS MAP_PSRTBL error = !XL>/FAO=3
! NAMER
        .FACILITY       NAMER, 9
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        ZERBOX   <!%D !AS BOXID was Zero >/FAO=2
        BADTYP   <!%D !AS Message !UL received from !UL >/FAO=3
! LANMON
        .FACILITY       LANMON, 10
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        NOMBX    <!%D !AS $CREMBX Error !XL >/FAO=3
        MAPCNT   <!%D !AS MAP_COUNTER_SECTION error = !XL>/FAO=3
        MAPCTL   <!%D !AS MAP_CONTROL_SECTION error = !XL>/FAO=3
        NODB     <!%D !AS Could not map to DB !UL, Error = !XL >/FAO=4
        DBWRITE  <!%D !AS Database Write error !XL Header: !XL !XL !XL !XL !XL >/FAO=8   
        SNDALT   <!%D !AS SEND_ALERT Error !XL >/FAO=3
! DBSYS LOCK
        .FACILITY       SYSDBLCK, 11
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        CREATE    <!%D !AS Creating Lock for PID !UL>/FAO=3
        CONVERT   <!%D !AS Converting Lock for PID !UL>/FAO=3
        COMPLT    <!%D !AS Lock for PID !UL Completed with status !XL.!XL>/FAO=5
        RELEASE   <!%D !AS Releasing Lock for PID !UL>/FAO=3
        RELEASD   <!%D !AS Lock for PID !UL Completed with status !XL.!XL>/FAO=5

        .SEVERITY       WARNING
        .SEVERITY       ERROR
! MAP_SECTIONS
        .FACILITY       MAPSEC, 12
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        DELTVA    <!%D !AS Error !XL deleting VM in UNMAP Struct: !XL !XL !XL !XL !XL >/FAO=8
! NODE SCAN
        .FACILITY       NODSCN, 13
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        ADDDN4    <!%D !AS Added node !UW.!UW >/FAO=4
        ADDDN5    <!%D !AS Added node !AS >/FAO=3
        ADDIP     <!%D !AS Added node !UB.!UB.!UB.!UB >/FAO=6
        IPERR     <!%D !AS IPADJ Error from !UB.!UB.!UB.!UB !XL >/FAO=7
        IPCNT     <!%D !AS IPADJ node count from !UB.!UB.!UB.!UB !UL >/FAO=7
	
        .SEVERITY       WARNING
        .SEVERITY       ERROR
! Listener
        .FACILITY       LISTEN, 14
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        GENVMS  <!%D !AS VMS Error !XL >/FAO=3 
        PRMQUE  <!%D !AS Error on Promiscuious que !XL >/FAO=3 
        .SEVERITY       ERROR
        FATAL   <!%D !AS FATAL Error !XL >/FAO=3 
! TSTDUP
        .FACILITY       TSTDUP, 15
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        PSRDEL  <!%D !AS Deleted !UL recs from  DB !UL Status = !XL >/FAO=5   
        DELETE  <!%D !AS Deleted !UL Records, Status = !XL>/FAO=4
        .SEVERITY       WARNING
        DBACCES  <!%D !AS Error accessing DB !UL, !XL>/FAO=4   
        .SEVERITY       ERROR
! REBUILD
        .FACILITY       RBUILD, 16
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        START  <!%D !AS Starting Rebuild for PID !UL >/FAO=3   
        END    <!%D !AS Restored !UL Records for PID !UL >/FAO=4   
        .SEVERITY       WARNING
        .SEVERITY       ERROR
        EXIT   <!%D !AS Error !XL while restoring PID !UL >/FAO=4   
        INVPID <!%D !AS PID !UL is invalid>/FAO=3   
        CREREL <!%D !AS Error from Create_Relater_Frame !XL>/FAO=3   
        SNDREL <!%D !AS Error from Send_Relater_Frame !XL>/FAO=3   
        LOCPSR <!%D !AS Error from LOCATE_PSR !XL>/FAO=3   
        SCNDB  <!%D !AS Error accessing SCANNER.DAT !XL>/FAO=3   
        READSCN <!%D !AS Error reading SCANNER.DAT !XL>/FAO=3   
        DBREAD <!%D !AS Error reading EMUDB.DAT !XL>/FAO=3   
        DBWRITE <!%D !AS Error writing EMUDB.DAT !XL>/FAO=3   
! Performance monitor
        .FACILITY       PRFMON, 17
        .SEVERITY       SUCCESS
        .SEVERITY       INFORMATIONAL
        .SEVERITY       WARNING
        MOPFAIL  <!%D !AS Failed to get counts from !XB-!XB-!XB-!XB-!XB-!XB !UL>/FAO=9   
        .SEVERITY       ERROR
        CTPREAD  <!%D !AS Error reading CNTRPRC.DAT !XL>/FAO=3   
        CTPOPEN  <!%D !AS Error opening CNTRPRC.DAT !XL>/FAO=3   
        CTPADD2  <!%D !AS Error From ADD2RETURN !XL>/FAO=3   
        CTPMATH  <!%D !AS Error From QUADMATH !XL>/FAO=3   
        CTPEXIT  <!%D !AS Exit Handler called !XL>/FAO=3   
        CTPERASE <!%D !AS Error deleting record from CNTRPRC.DAT !XL>/FAO=3   
        CTPPRC  <!%D !AS Error from CNTRPRC !XL>/FAO=3   
        CTPUPD  <!%D !AS Error updating CNTPOLL.DAT !XL>/FAO=3   
        CTPCPR  <!%D !AS Error from COUNTER_PROCESS !XL>/FAO=3   
        CTPTIM  <!%D !AS Error from TIMEDIFF !XL>/FAO=3   


  .END        

