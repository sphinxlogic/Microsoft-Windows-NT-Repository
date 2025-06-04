MPEG2PLAY, GRAPHICS, MPEG 2 Video player

  mpeg2play is based on mpeg2decode, (C) 1994, MPEG Software Simulation Group



Features
========

- plays MPEG-1 and MPEG-2 video bitstreams on 8 bit color X displays
- plays system layer (MPEG-1 only) streams, audio being ignored
- high quality ordered dither algorithm
- high speed interlaced to progressive scan conversion
- speed is about 90% of Berkeley's mpeg_play (somewhat platform and
  stream dependent)
- NCSA Mosaic compatible (as external viewer for video/mpeg)

Mpeg2play v 1.1b port under OpenVMS                                June 1997
===================================

Sources plus binaries (exe & object libraries) are provided.

Built under OpenVMS Alpha 6.2 with DEC C 5.0
            OpenVMS VAX 6.1 with DEC C 5.0
            OpenVMS VAX 6.1 with VAX C 3.2 (without USE_TIME)

Mpeg2 movies need to be in Stream_LMF format

Mpeg 2 example movie provided: TENNIS.M2V (8 frames)

Patrick Moreau

pmoreau@cena.dgac.fr
moreau_p@decus.fr
