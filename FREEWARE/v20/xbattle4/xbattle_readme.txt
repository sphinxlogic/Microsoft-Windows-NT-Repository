xbattle 	concurrent multi-player battle strategy game

		Steve Lehar  (slehar@cns.bu.edu)
		Greg  Lesher (lesher@cns.bu.edu)

DESCRIPTION

  This is  version  4.0 of  xbattle, by    Greg Lesher,   based on the
  original  by Steve  Lehar  released  in  1991  and including certain
  enhancements, modifications and bug  fixes suggested  by a number of
  contributers from all over the world.  The  new changes are outlined
  below.   The  latest  version  can  be obtained  by anonymous ftp to
  park.bu.edu (cd pub/xbattle) in both shar and tar.Z formats.

  xbattle  is  a  concurrent  multi-player battle strategy   game that
  captures the dynamics of a wide range  of military scenarios.  It is
  based on X windows, which you must have installed to  run this game.
  Players  play  from separate  displays,   and commands are  executed
  concurrently, i.e.  the players do  not take "turns", they all issue
  their commands simultaneously.  There can be  any number of players,
  and each player is assigned to a color team (or black,  white & gray
  for  monochrome monitors).  The  game  board is a matrix of  squares
  that can be occupied by  colored troops, and the aim  of the game is
  to eliminate the enemy from the board by  attacking squares occupied
  enemy troops.

  To get  the feel  of the  game, you can   run  the tutorials (called
  "tutorial1" and "tutorial2") that are supplied with the game.  These
  are shell scripts that run on unix systems and start up  a series of
  small  example games that  you can play   around  with  to learn the
  various options available with the game.  If  you are  not on a unix
  system, print out  the  tutorials and type  in the  command lines by
  hand. If you  are interested in  the philosophical and   game design
  issues of  xbattle,  or want to  get immediate  notice of the latest
  changes     bugs,     and    bug     fixes,  send      email      to
  xbattle_request@park.bu.edu and  we   will  put you  on  the xbattle
  mailing list.

INSTALLATION

  After unpacking  the shell archives, create  a  makefile  using  the
  command "xmkmf", then compile using the command "make".


DIFFERENCES BETWEEN VERSION VERSION 3.2 AND VERSION 4.0

  Added option  "-hex" whereby  the  game board  comes  up as hexagons
  instead of squares.  This makes for a more interesting geometry, but
  slows the game down noticably.  Also, this option will not work with
  the combination of "-horizon" AND "-wrap", among other restrictions.

  Changed the options "-fill <n>" and "-dig <n>"  to take command line
  arguments.   These options allow you  to modify the terrain, raising
  or lowering hill elevations, or digging  and filling seas.  With the
  new arguments, it costs  <n>  full troopsquares to completely dig or
  fill a  sea.  A partially filled sea  is displayed  as  a sea square
  with a square  island in the middle  whose size proportional  to the
  number of fills left.  The  argument for  "-dig" and "-fill" must be
  the same value, otherwise  the last argument supplied is  applied to
  both, i.e.  the first is ignored.

  Added option "-attack" whereby hitting 'a' when pointing to an enemy
  square causes all your adjacent squares to attack that square.  This
  helps synchronize simultaneous attacks.

  Added option "-area" whereby the troop strength is represented by a
  troop square whose area is proportional to the strength, rather than
  the length of the side, which is the default.

  Established new  default  that  grid appears automatically; replaced
  "-grid" option with "-nogrid" option if you require no grid.

  Changed the names of gray1 and gray3 to light and dark, representing
  light and dark shades  of gray.  (Gray2,  the "stealth" color  still
  exists, but is not documented).

  Added a new feature- xbattle now  prints out  the random number seed
  used in a particular game, to  make it easier to  reproduce the same
  game again using the parameter "-seed <n>".

  Added a new command key 'z' which cancels  all command vectors, like
  clicking in the  center of  the  gamesquare.  This  is  particularly
  useful with the new hex option.

  Allow fractional speed settings like "-speed  0.5".  Very slow games
  favor new players, and  make for a more  chess-like game of strategy
  and planning rather than an arcade style shoot-em-up.

  Defined  default   board and   square  sizes more   appropriate  for
  beginners (big squares, small boards).

  Changed the RGB color definitions for Cyan, Yellow and Green to more
  pleasing combinations, although they  are no  longer the true colors
  of those names.

  Added compile time option MULTIFLUSH (default FALSE) whereby command
  vectors are displayed immediately  after the command  is clicked, to
  give the user  immediate confirmation of  the command.  This feature
  noticably slows the game however.

  Bug fix: Previously when  playing with  colored armies on monochrome
  monitor  (for example color  "-red"  which appears as  black  with a
  letter "R") the team color displayed  on the  xbattle window was the
  monochrome color (or black, in  this example) instead  of the actual
  color (or red).  This has now been fixed.

  Bug   fix: Fixed  marching  bug which  accidentally enabled bounding
  movement window.

  Bug fix: Fixed movement bug which shortchanged fractional moves < 1.


COPYRIGHT

  Copied   from the GNU Emacs  'copyleft'  policy.  Basically, you are
  free to copy, modify and distribute this  program as long as  you do
  not sell it or use it in a product that you sell.

  			COPYING POLICIES

  1. You may copy and  distribute  verbatim  copies of  xbattle source
code as you receive it, in any medium, provided that you conspicuously
and  appropriately publish   on  each copy  a   valid copyright notice
"Copyright (C)    1991 Steve  Lehar"   (or   with  whatever   year  is
appropriate); keep intact the notices on all  files that refer to this
License Agreement  and to the  absence of  any warranty;  and give any
other recipients  of  the xbattle program   a  copy of  this   License
Agreement along with  the program.   You may charge a distribution fee
for the physical act of transferring a copy.

  2. You may modify your copy or copies of xbattle source  code or any
portion of it,  and copy and distribute  such  modifications under the
terms of Paragraph 1 above, provided that you also do the following:

    a) cause the modified files to carry prominent notices stating
    that you changed the files and the date of any change; and

    b) cause the whole of any work that you distribute or publish,
    that in whole or in part contains or is a derivative of xbattle
    or any part thereof, to be licensed at no charge to all third
    parties on terms identical to those contained in this License
    Agreement (except that you may choose to grant more extensive
    warranty protection to some or all third parties, at your option).

    c) You may charge a distribution fee for the physical act of
    transferring a copy, and you may at your option offer warranty
    protection in exchange for a fee.

Mere aggregation of another unrelated program with this program (or its
derivative) on a volume of a storage or distribution medium does not bring
the other program under the scope of these terms.

  3. You may copy and distribute xbattle (or a portion or derivative of it,
under Paragraph 2) in object code or executable form under the terms of
Paragraphs 1 and 2 above provided that you also do one of the following:

    a) accompany it with the complete corresponding machine-readable
    source code, which must be distributed under the terms of
    Paragraphs 1 and 2 above; or,

    b) accompany it with a written offer, valid for at least three
    years, to give any third party free (except for a nominal
    shipping charge) a complete machine-readable copy of the
    corresponding source code, to be distributed under the terms of
    Paragraphs 1 and 2 above; or,

    c) accompany it with the information you received as to where the
    corresponding source code may be obtained.  (This alternative is
    allowed only for noncommercial distribution and only if you
    received the program in object code or executable form alone.)

For an executable file, complete source code means all the source code for
all modules it contains; but, as a special exception, it need not include
source code for modules which are standard libraries that accompany the
operating system on which the executable file runs.

  4. You may not copy, sublicense, distribute or transfer xbattle
except as expressly provided under this License Agreement.  Any attempt
otherwise to copy, sublicense, distribute or transfer xbattle is void and
your rights to use xbattle under this License agreement shall be
automatically terminated.  However, parties who have received computer
software programs from you with this License Agreement will not have
their licenses terminated so long as such parties remain in full compliance.

			   NO WARRANTY

  BECAUSE XBATTLE IS LICENSED FREE OF CHARGE, WE PROVIDE ABSOLUTELY NO
WARRANTY, TO THE EXTENT PERMITTED BY APPLICABLE STATE LAW.  THE ENTIRE
RISK AS TO THE  QUALITY AND PERFORMANCE  OF THE PROGRAM IS  WITH  YOU.
SHOULD THE XBATTLE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL
NECESSARY SERVICING, REPAIR OR CORRECTION.

 IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW WILL STEVE LEHAR OR ANY
OTHER  PARTY  WHO MAY MODIFY AND   REDISTRIBUTE  XBATTLE  AS PERMITTED
ABOVE, BE LIABLE TO YOU FOR DAMAGES,  INCLUDING ANY LOST PROFITS, LOST
MONIES, OR OTHER SPECIAL, INCIDENTAL  OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE  (INCLUDING BUT NOT LIMITED TO LOSS
OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY THIRD
PARTIES OR  A FAILURE  OF  THE  PROGRAM TO OPERATE  WITH  PROGRAMS NOT
DISTRIBUTED BY STEVE LEHAR) THE PROGRAM, EVEN IF YOU HAVE BEEN ADVISED
OF THE POSSIBILITY OF SUCH  DAMAGES, OR FOR  ANY CLAIM   BY ANY  OTHER
PARTY.

