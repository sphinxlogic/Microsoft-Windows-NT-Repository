$! wsg.com - play Word Search Game
$!
$ if f$trnlnm("tti_run") .eqs. "" then @freeware$cd:[tti_intouch]intouch.com
$ a=f$environment("PROCEDURE")
$ devdir = f$parse("''a'",,,"DEVICE",) + f$parse("''a'",,,"DIRECTORY",)
$ write sys$output "Loading Word Search Game..."
$ define/nolog tti_game_loc 'devdir'
$ define/user  sys$input  sys$command
$ intouch/source 'devdir'wsg.int
$ exit
