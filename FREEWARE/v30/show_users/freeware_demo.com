$   where = f$element(0, "]", f$environment("procedure")) + "]"
$
$   if f$getsyi("hw_model") .ge. 1024
$   then
$	ext = "alpha"
$   else
$	ext = "vax"
$   endif
$
$   define/nolog show_users.exe 'where'show_users_v12.exe_'ext'
$   set command 'where'show_users_v12
$
$   disp/image=users
$   deas show_users.exe
