$!
$! VMS_CIRCLEMUD_UTILS.COM
$! Written By:  Robert Alan Byer
$!              Vice-President
$!              A-Com Computing, Inc.
$!              ByerRA@aol.com
$!              byer@mail.all-net.net
$!
$! Define The CircleMUD Symbols.
$!
$ DELOBJS   :== $CIRCLEMUD_BIN:DELOBJS.EXE
$ LISTRENT  :== $CIRCLEMUD_BIN:LISTRENT.EXE
$ MUDPASSWD :== $CIRCLEMUD_BIN:MUDPASSWD.EXE
$ PLAY2TO3  :== $CIRCLEMUD_BIN:PLAY2TO3.EXE
$ PURGEPLAY :== $CIRCLEMUD_BIN:PURGEPLAY.EXE
$ SHOPCONV  :== $CIRCLEMUD_BIN:SHOPCONV.EXE
$ SHOWPLAY  :== $CIRCLEMUD_BIN:SHOWPLAY.EXE
$ SPLIT     :== $CIRCLEMUD_BIN:SPLIT.EXE
$ WLD2HTML  :== $CIRCLEMUD_BIN:WLD2HTML.EXE
