$! List all (interesting) components of the DECtalk library.
$! The "linepr" program is available on the Decus C kit.
$! This command file assumes linepr was built using Vax-11C.
$!
$ linepr [.source]*.h [.source]*.c *.bas [.demo]demo.c%% -o dtlib.lst
$!
