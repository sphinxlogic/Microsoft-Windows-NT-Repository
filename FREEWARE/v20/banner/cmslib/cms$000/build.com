 $on error then goto error
*4D
 $on warning then goto error
*4E
*4I
 $on warning then continue
*4E
 $set ver
 $set proc/priv=all
 $set proc/prio=3
 $reply/user=saunders "Build of Banner starting."
*8D
 $set def fizbuz$user:[saunders.banner.c]
*8E
*8I
 $set def fizbuz$dua0:[saunders.banner.c]
*8E
 $cms set lib [.cmslib]
*5I
*7D
 $@I18N$LINGDEV:[MOTIF.DXM_BL1]DXM_LOGICALS.COM
*7E
*7I
 $define vmi$wkd sys$disk:[]
 $define shrlib$ sys$disk:[]
*7E
*5E
 $ flags = "(""cflags=/debug=all/noopt"",""bflags=/deb"")"
 $ if "''p1'" .nes. "NODEBUG" then goto no_debug
 $flags = "(""cflags=/nodebug/opt/nolist"",""bflags=/nodeb/nolist"")"
 $no_debug:
 $ if "''p2'" .nes. "NEW" then goto no_delete
*3D
 $del banner*.obj;*
 $del banner.olb;*
*3E
*3I
 $del *banner*.obj;*
 $del *banner*.uil;*
 $del *banner.olb;*
 $del *banner*.exe;*
*3E
*6I
 $mms/act/desc=banner.mms/CMS/ignore=warn/macro='flags/from_source
 $goto do_save
*6E
 $no_delete:
*2D
 $mms/act/desc=banner.mms/CMS/macro='flags
*2E
*2I
 $mms/act/desc=banner.mms/CMS/ignore=warn/macro='flags
*2E
*6I
 $do_save:
*6E
 $set proc/name="BannerSave"
 $@newbck
 $reply/user=saunders "Banner Build conpleted."
 $set nover
 $exit
 $error:
 $reply/user=saunders "Banner.c finnished with an error!!!!!!"
 $exit
