From pur-ee!j.cc.purdue.edu!i.cc.purdue.edu!purdue!gatech!bloom-beacon!husc6!necntc!ncoast!allbery Sun Jan 31 20:48:05 EST 1988

Comp.sources.misc: Volume 2, Issue 26
Submitted-By: Eric S. Raymond <eric@snark.UUCP>
Archive-Name: 3b1-cc

Here's a front end for cc that I use constantly. It acts exactly like cc but
links things to use shared libraries. I posted an earlier version last
year, but the -L option handling turned out to have a bug in it. Also, this
version links things into the fast-loading -F (0413) format -- thanks to Dennis
McCunney for passing me the incredibly obscure little hack necessary to get
that to work (believe it or not, you trigger it with the order of the link
arguments).

# ccs -- compile with shared libraries for AT&T 7300 or 3B1 (version 1.2)
#        Written to replace cc by Eric Raymond {cbmvax!snark!eric}

CC=/bin/cc		# Standard C compile program
LD=ld			# Standard loader
OSTART=/lib/crt0s.o	# Standard startup code
PSTART=/lib/mcrt0s.o	# Monitored startup code for profiling

# Find shared version of libraries
if [ -f shlib.ifile ]
then
	SHLIB=shlib.ifile	# Use local customized version if it exists
else
	SHLIB=/lib/shlib.ifile	# Otherwise use standard ones
fi

DEBUG=		# Set this to 'echo' to see actions

srclist= objlist= intermediates=
start=$OSTART
lflag=1

while [ $# != 0 ]
do
	source= linkarg=
	case $1 in

	# Options
	-W*) linkarg=-`expr $1 : "-W\(.*\)"` ;;
	-p)  start=$PSTART ;;
	-c)  lflag=0 ;;
	-[fgOSEPBtPCUDTIH]*) source=$1 ;;
	-y)  source="$1 $2" ; shift ;;
	-[efou] | -VS) linkarg=$1 ; shift ; linkarg="$linkarg $1" ;;
	-[almrsxzMNV]*|-L*) linkarg=$1 ;;
	-*) ;;

	# File types
	*.[cs])
		stem=`expr $1 : "\(.*\).[cs]"` ;
		source=$1 linkarg=${stem}.o
		intermediates="$intermediates ${stem}.o"
		;;

	# Everything else
	*) source=$1 linkarg=$1 ;;

	esac
	shift
	objlist="$objlist $linkarg"
	srclist="$srclist $source"
done

$DEBUG $CC -c $srclist		# Compile everything, suppressing linking

# Now, if there was no -c option, link-edit the results
if [ $lflag != 0 ]
then
	$DEBUG $LD $objlist $start $SHLIB
	rm -f $intermediates
fi
# ccs ends here

You can call this as ccs or copy it to /usr/bin/cc. It will give you cleaner
cleans, whiter whites, build strong bodies twelve ways, and make your programs
load and execute maybe 10%-15% faster.

-- 
      Eric S. Raymond
      UUCP:  {{seismo,ihnp4,rutgers}!cbmvax,sdcrdcf!burdvax,vu-vlsi}!snark!eric
      Post:  22 South Warren Avenue, Malvern, PA 19355    Phone: (215)-296-5718


