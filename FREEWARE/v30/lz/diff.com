$ set noon
$ diff := diff/match=10
$ diff lz.h     [-.orig]
$ diff lzcmp1.c [-.orig]
$ diff lzcmp2.c [-.orig]
$ diff lzcmp3.c [-.orig]
$ diff lzdcm1.c [-.orig]
$ diff lzdcm2.c [-.orig]
$ diff lzdcm3.c [-.orig]
$ diff lzio.c   [-.orig]
$ diff lzvio.c  [-.orig]
$ diff lzdcl.c  [-.orig]
