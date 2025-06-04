MPEG_PLAY, GRAPHICS, MPEG Video Player
!
MPEG_PLAY reads compressed motion picture video in
the MPEG format, decompresses in software and displays
on a standard X11 display.

Very useful for viewing things like the weather
animations from the University of Illinois WWW server.

This version provides shared memory extension access...
a bug in earlier versions of the extension would cause
it to hang (in which case, use the -shmem_off switch).

