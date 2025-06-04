net3d - a networked, multiplayer 3d combat game

Copyright (C) 1994 by Jamie Cameron
thanatos@yoyo.cc.monash.edu.au
jcam2@ccds.cc.monash.edu.au


This is version 0.08 of net3d, a simple multiplayer game written for
Unix and X11. At the moment, I have only successfully tested net3d on
the following systems -

	Ultrix 4.4 / 4.3	(DECstation)
	OSF/1			(DEC Alpha)
	HP-UX
	SunOS
	Solaris			(Sun Sparc Classic)
	Irix 5.2		(SGI)

If you are using either of the above, then it should work. As for
anything else... if you get it to compile and run then tell me about it,
and any changes / bug fixes needed to get it up and running.


COMPILING NET3D

Net3d now uses an Imakefile, so it should be possible to compile by running:

	xmkmf

followed by:

	make

If nothing goes wrong, two executables will be produced, net3d and net3dserv.
net3dserv is the server for multiplayer games, and net3d the client
run by each player. If you have any problems compiling, mail me and tell
me what kind of system you are using, what the error was and (hopefully :)
how to fix it, so I can include the fix in the next version.

There are several compile-time options that can be changed in config.h,
that may have to be adjusted to get net3d to compile on your system.

The important ones are:
CPPPATH		- the full path and filename for the C pre-processor on
		  your system. Should be found automatically by Imake
USEINLINES	- determines if inline functions are used (only with gcc).
		  Some versions of gcc don't seem to work with them
		  properly.

Note - if you plan on modifying the code, then it will be necessary to run:

	makedepend *.c

to add include file dependencies to the makefile.


RUNNING NET3D

net3d can by played in each single or multiplayer modes. To start a
single player game, just type :

	net3d <vehicle files> ...

For example, if you type  net3d map2.v heli.v  then you will be flying
a helicopter around a simple 3d map. Controls for the vehicle you are
piloting will be displayed before the game starts, and are by default
something like :

rotate left              - left    rotate right             - right
accellerate              - up      decellerate              - down
climb                    - a       dive                     - z
halt                     - s
gun left                 - o       gun right                - p
gun up                   - j       gun down                 - i
fire                     - return  lock on                  - l
eject                    - j
quit                     - q
wireframe toggle         - w       build menu toggle        - b

long range view              - 1  view from stationary point   - 2
view from behind vehicle     - 3  inside view                  - 4
above view                   - 5  turret view                  - 6
short range view             - 7  missile view                 - 8
interesting thing view       - 9

Not all controls will be appropriate for all vehicles. For example, a
vehicle like a tank (in tank.v) cannot fly, so the climb and dive keys
have no effect. Similarly, in a vehicle with no separate gun turret,
the keys for contolling the turret won't do anything.

Using the controls, you can fly or drive a vehicle around the game
world. Most objects in the game, including the hills, will cause damage
if your vehicle hits them. Also, other computer controlled vehicles may
open fire on you or accidentally collide with you.

Locking on
----------
When in viewing mode 4 or 6, the lock key (usually 'l') will lock on to
the closest vehicle in your gunsights. A short description of the
vehicle you have locked onto is displayed, and if your vehicle is armed
with missiles, then any missiles fired will home in on the target you
have chosen. 

Leaving your vehicle
--------------------
At any time you can eject from the vehicle you are driving, if it is a
tank, helicopter or other similar piloted machine. Once you have
ejected, you will be controlling a small fish (until I can create
something better). Outside your vehicle you are extremely vulnerable,
however you can board any unoccupied vehicle and collect trees for later
use in building things. Leaving a helicopter or other flying machine at
a high altitude is not recommended.

Building things
---------------
The 'menu' key brings up a row of buttons along the bottom of the
screen, for building various types of walls. If you have enough
resources, then clicking the mouse button on one of the icons will
construct the item chosen in front of your current vehicle. In addition
to walls on various angles, it is also possible to build mines (which
explode when hit) and autocannons (which fire at the nearest target, apart
from the vehicle that created them). Resources for building can only be
obtained by collecting trees, as described above.

Head-up displays
----------------
When in view-mode 4 (pilot view) or 6 (gun view), the following are
displayed on screen :
	lock information	(top left)
	radar		 	(top right)
	hit points		(below radar)
	ammunition		(below hit points)
	resources		(below ammo)
	velocity		(bottom left)
	altitude		(top right)


VEHICLE FILES

Several vehicle files are provided with net3d, some of which define
vehicles for players to drive and some which define worlds with scenery
to fly/drive around. There are also files that combine a map and a
number of vehicles, for easily starting a multi-player battle.

The scenery files are:
	map.v		- A complex, mountainous world with several
			  buildings and roads, as well as a few strange
			  creatures inhabiting it. Good for multi-player
			  games on fast machines.
	map2.v		- A simple map, suitable for machines with less
			  processing power to throw around. 
	map3.v		- A coastal area, with a few buildings and flying
			  creatures. Good for air combat.

The battle files are:
	battle.v	- Fought on map.v, between a helicopter, two
			  tanks and a cosmosphere. Includes an initially
			  unmanned helicopter on a helipad in the top
			  right corner of the map.

	battle2.v	- Fought on map2.v, between a tank and two helicopters.
			  Good for a simple multi-player game.

	battle3.v	- Fought on map3.v, between 3 helicopters and a
			  speedboat. Also includes a large battleship
			  with 2 cannons a short distance out to sea.
	space.v		- A space battle between 3 starfighters. At the
			  moment there isn't much scenery in this one,
			  though this will change when I create a few
			  more suitable vehicles.

The vehicle files are:
	amigaball.v	-
	amigaball.def.v	- A bouncing red and white amiga beachball. Useful
			  for scenery, but not much else.

	eye.v		-
	eye.def.v	- A flying eyeball with a tail and wings. Armed
			  with a light tracer weapon firing from it's
			  tail.

	heli.v		-
	heli.def.v	- A small helicopter with gun turret, firing homing
			  missiles.

	tank.v		-
	tank.def.v	- A small tank with gun turret and reasonably
			  heavy armour, and armed with a cannon.

	boat.def.v	- A small speedboat with a roof mounted gun

	fish.def.v	- A large, flying fish. Used in map.v as scenery,
			  where it chases small birds around.

	house.def.v	- Scenery object, used in all the map files.

	cannon.def.v	- A large gun installation, used for scenery in 
			  map3.v

	glassfish.v	-
	glassfish.def.v	- Like fish.def.v, but made of transparent polygons

	m2.v		-
	m2.def.v	- A lightly armed armoured personell carrier. Has
			  a turret mounted machinegun.

	cosmosphere.def.v - A huge, flying cosmosphere, inspired by the
			    lunatic rantings of McElwaine. :)

	gunship.v	-
	gunship.def.v	- A more heavily armed helicopter, but without a
			  gun turret.

	ptero.v		-
	ptero.def.v	- A flying pterodactyl-like bird. Used as scenery
			  in map.v and map3.v

	asteroid.def.v	- A slowly spinning chunk of rock, used as scenery
			  in space.v

	stingship.v
	stingship.def.v	- A small spacegoing fighter, armed with a plama
			  ball cannon.

Note that vehicle files can be grouped into two types, definitions
(ending with .def.v) and usable vehicles (ending with .v). Typically,
the vehicle is fully defined and described in the .def.v file, with is
included in the .v file, and instantiated. All the .def.v files are used
in the various map and battle files to create vehicles or creatures
living in that particular world.

In addition to the .v files that can be used as command line arguments,
there is one additional file, extras.v, that is used by net3d. extras.v
is contains the definitions of all the bullets, walls, missiles, fireballs
and commonly used miscellaneous objects used in the game. However, extras.v 
is not read in by net3d at run time, and thus simply modifying it will
not change the appearance of objects in the game. Instead, the file is
converted to a .c file during compilation and linked into the final executable
program. This means that after changing extras.v, you must re-compile net3d
for the changes to take effect. 


STARTING A MULTI-PLAYER GAME

1) Choose a machine that you want to be the game server, and on it
   run :
	net3dserv <vehicle file> ...
   For example, to start a simple game, you might run
	net3dserv battle.v

2) Each person who wants to join the game should run :
	net3d <server> <name>
  on their machine, where <server> is the address of the machine the
  server was run on, and <name> is the name this player wants to use
  in the game. If <name> is omitted, then net3d uses your current
  login name.
  For example, to connect to a server on the machine foo.cc, using
  the name Joe Smeg in the game, type :
	net3d foo.cc Joe Smeg

3) The server will receive connections from players and display info
   about each new player on the screen. When return is hit, the server
   sends information about the game world and vehicles to each player,
   assigns vehicles to players and starts the game. If there are not
   enough vehicles for players, then vehicles are assigned on a 'first-
   come, first-served' basis.

4) Once the game begins, the players can fight it out until only one
   only remains, who will be informed of victory by a message in their
   window. Once that final player hits the 'quit' key, the server and
   all the clients will exit.
   

COMPATABILITY PROBLEMS

This version of net3d will almost certainly fail on anything other than
a 8-bit pseudocolour display, as it's the only kind of display I've got
access to. Hopefully, the next version will include support for 24-bit
displays, as well as grey-scale.
All the networking code in net3d makes use of tcp/ip sockets, so if your
version of unix doesn't support sockets then the multi-player version
won't work. Hopefully single player mode will, though.
Net3d also needs cpp to pre-process it's vehicle files, and looks for it
in /lib/cpp by default. If you don't have cpp, then you're out of luck.
If you do, but it's not in /lib, then change CPPPATH in config.h, and
re-compile.


HISTORY

version 0.01
	- net3d is first let loose on the net

version 0.02
	- fixed bugs and hp-ux compatability problems suggested by
	  R.K.Lloyd@csc.liv.ac.uk, such as division by zero if a vehicle
	  has max velocity zero, and core dumps when no vehicles are
	  given.

version 0.03
	- sphere polygons are now drawn correctly
	- fireballs stored in the extra vehicle file
	- the server displays the name of the machine each player connects from
	- clients don't kill the whole game when killed by ctrl-c
	- HP-UX compatability (hopefully :)
	- Sun compatability   (even more hopefully)
	- the 'single' variable renamed to 'single_player'
	- strerror() kludged up for suns which are missing this function

version 0.04
	- improved depthsorting algorithm
	- "ground none" option in map files
	- "stars" option in map files
	- Linux compatability (maybe?)
	- new .v files and weapons
	- fixed problem with dropping keystrokes

version 0.05
	- added 'scale' statement for vehicles in .v files
	- added a workaround for problems with mmult under linux
	- improved space.v a bit

version 0.06
	- minor release to fix bugs and warning messages

version 0.07
	- minor bug fixes

version 0.08
	- fixed core dumps under linux (at last)
	- added weapon and ammuntion prizes
	- fixed Solaris compile-time bugs
	- fixed a few other minor niggles
	- changes the way movement works internally
	- started on support for brains
	- added version number checking between clients and server

TODO

* make depth sort order settable in .v file
* support greyscale and 24-bit graphics
* creator info for maps
* add missions and some kind of points system
* add various interesting things to collect
* make movement vector-based
* create some Star Trek vehicles
* optimise the 3-d graphics routines (perhaps using shared memory)
* net3d home page?
* use lex / yacc for parsing the vehicle file
* clean up the source code :-)
* add texture mapped polygons 
* allow players to send one another messages
* improve the intelligence of computer-controlled vehicles
* anything else anyone suggests to me...


BUGS

* Matrix multiplication dumps core under linux
* Problems with depthsorting causing some polygons to dissapear / reappear
* Computer controlled vehicles are stupid


CONTRIBUTORS

 - acb				(the cosmosphere, pterodactyl, fish, chicken
				 and bouncing ball vehicles)
 - warthog  			(the m2 tank)
 - gothic			(useful suggestions for the 3d algorithms)
 - R.K.Lloyd@csc.liv.ac.uk	(HPUX combatability, bug fixes, Imakefile)
 - lendl@cosy.sbg.ac.at         (Sun & Linux compatability, bug fixes)
 - humpy			(Linux compatability)
 - germond@ensta.fr		(Bug reports and suggestions)
 - jucoons@ursula.uoregon.edu	(Bug reports)
 - jaggy@purplet.demon.co.uk	(Bug fixes for linux)
 - ma_lwk@stu.ust.hk		(Bug reports and help with Solaris)


LEGAL STUFF

Net3d is freely distributable and modifyable, on the condition that no
fee is charged for it's distribution (except for the cost of a disc or
other media). In other words, you can't sell net3d for profit.
Net3d comes with no warranty, so if something goes horribly wrong with
the program, causing your computer to explode... tough :).

