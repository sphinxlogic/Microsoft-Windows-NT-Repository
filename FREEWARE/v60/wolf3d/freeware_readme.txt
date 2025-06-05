WOLF3D, GAMES, Wolfenstein 3D for OpenVMS

This is a port of the classic shoot-em-up Wolfenstein 3D. The code this port
was derived from was collected from:

	http://www.icculus.org/wolf3d/wolf3d-20011028.tar.gz

Running:

In order to run Wolf3D the WOLF3D_DATA logical needs to be defined. It
must point to the location of the Wolf3D data files, eg:

	$ DEFINE WOLF3D_DATA DUA0:[WOLF3D.DATA]

The files for shareware Wolf3D are provided in the [.DATA] directory. If you
have Spear of Destiny or the full version of Wolf3D then read the linking
instruction.

If you intend to load/save games you need to define the logical WOLF3D_SAVE,
eg:

	$ DEFINE WOLF3D_SAVE DUA0:[WOLF3D.SAVE]


Read FREEWARE_DEMO.TXT for more information.

Linking:

Linking is simple. Change to the directory [.SRC] and execute
BUILD_WOLF3D.COM. If there are objects in the directory it will pick them up
and use them instead of compiling from source.

Because the different versions of Wolfenstein require different executables
I have included the objects for all four versions in ZIP files:

For Alpha:
    WOLF3D_WL1_AXP.ZIP	=> Wolfenstein 3D Shareware
    WOLF3D_WL6_AXP.ZIP	=> Wolfenstein 3D
    WOLF3D_SDM_AXP.ZIP	=> Spear of Destiny Demo
    WOLF3D_SOD_AXP.ZIP	=> Spear of Destiny

For VAX:
    WOLF3D_WL1_VAX.ZIP	=> Wolfenstein 3D Shareware
    WOLF3D_WL6_VAX.ZIP	=> Wolfenstein 3D
    WOLF3D_SDM_VAX.ZIP	=> Spear of Destiny Demo
    WOLF3D_SOD_VAX.ZIP	=> Spear of Destiny

Simply unzip the contents of the relevant file into [.SRC] and execute the
build procedure:

    $ UNZIP WOLF3D_xxx_yyy.ZIP -d [.SRC]
    $ SET DEFAULT [.SRC}
    $ @BUILD_WOLF3D

Building:

To build Wolf3D from source you will need to delete all the objects in [.SRC],
set the correct version number and execute the build procedure:

    $ SET DEFAULT [.SRC]
    $ DELETE *.AXP_*;*	! For Alpha users
    $ DELETE *.VAX_*;*	! For VAX users

To set the version edit VERSION.H and modify the value of WMODE. From the
source:

From VERSION.H:

    #define WMODE 0

    /* Change WMODE to point to the executable you would like to build: */
    /* WL1 = 0 Wolfenstein 3D Shareware */
    /* WL6 = 1 Wolfenstein 3D */
    /* SDM = 2 Spear of Destiny Demo */
    /* SOD = 3 Spear of Destiny */

Then build Wolf3D:

    $ @BUILD_WOLF3D

Documentation:

The [.DOC] directory contains the documentation from the original package.
There is also a Wolfenstein 3D FAQ which contains cheats, etc.

Help:

From "-help":

        -debugmode
        -demotest
        -help           prints this message
        -nojoy          disables joystick input
        -nomouse        disables mouse input
        -noshm          disables X MIT-SHM support
        -nowait 	skip intro screen
        -version        prints software version
        -x2             increase window size x2, 640x480(default is 320x200)
        -x3             increase window size x3, 960x600

I pulled the following out of some of the source:

	Escape		Main Menu
	F1/Help		Help
	F2		Save game
	F3		Load game
	F4		Configure sound
	F5		Change view
	F6		Configure controls
	F7		End game
	F8		Quick save
	F9		Quick load
	F10		Quit
	Do		Pause

Build system:

The Alpha objects were built on:

    Alpha Evaluation Board 166 5/333 128Mb RAM
    OpenVMS Alpha V7.3-1
    DECwindows 1.3
    Compaq C V6.5-001
    2x Powerstorm 3D30

The Alpha executable was built on:

    Alpha XP1000 6/500
    OpeVMS Alpha V7.3-1
    DECwindows 1.2-6
    2x 3DLabs Oxygen VX1

The VAX objects and executable were built on:

    VAXstation 4000-90
    OpenVMS VAX V7.3
    DECwindows 1.2-6
    Compaq C V6.4-005
    LCSPX 8-plane graphics module

Changes:

I have made some changes to the code as listed below:

    o. Added -x2 and -x3 support to the X driver.
    o. Implemented WOLF3D_DATA logical. Everything used to have to be in
       the same directory.
    o. Implemented WOLF3D_SAVE logical. Somewhere to put your save files.
    o. Added -help
    o. Added support for all Fx keys, Help, Do and LK escape key.
    o. Wrote some macro to handle quadword arithmetic need for FixedByFrac()
       on the VAX, uses EMUL.
    o. Added XBell PC speaker emulation, a little bit slow, but still fun.

I would like to add sound support and the ability to change versions without
having to re-compile/re-link. Maybe one day :)

Problems:

There is an issue with the VAX version where keypresses seem to disappear.
I haven't had a chance to really look into this because it's so close to
the Freeware submission date (other things have come up).

There is no sound other than PC Speaker emulation using XBell(). If some one
wants to write a VAX or MMOV driver (or both) that would be good. I haven't
got any sound on any of my Alphas. I did try to get the Multia going, but the
MMOV server crashes with an access violation everytime I try to access the
device. The virtual address is 4 (from memory), this might mean something to
someone. If you are one such someone contact me, I wouldn't mind knowing
what is going on.

As far as I know this still doesn't work with the Oxygen card. If someone
wants to either fix this or let me know what is going on, so I can fix it,
it would be much appreciated.

Misc:

The glob() and globfree() come from the GNV source for the make utility. They
have been modified slightly so they can exist on their own and run on VAX.

Support:

This was/is a lunchtime/Friday afternoon project. I don't know how quickly I
will be able to respond to problems, etc. but I will try to answer any
queries (provided I'm not repeating what's in this document). Just
remember that I have work, a life and my own projects to work on also.

Supported graphics:

The following graphics cards have been tested and are known to work with
Wolfenstein 3D:

    ATI Mach64 (PB2GA-FA/FB)............not tested, but should work (I did
					DOOM with this card)
    Powerstorm 3D30 (PBXGB-AA)..........use with -noshm
    9FX Motion 331 (PB2GA-JC/JD)........use with -noshm (JA/JB should work)
    21030 (Multia/UBD TGA adapter)......works completely
    LCSPX 8-plane (54-21795-01).........work no problems
    ELSA GLoria Synergy (PBXGK-BB)......use with -noshm

The above list should be correct, if there is one you would like to add, or
something is incorrect then contact me at the address below.

Comments, problems, etc:

Tim Sneddon
tesneddon (at) bigpond.com
