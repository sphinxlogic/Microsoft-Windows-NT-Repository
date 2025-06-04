$!****************************************************************************
$! Copyright © 1992, 1994, 1996 by Brian Schenkenberger.  ALL RIGHTS RESERVED.
$!
$! This software is provided "AS IS" and is supplied for informational purpose
$! only.  No warranty is expressed or implied and no liability can be accepted
$! for any actions or circumstances incurred from the use of this software or 
$! from the information contained herein.  The author makes no claim as to the
$! suitablility or fitness of the software or information contain herein for a
$! particular purpose.
$!
$! Permission is hereby granted *ONLY* for the "not-for-profit" redistribution
$! of this software provided that ALL SOURCE and/or OBJECT CODE remains intact
$! and ALL COPYRIGHT NOTICES remain intact from its original distribution.  
$!
$!(!) NO TITLE TO AND/OR OWNERSHIP OF THIS SOFTWARE IS HEREBY TRANSFERRED. (!)
$!****************************************************************************
$!++
$! SSINT's BUILD.COM file should be used when building on systems which
$! are using Digital's MMS utility to define the macro __ALPHA__ which is
$! defined automatically when building with the Freeware MMK utility.  
$!
$! Also, although not currently available, if an Alpha implementation with
$! with a page size greater than 8K is available, this procedure should be
$! used to determine the target Alpha's page size and build the loadable
$! execlet accordingly.
$!--
$ IF "''F$type(MMK)'".NES."" THEN $ MMS = 'MMK'		! MMS or MMK
$ IF "''F$extract(F$getsyi("HW_MODEL").GE.1024,3,"VAXP")'".EQS."VAX"
$ THEN
$   MMS
$ ELSE
$   MMS/MACRO=(PAGESIZE='F$element(F$getsyi("PAGE_SIZE")/%x4000,"/","13/14/15//16"),__ALPHA__=1)
$ ENDIF
