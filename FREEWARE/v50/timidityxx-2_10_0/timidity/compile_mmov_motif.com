$ define/nolog mme                 MMOV$INCLUDE:,[]
$ define/nolog x11                 MMOV$INCLUDE:,DECW$INCLUDE
$ define/nolog decc$user_include   mme,decw$include:
$ define/nolog sys                 decc$user_include
$ define/nolog decc$system_include decc$user_include
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) AIFF_A.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) AQ.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) AU_A.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) AUDIO_CNV.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) AUDRIV_A.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) AUDRIV_MME.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) COMMON.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) CONTROLS.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) EFFECT.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) FILTER.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) INSTRUM.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) LIST_A.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) LOADTAB.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) MFNODE.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) MIDITRACE.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) MIX.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) MOD.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) MOD2MIDI.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) OUTPUT.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) PLAYMIDI.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) RAW_A.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) RCP.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) READMIDI.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) RECACHE.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) RESAMPLE.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) REVERB.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) SBKCONV.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) SFFILE.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) SFITEM.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) SMFCONV.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) SNDFONT.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) TABLES.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) TIMIDITY.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) VERSION.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) WAVE_A.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) WRDT.C
$ CC/DECC/NODEB/PREFIX=ALL/INCL=([],[-.INTERFACE],[-.LIBARC],[-.LIBUNIMOD],[-.UTILS])/DEFINE=(VMS,DEC,AU_DEC,_VMS_V6_SOURCE,mme_BLD,IPC_VMS,Long_bit=64) WRD_READ.C
$ deass mme
$ deass X11
$ deass decc$user_include
$ deass sys
$ deass decc$system_include
