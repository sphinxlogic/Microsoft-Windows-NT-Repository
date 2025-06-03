directory ../bzr
directory ../tfm
directory ../lib

define redo
symbol-file bzrto
exec-file bzrto
end

# Concat ggmr26D with Y-Rb (remapped J->bracketleft; now obsolete), to
# test sizing.
# 
#set args -verbose -metafont -range Y-\[ \
#  -concat ../ourfonts/garamond/r/ring/Y-Rb \
#  -output-file ../ourfonts/garamond/r/ring/YggD-R \
#  ../ourfonts/garamond/r/atlas/orig/ggmr26D

# Concat subset of ggmr26D with Y-R (with remapped Y (J) to bracketleft),
# to test sizing.
# 
#set args -verbose -metafont -range Y-\[ \
#  -concat ../ourfonts/garamond/r/ring/Y-R \
#  -output-file ../ourfonts/garamond/r/ring/YggD-R \
#  ../ourfonts/garamond/r/atlas/orig/ggmr26D

# Concat ggmr26D with ring.
set args -verbose -metafont \
  -concat ../ourfonts/garamond/r/ring/ring-R \
  -output-file ../ourfonts/garamond/r/ring/ringgD \
  ../ourfonts/garamond/r/atlas/orig/ggmr26D

set args -verbose -pstype1 -pstype3 -mf $ourfonts/cm/cmr10.bzr \
  -encoding=texlatin \
  -ps-font-info FontName:ComputerModern-Roman,UniqueID:75 \
  -ccc=test/aacute
