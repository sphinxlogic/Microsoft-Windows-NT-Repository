objs= arg.obj, blank.obj, board.obj, bonus.obj, bonuslif.obj, bonuspnt.obj, -
colour.obj, corner.obj, corner1.obj, corner2.obj, corner3.obj, corner4.obj, - 
cross.obj, direc.obj, dynamiel.obj, e0.obj, e180.obj, e270.obj, e90.obj, -
element.obj, food.obj, gamedata.obj, gblank.obj, gboard.obj, gbonlife.obj, -
gbonpnt.obj, gdynelem.obj, gfood.obj, gghost.obj, ghost.obj, gpacman.obj, -
graphele.obj, gspecwal.obj, gsupfood.obj, horiwall.obj, pac.obj, pacman.obj, -
point.obj, specwall.obj, supefood.obj, t0.obj, t180.obj, t270.obj, t90.obj, -
timing.obj, usrinter.obj, vertwall.obj, walls.obj

.cxx.obj : 
	cxx /define=(VMS,VERSION=1.001) $< /obj = sys$disk:[]$*.obj

pacman.exe : $(objs)
	cxxlink /exe=pacman.exe $(objs), pacman/opt
