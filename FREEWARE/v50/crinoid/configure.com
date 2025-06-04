$!
$!
$!  set configurations for CRINOID build/install
$!
$ wo := write sys$output
$ on error then goto cleanup
$ on control_y then goto cleanup
$!
$!
$ wo "     CRINOID build/install configuration"
$ wo "     ---------------------------------"
$ wo ""
$ wo "After configuration, you can do a @BUILD to compile and link"
$ wo "CRINOID, TENTACLE and the ancillary programs STUB and CRINOIDLOG"
$ wo "After a successful build, @INSTALL will copy files to the
$ wo "installation directories."
$ wo ""
$ wo "------------------------------------------------------------"
$!
$ wo "Checking MANIFEST for your kit ..."
$ missing = 0
$ open/read/err=nokit FD$CRINOID_CONFIGURE manifest.
$ mloop:
$   read/end=emloop FD$CRINOID_CONFIGURE line
$   line = f$edit(line,"trim")
$   f = f$search(line,22)
$   if f .eqs. ""
$   then
$       missing = 1
$       wo "File ''line' seems to be missing from your kit"
$   endif
$ goto mloop
$ nokit:
$   wo "Your manifest. file seems to be missing! Configure aborted"
$   exit
$ emloop:
$ close FD$CRINOID_CONFIGURE
$ if missing
$ then
$     wo ""
$     wo " There seems to be files missing from your kit... configure aborted"
$     wo ""
$     exit
$ endif
$ wo "                        OK!"
$ wo ""
$!
$ arch = "VAX"
$ if f$getsyi("ARCH_NAME") .nes. "VAX" then arch = "AXP"
$ wo "Building ''arch' version of CRINOID"
$ wo ""
$!
$!
$ installdir = "SYS$DISK:[]"
$ perl_loc = ""
$ perl_setup = "PERL_ROOT:[VMS]PERL_SETUP.COM"
$ make_name = ""
$ vms_debug = 0
$ verbose   = 0
$ showwhat  = ""
$ pdebug = 0
$ process_name = "CRINOID"
$ service_name = "WWWPERL"
$ crinoidusr   = "HTTP"
$!
$ x = f$search("configure.mms")
$ if x .eqs. ""
$ then
$   redux = 0
$   wo "No previous configuration file found...using standard defaults"
$ else
$   redux = 1
$   wo "Loading defaults from configuration file"
$   open/read FD$CRINOID_CONFIGURE configure.mms
$   rcloop:
$       read/end=ercloop FD$CRINOID_CONFIGURE line
$       x = f$elem(0,"=",line)
$       y = f$extract(f$len(x)+1,f$len(line),line)
$       if x .eqs. "INSTALLDIR" then installdir = y
$       if x .eqs. "PERLDIR"    then perl_loc = y
$       if x .eqs. "PERLSETUP"  then perl_setup = y
$       if x .eqs. "MAKE_NAME"  then make_name = y
$       if x .eqs. "DEBUG"      then vms_debug = 1
$       if x .eqs. "VERBOSE"    then verbose = 1
$       if x .eqs. "SHOW"       then showwhat = y
$       if x .eqs. "PERLDEBUG"  then pdebug = 1
$       if x .eqs. "CRINOID_MGR"   then crinoidmgr = y
$       if x .eqs. "CRINOID_USR"   then crinoidusr = y
$       if x .eqs. "EXC_WORKAROUND" then exc_workaround = y
$       if x .eqs. "PROCESS_NAME" then process_name = y
$       if x .eqs. "SERVICE_NAME" then service_name = y
$       if x .eqs. "NO_PERSONA"  then no_persona = y
$   goto rcloop
$   ercloop:
$   close FD$CRINOID_CONFIGURE
$ endif
$ showmode = (showwhat .nes. "")
$ wo ""
$!
$!  cc options/modes
$!
$ if f$type(exc_workaround) .eqs. ""
$ then
$   wo "Some DECC compiler/library versions have a bug compiling EXC_HANDLING.H"
$   wo "when in /DEBUG mode...Now testing for presense of the bug"
$   wo "If the bug is present, you may see a stack trace, ACCVIO or
$   wo "'out of memory' error"
$   wo "You can ignore error messages between the lines:"
$   wo "-------------------------------------------------------------"
$!
$   open/write FD$CRINOID_CONFIGURE test_exc.c
$   write FD$CRINOID_CONFIGURE "/* test */"
$   write FD$CRINOID_CONFIGURE "#include <exc_handling.h>"
$   write FD$CRINOID_CONFIGURE "main() {}"
$   close FD$CRINOID_CONFIGURE
$   set noon
$   cc/debug/noopt test_exc
$   ok = f$integer($STATUS) .and. 1
$   wo "-------------------------------------------------------------"
$   on error then goto cleanup
$   on control_y then goto cleanup
$   if (.not. ok)
$   then
$       wo "EXC_HANDLING.H/DEBUG problem detected, using workaround"
$       exc_workaround = 1
$   else
$       wo "Your compiler and library are okay..no workaround needed"
$       exc_workaround = 0
$   endif
$ endif
$!
$!  check for presense of PERSONA system service
$!
$ if f$type(NO_PERSONA) .eqs. ""
$ then
$   wo ""
$   wo "Checking for the presense of the PERSONA system services."
$   tempfile = "test_persona.lis"
$   lib/list='tempfile'/only=secureshr/names sys$library:imagelib.olb
$   has_persona = 0
$   open/read/error=done fd 'tempfile'
$   loop:
$     read/end=eloop fd line
$     if f$locate("SYS$PERSONA_CREATE",line) .lt. f$len(line) then goto gotit
$   goto loop
$   gotit:
$     wo "This system has PERSONA system services."
$     has_persona = 1
$   eloop:
$   close fd
$   done:
$     if f$trnlnm("FD") .nes. "" then close fd
$     if f$search(tempfile) .nes. "" then delete 'tempfile';*
$   no_persona = .not. has_persona
$   if no_persona then wo "This system does NOT have PERSONA system services, using workaround for STUB"
$ endif
$ wo ""
$!
$!
$! generate extra bits needed
$!
$!
$ wo "You need a 'make utility' to do the build..."
$ if make_name .eqs. ""
$ then
$   if f$type(mmk) .nes. ""
$   then
$       make_name = "MMK"
$   else
$       make_name = "MMS"
$   endif
$ endif
$ call query QQQ "Select make utility" "''make_name'" "MMK|MMS"
$ make_name = QQQ
$ make_util = "MMS"
$ if make_name .eqs. "MMK" then make_util = mmk
$!
$!  where's Perl? Where's its setup file?  Where's the .h files?
$!
$ if perl_loc .eqs. ""
$ then
$   if f$trnlnm("PERL_ROOT") .nes. ""
$   then
$       call unconceal Perl_Root:[000000] QQQ
$       perl_loc = QQQ
$   endif
$ endif
$ call query QQQ "Location of Perl to use with CRINOID" 'perl_loc'
$ perl_loc = QQQ
$ call unconceal 'perl_loc'  QQQ
$ perl_loc = QQQ
$!
$ x = perl_loc - "]" + ".VMS]"
$ y = f$search(x+"perl_setup.com;")
$ if y .eqs. "" then y = f$search(perl_loc+"Perl_setup.com;")
$ if y .eqs. "" then y = f$search(perl_loc+"setup.com;")
$ if y .eqs. ""
$ then
$   wo "Unable to find a 'perl_setup.com' file in PERL_ROOT:[000000]..."
$   wo "this is needed to set up Perl logicals for the TENTACLE processes."
$   wo "If you did an 'install' of Perl away from its build directory, you"
$   wo "may want to copy Perl_Setup.com to the Perl install directory and"
$   wo "re-run this procedure (control-Y out now).  Otherwise, enter the"
$   wo "name of a .com file that will set up Perl"
$   call query QQQ "COM file to setup the Perl selected" ""
$   y = QQQ
$   if (y .eqs. "" .or. f$find(y) .eqs. "")
$   then
$       wo "unable to find the Perl setup file entered!  Aborting..."
$       goto cleanup
$   endif
$ endif
$ perl_setup = y - f$parse(y,,,"version")
$ wo "Executing Perl setup procedure..."
$ @'perl_setup'
$ wo ""
$!
$!  check perl permissions
$!
$ call check_perl_permission
$!
$!  check if Opcode patch has been applied
$!
$ call check_opcode
$ if f$integer($STATUS) .eq. 9 then goto cleanup
$!
$!  get perl shareable images
$!
$ x = f$extract(1,f$len(perl),perl)
$ y = f$parse(x,".EXE",,"type")
$ ps = f$search("perl_root:[000000]perlshr''y'")
$ dps= f$search("perl_root:[000000]dbgperlshr''y'")
$ if ps .nes. "" .and. dps .nes. ""
$ then
$   def = "N"
$   if pdebug then def = "Y"
$   call query QQQ "Build with DBGPERLSHR?" 'def' "Y|N"
$   pdebug = (QQQ .eqs. "Y")
$   wo ""
$ else
$   pdebug = (dps .nes. "")
$ endif
$!
$ x = f$search("Perl_root:[000000...]extern.h")
$ if (x .eqs. "")
$ then
$   write sys$output "---ERROR: unable to locate EXTERN.h in PERL_ROOT:[*...]"
$   write sys$output "is your Perl installation complete?"
$   goto cleanup
$ endif
$ perl_inc = f$parse("Z.Z;",x) - "Z.Z;"
$!
$!
$!
$ wo ""
$ wo "It is recommended that you build CRINOID in a 'work' directory that"
$ wo "is separate from its installation directory. "
$ wo "Files will not be moved to the installation directory until the "
$ wo "@INSTALL procedure is run, but the directories will be created (if"
$ wo "necessary) now.
$ wo ""
$!
$ uic = f$getjpi("","UIC")
$ if f$trnlnm("WWW_ROOT") .nes. ""
$ then
$   f = f$search("WWW_ROOT:[000000]*.log")
$   if f .eqs. "" then f = f$search("WWW_ROOT:[000000]*.dir")
$   if f .nes. "" then uic = f$file(f,"UIC")
$ endif
$!
$ call query QQQ "CRINOID directories owner UIC" 'uic'
$ ownqual = ""
$ if uic .nes. "" then ownqual = "/own=''uic'"
$ call query QQQ "CRINOID installation location" 'installdir'
$ call unconceal 'QQQ' QQQ
$ installdir = QQQ
$ installlib = installdir - "]" + ".lib]"
$ if f$parse(installdir+"Z.Z;") .eqs. ""
$ then
$   wo ""
$   wo "Hmmm....looks like ''installdir' doesn't exist"
$   call query QQQ "Create ''installdir'?" "Y" "Y|N"
$   if QQQ .eqs. "Y"
$   then
$       create/dir/log'ownqual' 'installdir'
$   endif
$ endif
$ if f$parse(installlib+"Z.Z;") .eqs. ""
$ then
$   wo ""
$   wo "Hmmm....looks like ''installlib' doesn't exist"
$   call query QQQ "Create ''installlib'?" "Y" "Y|N"
$   if QQQ .eqs. "Y"
$   then
$       create/dir/log'ownqual' 'installlib'
$   endif
$ endif
$!
$ wo ""
$ wo "You can build VMS-DEBUG versions of the CRINOID programs; while useful"
$ wo "for debugging, they can't be installed with the privs they need to run"
$ wo "so they must be run from a priviledged account."
$ wo ""
$ def = "N"
$ if vms_debug then def = "Y"
$ call query QQQ "Build VMS-DEBUG version?" 'def' "Y|N"
$ vms_debug = (QQQ .eqs. "Y")
$ skip1:
$!
$ wo ""
$ wo "A 'verbose' build includes compiler listing and linker map files."
$ wo "Most of the time these aren't needed, but can help to track down"
$ wo "compilation or link problems."
$ wo ""
$ def = "N"
$ if verbose then def = "Y"
$ call query QQQ "Build VERBOSE?" 'def' "Y|N"
$ verbose = (QQQ .eqs. "Y")
$ skip2:
$!
$ if .not. verbose then showmode = 0
$ if .not. verbose then goto skip3
$ wo ""
$ wo "When building with VERBOSE, you can set the level of detail"
$ wo "in the compiler listings with the /SHOW=() qualifier.  See"
$ wo "the HELP for the CC command for allowed values for /SHOW. "
$ wo "Two useful cases are to omit the /SHOW qualifier "
$ wo "and /SHOW=(ALL) for maximum verbosity"
$ wo ""
$ call query QQQ "Build /SHOW=(...) options?" 'showwhat' ""
$ showwhat = QQQ
$ showmode = (showwhat .nes. "")
$ skip3:
$!
$ wo ""
$ wo "If the CRINOID server crashes, it can optionally send a VMSmail"
$ wo "notification.  Subsequent connections should restart CRINOID, but"
$ wo "the notification can be useful to track down problems.  The user"
$ wo "to notify is put in the CRINOID_MGR logical; if blank or undefined,"
$ wo "no crash notification will be sent."
$ call query QQQ "User to notify if CRINOID crashes?" 'CRINOIDmgr' ""
$ CRINOIDmgr = QQQ
$!
$wo ""
$wo "What USERNAME will the CRINOID server be running under?  This will"
$wo "only be used in starting up the server without needing to use a"
$wo "browser."
$wo ""
$ call query QQQ "Username for CRINOID server?" 'crinoidusr' ""
$ crinoidusr = f$edit(QQQ,"trim,upcase")
$ wo ""
$ wo "The following two options are generally only useful if you are going"
$ wo "to run multiple CRINOID servers. About the only reason to do so is to"
$ wo "do server debugging and testing separate from a 'production' server."
$ wo "If you will only be running a single CRINOID server, take the defaults."
$ wo "For multiple CRINOID servers, each separate server should get a different"
$ wo "process name, and a different DECNET service name.  (e.g., 'CRINOID2' and 'WWWPERL2')"
$ wo "Please keep in mind that there are length restrictions imposed by VMS."
$ wo ""
$ call query QQQ "Process name for CRINOID server?" 'process_name' ""
$ process_name = f$edit(QQQ,"trim,upcase")
$!
$ call query QQQ "DECNET service name for CRINOID server?" 'service_name' ""
$ service_name = f$edit(QQQ,"collapse,upcase")
$!
$ wo ""
$ wo "Writing Configuration file ... "
$ open/write FD$CRINOID_CONFIGURE CONFIGURE.MMS
$ write FD$CRINOID_CONFIGURE "__''ARCH'__=1"
$ if vms_debug then write FD$CRINOID_CONFIGURE "DEBUG=1"
$ if verbose   then write FD$CRINOID_CONFIGURE "VERBOSE=1"
$ if showmode  then write FD$CRINOID_CONFIGURE "SHOW=''showwhat'"
$ if pdebug    then write FD$CRINOID_CONFIGURE "PERLDEBUG=1"
$ write FD$CRINOID_CONFIGURE "INSTALLDIR=''installdir'"
$ write FD$CRINOID_CONFIGURE "INSTALLLIB=''installlib'"
$ write FD$CRINOID_CONFIGURE "PERLDIR=''perl_loc'"
$ write FD$CRINOID_CONFIGURE "PERLINC=''perl_inc'"
$ write FD$CRINOID_CONFIGURE "PERLSETUP=''perl_setup'"
$ write FD$CRINOID_CONFIGURE "MAKE_NAME=''make_name'"
$ write FD$CRINOID_CONFIGURE "MAKE_UTIL=''make_util'"
$ write FD$CRINOID_CONFIGURE "CRINOID_MGR=''crinoidmgr'"
$ write FD$CRINOID_CONFIGURE "CRINOID_USR=''crinoidusr'"
$ write FD$CRINOID_CONFIGURE "EXC_WORKAROUND=''exc_workaround'"
$ write FD$CRINOID_CONFIGURE "PROCESS_NAME=''process_name'"
$ write FD$CRINOID_CONFIGURE "SERVICE_NAME=''service_name'"
$ write FD$CRINOID_CONFIGURE "!XCD=,DEBUG_MEMORY"
$ if no_persona then write FD$CRINOID_CONFIGURE "NO_PERSONA=1"
$ close FD$CRINOID_CONFIGURE
$ wo "----Configuration file written---"
$!
$!
$ wo ""
$ wo "---------------------------------------------------------------"
$ wo "Configuration sucessfully completed"
$ if (redux)
$ then
$   wo "If you have changed DEBUG or PERL options, you may want to @BUILD CLEAN first"
$ endif
$ wo "@BUILD to build the CRINOID files."
$!
$ cleanup:
$   if f$type(QQQ) .nes. "" then delete/symbol/global QQQ
$   if f$trnlnm("FD$CRINOID_CONFIGURE") .nes. "" then close FD$CRINOID_CONFIGURE
$   if f$search("test_exc.c") .nes. "" then delete test_exc.c;
$   if f$search("test_exc.obj") .nes. "" then delete test_exc.obj;
$ exit
$!
$!
$ query: subroutine
$   on error then exit 20
$   on control_y then exit 44
$   symbol = p1
$   if symbol .eqs. ""
$   then
$       write sys$output "ERROR: symbol needed to query"
$       exit 20
$   endif
$   prompt = p2
$   default= p3
$   allowed= p4
$   edit   = p5
$   if edit .eqs. "" then edit = "collapse,upcase"
$   qagain:
$   read/end=qeof/prompt="''prompt' [''default']: " sys$command ans
$   if edit .nes. "" then ans = f$edit(ans,edit)
$   if ans .eqs. "" then ans = default
$   if allowed .eqs. "" then goto qok
$   j = 0
$   qtest:
$       aa = f$element(j,"|",allowed)
$       if aa .eqs. "|" then goto qbad
$       if aa .eqs. ans then goto qok
$       if aa .eqs. f$extract(0,f$len(aa),ans)
$       then
$           ans = aa
$           goto qok
$       endif
$       j = j + 1
$   goto qtest
$   qbad:
$       write sys$output "Please select one of (''allowed')"
$       goto qagain
$   qok:
$   'symbol' == ans
$   exit 1
$   qeof:
$   exit 44
$ endsubroutine
$!
$!  call unconceal file symbol-to-set
$!
$ unconceal: subroutine
$   p = f$parse(p1,,,"directory","no_conceal,syntax_only")
$!
$   d1 = f$element(1,"[",p) - "]"
$   if f$extract(f$len(d1)-1,1,d1) .eqs. "." then d1 = f$extract(0,f$len(d1)-1,d1)
$   l1:
$   if f$extract(0,7,d1) .eqs. "000000."
$   then
$       d1 = f$extract(7,f$len(d1)-7,d1)
$       goto l1
$   endif
$   if d1 .eqs. "000000" then d1 = ""
$!
$   d2 = f$element(2,"[",p) - "]"
$   if d2 .eqs. "[" then d2 = ""
$   l2:
$   if f$extract(0,7,d2) .eqs. "000000."
$   then
$       d2 = f$extract(7,f$len(d2)-7,d2)
$       goto l2
$   endif
$   if d2 .eqs. "000000" then d2 = ""
$!
$   if d1 .eqs. ""
$   then
$       if d2 .eqs. ""
$       then
$           p = "[000000]"
$       else
$           p = "[''d2']"
$       endif
$   else
$       if d2 .eqs. ""
$       then
$           p = "[''d1']"
$       else
$           p = "[''d1'.''d2']"
$       endif
$   endif
$   dev = f$parse(p1,,,"device","no_conceal,syntax_only")
$   dev2 = f$parse(p1,,,"device","syntax_only")
$   dev3 = f$parse(dev2,,,"directory","no_conceal,syntax_only")
$   if (f$locate("][",dev3) .ge. f$len(dev3)) then dev = dev2
$   'p2' == dev + p
$ exit 1
$ endsubroutine
$!
$!  check that PERL_ROOT:[LIB...] has W:RE permissions
$!
$ check_perl_permission: subroutine
$ wo "Checking permissions in PERL_ROOT:[LIB...], this may take a few minutes"
$ loop:
$   f = f$search("Perl_Root:[LIB...]*.*;")
$   if f .eqs. "" then goto eloop
$   p = f$file(f,"PRO")
$   x = f$element(4,"=",p) - "W" - "D"
$   if x .nes. "RE"
$   then
$       wo "-------------------------------------------------------------------------"
$       wo "WARNING: some files in PERL_ROOT:[LIB...] do not have World:Read+Execute !"
$       wo "this could cause user-scripts to fail."
$       wo ""
$       wo "You should do a  $SET FILE PERL_ROOT:[LIB...]*.*;/PROT=(W:RE) before "
$       wo "proceeding with the @BUILD"
$       wo "-------------------------------------------------------------------------"
$       exit
$   endif
$ goto loop
$ eloop:
$ wo "..permission check OK"
$ wo ""
$ exit
$ endsubroutine
$!
$!  check_opcode patchiness
$!
$ check_opcode: subroutine
$ wo "Checking for Opcode patch..."
$ open/write FD$CRINOID_CONFIGURE test_opcode.pl
$ wf = "write FD$CRINOID_CONFIGURE"
$ wf "#! perl "
$ wf "use Safe;"
$ wf "use Opcode;"
$ wf "$s = new Safe FOO;"
$ wf "$s->permit(':all');"
$ wf "if (-d '/sys$scratch' && -w _) {"
$ wf "   $tstdir = '/sys$scratch';
$ wf "} else {
$ wf "   opendir(D,'/sys$login');"
$ wf "   while (defined($_ = readdir(D))) {
$ wf "      last if -d $_ && -w _;
$ wf "   }
$ wf "   closedir(D);
$ wf "   $tstdir = $_;
$ wf "}"
$ wf "$script = <<'THEEND';
$ wf "   blem();
$ wf "   sub blem {"
$ wf "       my @s = caller;"
$ wf "       $BAR = $s[0];"
$ wf "       print $s[0],qq(\n);"
$ wf "   }"
$ wf "THEEND"
$ wf "if ($tstdir) {"
$ wf "    $script .= 'if (-d q('.$tstdir.') && -w _) {'.qq(\n);"
$ wf "    $script .= '    print qq(_OK\n);'.qq(\n);"
$ wf "    $script .= '} else {'.qq(\n);"
$ wf "    $script .= '    print qq(_FAILED\n);'.qq(\n);"
$ wf "    $script .= '}'.qq(\n);"
$ wf "} else {"
$ wf "    $script .= 'print qq(_NOTEST\n);'.qq(\n);"
$ wf "}"
$!
$ wf "print $],qq(\n);"
$ wf "print $Opcode::VERSION,qq(\n);"
$ wf "$s->reval($script);"
$ close FD$CRINOID_CONFIGURE
$ perl test_opcode.pl >test_opcode.out
$ delete test_opcode.pl;
$!
$ open/read FD$CRINOID_CONFIGURE test_opcode.out
$ read FD$CRINOID_CONFIGURE perl_version
$ read FD$CRINOID_CONFIGURE opcode_version
$ read FD$CRINOID_CONFIGURE main_test
$ read FD$CRINOID_CONFIGURE underbar_test
$ close FD$CRINOID_CONFIGURE
$!
$ if (main_test .eqs. "main" .and. underbar_test .eqs. "_OK")
$ then
$   delete test_opcode.out;
$   wo "Opcode patch test: OK"
$   exit 1
$ else
$   if (main_test .eqs. "FOO")
$   then
$      delete test_opcode.out;
$      wo "Opcode patch has not been applied to this Perl. Please read the"
$      wo "file 000_README.TXT and apply a patch to the file [.ext.Opcode]opcode.xs"
$      wo "in your Perl distribution, or CRINOID will not operate properly."
$      wo ""
$      wo "If you have an 'installed' Perl (i.e., with no source code), then"
$      wo "get a copy of Opcode from CPAN, patch it, build it and install it"
$      wo "in your Perl (installation = copy OPCODE.EXE to where previous version
$      wo "is in your PERL_ROOT:[LIB...] tree)"
$      wo ""
$      wo "You will need to use the correct patch for your version of Perl.  If you "
$      wo "get a copy of Opcode from CPAN, it will probably be for the most recent Perl."
$      wo "See 000_README.TXT for more details."
$      wo ""
$      wo "It is a good idea to do that NOW, before building CRINOID, since"
$      wo "a Perl rebuild might be triggered by the change to Opcode."
$      call query QQQ "Terminate CONFIGURE.COM" "Y" "Y|N"
$      if QQQ then exit 9
$   else if (main_test .eqs. "main")
$   then
$      if (underbar_test .eqs. "_FAILED")
$      then
$           delete test_opcode.out;
$           wo "You seem to have a patched Opcode.xs, but not the latest version "
$           wo "patch...you should apply the most recent patch to a 'virgin' copy"
$           wo "of Opcode.xs.  See 000_README.TXT for details."
$           call query QQQ "Terminate CONFIGURE.COM" "Y" "Y|N"
$           if QQQ then exit 9
$      else if (underbar_test .eqs. "_NOTEST")
$      then
$           delete test_opcode.out;
$           wo "Opcode.xs has been patched, but CONFIGURE was not able to determine"
$           wo "if the patch is an up-to-date one.  If you have applied an up-to-date"
$           wo "patch, then continue with CONFIGURE. "
$           call query QQQ "Terminate CONFIGURE.COM" "Y" "Y|N"
$           if QQQ then exit 9
$      else
$           goto weird
$      endif
$      endif
$   else
$      goto weird
$   endif
$ endif
$ exit 1
$ weird:
$      wo "Hmmm...there seems to be something funny with how Safe:: is working"
$      wo "Please send the following to lane@duphy4.physics.drexel.edu:"
$      wo "----------------------------------------------------------"
$      wo "test_opcode output:"
$      type test_opcode.out
$      delete test_opcode.out;
$      wo "----------------------------------------------------------"
$      wo "...aborting configure"
$      exit 9
$ endsubroutine
