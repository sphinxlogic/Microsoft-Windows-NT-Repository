read sys$loadable_images:sysdef
def debug @(bgdriver+dpt$ps_unload)+0a8
def krnldata @(debug)
def krnlcode @(krnldata+8)
def drv_accessfdt krnldata+20
def drv_unload krnldata+28
def access_table @ (krnldata+30)
def mutex krnldata+38
def unload_rtn 0 (krnldata+48)
def drv_fdtadr krnldata+60
