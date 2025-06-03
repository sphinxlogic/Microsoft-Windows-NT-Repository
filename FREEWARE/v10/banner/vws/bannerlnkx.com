$	link /trace /symbol=vwsbanner /exe=vwsbanner /map=vwsbanner /full -
		sys$input /options
cluster=banner,,1,banner,bannerclock,bannercpu,bannercube,bannermonitor,bannerremote,bannersubs,sys.stb/sel
