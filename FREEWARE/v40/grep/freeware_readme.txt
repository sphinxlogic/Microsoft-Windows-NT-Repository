GREP, UTILITIES, Utility to search for regular expressions

Executables are in the [.VAX] and [.ALPHA] subdirectories.
The VAX version was built with VAXC V3.2-044 on OpenVMS V6.1.  
The Alpha version was built with DEC C V5.00-004 on OpenVMS V6.2.

To Build:
  UNZIP GREP_SRC
  SET DEF [.GREP]
  @VMSMAKE

To run GREP:

 $ GREP :== $DRIVE:[DIRECTORY]GREP.EXE
 $ GREP
