$ CC/DEBUG/NOOPT creprc
$ LINK/DEBUG creprc,sys$input/opt
sys$share:vaxcrtl/share
$ EXIT
