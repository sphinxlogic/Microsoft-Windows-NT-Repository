From riacs!eos!agate!bionet!bloom-beacon!tut.cis.ohio-state.edu!mailrus!rutgers!rochester!udel!princeton!siemens!demon!fwb Wed Jan 18 08:20:47 PST 1989

The Soundex routine <19090@agate.BERKELEY.EDU> posted by Dean Pentcheff
(dean@violet.berkeley.edu) can be made into a simple spelling corrector.  I
did this to play around with his routine, and thought others might find it
amusing.  There is probably a better method for making a spelling
corrector, but this was fun.

Copyright?  Nah.

Step 1.  Modify Dean's program to print the soundex code along with the
	 input string.  Replace the printf statement with:

		printf("%s %s", soundex(instring), instring);

	 Remember to remove the \n in the original format string!

Step 2.	 Compile soundex.c

		cc -O -DTESTPROG soundex.c -o soundex

Step 3.  [Optional] Compute and store soundex values for all words in
	 /usr/dict/words

		soundex < /usr/dict/words | sort > words.soundex

Step 4.  Cut out the following shell script.  Make it executable, and have
	 fun.

#------------- cut here for mispel -----------------
#! /bin/sh
# A simple spelling corrector.
# Usage:
#   mispel word
#
# All words from the system dictionary with the same soundex code as word
# are printed to the standard output.

DICT=/WHERE/THIS/LIVES/words.soundex
SOUNDEX=/WHERE/THAT/LIVES/soundex

# calculate the soundex value for the input word and put it in $1
set `echo $1 | $SOUNDEX`

# did you cache the soundex dictionary?
if [ -f $DICT ]; then
    # look up the word in the cached dictionary
    look -d $1 $DICT | awk '{ print $2 }'
else
    # calculate the soundex value for the system dictionary and print 
    # all words which match the input word
    $SOUNDEX < /usr/dict/words | fgrep $1 | awk '{ print $2 }'
fi
#------------- end of mispel -----------------

--
Frederic W. Brehm	Siemens Corporate Research	Princeton, NJ  
fwb@demon.siemens.com	-or-	...!princeton!siemens!demon!fwb


