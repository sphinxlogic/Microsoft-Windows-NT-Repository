$ DELETE XFISH.EXE;*, XFISH.LIS;*, XFISH.OBJ;*, XFISH.MAP;*, GETOPT.OBJ;*, GETOPT.LIS;*
$
$ compile := cc/stand=vaxc/opt
$
$ compile XFISH
$ compile GETOPT
$ LINK XFISH ,GETOPT , XFISH.OPT/OPT
