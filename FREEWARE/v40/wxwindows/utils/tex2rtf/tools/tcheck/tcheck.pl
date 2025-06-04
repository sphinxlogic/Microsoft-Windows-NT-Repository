;# File:    tcheck.pl
;# Author:  Georg Rehfeld, directly converted from Julian Smart's check.awk
;#          and slightly enhanced
;# Date:    26th August 1996
;# Purpose: perl script to flag some common errors in input to Tex2RTF.
;#          Users are welcome to add to this -- please mail me your changes!
;#          J.Smart@ed.ac.uk (or georg@deerwood.hanse.de)
;# Usage:   You need perl4 pl36 or better. Sources may be ftp'ed from many sites.
;#          There is a executable perl5 version for NT and 95 available 
;#          at http://info.hip.com/ntperl/
;#          Use: perl tcheck.pl filename.tex [file2.tex ...]

;# Initialisation
$INTABULAR = 0;
@CriticalWords = ('bf', 'it', 'rm', 'normal', 'small', 'large', 'Large', 'LARGE', 'huge', 'Huge', 'HUGE');

;# break long lines if output is to terminal
$brk = ' ';
-t STDERR && ($brk = "\n\t");

while (<>) {

	;# don't look at commented out lines
	/^\%/ && next;

	;# Check for missing mandatory document style
	/documentstyle\[.*\]$/ &&
		warn "Error in $ARGV($.):\tmissing major document style.${brk}Add e.g. {report} after documentstyle[...]\n";

	;# Labels should immediately follow section headings or captions
	/^\\label/ &&
		warn "Error in $ARGV($.):\tlabel should DIRECTLY follow section or caption, not on the next line.\n";

	;# Common bracket confusion
	/\{(\\[a-zA-Z]*)\{/ &&
		warn "Error in $ARGV($.):\tbraces will confuse Tex2RTF.${brk}Try a space after the command '$1'.\n";

	;# Check for correct braces before or after critical words
	foreach $word (@CriticalWords) {

		;# Middle of line
;#		/[^\{]\\$word[^A-Za-z\{]/ &&
;#			warn "Error in $ARGV($.):\tno brace before or after \\$word.\n";

		;# Start of line
;#		/^\\$word[^A-Za-z\{]/ &&
;#			warn "Error in $ARGV($.):\tno brace before or after \\$word.\n";

		;# On its own
;#		/^\\$word$/ &&
;#			warn "Error in $ARGV($.):\tno brace before or after \\$word.\n";

		;# Everywhere ... is this really similar to above 3 patterns?
		/[^\{]?\\$word[^A-Za-z\{]?/ &&
			warn "Error in $ARGV($.):\tno brace before or after \\$word.\n";

	}

	;# \input must be at start of line
	/.+\\input/ &&
		warn "Error in $ARGV($.):\t\\input must be at beginning of line.\n";

	;# comments in the middle of line seems to confuse tex2RTF, at last on \\input lines
	/^[^\%].*%.+/ &&
		warn "Error in $ARGV($.):\tcomment char in middle of line.${brk}Comments should be on their own lines.\n";

	;# don't know about that '...}%', looks like split command, seems to be not allowed?
	/^[^\%].*%$/ &&
		warn "Error in $ARGV($.):\tsplit command not supported.${brk}Please remove intervening newline and %.\n";

	;# Opening brace at start of line
	/^\{[^\\]/ &&
		warn "Warning in $ARGV($.):\tpossible split command.${brk}If so, remove intervening newline.\n";

	;# Start tabular environment
	/begin\{tabular\}/ &&
	($INTABULAR = 1);

	;# End tabular environment
	/end\{tabular\}/ &&
	($INTABULAR = 0);

	;# Check that we don't have \\ on its own within tabular environment.
	;# It will crash Tex2RTF.
	/^\\\\/ && ($INTABULAR == 1) &&
		warn "Error in $ARGV($.):\t\\\\ on its own.${brk}Insert appropriate number of ampersands before end of row marker.\n";

	;# start over with line number reset to 1
	close(ARGV) if eof;
}


