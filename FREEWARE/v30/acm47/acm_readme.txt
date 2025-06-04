ACM Flight Simulator,  revision 4.6


What is ACM?
------------

ACM is a LAN-oriented, multiplayer aerial combat simulation.  My main design
objective was to provide source code that could be easily compiled and
executed on a wide variety of platforms.  To that end, acm is written entirely
in C, exploiting the programming features of Unix, X11, and the BSD socket
interface.

Players engage in air to air combat against one another using heat seeking
missiles and cannons.

ACM is implemented as two programs.  The first, named "acm", is a small
program that, when invoked, starts a flight session on a given workstation.
The second component, named "acms", is a server process that manages the
multiplayer environment.  It also manages the flight simulation and
display management that is required.  Most players will prefer to run the
acms process on a back-end server system on their local area network.  Players
at client workstations can then invoke the acm program to begin play.


Riley Rainey (rainey@netcom.com OR riley@atria.com)
Dallas, Texas
November 20, 1994


Building ACM:
-------------

ACM has been reworked to be built with the help of `configure'.
To build ACM, type:

	$ ./configure
	$ make

On systems where X11 is installed in an unusual location, configure may have
trouble locating the X include files and libraries (you'll know that
because the compiler will complain that it can't find files like "X11/X.h").
If this problem crops up, you can help configure to set up the
Makefiles by telling it where to find X11:

	$ x_includes=/usr/myX11/include; export x_includes
	$ x_libraries=/usr/myX11/lib; export x_libraries
	$ ./configure
	$ make

By default, configure will compile ACM using gcc if gcc is found in your PATH
environment variable.  Some combinations of gcc and OS platforms have problems
with ACM (gcc 2.2.2 seems to have problems on SunOS and HP-UX, for example).
You can override configure's selection of gcc:

	$ CC=cc; export CC
	$ ./configure
	$ make

ACM can then be executed by typing:

	$ cd src; acm

When you are ready to install ACM on your system:

	# make install


ODD FLIGHT BEHAVIOR (AND CORE DUMPS):

On slower or heavily loaded systems, you may experience odd
accelerations (say, to Mach 6 or more) or other strange behavior (such as
uncontrollable "porpoising").  Acms may even dump core in this situation.
To avoid this, you can recompile ACM without the "real-time deltaT" extensions
and avoid the problem:

	$ REAL_DELTA_T=no; export REAL_DELTA_T
	$ ./configure
	$ make clean
	$ make


IMPORTANT NOTE REGARDING IMAKE:

Imakefiles have been kept for historical reasons.  xmkmf should be used only
As a last resort in the case that `configure' fails to work properly.


What's different with revision 4.7:
-----------------------------------

A problem with V library 3-D to 2-D mapping has been fixed.

A problem with the V library could cause this esoteric error message on some
X servers:
	
	BadPixmap (invalid Pixmap parameter)

The fix to this problem was identified by Brent Holland.


What's different with revision 4.6:
-----------------------------------

Sound support has been added for many HP platforms.  Philippe-Andre Prindeville
made the changes required for ACM to function with the HP-UX audio API.
Configure no longer assumes that the NCD netaudio library is the only only one
named "libaudio.a".

Cloud layers can now be used to affect combat conditions.  A single cloud
top/base can be set.  This solid cloud layer is opaque to IR emissions, so
you will have a hard time shooting down an opponent who is on the other side
of the clouds.  The default cloud base is 2800 feet, tops at 6200 feet.
It is not currently possible to "turn-off" clouds.

Portability problems with ACM's usage of the random number generators rand()
and random() have been fixed.

A problem with architectures where a "long" is not the size of an "int" (DEC
Alphas) has been fixed.


What's different with revision 4.5:
-----------------------------------

The HUD compass and HSI were pointing to true north rather than to magnetic
north.  This has been fixed.  The magnetic variation has been set to
eight degrees east (it can be changed in navaid.c).  This means that the
18-36 runways at both team's airfields are actually 17-35 runways; the runway
heading for the ILS 17 approach at team one's airfield is 172 degrees;  the
runway heading for the ILS 36 approach at team two's airfield is 352 degrees.

A field has been added to the navaid description structure to account for
the fact that ILS localizers have a variable beam width (three to six degrees
according to the FAA Airman's Information Manual).  The two ILS approaches
have been adjusted so that their beam widths are 700 feet wide at the runway
threshold (per the standard).

Porting problems with Solaris 2.3 and DEC Alpha OSF/1 have been fixed.
Thanks to Bill Davidson, Peter Asenstorfer, and S.Y. Ni.

A problem in the VOR TO/FROM logic was pointed out by Jeff Westbrook and has
been fixed.

A problem with monochrome support was fixed by Jeff Westbrook.

ACM's configuration facility was rebuilt using autoconf 1.9.


What's different with revision 4.4:
-----------------------------------

This is essentially a patch release for 4.3.

Simple visual depth cueing has been added to ACM.  Depth cueing is only
activated on color or grey-scale screens that have at least 8 bit planes.
Visibility can be set using the "-visibility" switch on the acms command
line.  It produces interesting, if slow, results when used with any of the
Hawaiian Island scenes.  I intend to improve the performance and visual
fidelity of this feature.

A bug in the depth cueing color blending code has been fixed.

Fixes to src/makefile.in and V/lib/Makefile.in have been applied.  A
"make depend" target has been added.


What's different with revision 4.3:
-----------------------------------

Drones will now maneuver and attempt to attack you with missiles.
The original inspiration and code changes were supplied by 
Jason Nyberg (nyberg@ctron.com).  The drone tracking method was modified 
and the missile launch logic was created by Riley Rainey.

A bug in the AIM-9 time-to-target HUD calculation was fixed.

Support for NCD's netaudio software has been added.  The NCD netaudio software
is free and available in source form via anonymous ftp from ftp.x.org.

Netaudio 1.1 does not allow the user to switch the audio output port on
a Sun IPX (an IPX has both an internal speaker and an external jack for
headphones or separate speakers).  You can switch ports by using the SunOS
/usr/demo/SOUND/x_soundtool before executing acms.

The scene file format has been modified slightly.  The third line is
now the color to be used for the "ground".

The kauai-scene, oahu-scene and maui-scene files display environments
around those Hawaiian islands.  Terrain elevation data was extracted from
USGS one degree Digital Elevation Model (DEM) data files.  Because of the
complexity of these scenes, rendering speed is unacceptably slow on all but
the fastest contemporary workstations.

A bug in the V library backface clipping code was fixed by Tim Love.

A bug in the V library was resulting in oddly colored instrument panels at
seemingly random times.  This has been fixed.

A problem with the landing gear parameters on the F-16 has been fixed.

Acms 4.2 core dumped on AIX systems.  This has been fixed.  Tom McConnell
supplied the changes.


What's different with revision 4.2:
-----------------------------------

Several nits reported by CodeCenter have been repaired by Brad Bass.

Makefiles now use the $(MFLAGS) macro, where needed.


What's different with revision 4.1:
-----------------------------------

Ultrix and HP-UX make(1) was having problems with the $(MAKEFLAGS) directive in
V/Makefile.in.  It has been removed.  It's removal should not affect the
build process.

The Ultrix 4.3 shell was having problems with "./configure".  The problem
has been fixed.

A problem in "./Makefile" that would cause a mis-identification of the
default target has been fixed.

ClipPoly.c was unused in ACM 4, but has been updated so that it compiles.

A problem in inventory.c that was causing the nose-wheel steering mode to
stay enabled at all times has been fixed.  This problem was pointed-out by
      _
Mats Lofkvist (d87-mal@nada.kth.se).


What's different with revision 4.0:
-----------------------------------

Combat Simulation
-----------------

Mig-29's are now the default team two plane.

The HUD display has been improved.

A bug in the missile tracking algorithm that would induce misses when the
target was moving at a high speed has been fixed.

Two chase-plane view methods have been added. The 'N' key now displays a very
primitive chase plane view. Tetsuji Rai (tetsuji@rai.juice.or.jp or
74610.1537@compuserve.com) has added a much more sophisticated version that
has been enhanced by the author.  You can start a separate chase plane window
by entering this command:

	% acm -chase <player-id>

The player-id's for all active ACM players can be retrieved using:

	% acm -list

The keypad controls the aspect of the view: (2) front, (8) rear, (4) right,
(6) left. To remember these, simply imagine wanting to look that direction
and see the chased aircraft.  The (J) key moves you closer to the aircraft,
(K) moves you farther away.

You can always chase a plane that is owned by you.  You can only chase others
when the acms server is started with the "-c" flag:

	% acms -c &

ACM now supports situation recording and playback.  The "{" key will start
recording, the "}" key ends it.  The positions of all aircraft existing
during the recording period will be saved in the file "./back_box_output".
That file can be played back by moving it to the file "./black_box_input".
Activate the playback by pressing the "[" key while you're in ACM.

DME, VOR's, Localizers, and Glide Slopes have been added.  An HSI has been
added to provide VOR and ILS readouts.  

Two experimental navaids have been programmed in:

	A VOR/DME facility located southeast of the team 1's airport(109.0 MHz).

	An ILS system for runway 17 at team 1's airport (110.0 MHz).  The
	runway heading is 172 degrees.

To activate the HSI, press the 'R' key.  Each press of the 'R' key cycles
through RADAR -> HSI -> RADAR STANDBY.  The Omni Bearing Selector is moved
20 degrees with the '7' key, and one degree with the '8' key.  The '9' key
increases the NAV frequency by 1 MHz, the '0' key increases it by 0.05 MHz.
The selected frequency is displayed at the lower right-hand section of the HSI.

If you want to shoot an ILS approach, I suggest the following settings:

LEVEL FLIGHT:
	power	45 %RPM
	flaps	20
	gear	up
	speed brake  deployed as needed
	speed	~ 150 kts

ON GLIDE SLOPE:
	power	35 %RPM
	flaps	20
	gear	down
	speed brake  retracted
	speed	140-150 kts
	AOA	10-11 degrees

When you shoot the ILS 17 approach, don't forget to set the OBS dial to
the runway heading (172 degrees).  Otherwise, you'll get a very
confusing picture of which way to fly.
This approach configuration gives you a good opportunity to practice
soft landings.


Flight Simulation
-----------------

Engine response is no longer instantaneous.  Engine RPM (power) will
increase and decrease gradually to a newly selected setting.

The aircraft simulation model has been changed substantially.  Tim Tessin
uncovered a bug in pm.c that was substantially degrading the performance
of all aircraft in high angle of attack situations. It's been fixed.

Several problems with the lift-curve determination have been fixed.
The lift coefficient is now determined by interpolation.  The lift contribution
of flaps has been modified to be more consistent with reality.

Aircraft control surface effectiveness has been increased substantially.
I believe that this increase in consistent with a more realistic simulation.

ACM now models aircraft motion on the ground more accurately.  ACM actually
models the spring and motion damping effects of the landing gear struts, as
well as the contribution of ground friction by the wheels.

A problem with the roll dynamics of the flight simulation model pointed out
by Tim Tessin has been fixed.  The roll model (no pun intended) has been
substantially re-worked to be able to use standard NACA stability
derivatives to describe aircraft roll performance.

The pitch and yaw dynamics have been modified to use NACA derivatives.

The outside view has been modified.

The F-16 flight model has been modified to be more well behaved; lateral 
and longitudinal motion damping coefficients have been increased.


The Graphics
------------

The graphics have been re-vamped to be much faster on color workstations.
Monochrome graphics are slower.

Object clipping has been improved in the V library to improve rendering
performance.



The Guts of ACM
---------------

ACM now participates in the X11 WM_CLOSEDOWN and WM_DELETE_WINDOW protocols.

The directory structure has been modified to get the object description
files out of the source directory.  Object files now all have a '.obj'
extension.  The 'make install' target has been improved.

Aircraft descriptions are now defined in the file "objects/inventory".

Terrain layout is now described using a "scene" file.  Acms accepts a
"-s scene-file-name" command line option. The file "default-scene" describes
the default layout.

A periodic alarm facility has been added in the guts of ACM.  See alarm.[hc]
for details.  An example of its use can be found in navaid.c; it is used
to periodically update DME readouts on all aircraft.  It is designed to be
compatible with variants of ACM that modify deltaT on the fly.

The radar display has been modified to be more closely coupled to the V
library.  This code was borrowed from the ACM 4.0 version.

If no geometry specification is supplied, the ACM server now computes a
reasonable window geometry based on the screen's width.

Keypad view selection has been added for Sun systems with some help from
Keith Fredericks (keith@cray.com).



What's different with revision 2.4:
-----------------------------------

Missiles now use a more realistic target tracking method, known as proportional
navigation guidance.

Sustained aircraft load factors at high speeds are now limited to a range of
about +10.0 to -4.0 gees.

More math portability problems have been worked-around.  src/manifest.h
contains the definition NEEDS_COPYSIGN.  You may comment this out if your
system supports the IEEE copysign function.

This revision has been tested with X11R5.


What's different with revision 2.3:
-----------------------------------

This revision repairs several portability problems uncovered in revision 2.2.

A problem with IEEE math in interpolate.c has been worked-around.  Invalid
interpolation requests (which shouldn't happen in ACM) will now result in
messages being printed to standard error.

Several debugging code fragments are now only conditionally compiled. See
manifest.h for more information.

A problem controlling aircraft resupply could generate unexpected errors
from a system's sqrt function ("sqrt: domain error", for example). It has been
fixed.


What's different with revision 2.2:
-----------------------------------

Explosions now appear in the skies of ACM.  Explosions will appear when an
aircraft is struck by a missile or cannon shell.  Players go out with a
bang, too.

The flight simulation model has been tweaked to allow for spins.  I don't
think you can really spin an F-16, but if you'd like to try it in ACM, do
this: build up some airspeed in level flight and then go vertical (pitch
up to ninety degrees, or thereabouts).  Cut the throttle and afterburner
and keep the nose pointed vertically.  Eventually, the F-16 will begin to
fall back to earth -- and typically enter a spin.  The only recovery 
procedure that I've tried that works is to apply full forward (down)
elevator to break the stall; then re-apply power.

Elevator trim has been added.  The (J) key sets to the elevator trim to
be equal to the current elevator setting.  The (U) key resets it to the
normal takeoff setting.  Typically, you'll get the aircraft into the
pitch configuration that you want (e.g. level flight) and then press the
(J) key -- then move the elevator control to the neutral position
(the center of the HUD).

Aircraft now have functioning landing gear.  The (G) key acts as the landing
gear handle.  A landing gear handle and status lights are located to the right
of the radar set.

Each aircraft now carries a limited number of air-to-air missiles (eight, to
be precise). 

Aircraft can now be re-armed and re-fueled on the ground.
To do this, a player must land successfully at the team's home airfield
and come to a complete stop.  After an interval of not greater than 30
seconds, the aircraft will be completely re-armed and refueled.  Aircraft
damage is repaired, as well.

Acm's reaction to the resizing of its window has been improved substantially.

The code handling the HUD altitude, airspeed, and heading ladders has been
improved.

A problem with acms dumping core when it cannot access the graphics
description files has been fixed.  Thanks go to Mark Moraes.

Acm now makes an effort to select an appropriate X Visual to run in.  These
changes are based on code supplied by Mark Hall.

A problem with unwanted NoExpose events on monochrome displays was fixed
by Georges Lauri.

Acm's usage of the X DISPLAY environment variable has been improved with the
help of some code from Michael Pall.  Now systems where the ACM server is
running on the same system as the X server can get "unix:*" and "local:*"
connections.


GENERAL NOTES
-------------

This software is divided into two major parts: the 3-D graphics routines
and the flight simulator itself.  The 3-D stuff is contained in the V
directory.  V/lib holds the library itself.  V/test contains a program
that can be used to test your port of the V library.

`src' contains the remainder of the flight simulator.

Comments and suggestions to improve this software are welcome.

Several "features" in this revision of the software:

	* flaps and landing gear can be lowered at rediculously high speeds.

	* you can fly through mountains.

	* engines do not flame-out.

README: patchlevel-4.5
