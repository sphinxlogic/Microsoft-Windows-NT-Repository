$ CC /decc/standard=vaxc/nolist/obj=LZCMP1.OBJ LZCMP1.C
$ CC /decc/standard=vaxc/nolist/obj=LZCMP2.OBJ LZCMP2.C
$ CC /decc/standard=vaxc/nolist/obj=LZCMP3.OBJ LZCMP3.C
$ SET COMMAND /OBJECT=LZCOMP.OBJ  LZCOMP.CLD
$ CC /decc/standard=vaxc/nolist/obj=LZIO.OBJ LZIO.C
$ CC /decc/standard=vaxc/nolist/obj=LZVIO.OBJ LZVIO.C
$ CC /decc/standard=vaxc/nolist/obj=LZDCL.OBJ LZDCL.C
$ mms/noaction/from_sources/output=lz.tmp
$ @fixmms lz.tmp lz.com
$ delete lz.tmp;*
$ link /trace/nomap/exec=LZCOMP.EXE -
		lzcmp1.obj,lzcmp2.obj,lzcmp3.obj,lzcomp.obj, -
		lzio.obj,lzvio.obj,lzdcl.obj,sys$library:deccrtl/lib/inc=cma$tis
$ CC /decc/standard=vaxc/nolist/obj=LZDCM1.OBJ LZDCM1.C
$ CC /decc/standard=vaxc/nolist/obj=LZDCM2.OBJ LZDCM2.C
$ CC /decc/standard=vaxc/nolist/obj=LZDCM3.OBJ LZDCM3.C
$ SET COMMAND /OBJECT=LZDCMP.OBJ  LZDCMP.CLD
$ link /trace/nomap/exec=LZDCMP.EXE -
		lzdcm1.obj,lzdcm2.obj,lzdcm3.obj,lzdcmp.obj, -
		lzio.obj,lzvio.obj,lzdcl.obj,sys$library:deccrtl/lib/inc=cma$tis
$ If "''F$Search("LZ.HLB")'" .EQS. "" Then LIBRARY/Create/Help LZ.HLB
$ LIBRARY/REPLACE LZ.HLB LZ.HLP
$ ! do nothing here
