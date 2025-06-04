$! link/deb/exe=mtools mtools/opt
$ link/exe=mtools mtools/opt
$ cop mtools.exe mread.exe
$ cop mtools.exe mwrite.exe
$ cop mtools.exe mformat.exe
$ cop mtools.exe mdir.exe
$ cop mtools.exe minfo.exe
$ cop mtools.exe mlabel.exe
$ cop mtools.exe mdel.exe
