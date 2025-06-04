SDL, LANGUAGES, Data Structure/Interface Definition Language

  EV1-64

  This tool is a data definition language and is used to convert a 
  language-independent definition file (an SDL input file) into a 
  language-dependent definition file.  Alone or in conjunction with 
  mechanisms such as the OpenVMS MESSAGE/SDL command (a latent and
  undocumented qualifier which converts the .MSG file into an SDL
  file) and the Freeware GNM tool, the SDL tool can be used to 
  maintain common sources for a wide variety of language files.

  This version of SDL includes images for use on OpenVMS VAX systems 
  (in [.VAX_IMAGES]) and on OpenVMS Alpha (in [.ALPHA_IMAGES]) 
  systems.  These images were built with/for/on OpenVMS V7.3.

  The provided SDL.COM procedure will create logical names for the 
  correct set of images for the local system architecture, and will
  also load the SDL.CLD command definition into the current process.
  To invoke SDL:

  $ @SDL  ! needed once

  then

  $ SDL/ALPHA/LANGUAGE=CC filedef.SDL

  or

  $ SDL/VAX/LANGUAGE=CC filedef.SDL

  Also available is SDL/NOPARSE, which converts the intermediate
  files (.SDI files) found in STARLETSD.TLB into language-specific
  definition files.

