				      PIPE
				an X/Motif game

				 Michael Muller
				  Version 2.2

    You have unbundled the sources for pipe.

    Included in this tar file with the sources are a makefile to build 
    pipe and the vax-ultrix executables.

    The executables assume that the .uid, help, and high score files 
    are in your present working directory.  To enable pipe to be run 
    from a different directory, edit the makefile and re-compile.

    Disclaimer: This was my first program using X *or* Motif *or* 
    an event-driven toolkit *or* C.  I learned a lot, but I now know 
    that I did many things the hard way.  I've since gone back and fixed 
    some of these oddities, but some are so entangled in the code 
    that I don't have the time or patience to work them out.  For 
    exmaple, I used global variables instead of passing values to 
    callbacks.  This can be avoided, but it's a lot of work because 
    Motif callbacks take only one parameter.

    Files in this save set:

      Makefile         -- The makefile to build pipe.
      README           -- This file.
      pipe.c           -- The C source.
      pipe_widgets.uil -- The UIL containing the window, menus, etc...
      pipe_icons.uil   -- The UIL containing pictures of pipes.
      pipe_help        -- How to play.  Available on-line or "more" it.
      pipe             -- The game compiled.
      pipe_widgets.uid -- The window, menus, etc., compiled.
      pipe_icons.uid   -- The pictures of pipes, compiled.

    This code has been tested and should work on ULTRIX, VMS, color, and 
    black & white systems.  Please report any problems.

    Feel free to reuse, distribute (as long as you don't profit), 
    or enhance this program.  Tell me if you do!

    History:
    01-Oct-1990	    Creation
    18-Jun-1991	    Version 1.0 (first stable version)
    08-Jul-1991	    Version 2.0 (major enhancements)
    19-Jul-1991	    Version 2.1 (bug fixes)
    08-Feb-1992     Version 2.2 (small clean-ups for CD distribution)

    Done since V1.0:
	o Reservoirs.
	o Bonus pipes.
        o Varying board width and height.
	o Fast flow.
	o Add a fluid flow countdown to the status panel.
	o Created a speedometer for the fluid.
	o Enable starting from a certain level.
	o Implement abort game.
	o On-line help.
	o High Scores maintained.
	o Weight the likelihood of each pipe appearing for each level.
	o Clear the preview window before each game.
        o The screen no longer flashes when the drawing area is [de]sensitized.
        o RNG without modulus (doesn't seem to help, but thanks to Dave Miller)
	o Change "bummer." to "psyche!" for level over.
	o Add application icon (thanks to Dick Schoeller).
	o You no longer lose your head start if you pause before flow begins.
	o Unix-compatible (thanks to Paul Douglas).
	o Stripes now visible in B&W
	o B&W pixels returned correctly (thanks to JP Tessier)
	o A number of unix bugs fixed (thanks to Marc Evans)
	o Level increments *after* Psyche! is pressed.
	o Tiles can't be placed out of bounds.
        o One UIL file for icons -- b&w uses same as color!

    XUI window manager incompatibilties:
        o The playingArea displays wherever it feels like. (XUI only)
	o The pause menu option doesn't iconize. (XUI only)

    Bugs:
	o The preview window is wider than the tiles. (used to be XUI only)

    Things to do/suggestions:
	o Make fast flow flow faster (calling w/out timeouts doesn't work).
	o Implement wrap-around edges in certain places for some levels.
	o What about the bonus Tetris-like level?  (LucasFilms did it...)
	o Create application resources for use with resource file
	o Finish pipe ("drain")
	o Two flows
	o T pipes
	o Customization (preferences)
	o Level info read from file for customization
	o Make erasing quicker on the starting levels
	o Allow customization of pipe colors
