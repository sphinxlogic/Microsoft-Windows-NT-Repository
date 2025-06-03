directory ../gf
directory ../pbm
directory ../pk
directory ../lib

define redo
symbol-file imageto
exec-file imageto
end

# Bodoni.
#set args -verbose -encoding-file=encoding.bdk -baselines=285,301,399 \
#          -clean-threshold=.4 -print-guidelines bdk
#set args -verbose -encoding-file=encoding.bdb -baselines 265,280,296 bdb
# Some junk is between the first two real lines, and a black bar is after
# the last.  Hence the two extra baselines.
#set args -verbose -encoding-file=encoding.bdki -baselines=286,12,293,322,1 \
#  -clean-threshold=25 -range=I-I -nchars=1 -print-clean-info bdki
#set args -verbose -encoding-file=encoding.tmi -print-guidelines \
#  -print-clean-info -design-size=72 -clean-threshold=30 -nchars=6 tmi

# Atlas  (15 June 92)
#set args -verbose -strips  ../ourfonts/garamond/atlas/ggmr.img
#set args -verbose -baselines=327,338,342 -clean-threshold=23 \
#  -print-guidelines -print-clean-info \
#  -designsize=30 -encoding=gnulatin \
#  -output-file=../ourfonts/garamond/atlas/ggmr \
#  ../ourfonts/garamond/atlas/ggmr.img

#set args -verbose -baselines=1,335,1,340,333 -clean-threshold=31 \
#  -design-size=30 ../images/ggmri
#set args -verbose -strips ../images/ggmi
#set args -verbose -baselines=154,156,155 \
#  -design-size=14 ../images/ggmb
#set args -verbose -baselines=152,158,160,161,162 -clean-threshold=36 \
#  -design-size=14 ../images/ggmbi

# 09
#set args -print-guidelines -design-size=30 -dpi 1200 -input-format=img \
#  -verbose -info-file ../data/09.ifi -baselines 57 ../img/09.img
#set args -print-guidelines -design-size=30 -dpi 1200 -input-format=img \
#  -verbose -info-file ../data/09.ifi -output-file ../fonts/0930a \
#  -baselines 57 ../img/09.img
#set args -print-guidelines -design-size=30 -dpi 1200 -input-format=img \
#  -verbose -info-file ctst -output-file ../fonts/ggmc ../img/ctst.img
#set args -print-guidelines -design-size=30 -dpi 1200 -input-format=img \
#  -verbose -info-file 09 ../fonts/09.img
set args -verbose -strips -output-file ../ourfonts/garamond/r/09/09 \
   ../ourfonts/garamond/r/09/09.img

# ctst
#set args -design-size=30 -dpi 1200 -input-format=img -verbose  \
#     -strips  ../ourfonts/img/ctst.img
#set args -design-size=30 -dpi 1200 -input-format=img -verbose \
#    -info-file ../ourfonts/ifi/ctst ../ourfonts/img/ctst.img

# fract
#set args -verbose -strips -output-file fract ../ourfonts/garamond/img/fract.img
#set args -verbose -dpi 1200 -print-guidelines -print-clean-info \
#  -output-file fract -nchars 5 \
#  ../ourfonts/garamond/img/fract.img > fract.log
#set args -verbose -dpi 1200 -print-guidelines -print-clean-info \
#  -output-file fract ../ourfonts/garamond/img/fract.img
#set args -verbose -strips -output-file adobe ../ourfonts/garamond/img/adobe.img
#set args -verbose -input-format pbm -trace-scanlines -dpi 1 test.pbm

#set args -verbose -dpi 1200 -print-guidelines \
#  -output-file fract ../ourfonts/garamond/img/fract.img \
#set args -verbose -dpi 1200 -print-guidelines -baselines 155\
#  -output-file fract ../ourfonts/garamond/img/fract.img

# tsch
#set args -verbose -strips -output-file tsch \
#  ../ourfonts/garamond/img/tsch.img
#set args -verbose -dpi 1200 -print-guidelines \
#  -output-file tsch ../ourfonts/garamond/img/tsch.img
#set args -verbose -dpi 1200 -print-guidelines -baselines 192,0,20\
#  -output-file tsch ../ourfonts/garamond/img/tsch.img

# After adding -encoding option:
# ring
#set args -verbose -print-guidelines -print-clean-info -encoding gnulatin \
#  -baselines 59 -info-file ../ourfonts/garamond/r/ring/ring.ifi \
#  -output-file ../ourfonts/garamond/r/ring/ring \
#  ../ourfonts/garamond/r/ring/ring.img
#set args -verbose -strips -output-file ../ourfonts/garamond/r/ring/ring
#  ../ourfonts/garamond/r/ring/ring.img

# adobe
#set args -verbose -print-guidelines -print-clean-info -encoding gnulatin \
#  -info-file ../ourfonts/garamond/r/adobe/adobe.ifi \
#  -output-file ../ourfonts/garamond/r/adobe/adobe \
#   ../ourfonts/garamond/r/adobe/adobe.img

# chart
#set args -verbose -strips -output-file ../ourfonts/garamond/r/chart/chart \
#   ../ourfonts/garamond/r/chart/chart.img
#set args -verbose -print-guidelines -print-clean-info -encoding gnulatin \
#  -info-file ../ourfonts/garamond/r/chart/chart.ifi \
#  -output-file ../ourfonts/garamond/r/chart/chart \
#  -clean-threshold 38 -nchars 8 ../ourfonts/garamond/r/chart/chart.img

# percent
#set args -verbose -strips -output-file ../ourfonts/garamond/r/percent/perc \
#   ../ourfonts/garamond/r/percent/perc.img
#set args -verbose -print-guidelines -print-clean-info -encoding gnulatin \
#  -info-file ../ourfonts/garamond/r/percent/perc.ifi \
#  -output-file ../ourfonts/garamond/r/percent/perc \
#  ../ourfonts/garamond/r/percent/perc.img

# finding out about bb info with atlas (20 Sep 92)
#set args -verbose -print-guidelines -print-clean-info -nchars=26 \
#-baselines=121 \
#-info=$garamond/atlas/first -encoding=gnulatin $garamond/atlas/ggmr.img

set args -verbose ../gsrenderfont/cmr.pbm -dpi=300 -enc=dvips
