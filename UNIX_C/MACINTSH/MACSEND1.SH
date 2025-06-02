#! /bin/sh
# Shellscript for transmitting groups of files to a Macintosh via macput.
# Invokes /bin/sh upon entry.
# Files must be in three parts created by xbin : .rsrc, .data, .info
# 
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
mesg n
for f in $*
do
	if [ -f $f.* ]  && [ -r $f.* ] 
	then 
		if [ -r *.data ] && [ -r *.rsrc ] && [ -r *.info ] 
		then
			echo " macput $f"
			macput $f
		fi
	fi
	if [ -f $f ] && [ -r $f ]
	then 
		echo " macput -u $f"
		macput -u $f
	fi
done
echo -n " "
echo "Downloads done!"
echo -n 
echo -n 
echo -n 
mesg y
