$!
$! RUNOFF.COM - prepare Nmail documentation
$!
$	set noon
$	wrout = "write sys$output"
$	cvtdoc = "@lib_:cvtdoc"
$
$	if f$search("nm$*.rnt") .nes. "" then delete nm$*.rnt;*
$	if f$search("nm$*.brn") .nes. "" then delete nm$*.brn;*
$	if f$search("nm$*.doc") .nes. "" then delete nm$*.doc;*
$	if f$search("nm$*.hlp") .nes. "" then delete nm$*.hlp;*
$
$	wrout "nm$help"
$	dsr nm$help.rnh
$	cvtdoc nm$help.hlp
$	purge nm$help.hlp
$
$	wrout "nm$rnotes"
$	dsr nm$rnotes.rnd
$	cvtdoc nm$rnotes.doc
$	purge nm$rnotes.doc
$
$	wrout "nm$uguide"
$	dsr nm$uguide.rnd/automatic
$	cvtdoc nm$uguide.doc
$	purge nm$uguide.doc
$
$	wrout "nm$sysmgr"
$	dsr nm$sysmgr.rnd/automatic
$	cvtdoc nm$sysmgr.doc
$	purge nm$sysmgr.doc
$
$	wrout "nm$extif"
$	dsr nm$extif.rnd/automatic
$	cvtdoc nm$extif.doc
$	purge nm$extif.doc
$
$	if f$search("nm$*.rnt") .nes. "" then delete nm$*.rnt;*
$	if f$search("nm$*.brn") .nes. "" then delete nm$*.brn;*
$!
$! End of RUNOFF.COM
$!
