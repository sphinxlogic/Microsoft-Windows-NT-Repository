
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/resources.c,v 1.7 1993/02/04 18:22:22 ricks Exp $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988-1993, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * resources.c: routines for handling resource management
 */

#include "copyright.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include "xrn.h"
#include "utils.h"
#ifndef VMS
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#else
#include <decw$include/Intrinsic.h>
#include <decw$include/StringDefs.h>
#include <decw$include/Shell.h>
#endif
#include "avl.h"
#include "xthelper.h"
#include "mesg.h"
#include "resources.h"
#include "error_hnds.h"
#include "news.h"
#include "internals.h"
#ifdef VMS
#define index strchr
#endif /* VMS */

#define fixupString(field)					\
    if (app_resources.field && *app_resources.field == '\0')	\
	app_resources.field = NULL;
/*
 * resources and command list
 */

/* extra name and class specifications */
#define XtNaddBindings		"addBindings"
#define XtCAddBindings		"AddBindings"
#define XtNaddButtonList	"addButtonList"
#define XtCAddButtonList	"AddButtonList"
#define XtNaddPopupList		"addPopupList"
#define XtCAddPopupList		"AddPopupList"
#define XtNallBindings		"allBindings"
#define XtCAllBindings		"AllBindings"
#define XtNallButtonList	"allButtonList"
#define XtCAllButtonList	"AllButtonList"
#define XtNallPopupList		"allPopupList"
#define XtCAllPopupList		"AllPopupList"
#define XtNartBindings		"artBindings"
#define XtCArtBindings		"ArtBindings"
#define XtNartButtonList	"artButtonList"
#define XtCArtButtonList	"ArtButtonList"
#define XtNartLayout		"artLayout"
#define XtCArtLayout		"ArtLayout"
#define XtNartPopupList		"artPopupList"
#define XtCArtPopupList		"ArtPopupList"
#define XtNartSpecButtonList	"artSpecButtonList"
#define XtCArtSpecButtonList	"ArtSpecButtonList"
#define XtNauthorFullName	"authorFullName"
#define XtCAuthorFullName	"AuthorFullName"
#define XtNauthorLength		"authorLength"
#define XtCAuthorLength		"AuthorLength"
#define XtNautoRead		"autoRead"
#define XtCAutoRead		"AutoRead"
#define XtNautoWrap		"autoWrap"
#define XtCAutoWrap		"AutoWrap"
#define XtNbusyIcon		"busyIcon"
#define XtCBusyIcon		"BusyIcon"
#define XtNbusyIconName		"busyIconName"
#define XtCBusyIconName		"BusyIconName"
#define XtNbusyIconPixmap	"busyIconPixmap"
#define XtCBusyIconPixmap	"BusyIconPixmap"
#define XtNcalvin		"calvin"
#define XtCCalvin		"Calvin"
#define XtNcancelCount		"cancelCount"
#define XtCCancelCount		"CancelCount"
#define XtNcc			"cc"
#define XtCCc			"Cc"
#define XtNccForward		"ccForward"
#define XtCCcForward		"CcForward"
#define XtNcompBreakLength	"compBreakLength"
#define XtCCompBreakLength	"CompBreakLength"
#define XtNcompLayout		"compLayout"
#define XtCCompLayout		"CompLayout"
#define XtNcompLineLength	"compLineLength"
#define XtCCompLineLength	"CompLineLength"
#define XtNcompRows		"compRows"
#define XtCCompRows		"CompRows"
#define XtNconfirm		"confirm"
#define XtCConfirm		"Confirm"
#define XtNdeadLetters		"deadLetters"
#define XtCDeadLetters		"DeadLetters"
#define XtCDebug       		"Debug"
#define XtNdefaultLines		"defaultLines"
#define XtCDefaultLines		"DefaultLines"
#define XtNdelayedPrint		"delayedPrint"
#define XtCDelayedPrint		"DelayedPrint"
#define XtNdisplayLineCount	"displayLineCount"
#define XtCDisplayLineCount	"DisplayLineCount"
#ifdef REALLY_USE_LOCALTIME
#define XtNdisplayLocalTime	"displayLocalTime"
#define XtCDisplayLocalTime	"DisplayLocalTime"
#endif
#define XtNdistribution		"distribution"
#define XtCDistribution		"Distribution"
#define XtNdumpCore		"dumpCore"
#define XtNeditorCommand 	"editorCommand"
#define XtCEditorCommand	"EditorCommand"
#define XtNexecutableSignatures "executableSignatures"
#define XtCExecutableSignatures "ExecutableSignatures"
#define XtNextraMailHeaders	"extraMailHeaders"
#define XtCExtraMailHeaders	"ExtraMailHeaders"
#define XtNhostName		"hostName"
#define XtCHostName		"HostName"
#define XtNiconGeometry		"iconGeometry"
#define XtCIconGeometry		"IconGeometry"
#define XtNignoreNewsgroups	"ignoreNewsgroups"
#define XtCIgnoreNewsgroups	"IgnoreNewsgroups"
#define XtNincludeCommand	"includeCommand"
#define XtCIncludeCommand	"IncludeCommand"
#define XtNincludeHeader	"includeHeader"
#define XtCIncludeHeader	"IncludeHeader"
#define XtNincludePrefix	"includePrefix"
#define XtCIncludePrefix	"IncludePrefix"
#define XtNincludeSep		"includeSep"
#define XtCIncludeSep		"IncludeSep"
#define XtNindexLayout		"indexLayout"
#define XtCIndexLayout		"IndexLayout"
#define XtNinfo			"info"
#define XtCInfo			"Info"
#define XtNkillFiles		"killFiles"
#define XtCKillFiles		"KillFiles"
#define XtNlayout		"layout"
#define XtCLayout		"Layout"
#define XtNleaveHeaders		"leaveHeaders"
#define XtCLeaveHeaders		"LeaveHeaders"
#define XtNlocalSignatures	"localSignatures"
#define XtCLocalSignatures	"LocalSignatures"
#define XtNlockFile		"lockFile"
#define XtCLockFile		"LockFile"
#ifndef VMS
#define XtNmailer		"mailer"
#define XtCMailer		"Mailer"
#endif
#define XtNmaxLines		"maxLines"
#define XtCMaxLines		"MaxLines"
#define XtNmhPath		"mhPath"
#define XtCMhPath		"MhPath"
#define XtNminLines		"minLines"
#define XtCMinLines		"MinLines"
#define XtNnewsrcFile		"newsrcFile"
#define XtCNewsrcFile		"NewsrcFile"
#define XtNnextReadsFirst	"nextReadsFirst"
#define XtCNextReadsFirst	"NextReadsFirst"
#define XtNngBindings		"ngBindings"
#define XtCNgBindings		"NgBindings"
#define XtNngButtonList		"ngButtonList"
#define XtCNgButtonList		"NgButtonList"
#define XtNngPopupList		"ngPopupList"
#define XtCNgPopupList		"NgPopupList"
#define XtNnntpServer		"nntpServer"
#define XtCNntpServer		"NntpServer"
#define XtNonlyShow		"onlyShow"
#define XtCOnlyShow		"OnlyShow"
#define XtNorganization		"organization"
#define XtCOrganization		"Organization"
#define XtNpageArticles		"pageArticles"
#define XtCPageArticles		"PageArticles"
#ifdef VMS
#define XtNpersonalName		"personalName"
#define XtCPersonalName		"personalName"
#endif
#define XtNpointerBackground	"pointerBackground"
#define XtCPointerBackground	"PointerBackground"
#define XtNpointerForeground	"pointerForeground"
#define XtCPointerForeground	"PointerForeground"
#define XtNpopupButton		"popupButton"
#define XtCPopupButton		"PopupButton"
#define XtNprintCommand		"printCommand"
#define XtCPrintCommand		"PrintCommand"
#define XtNreplyPath		"replyPath"
#define XtCReplyPath		"ReplyPath"
#define XtNreplyTo		"replyTo"
#define XtCReplyTo		"ReplyTo"
#define XtNrescanTime		"rescanTime"
#define XtCRescanTime		"RescanTime"
#define XtNresetSave		"resetSave"
#define XtCResetSave		"ResetSave"
#define XtNretainKilled		"retainKilled"
#define XtCRetainKilled		"RetainKilled"
#define XtNretryLimit		"retryLimit"
#define XtCRetryLimit		"RetryLimit"
#define XtNretryPopupCount	"retryPopupCount"
#define XtCRetryPopupCount	"RetryPopupCount"
#define XtNretryPopupTimeout	"retryPopupTimeout"
#define XtCRetryPopupTimeout	"RetryPopupTimeout"
#define XtNretryPause		"retryPause"
#define XtCRetryPause		"RetryPause"
#define XtNsaveDir		"saveDir"
#define XtCSaveDir		"SaveDir"
#define XtNsaveMode		"saveMode"
#define XtCSaveMode		"SaveMode"
#define XtNsaveNewsrcFile	"saveNewsrcFile"
#define XtCSaveNewsrcFile	"SaveNewsrcFile"
#define XtNsavePostings		"savePostings"
#define XtCSavePostings		"SavePostings"
#define XtNsaveString		"saveString"
#define XtCSaveString		"SaveString"
#define XtNsignatureFile	"signatureFile"
#define XtCSignatureFile	"SignatureFile"
#define XtNsignatureNotify	"signatureNotify"
#define XtCSignatureNotify	"SignatureNotify"
#define XtNsortedSubjects	"sortedSubjects"
#define XtCSortedSubjects	"SortedSubjects"
#define XtRSortedSubjects	"SortedSubjects"
#define XtNstripHeaders		"stripHeaders"
#define XtCStripHeaders		"StripHeaders"
#define XtNsubjectLength	"subjectLength"
#define XtCSubjectLength	"SubjectLength"
#define XtNsubjectRead		"subjectRead"
#define XtCSubjectRead		"SubjectRead"
#define XtNtextLines		"textLines"
#define XtCTextLines		"TextLines"
#define XtNtmpDir		"tmpDir"
#define XtCTmpDir		"TmpDir"
#define XtNtopLines		"topLines"
#define XtCTopLines		"TopLines"
#define XtNtwoWindows		"twoWindows"
#define XtCTwoWindows		"TwoWindows"
#define XtNunsubNewGroups	"unsubNewGroups"
#define XtCUnsubNewGroups	"UnsubNewGroups"
#define XtNunreadIconName	"unreadIconName"
#define XtCUnreadIconName	"UnreadIconName"
#define XtNunreadIconPixmap	"unreadIconPixmap"
#define XtCUnreadIconPixmap	"UnreadIconPixmap"
#define XtNupdateNewsrc		"updateNewsrc"
#define XtCUpdateNewsrc		"UpdateNewsrc"
#define XtNuseGadgets		"useGadgets"
#define XtCUseGadgets		"UseGadgets"
#define XtNverboseKill		"verboseKill"
#define XtCVerboseKill		"VerboseKill"
#define XtNversion		"version"
#define XtCVersion		"Version"
#define XtNwatchProgress	"watchProgress"
#define XtCWatchProgress	"WatchProgress"
#ifdef WATCH
#define XtNwatchUnread		"watchUnread"
#define XtCWatchUnread		"WatchUnread"
#endif
#define XtNxrefMarkRead		"xrefMarkRead"
#define XtCXrefMarkRead		"XrefMarkRead"

static Boolean defaultFalse = False;
static Boolean defaultTrue  = True;

#ifndef XtRBitmap
#define XtRBitmap "Bitmap"
#endif

app_resourceRec app_resources;
char title[LABEL_SIZE];
static char unreadTitle[] = UNREADTITLE;

static char defaultNgButtons[] =
"ngQuit,ngRead,ngSubscribe,ngUnsub,ngCatchUp,ngGoto,ngRescan,ngCheckPoint,ngSelect,ngMove,ngPost";

static char defaultNgPopups[] =
"ngQuit,ngRead,ngSubscribe,ngUnsub,ngCatchUp,ngGoto,ngRescan,ngCheckPoint,ngPost";

static char defaultArtButtons[] =
"artQuit,artNext,artPrev,artNextUnread,artSubNext,artSubPrev,artLast,artNextGroup,artCatchUp,artMarkRead,artMarkUnread";

static char defaultArtPopups[] =
"artQuit,artNext,artPrev,artNextUnread,artSubNext,artSubPrev,artLast,artNextGroup,artSave,artReply,artForward,artFollowup,artMarkUnread";

/*
 * resources 'xrn' needs to get, rather than ones that the individual
 * widgets will handle
 */
static XtResource resources[] = {

   {XtNaddBindings, XtCAddBindings, XtRString, sizeof(char *),
      XtOffset(app_res,addBindings), XtRString, (caddr_t) NULL},

   {XtNaddButtonList, XtCAddButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,addButtonList), XtRString, (caddr_t) NULL},

   {XtNaddPopupList, XtCAddPopupList, XtRString, sizeof(char *),
      XtOffset(app_res,addPopupList), XtRString, (caddr_t) NULL},

   {XtNallBindings, XtCAllBindings, XtRString, sizeof(char *),
      XtOffset(app_res,allBindings), XtRString, (caddr_t) NULL},

   {XtNallButtonList, XtCAllButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,allButtonList), XtRString, (caddr_t) NULL},

   {XtNallPopupList, XtCAllPopupList, XtRString, sizeof(char *),
      XtOffset(app_res,allPopupList), XtRString, (caddr_t) NULL},

   {XtNartBindings, XtCArtBindings, XtRString, sizeof(char *),
      XtOffset(app_res,artBindings), XtRString, (caddr_t) NULL},

   {XtNartButtonList, XtCArtButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,artButtonList), XtRString, (caddr_t) defaultArtButtons},

   {XtNartPopupList, XtCArtPopupList, XtRString, sizeof(char *),
      XtOffset(app_res,artPopupList), XtRString, (caddr_t) defaultArtPopups},

   {XtNartLayout, XtCArtLayout, XtRString,  sizeof(char *),
      XtOffset(app_res,artLayout), XtRString, (caddr_t) NULL},
   
   {XtNartSpecButtonList, XtCArtSpecButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,artSpecButtonList), XtRString, (caddr_t) NULL},

   {XtNauthorFullName, XtCAuthorFullName, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,authorFullName), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNauthorLength, XtCAuthorLength, XtRInt, sizeof(int),
      XtOffset(app_res,authorLength), XtRImmediate, (caddr_t) 0},

   {XtNautoRead, XtCAutoRead, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,autoRead), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNautoWrap, XtCAutoWrap, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,autoWrap), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNbusyIcon, XtCBusyIcon, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,busyIcon), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNbusyIconName,  XtCBusyIconName,  XtRString,  sizeof(char *),
      XtOffset(app_res,busyIconName), XtRString, (caddr_t) NULL},

   {XtNbusyIconPixmap,  XtCBusyIconPixmap,  XtRBitmap,  sizeof(char *),
      XtOffset(app_res,busyIconPixmap), XtRPixmap, (caddr_t) NULL},

   {XtNcalvin, XtCCalvin, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,calvin), XtRBoolean, (caddr_t) &defaultFalse},
   
   {XtNcancelCount, XtCCancelCount, XtRInt, sizeof(int),
      XtOffset(app_res,cancelCount), XtRString, (caddr_t) CANCELCOUNT},

   {XtNcc, XtCCc, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,cc), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNccForward, XtCCcForward, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,ccForward), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNcompBreakLength, XtCCompBreakLength, XtRInt, sizeof(int),
      XtOffset(app_res,breakLength), XtRString, (caddr_t) BREAKLENGTH},

   {XtNcompLayout, XtCCompLayout, XtRString,  sizeof(char *),
      XtOffset(app_res,compLayout), XtRString, (caddr_t) NULL},
   
   {XtNcompLineLength, XtCCompLineLength, XtRInt, sizeof(int),
      XtOffset(app_res,lineLength), XtRString, (caddr_t) LINELENGTH},
   
   {XtNcompRows, XtCCompRows, XtRInt, sizeof(int),
      XtOffset(app_res,composeRows), XtRImmediate, (caddr_t) 30},
   
   {XtNconfirm, XtCConfirm, XtRString, sizeof(char *),
      XtOffset(app_res,confirm), XtRString, (caddr_t) NULL},

   {XtNdeadLetters, XtCDeadLetters, XtRString, sizeof(char *),
      XtOffset(app_res,deadLetters), XtRString, (caddr_t) DEADLETTER},

   {XtNdumpCore, XtCDebug, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,dumpCore), XtRBoolean, (caddr_t)
#ifdef DUMPCORE
							&defaultTrue},
#else
							&defaultFalse},
#endif

   {XtNdefaultLines, XtCDefaultLines, XtRInt, sizeof(int),
      XtOffset(app_res,defaultLines), XtRString, (caddr_t) MINLINES},

   {XtNdelayedPrint, XtCDelayedPrint, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res, delayedPrint), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNdisplayLineCount, XtCDisplayLineCount, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,displayLineCount), XtRBoolean, (caddr_t) &defaultFalse},

#ifdef REALLY_USE_LOCALTIME
   {XtNdisplayLocalTime, XtCDisplayLocalTime, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,displayLocalTime), XtRBoolean, (caddr_t) &defaultTrue},
#endif

   {XtNdistribution, XtCDistribution, XtRString, sizeof(char *),
      XtOffset(app_res,distribution), XtRString, (caddr_t) NULL},
   
   {XtNeditorCommand, XtCEditorCommand, XtRString, sizeof(char *),
      XtOffset(app_res,editorCommand), XtRString, (caddr_t) NULL},

   {XtNexecutableSignatures, XtCExecutableSignatures, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,executableSignatures), XtRBoolean, (caddr_t) &defaultFalse},
   
   {XtNextraMailHeaders, XtCExtraMailHeaders, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,extraMailHeaders), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNhostName, XtCHostName, XtRString, sizeof(char *),
      XtOffset(app_res,hostName), XtRString, (caddr_t) NULL},

   {XtNiconGeometry,  XtCIconGeometry,  XtRString,  sizeof(char *),
      XtOffset(app_res,iconGeometry), XtRString, (caddr_t) NULL},
   
   {XtNiconPixmap,  XtCIconPixmap,  XtRBitmap,  sizeof(char *),
      XtOffset(app_res,iconPixmap), XtRPixmap, (caddr_t) NULL},

   {XtNiconName,  XtCIconName,  XtRString,  sizeof(char *),
      XtOffset(app_res,iconName), XtRString,
#ifdef SHORT_ICONNAME
#ifndef MOTIF
	(caddr_t) "dxrn"},
#else
	(caddr_t) "mxrn"},
#endif
#else
	(caddr_t) title},
#endif

   {XtNignoreNewsgroups, XtCIgnoreNewsgroups, XtRString, sizeof(char *),
      XtOffset(app_res,ignoreNewsgroups), XtRString, (caddr_t) NULL},
   
   {XtNincludeCommand, XtCIncludeCommand, XtRString, sizeof(char *),
      XtOffset(app_res,includeCommand), XtRString, (caddr_t) NULL},

   {XtNincludeHeader, XtCIncludeHeader, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,includeHeader), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNincludePrefix, XtCIncludePrefix, XtRString, sizeof(char *),
     XtOffset(app_res,includePrefix), XtRString, (caddr_t) INCLUDEPREFIX},

   {XtNincludeSep, XtCIncludeSep, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,includeSep), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNindexLayout, XtCIndexLayout, XtRString,  sizeof(char *),
      XtOffset(app_res,indexLayout), XtRString, (caddr_t) NULL},
   
   {XtNinfo, XtCInfo, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,info), XtRBoolean, (caddr_t) &defaultTrue},
   
   {XtNkillFiles, XtCKillFiles, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,killFiles), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNlayout, XtCLayout, XtRString,  sizeof(char *),
      XtOffset(app_res,mainLayout), XtRString, (caddr_t) NULL},
   
   {XtNleaveHeaders, XtCLeaveHeaders, XtRString, sizeof(char *),
      XtOffset(app_res,leaveHeaders), XtRString, (caddr_t) NULL},
   
   {XtNlocalSignatures, XtCLocalSignatures, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,localSignatures), XtRBoolean, (caddr_t) &defaultFalse},
   
   {XtNlockFile, XtCLockFile, XtRString, sizeof(char *),
#ifndef VMS
      XtOffset(app_res,lockFile), XtRString, (caddr_t) "~/.xrnlock"},
#else
      XtOffset(app_res,lockFile), XtRString, (caddr_t) "sys$login:xrn.lock"},
#endif

#ifndef VMS
   {XtNmailer, XtCMailer, XtRString, sizeof(char *),
      XtOffset(app_res,mailer), XtRString, (caddr_t) SENDMAIL},
#endif

   {XtNmaxLines, XtCMaxLines, XtRInt, sizeof(int),
      XtOffset(app_res,maxLines), XtRImmediate, (caddr_t) -1},
   
   {XtNmhPath, XtCMhPath, XtRString, sizeof(String),
      XtOffset(app_res,mhPath), XtRString, (caddr_t) NULL},

   {XtNminLines, XtCMinLines, XtRInt, sizeof(int),
      XtOffset(app_res,minLines), XtRImmediate, (caddr_t) -1},

   {XtNnewsrcFile, XtCNewsrcFile, XtRString, sizeof(char *),
      XtOffset(app_res,newsrcFile), XtRString, (caddr_t) NEWSRCFILE},
   
   {XtNngBindings, XtCNgBindings, XtRString, sizeof(char *),
      XtOffset(app_res,ngBindings), XtRString, (caddr_t) NULL},

   {XtNngButtonList, XtCNgButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,ngButtonList), XtRString, (caddr_t) defaultNgButtons},

   {XtNngPopupList, XtCNgPopupList, XtRString, sizeof(char *),
      XtOffset(app_res,ngPopupList), XtRString, (caddr_t) defaultNgPopups},

   {XtNnntpServer, XtCNntpServer, XtRString, sizeof(char *),
      XtOffset(app_res,nntpServer), XtRString, (caddr_t) NULL},
   
   {XtNonlyShow, XtCOnlyShow, XtRInt, sizeof(int),
      XtOffset(app_res,onlyShow), XtRString, (caddr_t) ONLYSHOW},

   {XtNorganization, XtCOrganization, XtRString, sizeof(char *),
      XtOffset(app_res,organization), XtRString, (caddr_t) NULL},
   
   {XtNpageArticles, XtCPageArticles, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,pageArticles), XtRBoolean, (caddr_t) &defaultTrue},

#ifdef VMS
   {XtNpersonalName, XtCPersonalName, XtRString, sizeof(char *),
      XtOffset(app_res,personalName), XtRImmediate, (caddr_t) NULL},
#endif   

#ifdef MOTIF
   {XtNpopupButton,  XtCPopupButton,  XtRInt,	 sizeof(int),
      XtOffset(app_res, popupButton), XtRImmediate, (caddr_t) 3},
#else
   {XtNpopupButton,  XtCPopupButton,  XtRInt,	 sizeof(int),
      XtOffset(app_res, popupButton), XtRImmediate, (caddr_t) 2},
#endif

   {XtNpointerForeground, XtCPointerForeground, XtRPixel, sizeof(Pixel),
      XtOffset(app_res,pointer_foreground), XtRString, "white"},

   {XtNpointerBackground, XtCPointerBackground, XtRPixel, sizeof(Pixel),
      XtOffset(app_res,pointer_background), XtRString, "black"},

   {XtNprintCommand, XtCPrintCommand, XtRString, sizeof(char *),
      XtOffset(app_res,printCommand), XtRString, (caddr_t) PRINTCOMMAND},

   {XtNreplyPath, XtCReplyPath, XtRString, sizeof(char *),
      XtOffset(app_res,replyPath), XtRString, (caddr_t) NULL},

   {XtNreplyTo, XtCReplyTo, XtRString, sizeof(char *),
      XtOffset(app_res,replyTo), XtRString, (caddr_t) NULL},
   
   {XtNresetSave, XtCResetSave, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,resetSave), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNrescanTime, XtCRescanTime, XtRInt, sizeof(int),
      XtOffset(app_res,rescanTime), XtRString, (caddr_t) RESCAN_TIME},

   {XtNretainKilled, XtCRetainKilled, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,retainKilledArticles),
     XtRBoolean, (caddr_t) &defaultFalse},

   {XtNretryLimit, XtCRetryLimit, XtRInt, sizeof(int),
      XtOffset(app_res,retryLimit), XtRImmediate, (caddr_t) 10},

   {XtNretryPopupCount, XtCRetryPopupCount, XtRInt, sizeof(int),
      XtOffset(app_res,retryPopupCount), XtRImmediate, (caddr_t) 1},

   {XtNretryPopupTimeout, XtCRetryPopupTimeout, XtRInt, sizeof(int),
      XtOffset(app_res,retryPopupTimeout), XtRImmediate, (caddr_t) 60},

   {XtNretryPause, XtCRetryPause, XtRInt, sizeof(int),
      XtOffset(app_res,retryPause), XtRImmediate, (caddr_t) 60},

   {XtNsaveDir,  XtCSaveDir,  XtRString, sizeof(char *),
      XtOffset(app_res,saveDir), XtRString, (caddr_t) SAVEDIR},
   
   {XtNsaveNewsrcFile, XtCSaveNewsrcFile, XtRString, sizeof(char *),
      XtOffset(app_res,saveNewsrcFile), XtRString, (caddr_t) SAVENEWSRCFILE},
   
   {XtNsaveMode, XtCSaveMode, XtRString, sizeof(char *),
      XtOffset(app_res,strSaveMode), XtRString, (caddr_t) SAVEMODE},
   
   {XtNsavePostings, XtCSavePostings, XtRString, sizeof(char *),
      XtOffset(app_res,savePostings), XtRString, (caddr_t) SAVEPOSTINGS},
   
   {XtNsaveString, XtCSaveString, XtRString, sizeof(char *),
      XtOffset(app_res,saveString), XtRString, (caddr_t) ""},

   {XtNsignatureFile, XtCSignatureFile, XtRString, sizeof(char *),
      XtOffset(app_res,signatureFile), XtRString, (caddr_t) SIGNATUREFILE},
   
   {XtNsignatureNotify, XtCSignatureNotify, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,signatureNotify), XtRBoolean, (caddr_t) &defaultFalse},
   
   {XtNsortedSubjects, XtCSortedSubjects, XtRSortedSubjects, sizeof(int),
     XtOffset(app_res,defSortedSubjects), XtRString, (caddr_t) "off"},

   {XtNstripHeaders, XtCStripHeaders, XtRString, sizeof(char *),
      XtOffset(app_res,stripHeaders), XtRString, (caddr_t) NULL},
   
   {XtNsubjectLength, XtCSubjectLength, XtRInt, sizeof(int),
      XtOffset(app_res,subjectLength), XtRImmediate, (caddr_t) 0},
   
   {XtNsubjectRead, XtCSubjectRead, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,subjectRead), XtRBoolean, (caddr_t) &defaultFalse},
   
   {XtNtextLines, XtCTextLines, XtRInt, sizeof(int),
      XtOffset(app_res,textLines), XtRString, (caddr_t) TEXTLINES},
   
   {XtNtitle,  XtCTitle,  XtRString,  sizeof(char *),
      XtOffset(app_res,title), XtRString, (caddr_t) title},
   
   {XtNtmpDir, XtCTmpDir, XtRString, sizeof(char *),
      XtOffset(app_res,tmpDir), XtRString, (caddr_t) NULL},

   {XtNtopLines, XtCTopLines, XtRInt, sizeof(int),
      XtOffset(app_res,topLines), XtRString, (caddr_t) TOPLINES},

   {XtNtwoWindows, XtCTwoWindows, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,twoWindows), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNunreadIconName,  XtCUnreadIconName,  XtRString,  sizeof(char *),
      XtOffset(app_res,unreadIconName), XtRString,
#ifdef SHORT_ICONNAME
#ifndef MOTIF
	(caddr_t) "dxrn (New News)"},
#else
	(caddr_t) "mxrn (New News)"},
#endif
#else
	(caddr_t) unreadTitle},
#endif
   
   {XtNunreadIconPixmap,  XtCUnreadIconPixmap,  XtRBitmap,  sizeof(char *),
      XtOffset(app_res,unreadIconPixmap), XtRPixmap, (caddr_t) NULL},

   {XtNunsubNewGroups, XtCUnsubNewGroups, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,unsubNewGroups), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNupdateNewsrc, XtCUpdateNewsrc, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,updateNewsrc), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNuseGadgets, XtCUseGadgets, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,useGadgets), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNverboseKill, XtCVerboseKill, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,verboseKill), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNversion, XtCVersion, XtRString, sizeof(char *),
     XtOffset(app_res,version), XtRString, (caddr_t) NULL},

   {XtNwatchProgress, XtCWatchProgress, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,watchProgress), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNnextReadsFirst, XtCNextReadsFirst, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,nextReadsFirst), XtRBoolean, (caddr_t) &defaultFalse},

#ifdef WATCH
   {XtNwatchUnread, XtCWatchUnread, XtRString, sizeof(char *),
      XtOffset(app_res,watchList), XtRString, (caddr_t) NULL},
#endif

   {XtNxrefMarkRead, XtCXrefMarkRead, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,xrefMarkRead), XtRBoolean, (caddr_t) &defaultFalse},
};

/*
 * allowed command line options
 */

static char don[] = "on";
static char doff[]  = "off";

#ifndef VMS
static XrmOptionDescRec optionList[] = {
{"-addBindings",	XtNaddBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-addButtonList",	XtNaddButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-addPopupList",	XtNaddPopupList,	XrmoptionSepArg,(caddr_t) NULL},
{"-allBindings",	XtNallBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-allButtonList",	XtNallButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-allPopupList",	XtNallPopupList,	XrmoptionSepArg,(caddr_t) NULL},
{"-artBindings",	XtNartBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-artButtonList",	XtNartButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-artPopupList",	XtNartPopupList,	XrmoptionSepArg,(caddr_t) NULL},
{"-artSpecButtonList",	XtNartSpecButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-authorFullName",	XtNauthorFullName,	XrmoptionNoArg, (caddr_t) doff},
{"+authorFullName",	XtNauthorFullName,	XrmoptionNoArg, (caddr_t) don},
{"-authorLength",	XtNauthorLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-autoRead",		XtNautoRead,		XrmoptionNoArg, (caddr_t) doff},
{"+autoRead",		XtNautoRead,		XrmoptionNoArg, (caddr_t) don},
{"-autoWrap",		XtNautoWrap,		XrmoptionNoArg, (caddr_t) doff},
{"+autoWrap",		XtNautoWrap,		XrmoptionNoArg, (caddr_t) don},
{"-busyIcon",		XtNbusyIcon,		XrmoptionNoArg, (caddr_t) doff},
{"+busyIcon",		XtNbusyIcon,		XrmoptionNoArg, (caddr_t) don},
{"-busyIconName",	XtNbusyIconName,	XrmoptionSepArg,(caddr_t) NULL},
{"-busyIconPixmap",	XtNbusyIconPixmap,	XrmoptionSepArg,(caddr_t) NULL},
{"-calvin",		XtNcalvin,		XrmoptionNoArg, (caddr_t) don},
{"-cancelCount",	XtNcancelCount,		XrmoptionSepArg,(caddr_t) NULL},
{"-cc",			XtNcc,			XrmoptionNoArg, (caddr_t) doff},
{"+cc",			XtNcc,			XrmoptionNoArg, (caddr_t) don},
{"-ccForward",		XtNccForward,		XrmoptionNoArg, (caddr_t) doff},
{"+ccForward",		XtNccForward,		XrmoptionNoArg, (caddr_t) don},
{"-compBreakLength",	XtNcompBreakLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-compLineLength",	XtNcompLineLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-compRows",		XtNcompRows,		XrmoptionSepArg,(caddr_t) NULL},
{"-confirm",		XtNconfirm,		XrmoptionSepArg,(caddr_t) NULL},
{"-deadLetters",	XtNdeadLetters,		XrmoptionSepArg,(caddr_t) NULL},
{"-dumpCore",		XtNdumpCore,		XrmoptionNoArg, (caddr_t) doff},
{"+dumpCore",		XtNdumpCore,		XrmoptionNoArg, (caddr_t) don},
{"-defaultLines",	XtNdefaultLines,	XrmoptionSepArg,(caddr_t) NULL},
{"-delayedPrint",	XtNdelayedPrint,	XrmoptionNoArg, (caddr_t) doff},
{"+delayedPrint",	XtNdelayedPrint,	XrmoptionNoArg, (caddr_t) don},
{"-displayLineCount",	XtNdisplayLineCount,	XrmoptionNoArg, (caddr_t) doff},
{"+displayLineCount",	XtNdisplayLineCount,	XrmoptionNoArg, (caddr_t) don},
#ifdef REALLY_USE_LOCALTIME
{"-displayLocalTime",	XtNdisplayLocalTime,	XrmoptionNoArg, (caddr_t) doff},
{"+displayLocalTime",	XtNdisplayLocalTime,	XrmoptionNoArg, (caddr_t) don},
#endif
{"-distribution",	XtNdistribution,	XrmoptionSepArg,(caddr_t) NULL},
{"-editorCommand",	XtNeditorCommand,	XrmoptionSepArg,(caddr_t) NULL},
{"-executableSignatures",XtNexecutableSignatures,XrmoptionNoArg,(caddr_t) doff},
{"+executableSignatures",XtNexecutableSignatures,XrmoptionNoArg,(caddr_t) don},
{"-extraMailHeaders",	XtNextraMailHeaders,	XrmoptionNoArg, (caddr_t) doff},
{"+extraMailHeaders",	XtNextraMailHeaders,	XrmoptionNoArg, (caddr_t) don},
{"-hostName",		XtNhostName,		XrmoptionSepArg,(caddr_t) NULL},
{"-iconGeometry",	XtNiconGeometry,	XrmoptionSepArg,(caddr_t) NULL},
{"-iconName",           XtNiconName, 		XrmoptionSepArg,(caddr_t) NULL},
{"-iconPixmap",		XtNiconPixmap, 		XrmoptionSepArg,(caddr_t) NULL},
{"-ignoreNewsgroups",	XtNignoreNewsgroups,	XrmoptionSepArg,(caddr_t) NULL},
{"-includeCommand",	XtNincludeCommand,	XrmoptionSepArg,(caddr_t) NULL},
{"-includeHeader",	XtNincludeHeader,	XrmoptionNoArg, (caddr_t) doff},
{"+includeHeader",	XtNincludeHeader,	XrmoptionNoArg, (caddr_t) don},
{"-includePrefix",	XtNincludePrefix,	XrmoptionSepArg,(caddr_t) NULL},
{"-includeSep",		XtNincludeSep,		XrmoptionNoArg, (caddr_t) doff},
{"+includeSep",		XtNincludeSep,		XrmoptionNoArg, (caddr_t) don},
{"-info",		XtNinfo,		XrmoptionNoArg, (caddr_t) doff},
{"+info",		XtNinfo,		XrmoptionNoArg, (caddr_t) don},
{"-killFiles",		XtNkillFiles,		XrmoptionNoArg, (caddr_t) doff},
{"+killFiles",		XtNkillFiles,		XrmoptionNoArg, (caddr_t) don},
{"-layout",		XtNlayout,		XrmoptionSepArg,(caddr_t) NULL},
{"-leaveHeaders",	XtNleaveHeaders,	XrmoptionSepArg,(caddr_t) NULL},
{"-localSignatures",	XtNlocalSignatures,	XrmoptionNoArg, (caddr_t) doff},
{"+localSignatures",	XtNlocalSignatures,	XrmoptionNoArg, (caddr_t) don},
{"-lockFile",		XtNlockFile,		XrmoptionSepArg,(caddr_t) NULL},
{"-mailer",		XtNmailer,		XrmoptionSepArg,(caddr_t) NULL},
{"-maxLines",		XtNmaxLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-mhPath",		XtNmhPath,		XrmoptionSepArg,(caddr_t) NULL},
{"-minLines",		XtNminLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-newsrcFile",		XtNnewsrcFile,		XrmoptionSepArg,(caddr_t) NULL},
{"-ngBindings",		XtNngBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-ngButtonList",	XtNngButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-ngPopupList",	XtNngPopupList,		XrmoptionSepArg,(caddr_t) NULL},
{"-nntpServer",		XtNnntpServer,		XrmoptionSepArg,(caddr_t) NULL},
{"-onlyShow",		XtNonlyShow,		XrmoptionSepArg,(caddr_t) NULL},
{"-organization",	XtNorganization,	XrmoptionSepArg,(caddr_t) NULL},
{"-pageArticles",	XtNpageArticles,	XrmoptionNoArg, (caddr_t) doff},
{"+pageArticles",	XtNpageArticles,	XrmoptionNoArg, (caddr_t) don},
{"-popupButton",	XtNpopupButton,		XrmoptionSepArg,(caddr_t) NULL},
{"-pointerBackground",	XtNpointerBackground,	XrmoptionSepArg,(caddr_t) NULL},
{"-pointerForeground",	XtNpointerForeground,	XrmoptionSepArg,(caddr_t) NULL},
{"-printCommand",	XtNprintCommand,	XrmoptionSepArg,(caddr_t) NULL},
{"-replyPath",		XtNreplyPath,		XrmoptionSepArg,(caddr_t) NULL},
{"-replyTo",		XtNreplyTo,		XrmoptionSepArg,(caddr_t) NULL},
{"-resetSave",		XtNresetSave,		XrmoptionNoArg, (caddr_t) doff},
{"+resetSave",		XtNresetSave,		XrmoptionNoArg, (caddr_t) don},
{"-rescanTime",		XtNrescanTime,		XrmoptionSepArg,(caddr_t) NULL},
{"-retainKilled",	XtNretainKilled,	XrmoptionNoArg, (caddr_t) doff},
{"+retainKilled",	XtNretainKilled,	XrmoptionNoArg, (caddr_t) don},
{"-retryLimit",		XtNretryLimit,		XrmoptionSepArg,(caddr_t) NULL},
{"-retryPopupCount",	XtNretryPopupCount,	XrmoptionSepArg,(caddr_t) NULL},
{"-retryPopupTimeout",	XtNretryPopupTimeout,	XrmoptionSepArg,(caddr_t) NULL},
{"-retryPause",		XtNretryPause,		XrmoptionSepArg,(caddr_t) NULL},
{"-saveDir",		XtNsaveDir,		XrmoptionSepArg,(caddr_t) NULL},
{"-saveNewsrcFile",	XtNsaveNewsrcFile,	XrmoptionSepArg,(caddr_t) NULL},
{"-saveMode",		XtNsaveMode,		XrmoptionSepArg,(caddr_t) NULL},
{"-savePostings",	XtNsavePostings,	XrmoptionSepArg,(caddr_t) NULL},
{"-saveString",		XtNsaveString,		XrmoptionSepArg,(caddr_t) NULL},
{"-signatureFile",	XtNsignatureFile,	XrmoptionSepArg,(caddr_t) NULL},
{"-signatureNotify",	XtNsignatureNotify,	XrmoptionNoArg, (caddr_t) doff},
{"+signatureNotify",	XtNsignatureNotify,	XrmoptionNoArg, (caddr_t) don},
{"-sortedSubjects",	XtNsortedSubjects,	XrmoptionNoArg, (caddr_t) doff},
{"+sortedSubjects",	XtNsortedSubjects,	XrmoptionNoArg, (caddr_t) don},
{"-stripHeaders",	XtNstripHeaders,	XrmoptionSepArg,(caddr_t) NULL},
{"-subjectLength",	XtNsubjectLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-subjectRead",	XtNsubjectRead,		XrmoptionNoArg, (caddr_t) doff},
{"+subjectRead",	XtNsubjectRead,		XrmoptionNoArg, (caddr_t) don},
{"-textLines",		XtNtextLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-tmpDir",		XtNtmpDir,		XrmoptionSepArg,(caddr_t) NULL},
{"-topLines",		XtNtopLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-unreadIconName",	XtNunreadIconName,	XrmoptionSepArg,(caddr_t) NULL},
{"-unreadIconPixmap",	XtNunreadIconPixmap,	XrmoptionSepArg,(caddr_t) NULL},
{"-updateNewsrc",	XtNupdateNewsrc,	XrmoptionNoArg, (caddr_t) doff},
{"+updateNewsrc",	XtNupdateNewsrc,	XrmoptionNoArg, (caddr_t) don},
{"-useGadgets",		XtNuseGadgets,		XrmoptionNoArg, (caddr_t) doff},
{"+useGadgets",		XtNuseGadgets,		XrmoptionNoArg, (caddr_t) don},
{"-verboseKill",	XtNverboseKill,		XrmoptionNoArg, (caddr_t) doff},
{"+verboseKill",	XtNverboseKill,		XrmoptionNoArg, (caddr_t) don},
{"-watchProgress",	XtNwatchProgress,	XrmoptionNoArg, (caddr_t) doff},
{"+watchProgress",	XtNwatchProgress,	XrmoptionNoArg, (caddr_t) don},
#ifdef WATCH
{"-watchUnread",	XtNwatchUnread,		XrmoptionSepArg,(caddr_t) NULL},
#endif
{"-xrefMarkRead",	XtNxrefMarkRead,	XrmoptionNoArg, (caddr_t) doff},
{"+xrefMarkRead",	XtNxrefMarkRead,	XrmoptionNoArg, (caddr_t) don},
{"-nextReadsFirst",	XtNnextReadsFirst,	XrmoptionNoArg, (caddr_t) doff},
{"+nextReadsFirst",	XtNnextReadsFirst,	XrmoptionNoArg, (caddr_t) don},
};  
#else
static XrmOptionDescRec optionList[] = {
{"-addbindings",	XtNaddBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-addbuttonlist",	XtNaddButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-addpopuplist",	XtNaddPopupList,	XrmoptionSepArg,(caddr_t) NULL},
{"-allbindings",	XtNallBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-allbuttonlist",	XtNallButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-allpopuplist",	XtNallPopupList,	XrmoptionSepArg,(caddr_t) NULL},
{"-artbindings",	XtNartBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-artbuttonlist",	XtNartButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-artpopuplist",	XtNartPopupList,	XrmoptionSepArg,(caddr_t) NULL},
{"-artspecbuttonlist",	XtNartSpecButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-authorfullname",	XtNauthorFullName,	XrmoptionNoArg, (caddr_t) doff},
{"+authorfullname",	XtNauthorFullName,	XrmoptionNoArg, (caddr_t) don},
{"-authorlength",	XtNauthorLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-autoread",		XtNautoRead,		XrmoptionNoArg, (caddr_t) doff},
{"+autoread",		XtNautoRead,		XrmoptionNoArg, (caddr_t) don},
{"-autowrap",		XtNautoWrap,		XrmoptionNoArg, (caddr_t) doff},
{"+autowrap",		XtNautoWrap,		XrmoptionNoArg, (caddr_t) don},
{"-busyicon",		XtNbusyIcon,		XrmoptionNoArg, (caddr_t) doff},
{"+busyicon",		XtNbusyIcon,		XrmoptionNoArg, (caddr_t) don},
{"-busyiconname",	XtNbusyIconName,	XrmoptionSepArg,(caddr_t) NULL},
{"-busyiconpixmap",	XtNbusyIconPixmap,	XrmoptionSepArg,(caddr_t) NULL},
{"-calvin",		XtNcalvin,		XrmoptionNoArg, (caddr_t) don},
{"-cancelcount",	XtNcancelCount,		XrmoptionSepArg,(caddr_t) NULL},
{"-cc",			XtNcc,			XrmoptionNoArg, (caddr_t) doff},
{"+cc",			XtNcc,			XrmoptionNoArg, (caddr_t) don},
{"-ccforward",		XtNccForward,		XrmoptionNoArg, (caddr_t) doff},
{"+ccforward",		XtNccForward,		XrmoptionNoArg, (caddr_t) don},
{"-compbreaklength",	XtNcompBreakLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-complinelength",	XtNcompLineLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-comprows",		XtNcompRows,		XrmoptionSepArg,(caddr_t) NULL},
{"-confirm",		XtNconfirm,		XrmoptionSepArg,(caddr_t) NULL},
{"-deadletters",	XtNdeadLetters,		XrmoptionSepArg,(caddr_t) NULL},
{"-dumpcore",		XtNdumpCore,		XrmoptionNoArg, (caddr_t) doff},
{"+dumpcore",		XtNdumpCore,		XrmoptionNoArg, (caddr_t) don},
{"-defaultlines",	XtNdefaultLines,	XrmoptionSepArg,(caddr_t) NULL},
{"-delayedprint",	XtNdelayedPrint,	XrmoptionNoArg, (caddr_t) doff},
{"+delayedprint",	XtNdelayedPrint,	XrmoptionNoArg, (caddr_t) don},
{"-displaylinecount",	XtNdisplayLineCount,	XrmoptionNoArg, (caddr_t) doff},
{"+displaylinecount",	XtNdisplayLineCount,	XrmoptionNoArg, (caddr_t) don},
#ifdef REALLY_USE_LOCALTIME
{"-displaylocaltime",	XtNdisplayLocalTime,	XrmoptionNoArg, (caddr_t) doff},
{"+displaylocaltime",	XtNdisplayLocalTime,	XrmoptionNoArg, (caddr_t) don},
#endif
{"-distribution",	XtNdistribution,	XrmoptionSepArg,(caddr_t) NULL},
{"-editorcommand",	XtNeditorCommand,	XrmoptionSepArg,(caddr_t) NULL},
{"-executablesignatures",XtNexecutableSignatures,XrmoptionNoArg,(caddr_t) doff},
{"+executablesignatures",XtNexecutableSignatures,XrmoptionNoArg,(caddr_t) don},
{"-extramailheaders",	XtNextraMailHeaders,	XrmoptionNoArg, (caddr_t) doff},
{"+extramailheaders",	XtNextraMailHeaders,	XrmoptionNoArg, (caddr_t) don},
{"-hostname",		XtNhostName,		XrmoptionSepArg,(caddr_t) NULL},
{"-icongeometry",	XtNiconGeometry,	XrmoptionSepArg,(caddr_t) NULL},
{"-iconname",           XtNiconName, 		XrmoptionSepArg,(caddr_t) NULL},
{"-iconpixmap",		XtNiconPixmap, 		XrmoptionSepArg,(caddr_t) NULL},
{"-ignorenewsgroups",	XtNignoreNewsgroups,	XrmoptionSepArg,(caddr_t) NULL},
{"-includecommand",	XtNincludeCommand,	XrmoptionSepArg,(caddr_t) NULL},
{"-includeheader",	XtNincludeHeader,	XrmoptionNoArg, (caddr_t) doff},
{"+includeheader",	XtNincludeHeader,	XrmoptionNoArg, (caddr_t) don},
{"-includeprefix",	XtNincludePrefix,	XrmoptionSepArg,(caddr_t) NULL},
{"-includesep",		XtNincludeSep,		XrmoptionNoArg, (caddr_t) doff},
{"+includesep",		XtNincludeSep,		XrmoptionNoArg, (caddr_t) don},
{"-info",		XtNinfo,		XrmoptionNoArg, (caddr_t) doff},
{"+info",		XtNinfo,		XrmoptionNoArg, (caddr_t) don},
{"-killfiles",		XtNkillFiles,		XrmoptionNoArg, (caddr_t) doff},
{"+killfiles",		XtNkillFiles,		XrmoptionNoArg, (caddr_t) don},
{"-layout",		XtNlayout,		XrmoptionSepArg,(caddr_t) NULL},
{"-leaveheaders",	XtNleaveHeaders,	XrmoptionSepArg,(caddr_t) NULL},
{"-localsignatures",	XtNlocalSignatures,	XrmoptionNoArg, (caddr_t) doff},
{"+localsignatures",	XtNlocalSignatures,	XrmoptionNoArg, (caddr_t) don},
{"-lockfile",		XtNlockFile,		XrmoptionSepArg,(caddr_t) NULL},
{"-maxlines",		XtNmaxLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-mhpath",		XtNmhPath,		XrmoptionSepArg,(caddr_t) NULL},
{"-minlines",		XtNminLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-newsrcfile",		XtNnewsrcFile,		XrmoptionSepArg,(caddr_t) NULL},
{"-ngbindings",		XtNngBindings,		XrmoptionSepArg,(caddr_t) NULL},
{"-ngbuttonlist",	XtNngButtonList,	XrmoptionSepArg,(caddr_t) NULL},
{"-ngpopuplist",	XtNngPopupList,		XrmoptionSepArg,(caddr_t) NULL},
{"-nntpserver",		XtNnntpServer,		XrmoptionSepArg,(caddr_t) NULL},
{"-onlyshow",		XtNonlyShow,		XrmoptionSepArg,(caddr_t) NULL},
{"-organization",	XtNorganization,	XrmoptionSepArg,(caddr_t) NULL},
{"-pagearticles",	XtNpageArticles,	XrmoptionNoArg, (caddr_t) doff},
{"+pagearticles",	XtNpageArticles,	XrmoptionNoArg, (caddr_t) don},
{"-personalname",	XtNpersonalName,	XrmoptionSepArg,(caddr_t) NULL},
{"-popupbutton",	XtNpopupButton,		XrmoptionSepArg,(caddr_t) NULL},
{"-pointerbackground",	XtNpointerBackground,	XrmoptionSepArg,(caddr_t) NULL},
{"-pointerforeground",	XtNpointerForeground,	XrmoptionSepArg,(caddr_t) NULL},
{"-printcommand",	XtNprintCommand,	XrmoptionSepArg,(caddr_t) NULL},
{"-replypath",		XtNreplyPath,		XrmoptionSepArg,(caddr_t) NULL},
{"-replyto",		XtNreplyTo,		XrmoptionSepArg,(caddr_t) NULL},
{"-resetsave",		XtNresetSave,		XrmoptionNoArg, (caddr_t) doff},
{"+resetsave",		XtNresetSave,		XrmoptionNoArg, (caddr_t) don},
{"-rescantime",		XtNrescanTime,		XrmoptionSepArg,(caddr_t) NULL},
{"-retainkilled",	XtNretainKilled,	XrmoptionNoArg, (caddr_t) doff},
{"+retainkilled",	XtNretainKilled,	XrmoptionNoArg, (caddr_t) don},
{"-retrylimit",		XtNretryLimit,		XrmoptionSepArg,(caddr_t) NULL},
{"-retrypopupcount",	XtNretryPopupCount,	XrmoptionSepArg,(caddr_t) NULL},
{"-retrypopuptimeout",	XtNretryPopupTimeout,	XrmoptionSepArg,(caddr_t) NULL},
{"-retrypause",		XtNretryPause,		XrmoptionSepArg,(caddr_t) NULL},
{"-savedir",		XtNsaveDir,		XrmoptionSepArg,(caddr_t) NULL},
{"-savenewsrcfile",	XtNsaveNewsrcFile,	XrmoptionSepArg,(caddr_t) NULL},
{"-savemode",		XtNsaveMode,		XrmoptionSepArg,(caddr_t) NULL},
{"-savepostings",	XtNsavePostings,	XrmoptionSepArg,(caddr_t) NULL},
{"-savestring",		XtNsaveString,		XrmoptionSepArg,(caddr_t) NULL},
{"-signaturefile",	XtNsignatureFile,	XrmoptionSepArg,(caddr_t) NULL},
{"-signaturenotify",	XtNsignatureNotify,	XrmoptionNoArg, (caddr_t) doff},
{"+signaturenotify",	XtNsignatureNotify,	XrmoptionNoArg, (caddr_t) don},
{"-sortedsubjects",	XtNsortedSubjects,	XrmoptionNoArg, (caddr_t) doff},
{"+sortedsubjects",	XtNsortedSubjects,	XrmoptionNoArg, (caddr_t) don},
{"-stripheaders",	XtNstripHeaders,	XrmoptionSepArg,(caddr_t) NULL},
{"-subjectlength",	XtNsubjectLength,	XrmoptionSepArg,(caddr_t) NULL},
{"-subjectread",	XtNsubjectRead,		XrmoptionNoArg, (caddr_t) doff},
{"+subjectread",	XtNsubjectRead,		XrmoptionNoArg, (caddr_t) don},
{"-textlines",		XtNtextLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-tmpdir",		XtNtmpDir,		XrmoptionSepArg,(caddr_t) NULL},
{"-toplines",		XtNtopLines,		XrmoptionSepArg,(caddr_t) NULL},
{"-unreadiconname",	XtNunreadIconName,	XrmoptionSepArg,(caddr_t) NULL},
{"-unreadiconpixmap",	XtNunreadIconPixmap,	XrmoptionSepArg,(caddr_t) NULL},
{"-updatenewsrc",	XtNupdateNewsrc,	XrmoptionNoArg, (caddr_t) doff},
{"+updatenewsrc",	XtNupdateNewsrc,	XrmoptionNoArg, (caddr_t) don},
{"-usegadgets",		XtNuseGadgets,		XrmoptionNoArg, (caddr_t) doff},
{"+usegadgets",		XtNuseGadgets,		XrmoptionNoArg, (caddr_t) don},
{"-verbosekill",	XtNverboseKill,		XrmoptionNoArg, (caddr_t) doff},
{"+verbosekill",	XtNverboseKill,		XrmoptionNoArg, (caddr_t) don},
{"-watchprogress",	XtNwatchProgress,	XrmoptionNoArg, (caddr_t) doff},
{"+watchprogress",	XtNwatchProgress,	XrmoptionNoArg, (caddr_t) don},
#ifdef WATCH
{"-watchunread",	XtNwatchUnread,		XrmoptionSepArg,(caddr_t) NULL},
#endif
{"-xrefmarkread",	XtNxrefMarkRead,	XrmoptionNoArg, (caddr_t) doff},
{"+xrefmarkread",	XtNxrefMarkRead,	XrmoptionNoArg, (caddr_t) don},
{"-nextreadsfirst",	XtNnextReadsFirst,	XrmoptionNoArg, (caddr_t) doff},
{"+nextreadsfirst",	XtNnextReadsFirst,	XrmoptionNoArg, (caddr_t) don},
};  
#endif

static XrmOptionDescRec defOptionList[] = {
{"+rv",			"*reverseVideo",	XrmoptionNoArg, (caddr_t) "off"},
{"+synchronous",	"*synchronous",		XrmoptionNoArg, (caddr_t) "off"},
{"-background",		"*background",		XrmoptionSepArg,(caddr_t) NULL},
{"-bd",			"*borderColor",		XrmoptionSepArg,(caddr_t) NULL},
{"-bg",			"*background",		XrmoptionSepArg,(caddr_t) NULL},
{"-bordercolor",	"*borderColor",		XrmoptionSepArg,(caddr_t) NULL},
{"-borderwidth",	".borderWidth",		XrmoptionSepArg,(caddr_t) NULL},
{"-bw",			".borderWidth",		XrmoptionSepArg,(caddr_t) NULL},
{"-display",		".display",		XrmoptionSepArg,(caddr_t) NULL},
{"-fg",			"*foreground",		XrmoptionSepArg,(caddr_t) NULL},
{"-fn",			"*font",		XrmoptionSepArg,(caddr_t) NULL},
{"-font",		"*font",		XrmoptionSepArg,(caddr_t) NULL},
{"-foreground",		"*foreground",		XrmoptionSepArg,(caddr_t) NULL},
{"-geometry",		".geometry",		XrmoptionSepArg,(caddr_t) NULL},
{"-iconic",		".iconic",		XrmoptionNoArg, (caddr_t) "on"},
{"-name",		".name",		XrmoptionSepArg,(caddr_t) NULL},
{"-reverse",		"*reverseVideo",	XrmoptionNoArg, (caddr_t) "on"},
{"-rv",			"*reverseVideo",	XrmoptionNoArg, (caddr_t) "on"},
{"-selectiontimeout",	".selectionTimeout",	XrmoptionSepArg,(caddr_t) NULL},
{"-synchronous",	"*synchronous",		XrmoptionNoArg, (caddr_t) "on"},
{"-title",		".title",		XrmoptionSepArg,(caddr_t) NULL},
{"-xnllanguage",	".xnlLanguage",		XrmoptionSepArg,(caddr_t) NULL},
{"-xrm",		NULL,			XrmoptionResArg,(caddr_t) NULL},
};
static void
usage(ac, av)
int ac;
char **av;  /* program name */
/*
 * print out the usage message
 *
 *   returns: void
 *   
 */
{
    int i;

    (void) printf("Unknown options:");
    for (i = 1; i <= ac - 1; i++) {
	if (index(av[i], ':') == NIL(char)) {
	    printf(" %s", av[i]);
	}
    }
    printf("\n");
    printf("usage: %s [options] [-display host:display]\n",
		app_resources.progName);
    printf("   -addBindings\t\tAdd mode bindings\n");
    printf("   -addButtonList\tList of Add mode buttons\n");
    printf("   -addPopupList\tList of Add mode popup buttons\n");
    printf("   -allBindings\t\tAll mode bindings\n");
    printf("   -allButtonList\tList of All mode buttons\n");
    printf("   -allPopupList\tList of All mode popup buttons\n");
    printf("   -artBindings\t\tArticle mode bindings\n");
    printf("   -artButtonList\tList of Article mode buttons (top box)\n");
    printf("   -artPopupList\tList of Article mode popup buttons\n");
    printf("   -artSpecButtonList\tList of Article mode buttons (bottom box)\n");
    printf("   +/-authorFullName\tUse author's fullname in article list\n");
    printf("   +/-autoRead\t\tRead first article when opening group\n");
#ifdef MOTIF
    printf("   +/-autoWrap\t\tWord wrap articles\n");
#endif
    printf("   +/-busyIcon\t\tInvert icon image when busy\n");
    printf("   -busyIconName\tIcon name used when busy\n");
    printf("   -busyIconPixmap\tIcon pixmap used when busy\n");
    printf("   +/-cc\t\tInclude 'Cc: user' in replies\n");
    printf("   +/-ccForward\t\tInclude 'Cc: user' in forwarded messages\n");    
    printf("   -compBreakLength\tLength of line at which line wrapping begins\n");
    printf("   -compLineLength\tLength of lines for article postings\n");
    printf("   -compRows\t\tNumber of composition text rows\n");
    printf("   -confirm\t\tTurn on/off confirmation boxes\n");
    printf("   -deadLetters file\tFile to store failed postings/messages\n");
    printf("   -defaultLines number\tDefault number of lines above cursor\n");
    printf("   +/-delayedPrint\tDelay print file until exit\n");
    printf("   +/-displayLineCount\tDisplay line count in the subject index\n");
#ifdef REALLY_USE_LOCALTIME
    printf("   +/-displayLocalTime\tDisplay local time in the Date: field\n");
#endif
    printf("   -distribution\tDefault distribution for messages\n");
#ifdef DUMPCORE
    printf("   +/-dumpCore\t\tDump core on error exit\n");
#endif
    printf("   -editorCommand\tEditor to use (defaults to the toolkit editor)\n");
    printf("   +/-executableSignatures Execute signature file to get signature text\n");
    printf("   -hostName\t\tOverride your host's default name\n");
    printf("   -iconGeometry +X+Y\tPosition of icon\n");
    printf("   -ignoreNewsgroups\tRegexps of newsgroups to ignore\n");
    printf("   +/-includeHeader\tInclude original article's header\n");
    printf("   -includePrefix\tPrefix [\"%s\"] for included lines\n",
	   INCLUDEPREFIX);
    printf("   +/-includeSep\tPut '>' in front of included lines\n");
    printf("   +/-info\t\tPut all information in the message pane\n");
    printf("   +/-killFiles\t\tTurn on/off the use of kill files\n");
    printf("   -layout WxH+X+Y\tSize and position of window\n");
    printf("   -leaveHeaders list\tHeaders to leave\n");
    printf("   +/-localSignatures\tSearch for signature files as for local kill files\n");
    printf("   -lockFile\t\tname of the XRN lock file\n");
#ifndef VMS
    printf("   -mailer\t\tMailer to use\n");
#endif
    printf("   -maxLines number\tMaximum number of lines above cursor\n");
    printf("   -minLines number\tMinimum number of lines above cursor\n");
    printf("   -newsrcFile file\t.newsrc filename\n");
    printf("   +/-nextReadsFirst\tNext Unread reads first unread article\n");
    printf("   -ngBindings\t\tNewsgroup mode bindings\n");
    printf("   -ngButtonList\tList of Newsgroup mode buttons\n");
    printf("   -ngPopupList\t\tList of Newsgroup mode popup buttons\n");
    printf("   -nntpServer name\tNNTP server\n");
    printf("   -onlyShow\t\tmark all but the last N articles in each group as read\n");
    printf("   -organization\tName of your organization\n");
#ifdef VMS
    printf("   -personalName\t\tYour name\n");
#endif
    printf("   +/-pageArticles\tSpacebar scrolls the current article\n");
    printf("   -pointerBackground color background color of mouse cursor\n");
    printf("   -pointerForeground color foreground color of mouse cursor\n");
    printf("   -popupButton\t\tButton for popup menus (2 or 3)\n");
    printf("   -printCommand\tCommand to use to print out an article\n");
    printf("   -replyPath\t\tProvide default path for mail replies\n");
    printf("   -replyTo\t\tValue used for the Reply-To field\n");
    printf("   -rescanTime\t\tIdle time before checking for new articles\n");
    printf("   +/-resetSave\t\treset the save dialog string on each posting\n");
    printf("   +/-retainKilled\tRetain killed articles in subject list\n");
    printf("   -retryLimit\t\tNumber of times to retry failed connection\n");
    printf("   -retryPopupCount\tNumber of times to retry before allowing abort\n");
    printf("   -retryPopupTimeout\tNumber of seconds to wait before continuing\n");
    printf("   -retryPause\t\tNumber of seconds between retries\n");
    printf("   -saveDir directory\tDirectory for saving files\n");
    printf("   -saveMode mode\tMethod of saving articles\n");
    printf("   -saveNewsrcFile file\tSaved .newsrc filename\n");
    printf("   -savePostings file\tFile to save postings/messages\n");
    printf("   -saveString\t\tstring to use in the save dialog\n");
    printf("   -signatureFile file\tSignature file for posting\n");
    printf("   +/-signatureNotify\tNotify user which signature file is being used\n");
    printf("   +/-sortedSubjects\tSort or do not sort the subjects\n");
    printf("   -stripHeaders list\tHeaders to strip\n");
    printf("   -subjectLength\tLength of subject lines\n");
    printf("   +/-subjectRead\tChange default from next unread to subject next\n");
    printf("   -textLines number\tNumber of lines used by the text window\n");
    printf("   -tmpDir\t\tTemporary article directory\n");
    printf("   -topLines number\tNumber of lines used by the top window\n");
    printf("   +/-unreadIconName\tIcon name used when unread articles\n");
    printf("   -unreadIconPixmap\tIcon pixmap used when unread articles\n");
    printf("   +/-updateNewsrc\tUpdate the .newsrc file each time a group is exited\n");
    printf("   +/-useGadgets\tUse gadget buttons instead of widgets\n");
    printf("   +/-verboseKill\tList subjects when killing articles\n");
    printf("   +/-watchProgress\tWatch catch-up progress\n");
#ifdef WATCH
    printf("   -watchUnread\t\tList of news groups to monitor\n");
#endif
    printf("   +/-xrefMarkRead\tCrossref articles marked read\n");

    ehErrorExitXRN("Unrecognized command - exiting.");
}

static int
strncmpIgnoringCase(str1, str2, length)
char *str1;
char *str2;
int  length;
{
    register int i, diff;
    for (i = 0; i < length; i++, str1++, str2++) {
	diff = ((*str1 >= 'A' && *str1 <= 'Z') ? (*str1 + 'a' - 'A') : *str1) -
	       ((*str2 >= 'A' && *str2 <= 'Z') ? (*str2 + 'a' - 'A') : *str2);
	if (diff) return diff;
    }
    return 0;
}

static void
CvtStringToSortedSubjects(args, num_args, fromVal, toVal)
XrmValuePtr args;
Cardinal *num_args;
XrmValuePtr fromVal;
XrmValuePtr toVal;
{

    char *ptr;
    static int result;
    result = -1;

    if (*num_args != 0) {
	XtWarningMsg("cvtStringToSortedSubjects", "wrongParameters",
		     "conversionError",
		     "String to Sorted Subjects conversion needs no extra arguments",
		      NULL, NULL);
    }

    ptr = (char *) fromVal->addr;
    if ((strncmpIgnoringCase("off", ptr, 3) == 0) ||
	(strncmpIgnoringCase("none", ptr, 4) == 0) ||
	(strncmpIgnoringCase("unsorted", ptr, 8) == 0) ||
	(strncmpIgnoringCase("false", ptr, 5) == 0)) {
	result = UNSORTED;
    } else {
	if ((strncmpIgnoringCase("on", ptr, 2) == 0) ||
	    (strncmpIgnoringCase("true", ptr, 6) == 0) ||
	    (strncmpIgnoringCase("sorted", ptr, 6) == 0) ||
	    (strncmpIgnoringCase("article", ptr, 7) == 0) ||
	    (strncmpIgnoringCase("subject", ptr, 7) == 0)) {
	    result = ARTICLE_SORTED;
	} else {
	    if (strncmpIgnoringCase("strict", ptr, 6) == 0) {
		result = STRICT_SORTED;
	    } else {
		if (strncmpIgnoringCase("thread", ptr, 6) == 0) {
		    result = THREAD_SORTED;
		}
	    }
	}
    }
    if (result != -1) {
	toVal->size = sizeof(int);
	toVal->addr = (caddr_t) &result;
    } else {
	toVal->size = 0;
	toVal->addr = NULL;
#ifdef MOTIF
	XtStringConversionWarning((char *) fromVal->addr, "SortedSubjects");
#else
	printf("Can't convert string %s to subject sort type\n",
		(char *) fromVal->addr);
#endif
    }
}

extern XrmDatabase XtDatabase();
static XrmDatabase	xrmDB;
static XrmDatabase	sysDB;
char   customName[512];

Widget
XrnInitialize(argc, argv)
unsigned int argc;
char **argv;
/*
 * initialize the toolkit, parse the command line, and handle the Xdefaults
 *
 *   returns: top level widget
 *
 */
{
    Widget widget;
    int uargc;
    char *ptr;
    static Arg shell_args[] = {
	{XtNinput, (XtArgVal)True},
	{XtNiconName, (XtArgVal) NULL},
	{XtNtitle, (XtArgVal) NULL},
    };

#ifdef BSD_BFUNCS
    bzero((char *) &app_resources, sizeof(app_resources));
#else
    memset((char *) &app_resources, 0, sizeof(app_resources));
#endif

    /* set up the program name */
#ifndef VMS
    if ((ptr = rindex(argv[0], '/')) == NIL(char)) {
	app_resources.progName = XtNewString(argv[0]);
    } else {
	ptr++;
	app_resources.progName = XtNewString(ptr);
    }
    customName[0] = '\0';
#ifdef USE_XAPPLRESDIR
/*
 * I've had several complaints about this not working
 * because XAPPLRESDIR isn't a simple directory, or isn't
 * writable. So .. it's gone (but not forgotten).
 */
    if ((ptr = getenv("XAPPLRESDIR")) != NULL) {
	strcpy(customName, ptr);
    } else {
	strcpy(customName, "~");
    }
#else /* USE_XAPPLRESDIR */
    strcpy(customName, "~");
#endif /* USE_XAPPLRESDIR */

#ifdef MOTIF
    strcat(customName, "/.mxrn");
#else
    strcat(customName, "/.dxrn");
#endif
    ptr = utNameExpand(customName);
    strcpy(customName, ptr);    
#else /* VMS */
#ifdef MOTIF
    app_resources.progName = XtNewString("mxrn");
    (void) strcpy(argv[0], "mxrn");
    (void) strcpy(customName, "decw$user_defaults:mxrn.dat");
#else
    app_resources.progName = XtNewString("dxrn");
    (void) strcpy(argv[0], "dxrn");
    (void) strcpy(customName, "decw$user_defaults:dxrn.dat");
#endif
#endif

    StringPool = NULL;

    XrmInitialize();
#ifdef MOTIF
    widget = XtInitialize(app_resources.progName, "mxrn",
#else
    widget = XtInitialize(app_resources.progName, "dxrn",
#endif
			  defOptionList, XtNumber(defOptionList),
			  (int *) &argc, argv);

    XtAddConverter(XtRString, XtRSortedSubjects, CvtStringToSortedSubjects,
	(XtConvertArgList) NULL, (Cardinal) 0);
    xrmDB = XrmGetFileDatabase(customName);
    sysDB = XtDatabase(XtDisplay(widget));
    if (sysDB != NULL) {
#ifdef MOTIF
	XrmPutStringResource(&sysDB, "*XmText*font",
		"*-*-*-Medium-R-Normal-*-*-120-*-*-M-*-ISO8859-1");
	XrmPutStringResource(&sysDB, "*XmText*fontList",
		"*-*-*-Medium-R-Normal-*-*-120-*-*-M-*-ISO8859-1");
	XrmPutStringResource(&sysDB, "*index*fontList",
		"*-*-*-Medium-R-Normal-*-*-120-*-*-M-*-ISO8859-1");
#else
	XrmPutStringResource(&sysDB, "*headerText*font",
		"*-*-*-Medium-R-Normal-*-*-120-*-*-M-*-ISO8859-1");
	XrmPutStringResource(&sysDB, "*composeText*font",
		"*-*-*-Medium-R-Normal-*-*-120-*-*-M-*-ISO8859-1");
	XrmPutStringResource(&sysDB, "*text*font",
		"*-*-*-Medium-R-Normal-*-*-120-*-*-M-*-ISO8859-1");
	XrmPutStringResource(&sysDB, "*index*font",
		"*-*-*-Medium-R-Normal-*-*-120-*-*-M-*-ISO8859-1");
#endif
    }
    if (xrmDB != NULL && sysDB != NULL) {
	XrmMergeDatabases(xrmDB, &sysDB);
    }

    /*
     * Parse the remaining command line arguments
     */

    uargc = argc;
    XrmParseCommand(&sysDB, optionList, XtNumber(optionList),
	app_resources.progName, &uargc, argv);
    argc = uargc;

    if (argc > 1) {
	usage(argc, argv);
    }

#if defined(__DATE__) && defined(WANT_DATE)
#ifdef MOTIF
    (void) sprintf(title, "mxrn - version %s (compiled on %s)",
		   XRN_VERSION, __DATE__);
#else
    (void) sprintf(title, "dxrn - version %s (compiled on %s)",
		   XRN_VERSION, __DATE__);
#endif
#else
#ifdef MOTIF
    (void) sprintf(title, "mxrn - version %s",
		   XRN_VERSION);
#else
    (void) sprintf(title, "dxrn - version %s",
		   XRN_VERSION);
#endif
#endif

    /* get the resources needed by xrn itself */
    XtGetApplicationResources(widget, (caddr_t) &app_resources,
			      resources, XtNumber(resources), 0, 0);

    /*
     * Convert empty strings to default values
     */

    fixupString(nntpServer);
    fixupString(leaveHeaders);
    fixupString(stripHeaders);
#ifdef VMS
    fixupString(personalName);
#endif
    fixupString(confirm);
    fixupString(editorCommand);
    fixupString(organization);
    fixupString(distribution);
    fixupString(replyTo);
    fixupString(printCommand);
    fixupString(hostName);
    fixupString(replyPath);
#ifdef WATCH
    fixupString(watchList);
#endif

    /* 
     * check and set the lock file - must be after the application resources
     * are processed
     */
    checkLock();

    /* set up the titles */
    shell_args[1].value = (XtArgVal) app_resources.iconName;
    shell_args[2].value = (XtArgVal) app_resources.title;

    XtSetValues(widget, shell_args, XtNumber(shell_args));

    /* article saving mode */

    app_resources.saveMode = 0;
    if (utSubstring(app_resources.strSaveMode, "mailbox") == 1) {
	app_resources.saveMode |= MAILBOX_SAVE;
    } else {
	app_resources.saveMode |= NORMAL_SAVE;
    }
    if (utSubstring(app_resources.strSaveMode, "noheaders") == 1) {
	app_resources.saveMode |= NOHEADERS_SAVE;
    } else {
	app_resources.saveMode |= HEADERS_SAVE;
    }

    if (utSubstring(app_resources.strSaveMode, "subdirs") == 1) {
	app_resources.saveMode |= SUBDIRS_SAVE;
    } else if (utSubstring(app_resources.strSaveMode, "onedir") == 1) {
	app_resources.saveMode |= ONEDIR_SAVE;
    } else {
	app_resources.saveMode |= SAVE_DIR_DEFAULT;
    }

#define ariN app_resources.ignoreNewsgroups
#define ariNL app_resources.ignoreNewsgroupsList

    if (ariN) {
	char *str = XtNewString(ariN);
	int i = 1;
	
	if (! (strtok(str, ", \n\t"))) {
	    XtFree(str);
	    goto no_ignored_newsgroups;
	}
	while (strtok((char *) 0, ", \n\t")) {
	    i++;
	}

	ariNL = (char **) XtMalloc(sizeof(char *) * (i + 1));
	if (! ariNL) {
	    ehErrorExitXRN("out of memory");
	}

	XtFree(str);

	for (i = 0, str = strtok(ariN, ", \n\t"); str;
	    str = strtok((char *) 0, ", \n\t")) {
	    ariNL[i++] = str;
#ifdef DEBUG
	    fprintf(stderr, "Ignoring \"%s\" groups.\n", str);
#endif
	}
	ariNL[i++] = 0;
    }
    else {
no_ignored_newsgroups:
	ariNL = (char **) XtMalloc(sizeof(char *));
	ariNL[0] = 0;
    }

#undef ariN
#undef ariNL

    /* header stripping mode */

    /* STRIP_HEADERS with a NIL table will leave all headers (nothing to strip) */
    app_resources.headerTree = avl_init_table((void *)strcmp);
    if (! app_resources.headerTree) {
	 ehErrorExitXRN("out of memory");
    }
    app_resources.headerMode = STRIP_HEADERS;

    /*
     * A leaveHeaders value of "all" cancels leaveHeaders, and a
     * stripHeaders value of "none" cancels stripHeaders.
     */

    if (app_resources.leaveHeaders) {
	utDowncase(app_resources.leaveHeaders);
	if (! strcmp(app_resources.leaveHeaders, "all"))
	    app_resources.leaveHeaders = NIL(char);
    }
    if (app_resources.stripHeaders) {
	utDowncase(app_resources.stripHeaders);
	if (! strcmp(app_resources.stripHeaders, "none"))
	    app_resources.stripHeaders = NIL(char);
    }

    if ((app_resources.leaveHeaders != NIL(char)) &&
	(app_resources.stripHeaders != NIL(char))) {
	ehErrorExitXRN("Only one of 'stripHeaders, leaveHeaders' allowed\n");
     }  

    if (app_resources.leaveHeaders != NIL(char)) {
	char *p, *token;
	
	app_resources.headerMode = LEAVE_HEADERS;
	p = app_resources.leaveHeaders;
	while ((token = strtok(p, ", \t\n")) != NIL(char)) {
	    utDowncase(token);
	    if (avl_insert(app_resources.headerTree, token, (char *) 1) < 0) {
		 ehErrorExitXRN("out of memory");
	    }
	    p = NIL(char);
	}
	
    } else if (app_resources.stripHeaders != NIL(char)) {
	char *p, *token;
	
	app_resources.headerMode = STRIP_HEADERS;
	p = app_resources.stripHeaders;
	while ((token = strtok(p, ", \t\n")) != NIL(char)) {
	    utDowncase(token);
	    if (avl_insert(app_resources.headerTree, token, (char *) 1) < 0) {
		 ehErrorExitXRN("out of memory");
	    }
	    p = NIL(char);
	}
    }

    /* confirm boxes */

    app_resources.confirmMode = 0;

    if (app_resources.confirm != NIL(char)) {
	char *p, *token;

	p = app_resources.confirm;
	while ((token = strtok(p, ", \t\n")) != NIL(char)) {
	    if (strcmp(token, "ngQuit") == 0) {
		app_resources.confirmMode |= NG_QUIT;
	    } else if (strcmp(token, "ngExit") == 0) {
		app_resources.confirmMode |= NG_EXIT;
	    } else if (strcmp(token, "ngCatchUp") == 0) {
		app_resources.confirmMode |= NG_CATCHUP;
	    } else if (strcmp(token, "artCatchUp") == 0) {
		app_resources.confirmMode |= ART_CATCHUP;
	    } else if (strcmp(token, "artFedUp") == 0) {
		app_resources.confirmMode |= ART_FEDUP;
	    } else if (strcmp(token, "ngUnsub") == 0) {
		app_resources.confirmMode |= NG_UNSUBSCRIBE;
	    } else if (strcmp(token, "artUnsub") == 0) {
		app_resources.confirmMode |= ART_UNSUBSCRIBE;
	    } else if (strcmp(token, "artEndAction") == 0) {
		app_resources.confirmMode |= ART_ENDACTION;
	    } else if (strcmp(token, "artSave") == 0) {
		app_resources.confirmMode |= ART_SAVE;
	    } else if (strcmp(token, "post") == 0) {
		app_resources.confirmMode |= COMP_POST;
	    } else {
		mesgPane(XRN_SERIOUS, "unknown confirm button: %s", token);
	    }
	    p = NIL(char);
	}
    }

    /* temporary directory */

    if (app_resources.tmpDir == NIL(char)) {
	char *p = getenv("TMPDIR");

	if (p == NIL(char)) {
	    /* 
	     * XXX added to deal with a possible compiler problem on
	     * the IBM RT running AOS using the hc2.1s compiler
	     * (reported by Jay Ford <jnford@jay.weeg.uiowa.edu>).
	     */
	    char *tmp_ptr = TEMPORARY_DIRECTORY;
	    app_resources.tmpDir = XtNewString(tmp_ptr);
	} else {
	    app_resources.tmpDir = XtNewString(p);
	}
    }

#ifdef VMS
    app_resources.tmpDir = XtNewString(utTildeExpand(app_resources.tmpDir));
#else
    {
	char tmpDir[128], *p;
	strcpy(tmpDir, utTildeExpand(app_resources.tmpDir));
	p = tmpDir + strlen(tmpDir) - 1;
	if (*p != '/') {
	    *(++p) = '/';
	    *(++p) = '\0';
	}
	app_resources.tmpDir = XtNewString(tmpDir);
    }
#endif

    /* reply path */

    if (app_resources.replyPath == NIL(char)) {
	char *p = getenv("REPLYPATH");

	if (p != NIL(char)) {
	    app_resources.replyPath = XtNewString(p);
	}
    }
    /* top list line limits */
    if (app_resources.topLines < 3)
	app_resources.topLines = 3;
    if (app_resources.minLines == -1)
	app_resources.minLines = app_resources.topLines/2;

    if (app_resources.maxLines == -1)
	app_resources.maxLines = app_resources.topLines/2;

    if (app_resources.textLines < 3)
	app_resources.textLines = 3;

    if (app_resources.popupButton != 2 &&
        app_resources.popupButton != 3) {
#ifdef MOTIF
	printf("Invalid popup button, default 3 used.\n");
        app_resources.popupButton = 3;
#else
	printf("Invalid popup button, default 2 used.\n");
        app_resources.popupButton = 2;
#endif
    }
#ifdef VMS
    ptr = getenv("FULLNAME");
    if (ptr != NULL) {
	app_resources.personalName = XtNewString(ptr);
    }
#endif

    /* delete empty editorCommand */
    if (app_resources.editorCommand != NIL(char) &&
       *app_resources.editorCommand == '\0') {
	app_resources.editorCommand = NIL(char);
    }
    /* line breaking */

    if (app_resources.editorCommand != NIL(char)) {
	app_resources.breakLength = 0;
	app_resources.lineLength = 0;
    }

    if (app_resources.authorLength == 0) {
	if (app_resources.displayLineCount)
	    app_resources.authorLength = 20;
	else
	    app_resources.authorLength = 24;
    }

    if (app_resources.subjectLength == 0) {
	if (app_resources.displayLineCount)
	    app_resources.subjectLength = 52;
	else
	    app_resources.subjectLength = 53;
    }
    return widget;
}
