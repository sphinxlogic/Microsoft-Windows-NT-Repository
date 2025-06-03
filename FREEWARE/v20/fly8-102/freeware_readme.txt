FLY8-102, GAMES, Flight Simulator under X window

  DESCRIPTION

    This program is a flight simulator. It puts more emphasis on
    the dynamics than on the cosmetics: just wire-frame. It  can
    run on any machine but a 386DX is the minimum for any decent
    performance. A fast video controller is a boon as  the  pro-
    gram,  when  running  on a 386DX/40Mhz, spends 70-80% of its
    time pushing pixels. On non-intel machines you can  try  and
    see if it is fast enough...

    The  program  was  written  for  fun.  I borrowed ideas from
    everywhere and hope to hear some more. The basic  design  is
    based  on  a  program  I wrote more than 20 years ago at uni
    (the Technion). I had an excellent coach (Danny Cohen) and I
    still have fond memories of those times. But now my computer
    has more than 24Kbytes of memory! so Fly8 is  written  in  C
    (Fly8  was the name of the last version of the original pro-
    gram dated 12-JAN-1974, it was written in PDP15  assembly  -
    macro15 - for a VT15 graphics processor).
    The actual purpose of the program was to give me an opportu-
    nity to experiment with various aspects  of  flight  simula-
    tion, but mostly with (1) the HUD symbology (or, more gener-
    aly, with man-machine interaction) and (2) the  studying  of
    basic  aerodynamics (as well as general real time simulation
    techniques). This explains why there are so many HUD options
    and  such a proliferation of flight dynamics models, as well
    as why the simulation parameters are user definable as input
    files at run time. Of course, the experimental nature of the
    program means that it must be distributed in source form.
 
    On the  PC  the  basic  screen  drawing  uses  the  standard
    Microsoft  graphics library. It is OK but not very fast; the
    main advantage is that it will support most video  adapters.
    The  fast  graphics  driver was built from the routines from
    DJGPP with much personal additions. The flight dynamics  was
    influenced  by  an SGI program I saw and ACM. The timer rou-
    tines come from a microsoft journal article, the  user-input
    routine (notice how you can use arrow keys etc? use up-arrow
    to retrieve history.. I will document it one day) comes from
    DDJ (or was it CUJ? author name is Bob Bybee). Well, I avoid
    re-inventing wheels unless it is fun. The  program  compiles
    with Microsoft C, Borland C, gcc on a friends Amiga, Sun and
    Linux and I hope on other platforms; it  is  written  to  be
    portable.  It runs under MSDOS, MSWindows, Amiga and unix/X-
    Windows... and now on OpenVMS !!

    What? what? WHAT? you want to see some action? OK. just skip
    to the next chapter then come back.
    The full set of commands is detailed in the 'commands' chap-
    ter. Here we will look at the program areas in general.
    There are two rather distinct kinds  of  commands  that  one
    uses:  commands  that drive (fly) the game and commands that
    configure, set options and so on (which are used  with  less
    urgency).  It  was  attempted  to get the important commands
    into the keyboard (a one keystroke command) while the others
    go  into  the  main menu system (accessed with the Esc key).
    Some of the urgent commands may bring up a  menu  which  you
    may ignore if you know the keystokes.
    The  urgent commands will control the vehicle flight and the
    other subsystems (radar, HUD, HDD, weapons etc.).  You  will
    notice  early that the program lacks the traditional instru-
    ment panel: it is intended to be driven  from  the  HUD  and
    other digital displays.
    The vehicle is also driven by a pointing device (a mouse or,
    preferably, a joystick). It will run off the keypad when you
    have  no such device.  The pointer is used only for steering
    control although the buttons  can  be  mapped  to  auxiliary
    functions (by default both 'fire').

    The  display area is typicaly divided into the main view and
    a number of secondary Head Down Displays (HDD).  The  design
    has  a dozen or so on board instruments that generate visual
    data; you select which ones should  be  displayed  on  which
    HDD.  The main view is what you see through the cockpit. The
    HUD can be overlaid onto this view (as is the  case  with  a
    real  plane).  Other  data may also be shown here for conve-
    nience.

    One other instrument is designated as an alternate main view
    (use  the 'v' command to see it). The 'windows' menu handles
    the screen format and configuration.
    The program generates various messages  as  it  goes  along,
    these  will  appear at the bottom of the main view and stack
    up. Each message has a time-out for deletion but you can use
    'c'  to  clear the lot. When the program needs user input it
    will open a prompt line at the very bottom of the main  view
    (in  magenta  color)  where your data will show. You can use
    the normal editing keys while entering data here -  previous
    entries  are  accessible with the up/down arrows. See 'input
    line editing' later.

    You may find some of the commands/options  strange  (if  not
    outright  insane);  this  will  be  related  to  my taste or
    (mostly) to much history and quick fixing that did not  com-
    pletely  settle  yet.  I have looked at other programs (like
    F3, JF2, ACM and SGI f.s.) but this was after the first ver-
    sion of this program was finished, so some good ideas missed
    the bus this time. In the future I hope to polish  the  user
    interface  (especialy  after  other people get to use it and
    express an opinion).

    Being as the program is still evolving you  will  find  some
    areas less complete than others. I hope that there is enough
    of it to make it useful. I expect to see  contributions  (of
    ideas  and  code)  from  other  people;  I  will continue to
    develop the program (at least for a while) and would like to
    see it take it's own path in life [heavy stuff :-)].

  QUICK_START

    In  this  chapter the symbol '@' is used to denote the Enter
    key. This will give you a quick feel for what the program is
    like.  With the program installed, type
      $ fly -z5
    This  starts  the program in a demo setting and is useful to
    see if all is OK and also great as a screen blanker :-)
    If the fly.ini options are correct then your plane will take
    off  and  start  looking  for action. Some messages are dis-
    played during startup - these will disappear after  a  short
    while.  The  screen will show a simple view of the runway, a
    ground grid (in gray) and an overlaid HUD.

    To exit hit 'Esc' 'x' and 'y'.

  Author

    Eyal Lebedinsky (eyal@ise.canberra.edu.au)

