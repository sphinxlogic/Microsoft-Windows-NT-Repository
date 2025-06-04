Return-path: <hornlo@okra.millsaps.edu>
Received: from okra02.millsaps.edu by Cerritos.edu (PMDF V4.2-14 #4768) id
 <01HBM4NNJL2O9GXNE1@Cerritos.edu>; Tue, 26 Apr 1994 02:16:06 PST
Received: by okra.millsaps.edu (MX V4.0-1 VAX) id 21; Tue,
 26 Apr 1994 04:16:51 CDT
Date: Tue, 26 Apr 1994 04:16:54 -0500 (CDT)
From: Larry Horn <hornlo@okra.millsaps.edu>
Subject: QI inetd com file
To: TANNER@Cerritos.edu
Cc: hornlo@okra.millsaps.edu
Message-id: <0097D843.8E55B8A0.21@okra.millsaps.edu>
Content-transfer-encoding: 7BIT

This is the COM file that the QI UCX service points to.

$!----------------------------------------------------------------------------
$! QI_SERVER.COM
$! Last edit: 24-APR-1994 11:50:17.38
$!----------------------------------------------------------------------------
$!
$ DEFINE = "DEFINE"
$ RUN = "RUN"
$ EXIT = "EXIT"
$!
$ mynode = f$edit( f$getsyi("SCSNODE"), "TRIM" )
$!
$ DEFINE SYS$OUTPUT CSO_LOGDIR:QI_SERVER_'mynode'.OUT
$ DEFINE SYS$ERROR  CSO_LOGDIR:QI_SERVER_'mynode'.ERR
$ RUN CSO_EXE:QI.EXE
$ EXIT
$!
$!----------------------------------------------------------------------------
