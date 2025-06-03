directory ../bzr
directory ../gf
directory ../widgets
directory ../lib
directory ../pk
directory ../tfm

define redo
symbol-file limn
exec-file limn
end

#handle 5 nostop noprint
#set env DISPLAY localhost:0
#directory /usr/local/src/X/mit/lib/Xaw
#directory /usr/local/src/X/mit/lib/Xt

# The 67 for corner-always comes from the serifs at the foot of the `T',
# which are otherwise asymmetrical.
# The line-threshold value comes from the serifs on the `h' turning
# (inappropriately) into straight lines.
# The corner-threshold value comes from the pixel at the join of the
# crossbar and left leg of the `A' (it also helps the M, N, V, W).
# The filter-iterations value comes from looking at the `O'.
# 
#set args -verbose +log +metafont bsq18 \
#+corner-threshold=105 \
#+corner-always-threshold=67 \
#+line-threshold=.14 \
#-tangent-surround=5 \
#-corner-surround=5 \
#-reparameterize-improve=3 \
#-error-threshold=0.5 \
#-filter-iterations=12 \
#-filter-secondary-surround=6 \
#-subdivide-threshold=.1 \
#-range=\'-\' \
#-do-display  \
#
#set args -verbose -log chab30 \
#-line-threshold=.2 \
#-filter-iterations=10 \
#-filter-surround=4 \
#-range=%-%
#-do-display  \

# set args -verbose -do-display  trap -dpi=72270


# +dpi 1200 +line-threshold 16.0
# +corner-surround 16 +error-threshold 35.0 +reparameterize-threshold 60.0 \

# The subdivide parameters make the stem of the t flow more nicely into
# the tip.
# 
# -filter-secondary-surround=6 
#set args -verbose ../ourfonts/ggmr26d.1200 \
#  -align-threshold=.5 \
#  -corner-surround=4 -corner-threshold=100 \
#  -display-pixel-size=5 -display-rectangle-size=4 \
#  -error-threshold=1.0 \
#  -filter-iterations=3 -filter-surround=6 \
#  -filter-alternative-surround=3 -filter-epsilon=5.0 \
#  -filter-percent=30 \
#  -line-reversion-threshold=.009 \
#  -reparameterize-improve=7 -reparameterize-threshold=5.0 \
#  -subdivide-search=20 -subdivide-surround=6 -subdivide-threshold=.1 \
#  -tangent-surround=6							\
#  -output-file ggmr26D
## -range=a-z \
##  -do-display  \
##  -log \

# Get Y from ring for sizing.
#set args -verbose -output-file ../ourfonts/garamond/r/ring/Y-R \
# -dpi 1200 ../ourfonts/garamond/r/ring/Y-r

# Do remapped Y ([).
#set args -verbose -output-file ../ourfonts/garamond/r/ring/Y-Rb \
#  -dpi 1200 ../ourfonts/garamond/r/ring/Y-rb

# Do ring-r->ring-R (accents we want).
set args -verbose -dpi 1200 \
  -output-file ../ourfonts/garamond/r/ring/ring-R \
  ../ourfonts/garamond/r/ring/ring-r

set args -verbose -log ggsbi10.1200 -range=37-37
