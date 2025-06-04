laser.exe : laser.obj, laser_err.obj, laserref.exe, [-.src]laser.opt
		link/notrace/nodebug/exe=$@ [-.src]laser.opt/option
laserref.exe : laser.obj, laser_err.obj, [-.src]laser.opt
		link/trace/debug/exe=$@ [-.src]laser.opt/option
laser.obj : [-.src]laser.c, [-.src]laser.h
laser_err.obj : [-.src]laser_err.msg
[-.src]laser.opt : [-.src]laser.c
	@[-]mkoptfile [-.src]laser.opt
sys$common:[sysexe]laserdbgsmb.exe : laser.exe
	copy $? $@
