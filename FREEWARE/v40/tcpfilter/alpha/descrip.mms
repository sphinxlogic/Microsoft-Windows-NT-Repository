tcpfilter.exe : init_driver.obj, loadcode.obj, tcpfilter.obj, \
		loadcode_noaudit.obj
	@tcplink
tcpfilter.obj : [-.src]tcpfilter.c
loadcode.obj : loadcode.m64,[-]tcpfilter.mlb(act_def)
	macro/alpha	loadcode.m64
loadcode_noaudit.obj : [-.src]noaudit.mar,loadcode.m64,\
			[-]tcpfilter.mlb(act_def)
	macro/alpha	[-.src]noaudit.mar+[]loadcode.m64/object=$@
init_driver.obj : init_driver.mar, [-]tcpfilter.mlb(act_def)
[-]tcpfilter.mlb(act_def) : [-.src]act_def.mar
	lib/replace [-]tcpfilter.mlb [-.src]act_def.mar
