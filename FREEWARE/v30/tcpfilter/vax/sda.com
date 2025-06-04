read sys$system:sysdef
def debug @(bgdriver+84)+40	! drv_unload + 64 = debug
def data @(debug)
def table @(data+8)
