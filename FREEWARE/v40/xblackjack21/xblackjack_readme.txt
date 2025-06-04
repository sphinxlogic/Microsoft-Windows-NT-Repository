
Xblackjack is a MOTIF/OLIT based tool constructed to get you ready for the
casino.  It was inspired by a book called "Beat the Dealer" by Edward O.
Thorp, Ph.D. of UCLA.  A number of important statistics are maintained for
display, and used by the program to implement Thorp's "Complete Point System"
(high-low system).

Beginners can learn a basic strategy which doesn't require counting.  The
best strategy employs a simple card counting technique which can be used to
distinguish favorable situations.  Avid players can learn to compute Thorp's
high-low index, which takes the number of unseen cards into account.

The index is used by the program to suggest a bet, indicate when you should
buy insurance, and whether you should stand or hit, etc.  You can test the
strategy using a slider to control "autoplay mode".  Pressing the message
button below the playing area provides hints; the current count, index
and strategy table are revealed.

To build and run: 

	vi Imakefile
	  <indicate you want OLIT, if you don't want Motif>
	  <identify non-standard install targets, if any>
	xmkmf
	make World
	xrdb -merge XBlackjack.ad
	xblackjack

to install:

	make install install.man

This version includes changes from GUX (Guy Renard) grenard@elrond.stg.trw.com,
which make placing a bet under OLIT easier.  I
allows you to vary the number of decks and starting bankroll
through resources (see XBlackjack.ad).  It's hardcoded to play Reno rules;
you can double down on 10 or 11 only, and dealer hits soft 17.

In addition to incorporating the patches that appeared on export:

	xblackjack-2.0.patch1
	xblackjack-2.0.patch2

this version includes some OLIT specific changes contributed by Guy
Renard (grenard@elrond.stg.trw.com), which make the bet scale easier to
use:  "Now a person can see what they are about to bet  before releasing
the select button.  I think the deal button is still useful since There
is still no way to simply click on the slider and get the default bet"
(introduced in patch2).

Most lint errors and warnings generated compiling under IRIX have been
fixed in this version.

Alain Brossard (brossard@siisun.epfl.ch) caught an XtAddCallback in
AddCallbacks() that needed an additional NULL argument (client_data) to
compile using gcc on a sun4.

Please direct comments, suggestions, and patches to:
---------------------   o  ---------------------------------------------
Kirk Erickson         <(        Silicon Graphics Inc., Mountain View, CA
kirke@sgi.com          [\.                               uunet!sgi!kirke
