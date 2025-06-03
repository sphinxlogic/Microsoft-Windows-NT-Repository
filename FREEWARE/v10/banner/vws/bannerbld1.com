$bliss_opts = "/list /machine=noobject /opt=(lev:3,speed) /term:stat" + f$logical ("banner_bliss_opts")
$on warning then goto error
$	bliss banner'p1'   'bliss_opts'
$ if p2 .nes. "" then 	bliss banner'p2'          'bliss_opts'
$	link /trace /symbol=vwsbanner /exe=vwsbanner /map=vwsbanner /full -
		sys$input /options
cluster=banner,,1,banner,bannerclock,bannercpu,bannercube,bannermonitor,bannerremote,bannersubs,sys$system:sys.stb/sel
