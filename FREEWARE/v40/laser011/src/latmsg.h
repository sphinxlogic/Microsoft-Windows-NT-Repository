/**/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1990                                                      **/
/**  by DIGITAL Equipment Corporation, Maynard, Mass.                        **/
/**                                                                          **/
/**  This software is furnished under a license and may be used and  copied  **/
/**  only  in  accordance  with  the  terms  of  such  license and with the  **/
/**  inclusion of the above copyright notice.  This software or  any  other  **/
/**  copies  thereof may not be provided or otherwise made available to any  **/
/**  other person.  No title to and ownership of  the  software  is  hereby  **/
/**  transferred.                                                            **/
/**                                                                          **/
/**  The information in this software is subject to change  without  notice  **/
/**  and  should  not  be  construed  as  a commitment by DIGITAL Equipment  **/
/**  Corporation.                                                            **/
/**                                                                          **/
/**  DIGITAL assumes no responsibility for the use or  reliability  of  its  **/
/**  software on equipment which is not supplied by DIGITAL.                 **/
/**                                                                          **/
/******************************************************************************/
/********************************************************************************************************************************/
/* Created 20-JUL-1990 13:25:46 by VAX SDL V3.2-12     Source: 20-JUL-1990 13:25:10 RESD$:[LAT_KIT.GENERATE_FILES]LATMSG.SDL;5 */
/********************************************************************************************************************************/
 
/*** MODULE $LATDEF ***/
/*                                                                          */
/*                                                                          */
/* MODULE:   LATMSG.MSG                                                     */
/* VERSION:  'V05-37'                                                       */
/*                                                                          */
/***************************************************************************** */
/**									    * */
/**  COPYRIGHT (C) 1978, 1980, 1982, 1984, 1985, 1987 BY			    * */
/**  DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASSACHUSETTS.		    * */
/**  ALL RIGHTS RESERVED.						    * */
/** 									    * */
/**  THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY BE USED AND COPIED   * */
/**  ONLY IN  ACCORDANCE WITH  THE  TERMS  OF  SUCH  LICENSE  AND WITH THE   * */
/**  INCLUSION OF THE ABOVE COPYRIGHT NOTICE. THIS SOFTWARE OR  ANY  OTHER   * */
/**  COPIES THEREOF MAY NOT BE PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY   * */
/**  OTHER PERSON.  NO TITLE TO AND OWNERSHIP OF  THE  SOFTWARE IS  HEREBY   * */
/**  TRANSFERRED.							    * */
/** 									    * */
/**  THE INFORMATION IN THIS SOFTWARE IS  SUBJECT TO CHANGE WITHOUT NOTICE   * */
/**  AND  SHOULD  NOT  BE  CONSTRUED AS  A COMMITMENT BY DIGITAL EQUIPMENT   * */
/**  CORPORATION.							    * */
/** 									    * */
/**  DIGITAL ASSUMES NO RESPONSIBILITY FOR THE USE  OR  RELIABILITY OF ITS   * */
/**  SOFTWARE ON EQUIPMENT WHICH IS NOT SUPPLIED BY DIGITAL.		    * */
/** 									    * */
/**									    * */
/***************************************************************************** */
#define LAT$_FACILITY 374
#define LAT$_CMDBUFOVFLW 24543244
#define LAT$_CANTBIND 24543250
#define LAT$_IVCMD 24543258
#define LAT$_IVDEV 24543266
#define LAT$_IVQUAL 24543274
#define LAT$_MAXSERV 24543282
#define LAT$_NOTERMS 24543290
#define LAT$_NONODE 24543298
#define LAT$_NOTLOADED 24543306
#define LAT$_NOTINITED 24543314
#define LAT$_NOTSTARTED 24543322
#define LAT$_NOTSTOPPED 24543330
#define LAT$_NOTSET 24543338
#define LAT$_NOHISTORY 24543346
#define LAT$_NOSTARTHIST 24543354
#define LAT$_NOSUCHSERV 24543362
#define LAT$_NOTFROMLAT 24543370
#define LAT$_INTERNAL 24543378
#define LAT$_SERVEXISTS 24543386
#define LAT$_LOCKED 24543394
#define LAT$_UNDEFLINK 24543402
#define LAT$_LINKEXISTS 24543410
#define LAT$_LINKACTIVE 24543418
#define LAT$_MAXLINKS 24543426
#define LAT$_NOLINK 24543432
#define LAT$_STARTED 24543443
#define LAT$_STOPPED 24543451
#define LAT$_SET 24543459
#define LAT$_NOSERVERS 24543467
/*                                                                          */
/* NEW MESSAGES ARE ADDED ONLY IN THE SECTIONS BELOW                        */
/*                                                                          */
#define LAT$_ALREADYDEF 24544835
#define LAT$_NOTDEF 24544843
#define LAT$_NOCONTEXT 24544851
#define LAT$_RELNOTES 24544859
#define LAT$_ACPINIT 24544867
#define LAT$_ACPSTATS 24544875
#define LAT$_FAILOVER 24544883
#define LAT$_DISCONNECTED 24544891
#define LAT$_LOSTMSG 24544899
#define LAT$_PWDPROT 24544907
#define LAT$_OUTOFRANGE 24544915
#define LAT$_END 24544923
#define LAT$_CTRLY 24544931
#define LAT$_PWDPROMPT 24544939
#define LAT$_TODISCON 24544947
#define LAT$_CONTROLC 24544955
#define LAT$_NEWLINK 24544963           /* CREATE /LOG MESSAGES             */
#define LAT$_NEWPORT 24544971
#define LAT$_NEWSERVICE 24544979
#define LAT$_NEWLOGNAME 24544987
#define LAT$_DELLINK 24544995           /* DELETE /LOG MESSAGES             */
#define LAT$_DELPORT 24545003
#define LAT$_DELSERVICE 24545011
#define LAT$_MODLINK 24545019           /* SET /LOG MESSAGES                */
#define LAT$_MODPORT 24545027
#define LAT$_MODSERVICE 24545035
#define LAT$_MODNODE 24545043
#define LAT$_ZEROLINK 24545051          /* ZERO /LOG MESSAGES               */
#define LAT$_ZEROPORT 24545059
#define LAT$_ZEROSERVICE 24545067
#define LAT$_ZERONODE 24545075
#define LAT$_DEFINEKEY 24545083
#define LAT$_LOGSTOPPED 24546434
#define LAT$_CONTEXTINIT 24546442
#define LAT$_ILLSTRUCT 24546450
#define LAT$_CONSISTANCY 24546458
#define LAT$_NOCNCT 24546466
#define LAT$_BADGETJPI 24546474
#define LAT$_BRDCSTMSG 24546482
#define LAT$_BRDCSTOUT 24546490
#define LAT$_CANTATTACH 24546498
#define LAT$_CANTSPAWN 24546506
#define LAT$_CANTCOPYSTR 24546514
#define LAT$_ERRCREKBD 24546522
#define LAT$_ERRVIRDPY 24546530
#define LAT$_NOGCDAVAIL 24546538
#define LAT$_NOIDBAVAIL 24546546
#define LAT$_NOITMLST 24546554
#define LAT$_NOREQUEST 24546562
#define LAT$_DRVERROR 24546570
#define LAT$_INVCMD 24548032
#define LAT$_CMDERROR 24548040
#define LAT$_REINITERR 24548048
#define LAT$_LNKOBS 24548056
#define LAT$_CNTRSOBS 24548064
#define LAT$_SRVRSOBS 24548072
#define LAT$_PRTOBS 24548080
#define LAT$_CONNECTED 24549633
/*                                                                          */
/* LAT CONNECTION REJECTION REASON CODES. THESE MESSAGES CORRESPOND TO      */
/* THE REJECTION REASON CODES GENERATED BY LTDRIVER WHEN A CONNECTION       */
/* REQUEST IS ABORTED (IE. FAILS). THE REASON CODE IS RETURNED IN PART      */
/* OF THE IOSB AND MAY BE TRANSLATED TO THE CORRESPONDING CODE BELOW        */
/* WITH A SIMPLE TRANSLATION VECTOR.                                        */
/*                                                                          */
#define LAT$_LRJUNKNOWN 24551236
#define LAT$_LRJSHUTDOWN 24551244
#define LAT$_LRJRESOURCE 24551252
#define LAT$_LRJINUSE 24551260
#define LAT$_LRJNOSERVICE 24551268
#define LAT$_LRJDISABLE 24551276
#define LAT$_LRJNOTOFFERED 24551284
#define LAT$_LRJNAMEUNKNOWN 24551292
#define LAT$_LRJACCESSREJECT 24551300
#define LAT$_LRJACCESSDENIED 24551308
#define LAT$_LRJCORRUPT 24551316
#define LAT$_LRJNOTSUPPORT 24551324
#define LAT$_LRJNOSTART 24551332
#define LAT$_LRJDELETED 24551340
#define LAT$_LRJILLEGAL 24551348
#define LAT$_LRJUSERDIS 24551356
#define LAT$_LRJIVPASSWORD 24551364
#define LAT$_CONTIMEOUT 24551372
#define LAT$_CONAPPLICATION 24551380
#define LAT$_LICREQUIRED 24551388
#define LAT$_UNREACHABLE 24551396
#define LAT$_NOSUCHNODE 24551404
#define LAT$_NOSRVC 24551412
#define LAT$_PWDSYNTAX 24551420
#define LAT$_SERUNAV 24551428
#define LAT$_SESLIM 24551436
#define LAT$_SRVCNODE 24551444
#define LAT$_VCLIM 24551452
#define LAT$_NODUNAV 24551460
#define LAT$_INITERR 24551468
#define LAT$_OUTERROR 24551476
#define LAT$_AUTHFAIL 24551484
#define LAT$_CONFAIL 24551492
#define LAT$_CONLOST 24551500
#define LAT$_INSRES 24551508
#define LAT$_NOMASTER 24551516
#define LAT$_TIMEOUT 24551524
#define LAT$_PROTERR 24551532
#define LAT$_SRVDIS 24551540
#define LAT$_NOACP 24551548
#define LAT$_NOSLOT 24551556
#define LAT$_BADCIRC 24551564
#define LAT$_VCSESLIM 24551572
#define LAT$_BADLINK 24551580
#define LAT$_NOTATERM 24551588
#define LAT$_NOCIB 24551596
#define LAT$_INSRESOURCE 24551604
#define LAT$_CIBLIM 24551612
#define LAT$_PROTVIO 24551620
#define LAT$_NOROOM 24551628
#define LAT$_LISTTOOSHORT 24551636
#define LAT$_LATSTOPPING 24551644
#define LAT$_NOTWITHCONN 24551652
#define LAT$_ENTNOTFOU 24551660
#define LAT$_NOMORENODS 24551668
#define LAT$_NOMORESVCS 24551676
#define LAT$_NOSELF 24551684
#define LAT$_NOTOFFERED 24551692
#define LAT$_ACPNOCTL 24551700
#define LAT$_ACPNOKSTK 24551708
#define LAT$_ACPNOVIRT 24551716
#define LAT$_NODLIMIT 24551724
#define LAT$_VERMISMATCH 24551732
#define LAT$_DRIVERNOTSHUT 24551740
#define LAT$_CONFQUAVAL 24551748
