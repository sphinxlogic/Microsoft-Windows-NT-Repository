XANIM, MOTIF TOYS, multiformat animation viewer for X WINDOW

XAnim is a program that can display animations of various formats on systems
running X11.  XAnim currently supports the following animation types:
	  +  FLI animations.
	  +  FLC animations.
	  +  IFF animations.  The  following  features  are  sup-
	     ported:
	       -> Compressions 3,5,7,J(movies) and l(small L).
	       -> Color cycling during single images and anims.
	       -> Display Modes: depth 1-8, EHB, HAM and HAM8.
	  +  GIF87a and GIF89a files.
	       -> single and multiple images supported.
	       -> GIF89a animation extensions supported.
	  +  GIF89a animation extension support.
	  +  a kludgy text file listing gifs and  what  order  to
	     show them in.
	  +  DL animations. Formats 1, 2 and 3. TEMPORARILY DISABLED.
	  +  Amiga PFX(PageFlipper Plus F/X) animations. DISABLED TEMPORARILY.
	  +  Amiga MovieSetter animations(For those Eric Schwartz
	     fans).
	  +  Utah Raster Toolkit RLE images and anims.
	  +  AVI animations. Currently supported are
	       ->*IBM Ultimotion     (ULTI) depth 16.
               -> JPEG               (JPEG) depth 24.
               -> Motion JPEG        (MJPG) depth 24.
               -> Intergraph JPEG    (IJPG) depth 24.
	       -> Microsoft Video 1  (CRAM) depth 8 and 16.
	       -> SuperMac Cinepak   (CVID) depth 24.
	       -> Uncompressed       (RGB ) depth 8.
	       -> Uncompressed       (RGB ) depth 24.
	       -> Run length encoded (RLE8) depth 8.
	       -> Editable MPEG      (XMPG) depth 24.
	  +  Quicktime Animations.  The  following  features  are
	     supported:
	       -> Uncompressed     (RAW ) depth 4,8,16,24,24+ and GRAY 4,8.
	       -> Apple Graphics   (RLE ) depth 1,8,16 and 24 and GRAY 8.
	       -> Apple Animation  (SMC ) depth 8 and GRAY.
	       -> Apple Video      (RPZA) depth 16.
	       -> SuperMac Cinepak (CVID) depth 24 and GRAY 8.
	       -> Component Video  (YUV2) depth 24.
	       -> Photo JPEG       (JPEG) depth 8 and 24.
	       -> Supports multiple video trak's.
	       -> Supports animations with multiple codecs.
	       -> Supports single-fork and separate .rsrc and .data forks.
	  +  JFIF images. NOTE: use XV for single images. This is more
	     for animation of a sequence of JPEG images.
          +  MPEG animations. Currently only Type  I  Frames  are
             displayed.  Type  B  and Type P frames are currently
             ignored, but will be added in future revs.
	  +  WAV audio files may have their sound added to any animation
	     type that doesn't already have audio, by specifying the .wav
	     file after the animation file on the command line. Currently
	     only the PCM audio codec is supported.
	  +  any combination of the above  on  the  same  command
	     line.

