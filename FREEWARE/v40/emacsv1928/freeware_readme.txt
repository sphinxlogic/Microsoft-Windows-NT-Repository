GNU EMACS 19.28, UTILITIES, Editor found on a wide variety of systems

Easiest way to use it:

    Add

	@DAD<n>:[Emacs.SysCommon.Sys$Startup]GNU_Startup_Common Emacs

    to your Login.Com, then type

	@login
	emacs <name of file you want to edit>

    For more information on GNU startup, type

	@DAD<n>:[Emacs.SysCommon.Sys$Startup]GNU_Startup_Common Help Help

    For help using Emacs, type Control/H, then T once in Emacs.

Where it came from:

    (image mode) ftp from

        ftp://ftp.vms.stacken.kth.se/gnu-vms/beta/emacs-19_22-19940806.tar-gz
	(12600 blocks)

How it was built:

    $ create/directory somewhere:[gnu]
    $ set default somewhere:[gnu]
    $ gunzip "-S -gz" -v emacs-19_22-19940806.tar-gz
    $!	(yielding emacs-19_22.tar - 47080 blocks)
    $ tar -xvf emacs-19_22.tar
    $ define/translation:concealed emacs_library somewhere:[gnu.emacs-19_22.]
    $ set default emacs_library:[000000]
    $ delete/symbol/global/all
    $ @configure --with-x --with-gcc=no --with-ucx=no --force -
     --single-tree=EMACS_LIBRARY:[000000] -
     --startupdir=EMACS_LIBRARY:[SYSCOMMON.SYS$STARTUP] -
     --vuelibdir=EMACS_LIBRARY:[SYSCOMMON.VUE$LIBRARY.USER]
    $! then I edited descrip.mms in [000000], [cpp], [lib-src], [oldxmenu],
    $! [src] & [vms] to change /DEBUG to /OPTIMIZE in CFLAG's definition]
    $ mms/ignore=warnings
    $ mms/ignore=warnings install

Differences from the standard distribution:

    I have edited GNU_Startup.Com so that one version supports startup on
    either VAX or Alpha and renamed it to GNU_Startup_Common.  I fixed a
    couple of minor bugs in Configure.Com that prevented me from building
    Emacs in precisely the way I wanted to.

    Note that because it was built with the SINGLE-TREE option, you can
    easily build Emacs for additional (older and newer) versions of VMS
    and have everything but the .Exe and .Dump files shared.

    The standard VMS Descrip.MMS uses SET FILE/ENTER to create EMACS.EXE
    as an alternate name for EMACS-19_22.EXE and likewise for
    EMACS-19_22.DUMP and REBUILD-EMACS-19_22.COM.  I've also used SET
    FILE/ENTER to point DOC-19.22;2, DOC.;1 and DOC.;2 at DOC-19.22;1
    (I haven't yet figured out how to stop the Alpha build from creating
    an exact duplicate of this file built by the VAX build.)  If you
    copy the files from this CDROM, these SET FILE/ENTER commands will
    have to be redone, because BACKUP and COPY will create separate
    files for each entry.
