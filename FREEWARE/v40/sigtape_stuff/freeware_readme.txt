Sigtape Stuff, Software, Spreadsheet/database/F2C/defragger/communications program from sigtapes

This area contains material pulled from several older VMS SIG tapes
by Glenn Everhart (VMS Sig librarian). It includes:

AnalyRIM - spreadsheet and DBMS integrated, runs on vms, rsx, unix,
	amigados, and msdos. Full sources are present.
DBAG - roughly, a clone of dBase III for VMS with source & docs
DECOMPRESS_TOOLS - decompression tools from sigtapes you may need to
	decompress archives in zoo, lharc, zip, etc. formats in this
	material. AXP and Vax executables are present.
F2C and Setuser - F2C converts fortran 77 to C; VMS port by Tony
	Scandora. Setuser - "become" some other user if you have ]
	privs.
JUICER3 - Defragger that uses movefile, so is safe. Full source present.
	Defrags ODS-2 disks only (normal VMS structure) and does not
	handle volume sets.
READVMSBACKUP - Reads vms backup savesets on unix. Works on little endian
	machines (e.g. AXP) or big endian ones (e.g. sun)
VAXNET - Communications program for VMS. Knows kermit and xmodem
	and its own protocols and has a sophisticated script language
	so it can be used to call pagers etc. (Note the kermit protocol
	used is a basic one, but it does work. Xmodem checksum or crc
	are both there. VAXnet tends to work better as a remote terminal
	than other communications programs, in that it uses negligible
	CPU (unlike others that do single character I/O).

Unzip executables suited to these zipfiles are also provided for vax
and alpha.
