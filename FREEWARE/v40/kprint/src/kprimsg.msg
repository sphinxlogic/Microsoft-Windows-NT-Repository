! KPRIMSG.MSG - system definition file for KPRINT messages
!
! Version:	'V5.0-000'
!
!****************************************************************************
!*									    *
!*  COPYRIGHT (c) 1978, 1980, 1982, 1984 BY				    *
!*  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    *
!*  ALL RIGHTS RESERVED.						    *
!* 									    *
!*  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   *
!*  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   *
!*  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   *
!*  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   *
!*  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   *
!*  TRANSFERRED.							    *
!* 									    *
!*  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   *
!*  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   *
!*  CORPORATION.							    *
!* 									    *
!*  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   *
!*  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    *
!* 									    *
!*									    *
!****************************************************************************

!++
!
! FACILITY:  VAX/VMS System Macro Libraries
!
! ABSTRACT:
!
!	This file contains the MDL source for KPRINT messages
!
! ENVIRONMENT:
!
!	V5.0 以降
!
!--
!
!
! AUTHOR:  The Nihongo VMS Group	 CREATION DATE:   7-Feb-1985
!
! MODIFICATION HISTORY:
!

!
! DEFINE KPRINT RELATED ERROR/STATUS VALUES
!

 
!
! DEFINE ERROR MESSAGES
!
	.FACILITY	KPRI, 1 /PREFIX=KPRI$_

	.SEVERITY	WARNING

	CODE_CHECK	<漢字コード・チェック; ファイル - !AS> /FAO=1
	LEVEL_II	<ＪＩＳレベル２漢字 '!AC',（コード %X'!XW'）が検出されました。> /FAO=2
	EXPANDED	<ＤＥＣ拡張漢字 '!AC',（コード %X'!XW'）が検出されました。> /FAO=2
	KEISEN  	<罫線漢字 '!AC',（コード %X'!XW'）が検出されました。> /FAO=2
	INVALID_COMMAND	<コマンド '!AD' が誤ってます。無視します。> /FAO=2
	INVALID_PARAM   </!AS 修飾子のパラメータ '!SL' が誤ってます。!/!_省略時設定値を使用します。> /FAO=2

	.SEVERITY	ERROR

	OPENIN		<入力ファイル !AS がオープンできません。> /FAO=1
	OPENOUT		<出力ファイル !AS がオープンできません。> /FAO=1
	CLOSEIN		<入力ファイル !AS がクローズできません。> /FAO=1
	CLOSEOUT	<出力ファイル !AS がクローズできません。> /FAO=1
	READERR		<ファイル !AS が読めません。> /FAO=1
	WRITEERR	<ファイル !AS に書き込めません。> /FAO=1

	.END
