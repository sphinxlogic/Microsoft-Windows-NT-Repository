#
STD_MODULES= emon,screen,emoncmd,initstat,stat,\
		show, analyze, trace, record, ni_routines, \
		mon_top, mon_area, mon_node, mon_protocol,\
		mon_multicast, mon_size, mon_summary, mon_map
#
.IFDEF ALPHA
MODULES= $(STD_MODULES)
SYSEXE= /SYSEXE
LIBOBJ= EMON
.ELSE
MODULES= $(STD_MODULES), vector=emon_vector.obj
LIBOBJ= EMON,VECTOR
.ENDIF
#
.IFDEF VAXC
VAXCOPT= ,[-.src]vaxcrtl/opt
.endif
###.first
###	set default [.'f$getsyi("Arch_NAME")']
#
emon.exe : emon.olb($(MODULES)),[-.src]emon.opt,[-.src]emon_$(ARCH).opt
	link$(SYSEXE)/debug/share=emon_dbg.exe \
		emon.olb/lib/incl=($(LIBOBJ)), \
		[-.src]emon.opt/opt,[-.src]emon_$(ARCH).opt/opt $(VAXCOPT)
	link$(SYSEXE)/nodebug/notrace/share=emon.exe/map/full \
		emon.olb/lib/incl=($(LIBOBJ)), \
		[-.src]emon.opt/opt,[-.src]emon_$(ARCH).opt/opt $(VAXCOPT)
	@[-]mkfilter
	purge	   
#
emon.obj : [-.src]emon.c, [-.src]emon.h, [-.src]ni_routines.h, [-.src]monitor.h, [-.src]stat.h
screen.obj  : [-.src]screen.c, [-.src]emon.h, [-.src]monitor.h
emon.olb(emoncmd)  : [-.src]emoncmd.cld
#
initstat.obj  : [-.src]initstat.c, [-.src]emon.h
stat.obj : [-.src]stat.c, [-.src]emon.h
	cc$(CFLAGS)/optimize [-.src]stat.c
#
ni_routines.obj : [-.src]ni_routines.c, [-.src]ni_routines.h, [-.src]emon.h
	cc$(CFLAGS)/optimize [-.src]ni_routines.c
#
mon_top.obj  : [-.src]mon_top.c, [-.src]emon.h, [-.src]stat.h, [-.src]monitor.h
mon_area.obj  : [-.src]mon_area.c, [-.src]emon.h, [-.src]stat.h, [-.src]monitor.h
mon_map.obj  : [-.src]mon_map.c, [-.src]emon.h, [-.src]stat.h, [-.src]monitor.h
mon_node.obj  : [-.src]mon_node.c, [-.src]emon.h, [-.src]stat.h, [-.src]monitor.h
mon_protocol.obj  : [-.src]mon_protocol.c, [-.src]emon.h, [-.src]stat.h, [-.src]monitor.h
mon_multicast.obj  : [-.src]mon_multicast.c, [-.src]emon.h, [-.src]stat.h, [-.src]monitor.h
mon_size.obj  : [-.src]mon_size.c, [-.src]emon.h, [-.src]stat.h, [-.src]monitor.h
mon_summary.obj  : [-.src]mon_summary.c, [-.src]emon.h, [-.src]monitor.h
#
analyze.obj  : [-.src]analyze.c, [-.src]emon.h, [-.src]monitor.h, [-.src]stat.h, [-.src]trace.h,\
		[-.src]filter.h
trace.obj :  [-.src]trace.c, [-.src]emon.h, [-.src]monitor.h, [-.src]stat.h, [-.src]trace.h,\
		[-.src]filter.h
show.obj  : [-.src]show.c, [-.src]monitor.h
record.obj : [-.src]record.c
.IFDEF ALPHA
.ELSE
emon_vector.obj : [-.src]emon_vector.mar
.ENDIF
