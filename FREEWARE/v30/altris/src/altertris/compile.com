$ def sys sys$library:
$ CC/DEFINE=VMS/INCLUDE=[] PLAYING.C
$ CC/DEFINE=VMS/INCLUDE=[] RNGS.C
$ CC/DEFINE=VMS/INCLUDE=[] TERTRIS.C
$ CC/DEFINE=VMS/INCLUDE=[] UTILS.C
$ CC/DEFINE=VMS/INCLUDE=[] UNIX_TIMES.C
$ CC/DEFINE=VMS/INCLUDE=[] STRINGS.C
$ CC/DEFINE=VMS/INCLUDE=[] GETHOSTNAME.C
$ CC/DEFINE=VMS/INCLUDE=[] UNLINK.C
