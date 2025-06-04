README.txt

3D Pong v.0.2

by Bill Kendrick
kendrick@zippy.sonoma.edu

New Breed Software
http://zippy.sonoma.edu/kendrick/nbs/

December, 1997


WHAT IS 3D PONG?

  3D Pong is an X-Window game for 1 or 2 players.  It's a 3D game based
  on the first arcade game, "Pong."  Like "Pong," it's a game played with
  a ball and paddles.  The object is to hit the ball and get it in the
  other player's goal.  Use your paddle to deflect the ball and keep it
  from getting in your goal.

  3D Pong has three game modes:

    1. Two player - requires two X servers
    2. One player - against a computer player
    3. Handball - one player against gravity


INSTALLING

  To install 3D Pong, edit the "makefile" as needed.  Predefined variables
  are declared for:

    CC   - Your C Compiler.  "gcc" is recommended.
    XLIB - The location and name of your X window library.

  From a telnet session, simply type "make".  The "3dpong" executable should
  be created.

  Some #define's are available in the "3dpong.c" source code if you really
  wish to tweak with the game.  They are fully commented.  If you know
  a little about programming, you can have fun with them...


RUNNING THE GAME

  To run the game, type "3dpong" followed by the following, depending on the
  game mode:

  Two player game:

    player 1's server
    player 2's server

    Example:  3dpong $DISPLAY server.domain.com:0.0
              3dpong server.domain.com:0.0 machine.domain.com:0.0

  One player game:

    player's server
    the word "computer"

    Example:  3dpong $DISPLAY computer
              3dpong machine.domain.com:0.0 computer

  Handball game:

    player's server

    Example:  3dpong $DISPLAY
              3dpong server.domain.com:0.0


  You can also specify the "gravity" to be used in the game.  In the
  one- and two-player ping-pong games, this defines gravity towards the
  "floor" of the game.  (The floor is marked with a divider in the middle
  between the two players's sides.)

  In the handball game, gravtiy is defined towards your goal/paddle.
  The minimum gravity is 0.1; negative gravity is not allowed.  If you
  enter a negative gravity, it's absolute value will be taken.  If the
  gravity is less than 0.1, 0.1 will be used.

  To specify gravity, append "-gravity value" or "-g value" to the
  command line.

    Example:  3dpong $DISPLAY -gravity 0.5
              3dpong server.domain.com:0.0 machine.domain.com:0.0 -g 0.5

  To play a game with a net (not available in "handball" mode), append
  "-net value" or "-n value" to the command line.  The net appears in the
  middle of the game arena, where the marker normally is.  If the ball hits
  the net, it bounces back with full force.  You can specify a net height up
  to 0.75 (one half the hieght of the game arena).  Values under 0.0 get
  taken as their absolute value.  Values over 0.75 are taken as 0.75.

  The "-net" switch must appear after the "-gravity" switch, if you used one.

    Example:  3dpong $DISPLAY -computer -net 0.5
              3dpong $DISPLAY other.domain.com:0.0 -gravity 0.2 -n 0.75

  To play a game with sound, append "-sound" or "-s" to the command
  line.  (It must appear after the "-net" switch, if you used one.)

    Example:  3dpong $DISPLAY -sound
              3dpong $DISPLAY computer -g 0.5 -sound

  The sound will be played on the machine on which the game is
  executing.  (The program simply sends ".au" audio files to the
  "/dev/audio" device.  The sounds were "stolen" from the game
  "xboing"; thanks guy(s)!)

  To control the paddle in the game without holding the mouse button down,
  add "-noclick1" for player one, and "-noclick2" for player two.
  They must be after any of the previously mentioned options, and player
  one's "-noclick1", if any, should come first.

  Two extremely simple scripts have been created to start a one-player
  3dpong session:

    ./go       - Play against the computer on your default $DISPLAY with
                 a 0.5-high net
    ./handball - Play handball on your default $DISPLAY


  So, in summary, you have the following switches available to you, in this
  order:

    3dpong server1 [server2 | -computer] [-gravity value]
           [-net value] [-sound] [-nockick1] [-noclick2]


ON THE SCREEN:

  The game is viewed in 3D.  Depending on your viewing mode, you see the
  game from a different angle.  (See "3D PONG CONTROLS" below.)

  One- And Two-Player Ping Pong Mode:

    * There is a white divider line in the middle of the court to show
      you where the "floor" is.
    * Player one's paddle is a red square.  Player two's is blue.
    * Score is displayed at the upper left.  Player one's score is red.
      Player two's score is blue.

  Handball Mode:

    * Your paddle is a red square.
    * Score and high score are displayed at the upper left.

  Both Modes:

    * The ball is a green diamond shape.
    * Dark green markers appear on two walls of the court, showing you
      relative positions of the ball.


3D PONG CONTROLS:

  Moving the paddle:

    Left-Click in your window and drag (holding the left mouse button)
    to move the paddle around.

  Serving:

    In a one- or two-player ping-pong game, when it's your turn to serve,
    or to start a game in handball mode:

    * Position your paddle where you wish the ball to start
    * Right-Click to launch the ball

  Changing views:

    Press the [V] key to change your viewing perspective.  There are three
    modes it goes through:

      1. Standard - First person perspective from behind your paddle.
      2. Bleacher - From the side of the game.  (Your paddle on the left.)
      3. Above    - From above the game.  (Your paddle on the top.)
      4. Freeview - Choose any angle. (Use Middle-Click and drag to change
                    angles.  Careful not to get confused!)

    Press the [3] keyto toggle 3D glasses (red/blue) mode.
    When using 3D glasses, you must use red/blue glasses with the red
    lense on your left eye.  For best 3D results, get very close to the
    screen.  (Careful of health risks though!!!)  Your milage may vary.

  Changing controls:

    To toggle whether your paddle is controlled while mouse button one
    is down (normal) or not ("-noclick..."), press [C].

  So, in summary, you have the following keyboard controls:

    [V] - Change view
    [3] - Toggle 3D glasses mode
    [C] - Toggle "noclick" mode
    [Q] - Quit


GAME RULES:

  One and two-player ping-pong games:

    * The game is played until the player(s) decide to quit.
    * The ball moves faster and faster until someone scores, at which point
      it slows down again.
    * Score is accumulated during the game when the ball goes past a
      player's paddle and into their goal.
    * When a player scores (the ball goes into their opponent's goal),
      they get to server.

  Handball game:

    * Multiple games are played until the player decides to quit.
    * Score is accumulated during the game when the ball bounces off the
      player's paddle.
    * The score is zeroed and the game starts over when
      the ball goes past the player's paddle and into their goal.
    * A high score is kept for the session.


IDEAS FOR UPGRADES:

  I'd like to implement more control over the ball when you hit it with
  the paddle.  Currently, the ball just keeps going the way it was (X/Y)
  and goes away at a random speed.  I'd like to keep a queue of your
  latest paddle movements and use an average of that queue as the ball's
  new direction.  Or perhaps add angle controls with the keyboard, or
  depending on what side of the paddle was hit.  Suggestions are welcome!

  I'd like to make the computer player more intelligent (vs. random).

  I'd like to port this to Win95 <shudder!> or the Mac <gulp!>...


CREDITS:

  * 3D Pong was written by Bill Kendrick, (c) New Breed Software 1997.
  * Original 3D source code based on a Linear Algebra project by
    Bill Kendrick, 1997.
  * "Pong" is (c) Atari, now a division of JTS.
  * Thanks to:
    Brian Jerskey, my Linear Algebra professor
    Brian Mordecai, a Lin.Alg. friend who helped a little with the 3D
    James O'Connor, a Lin.Alg. friend who gave me a pair of 3D glasses
    Goeff Kruth, a friend with a Linux box to play on.
    And everyone who plays 3D Pong! :)


CHANGES

  * Version 0.2 - December 15, 1997
    Tested under RedHat Linux.
    Added simple /dev/audio sound support.  (Sounds from "xboing.")
    Added clickless mouse control for the paddles.
    Made the default (#define'd) window size bigger.
    The sparks were turned into actual 3D lines (looks 1000x better!)
    A net was added.
    The game now uses a backbuffer. (I may turn this into an option, as
      it seems to go a tad slower.)
    A few tweaks here and there.
    Red/blue mode has the colors reversed (someone told me red is usually
      on the left, not the right).

  * Version 0.1 - December 11, 1997
    Added "free view", "follow the ball" and "follow the
      paddle" mode.
    Added ball angle control using the paddle.
    Added "sparks" (or "debris") when the ball hits walls/etc.

  * Version 0.0 - December 10, 1997
    First release


CONTACT ME!

  Please tell me what you think of 3D Pong!
  Or send some money or a postcard to encourage me to release more and
  more and more for X!

  New Breed Software
  c/o Bill Kendrick
  7673 Melody Drive
  Rohnert Park, CA 94928 USA

  kendrick@zippy.sonoma.edu
  http://zippy.sonoma.edu/kendrick/nbs/

  1-707-795-1234 ext. 2
  1-707-795-5678 FAX

  3D Pong's website is at:
  http://zippy.sonoma.edu/kendrick/nbs/unix/x/3dpong/

  Check out my other X-Window software at:
  http://zippy.sonoma.edu/kendrick/nbs/unix/x/
