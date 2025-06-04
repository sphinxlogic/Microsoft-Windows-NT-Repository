TIMIDITY, MISCELLANEOUS, MIDI real time player

        TiMidity is a MIDI to WAVE converter that uses Gravis
Ultrasound(*)-compatible patch files to generate digital audio data
from General MIDI files. The audio data can be played through any
sound device or stored on disk. On a fast machine, music can be
played in real time. TiMidity runs under Linux, FreeBSD, HP-UX, SunOS, and
Win32, and porting to other systems with gcc should be easy.

        TiMidity Features:

        * 32 or more dynamically allocated fully independent voices
        * Compatibility with GUS patch files
        * Output to 16- or 8-bit PCM or uLaw audio device, file, or
          stdout at any sampling rate
        * Optional interactive mode with real-time status display
          under ncurses and SLang terminal control libraries. Also
          a user friendly motif interface since version 0.2h
        * Support for transparent loading of compressed MIDI files and
          patch files

        * Support for the following MIDI events:
          - Program change
          - Key pressure
          - Channel main volume
          - Tempo
          - Panning
          - Damper pedal (Sustain)
          - Pitch wheel
          - Pitch wheel sensitivity
          - Change drum set

                        Tuukka Toivonen <toivonen@clinet.fi>

Timidity 0.2I port under OpenVMS                              30 July 1997
================================

Configuration files are referenced with timidity_dir rooted logical:

$ define/trans=conc timidity_dir disk:[rep1.timidity-0_2I.]

                                    don't forget the dot ^

You need MMOV 2.0 (multimedia Services for OpenVMS V 2.0) to play midi files
in real time on good contidions on your Alphastation (and SYSNAM privileges
to connect to Multimedia server; timidity is linked /notraceback to allow image
installation with SYSNAM privilege).

I've added the AMD audio device support (taken from Xlockmore source
distribution). The result is not very good but however on Alpha its audible.
It also run on VAX but you need a very fast one !!

To rebuild on Alpha with MMOV and DEC C 5.2/VMS 6.2:

$ @COMPILE_MMOV
$ @LINK_MMOV

To rebuild on Alpha or VAX with AMD audio and DEC C 5.2/VMS 6.2

$ @COMPILE_AMD
$ @LINK_AMD

Actually only the command-line interface is ported under VMS, but with that you
can even use Timidity as a Midi sound player for the Web (tested with Mosaic
and Netscape). You just need to add one line into your SYS$LOIN:MAILCAP. file:

audio/midi; timidity  %s

By default, with MMOV, midi files are played in stereo, 16 bits samples
at 22,050 Khz of sample rate which gives a good sound quality with a
moderate cpu charge (between 20% and 30% on Alphastation 255/233). With AMD
audio, midi files are played in mono, 8 bits samples at 8,000 Khz of sample
rate.

Patch Level 1 (20 July 1997):

Some parameters are modified to improve peformance and allow use of 44,100 Khz
sample frequency (with about 50% CPU on a 255/233):

- AUDIO_BUFFER_BITS is set to 12        (CONFIG.H)
- CONTROLS_PER_SECOND is reduced to 250 (CONFIG.H)
- The size of sample buffer in DEC_A.C is set to 2048*8 (16 Kb) instead of
   1024*8

Patch Level 2 (20 July 1997):

Some rewrite of AMD audio output using a flip/flop of two buffers and
asynchronous QIO's. The result is far better and if not great is at least
audible without trouble. The size of audio buffer is now 2048 samples to
improve performance (it is probably the max for AMD device driver SODRIVER).

Instrument samples:

Only a few patches (sounds samples needed to play midi files) are provided into
this kit (bright piano and nylon guitar). A rather complete set of samples is
available at my site (http://www2.cenaath.cena.dgac.fr/ftp/decwindows) or into
this distribution:


  o timidity_base_samples.zip        Base samples (piano and acoustic guitar).
  o timidity_instruments_samples.zip Full General MIDI set(Midia).
  o timidity_drums_samples.zip       Dustin McCartney's drum set.
  o timidity_wow_samples.zip         Dustin McCartney's Wow patches.
  o timidity_new_samples.zip         Flutes, Organs etc ... (good
                                     quality) Also contains last configuration
                                     files, the last file to be unzipped.



Enjoy !!

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
