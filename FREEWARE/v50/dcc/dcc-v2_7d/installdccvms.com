$ ! INSTALLDCCVMS.COM
$ arch := 'F$GETSYI("arch_name")  ! ':=' converts to upper-case
$ if arch .nes. "VAX" .and. arch .nes. "ALPHA"
$ then
$   write sys$error "Current machine has unknown architecture: ''arch'"
$   exit
$ endif
$ copy sys$input sys$output
'dcc' needs two logical names to be defined to work:
- dccexecfilesdir  directory where its execution files are to be installed,
- dccsyshdrdir     directory where C system header files ('stdio.h' etc.) are
                   located.
$loop1:
$ inquire answer "Are those names defined and up-to-date ? (y/n)"
$ if answer .eqs. "N" then exit
$ if answer .nes. "Y" then goto loop1
$ if F$SEARCH(F$TRNLNM("dccsyshdrdir") + "stdio.h") .eqs. ""
$ then
$   write sys$error "Incorrect 'dccsyshdrdir' logical name"
$   exit
$ endif
$loop2:
$ inquire answer "Save previous version ? (if yes, kills previous saved version) (y/n)"
$ if answer .eqs. "N" then goto skip1
$ if answer .nes. "Y" then goto loop2
$loop3:
$ inquire answer "Directory where dcc.exe was installed ?"
$ ansDir := 'F$TRNLNM(answer)
$ if ansDir .eqs. "" then ansDir = answer
$ dirLgt = F$LENGTH(ansDir)
$ posOlb = F$LOCATE("[", ansDir)
$ dirName := 'F$EXTRACT(posOlb + 1, dirLgt - posOlb - 2, ansDir) !':=' necessary
$ if F$LOCATE("]", ansDir) .ne. dirLgt - 1				       -
   .or. dirName .eqs. ""						       -
   .or. dirName .eqs. " "						       -
   .or. F$PARSE("''ansDir*") .eqs. ""
$ then
$   write sys$error "Incorrect name or nonexistent directory"
$   goto loop3
$ endif
$ targetFile = ansDir + "dcc.exe"
$ if F$SEARCH("[-]oldDcc.dir") .eqs. "" then create/directory [-.oldDcc]
$ copy dccexecfilesdir:*.adj		[-.oldDcc]*
$ copy dccexecfilesdir:dynarray.h	[-.oldDcc]*
$ copy dccexecfilesdir:starter.dcc	[-.oldDcc]*
$ copy 'targetFile			[-.oldDcc]*
$ if arch .eqs. "VAX"
$ then
$   rename [-.oldDcc]starter.dcc	[-.oldDcc]starter.dccvax ! done that
$!							       way to keep dates
$   rename [-.oldDcc]dcc.exe		[-.oldDcc]dccvax.exe
$ endif
$ if arch .eqs. "ALPHA"
$ then
$   rename [-.oldDcc]starter.dcc	[-.oldDcc]starter.dccalpha
$   rename [-.oldDcc]dcc.exe		[-.oldDcc]dccalpha.exe
$ endif
$ purge/keep=1				[-.oldDcc]
$skip1:
$ if F$SEARCH("DccExecFiles.dir") .nes. ""
$ then
$   copy [.DccExecFiles]*.* *
$   @rmDirVMS DccExecFiles
$ endif
$ inquire answer "Language for warning/error messages ? ('eng'lish or 'fre'nch)"
$ targetFile = "dcmsg." + answer
$ mms/description=frstTime
$ @askForDfltOpt
$loop4:
$ inquire answer "Unpack all C system header files (instead of most common ones) (y/n)"
$ if answer .eqs. "N" then goto skip2
$ if answer .nes. "Y" then goto loop4
$ @extracthdrfilesvms
$skip2:
$ write sys$output ">>> Compiling and linking dcc"
$ set verify
$ copy 'targetFile dcmsg.txt
$ purge/keep=1 dcmsg.txt
$ if arch .eqs. "VAX"   then copy starter.dccvmsvax   starter.dcc
$ if arch .eqs. "ALPHA" then copy starter.dccvmsalpha starter.dcc
$ @adaptstarterfilevms 'arch
$ purge/keep=1 starter.dcc
$ mms
$ purge/keep=1 dc*.exe
$ set noverify
$ write sys$output ">>> dcc now being tested on itself; should generate no error nor warning"
$ set verify
$ temp = F$TRNLNM("dccexecfilesdir")
$ define/nolog dccexecfilesdir 'F$ENVIRONMENT("default")
$ dcc := $dccexecfilesdir:dc
$ @tstdccvms
$ define/nolog dccexecfilesdir 'temp
$ set noverify
$loop5:
$ inquire answer "Install dcc execution files in 'dccexecfilesdir', that is ''F$TRNLNM("dccexecfilesdir") directory ? (y/n)"
$ if answer .eqs. "N" then exit
$ if answer .nes. "Y" then goto loop5
$ write sys$output ">>> Installing dcc execution files"
$ set verify
$ copy *.adj        dccexecfilesdir:/prot=(w:r)
$ copy starter.dcc  dccexecfilesdir:/prot=(w:r)
$ copy dynarray.h   dccexecfilesdir:/prot=(w:r)
$ purge/keep=1      dccexecfilesdir:*.dcc
$ purge/keep=1      dccexecfilesdir:*.adj
$ purge/keep=1      dccexecfilesdir:dynarray.h
$ set noverify
$loop6:
$ inquire answer "Directory where to install dcc executable ?"
$ if answer .eqs. "" then exit
$ ansDir := 'F$TRNLNM(answer)
$ if ansDir .eqs. "" then ansDir = answer
$ dirLgt = F$LENGTH(ansDir)
$ posOlb = F$LOCATE("[", ansDir)
$ dirName := 'F$EXTRACT(posOlb + 1, dirLgt - posOlb - 2, ansDir) !':=' necessary
$ if F$LOCATE("]", ansDir) .ne. dirLgt - 1				       -
   .or. dirName .eqs. ""						       -
   .or. dirName .eqs. " "						       -
   .or. F$PARSE("''ansDir*") .eqs. ""
$ then
$   write sys$error "Incorrect name or nonexistent directory"
$   goto loop6
$ endif
$ targetFile = ansDir + "dcc.exe"
$ write sys$output ">>> Installing dcc binary"
$ set verify
$ copy/prot=(w=e) dc.exe 'targetFile
$ purge/keep=1 'targetFile
$ delete/noconfirm/nolog dc*.obj;*
$ purge/keep=2 *
$ set noverify
$ write sys$output ">>> dcc installation successful !"
$ write sys$output ""
$ ! End INSTALLDCCVMS.COM
