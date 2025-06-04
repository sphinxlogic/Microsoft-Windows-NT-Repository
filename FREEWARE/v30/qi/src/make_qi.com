$ on error then goto end
$
$ v = 0
$ if p1 .nes. "UCX" .and. p1 .nes. "MULTINET" .and. -
  p1 .nes. "WOLLONGONG" .and. p1 .nes. "NETLIB"
$ then
$   write sys$output "@make ucx or @make multinet or @make wollongong or @make netlib"
$   exit
$ endif
$ opt_flags = p2
$ network_include = ""
$ socket = "SOCKET"
$! TCP/IP package specific values
$ if f$extract(0,1,p1) .eqs. "M"
$ then
$   opt_flags = opt_flags + "/define=(multinet=1)"
$ endif
$ if f$extract(0,1,p1) .eqs. "W"
$ then
$   opt_flags = opt_flags + "/define=(wollongong=1)"
$ endif
$ if f$extract(0,1,p1) .eqs. "U"
$ then
$   opt_flags = opt_flags + "/define=(ucx=1)"
$ endif
$ if f$extract(0,1,p1) .eqs. "N"
$ then
$   opt_flags = opt_flags + "/define=(netlib=1)"
$   socket = "NETLIB"
$ endif
$! determine if VAX CPU
$ if (f$getsyi("cpu") .lt. 128)
$ then
$   cpu = "vax"
$   apitv = "apitv.obj"
$   apitvc = "apitv.obj,"
$ else
$   cpu = "axp"
$!  transfer vectors are in the option file
$   apitv = ""       
$   apitvc = ""       
$ endif
$ if (p2 .eqs. "/VAXC") .or. (p2 .eqs. "/vaxc") .or. -
     ((cpu .eqs. "vax") .and. (f$trnlnm("decc$cc_default") .nes. "/DECC"))
$ then
$   opt = "vaxc"
$ else
$   opt = "decc"
$ endif
$
$ multinet_vax_vaxc = ""
$ multinet_vax_decc = "/prefix=all"
$ multinet_axp_decc = "/prefix=all"
$ wollongong_vax_vaxc = ""
$ wollongong_vax_decc = "/prefix=(all,except=getpeername)"
$ wollongong_axp_decc = "/prefix=(all,except=getpeername)"
$ ucx_vax_vaxc = ""
$ ucx_vax_decc = "/prefix=all"
$ ucx_axp_decc = "/prefix=all"
$ netlib_vax_vaxc = ""
$ netlib_vax_decc = "/prefix=all"
$ netlib_axp_decc = "/prefix=all"
$
$ opt_flags = opt_flags + 'p1'_'cpu'_'opt'
$ opt_file = "''p1'_''cpu'_''opt'.opt"
$
$ define/job cso_apishr [-]qi_api.exe
$
$ if f$search("SYS$SYSTEM:MMS.EXE") .eqs. ""
$ then
$   v = f$verify(1)
$   cc 'opt_flags' qi_main
$   cc 'opt_flags' qi_util
$   cc 'opt_flags' qi_query
$   cc 'opt_flags' qi_'socket'
$   cc 'opt_flags' qi_soundex
$   cc 'opt_flags' crypt
$   cc 'opt_flags' cryptit
$   cc 'opt_flags' strcase
$   link /exec=[-]qi qi_main, qi_util, qi_query, qi_'socket', qi_soundex, -
    crypt, cryptit, strcase, 'opt_file'/opt
$   cc 'opt_flags' qi_api
$   if apitv .nes. "" then macro apitv.mar
$   link /share=[-]qi_api qi_api, 'apitvc' qi_util, qi_query, qi_soundex, -
    crypt, cryptit, strcase, qi_api_'cpu'.opt/opt, 'opt_file'/opt
$   cc 'opt_flags' apitest
$   link /exec=[-]apitest apitest, api.opt/opt, 'opt_file'/opt
$   cc 'opt_flags' qi_make
$   link /exec=[-]qi_make qi_make, 'opt_file'/opt 
$   cc 'opt_flags' qi_build
$   set command/object qi_buildcld
$   link /exec=[-]qi_build qi_build, qi_buildcld, strcase, 'opt_file'/opt 
$   cc 'opt_flags' qi_add_soundex
$   link /exec=[-]qi_add_soundex qi_add_soundex, qi_soundex, 'opt_file'/opt 
$   cc 'opt_flags' qi_add_alias
$   link /exec=[-]qi_add_alias qi_add_alias, 'opt_file'/opt 
$   cc 'opt_flags' qi_add_nickname
$   link /exec=[-]qi_add_nickname qi_add_nickname, 'opt_file'/opt 
$   vx = 'f$verify(v)'
$ else
$   mms /descrip=qi 'p3'
$ endif
$ deassign/job cso_apishr
$end:
$ vx = 'f$verify(v)'
$ exit

