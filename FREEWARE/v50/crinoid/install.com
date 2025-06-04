$!
$!
$ on error then goto cleanup
$ on control_y then goto cleanup
$ wo := write sys$output
$!
$ wo "---------------CRINOID Install-------------"
$ wo "...reading configuration"
$ open/read/err=noconfig fd CONFIGURE.MMS
$ loop:
$   read/end=eloop fd line
$   if f$elem(0,"=",line) .eqs. "MAKE_UTIL"    then make_util = f$elem(1,"=",line)
$   if f$elem(0,"=",line) .eqs. "PERLSETUP"    then perlsetup = f$elem(1,"=",line)
$   if f$elem(0,"=",line) .eqs. "INSTALLDIR"   then installdir = f$elem(1,"=",line)
$   if f$elem(0,"=",line) .eqs. "SERVICE_NAME" then service_name = f$elem(1,"=",line)
$ goto loop
$ eloop:
$ close fd
$ wo "...setting up Perl"
$ @'perlsetup'
$!
$ wo "...doing installation"
$ make_util/macro=(configure.mms) install
$!
$ if f$search(installdir+"CRINOID.CONFIG") .eqs. ""
$ then
$   copy/log [.misc]crinoid.config 'installdir'
$ else
$   wo "CRINOID.CONFIG already exists, not modified"
$ endif
$!
$ q1:
$ read/end=cleanup/prompt="Put ''service_name'.COM in your OSU HTTP directory? [N]: " sys$command yesno
$ yesno = f$edit(yesno,"collapse,upcase")
$ if yesno .eqs. "" then yesno = "N"
$ yesno = f$extract(0,1,yesno)
$ if yesno .eqs. "N" then goto eq1
$ if yesno .nes. "Y" then goto q1
$ if f$trnlnm("WWW_ROOT") .nes. ""
$ then
$   def = f$trnlnm("WWW_ROOT") - ".]" + "]"
$ else
$   def = ""
$ endif
$ read/end=cleanup/prompt="OSU HTTP directory? [''def']: " sys$command osuroot
$ osuroot = f$edit(osuroot,"collapse,upcase")
$ if osuroot .eqs. "" then osuroot = def
$ copy/log 'service_name'.com  'osuroot'
$ eq1:
$ wo "----------------Installation complete------------"
$ wo ""
$ wo "You now need to add a 'exec' rules to the OSU HTTP server config:"
$ wo "    exec  /cgi_base_url/*      0::""0=''service_name'""nl:"
$ wo "and edit CRINOID.CONFIG in ''installdir'"
$ wo "See the documentation for more details"
$ wo "-------------------------------------------------"
$ wo "If you wish to clean up the build directory, use "
$ wo "    @BUILD <target> "
$ wo "where <target> is one of:   "
$ wo "    tidy      -- purge extra versions"
$ wo "    clean     -- tidy + delete tempfiles"
$ wo "    realclean -- delete files created by @CONFIGURE and @BUILD"
$ wo "--------------------------------------------------"
$ cleanup:
$ if f$trnlnm("FD") .nes. "" then close fd
$ exit
