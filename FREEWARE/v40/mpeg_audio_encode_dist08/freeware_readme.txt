MPEG_AUDIO_ENCODE, MISCELLANEOUS, MPEG audio encoder

ISO MPEG Audio Subgroup Software Simulation Group (1996)
ISO 13818-3 MPEG-2 Audio Codec

Mpeg audio Layer 2 & 3 encoder port under OpenVMS                Oct 1997
=================================================

The source kit contains ISO mpeg original audio encoder and decoder. Only the
encoder is ported under OpenVMS, an optimised decoder already existing
(mpeg3play).

You can find the encoder sources , object and executables files for VAX & Alpha
into [.LSF.ENCODER] directory. This encoder is Mpeg-1 Layer II and layer III
capable. However, it is not optimised and you need a fast cpu especially with
layer III.

You can compress at bitrates as low as 128 kbs before really earing artifacts,
even with Layer II !! At such a bitrate, the resulting Mpeg file of a 44 Khz
16 bits stereo stream is 30% smaller than a mono 8 bits 22 Khz WAV file and
gives a far better quality.

Patrick Moreau
pmoreau@cena.dgac.fr
moreau_p@decus.fr
