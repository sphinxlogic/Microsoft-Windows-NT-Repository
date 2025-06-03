$verify = 'f$verify(0)
$set noon
$on control then goto done
$privs = f$setprv ("SYSPRV")
$old_default = f$environment ("DEFAULT")
$default = f$environment ("PROCEDURE")
$default = f$parse (default,,, "DEVICE") + f$parse (default,,, "DIRECTORY")
$set default 'default'
$if f$search("banner*.obj") .nes. "" then delete /log banner*.obj;*
$if f$search("banner-pem.r32") .nes. "" then delete /log banner-pem.r32;*
$if f$search("resd$:<driver.src>pem_def.r32") .nes. ""
$then
$set ver
$define pem_def resd$:<driver.src>pem_def
$endif !'f$verify(0)
$bliss_opts = "/list /machine=noobject /opt=(lev:3,speed) /term:stat" + f$logical ("banner_bliss_opts")
$
$on warning then goto error
$
$	set ver
$	set default 'default'
$	if f$search ("SHRLIB$:PE.MLB") .eqs. "" then goto nopem
$	macro banner-pem+sys$library:lib/lib+shrlib$:pe/lib+shrlib$:palib/lib
$	link  banner-pem
$nopem:	define /user sys$output banner-pem.r32
$	run   banner-pem
$	if p1 .nes. "" then goto show_pe
$	bliss bannerclock     'bliss_opts'
$	bliss bannercpu       'bliss_opts'
$	bliss bannercube      'bliss_opts'
$	bliss bannermonitor   'bliss_opts'
$	bliss bannerremote    'bliss_opts'
$	bliss bannersubs      'bliss_opts'
$	bliss banner          'bliss_opts'
$	link /notrace /symbol=vwsbanner /exe=vwsbanner /map=vwsbanner /full 'f$logical ("banner_link_opts")' -
		sys$input /options
cluster=banner,,1,banner,bannerclock,bannercpu,bannercube,bannermonitor,bannerremote,bannersubs,sys$system:sys.stb/sel
$show_pe:
$	bliss show-pe         'bliss_opts'
$	link /trace /symbol=show-pe /exe=show-pe /map=show-pe /full /cross 'f$logical ("banner_link_opts")' -
		sys$input /options
cluster=show_pe,,1,show-pe,sys$system:sys.stb/sel
$goto done	!'f$verify(0)
$error:		!'f$verify(0)
$mail nl: hobbs /subj="error building banner"
$reply /user=hobbs "error building banner"
$done:		!'f$verify(0)
$!purge /keep=3 /nolog banner*.*,show-pe.*
$!if f$search("banner*.obj") .nes. "" then delete /nolog banner*.obj;*
$privs = f$setprv (privs)
$set ver
$	set default 'old_default'
$!'f$verify(0)
$verify = f$verify(verify)	
