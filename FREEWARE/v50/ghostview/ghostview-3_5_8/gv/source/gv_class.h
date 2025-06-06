"GV.pageMedia:		automatic",
"GV.orientation:		automatic",
"GV.fallbackOrientation:	portrait",
"GV.swapLandscape:	False",
"GV.autoCenter:		True",
"GV.antialias:		False",
"GV.respectDSC:		True",
"GV.ignoreEOF:		True",
"GV.confirmPrint:	True",
"GV.reverseScrolling:	False",
"GV.scrollingEyeGuide:	True",
"GV.autoResize:		True",
"GV.maximumWidth:	screen-20",
"GV.maximumHeight:	screen-44",
"GV.minimumWidth:	400",
"GV.minimumHeight:	430",
"GV.confirmQuit:		1",
"GV.watchFile:		False",
"GV.watchFileFrequency:	1000",
"GV.showTitle:		True",
"GV.miscMenuEntries:	redisplay	\\n\
			# update	\\n\
			stop		\\n\
			line		\\n\
			toggle_current	\\n\
			toggle_even	\\n\
			toggle_odd	\\n\
			unmark		\\n\
			line		\\n\
			print_all	\\n\
			print_marked	\\n\
			save_all	\\n\
			save_marked",
"GV.scale:		0",
"GV.scaleBase:		1",
"GV.scales:		Natural	size,	1.000,	screen	\\n\
			Pixel based,	1.000,	pixel	\\n\
			0.100,		0.100		\\n\
			0.125,		0.125		\\n\
			0.250,		0.250		\\n\
			0.500,		0.500		\\n\
			0.707,		0.707		\\n\
			1.000,		1.000		\\n\
			1.414,		1.414		\\n\
			2.000,		2.000		\\n\
			4.000,		4.000		\\n\
			8.000,		8.000		\\n\
			10.00,		10.00",
"GV.medias:		Letter,		612	792	\\n\
			# LetterSmall,	612	792	\\n\
			Legal,		612	1008	\\n\
			Statement,	396	612	\\n\
			Tabloid,		792	1224	\\n\
			Ledger,		1224	792	\\n\
			Folio,		612	936	\\n\
			Quarto,		610	780	\\n\
			# 7x9,		504	648	\\n\
			# 9x11,		648	792	\\n\
			# 9x12,		648	864	\\n\
			# 10x13,		720	936	\\n\
			10x14,		720	1008	\\n\
			Executive,	540	720	\\n\
			# A0,		2384	3370	\\n\
			# A1,		1684	2384	\\n\
			# A2,		1191	1684	\\n\
			A3,		842	1191	\\n\
			A4,		595	842	\\n\
			# A4Small,	595	842	\\n\
			A5,		420	595	\\n\
			# A6,		297	420	\\n\
			# A7,		210	297	\\n\
			# A8,		148	210	\\n\
			# A9,		105	148	\\n\
			# A10,		73	105	\\n\
			# B0,		2920	4127	\\n\
			# B1,		2064	2920	\\n\
			# B2,		1460	2064	\\n\
			# B3,		1032	1460	\\n\
			B4,		729	1032	\\n\
			B5,		516	729	\\n\
			# B6,		363	516	\\n\
			# B7,		258	363	\\n\
			# B8,		181	258	\\n\
			# B9,		127	181	\\n\
			# B10,		91	127	\\n\
			# ISOB0,		2835	4008	\\n\
			# ISOB1,		2004	2835	\\n\
			# ISOB2,		1417	2004	\\n\
			# ISOB3,		1001	1417	\\n\
			# ISOB4,		709	1001	\\n\
			# ISOB5,		499	709	\\n\
			# ISOB6,		354	499	\\n\
			# ISOB7,		249	354	\\n\
			# ISOB8,		176	249	\\n\
			# ISOB9,		125	176	\\n\
			# ISOB10,	88	125	\\n\
			# C0,		2599	3676	\\n\
			# C1,		1837	2599	\\n\
			# C2,		1298	1837	\\n\
			# C3,		918	1296	\\n\
			# C4,		649	918	\\n\
			# C5,		459	649	\\n\
			# C6,		323	459	\\n\
			# C7,		230	323	\\n\
			# DL,		312	624",
"GV.magMenu:		2,	2	\\n\
			4,	4	\\n\
			8,	8	\\n\
			16,	16	\\n\
			32,	32	\\n\
			64,	64",
"GV*Ghostview.background:		white",
"GV*Ghostview.foreground:		black",
"GV.gsInterpreter:	gs",
"GV.gsCmdScanPDF:	gs \"-dNODISPLAY\" \"-dQUIET\" \"-sPDFname\"=%s \"-sDSCname\"=%s pdf2dsc.ps -c quit",
"GV.gsCmdConvPDF:	gs \"-dNODISPLAY\" \"-dQUIET\"  \"-dNOPAUSE\" \"-sPSFile\"=%s %s -c quit",
"GV.gsX11Device:	\"-sDEVICE=x11\"",
"GV.gsX11AlphaDevice:\"-dNOPLATFONTS\" \"-sDEVICE=x11alpha\"",
"GV.gsSafer:	True",
"GV.gsQuiet:	True",
"GV.gsArguments:",
"GV.printCommand:	Print /NoIdent /Delete /Queue = HP_417",
"GV.uncompressCommand:gzip -d -c %s > %s",
"GV.scratchDir:		SYS$SCRATCH:",
"GV.defaultSaveDir:	SYS$SCRATCH:",
"GV.fallbackPageMedia:	letter",
"GV.useBackingPixmap:	True",
"GV*fileSelPopup*fileSel*pathtext*font:	-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*fileSelPopup*fileSel.minimumWidth:	480",
"GV*dirs:		Home\\n\
			Tmp",
"GV*filter:		",
"GV*filters:		None\\n\
			*.*ps* *.pdf*\\n\
			*.*ps*\\n\
			*.pdf*",
"GV*font:			-*-Helvetica-Medium-R-Normal--*-140-*-*-P-*-ISO8859-1",
"GV*SmeBSB.font:			-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*Button.font:			-*-Helvetica-Medium-R-Normal--*-140-*-*-P-*-ISO8859-1",
"GV.?.processButton.font:	-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV.?.Label*font:		-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV.?.Button*font:		-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV.?.locator*font:		-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV.?.dateButton*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV.?.titleButton*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV.?.prevPage*font:		-*-Helvetica-Medium-R-Normal--*-140-*-*-P-*-ISO8859-1",
"GV.?.nextPage*font:		-*-Helvetica-Medium-R-Normal--*-140-*-*-P-*-ISO8859-1",
"GV*newtoc.font:			-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*fileSel*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*fileSel*button1.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*fileSel*button2.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*fileSel*button3.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*fileSel*button4.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*fileSel*pathtext*font:	-*-Helvetica-Medium-R-Normal--*-140-*-*-P-*-ISO8859-1",
"GV*optionsetupPopup*Label*font:	-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optionsetupPopup*confirmButton.font:-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optionsetupPopup*Switch*font:-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optionsetupPopup*font:	-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*optionsetupPopup*Button.font:-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*optiongvPopup*Label*font:	-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optiongvPopup*Switch*font:	-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optiongvPopup*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*optiongvPopup*Button.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*optionfsPopup*Label*font:	-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optionfsPopup*Switch*font:	-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optionfsPopup*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*optionfsPopup*Button.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*optiongsPopup*Label*font:	-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optiongsPopup*Switch*font:	-*-Helvetica-Medium-R-Normal--*-100-*-*-P-*-ISO8859-1",
"GV*optiongsPopup*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*optiongsPopup*Button.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*dialogPopup*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*dialogPopup*Button.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*dialogPopup*message*font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*notePopup*Button.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*notePopup*message*font:	-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*confirmPopup*font:		-*-Helvetica-Medium-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*confirmPopup*Button.font:	-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*versionPopup*Label*font:	-*-Courier-Medium-R-Normal--*-100-*-*-M-*-ISO8859-1",
"GV*versionPopup*versionTexta*font:-*-Helvetica-Bold-R-Normal--*-180-*-*-P-*-ISO8859-1",
"GV*versionPopup*versionTextb*font:-*-Helvetica-Bold-R-Normal--*-120-*-*-P-*-ISO8859-1",
"GV*infoPopup*Text*font:		-*-Courier-Medium-R-Normal--*-100-*-*-M-*-ISO8859-1",
"GV*highlightThickness:			1",
"GV*ShapeStyle:				Rectangle",
"GV*beNiceToColormap:			False",
"GV*input:				True",
"GV*initialResourcesPersistent:		False",
"GV.version:				gv 3.5.8",
"GV*background:				gray71",
"GV*foreground:				black",
"GV*Text*background:			#D3B5B5",
"GV*SimpleMenu*background:		AntiqueWhite3",
"GV*locator*background:			gray65",
"GV*panner*foreground:			gray78",
"GV*Label.background:			AntiqueWhite3",
"GV*fileSel*Frame*background:		gray78",
"GV*fileSel*Frame*Text*background:	#D3B5B5",
"GV*optiongvPopup*Toggle*foreground:	gray65",
"GV*optiongsPopup*Toggle*foreground:	gray65",
"GV*optionsetupPopup*Toggle*foreground:	gray65",
"GV*infoPopup*Scrollbar.background:	gray71",
"GV*notePopup*message.background:	#D3B5B5",
"GV*confirmPopup*message3.background:	#D3B5B5",
"GV*confirmPopup*message1.background:	gray71",
"GV*Button.background:			AntiqueWhite3",
"GV*Button.highlightedBackground:	AntiqueWhite2",
"GV*Button.setBackground:		AntiqueWhite4",
"GV*Switch.background:			AntiqueWhite3",
"GV*Switch.highlightedBackground:	AntiqueWhite4",
"GV*Switch.setBackground:		AntiqueWhite2",
"GV.?.Button.highlightedBackground:	AntiqueWhite2",
"GV.?.Button.background:			AntiqueWhite3",
"GV.?.Button.setBackground:		AntiqueWhite4",
"GV*MButton*setBackground:		AntiqueWhite2",
"GV*MButton*background:			AntiqueWhite3",
"GV*Scrollbar.background:		gray78",
"GV*Scrollbar.pointerColor:		black",
"GV*Scrollbar.pointerColorBackground:	White",
"GV*Scrollbar.scrollbarBackground:	gray71",
"GV*Scrollbar.foreground:		gray78",
"GV*optionsetupPopup*Text*Scrollbar.background:gray71",
"GV*optionfsPopup*Text*Scrollbar.background:gray71",
"GV*optionfsPopup*Label.background:	gray71",
"GV*optiongsPopup*Label.background:	gray71",
"GV*optiongvPopup*Label.background:	gray71",
"GV*optionsetupPopup*Label.background:	gray71",
"GV*newtocFrame*background:		gray78",
"GV*newtocScroll.background:		gray78",
"GV*Vlist.markBackground:		red",
"GV*Vlist.selectedBackground:		#D3B5B5",
"GV*Vlist.highlightedBackground:		gray65",
"GV*shadowWidth:				2",
"GV*Label.shadowWidth:			1",
"GV*Label*topShadowContrast:		-40",
"GV*Label*bottomShadowContrast:		-20",
"GV*Scrollbar.TopShadowPixel:		gray90",
"GV*Scrollbar.BottomShadowPixel:		gray40",
"GV*newtoc.shadowWidth:			0",
"GV*newtocScroll.bottomShadowPixel:	gray40",
"GV*optionfsPopup*Label.shadowWidth:	0",
"GV*optiongsPopup*Label.shadowWidth:	0",
"GV*optiongvPopup*Label.shadowWidth:	0",
"GV*optionsetupPopup*Label.shadowWidth:	0",
"GV*confirmPopup*message1.shadowWidth:	0",
"GV*Frame.frameType:			sunken",
"GV*Frame.topShadowPixel:		gray85",
"GV*Frame.bottomShadowPixel:		gray40",
"GV*Frame.hSpace:			0",
"GV*Frame.vSpace:			0",
"GV*MButton*unsetFrameStyle:		chiseled",
"GV.?.Button.unsetFrameStyle:		chiseled",
"GV.?.Button.highlightedFrameStyle:	chiseled",
"GV*rescan.unsetFrameStyle:		chiseled",
"GV*filter.unsetFrameStyle:		chiseled",
"GV*borderWidth:				0",
"GV*processButton.borderWidth:		0",
"GV*page.borderWidth:			1",
"GV*SimpleMenu.borderWidth:		1",
"GV*infoPopup*Text.borderWidth:		1",
"GV*viewFrame.topShadowPixel:		gray78",
"GV*viewFrame.hSpace:			1",
"GV*viewFrame.vSpace:			1",
"GV*page.borderColor:			gray65",
"GV*viewClip.borderWidth:		1",
"GV*viewClip.borderColor:		gray71",
"GV*Scrollbar.thickness:			17",
"GV*Scrollbar.thumb:			None",
"GV*magMenu*VertSpace:			2",
"GV*magMenu*leftMargin:			15",
"GV*magMenu*rightMargin:			15",
"GV*magMenu*justify:			center",
"GV*fileSel.filterframe.background:	#D3B5B5",
"GV*fileSel.filterframe.hSpace:		4",
"GV*fileSel.filterframe*Text*Margin:	4",
"GV*fileSel.pathframe.background:	#D3B5B5",
"GV*fileSel.pathframe.hSpace:		4",
"GV*fileSel.pathframe*Text*Margin:	6",
"GV*fileSel*Vlist.selectedBackground:	gray58",
"GV*fileSel*Vlist.selectedGeometry:	3 0 -3 1",
"GV*fileSel*Vlist.highlightedGeometry:	3 0 -3 1",
"GV*fileSel*topframe.forcedWidth:	100",
"GV*fileSel*subframe.forcedWidth:	100",
"GV*fileSel*Scrollbar.showArrows:	False",
"GV*fileSel*Scrollbar.thumbTopIndent:	0",
"GV*fileSel*Scrollbar.width:		12",
"GV*fileSel*Scrollbar.shadowWidth:	1",
"GV*fileSel*Scrollbar.background:	gray78",
"GV*fileSel*Scrollbar.foreground:	gray71",
"GV*fileSel.minimumWidth:		400",
"GV*fileSel.height:			350",
"GV*fileSelPopup.minWidth:		350",
"GV*fileSelPopup.minHeight:		200",
"GV*fileSel.highlightPixel:		#D3B5B5",
"GV*zoomPopup.title:			Zoom",
"GV*zoomControl.maximumWidth:		600",
"GV*zoomControl.maximumHeight:		600",
"GV*versionPopup*Label.internalHeight:	6",
"GV*versionPopup*Label.internalWidth:	8",
"GV*infoPopup.title:			GhostScript Messages",
"GV*infoPopup.geometry:			+50+50",
"GV*dialogPopup.title:			Dialog",
"GV*dialogPopup*message.internalHeight:	2",
"GV*dialogPopup*message.internalWidth:	8",
"GV*dialogPopup*defaultDistance:		2",
"GV*notePopup.title:			Message",
"GV*notePopup*message.internalHeight:	6",
"GV*notePopup*message.internalWidth:	8",
"GV*confirmPopup.title:			Confirm",
"GV*confirmPopup*Label.internalHeight:	6",
"GV*confirmPopup*Label.internalWidth:	8",
"GV*optionfsPopup.title:			File Selection Options",
"GV*optionfsPopup*defaultDistance:	2",
"GV*optionfsPopup*Text*leftMargin:	8",
"GV*optionfsPopup*filters.Margin:	8",
"GV*optionfsPopup*dirs.Margin:		8",
"GV*optionfsPopup*filters.height:	120",
"GV*optionfsPopup*dirs.height:		120",
"GV*optiongsPopup.title:			Ghostscript Options",
"GV*optiongsPopup*defaultDistance:	2",
"GV*optiongsPopup*Text*Margin:		5",
"GV*optiongvPopup.title:			gv Options",
"GV*optionsetupPopup.title:		Setup Options",
"GV*optionsetupPopup*defaultDistance:	2",
"GV*optionsetupPopup*Text*leftMargin:	8",
"GV*optionsetupPopup*scales.Margin:	8",
"GV*optionsetupPopup*scalesFrame.forcedWidth:	230",
"GV*optionsetupPopup*mediasFrame.forcedWidth:	240",
"GV*optionsetupPopup*mediasFrame.forcedHeight:	400",
"GV*pannerFrame.frameType:		sunken",
"GV*pannerFrame.hSpace:			0",
"GV*pannerFrame.vSpace:			0",
"GV*panner.background:			gray65",
"GV*pannerFrame.shadowWidth:		1",
"GV*pannerFrame.topShadowPixel:		gray90",
"GV*pannerFrame.bottomShadowPixel:	gray40",
"GV*panner.width:			50",
"GV*panner.height:			50",
"GV*slider.background:			gray78",
"GV*slider.shadowWidth:			1",
"GV*slider.topShadowPixel:		gray90",
"GV*slider.bottomShadowPixel:		gray40",
"GV*stateButton*menu*leftMargin:		20",
"GV*orientationButton*menu*leftMargin:	20",
"GV*pagemediaButton*menu*leftMargin:	20",
"GV*scaleButton*menu*leftMargin:		20",
"GV.control.prevPage*internalHeight:	4",
"GV.control.nextPage*internalHeight:	4",
"GV*locator*internalHeight:		3",
"GV.control.Button*internalWidth:	0",
"GV*cancel.Label:			Cancel",
"GV*dismiss.Label:			Dismiss",
"GV*apply.label:				Apply",
"GV*copyright.Label:			Copyright",
"GV*quit.Label:				Quit",
"GV*fileButton.Label:			File",
"GV*open.Label:				Open ...",
"GV*reopen.Label:			Reopen",
"GV*update.Label:			Update",
"GV*printAllPages.Label:			Print document ...",
"GV*printMarkedPages.Label:		Print marked pages ...",
"GV*saveAllPages.Label:			Save document ...",
"GV*saveMarkedPages.Label:		Save marked pages ...",
"GV*stateButton.Label:			State",
"GV*stop.Label:				Stop interpreter",
"GV*dsc.Label:				Respect document structure",
"GV*eof.Label:				Ignore EOF comments",
"GV*antialias.Label:			Antialias",
"GV*watchFile.Label:			Watch file",
"GV*showTitle.Label:			Show Title",
"GV*pixmap.Label:			Backing Pixmap",
"GV*size.Label:				Automatic resizing",
"GV*watch.Label:				Watch File",
"GV*optionsgv.Label:			gv Options ...",
"GV*optionsfs.Label:			File Selection Options ...",
"GV*optionsgs.Label:			Ghostscript Options ...",
"GV*optionssetup.Label:			Setup Options ...",
"GV*pageButton.Label:			Page",
"GV*next.Label:				Next",
"GV*show.Label:				Redisplay",
"GV*prev.Label:				Previous",
"GV*center.Label:			Center",
"GV*pageButton*current.Label:		Toggle current mark",
"GV*pageButton*even.Label:		Toggle even marks",
"GV*pageButton*odd.Label:		Toggle odd marks",
"GV*pageButton*unmark.Label:		Unmark all",
"GV*processButton.Label:			\\ processing",
"GV*mark.Label:				Mark",
"GV*prevPage.Label:			<<",
"GV*nextPage.Label:			>>",
"GV*unmark.Label:			Unmark",
"GV*automatic.Label:			Automatic",
"GV*portrait.Label:			Portrait",
"GV*landscape.Label:			Landscape",
"GV*upsidedown.Label:			Upside-Down",
"GV*seascape.Label:			Seascape",
"GV*swap.Label:				Swap Landscape",
"GV*toggleCurrent.label:",
"GV*toggleEven.label:",
"GV*toggleOdd.label:",
"GV*unmarkAll.label:",
"GV*printAll.label:			Print All ",
"GV*printMarked.label:			Print Marked",
"GV*saveAll.label:			Save All",
"GV*saveMarked.label:			Save Marked",
"GV*openFile.label:			Open",
"GV*redisplay.label:			Redisplay",
"GV*updateFile.label:			Update File",
"GV*checkFile.label:			Check File",
"GV*filters.label:			Filters:",
"GV*dirs.label:				Directories",
"GV*button1.label:			Cancel",
"GV*rescan.label:			Rescan Directory",
"GV*screenSizeLabel.label:		Screen Size (mm)",
"GV*versionPopup*versionDone*label:	Dismiss",
"GV*dialogPopup*cancel.label:		Cancel",
"GV*notePopup*done.label:		Okay",
"GV*confirmPopup*cancel.label:		Cancel",
"GV*confirmPopup*done.label:		Quit",
"GV*save.label:				Save",
"GV*optiongvPopup*mediaLabel.label:	\\ Paper Size \\ ",
"GV*optiongvPopup*fmediaLabel.label:	Fallback Paper Size",
"GV*optiongvPopup*orientLabel.label:	\\     Orientation    \\ ",
"GV*optiongvPopup*forientLabel.label:	Fallback Orientation",
"GV*optiongvPopup*magLabel.label:	Scale",
"GV*optiongvPopup*antialias.label:	Antialias",
"GV*optiongvPopup*respectDSC.label:	Respect DSC",
"GV*optiongvPopup*eof.label:		Ignore EOF",
"GV*optiongvPopup*autoResize.label:	Variable Size",
"GV*optiongvPopup*swapLandscape.label:	Swap Landscape",
"GV*optiongvPopup*scalesLabel.label:	\\   Scale  \\ ",
"GV*miscmenuLabel.label:			Misc Menu Entries",
"GV*magmenuLabel.label:			Mag Menu Entries",
"GV*mediasLabel.label:			Media Sizes",
"GV*confirmLabel.label:			Confirm Quit",
"GV*misc.label:				Miscellaneous",
"GV*scrolling.label:			Reverse Scrolling",
"GV*eyeGuide.label:			Scrolling Eye Guide",
"GV*confirmPrint.label:			Confirm Printing",
"GV*uncompressLabel.label:		Uncompress",
"GV*autoCenter.label:			Auto Center",
"GV*printCommandLabel.label:		Print Command",
"GV*scratchDirLabel.label:		Scratch Directory",
"GV*saveDirLabel.label:			Save Directory",
"GV*scalesLabel.label:			Scales",
"GV*scaleBaseLabel.label:		Scale Base",
"GV*safer.label:		Safer",
"GV*quiet.label:		Quiet",
"GV*defaults.label:	System Defaults",
"GV*scanLabel.label:	Scan PDF",
"GV*convLabel.label:	Convert PDF",
"GV*gsLabel.label:	Interpreter",
"GV*x11devLabel.label:	Device",
"GV*x11alphadevLabel.label:	Antialias Device",
"GV*argumentsLabel.label:	Arguments",
"GV*filtersLabel.label:			File Selection Filters",
"GV*dirsLabel.label:			File Selection Directories",
"GV*filterLabel.label:			Default Filter",
"GV*miscMenu.update.label:		Update File",
"GV*miscMenu.redisplay.label:		Redisplay",
"GV*miscMenu.stop.label:			Stop Interpreter",
"GV*miscMenu.toggle_current.label:	Toggle Current",
"GV*miscMenu.toggle_odd.label:		Toggle Odd",
"GV*miscMenu.toggle_even.label:		Toggle Even",
"GV*miscMenu.unmark.label:		Unmark All",
"GV*miscMenu.print_all.label:		Print All",
"GV*miscMenu.print_marked.label:		Print Marked",
"GV*miscMenu.save_all.label:		Save All",
"GV*miscMenu.save_marked.label:		Save Marked",
"GV.control.baseTranslations:#replace		\\n\
 <Key>A:		GV_Antialias()		\\n\
 <Key>O:		GV_Open()		\\n\
 <Key>Q:		GV_Quit()		\\n\
 <Key>R:		GV_Resizing()		\\n\
 <Key>I:		GV_HandleDSC()		\\n\
 <Key>W:		GV_WatchFile()		\\n\
 ~s ~c <Key>s:		GV_Save(marked)		\\n\
 <Key>s:		GV_Save(all)		\\n\
 ~s ~c <Key>p:		GV_Print(marked)	\\n\
 <Key>p:		GV_Print(all)		\\n\
 ~c ~s <Key>period:	GV_Page(redisplay)	\\n\
 <Key>period:		GV_Reopen()		\\n\
 c <Key>L:		GV_Page(redisplay)	\\n\
 <Key>M:		GV_SetPageMark(current,mark)\\n\
 <Key>N:		GV_SetPageMark(current,unmark)\\n\
						\
 <Key>7:		GV_SetOrientation(portrait)\\n\
 <Key>8:		GV_SetOrientation(landscape)\\n\
 <Key>9:		GV_SetOrientation(upside-down)\\n\
 <Key>0:		GV_SetOrientation(seascape)\\n\
						\
 ~c ~s <Key>1:		GV_SetScale(1)		\\n\
 ~c ~s <Key>2:		GV_SetScale(2)		\\n\
 ~c ~s <Key>3:		GV_SetScale(3)		\\n\
 ~c ~s <Key>4:		GV_SetScale(4)		\\n\
 ~c ~s <Key>5:		GV_SetScale(5)		\\n\
 ~c ~s <Key>6:		GV_SetScale(6)		\\n\
 <Key>6:		GV_SetScale(-6)		\\n\
 <Key>5:		GV_SetScale(-5)		\\n\
 <Key>4:		GV_SetScale(-4)		\\n\
 <Key>3:		GV_SetScale(-3)		\\n\
 <Key>2:		GV_SetScale(-2)		\\n\
 <Key>1:		GV_SetScale(-1)		\\n\
 <Key>grave:		GV_SetScale(0)		\\n\
 <Key>asciicircum:	GV_SetScale(0)		\\n\
 <Key>-:		GV_SetScale(-)		\\n\
 <Key>=:		GV_SetScale(+)		\\n\
 <Key>+:		GV_SetScale(+)		\\n\
						\
 ~c ~s <Key>Up:		GV_Page(up)		\\n\
 ~c ~s <Key>Right:	GV_Page(right)		\\n\
 ~c ~s <Key>Down:	GV_Page(down)		\\n\
 ~c ~s <Key>Left:	GV_Page(left)		\\n\
 ~c  s <Key>Up:		GV_Page(up,left-bottom,!page-1,bottom-rightedge)\\n\
 ~c  s <Key>Down:	GV_Page(down,right-top,!page+1,top-leftedge)\\n\
 ~c  s <Key>Left:	GV_Page(left,up-rightedge,!page-1,bottom-rightedge)\\n\
 ~c  s <Key>Right:	GV_Page(right,down-leftedge,!page+1,top-leftedge)\\n\
  c ~s <Key>Up:		GV_Page(!page-1,top-leftedge)\\n\
  c ~s <Key>Down:	GV_Page(!page+1,top-leftedge)\\n\
  c ~s <Key>Left:	GV_Page(!page-5,top-leftedge)\\n\
  c ~s <Key>Right:	GV_Page(!page+5,top-leftedge)\\n\
						\
 ~c ~s <Key>d:		GV_Page(up)		\\n\
 ~c ~s <Key>x:		GV_Page(down)		\\n\
 ~c ~s <Key>z:		GV_Page(left)		\\n\
 ~c ~s <Key>y:		GV_Page(left)		\\n\
 ~c ~s <Key>c:		GV_Page(right)		\\n\
 ~c  s <Key>d:		GV_Page(up,left-bottom,!page-1,bottom-rightedge)\\n\
 ~c  s <Key>x:		GV_Page(down,right-top,!page+1,top-leftedge)\\n\
 ~c  s <Key>z:		GV_Page(left,up-rightedge,!page-1,bottom-rightedge)\\n\
 ~c  s <Key>y:		GV_Page(left,up-rightedge,!page-1,bottom-rightedge)\\n\
 ~c  s <Key>c:		GV_Page(right,down-leftedge,!page+1,top-leftedge)\\n\
  c ~s <Key>d:		GV_Page(!page-1,top-leftedge)\\n\
  c ~s <Key>x:		GV_Page(!page+1,top-leftedge)\\n\
  c ~s <Key>z:		GV_Page(!page-5,top-leftedge)\\n\
  c ~s <Key>y:		GV_Page(!page-5,top-leftedge)\\n\
  c ~s <Key>c:		GV_Page(!page+5,top-leftedge)\\n\
 <Key>V:		GV_Page(center)		\\n\
 ~c ~s<Key>space:	GV_Page(down,right-top,!page+1,top-leftedge)\\n\
 <Key>space:		GV_Page(up,left-bottom,!page-1,bottom-rightedge)\\n\
						\
 <Key>BackSpace:	GV_Page(up,left-bottom,!page-1,bottom-rightedge)\\n\
 <Key>Insert:		GV_Page(page-5)		\\n\
 <Key>Delete:		GV_Page(page+5)		\\n\
 <Key>apLineDel:	GV_Page(page+5)		\\n\
 <Key>Home:		GV_Page(page=0)		\\n\
 <Key>End:		GV_Page(page=99999)	\\n\
 <Key>Prior:		GV_Page(page-1)		\\n\
 <Key>Next:		GV_Page(page+1)		\\n\
						\
 <Key>KP_0:		GV_Page(highlight=0)	\\n\
 <Key>KP_1:		GV_Page(highlight=1)	\\n\
 <Key>KP_2:		GV_Page(highlight=2)	\\n\
 <Key>KP_3:		GV_Page(highlight=3)	\\n\
 <Key>KP_4:		GV_Page(highlight=4)	\\n\
 <Key>KP_5:		GV_Page(highlight=5)	\\n\
 <Key>KP_6:		GV_Page(highlight=6)	\\n\
 <Key>KP_7:		GV_Page(highlight=7)	\\n\
 <Key>KP_8:		GV_Page(highlight=8)	\\n\
 <Key>KP_9:		GV_Page(highlight=9)	\\n\
 <Key>KP_Subtract:	GV_Page(highlight-1)	\\n\
 <Key>KP_Add:		GV_Page(highlight+1)	\\n\
 <Key>KP_Enter:		GV_Page(page=highlighted)\\n\
						\
 ~c ~s <Key>Return: 	GV_Page(page+1)		\\n\
 <Key>Return: 	GV_Page(page-1)			\\n\
 <Key>B:		GV_Page(page-1)		\\n\
 <Key>F:		GV_Page(page+1)",
"GV.zoomPopup.Aaa.baseTranslations:#replace	\\n\
 <Key>Q:		GV_DismissPopup()	\\n\
						\
 ~c ~s <Key>Up:		GV_Page(up)		\\n\
 ~c ~s <Key>Right:	GV_Page(right)		\\n\
 ~c ~s <Key>Down:	GV_Page(down)		\\n\
 ~c ~s <Key>Left:	GV_Page(left)		\\n\
 ~c  s <Key>Up:		GV_Page(up,left-bottom)\\n\
 ~c  s <Key>Down:	GV_Page(down,right-top)\\n\
 ~c  s <Key>Left:	GV_Page(left,up-rightedge)\\n\
 ~c  s <Key>Right:	GV_Page(right,down-leftedge)\\n\
						\
 ~c ~s <Key>d:		GV_Page(up)		\\n\
 ~c ~s <Key>x:		GV_Page(down)		\\n\
 ~c ~s <Key>z:		GV_Page(left)		\\n\
 ~c ~s <Key>y:		GV_Page(left)		\\n\
 ~c ~s <Key>c:		GV_Page(right)		\\n\
 ~c  s <Key>d:		GV_Page(up,left-bottom)\\n\
 ~c  s <Key>x:		GV_Page(down,right-top)\\n\
 ~c  s <Key>z:		GV_Page(left,up-rightedge)\\n\
 ~c  s <Key>y:		GV_Page(left,up-rightedge)\\n\
 ~c  s <Key>c:		GV_Page(right,down-leftedge)\\n\
 <Key>V:		GV_Page(center)		\\n\
 ~c ~s<Key>space:	GV_Page(down,right-top)\\n\
 <Key>space:		GV_Page(up,left-bottom)\\n\
						\
 <Key>BackSpace:	GV_Page(up,left-bottom)",
"GV*newtoc.baseTranslations: #replace		\\n\
 <EnterWindow>:		GV_Toc(highlight)	\\n\
 <LeaveWindow>:		GV_Toc(unhighlight)	\\n\
 <Btn1Down>:		GV_Toc(scrollon)	\\n\
 <Btn1Motion>:		GV_Toc(scroll)		\\n\
 <Btn1Up>:		GV_Toc(select)		\
			GV_Toc(scrolloff)	\
			GV_Toc(highlight)	\\n\
 <Btn2Down>:		GV_Toc(toggleon)	\\n\
 <Btn2Motion>:		GV_Toc(toggleextend)	\
			GV_Toc(highlight)	\\n\
 <Btn2Up>:		GV_Toc(toggleoff)	\\n\
 <Btn3Down>:		GV_Toc(toggleon)	\\n\
 <Btn3Motion>:		GV_Toc(toggleextend)	\
			GV_Toc(highlight)	\\n\
 <Btn3Up>:		GV_Toc(toggleoff)	\\n\
 <MotionNotify>:	GV_Toc(highlight)",
"GV*SimpleMenu.baseTranslations:#replace\\n\
 <Enter>:	highlight()\\n\
 <Leave>:	unhighlight()\\n\
 <BtnMotion>:	highlight()\\n\
 <BtnUp>:	MenuPopdown()\
		GV_MenuPopdown()\
		notify()\
		unhighlight()",
"GV*Ghostview.baseTranslations:		#replace		\\n\
~Button2 ~Button3	<Btn1Down>:	cursor(scroll)		\
					GV_MovePage(start)	\
					GV_EraseLocator()	\\n\
~Button2 ~Button3	<Btn1Motion>:	GV_MovePage(move,1.0,0)	\\n\
			<Btn1Up>:	cursor(normal)		\
					GV_MovePage(stop)	\
					GV_OtherPage()		\
					notify()		\\n\
~Button1 ~Button3 s	<Btn2Down>:	cursor(scroll)		\
					GV_MovePage(start)	\
					GV_EraseLocator()	\
					GV_MiscMenu(init)	\\n\
~Button1 ~Button3 s	<Btn2Motion>:	GV_MovePage(move,0,1.0)	\\n\
		  s	<Btn2Up>:	cursor(normal)		\
					GV_MiscMenu(popup)	\
					notify()		\\n\
~Button1 ~Button2 s	<Btn3Down>:	GV_MagMenu(begin)	\\n\
~Button1 ~Button2 s	<Btn3Motion>:	GV_MagMenu(extend)	\\n\
~Button1 ~Button2 s	<Btn3Up>:	GV_MagMenu(choose)	\\n\
~Button1 ~Button3	<Btn2Down>:	GV_MagMenu(begin)	\\n\
 Button2		<Btn1Down>:	GV_MagMenu(reset)	\\n\
 Button2		<Btn3Down>:	GV_MagMenu(reset)	\\n\
~Button1 ~Button3	<Btn2Motion>:	GV_MagMenu(extend)	\\n\
~Button1 ~Button3	<Btn2Up>:	GV_MagMenu(choose)	\\n\
~Button1 ~Button2	<Btn3Down>:	cursor(scroll)		\
					GV_MovePage(start)	\
					GV_EraseLocator()	\
					GV_MiscMenu(init)	\\n\
	  Button3	<Btn1Down>:	GV_MiscMenu(reset)	\\n\
	  Button3	<Btn2Down>:	GV_MiscMenu(reset)	\\n\
~Button1 ~Button2	<Btn3Motion>:	GV_MovePage(move,0,1.0)	\\n\
			<Btn3Up>:	cursor(normal)		\
					GV_MiscMenu(popup)	\
					notify()		\\n\
 Button1 ~Button3 	<Btn2Down>:  	GV_Page(redisplay)	\\n\
 Button1 ~Button2 	<Btn3Down>:  	GV_Page(redisplay)	\\n\
			<Message>:	message() 		\\n\
			<EnterWindow>:	notify()		\\n\
			<LeaveWindow>:	GV_EraseLocator()	\\n\
			<MotionNotify>:	notify()",
"GV.baseTranslations:	#replace		\\n\
 <MapNotify>:		GV_CheckFile(date)	\\n\
 <ConfigureNotify>:	GV_ConfigureNotify()	\\n\
 <Message>WM_PROTOCOLS:	GV_DeleteWindow()",
"GV.control.titleButton.baseTranslations:#replace\\n\
 <Btn1Down>:		PopupMenu(1)",
"GV.control.dateButton.baseTranslations:#replace	\\n\
 <Btn1Down>:		PopupMenu(1)",
"GV.control.prevPage.baseTranslations: #replace\\n\
 <EnterWindow>:	highlight()	\\n\
 <LeaveWindow>:	reset()		\\n\
 <BtnDown>:	set()		\\n\
 <Btn1Up>:	notify() unset() GV_Page(page-1)\\n\
 <Btn2Up>:	notify() unset() GV_Page(page-5)\\n\
 <Btn3Up>:	notify() unset() GV_Page(page-10)",
"GV.control.nextPage.baseTranslations: #replace\\n\
 <EnterWindow>:	highlight()	\\n\
 <LeaveWindow>:	reset()		\\n\
 <BtnDown>:	set()		\\n\
 <Btn1Up>:	notify() unset() GV_Page(page+1)\\n\
 <Btn2Up>:	notify() unset() GV_Page(page+5)\\n\
 <Btn3Up>:	notify() unset() GV_Page(page+10)",
"GV*TransientShell*optionControl*Button.baseTranslations: #replace\\n\
 <EnterWindow>:	highlight()	\\n\
 <LeaveWindow>:	reset()		\\n\
 <Btn1Down>:	set()		\\n\
 <Btn1Up>:	notify() unset()",
"GV*optiongvPopup*Toggle.baseTranslations: #replace\\n\
 <EnterWindow>:					\\n\
 <LeaveWindow>:					\\n\
 <Btn1Down>,<Btn1Up>:	toggle() notify()",
"GV*optiongsPopup*Toggle.baseTranslations: #replace\\n\
 <EnterWindow>:					\\n\
 <LeaveWindow>:					\\n\
 <Btn1Down>,<Btn1Up>:	toggle() notify()",
"GV*panner.baseTranslations: #replace		\\n\
	<Btn1Down>:	GV_Panner(on)		\\n\
	<Btn1Motion>:	GV_Panner(move)		\\n\
	<Btn1Up>:	GV_Panner(off)		\\n\
 s	<Btn2Down>:	GV_Page(page+1)		\\n\
 s	<Btn3Down>:	GV_Page(page-1)		\\n\
	<Btn2Down>:	GV_Page(page-1)		\\n\
	<Btn3Down>:	GV_Page(page+1)",
"GV.TransientShell.baseTranslations:#replace\\n\
 <Message>WM_PROTOCOLS:	GV_DeleteWindow()",
"GV.TopLevelShell.baseTranslations:#replace\\n\
 <Message>WM_PROTOCOLS:	GV_DeleteWindow()",
"GV.TransientShell.Aaa.baseTranslations:#replace\\n\
 <Key>Q:		GV_DismissPopup()",
"GV.TopLevelShell.Aaa.baseTranslations:#replace\\n\
 <Key>Q:		GV_DismissPopup()",
"GV.TopLevelShell*Button.accelerators:#override\\n\
 <Key>Return:	set() notify() unset()",
"GV.TopLevelShell*Button.baseTranslations: #replace\\n\
 <Key>Return:	set() notify() unset()\\n\
 <LeaveWindow>:	unset()		\\n\
 <Btn1Down>:	set()		\\n\
 <Btn1Up>:	notify() unset()",
"GV*TransientShell*Button.accelerators:#override\\n\
 <Key>Return:	set() notify() unset()",
"GV*TransientShell*Button.baseTranslations: #replace\\n\
 <Key>Return:	set() notify() unset()\\n\
 <LeaveWindow>:	unset()		\\n\
 <Btn1Down>:	set()		\\n\
 <Btn1Up>:	notify() unset()",
"GV*magMenu.baseTranslations:	#replace	\\n\
 <EnterWindow>:			highlight() 	\\n\
 <LeaveWindow>:			unhighlight()	\
				GV_MagMenu(reset)\\n\
 <MapNotify>:			GV_MagMenu(check)\\n\
 <BtnMotion>:			highlight() 	\\n\
 <MotionNotify>:		highlight() 	\\n\
 <BtnDown>:			GV_MagMenu(show)\
				MenuPopdown(magMenu)",
"				",
"GV*miscMenu.baseTranslations:	#replace	\\n\
 <EnterWindow>:			highlight() 	\\n\
 <LeaveWindow>:			unhighlight()	\
				GV_MiscMenu(reset)\\n\
 <MapNotify>:			GV_MiscMenu(check)\\n\
 <BtnMotion>:			highlight() 	\\n\
 <MotionNotify>:		highlight() 	\\n\
 <BtnUp>:			MenuPopdown(miscMenu)\
				notify()\
				unhighlight()\
				GV_MiscMenu(reset)",
"GV*viewControl.layout:\
|v{\
  4<+[1]>\
  |h{\
    4<+[1]>\
    page\
    4<+[1]>\
  }\
  4<+[1]>\
}",
"GV*control.layout:\
|v{\
  f=8\
  s=8\
  ss=4\
  sss=2\
  a=6\
  pbw=(10*(|width processButton))\
  pbss=($ss*($pbw/($pbw+1)))\
  $f\
  |h{\
    $f\
    |v{\
      fileButton<+[1]*>\
      $s\
      |h{\
        0<+[1]>\
        |v{\
          autoResize<+30*>\
          $a\
          |h{\
            1<+[1]>\
            locator\
            1<+[1]>\
          }\
          $a\
          openFile<+30*>\
          1\
          printAll<+30*>\
          1\
          printMarked<+30*>\
          1\
          saveAll<+30*>\
          1\
          saveMarked<+30*>\
          $a\
          |h{\
            0<+[1]>\
            pannerFrame\
            0<+[1]>\
          }\
          $a\
          |h{\
            prevPage<+30*>\
            1\
            nextPage<+30*>\
          }\
          1\
          redisplay<+30*>\
        }\
        0<+[1]>\
      }\
      $a\
      |h{\
        0<+[1]>\
        toggleOdd\
        1\
        toggleEven\
        1\
        toggleCurrent\
        1\
        unmarkAll\
        0<+[1]>\
      }\
      $a\
      |h{\
        0<+30>\
        newtocScroll<*+200[1]-[1]>\
        $ss\
        newtocFrame<-[2]*+200[1]-[1]>\
        0<+30>\
      }\
    }\
    $s\
    |v{\
      |h{\
        stateButton<-90%*>\
        $ss<-90%>\
        pageButton<-90%*>\
        $ss<-90%>\
        orientationButton<-90%*>\
        $ss<-90%>\
        scaleButton<-90%*>\
        $ss<-90%>\
        pagemediaButton<-90%*>\
        $pbss<-90%>\
        |v{\
          0<+[1]>\
          processButton<-90%*>\
          0<+[1]>\
        }\
        $ss<-90%>\
        |v{\
          0<+[1]>\
          |h{\
             titleButton<+5[1]-(100%)[1]*>\
             $ss<-90%>\
             dateButton<+[1]-(100%)[1]*>\
          }\
          0<+[1]>\
        }\
      }\
      $s\
      |h{\
         viewFrame<+9999-100%*+9999-100%>\
      }\
    }\
    $f\
  }\
  $f\
}",
"GV*newtocControl.layout:\
|v{\
  |h{\
    newtoc\
    0<+[1]>\
  }\
  0<+[1]>\
}",
"GV*zviewControl.layout:\
|v{\
  4<+[1]>\
  |h{\
    4<+[1]>\
    page<-100%*-100%>\
    4<+[1]>\
  }\
  4<+[1]>\
}",
"GV*zoomControl.layout:\
|h{ \
  s=8\
  $s\
  |v{ \
    $s\
    zviewFrame<+[2]-100%*+[2]-100%>\
    $s\
    dismiss<+[1]-100%*>\
    $s\
  }\
  $s\
}",
"GV*optiongvPopup*layout:\
|v{ \
  bs=8\
  s=4\
  ss=3\
  sss=2\
  $bs\
  |h{\
    $bs\
    |v{\
      orientLabel\
      $sss\
      orientButton\
      $sss\
      forientLabel\
      $sss\
      forientButton\
      $sss\
      scalesLabel\
      $sss\
      scalesButton\
      (2*$bs)\
      swapLandscape<+[1]*>\
      $sss\
      autoResize<+[1]*>\
      $sss\
      watch<+[1]*>\
    }\
    $ss\
    |v{\
      mediaLabel\
      $sss\
      mediaButton\
      $sss\
      fmediaLabel\
      $sss\
      fmediaButton\
      $sss\
      scaleBaseLabel\
      $sss\
      scaleBaseButton\
      (2*$bs)\
      respectDSC<+[1]*>\
      $sss\
      eof<+[1]*>\
      $sss\
      antialias<+[1]*>\
    }\
    $bs\
  }\
  (2*$bs)\
  |h{\
    (3*$bs)<-90%>\
    apply\
    $bs<+[1]-100%>\
    save\
    $bs<+[1]-100%>\
    dismiss\
    (3*$bs)<-90%>\
  }\
  $bs\
}",
"GV*optionsetupPopup*layout:\
|v{ \
  bs=8\
  s=4\
  ss=3\
  sss=2\
  $bs\
  |h{\
    $bs\
    |v{\
      confirmLabel<+[1]*>\
      $sss\
      confirmButton<+[1]*>\
      $sss\
      misc<+[1]*>\
      $sss\
      scrolling<+[1]*>\
      $sss\
      eyeGuide<+[1]*>\
      $sss\
      confirmPrint<+[1]*>\
      $sss\
      autoCenter<+[1]*>\
      $sss\
      showTitle<+[1]*>\
      $sss\
      pixmap<+[1]*>\
      $sss\
      magmenuLabel<+[1]*>\
      $sss\
      magmenuFrame<+[1]*+[1]>\
      $sss\
      miscmenuLabel<+[1]*>\
      $sss\
      miscmenuFrame<+[1]*+[1]>\
    }\
    $s\
    |v{\
      scalesLabel<+[2]-100%*>\
      $ss\
      scalesFrame<+[2]-100%*+[1]>\
      $s\
      |h{\
        |v{\
          $s\
          screenSizeLabel<+[1]*>\
          $bs\
          printCommandLabel<+[1]*>\
          $bs\
          uncompressLabel<+[1]*>\
        }\
        $s\
        |v{\
          screenSizeFrame<+[2]-100%*>\
          $s\
          printCommandFrame<+[2]-100%*>\
          $s\
          uncompressFrame<+[2]-100%*>\
        }\
      }\
    }\
    $s\
    |v{\
      mediasLabel<+[2]-100%*>\
      $ss\
      mediasFrame<+[2]-100%*+[1]>\
    }\
    $bs\
  }\
  (2*$bs)\
  |h{\
    (3*$bs)<+[1]-90%>\
    apply\
    $bs<+[1]-100%>\
    save\
    $bs<+[1]-100%>\
    dismiss\
    (3*$bs)<+[1]-90%>\
  }\
  $bs\
}",
"GV*optionfsPopup*layout:\
|v{ \
  bs=8\
  s=4\
  ss=3\
  sss=2\
  $bs\
  |h{\
    $bs\
    |v{\
      filtersLabel<+[2]-100%*>\
      $ss\
      filtersFrame<+[2]-100%*+[1]>\
    }\
    $bs\
    |v{\
      dirsLabel<+[2]-100%*>\
      $ss\
      dirsFrame<+[2]-100%*+[1]>\
    }\
    $bs\
  }\
  $bs\
  |h{\
    $bs\
    |v{\
      $s\
      filterLabel<+[1]*>\
      $bs\
      scratchDirLabel<+[1]*>\
      $bs\
      saveDirLabel<+[1]*>\
    }\
    $s\
    |v{\
      filterFrame<+[2]-100%*>\
      $s\
      scratchDirFrame<+[2]-100%*>\
      $s\
      saveDirFrame<+[2]-100%*>\
    }\
    $bs\
  }\
  (2*$bs)\
  |h{\
    (3*$bs)<-90%>\
    apply\
    $bs<+[1]-100%>\
    save\
    $bs<+[1]-100%>\
    dismiss\
    (3*$bs)<-90%>\
  }\
  $bs\
}",
"GV*optiongsPopup*layout:\
|v{\
  ls=15\
  bs=8\
  ss=3\
  s=4\
  sss=2\
  $bs\
  |h{\
    $bs\
    gsLabel\
    0<+[1]>\
  }\
  |h{\
    $bs\
    gsFrame<+[1]-95%*>\
    $ss\
    safer\
    $ss\
    quiet\
    $bs\
  }\
  $sss\
  |h{\
    $bs\
    argumentsLabel\
    0<+[1]>\
  }\
  |h{\
    $bs\
    argumentsFrame<+[1]-95%*>\
    $bs\
  }\
  $sss\
  |h{\
    $bs\
    |v{\
      |h{\
        x11devLabel\
        0<+[1]>\
      }\
      x11devFrame<+[1]-95%*>\
    }\
    $ss\
    |v{\
      |h{\
        x11alphadevLabel\
        0<+[1]>\
      }\
      x11alphadevFrame<+[1]-95%*>\
    }\
    $bs\
  }\
  $sss\
  |h{\
    $bs\
    scanLabel\
    0<+[1]>\
  }\
  |h{\
    $bs\
    scanFrame<+[1]-50%*+[1]>\
    $bs\
  }\
  $sss\
  |h{\
    $bs\
    convLabel<+50*>\
    0<+[1]>\
  }\
  |h{\
    $bs\
    convFrame<+[1]-50%*+[1]>\
    $bs\
  }\
  (2*$bs)\
  |h{\
    (4*$bs)<-90%>\
    apply\
    $bs<+[1]-100%>\
    save\
    $bs<+[1]-100%>\
    defaults\
    $bs<+[1]-100%>\
    dismiss\
    (4*$bs)<-90%>\
  }\
  $bs\
}",
"GV*notePopup*layout:\
|v{ \
  vs=8\
  $vs<+[1]>\
  |h{ \
    $vs<+[1]>\
    message<+[1]-100%*+[1]-100%>\
    $vs<+[1]>\
  }\
  $vs<+[1]>\
  |h{\
    $vs<+[1]>\
    done<-100%*>\
    $vs<+[1]>\
  }\
  $vs<+[1]>\
}",
"GV*dialogPopup*layout:\
|v{ \
  s=7\
  bs=14\
  $bs\
  |h{ \
    (2*$bs)\
    message<+[1]*>\
    (2*$bs)\
  }\
  $bs\
  |h{\
    $bs\
    dialog_textLabel\
    $s \
    dialog_textFrame<+[1]-100%*>\
    $bs\
  }\
  $bs\
  |h{\
    (2*$bs)<+[1]>\
    done\
    $bs<+[1]>\
    cancel\
    (2*$bs)<+[1]>\
  }\
  $bs\
}",
"GV*confirmPopup*layout:\
|v{ \
  h2 =(|height message2)\
  h3 =(|height message3)\
  s2 =(4*($h2/($h2+1)))\
  s3 =(8*($h3/($h3+1)))\
  vs=10\
  (2*$vs)\
  |h{\
    $vs\
    |v{\
      message2<+[1]*>\
      (2*$s2)\
      message3<+[1]*>\
      $s3\
    }\
    $vs\
  }\
  $vs\
  |h{\
    (2*$vs)<+[1]>\
    done\
    $vs<+[1]>\
    cancel\
    (2*$vs)<+[1]>\
  }\
  $vs\
}",
"GV*versionPopup*layout:\
|v{ \
  vs=8\
  (3*$vs)<+[1]-100%>\
  |h{ \
    $vs<+[1]>\
    |h{\
      |v{ \
        versionTexta\
        (|height versionTextb)\
        $vs\
      }\
      $vs\
      |v{ \
        0<+[1]>\
        versionTextb\
      }\
      $vs\
    }\
    $vs\
    $vs<+2[1]>\
    |v{ 0<+[1]> versionDone  0<+[1]> }\
    $vs<+[1]>\
  }\
  (3*$vs)\
  |h{ $vs<+[1]> license $vs <+[1]> }\
  (3*$vs)\
  |h{ $vs<+[1]> authora $vs<+[1]> }\
  $vs\
  |h{ $vs<+[1]> authorb $vs<+[1]> }\
  $vs\
  |h{ $vs<+[1]> authorc $vs <+[1]> }\
  (3*$vs)\
  |h{ $vs<+[1]> authord $vs <+[1]> }\
  (2*$vs)\
}",
"GV*fileSel*Clip*Aaa.layout:\
|v{\
  list<+[1]*>\
  0<+[1]>\
}",
"GV*fileSel.layout:\
|v{\
  bs  = 8\
  s   = 4\
  hs2 =(|height button2)\
  hs3 =(|height button3)\
  hs4 =(|height button4)\
  bs2 =($bs*($hs2/($hs2+1)))\
  bs3 =($bs*($hs3/($hs3+1)))\
  bs4 =($bs*($hs4/($hs4+1)))\
  $bs\
  |h{\
    $bs\
    pathframe<+[1]-95%*>\
    $bs\
  }\
  $bs\
  |h{\
    $bs\
    |v{\
      topframe<+200-95%*-90%>\
      2\
      subframe<+[1]-[1]*+[1]-[1]>\
    }\
    $s\
    |v{\
      |h{\
        |v{\
          |h{\
            rescan<+[1]-100%*>\
            $s\
            dirs<+[1]-100%*>\
          }\
          $s\
          |h{\
	    filters\
            $s\
            filterframe<+[1]-95%*>\
          }\
        }\
      }\
      $s\
      |h{\
        subscroll<*+[1]-[1]>\
        $s\
        curframe<+[1]-[1]*+[1]-[1]>\
        $s\
        curscroll<*+[1]-[1]>\
      }\
    }\
    $bs\
  }\
  $bs\
  |h{\
    0<+[1]>\
    (6*$bs)<-95%>\
    button2\
    0<+[1]>\
    $bs2<+($bs2)-90%>\
    button3\
    0<+[1]>\
    $bs3<+($bs3)-90%>\
    button4\
    0<+[1]>\
    $bs4<+($bs3)-90%>\
    button1\
    0<+[1]>\
    (6*$bs)<-95%>\
  }\
  $bs\
}",
