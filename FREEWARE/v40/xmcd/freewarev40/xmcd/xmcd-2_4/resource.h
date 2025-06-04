/*
 *   xmcd - Motif(tm) CD Audio Player
 *
 *   Copyright (C) 1993-1998  Ti Kan
 *   E-mail: ti@amb.org
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#ifndef LINT
static char *_resource_h_ident_ = "@(#)resource.h	6.66 98/10/09";
#endif


/* Library path */
#define XmcdNlibdir			"libdir"
#define XmcdCLibdir			"Libdir"

/* X resources */
#define XmcdNversion			"version"
#define XmcdCVersion			"Version"
#define XmcdNmainWindowMode		"mainWindowMode"
#define XmcdCMainWindowMode		"MainWindowMode"
#define XmcdNmodeChangeGravity		"modeChangeGravity"
#define XmcdCModeChangeGravity		"ModeChangeGravity"
#define XmcdNnormalMainWidth		"normalMainWidth"
#define XmcdCNormalMainWidth		"NormalMainWidth"
#define XmcdNnormalMainHeight		"normalMainHeight"
#define XmcdCNormalMainHeight		"NormalMainHeight"
#define XmcdNbasicMainWidth		"basicMainWidth"
#define XmcdCBasicMainWidth		"BasicMainWidth"
#define XmcdNbasicMainHeight		"basicMainHeight"
#define XmcdCBasicMainHeight		"BasicMainHeight"
#define XmcdNdisplayBlinkOnInterval	"displayBlinkOnInterval"
#define XmcdCDisplayBlinkOnInterval	"DisplayBlinkOnInterval"
#define XmcdNdisplayBlinkOffInterval	"displayBlinkOffInterval"
#define XmcdCDisplayBlinkOffInterval	"DisplayBlinkOffInterval"
#define XmcdNmainShowFocus		"mainShowFocus"
#define XmcdCMainShowFocus		"MainShowFocus"

/* Common config parameters */
#define XmcdNdevice			"device"
#define XmcdCDevice			"Device"
#define XmcdNcddbPath			"cddbPath"
#define XmcdCCddbPath			"CddbPath"
#define XmcdNcddbFileMode		"cddbFileMode"
#define XmcdCCddbFileMode		"CddbFileMode"
#define XmcdNcddbMailSite		"cddbMailSite"
#define XmcdCCddbMailSite		"CddbMailSite"
#define XmcdNcddbMailCmd		"cddbMailCmd"
#define XmcdCCddbMailCmd		"CddbMailCmd"
#define XmcdNproxyServer		"proxyServer"
#define XmcdCProxyServer		"ProxyServer"
#define XmcdNcddbDefaultPort		"cddbDefaultPort"
#define XmcdCCddbDefaultPort		"CddbDefaultPort"
#define XmcdNcddbTimeOut		"cddbTimeOut"
#define XmcdCCddbTimeOut		"CddbTimeOut"
#define XmcdNstatusPollInterval		"statusPollInterval"
#define XmcdCStatusPollInterval		"StatusPollInterval"
#define XmcdNinsertPollInterval		"insertPollInterval"
#define XmcdCInsertPollInterval		"InsertPollInterval"
#define XmcdNpreviousThreshold		"previousThreshold"
#define XmcdCPreviousThreshold		"PreviousThreshold"
#define XmcdNsampleBlocks		"sampleBlocks"
#define XmcdCSampleBlocks		"SampleBlocks"
#define XmcdNtimeDisplayMode		"timeDisplayMode"
#define XmcdCTimeDisplayMode		"TimeDisplayMode"
#define XmcdNtooltipDelayInterval	"tooltipDelayInterval"
#define XmcdCTooltipDelayInterval	"TooltipDelayInterval"
#define XmcdNtooltipActiveInterval	"tooltipActiveInterval"
#define XmcdCTooltipActiveInterval	"TooltipActiveInterval"
#define XmcdNinsertPollDisable		"insertPollDisable"
#define XmcdCInsertPollDisable		"InsertPollDisable"
#define XmcdNcddbRemoteDisable		"cddbRemoteDisable"
#define XmcdCCddbRemoteDisable		"CddbRemoteDisable"
#define XmcdNcddbRemoteAutoSave		"cddbRemoteAutoSave"
#define XmcdCCddbRemoteAutoSave		"CddbRemoteAutoSave"
#define XmcdNcddbUseHttpProxy		"cddbUseHttpProxy"
#define XmcdCCddbUseHttpProxy		"CddbUseHttpProxy"
#define XmcdNshowScsiErrMsg		"showScsiErrMsg"
#define XmcdCShowScsiErrMsg		"ShowScsiErrMsg"
#define XmcdNsolaris2VolumeManager	"solaris2VolumeManager"
#define XmcdCSolaris2VolumeManager	"Solaris2VolumeManager"
#define XmcdNcurfileEnable		"curfileEnable"
#define XmcdCCurfileEnable		"CurfileEnable"
#define XmcdNtooltipEnable		"tooltipEnable"
#define XmcdCTooltipEnable		"TooltipEnable"
#define XmcdNdebugMode			"debugMode"
#define XmcdCDebugMode			"DebugMode"

/* Device-specific config parameters */
#define XmcdNdevnum			"devnum"
#define XmcdCDevnum			"Devnum"
#define XmcdNdeviceList			"deviceList"
#define XmcdCDeviceList			"DeviceList"
#define XmcdNdeviceInterfaceMethod	"deviceInterfaceMethod"
#define XmcdCDeviceInterfaceMethod	"DeviceInterfaceMethod"
#define XmcdNdriveVendorCode		"driveVendorCode"
#define XmcdCDriveVendorCode		"DriveVendorCode"
#define XmcdNnumDiscs			"numDiscs"
#define XmcdCNumDiscs			"NumDiscs"
#define XmcdNmediumChangeMethod		"mediumChangeMethod"
#define XmcdCMediumChangeMethod		"MediumChangeMethod"
#define XmcdNscsiAudioVolumeBase	"scsiAudioVolumeBase"
#define XmcdCScsiAudioVolumeBase	"ScsiAudioVolumeBase"
#define XmcdNminimumPlayBlocks		"minimumPlayBlocks"
#define XmcdCMinimumPlayBlocks		"MinimumPlayBlocks"
#define XmcdNplayAudio10Support		"playAudio10Support"
#define XmcdCPlayAudio10Support		"PlayAudio10Support"
#define XmcdNplayAudio12Support		"playAudio12Support"
#define XmcdCPlayAudio12Support		"PlayAudio12Support"
#define XmcdNplayAudioMSFSupport	"playAudioMSFSupport"
#define XmcdCPlayAudioMSFSupport	"PlayAudioMSFSupport"
#define XmcdNplayAudioTISupport		"playAudioTISupport"
#define XmcdCPlayAudioTISupport		"PlayAudioTISupport"
#define XmcdNloadSupport		"loadSupport"
#define XmcdCLoadSupport		"LoadSupport"
#define XmcdNejectSupport		"ejectSupport"
#define XmcdCEjectSupport		"EjectSupport"
#define XmcdNmodeSenseSetDBD		"modeSenseSetDBD"
#define XmcdCModeSenseSetDBD		"ModeSenseSetDBD"
#define XmcdNvolumeControlSupport	"volumeControlSupport"
#define XmcdCVolumeControlSupport	"VolumeControlSupport"
#define XmcdNbalanceControlSupport	"balanceControlSupport"
#define XmcdCBalanceControlSupport	"BalanceControlSupport"
#define XmcdNchannelRouteSupport	"channelRouteSupport"
#define XmcdCChannelRouteSupport	"ChannelRouteSupport"
#define XmcdNpauseResumeSupport		"pauseResumeSupport"
#define XmcdCPauseResumeSupport		"PauseResumeSupport"
#define XmcdNcaddyLockSupport		"caddyLockSupport"
#define XmcdCCaddyLockSupport		"CaddyLockSupport"
#define XmcdNcurposFormat		"curposFormat"
#define XmcdCCurposFormat		"CurposFormat"
#define XmcdNnoTURWhenPlaying		"noTURWhenPlaying"
#define XmcdCNoTURWhenPlaying		"NoTURWhenPlaying"
#define XmcdNvolumeControlTaper		"volumeControlTaper"
#define XmcdCVolumeControlTaper		"VolumeControlTaper"
#define XmcdNstartupVolume		"startupVolume"
#define XmcdCStartupVolume		"StartupVolume"
#define XmcdNchannelRoute		"channelRoute"
#define XmcdCChannelRoute		"ChannelRoute"
#define XmcdNsearchSkipBlocks		"searchSkipBlocks"
#define XmcdCSearchSkipBlocks		"SearchSkipBlocks"
#define XmcdNsearchPauseInterval	"searchPauseInterval"
#define XmcdCSearchPauseInterval	"SearchPauseInterval"
#define XmcdNsearchSpeedUpCount		"searchSpeedUpCount"
#define XmcdCSearchSpeedUpCount		"SearchSpeedUpCount"
#define XmcdNsearchVolumePercent	"searchVolumePercent"
#define XmcdCSearchVolumePercent	"SearchVolumePercent"
#define XmcdNsearchMinVolume		"searchMinVolume"
#define XmcdCSearchMinVolume		"SearchMinVolume"
#define XmcdNspinDownOnLoad		"spinDownOnLoad"
#define XmcdCSpinDownOnLoad		"SpinDownOnLoad"
#define XmcdNplayOnLoad			"playOnLoad"
#define XmcdCPlayOnLoad			"PlayOnLoad"
#define XmcdNejectOnDone		"ejectOnDone"
#define XmcdCEjectOnDone		"EjectOnDone"
#define XmcdNejectOnExit		"ejectOnExit"
#define XmcdCEjectOnExit		"EjectOnExit"
#define XmcdNstopOnExit			"stopOnExit"
#define XmcdCStopOnExit			"StopOnExit"
#define XmcdNexitOnEject		"exitOnEject"
#define XmcdCExitOnEject		"ExitOnEject"
#define XmcdNcloseOnEject		"closeOnEject"
#define XmcdCCloseOnEject		"CloseOnEject"
#define XmcdNcaddyLock			"caddyLock"
#define XmcdCCaddyLock			"CaddyLock"
#define XmcdNrepeatMode			"repeatMode"
#define XmcdCRepeatMode			"RepeatMode"
#define XmcdNshuffleMode		"shuffleMode"
#define XmcdCShuffleMode		"ShuffleMode"
#define XmcdNmultiPlay			"multiPlay"
#define XmcdCMultiPlay			"MultiPlay"
#define XmcdNreversePlay		"reversePlay"
#define XmcdCReversePlay		"ReversePlay"
#define XmcdNinternalSpeakerEnable	"internalSpeakerEnable"
#define XmcdCInternalSpeakerEnable	"InternalSpeakerEnable"

/* Various application message strings */
#define XmcdNmainWindowTitle		"mainWindowTitle"
#define XmcdCMainWindowTitle		"MainWindowTitle"
#define XmcdNcddbLocalMsg		"cddbLocalMsg"
#define XmcdCCddbLocalMsg		"CddbLocalMsg"
#define XmcdNcddbRemoteMsg		"cddbRemoteMsg"
#define XmcdCCddbRemoteMsg		"CddbRemoteMsg"
#define XmcdNcddbQueryMsg		"cddbQueryMsg"
#define XmcdCCddbQueryMsg		"CddbQueryMsg"
#define XmcdNprogModeMsg		"progModeMsg"
#define XmcdCProgModeMsg		"ProgModeMsg"
#define XmcdNelapseMsg			"elapseMsg"
#define XmcdCElapseMsg			"ElapseMsg"
#define XmcdNelapseDiscMsg		"elapseDiscMsg"
#define XmcdCElapseDiscMsg		"ElapseDiscMsg"
#define XmcdNremainTrackMsg		"remainTrackMsg"
#define XmcdCRemainTrackMsg		"RemainTrackMsg"
#define XmcdNremainDiscMsg		"remainDiscMsg"
#define XmcdCRemainDiscMsg		"RemainDiscMsg"
#define XmcdNplayMsg			"playMsg"
#define XmcdCPlayMsg			"PlayMsg"
#define XmcdNpauseMsg			"pauseMsg"
#define XmcdCPauseMsg			"PauseMsg"
#define XmcdNreadyMsg			"readyMsg"
#define XmcdCReadyMsg			"ReadyMsg"
#define XmcdNsampleMsg			"sampleMsg"
#define XmcdCSampleMsg			"SampleMsg"
#define XmcdNusageMsg			"usageMsg"
#define XmcdCUsageMsg			"UsageMsg"
#define XmcdNbadOptsMsg			"badOptsMsg"
#define XmcdCBadOptsMsg			"BadOptsMsg"
#define XmcdNnoDiscMsg			"noDiscMsg"
#define XmcdCNoDiscMsg			"NoDiscMsg"
#define XmcdNdevBusyMsg			"devBusyMsg"
#define XmcdCDevBusyMsg			"DevBusyMsg"
#define XmcdNunknownDiscMsg		"unknownDiscMsg"
#define XmcdCUnknownDiscMsg		"UnknownDiscMsg"
#define XmcdNunknownTrackMsg		"unknownTrackMsg"
#define XmcdCUnknownTrackMsg		"UnknownTrackMsg"
#define XmcdNdataMsg			"dataMsg"
#define XmcdCDataMsg			"DataMsg"
#define XmcdNwarningMsg			"warningMsg"
#define XmcdCWarningMsg			"WarningMsg"
#define XmcdNfatalMsg			"fatalMsg"
#define XmcdCFatalMsg			"FatalMsg"
#define XmcdNconfirmMsg			"confirmMsg"
#define XmcdCConfirmMsg			"ConfirmMsg"
#define XmcdNinfoMsg			"infoMsg"
#define XmcdCInfoMsg			"InfoMsg"
#define XmcdNaboutMsg			"aboutMsg"
#define XmcdCAboutMsg			"AboutMsg"
#define XmcdNquitMsg			"quitMsg"
#define XmcdCQuitMsg			"QuitMsg"
#define XmcdNnoMemMsg			"noMemMsg"
#define XmcdCNoMemMsg			"NoMemMsg"
#define XmcdNtmpdirErrMsg		"tmpdirErrMsg"
#define XmcdCTmpdirErrMsg		"TmpdirErrMsg"
#define XmcdNlibdirErrMsg		"libdirErrMsg"
#define XmcdCLibdirErrMsg		"LibdirErrMsg"
#define XmcdNlongPathErrMsg		"longPathErrMsg"
#define XmcdCLongPathErrMsg		"LongPathErrMsg"
#define XmcdNnoMethodErrMsg		"noMethodErrMsg"
#define XmcdCNoMethodErrMsg		"NoMethodErrMsg"
#define XmcdNnoVuErrMsg			"noVuErrMsg"
#define XmcdCNoVuErrMsg			"NoVuErrMsg"
#define XmcdNnoHelpMsg			"noHelpMsg"
#define XmcdCNoHelpMsg			"NoHelpMsg"
#define XmcdNnoLinkMsg			"noLinkMsg"
#define XmcdCNoLinkMsg			"NoLinkMsg"
#define XmcdNnoDbMsg			"noDbMsg"
#define XmcdCNoDbMsg			"NoDbMsg"
#define XmcdNnoCfgMsg			"noCfgMsg"
#define XmcdCNoCfgMsg			"NoCfgMsg"
#define XmcdNnotRomMsg			"notRomMsg"
#define XmcdCNotRomMsg			"NotRomMsg"
#define XmcdNnotScsi2Msg		"notScsi2Msg"
#define XmcdCNotScsi2Msg		"NotScsi2Msg"
#define XmcdNsendConfirmMsg		"sendConfirmMsg"
#define XmcdCSendConfirmMsg		"SendConfirmMsg"
#define XmcdNmailErrMsg			"mailErrMsg"
#define XmcdCMailErrMsg			"MailErrMsg"
#define XmcdNmodeErrMsg			"modeErrMsg"
#define XmcdCModeErrMsg			"ModeErrMsg"
#define XmcdNstatErrMsg			"statErrMsg"
#define XmcdCStatErrMsg			"StatErrMsg"
#define XmcdNnodeErrMsg			"nodeErrMsg"
#define XmcdCNodeErrMsg			"NodeErrMsg"
#define XmcdNdbIncmplErrMsg		"dbIncmplErrMsg"
#define XmcdCDbIncmplErrMsg		"DbIncmplErrMsg"
#define XmcdNseqFmtErrMsg		"seqFmtErrMsg"
#define XmcdCSeqFmtErrMsg		"SeqFmtErrMsg"
#define XmcdNinvPgmTrkMsg		"invPgmTrkMsg"
#define XmcdCInvPgmTrkMsg		"InvPgmTrkMsg"
#define XmcdNrecovErrMsg		"recovErrMsg"
#define XmcdCRecovErrMsg		"RecovErrMsg"
#define XmcdNmaxErrMsg			"maxErrMsg"
#define XmcdCMaxErrMsg			"MaxErrMsg"
#define XmcdNsavErrForkMsg		"savErrForkMsg"
#define XmcdCSavErrForkMsg		"SavErrForkMsg"
#define XmcdNsavErrSuidMsg		"savErrSuidMsg"
#define XmcdCSavErrSuidMsg		"SavErrSuidMsg"
#define XmcdNsavErrOpenMsg		"savErrOpenMsg"
#define XmcdCSavErrOpenMsg		"SavErrOpenMsg"
#define XmcdNsavErrCloseMsg		"savErrCloseMsg"
#define XmcdCSavErrCloseMsg		"SavErrCloseMsg"
#define XmcdNsavErrWriteMsg		"savErrWriteMsg"
#define XmcdCSavErrWriteMsg		"SavErrWriteMsg"
#define XmcdNsavErrKilledMsg		"savErrKilledMsg"
#define XmcdCSavErrKilledMsg		"SavErrKilledMsg"
#define XmcdNlnkErrForkMsg		"lnkErrForkMsg"
#define XmcdCLnkErrForkMsg		"LnkErrForkMsg"
#define XmcdNlnkErrSuidMsg		"lnkErrSuidMsg"
#define XmcdCLnkErrSuidMsg		"LnkErrSuidMsg"
#define XmcdNlnkErrLinkMsg		"lnkErrLinkMsg"
#define XmcdCLnkErrLinkMsg		"LnkErrLinkMsg"
#define XmcdNlnkErrKilledMsg		"lnkErrKilledMsg"
#define XmcdCLnkErrKilledMsg		"LnkErrKilledMsg"
#define XmcdNsearchDbMsg		"searchDbMsg"
#define XmcdCSearchDbMsg		"SearchDbMsg"
#define XmcdNremoteMatchMsg		"remoteMatchMsg"
#define XmcdCRemoteMatchMsg		"RemoteMatchMsg"
#define XmcdNchangeSaveMsg		"changeSaveMsg"
#define XmcdCChangeSaveMsg		"ChangeSaveMsg"
#define XmcdNdevlistUndefMsg		"devlistUndefMsg"
#define XmcdCDevlistUndefMsg		"DevlistUndefMsg"
#define XmcdNdevlistCountMsg		"devlistCountMsg"
#define XmcdCDevlistCountMsg		"DevlistCountMsg"
#define XmcdNchangerInitErrMsg		"changerInitErrMsg"
#define XmcdCChangerInitErrMsg		"ChangerInitErrMsg"
#define XmcdNproxyConfigErrMsg		"proxyConfigErrMsg"
#define XmcdCProxyConfigErrMsg		"ProxyConfigErrMsg"
#define XmcdNproxyAuthFailMsg		"proxyAuthFailMsg"
#define XmcdCProxyAuthFailMsg		"ProxyAuthFailMsg"
#define XmcdNbrowserStartMsg		"browserStartMsg"
#define XmcdCBrowserStartMsg		"BrowserStartMsg"
#define XmcdNbrowserFailMsg		"browserFailMsg"
#define XmcdCBrowserFailMsg		"BrowserFailMsg"
#define XmcdNappDefFileMsg		"appDefFileMsg"
#define XmcdCAppDefFileMsg		"AppDefFileMsg"
#define XmcdNkpModeDisableMsg		"kpModeDisableMsg"
#define XmcdCKpModeDisableMsg		"KpModeDisableMsg"

/* Short-cut key definitions */
#define XmcdNmodeKey			"modeKey"
#define XmcdCModeKey			"ModeKey"
#define XmcdNlockKey			"lockKey"
#define XmcdCLockKey			"LockKey"
#define XmcdNrepeatKey			"repeatKey"
#define XmcdCRepeatKey			"RepeatKey"
#define XmcdNshuffleKey			"shuffleKey"
#define XmcdCShuffleKey			"ShuffleKey"
#define XmcdNejectKey			"ejectKey"
#define XmcdCEjectKey			"EjectKey"
#define XmcdNquitKey			"quitKey"
#define XmcdCQuitKey			"QuitKey"
#define XmcdNdbprogKey			"dbprogKey"
#define XmcdCDbprogKey			"DbprogKey"
#define XmcdNhelpKey			"helpKey"
#define XmcdCHelpKey			"HelpKey"
#define XmcdNoptionsKey			"optionsKey"
#define XmcdCOptionsKey			"OptionsKey"
#define XmcdNtimeKey			"timeKey"
#define XmcdCTimeKey			"TimeKey"
#define XmcdNabKey			"abKey"
#define XmcdCAbKey			"AbKey"
#define XmcdNsampleKey			"sampleKey"
#define XmcdCSampleKey			"SampleKey"
#define XmcdNkeypadKey			"keypadKey"
#define XmcdCKeypadKey			"KeypadKey"
#define XmcdNplayPauseKey		"playPauseKey"
#define XmcdCPlayPauseKey		"PlayPauseKey"
#define XmcdNstopKey			"stopKey"
#define XmcdCStopKey			"StopKey"
#define XmcdNprevDiscKey		"prevDiscKey"
#define XmcdCPrevDiscKey		"PrevDiscKey"
#define XmcdNnextDiscKey		"nextDiscKey"
#define XmcdCNextDiscKey		"NextDiscKey"
#define XmcdNprevTrackKey		"prevTrackKey"
#define XmcdCPrevTrackKey		"PrevTrackKey"
#define XmcdNnextTrackKey		"nextTrackKey"
#define XmcdCNextTrackKey		"NextTrackKey"
#define XmcdNprevIndexKey		"prevIndexKey"
#define XmcdCPrevIndexKey		"PrevIndexKey"
#define XmcdNnextIndexKey		"nextIndexKey"
#define XmcdCNextIndexKey		"NextIndexKey"
#define XmcdNrewKey			"rewKey"
#define XmcdCRewKey			"RewKey"
#define XmcdNffKey			"ffKey"
#define XmcdCFfKey			"FfKey"
#define XmcdNkeypadNumKey0		"keypadNumKey0"
#define XmcdCKeypadNumKey0		"KeypadNumKey0"
#define XmcdNkeypadNumKey1		"keypadNumKey1"
#define XmcdCKeypadNumKey1		"KeypadNumKey1"
#define XmcdNkeypadNumKey2		"keypadNumKey2"
#define XmcdCKeypadNumKey2		"KeypadNumKey2"
#define XmcdNkeypadNumKey3		"keypadNumKey3"
#define XmcdCKeypadNumKey3		"KeypadNumKey3"
#define XmcdNkeypadNumKey4		"keypadNumKey4"
#define XmcdCKeypadNumKey4		"KeypadNumKey4"
#define XmcdNkeypadNumKey5		"keypadNumKey5"
#define XmcdCKeypadNumKey5		"KeypadNumKey5"
#define XmcdNkeypadNumKey6		"keypadNumKey6"
#define XmcdCKeypadNumKey6		"KeypadNumKey6"
#define XmcdNkeypadNumKey7		"keypadNumKey7"
#define XmcdCKeypadNumKey7		"KeypadNumKey7"
#define XmcdNkeypadNumKey8		"keypadNumKey8"
#define XmcdCKeypadNumKey8		"KeypadNumKey8"
#define XmcdNkeypadNumKey9		"keypadNumKey9"
#define XmcdCKeypadNumKey9		"KeypadNumKey9"
#define XmcdNkeypadClearKey		"keypadClearKey"
#define XmcdCKeypadClearKey		"KeypadClearKey"
#define XmcdNkeypadEnterKey		"keypadEnterKey"
#define XmcdCKeypadEnterKey		"KeypadEnterKey"
#define XmcdNkeypadCancelKey		"keypadCancelKey"
#define XmcdCKeypadCancelKey		"KeypadCancelKey"



STATIC XtResource	resources[] = {
	/* Library path */
	{
		XmcdNlibdir, XmcdCLibdir,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, libdir), XmRImmediate,
		(XtPointer) NULL,
	},

	/* X resources */
	{
		XmcdNversion, XmcdCVersion,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, version), XmRImmediate,
		(XtPointer) NULL,
	},
	{
		XmcdNmainWindowMode, XmcdCMainWindowMode,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, main_mode), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNmodeChangeGravity, XmcdCModeChangeGravity,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, modechg_grav), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNnormalMainWidth, XmcdCNormalMainWidth,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, normal_width), XmRImmediate,
		(XtPointer) 360,
	},
	{
		XmcdNnormalMainHeight, XmcdCNormalMainHeight,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, normal_height), XmRImmediate,
		(XtPointer) 135,
	},
	{
		XmcdNbasicMainWidth, XmcdCBasicMainWidth,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, basic_width), XmRImmediate,
		(XtPointer) 195,
	},
	{
		XmcdNbasicMainHeight, XmcdCBasicMainHeight,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, basic_height), XmRImmediate,
		(XtPointer) 60,
	},
	{
		XmcdNmainShowFocus, XmcdCMainShowFocus,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, main_showfocus), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNdisplayBlinkOnInterval, XmcdCDisplayBlinkOnInterval,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, blinkon_interval), XmRImmediate,
		(XtPointer) 850,
	},
	{
		XmcdNdisplayBlinkOffInterval, XmcdCDisplayBlinkOffInterval,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, blinkoff_interval), XmRImmediate,
		(XtPointer) 150,
	},

	/* Common config parameters */
	{
		XmcdNdevice, XmcdCDevice,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, device), XmRImmediate,
		(XtPointer) NULL,
	},
	{
		XmcdNcddbPath, XmcdCCddbPath,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, cddb_path), XmRImmediate,
		(XtPointer) NULL,
	},
	{
		XmcdNcddbFileMode, XmcdCCddbFileMode,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, cddb_filemode), XmRImmediate,
		(XtPointer) NULL,
	},
	{
		XmcdNcddbMailSite, XmcdCCddbMailSite,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, cddb_mailsite), XmRImmediate,
		(XtPointer) CDDB_MAILSITE,
	},
	{
		XmcdNcddbMailCmd, XmcdCCddbMailCmd,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, cddb_mailcmd), XmRImmediate,
		(XtPointer) CDDB_MAILCMD,
	},
	{
		XmcdNproxyServer, XmcdCProxyServer,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, proxy_server), XmRImmediate,
		(XtPointer) NULL,
	},
	{
		XmcdNcddbDefaultPort, XmcdCCddbDefaultPort,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, cddb_port), XmRImmediate,
		(XtPointer) CDDB_PORT,
	},
	{
		XmcdNcddbTimeOut, XmcdCCddbTimeOut,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, cddb_timeout), XmRImmediate,
		(XtPointer) CDDB_TIMEOUT,
	},
	{
		XmcdNstatusPollInterval, XmcdCStatusPollInterval,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, stat_interval), XmRImmediate,
		(XtPointer) 260,
	},
	{
		XmcdNinsertPollInterval, XmcdCInsertPollInterval,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, ins_interval), XmRImmediate,
		(XtPointer) 2000,
	},
	{
		XmcdNpreviousThreshold, XmcdCPreviousThreshold,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, prev_threshold), XmRImmediate,
		(XtPointer) 100,
	},
	{
		XmcdNsampleBlocks, XmcdCSampleBlocks,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, sample_blks), XmRImmediate,
		(XtPointer) 750,
	},
	{
		XmcdNtimeDisplayMode, XmcdCTimeDisplayMode,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, timedpy_mode), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNtooltipDelayInterval, XmcdCTooltipDelayInterval,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, tooltip_delay), XmRImmediate,
		(XtPointer) 1000,
	},
	{
		XmcdNtooltipActiveInterval, XmcdCTooltipActiveInterval,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, tooltip_time), XmRImmediate,
		(XtPointer) 3000,
	},
	{
		XmcdNinsertPollDisable, XmcdCInsertPollDisable,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, ins_disable), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNcddbRemoteDisable, XmcdCCddbRemoteDisable,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, cddb_rmtdsbl), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNcddbRemoteAutoSave, XmcdCCddbRemoteAutoSave,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, cddb_rmtautosave), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNcddbUseHttpProxy, XmcdCCddbUseHttpProxy,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, http_proxy), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNshowScsiErrMsg, XmcdCShowScsiErrMsg,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, scsierr_msg), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNsolaris2VolumeManager, XmcdCSolaris2VolumeManager,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, sol2_volmgt), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNcurfileEnable, XmcdCCurfileEnable,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, write_curfile), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNtooltipEnable, XmcdCTooltipEnable,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, tooltip_enable), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNdebugMode, XmcdCDebugMode,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, debug), XmRImmediate,
		(XtPointer) False,
	},

	/* Device-specific config parameters */
	{
		XmcdNdevnum, XmcdCDevnum,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, devnum), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNdeviceList, XmcdCDeviceList,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, devlist), XmRImmediate,
		(XtPointer) NULL,
	},
	{
		XmcdNdeviceInterfaceMethod, XmcdCDeviceInterfaceMethod,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, di_method), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNdriveVendorCode, XmcdCDriveVendorCode,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, vendor_code), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNnumDiscs, XmcdCNumDiscs,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, numdiscs), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNmediumChangeMethod, XmcdCMediumChangeMethod,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, chg_method), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNscsiAudioVolumeBase, XmcdCScsiAudioVolumeBase,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, base_scsivol), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNminimumPlayBlocks, XmcdCMinimumPlayBlocks,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, min_playblks), XmRImmediate,
		(XtPointer) 25,
	},
	{
		XmcdNplayAudio10Support, XmcdCPlayAudio10Support,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, play10_supp), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNplayAudio12Support, XmcdCPlayAudio12Support,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, play12_supp), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNplayAudioMSFSupport, XmcdCPlayAudioMSFSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, playmsf_supp), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNplayAudioTISupport, XmcdCPlayAudioTISupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, playti_supp), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNloadSupport, XmcdCLoadSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, load_supp), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNejectSupport, XmcdCEjectSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, eject_supp), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNmodeSenseSetDBD, XmcdCModeSenseSetDBD,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, msen_dbd), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNvolumeControlSupport, XmcdCVolumeControlSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, mselvol_supp), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNbalanceControlSupport, XmcdCBalanceControlSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, balance_supp), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNchannelRouteSupport, XmcdCChannelRouteSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, chroute_supp), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNpauseResumeSupport, XmcdCPauseResumeSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, pause_supp), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNcaddyLockSupport, XmcdCCaddyLockSupport,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, caddylock_supp), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNcurposFormat, XmcdCCurposFormat,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, curpos_fmt), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNnoTURWhenPlaying, XmcdCNoTURWhenPlaying,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, play_notur), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNvolumeControlTaper, XmcdCVolumeControlTaper,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, vol_taper), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNstartupVolume, XmcdCStartupVolume,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, startup_vol), XmRImmediate,
		(XtPointer) -1,
	},
	{
		XmcdNchannelRoute, XmcdCChannelRoute,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, ch_route), XmRImmediate,
		(XtPointer) 0,
	},
	{
		XmcdNsearchSkipBlocks, XmcdCSearchSkipBlocks,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, skip_blks), XmRImmediate,
		(XtPointer) 90,
	},
	{
		XmcdNsearchPauseInterval, XmcdCSearchPauseInterval,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, skip_pause), XmRImmediate,
		(XtPointer) 80,
	},
	{
		XmcdNsearchSpeedUpCount, XmcdCSearchSpeedUpCount,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, skip_spdup), XmRImmediate,
		(XtPointer) 15,
	},
	{
		XmcdNsearchVolumePercent, XmcdCSearchVolumePercent,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, skip_vol), XmRImmediate,
		(XtPointer) 35,
	},
	{
		XmcdNsearchMinVolume, XmcdCSearchMinVolume,
		XmRInt, sizeof(int),
		XtOffsetOf(appdata_t, skip_minvol), XmRImmediate,
		(XtPointer) 2,
	},
	{
		XmcdNspinDownOnLoad, XmcdCSpinDownOnLoad,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, load_spindown), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNplayOnLoad, XmcdCPlayOnLoad,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, load_play), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNejectOnDone, XmcdCEjectOnDone,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, done_eject), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNejectOnExit, XmcdCEjectOnExit,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, exit_eject), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNstopOnExit, XmcdCStopOnExit,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, exit_stop), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNexitOnEject, XmcdCExitOnEject,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, eject_exit), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNcloseOnEject, XmcdCCloseOnEject,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, eject_close), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNcaddyLock, XmcdCCaddyLock,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, caddy_lock), XmRImmediate,
		(XtPointer) True,
	},
	{
		XmcdNrepeatMode, XmcdCRepeatMode,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, repeat_mode), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNshuffleMode, XmcdCShuffleMode,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, shuffle_mode), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNmultiPlay, XmcdCMultiPlay,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, multi_play), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNreversePlay, XmcdCReversePlay,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, reverse), XmRImmediate,
		(XtPointer) False,
	},
	{
		XmcdNinternalSpeakerEnable, XmcdCInternalSpeakerEnable,
		XmRBoolean, sizeof(Boolean),
		XtOffsetOf(appdata_t, internal_spkr), XmRImmediate,
		(XtPointer) False,
	},

	/* Various application message strings */
	{
		XmcdNmainWindowTitle, XmcdCMainWindowTitle,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, main_title), XmRImmediate,
		(XtPointer) MAIN_TITLE,
	},
	{
		XmcdNcddbLocalMsg, XmcdCCddbLocalMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_cddblocal), XmRImmediate,
		(XtPointer) STR_CDDBLOCAL,
	},
	{
		XmcdNcddbRemoteMsg, XmcdCCddbRemoteMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_cddbremote), XmRImmediate,
		(XtPointer) STR_CDDBREMOTE,
	},
	{
		XmcdNcddbQueryMsg, XmcdCCddbQueryMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_cddbquery), XmRImmediate,
		(XtPointer) STR_CDDBQUERY,
	},
	{
		XmcdNprogModeMsg, XmcdCProgModeMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_progmode), XmRImmediate,
		(XtPointer) STR_PROGMODE,
	},
	{
		XmcdNelapseMsg, XmcdCElapseMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_elapse), XmRImmediate,
		(XtPointer) STR_ELAPSE,
	},
	{
		XmcdNelapseDiscMsg, XmcdCElapseDiscMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_elapsedisc), XmRImmediate,
		(XtPointer) STR_ELAPSE_DISC,
	},
	{
		XmcdNremainTrackMsg, XmcdCRemainTrackMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_remaintrk), XmRImmediate,
		(XtPointer) STR_REMAIN_TRK,
	},
	{
		XmcdNremainDiscMsg, XmcdCRemainDiscMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_remaindisc), XmRImmediate,
		(XtPointer) STR_REMAIN_DISC,
	},
	{
		XmcdNplayMsg, XmcdCPlayMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_play), XmRImmediate,
		(XtPointer) STR_PLAY,
	},
	{
		XmcdNpauseMsg, XmcdCPauseMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_pause), XmRImmediate,
		(XtPointer) STR_PAUSE,
	},
	{
		XmcdNreadyMsg, XmcdCReadyMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_ready), XmRImmediate,
		(XtPointer) STR_READY,
	},
	{
		XmcdNsampleMsg, XmcdCSampleMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_sample), XmRImmediate,
		(XtPointer) STR_SAMPLE,
	},
	{
		XmcdNusageMsg, XmcdCUsageMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_usage), XmRImmediate,
		(XtPointer) STR_USAGE,
	},
	{
		XmcdNbadOptsMsg, XmcdCBadOptsMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_badopts), XmRImmediate,
		(XtPointer) STR_BADOPTS,
	},
	{
		XmcdNnoDiscMsg, XmcdCNoDiscMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_nodisc), XmRImmediate,
		(XtPointer) STR_NODISC,
	},
	{
		XmcdNdevBusyMsg, XmcdCDevBusyMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_busy), XmRImmediate,
		(XtPointer) STR_BUSY,
	},
	{
		XmcdNunknownDiscMsg, XmcdCUnknownDiscMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_unkndisc), XmRImmediate,
		(XtPointer) STR_UNKNDISC,
	},
	{
		XmcdNunknownTrackMsg, XmcdCUnknownTrackMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_unkntrk), XmRImmediate,
		(XtPointer) STR_UNKNTRK,
	},
	{
		XmcdNdataMsg, XmcdCDataMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_data), XmRImmediate,
		(XtPointer) STR_DATA,
	},
	{
		XmcdNwarningMsg, XmcdCWarningMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_warning), XmRImmediate,
		(XtPointer) STR_WARNING,
	},
	{
		XmcdNfatalMsg, XmcdCFatalMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_fatal), XmRImmediate,
		(XtPointer) STR_FATAL,
	},
	{
		XmcdNconfirmMsg, XmcdCConfirmMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_confirm), XmRImmediate,
		(XtPointer) STR_CONFIRM,
	},
	{
		XmcdNinfoMsg, XmcdCInfoMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_info), XmRImmediate,
		(XtPointer) STR_INFO,
	},
	{
		XmcdNaboutMsg, XmcdCAboutMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_about), XmRImmediate,
		(XtPointer) STR_ABOUT,
	},
	{
		XmcdNquitMsg, XmcdCQuitMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_quit), XmRImmediate,
		(XtPointer) STR_QUIT,
	},
	{
		XmcdNnoMemMsg, XmcdCNoMemMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_nomemory), XmRImmediate,
		(XtPointer) STR_NOMEMORY,
	},
	{
		XmcdNnoMethodErrMsg, XmcdCNoMethodErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_nomethod), XmRImmediate,
		(XtPointer) STR_NOMETHOD,
	},
	{
		XmcdNnoVuErrMsg, XmcdCNoVuErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_novu), XmRImmediate,
		(XtPointer) STR_NOVU,
	},
	{
		XmcdNtmpdirErrMsg, XmcdCTmpdirErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_tmpdirerr), XmRImmediate,
		(XtPointer) STR_TMPDIRERR,
	},
	{
		XmcdNlibdirErrMsg, XmcdCLibdirErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_libdirerr), XmRImmediate,
		(XtPointer) STR_LIBDIRERR,
	},
	{
		XmcdNlongPathErrMsg, XmcdCLongPathErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_longpatherr), XmRImmediate,
		(XtPointer) STR_LONGPATHERR,
	},
	{
		XmcdNnoHelpMsg, XmcdCNoHelpMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_nohelp), XmRImmediate,
		(XtPointer) STR_NOHELP,
	},
	{
		XmcdNnoLinkMsg, XmcdCNoLinkMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_nolink), XmRImmediate,
		(XtPointer) STR_NOLINK,
	},
	{
		XmcdNnoDbMsg, XmcdCNoDbMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_nodb), XmRImmediate,
		(XtPointer) STR_NODB,
	},
	{
		XmcdNnoCfgMsg, XmcdCNoCfgMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_nocfg), XmRImmediate,
		(XtPointer) STR_NOCFG,
	},
	{
		XmcdNnotRomMsg, XmcdCNotRomMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_notrom), XmRImmediate,
		(XtPointer) STR_NOTROM,
	},
	{
		XmcdNnotScsi2Msg, XmcdCNotScsi2Msg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_notscsi2), XmRImmediate,
		(XtPointer) STR_NOTSCSI2,
	},
	{
		XmcdNsendConfirmMsg, XmcdCSendConfirmMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_send), XmRImmediate,
		(XtPointer) STR_SEND,
	},
	{
		XmcdNmailErrMsg, XmcdCMailErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_mailerr), XmRImmediate,
		(XtPointer) STR_MAILERR,
	},
	{
		XmcdNmodeErrMsg, XmcdCModeErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_moderr), XmRImmediate,
		(XtPointer) STR_MODERR,
	},
	{
		XmcdNstatErrMsg, XmcdCStatErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_staterr), XmRImmediate,
		(XtPointer) STR_STATERR,
	},
	{
		XmcdNnodeErrMsg, XmcdCNodeErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_noderr), XmRImmediate,
		(XtPointer) STR_NODERR,
	},
	{
		XmcdNdbIncmplErrMsg, XmcdCDbIncmplErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_dbincmplerr), XmRImmediate,
		(XtPointer) STR_DBINCMPLERR,
	},
	{
		XmcdNseqFmtErrMsg, XmcdCSeqFmtErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_seqfmterr), XmRImmediate,
		(XtPointer) STR_SEQFMTERR,
	},
	{
		XmcdNinvPgmTrkMsg, XmcdCInvPgmTrkMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_invpgmtrk), XmRImmediate,
		(XtPointer) STR_INVPGMTRK,
	},
	{
		XmcdNrecovErrMsg, XmcdCRecovErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_recoverr), XmRImmediate,
		(XtPointer) STR_RECOVERR,
	},
	{
		XmcdNmaxErrMsg, XmcdCMaxErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_maxerr), XmRImmediate,
		(XtPointer) STR_MAXERR,
	},
	{
		XmcdNsavErrForkMsg, XmcdCSavErrForkMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_saverr_fork), XmRImmediate,
		(XtPointer) STR_SAVERR_FORK,
	},
	{
		XmcdNsavErrSuidMsg, XmcdCSavErrSuidMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_saverr_suid), XmRImmediate,
		(XtPointer) STR_SAVERR_SUID,
	},
	{
		XmcdNsavErrOpenMsg, XmcdCSavErrOpenMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_saverr_open), XmRImmediate,
		(XtPointer) STR_SAVERR_OPEN,
	},
	{
		XmcdNsavErrCloseMsg, XmcdCSavErrCloseMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_saverr_close), XmRImmediate,
		(XtPointer) STR_SAVERR_CLOSE,
	},
	{
		XmcdNsavErrWriteMsg, XmcdCSavErrWriteMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_saverr_write), XmRImmediate,
		(XtPointer) STR_SAVERR_WRITE,
	},
	{
		XmcdNsavErrKilledMsg, XmcdCSavErrKilledMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_saverr_killed), XmRImmediate,
		(XtPointer) STR_SAVERR_KILLED,
	},
	{
		XmcdNlnkErrForkMsg, XmcdCLnkErrForkMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_lnkerr_fork), XmRImmediate,
		(XtPointer) STR_LNKERR_FORK,
	},
	{
		XmcdNlnkErrSuidMsg, XmcdCLnkErrSuidMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_lnkerr_suid), XmRImmediate,
		(XtPointer) STR_LNKERR_SUID,
	},
	{
		XmcdNlnkErrLinkMsg, XmcdCLnkErrLinkMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_lnkerr_link), XmRImmediate,
		(XtPointer) STR_LNKERR_LINK,
	},
	{
		XmcdNlnkErrKilledMsg, XmcdCLnkErrKilledMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_lnkerr_killed), XmRImmediate,
		(XtPointer) STR_LNKERR_KILLED,
	},
	{
		XmcdNsearchDbMsg, XmcdCSearchDbMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_srchdb), XmRImmediate,
		(XtPointer) STR_SRCHDB,
	},
	{
		XmcdNremoteMatchMsg, XmcdCRemoteMatchMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_rmtmatch), XmRImmediate,
		(XtPointer) STR_RMTMATCH,
	},
	{
		XmcdNchangeSaveMsg, XmcdCChangeSaveMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_chgsave), XmRImmediate,
		(XtPointer) STR_CHGSAVE,
	},
	{
		XmcdNdevlistUndefMsg, XmcdCDevlistUndefMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_devlist_undef), XmRImmediate,
		(XtPointer) STR_DEVLIST_UNDEF,
	},
	{
		XmcdNdevlistCountMsg, XmcdCDevlistCountMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_devlist_count), XmRImmediate,
		(XtPointer) STR_DEVLIST_COUNT,
	},
	{
		XmcdNchangerInitErrMsg, XmcdCChangerInitErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_medchg_noinit), XmRImmediate,
		(XtPointer) STR_MEDCHG_NOINIT,
	},
	{
		XmcdNproxyConfigErrMsg, XmcdCProxyConfigErrMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_proxyerr), XmRImmediate,
		(XtPointer) STR_PROXYERR,
	},
	{
		XmcdNproxyAuthFailMsg, XmcdCProxyAuthFailMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_authfail), XmRImmediate,
		(XtPointer) STR_AUTHFAIL,
	},
	{
		XmcdNbrowserStartMsg, XmcdCBrowserStartMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_browser_start), XmRImmediate,
		(XtPointer) STR_BROWSER_START,
	},
	{
		XmcdNbrowserFailMsg, XmcdCBrowserFailMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_browser_fail), XmRImmediate,
		(XtPointer) STR_BROWSER_FAIL,
	},
	{
		XmcdNappDefFileMsg, XmcdCAppDefFileMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_appdef), XmRImmediate,
		(XtPointer) STR_APPDEF,
	},
	{
		XmcdNkpModeDisableMsg, XmcdCKpModeDisableMsg,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, str_kpmodedsbl), XmRImmediate,
		(XtPointer) STR_KPMODEDSBL,
	},

	/* Short-cut key definitions */
	{
		XmcdNmodeKey, XmcdCModeKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, mode_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNlockKey, XmcdCLockKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, lock_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNrepeatKey, XmcdCRepeatKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, repeat_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNshuffleKey, XmcdCShuffleKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, shuffle_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNejectKey, XmcdCEjectKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, eject_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNquitKey, XmcdCQuitKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, quit_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNdbprogKey, XmcdCDbprogKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, dbprog_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNhelpKey, XmcdCHelpKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, help_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNoptionsKey, XmcdCOptionsKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, options_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNtimeKey, XmcdCTimeKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, time_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNabKey, XmcdCAbKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, ab_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNsampleKey, XmcdCSampleKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, sample_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadKey, XmcdCKeypadKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNplayPauseKey, XmcdCPlayPauseKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, playpause_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNstopKey, XmcdCStopKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, stop_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNprevDiscKey, XmcdCPrevDiscKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, prevdisc_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNnextDiscKey, XmcdCNextDiscKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, nextdisc_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNprevTrackKey, XmcdCPrevTrackKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, prevtrk_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNnextTrackKey, XmcdCNextTrackKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, nexttrk_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNprevIndexKey, XmcdCPrevIndexKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, previdx_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNnextIndexKey, XmcdCNextIndexKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, nextidx_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNrewKey, XmcdCRewKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, rew_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNffKey, XmcdCFfKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, ff_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey0, XmcdCKeypadNumKey0,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad0_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey1, XmcdCKeypadNumKey1,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad1_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey2, XmcdCKeypadNumKey2,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad2_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey3, XmcdCKeypadNumKey3,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad3_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey4, XmcdCKeypadNumKey4,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad4_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey5, XmcdCKeypadNumKey5,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad5_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey6, XmcdCKeypadNumKey6,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad6_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey7, XmcdCKeypadNumKey7,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad7_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey8, XmcdCKeypadNumKey8,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad8_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadNumKey9, XmcdCKeypadNumKey9,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypad9_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadClearKey, XmcdCKeypadClearKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypadclear_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadEnterKey, XmcdCKeypadEnterKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypadenter_key), XmRImmediate,
		(XtPointer) "",
	},
	{
		XmcdNkeypadCancelKey, XmcdCKeypadCancelKey,
		XmRString, sizeof(String),
		XtOffsetOf(appdata_t, keypadcancel_key), XmRImmediate,
		(XtPointer) "",
	},
};


STATIC XrmOptionDescRec	options[] = {
	{ "-dev",	"*device",	XrmoptionSepArg,	NULL },
	{ "-debug",	"*debugMode",	XrmoptionNoArg,		"True" },
#if defined(SVR4) && (defined(sun) || defined(__sun__))
	/* Solaris 2 volume manager auto-startup support */
	{ "-c",		"*device",	XrmoptionSepArg,	NULL },
	{ "-X",		"*exitOnEject",	XrmoptionNoArg,		"True" },
	{ "-o",		"",		XrmoptionNoArg,		"False" },
#endif
};

#endif	/* __RESOURCE_H__ */

