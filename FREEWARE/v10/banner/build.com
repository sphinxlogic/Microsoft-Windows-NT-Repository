$on error then goto error
$on warning then continue
$set ver
$set proc/priv=all
$!set proc/prio=3
$reply/user=saunders "Build of Banner starting."
$set def fizbuz$dua0:[saunders.banner.c]
$cms set lib [.cmslib]
$define vmi$wkd sys$disk:[]
$define shrlib$ sys$disk:[]
$ flags = "(""cflags=/debug=all/noopt"",""bflags=/deb"")"
$ if "''p1'" .nes. "NODEBUG" then goto no_debug
$flags = "(""cflags=/nodebug/opt/nolist"",""bflags=/nodeb/nolist"")"
$no_debug:
$ if "''p2'" .nes. "NEW" then goto no_delete
$del *banner*.obj;*
$del *banner*.uil;*
$del *banner.olb;*
$del *banner*.exe;*
$mms/act/desc=banner.mms/CMS/ignore=warn/macro='flags/from_source
$goto do_save
$no_delete:
$mms/act/desc=banner.mms/CMS/ignore=warn/macro='flags
$do_save:
$set proc/name="BannerSave"
$@newbck
$reply/user=saunders "Banner Build conpleted."
$set nover
$exit
$error:
$reply/user=saunders "Banner.c finnished with an error!!!!!!"
$exit
