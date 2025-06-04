DECxterm, UTILITIES, xterm key remapping (for use on UNIX Systems)

The procedure remaps the Sun Keyboard, IBM AIX PC type Keyboard, Linux PC type 
Keyboard and Tru64 Compaq Unix keyboards to mappings suitable for VMS
This program is designed to run on a Unix machine, create a terminal emulator
with VMS keyboard mappings and then telnet to a VMS machine.

USAGE: DECxterm [-option ...]
          which are identical to xterm. "#man xterm" for further details
   eg: DECxterm -e telnet yakka.ali.dec.com

Note 1: All keys in the same position are mapped to the standard
  VMS LKxxx ikeyboard except for the keys between the main keyboard
  keys and the Numeric/Application keys which remain as labelled.
  The keys on an DEC LKxxx Keyboard are marked on other keyboards as:-
  Find marked Home, Insert marked the same, Remove marked Delete,
  Select marked End, Previous marked Page-Up and Next marked Page-Down.

Note 2: Shift/F1 to Shift/F10 are mapped to F11 to F20

Note 3: Other Terminal Emulators such as dtterm or dxterm can be used
  by setting the variable DECXTERM_EMULATOR. The default is xterm.

Note 4: The default xterm qualiers are defined by the variable
  DECXTERM_OPTIONS and are set to:-
   -fn 9X15   Default font
   -sb        Enable scroll bars
   -sl 500    Number of lines saved that are scrolled off the top of window
   -cr red    Set the cursor color to red
   -tn vt100  Set the terminal type to vt100
   -132       Allow switching between 80 and 132 columns
  The default settings can be changed by defining this environment
  variable before running DECxterm.

xterm_options="${DECXTERM_OPTIONS:=-fn 9X15 -sb -sl 500 -cr red -tn vt100 -132}"

Note 5: To modify this program or to add further keyboard mappings edit out
  the C code from within DECxterm and follow the procedures documented in 
  DECxterm.
