$!
$!	logicals to read vcg and pli files on TLE
$!
$ assign tle::vcg$:[vcg.maint.stable] vsrc$$:
$! assign vcg$ vsrc$$:
$ assign tle::vcg$:[vcg.maint.stable] vobj$$:
$! assign vcg$ vobj$$:
$ assign tle::vcg$:[vcg.lis] vlis$$:
$ assign tle::vcg$:[vcg.com] vcom$$:
$ assign tle::pli$:[pl1.src] psrc$$:
$ assign tle::pli$:[pl1.obj] pobj$$:
$ assign tle::pli$:[pl1.lis] plis$$:
$ assign tle::pli$:[pli.com] pcom$$:
$ assign tle::dbgd$:[debug.src] dsrc$$:
$ assign tle::dbgd$:[debug.obj] dobj$$:
