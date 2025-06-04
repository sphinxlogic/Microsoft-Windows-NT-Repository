SHAD-DSK-MAKER, Storage_Management, Virtual disk OK with DEC shadowing
Shadowing Unlike Disks

This version of VDdriver permits one to make virtual disks of parts of
other disks and works with Digital volume shadowing. Thus you can take
unlike disks and make virtual disks out of parts of them, and shadow the
parts. The remaining parts of the disks can be used either as separate
virtual disks or as other file structures, if you decide to make file
structures on the raw disks first and place the virtual disks on contiguous
files on them.

This code is presented as a "calling card" and works nicely with the author's
product SAFETY. 

Please read the file "Safetyspd.txt" in this directory to learn about
Safety, a comprehensive data safety package offering a reliable user
undelete system, storage management, and data security functions unequalled
anywhere in the industry.

This version of the VDdriver is released for general use by GCE.

Note that the file dfvax_shad_ok is an attempt at a retrofit for vax
of the same techniques for making virtual disks work on vax with DEC
volume shadowing. Use with asndf after set command asndf.
