$!
$!	logicals to read vcg and pli files on TLE
$!
$ assign tle::vcg$:[vcg.maint.stable] vsrc$$:
$ assign tle::vcg$:[vcg.maint.stable] vobj$$:
$ assign tle::vcg$:[vcg.lis] vlis$$:
$ assign tle::vcg$:[vcg.com] vcom$$:
$ assign tle::dbgd$:[debug.obj] dobj$$:
