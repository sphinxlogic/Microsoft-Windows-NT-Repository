$ set noon
$ olddir = f$environment("default")
$ mydisk = f$trnlmn("SYS$DISK")
$ workdir  = mydisk+f$directory()
$ if f$getsyi("cpu") .eq. 128
$    then
$       cpu = "AXP"
$    else
$       cpu = "VAX"
$ endif
$ xscreensaver :== $'workdir'xscreensaver_'cpu'
$ xscreen*command :== $'workdir'xscreensaver-command_'cpu'
$ attrac*tion :== $'workdir'attraction_'cpu'
$ blitspin    :== $'workdir'blitspint_'cpu'
$ decays*creen:== $'workdir'decayscreen_'cpu'
$ flame       :== $'workdir'flame_'cpu'
$ greyne*tic  :== $'workdir'greynetic_'cpu'
$ halo        :== $'workdir'halo_'cpu'
$ helix       :== $'workdir'helix_'cpu'
$ hopalong    :== $'workdir'hopalong_'cpu'
$ hypercube   :== $'workdir'hypercube_'cpu'
$ imsmap      :== $'workdir'imsmap_'cpu'
$ maze        :== $'workdir'maze_'cpu'
$ noseguy     :== $'workdir'noseguy_'cpu'
$ pyro        :== $'workdir'pyro_'cpu'
$ qix         :== $'workdir'qix_'cpu'
$ rocks       :== $'workdir'rocks_'cpu'
$ rorsch*ach  :== $'workdir'rorschach_'cpu'
$ slidescr*een:== $'workdir'slidescreen_'cpu'
$ xroger      :== $'workdir'xroger_'cpu'
$ define/nolog sys$error nl:
$ define/nolog sys$output nl:
$ xscreensaver -demo
$ deassign sys$error
$ deassign sys$output
$ set def 'olddir'
$ exit
