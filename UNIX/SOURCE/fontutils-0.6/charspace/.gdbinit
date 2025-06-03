directory ../gf
directory ../pk
directory ../tfm
directory ../lib

define redo
symbol-file charspace
exec-file charspace
end

#set args  -fontdimens 2:10 -verbose -dpi 1200 \
#	  -input-file ggmr30 -encoding-file gnulatin ggmr30o
#set args   -verbose -dpi 1200 -sbi-files ggmc30a,ggmr30 \
#	   -encoding-file gnulcomp ../ourfonts/ggmc30a.1200gf
#set args  -fontdimens 2:10 -verbose -dpi 1200 \
#	  -sbi-files ggmr30p -encoding-file gnulatin ../ourfonts/ggmr30p
#set args   -verbose -dpi 1200 -sbi-files ggmc30a,ggmr30p \
#	   -encoding-file gnulcomp ../ourfonts/ggmc30a.1200gf
#set args  -verbose -dpi 1200 -sbi-files ggmr30p -encoding-file gnulatin \
#	   ../ourfonts/ggmr30p
#set args  -verbose -dpi 1200 -sbi-files ggmr30q -encoding-file gnulatin \
#	  -output-file ../ourfonts/ggmr30q ../ourfonts/ggmr30p
#set args  -verbose -dpi 1200 -sbi-files ggmr30r -encoding-file gnulatin \
#	  -output-file ../ourfonts/ggmr30r ../ourfonts/ggmr30q

# testing output files.
#set args  -verbose -dpi 1200 -sbi-files ggmc30a -encoding-file gnulatin \
#	  -output-file foo ../ourfonts/ggmc30a
#set args  -verbose -dpi 1200 -sbi-files ggmc30a -encoding-file gnulatin \
#	  -output-file foo.bar ../ourfonts/ggmc30a

#set args  -verbose -dpi 1200 -sbi-files ggmr30s -encoding-file gnulatin \
#	  -output-file ../ourfonts/ggmr30t ../ourfonts/ggmr30s
#set args  -verbose -dpi 1200 -sbi-files ggmr30t -encoding-file gnulatin \
#	  -output-file ../ourfonts/ggmr30u ../ourfonts/ggmr30t

# change en size
#set args -verbose -dpi 1200 -sbi-files ggmr26a -encoding-file gnulatin \
#  	 -output-file ../ourfonts/ggmr26b ../ourfonts/ggmr26a

# change number displacement to be en, adjust some letters.
#set args -verbose -dpi 1200 -sbi-files ggmr26c -encoding-file gnulatin \
#  	 -output-file ../ourfonts/ggmr26d ../ourfonts/ggmr26c

#set args -verbose \
#  -cmi=../ourfonts/garamond/example/ggmr.1200cmi \
#  -output=../ourfonts/garamond/example/ggmr30c \
#  ../ourfonts/garamond/example/ggmr30b.1200 

# Actually, the encoding is ``AEFMNOT only'', but it doesn't matter.
#set args -verbose -encoding=ascii -cmi-files=logo10 logo10 -o=xlogo10

#set args -verbose cmr10 -o=xcmr10

set args -verbose \
  -cmi-file ../ourfonts/garamond/r/atlas/ggmr.1200cmi \
  -encoding gnulatin \
  -output-file foo \
  ../ourfonts/garamond/r/atlas/orig/ggmr26d.1200
