#! /bin/sh
# Shellscript for transmitting groups of files to a Macintosh via macput.
# Invokes /bin/sh upon entry.
# Files in three parts as created by xbin or macget (.rsrc, .data, .info)
# will be sent as one unit. ( References to such files may either be by
# prefix or by complete list (e.g. f or f.* )
# Otherwise:
# Files with the extension .rsrc will be sent as resource files using
#	the -r option.
# Files with the extension .data will be sent as data files using
#	the -d option.
# All other files will be sent as text files using the -u option.
# This shellscript will ignore directories and files that are unreadable.
# Wildcards may be used.  Thus, to transmit all files beginning with
#	a capital letter, use "macsend [A-Z]*"
# Upon termination of the operation, the Macintosh bell will ring
#	three times.  This is your clue to wake up and see what you got!
# Be sure that macput is in your path, otherwise the shellscript won't work!
#
# This shellscript is based on the original macsend.
#	
# Modifications done on April 20, 1985 by 
#		
# Chris Borton 
# University of California at San Diego.
# Undergraduate CS
# sdcsvax!sdcc6!ix924
#
# Further modifications to cover more cases 4/27 by
# Barry Eynon (barry!playfair@Score)
#
mesg n
for f in $*
do
	case $f in
	*.data) g=`basename $f .data`
		if [ -r $g.data ] && [ -r $g.rsrc ] && [ -r $g.info ]
		then
			echo -n " "
		elif [ -f $f ] && [ -r $f ]
		then
			echo " macput -d $f"
			macput -d $f
		fi
	;;
	*.rsrc) g=`basename $f .rsrc`
		if [ -r $g.data ] && [ -r $g.rsrc ] && [ -r $g.info ]
		then
			echo -n " "
		elif [ -f $f ] && [ -r $f ]
		then
			echo " macput -r $f"
			macput -r $f
		fi
	;;
	*.info) g=`basename $f .info`
		if [ -r $g.data ] && [ -r $g.rsrc ] && [ -r $g.info ]
		then
			echo " macput $g"
			macput $g
		elif [ -f $f ] && [ -r $f ]
		then
			echo " macput -u $f"
			macput -u $f
		fi
	;;
	*)
		if [ -r $f.data ] && [ -r $f.rsrc ] && [ -r $f.info ]
		then
			echo " macput $f"
			macput $f
		elif [ -f $f ] && [ -r $f ]
		then
			echo " macput -u $f"
			macput -u $f
		fi
	;;
	esac
done
echo -n " "
echo "Downloads done!"
echo -n 
echo -n 
echo -n 
mesg y
p
