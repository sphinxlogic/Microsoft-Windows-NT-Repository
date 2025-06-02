From pur-ee!j.cc.purdue.edu!i.cc.purdue.edu!purdue!decwrl!ames!necntc!ncoast!allbery Wed Jun  8 08:19:09 EST 1988

comp.sources.misc: Volume 3, Issue 42
Submitted-By: "Steven Pemberton" <steven@cwi.nl.UUCP>
Archive-Name: which2

Here is the version of 'which' that I use. Advantages:
	- twice as fast as the ucb version
	- still a shell script
	- tells you about ALL versions reachable from your path,
	  not just the first
	- silent if it finds nothing, so you can use it in `` expansions
		e.g. for f in `which which`; do file $f; done
	- (this is the biggy:) wildcards work!
		which '*uu*' gives all commands reachable with uu in the name.
		which '?'    gives all 1 character commands
		which '*'    gives *all* commands.
	  Don't forget the quotes if you use wildcards.

It only works if 'test -x' works for you.

Steven Pemberton, CWI, Amsterdam; steven@cwi.nl

Here's the source:

-------------------- Cut here -------------------------
# A faster version of which, that also prints ALL versions in your PATH,
# and accepts wildcards, e.g.: which '*uu*'. Silent if nothing found.
# Only works if test -x works...

case $# in
0) echo Usage: $0 cmd ...; exit 1;;
esac

dirs=`echo $PATH|sed 's/^:/. /
     s/:$/ ./
     s/::/ . /g
     s/:/ /g'`

for cmd
do
   for d in $dirs
   do
      for file in $d/$cmd
      do
         if test -x $file -a ! -d $file
         then echo $file
         fi
      done
   done
done


