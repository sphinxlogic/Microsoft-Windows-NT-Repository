$! This file used to define global symbols to use mtools
$ def mtools_dir sys$user:[tsv.mtools]
$ mread :== $mtools_dir:mread.exe
$ mwrite :== $mtools_dir:mwrite.exe
$ mformat :== $mtools_dir:mformat.exe
$ mdir :== $mtools_dir:mdir.exe
$ minfo :== $mtools_dir:minfo.exe
$ mlabel :== $mtools_dir:mlabel.exe
$ mdel :== $mtools_dir:mdel.exe
