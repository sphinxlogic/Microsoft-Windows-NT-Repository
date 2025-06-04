$!  FASTLOGIN$STARTUP.COM - Thierry Lelegard - 15-APR-1996
$!
$!---- Custom section
$!
$!  To allow usage of DECnet proxies, define the following logical name.
$!
$ DEFINE/SYSTEM/EXEC/NOLOG FASTLOGIN_ALLOW_DECNET_PROXIES TRUE
$!
$!  To allow usage of "LAT proxies", define the following logical name.
$!  There is not "LAT proxy database". FastLogin uses the DECnet proxy
$!  database. SET HOST/LAT sends the LAT node name and user UIC. On the
$!  target system, the DECnet proxy database is searched for a specification
$!  matching lat_node_name::[group,member]. IMPORTANT: It has been observed
$!  that looking for such a proxy may crash the security server.
$!  ==> USE WITH CAUTION.
$!
$! DEFINE/SYSTEM/EXEC/NOLOG FASTLOGIN_ALLOW_LAT_PROXIES TRUE
$!
$!  To allow implicit user recognition inside the cluster when the incoming
$!  connection comes from DECnet (SET HOST), define the following logical name.
$!
$ DEFINE/SYSTEM/EXEC/NOLOG FASTLOGIN_ALLOW_DECNET_CLUSTER TRUE
$!
$!  To allow implicit user recognition inside the cluster when the incoming
$!  connection comes from LAT (SET HOST/LAT), define the following logical name.
$!
$ DEFINE/SYSTEM/EXEC/NOLOG FASTLOGIN_ALLOW_LAT_CLUSTER TRUE
$!
$!  If the cluster has an alias which is used by some nodes but not all,
$!  define the following logical name on nodes which do not use the alias.
$!
$ DEFINE/SYSTEM/EXEC/NOLOG FASTLOGIN_CLUSTER_ALIAS SWTHOM::
$!
$!  In case of problem, uncomment the following line to get trace messages
$!  during login.
$!
$! DEFINE/SYSTEM/NOLOG FASTLOGIN_TRACE TRUE
$!
$!------------------------------------------------------------------------------
$!
$!  Fixed section, do not modify
$!
$ on warning then exit
$ major = f$integer(f$extract(1,1,f$getsyi("version")))
$ minor = f$integer(f$extract(3,1,f$getsyi("version")))
$!
$!  FastLogin is supported only on OpenVMS V6.1 and higher.
$!
$ if major.lt.6 .or. (major.eq.6 .and. minor.lt.1) then exit 1
$!
$!  On Alpha V6.1, we use a different image (without proxy support).
$!
$ if f$getsyi("arch_type").eq.2 .and. major.eq.6 .and. minor.lt.2
$ then arch = "Alpha_V61"
$ else arch = f$getsyi("arch_name")
$ endif
$!
$!  Determine which image to use.
$!
$ fastlogin_image := fastlogin_'arch'
$ if f$search("sys$share:''fastlogin_image'.exe").nes.""
$ then
$   define/system/exec/nolog fastlogin fastlogin_'arch'
$ else
$   fastlogin_image := fastlogin
$   if f$search("sys$share:''fastlogin_image'.exe").eqs."" then exit 1
$ endif
$!
$!  Any shareable image containing LOGINOUT callouts must be installed.
$!
$ if f$file("sys$share:''fastlogin_image'.exe","known") then -
$ install remove sys$share:'fastlogin_image'.exe
$ install add sys$share:'fastlogin_image'.exe
$!
$!  This final step actually hooks FastLogin in the system.
$!
$ define/system/exec/nolog lgi$loginout_callouts fastlogin
$ run sys$system:sysgen
use active
set lgi_callouts 1
write active
$!
$ exit 1
