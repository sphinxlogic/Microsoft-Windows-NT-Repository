directory ../pbm
directory ../lib

define redo
symbol-file bbcount
exec-file bbcount
end

set args -verbose -trace psyr>/tmp/out
