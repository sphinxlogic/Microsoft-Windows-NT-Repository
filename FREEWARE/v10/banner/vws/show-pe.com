$	bliss/deb show-pe         'bliss_opts'
$	link /trace/deb /symbol=show-pe /exe=show-pe /map=show-pe /full /cross 'f$logical ("banner_link_opts")' -
		sys$input /options
cluster=show_pe,,1,show-pe,sys$system:sys.stb/sel
