set print pretty
b KludgeOutputLine if **pline != '#'
set args -I.././config  -DTOPDIR=../. -DCURDIR=./config