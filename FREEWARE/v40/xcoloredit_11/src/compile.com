$ def sys sys$library
$ CC/DEFINE=VMS APPWIN.C
$ CC/DEFINE=VMS CEDIT.C
$ CC/DEFINE=VMS COLORS.C
$ CC/DEFINE=VMS TABLEWIN.C
