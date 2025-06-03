JED, TOOLS, Editor that contains EDT/Emacs/Wordstar key mappings.

JED is an editor that contains EDT, Emacs, and Wordstar key mappings.
It runs on OpenVMS, UNIX, and PC systems.   It's very useful for learning Emacs
if you are a traditional OpenVMS programmer.  Some other features are;

	- Language sensitive editing.  There's a C mode, text mode, DCL
	mode, and others.  
	
	- ANSI color support.  Parts of a C source file can be programmed to 
	be displayed in different colors (i.e., comments one color, reserved
	words another, etc.).
	
If you're on a VAX, you may use JED directly off of the FREEWARE CD.  Reading
[FREEWARE.JED.JED.DOC]INSTALL.VMS will provide you with information about
how to do this.  Basically, put the following in your LOGIN.COM file;

	$ define/job/trans=conceal jed_root freeware$dev:[jed097.jed.]
	$ @jed_root:[bin]init_jed 1 0 
	
AXP binaries are also shipped, but the standared JED command procedure
listed above isn't converted to work with both types of images.

Jed is also shipped in a .ZIP file.  To unzip on an OpenVMS system, use the
Info-Zip unzip program that's located elsewhere on the OpenVMS FREEWARE CD,
and provide the following parameters (on UNIX and PC, the "-V" isn't
necessary);

	$ unzip "-V" jed097b8.zip
	
This will create two directories under the one that you execute the unzip
command from;  [.JED] and [.SLANG].  OpenVMS binaries are shipped in the
[.JED.BIN] directory.  JED.EXE is the VAX executable, and JED_ALPHA.EXE is
the AXP executable.

To install, read the file [.JED]INSTALL.VMS.  This provides all the
information that you need.

Jed comes from John Davis (davis@amy.tch.harvard.edu).  Read the
documentation in the [.JED.DOC] directory for more information.

HINT: copy [.JED.LIB]JED.RC to your SYS$LOGIN directory and edit your
changes there (turn color on, etc.).  Color works best on a terminal with a
black background and a light foreground.
