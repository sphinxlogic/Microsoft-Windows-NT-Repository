     ______      _
    / __/ /_  __(_)___  ____ _
   / /_/ / / / / / __ \/ __ `/				original source available at
  / __/ / /_/ / / / / / /_/ /             ftp.x.org:/contrib/games/multiplayer
 /_/ /_/\__, /_/_/ /_/\__, /	V6.18
       /____/        /____/
 
 
 Creating the executable shouldn't be too difficult, even though no Imakefile
 is included. But the normal Makefile isn't too complex and you only should
 check out, if the main switches at the beginning are set to the right
 C++-compiler and the X-Library and includes.

 (Ideas for creating an Imakefile for this C++-package are very welcome!!!)

 After linking, you should be able to just use it as the manual page says.
 But if you want to install it to be use by more persons or if you're just
 curious, you might want to know a bit more about internals.
 
 Since the main purpose for me was not directly writing a game, but more to
 experiment with some algorithms, many things are configurable in the program.
 All the parameters that would change the executable are to be set in the file
 'global.h'. One important thing is to name a directory, where the program can
 later on read and write data-files for the pixmap-data, that is needed for the
 smooth ball animation in the deluxe-version of pool. If the data-files are not
 accessible, the program has to recompute it, which might take a lot of time.
 (For information, on my 1280 pixel wide screen, I need 37x37=1369 bitmaps for
  half balls and 75x37=2775 bitmaps for full balls in the default size. When
  too many bitmaps will get neccessary you might get memory problems, e.g.
  on X-Terminals.)
 
 An interresting thing to experiment with might be the Real-Class. Real is
 usually defined as float or double. But there is also a complete inline-class
 available for it. Unfortunately using the class isn't too fast, but this
 probably depends on the smartness of the compiler. (Anyway, does anybody have
 a similar Real-Class implemented by using integer? I could really use it on
 my slow 386).
 
 There is also the main DEBUG-flag, which enables debugging switches all over
 the source and should usually be unset, even so the switch don't consume too
 much time.
 
 Constant game parameters like object size and friction are all defined in
 'global.C'. But for not always re-compiling the program when changing them
 they can also be set dynamically in the file 'presets.txt'. The correct syntax
 and the current value can be seen with the option -presets when starting the
 game.
 
 
 By the way, there actually exists a DOS-version of the program, but that was
 never really meant for playing. It's just there because the development of
 the game took place on a small 386-notebook. After running the program got too
 slow when too many objects were there, I ported the program to use X-windows.
 Even though I still developed the routines at home, the real testing then took
 place during the coffee and lunch breaks at my office. There are even some
 special parts like the ball animation, that don't run at all in DOS. I am
 thinking on porting these things back to DOS, but I probably would need some
 specialized assembler library to do the smooth animation.
 
 
 
 After installation, I would be pleased if you send me a short note
 just for curiosity and tell me:
 - if there were any problems and what they were
 - if you like it and what you think is missing
 The more notes I get, the higher the chances that I will spend
 more time on that packages.
 
 
 Have fun.
 
        __  __     __                __     __  __                 _
       / / / /__  / /___ ___  __  __/ /_   / / / /___  ___  ____  (_)___ _
      / /_/ / _ \/ / __ `__ \/ / / / __/  / /_/ / __ \/ _ \/ __ \/ / __ `/
     / __  /  __/ / / / / / / /_/ / /_   / __  / /_/ /  __/ / / / / /_/ /
  _ /_/ /_/\___/_/_/ /_/ /_/\__,_/\__/  /_/ /_/\____/\___/_/ /_/_/\__, /
 /__________________________________e-mail: Helmut.Hoenig@hub.de______/
 
 
 
 History:
 ========
 
 V6.3  - just a few includes were changed to make the program more portable.
       - a SOLARIS switch was added
 V6.9  - new background colors / new balls / includes Xos.h /
         improved color handling / less ANSI C
 V6.11 - new flags: -nohints, -red, -green, -blue
         minor corrections
 V6.15 - enhanced (much faster on X-Terminals) ball animation
         waits with select(..) to reduce CPU-usage in idle periods
         interpixel-motion for fine adjustment of cursor
 V6.18 - corrections in setting up colors (fvwm now loses the colormap struggle :-)
         enhanced reset feature
         pool9-class
