$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_669.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_AMF.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_DSM.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_FAR.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_GDM.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_IMF.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_IT.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_M15.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_MED.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_MOD.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_MTM.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_S3M.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_STM.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_STX.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_ULT.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_UNI.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) LOAD_XM.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) MLOADER.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) MLUTIL.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) MMSUP.C
$ CC/DECC/PREFIX=ALL/INCLUDE=([],[-.LIBARC],[-.TIMIDITY]) MUNITRK.C
$ lib/cre libunimod.olb_axp
$ lib libunimod.olb_axp *.obj
$ delete/noconf *.obj;
