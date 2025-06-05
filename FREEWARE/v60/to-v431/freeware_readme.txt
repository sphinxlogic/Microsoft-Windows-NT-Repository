TO, UTILITIES, SET DEFAULT program - saves typing

TO.COM is a set-default program that works on Files-11 ODS-2 disks. I
haven't tried it on an ODS-5 disk so I don't know how well it would
work with ODS-5. 

Features:

o  Correct processing of nested logical names.
o  Smart Logical-Name Recall Stack... 
   - holds last 9 defaults plus the current default (the stack size 
     is adjustable)
   - assigns convenient logical names to stack entries
   - uses numbers to reference stack entries in TO.COM
   - avoids duplicate entries 
o  New default is checked for validity and existence.
o  Filename stripping: If P1 is a logical name pointing to a file, 
   TO.COM looks for the file and sets default to the directory in 
   which that file resides. If no file is found, the device and 
   directory are extracted from P1 and TO.COM sets default to the 
   result.
o  Has logic to handle situations in which the default was changed by 
   another program. 
o  Directory brackets are optional (logical names must still be 
   defined properly).
o  Shortcuts for MFD, top level of current tree, up n levels
o  Two-step default function (saves typing)
o  Superfluous 000000's are removed from new defaults.
o  23-line quick-help page function

See README.TXT or the beginning of MANUAL.TXT for easy installation
and Quick Start instructions. 
