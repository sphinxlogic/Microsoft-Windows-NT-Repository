.first
	define	lib$ [-]
#
tcpfilter.exe : tcpfilter.obj, init_driver.obj, init_driver_noaudit.obj, \
		tcpfilter_vaxc.obj
	@tcplink
tcpfilter.obj : [-.src]tcpfilter.c
	cc	[-.src]tcpfilter
tcpfilter_vaxc.obj : [-.src]tcpfilter.c
	cc/vaxc [-.src]tcpfilter/object=$@
init_driver_noaudit.obj : [-.src]noaudit.mar,init_driver.mar, sys$system:sys.stb
	macro [-.src]noaudit.mar+[]init_driver.mar/object=$@
init_driver.obj : init_driver.mar, sys$system:sys.stb
[-]tcpfilter.mlb(act_def) : [-.src]act_def.mar
	library/replace [-]tcpfilter.mlb [-.src]act_def.mar
