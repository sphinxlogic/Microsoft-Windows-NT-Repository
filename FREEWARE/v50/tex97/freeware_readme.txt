TeX, MISCELLANEOUS, TeX Typesetting

This is the OpenVMS version of Donald Knuth's TeX typesetting
package.

There were some discussions about the availability of TeX on VMS.
Especially one user sent me a mail where he told me that the TeX
version on the Freeware 4 CD was dated 1995 and that he got the
actual version from the US DECUS site. This was surprising to me 
since I thought that the 1997 version, which I submitted to DECUS
Germany, made it to the US chapter and onto the Freeware CD. So
here is the VMS TeX as of 1997. All images have been compiled on
VMS 6.2 and should work on later versions. If not one may at first
relink the images before recompiling them. Some of the supplied
utilities are now out-of-date, but this is not important. Any
installation will of course update the LaTeX and other macro packages.
This should be no problem if one reads the VMS_TEX_MGR.GUIDE in the
manager directory. Since I now have some more time and resources
available (incl. a DS10 at home) I can work more extensive on the
next version.

Ralf G"artner , 2001

gaertner@cthulhu.westfalen.de , gaertner@decus.decus.de



P.S.  TeX is Y2K compliant, the only "bug" is in one of the output
      routines, where only two digits are used.


============================================================================

 To install TeX on OpenVMS perform the following steps:

 0) copy the distribution (either the ZIP archive or the save-sets)
    to the root directory of the disk that shall hold TeX

 1.1) if you got the CTAN archive (TEX97_CTAN.ZIP) extract the archive
    with UNZIP 5.x into TEX97_CTAN.BCK and restore the save-set with
    the BACKUP utility; this will create the TeX tree under
    <disk>:[TEXMF...]
    The ZIP archive has the OpenVMS record attributes included so one
    really needs the OpenVMS version of UNZIP.

 1.2) if you got the DECUS archives (TEX97_%%%.BCK) restore at least
    TEX97_EXE.BCK and TEX97_FNT.BCK with the BACKUP utility; this will
    create the TeX tree under <disk>:[TEXMF...]

 2) delete any the directories starting with two underscores, __*.DIR . 
    these directories should be empty and have been used during the
    development. if they contain some files, please notify us, since
    you found a bug in the distribution procedure.

 3) define the basic logicals by

      DEFINE/SYSTEM/EXEC                        TEX_DISK   <disk>
      DEFINE/SYSTEM/EXEC/TRANSLATION=CONCEALED  TEX_ROOT   TEX_DISK:[TEXMF.]
      DEFINE/SYSTEM/EXEC                        TEX_USAGE  CLD 

    in your SYS$MANAGER:SYLOGICALS.COM

 4) define the remaining logicals after <disk> has been mounted in
    SYS$MANAGER:SYSTARTUP_VMS.COM or your local startup procedure by

      @TEX_ROOT:[EXE.COMMON]TEX_LOGICALS.COM

 5) add the following to SYS$MANAGER:SYLOGIN.COM or the login procedure
    defined by SYS$SYLOGIN:

      @TEX_EXE:TEX_COMMANDS.COM

 After a reboot TeX will be available. For other installation opportunities
 see TEX_ROOT:[MGR]VMS_TEX_MGR.GUIDE .

 One may find some objects/images in the [SRC...] and [EXE...] tree. Those
 in the [EXE..] tree have been recompiled; those in the [SRC...] tree have
 been fetched from other archives. To save space, one may delete some of the. 

 The file LAST_MINUTE.BUGS in [TEXMF.MGR] describes some bugs found prior
 to the creation of the distribution. All these bugs and oddities will
 (hopefully) be fixed in the next release.

 Important:
 ---------
 Sites who fetched TeX from CTAN will have a basic TeX installation. These
 sites should read the whole TEX_ROOT:[MGR]VMS_TEX_MGR.GUIDE . They will
 need additonal files CTAN (for example Metafont sources). A complete
 file list is given by [MGR]FILES.LIST . 

 Sites who got TeX from DECUS will have a complete installation if they
 installed TEX97_SRC.BCK and TEX97_DOC.BCK. Even if these save-sets have
 not been restored, they will have a complete run-time version.


 Bug reports should be send to one of the addresses below.

 Ralf G"artner

 gaertner@cthulhu.westfalen.de , gaertner@decus.decus.de , (vms@dante.de)

