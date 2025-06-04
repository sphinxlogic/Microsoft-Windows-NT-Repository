NOVIRUS, UTILITIES, Validates images using CRC32 checksum

VALIDATING VMS IMAGE FILES AT RUN-TIME BY CALCULATING A CRC32 CHECKSUM.
=======================================================================

Maarten van Breemen
E-mail: vbreemen@knoware.nl
CIS: 100125,2704

About VMS virus protection:

I am working in a GLP (Good Laboratory Practice) environment and was bothered
with questions about VMS virus protection several times. I myself are not
worried at all but to be able to reply to the customers and regulatory 
inspectors I added a self-check mechanism to our self-developed images, both
on MS-DOS and VMS. It is easy to program. There are lots of public domain
examples for PC which are easy to port to VMS.

Directly after image activation, the program calculates a CRC32 checksum on the
image file, makes some site-specific changes to it and compares it with a
pre-calculated CRC32 in a separate key-file. If there is a difference, an 
operator request is made and the program halts. It is actually a waste of 
initial response time but serves as an early warning system for future virus 
attacks, media corruption, hacks etc. I now can prove that a program is in a
well-defined state and should execute just like the validated version.

This distribution is compiled with DEC C++ on a microVAX3100-95 running VMS 6.2 
on 21 february 1996. Any other C compiler will probably compile the code
correctly. All standard disclaimers apply. Use at your own risk.

Library CRCLIB.OBJ
Source CRCLIB.C, CRCLIB.H
Functions selfcheck() en crc32file()
Copyright (C) 1994 Maarten van Breemen
Released to the public domain 210795 MVB
Includes code by Gary S. Brown

This are routines to perform 32 bits Cyclic Redundancy Checks.
The selfcheck routine calculates a CRC and compares it with a pre-defined CRC
in an external key file. The selfcheck routine can be included in applications
where security demands are high. See CALCCRC and DEMOCRC for a demonstration.

The CRC is the well-known 32 bits CRC from telecommunications and is commonly
used by virus scanning software and data-integrity checks. The CRC is very
sensitive for data changes, even for changes in one bit.

CALCCRC is an utility to calculate the CRC checksum of an arbitrary file.
Its main goal here is to calculate the CRC value for inclusion in the external
key files and therefore should be kept in a safe place by the system manager
and/or trusted personell :-). It is advised to change the symbol 
APPLICATION_OFFSET in module CRCLIB.C to something which is at least 
site-specific. CALCCRC can also be used to calculate checksums for data files.

Define CALCCRC as a foreign command with @MSYM CALCCRC

$! MSYM.COM
$ 'p1 == "$''F$ENVIRONMENT("DEFAULT")'''p1'"

Syntax to define the CRC of the image filename.exe:

$ CALCCRC filename.exe

Enter the reported CRC value in a key file with name filename.crc.
Use an editor to create the key file.

CALCCRC and CRCLIB can be compiled for both MS-DOS and VMS. Because of the odd
VMS file formats and record attributes and its effect on the fgetc() runtime
function, the CRC value probably differs from CRC values calculated with other
packages but the value is always reproducable.

DEMOCRC contains VMS specific code but is easy to port to other operating
systems. If the CRC check fails, an OPCOM request is posted and the program
halts (loops).

