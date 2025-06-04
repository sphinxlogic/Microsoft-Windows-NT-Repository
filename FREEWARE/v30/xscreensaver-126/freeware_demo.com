$ on control_y then goto done
$ a = f$env("procedure")
$ xdev = F$PARSE(a,,,"DEVICE")
$ xdir = F$PARSE(a,,,"DIRECTORY")
$ arch = f$getsyi("ARCH_TYPE")
$if arch .eq. 2
$  then
$    cpu = "AXP"
$  else
$    cpu = "VAX"
$ endif               
$ xscreensaver :== $'xdev''xdir'xscreensaver_'cpu'
$ xscreen*command :== $'xdev''xdir'xscreensaver-command_'cpu'
$ attrac*tion :== $'xdev''xdir'attraction_'cpu'
$ blitspin    :== $'xdev''xdir'blitspin_'cpu'
$ decays*creen:== $'xdev''xdir'decayscreen_'cpu'
$ flame       :== $'xdev''xdir'flame_'cpu'
$ greyne*tic  :== $'xdev''xdir'greynetic_'cpu'
$ halo        :== $'xdev''xdir'halo_'cpu'
$ helix       :== $'xdev''xdir'helix_'cpu'
$ hopalong    :== $'xdev''xdir'hopalong_'cpu'
$ hypercube   :== $'xdev''xdir'hypercube_'cpu'
$ imsmap      :== $'xdev''xdir'imsmap_'cpu'
$ lmorph      :== $'xdev''xdir'lmorph_'cpu'
$ maze        :== $'xdev''xdir'maze_'cpu'
$ noseguy     :== $'xdev''xdir'noseguy_'cpu'
$ pedal       :== $'xdev''xdir'pedal_'cpu'
$ pyro        :== $'xdev''xdir'pyro_'cpu'
$ qix         :== $'xdev''xdir'qix_'cpu'
$ rocks       :== $'xdev''xdir'rocks_'cpu'
$ rorsch*ach  :== $'xdev''xdir'rorschach_'cpu'
$ slidescr*een:== $'xdev''xdir'slidescreen_'cpu'
$ xroger      :== $'xdev''xdir'xroger_'cpu'
$ xscreensaver -demo
$ done:
$ exit
