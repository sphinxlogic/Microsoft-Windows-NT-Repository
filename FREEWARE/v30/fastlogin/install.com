$! INSTALL.COM for FastLogin
$!
$ copy fastlogin$startup.com -
  utl$common:[sys$startup]
$ copy fastlogin_alpha.exe, fastlogin_alpha_v61.exe, fastlogin_vax.exe -
  utl$common:[syslib]
$ set file /owner=[system] /protection=(o:rwed, s:rwed, g:re, w:re) -
  utl$common:[sys$startup]fastlogin$startup.com, -
  utl$common:[syslib]fastlogin_alpha.exe, -
  utl$common:[syslib]fastlogin_alpha_v61.exe, -
  utl$common:[syslib]fastlogin_vax.exe
