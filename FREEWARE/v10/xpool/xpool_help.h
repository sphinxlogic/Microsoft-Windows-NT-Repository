static char *header = "XPOOL V4.0-1  Dual Architecture VMSclusters Demo";

#define ABOUT_XPOOL \
"XPool V4.0-1      10-May-1990\n\
\n\
DECWindows version of an interactive pool game\n\
\n\
\n\
Software version: OpenVMS VAX V5.x, OpenVMS AXP V1.0\n\
\n\
Author: Doug Stefanelli\n\
\n\
For more information on XPool, select\n\
the \"More About\" help option\n\
\n"

static char *xpool_help_strings[] = {
"\n\
XPool V4.0-1\n\
----------\n\
\n\
XPool is a DECwindows version of a graphical pool game.  There\n\
are a number of different variations which are described below.\n\
\n",

"\n\
Selecting an Opponent\n\
---------------------\n\
\n\
  There are 4 different opponents which can be chosen.\n\
\n\
    Two Player   allows you to challenge another person locally,\n\
    vs. Computer allows you to challenge the computer,\n\
    Network      allows you to challenge someone on another\n\
                 DECwindows system,\n\
    Automatic    allows you to watch the computer challenge itself,\n\
    AutoNet      allows computer vs computer over the network,\n\
    AutoLock     allows computer vs computer in a VMScluster.\n\
\n",

"\n\
Network Play\n\
------------\n\
\n\
  If you have selected \"Network\" as the opponent, you are\n\
  automatically prompted for the remote node AFTER you start\n\
  the game.  Under VMS, you can also choose either Local Area or\n\
  Wide Area network play.  With Ultrix, Local Area is always used.\n\
\n\
  With Local Area, the system on which Xpool is run will act as the\n\
  client for both systems running the game.  To use this technique,\n\
  you need to do the following:\n\
    1) if desired, move the resource file, XPOOL.DAT on VMS or\n\
       default.Xpool on Ultrix, to the local node into the directory\n\
       DECW$USER_DEFAULTS: on VMS or XAPPLRESDIR or HOME on Ultrix\n\
    2) go into the customize security option of the session manager's\n\
       menu bar on the remote node and give access to the user on the\n\
       local node\n\
  The advantages to this type of play is that less resources are\n\
  required on the remote node and no special proxy account is required.\n\
\n",

"  With Wide Area, the system on which Xpool is run will send a request\n\
  to the remote system to start up a second Xpool program.  The two\n\
  programs will each send messages containing user input to each other.\n\
  To accomplish this, you need to do the following:\n\
    1) create a proxy account on the remote node for the local user\n\
    2) put a file called XPOOL.COM in the default directory of the\n\
       proxy account.  It should contain the lines:\n\
           $ SET DISPLAY/CREATE/NODE=local_node\n\
           $ RUN dev:[dir]XPOOL\n\
    3) if desired, move resource files, XPOOL.DAT, into the directory\n\
       DECW$USER_DEFAULTS: on the local node, the remote node, or both\n\
    4) go into the customize security option of the session manager's\n\
       menu bar on the remote node and give access to the proxy account\n\
       using local transport and node 0\n\
  The advantage to this technique is improved performance over bad or\n\
  long DECnet links and the flexibility in using separate resource\n\
  files for both systems.\n\
\n",

"\n\
Selecting the Game\n\
------------------\n\
\n\
  There are a number of pool games which can be played.  They are:\n\
\n\
    Straight Pool:\n\
        Object:\n\
            Sink the number of balls specified as the straight pool\n\
            limit (default is 50).  The target ball and the target\n\
            pocket must be called before each shot is made.\n\
        Special Rules:\n\
            * If the target ball is not sunk in the target pocket,\n\
              any balls sunk on the shot are respotted and no points\n\
              are awarded.\n\
            * After a scratch the cue ball must be placed on the left\n\
              quarter of the table.  There are tick marks to indicate\n\
              the right edge of this area.  Also, the first ball\n\
              contacted must be to the right of the same tick marks.\n\
            * Scratching, sinking no balls, or making an illegal shot\n\
              after a scratch results in loss of turn.\n\
            * If there is one or no numbered balls on the table after\n\
              a legal shot, all sunken balls are reracked (leaving the\n\
              one-ball position empty if a numbered ball was left on\n\
              the table).  If the remaining numbered ball would overlap\n\
              rack, it is placed in the one-ball position.  If the cue\n\
              ball overlaps the rack, it is respotted as after a\n\
              scratch.\n\
\n",

"    8 Ball:\n\
        Object:\n\
            Sink the 8 ball after sinking all of the high (above 8)\n\
            or low (under 8) numbered balls on the table or on the\n\
            break.\n\
        Special Rules:\n\
            * The selection of high or low balls is made by the\n\
              player that sinks the first ball without scratching.\n\
            * Any selected balls sunk on a shot that results in\n\
              a scratch are respotted on the table.  If no selected\n\
              balls are sunk, one is taken from the ball rack (if\n\
              there are any).\n\
            * If the high/low selection has not been made, all balls\n\
              sunk on a scratch are respotted.\n\
            * After a scratch the cue ball must be placed on the left\n\
              quarter of the table.  There are tick marks to indicate\n\
              the right edge of this area.  Also, the first ball\n\
              contacted must be to the right of the same tick marks.\n\
            * Failing to contact a ball from the selected group first,\n\
              or a ball other than the 8 ball if no selection has been\n\
              made results in loss of turn.  Any selected balls sunk on\n\
              the shot are respotted.\n\
            * Scratching, sinking no selected balls, or making an\n\
              illegal shot after a scratch results in loss of turn.\n\
            * Sinking the 8 ball prematurely results in loss of game.\n\
            * Sinking the 8 ball without contacting the 8 ball first\n\
              results in loss of game (except on the break).\n\
\n",

"    9 Ball\n\
        Object:\n\
            Sink the 9 ball while always hitting the lowest numbered\n\
            ball on the table first.\n\
        Special Rules:\n\
            * Failing to hit the lowest numbered ball on the table\n\
              first results in loss of turn.  Any balls sunk on the\n\
              shot are respotted.  The opposing player may place the\n\
              cue ball anywhere on the table.\n\
            * Scratching, or not contacting the lowest numbered ball\n\
              on the table first results in loss of turn.\n\
            * After scratching, the cue ball may be placed anywhere\n\
              on the table.\n\
\n",

"    Rotation\n\
        Object:\n\
            Scoring 61 points by sinking numbered balls and scoring\n\
            their number value while always hitting the lowest numbered\n\
           ball first.\n\
        Special Rules:\n\
            * Failing to hit the lowest numbered ball on the table\n\
              first results in loss of turn.  Any balls sunk on the\n\
              shot are respotted.  The opposing player may place the\n\
              cue ball anywhere on the table.\n\
            * Scratching, not contacting the lowest numbered ball on\n\
              on the table first, or making an illegal shot after a\n\
              scratch results in loss of turn.\n\
            * After scratching, the cue ball may be placed anywhere\n\
              on the table.\n\
            * If the table is clear and both players have scored 60\n\
              points, the game is a draw.\n\
\n",

"\n\
Options\n\
-------\n\
\n\
  Preset Velocity\n\
    You can choose to have the computer select a velocity for you.\n\
    This does prevent you from putting any spin on the cue ball.\n\
\n\
  Confirm Shots\n\
    You can request that after you select a shot, you are asked for\n\
    confirmation.  The direction and velocity of the shot are displayed\n\
    as an arrow pointing in the direction of the selected shot, with\n\
    the size of the arrow indicating the velocity.\n\
\n\
  Computer Skill Level\n\
    You can set the skill level of the computer.  By default it\n\
    is set to a medium skill level.\n\
\n\
  Straight Pool Limit\n\
    You can set the limit up to which you play in straight pool.\n\
    The range is 10 to 200.  The default is 50.\n\
\n\
  Respot Ball\n\
    This option is only valid when you have selected Two Player as\n\
    the opponent.  It is active while you are prompted to set the\n\
    direction of your shot.  It allows you to respot balls from\n\
    the ballrack, or move balls that are on the table.\n\
\n",

"\n\
XPool Resources\n\
---------------\n\
\n\
  XPool supports the setting of a number of resources by either adding\n\
  them to the generic Xdefaults file or creating an Xpool specific file\n\
  DECW$USER_DEFAULTS:XPOOL.DAT.\n\
\n\
  The following resources can be set:\n\
\n\
    Resource             Default        Description\n\
    --------             -------        -----------\n\
    Xpool.game           EightBall      Game type - EightBall, NineBall,\n\
                                        Rotation, or StraightPool\n\
    Xpool.opponent       TwoPlayer      Opponent type - TwoPlayer,\n\
                                        Computer, Network, or Automatic\n\
    Xpool.x              Centered       X position of main window\n\
    Xpool.y              Centered       Y position of main window\n\
    Xpool.skill          50             Computer skill setting (0-100)\n\
    Xpool.limit          50             Straight pool limit (10-200)\n\
    Xpool.presetVelocity Off            Preset velocity disabled\n\
    Xpool.confirmShots   Off            Confirmation of shots disabled\n\
    Xpool.network        LocalArea      Network connect type - LocalArea,\n\
                                        or WideArea\n",

"    Xpool.background     Black          Background color - background\n\
                                        around table, numbers on balls\n\
    Xpool.foreground     White          Foreground color - text, circles\n\
                                        behind numbers on balls\n\
    Xpool.felt           MediumSeaGreen Felt color - felt, ballrack,\n\
                                        tick marks\n\
    Xpool.table          Brown          Table color - table border\n\
    Xpool.cueBall        White          Cue ball color\n\
    Xpool.oneBall        Yellow         One ball color\n\
    Xpool.twoBall        Blue           Two ball color\n\
    Xpool.threeBall      Red            Three ball color\n\
    Xpool.fourBall       BlueViolet     Four ball color\n\
    Xpool.fiveBall       Orange         Five ball color\n\
    Xpool.sixBall        Green          Six ball color\n\
    Xpool.sevenBall      Maroon         Seven ball color\n\
    Xpool.eightBall      Black          Eight ball color\n\
    Xpool.nineBall       Yellow         Nine ball color\n\
    Xpool.tenBall        Blue           Ten ball color\n\
    Xpool.elevenBall     Red            Eleven ball color\n\
    Xpool.twelveBall     BlueViolet     Twelve ball color\n\
    Xpool.thirteenBall   Orange         Thirteen ball color\n\
    Xpool.fourteenBall   Green          Fourteen ball color\n\
    Xpool.fifteenBall    Maroon         Fifteen ball color\n",

"\n\
  Notes:\n\
      * On monochrome systems, the default felt color is Black,\n\
        the table color is White, and all pool balls are White.\n\
\n\
      * Colors may either be specified by name or by using the format\n\
        #RRRRGGGGBBBB where RRRR is a 1 to 4 digit hexadecimal number\n\
        indicating the intensity of the red component of the color,\n\
        GGGG is the green component and BBBB is the blue component.\n\
        All three color components must have the same number of digits.\n\
\n\
      * On VMS systems you can change the player name by defining the\n\
        logical name XPOOL$ME on the node you are running on."
};
