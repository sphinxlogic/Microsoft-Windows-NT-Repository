XStar by Wayne Schlitt, wayne@backbone.UUCP (wayne@cse.unl.edu)


This program "solves" the n-body problem, and displays the results on
the screen.  It starts by putting a bunch of stars on the screen, and
then it lets the inter-body gravitational forces move the stars
around.  The result is a lot of neat wandering paths, as the stars
interact and collide.

XStar can be used to animate the root window, as a screen saver or just
to display stuff in a regular window.



All comments, bug reports, bug fixes, enhancements, etc are welcome.
Send them to me at wayne@backbone.uucp or wayne@cse.unl.edu.


This program is really a heavily modified version of XGrav, which was
written by David Flater (dave@case50.ncsl.nist.gov) and posted to
alt.sources on 1/21/95.  I liked the program enough that I was really
interested in it, but I didn't like it enough to leave it alone.  The
idea was Dave's, but I don't think too much of his code has been left
unchanged.  There is probably more untouched code from XSwarm, which
Dave used to implement the X port of his n-body problem solving code.

Like XGrav, any claim to this program that I have (which isn't much)
is under the GNU General Public License.  Have fun with it.


Changes to XStar
----------------
Version 1.0.0	3/12/95
	* initial release to alt.sources

Version 1.1.0	3/22/95
	* second release to alt.sources
	* the runtime 'd' command could cause a divide by zero
	* fixed up an error msg when allocating colors
	* portability patches for HPUX and AIXV3
	* GPL license is included in the shar file
	* switched to the rand48() family of random number generators
	* a 'multi-color' mode as added so that each star could have
          it's own color.  Can be enabled with the -m option.
	  (From Jeffrey Mogul <mogul@pa.dec.com>)
	* run time commands 'm' and 'r' to toggle between multi-colored
	  and rainbow modes. (From Jeffrey Mogul <mogul@pa.dec.com>)
	* Jeff Mogul also created a man page for XStar.
	* Don't start collapsars at exactly the COLLAPSAR mass to avoid
	  FP rounding problems.
	* I found, to my shock and horror, that the code I inherited from
	  xgrav implemented some sort of Aristotelean physics where
	  heavier objects fell faster than lighter objects.  Converting to
	  Newtonian physics made things slightly slower, but I think
	  more interesting.
	* XGrav calculated the new locations of the stars by using the
	  formula:  x(t) = x0 + v t
	  This assumes that the acceleration is constant (it isn't).
	  Instead, I now use:  x(t) = x0 + v t + 1/2 a t^2 + 1/(2*3) (Dt a) t^3
	  This assumes that the second derivative of the acceleration is
	  constant (it isn't).  At least it is _closer_ to being correct.
	  Even dropping the term with the first derivative of the acceleration
	  makes the movement _much_ less accurate.
	* changing to Newtonian physics makes the experimental bounce code
	  work much better, but it still doesn't work very well.
	* added the -a and -m options to let people tweak the FV and FM
	  values without having to recompile the system.



Changes from XGrav
------------------

Performance improvements and bug fixes:

- XGrav used some sort of Aristotelian physics where heavier objects fell
  faster than light objects.  I switched to Newtonian physics.

- Lots of work was done to make the calculation of new star positions
  more efficient.  It looked like some of the collisions weren't
  working right, so I threw out Dave's calculations and re-derived
  my own.  I found out that Dave's code was correct, but slightly
  slower than mine and the problems with collisions was elsewhere.

  Anyway, after a lot of rewriting, the net result is that XStar can
  calculate many more interactions per second than XGrav could.

- The gravitational constant has been replaced with fudge factors that
  are easier to adjust.  These fudge factors have been cranked up to
  make the simulation many times more accurately.

  This increased accuracy prevents stars from shooting off at very
  high velocities instead of colliding like they should and it also
  allows collapsars to be orbited easier.  As a result, collapsars no
  longer eat most of the stars in the system, and thus the runtime 'd'
  command is much more useful.

  This change slows things down, but the other speed increases more than
  offset this.

- I changed the star trails from thick lines to single pixel width
  lines so that you can see more intricate interactions between the
  stars. I also buffer the output to the X server, and I don't
  check for X events as often.  This reduces the cpu consumed by the
  kernel and the X server by a huge amount.  This, in turn, allows you
  to run xstar at a very low priority and it won't have as big of an
  impact on other processes.  (The kernel and the X server always run
  at a high priority.)

- The default number of stars has been upped from 9 to 15.

- The delay option (-D msec) no longer slows the program down by a
  huge amount, so it is actually fairly useful now.

- Stars that float too far away from the center are now forgotten
  about.  They don't have much affect when they are way out there and
  they just slow things down.

The net result is that XStar simulates more stars, more accurately and
much quicker than XGrav could.


New bells and whistles  (XGrav vs XStar 1.0.0)
----------------------

- XStar tries to detect when "interesting" things are no longer
  happening on the screen and creates a new star system automatically.
  It no longer waits for a fixed amount of time before restarting.

- The initial star configuration code is much more sophisticated.  It
  randomly creates 0 to 4 collapsars and adjusts the masses and
  velocities of the initial stars to compensate.  It puts a
  counter-clockwise spin on the system so that the stars tend to orbit
  each other instead of just collapsing toward the center or flying
  off the screen.  The initial configuration seems to be fairly good
  for a wide range of the initial number of stars (-b num).

- The -v, -R command line options are new, as are the run time
  commands n, +, - and p.  The support of -r using the -g values is
  also new.



Known Bugs and Missing Features
-------------------------------

- There are several constants many that really depend on how fast your
  computer is and how good your compiler and X server are.  These may
  need to be tweaked to get them right for your system. Unfortunately,
  they are not documented well, if documented at all. Some could be
  calculated at runtime, others should be selectable by command line
  options.

- The initial configuration of the stars works well for a limited
  range of the initial number of stars, and works ok for a larger
  range.  I just guessed at many of the formulas that I used, but
  exact relations could be derived.  It would just be a lot of work to
  do and it is easier to tweak constants when you move away from the
  default of 15 stars.

- I would like to add an option so that stars bounce off each other
  like billiard balls instead of smushing together.  I think this would
  be a neat effect, but the but my first attempts have not been to
  successful.  There is some experimental code in XStar that can be
  enabled, but I it has lots of problems.

  It has also been suggested (michael@dsndata.com) that it would be neat if
  collisions could cause the stars to break apart into several peices.

  Maybe we could do something like if they collision is very soft, the
  particles stick together, but if they collide with a fair amount of
  force, they bounce.  If they collide with a lot of force, then they
  break apart.  This could let a single "system" evolve for quite a while. 

- It would be nice if XStar detected heavy loads on the system and
  slowed down the updates during those times.  Unfortunately, it looks
  like about the most portable way of determining the loadavg would be
  to run uptime and parse the output.  This is kind of expensive to do.

  So, XStar works ok as a background process in the root window, but
  sometimes you have to hit 'p' to let other things get done sooner.

- I wasn't able to get the save-under feature to work in the root
  window, nor did the screen saver work correctly for me.  I think
  this is a problem with my (old) X Server, rather than a bug in the
  code.

- The colors of the rainbow don't seem to be too even.  The red, green
  and blue seem to dominate over the yellow, purple and cyan.

- I think it would be neat to plug in the values for the solar system
  and see what happens.  By using the value for G and any arbitrary
  number of pixel distances, we can obtain conversion factors for the
  mass and distances.  Time is defined such that each iteration takes
  one unit of time

- It might be nice to use the mouse to create stars or temporary collapsars.
