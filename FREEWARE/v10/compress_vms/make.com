!$ Make the VMS version of COMPRESS/UNCOMPRESS.
!$
$ set verify
$ cc compress.c,zfinfo.c,zfdele.c
$ link/nosysshr compress,zfinfo,zfdele
$ delete *.obj;*
        