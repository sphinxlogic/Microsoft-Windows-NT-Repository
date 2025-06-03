VTB, TOOLS, Bookreader for VT terminal

    To start the installation, simply type:

    $ @SYS$UPDATE:VMSINSTAL VTB013 <dev>:<dir>

    replacing <dev> and <dir> with the device and directory in which the
    VTB saveset resides.

    Please add the following symbol definition to the system-wide login
    command procedure SYS$MANAGER:SYLOGIN.COM :

                    $ VTB == "$SYS$SYSTEM:VTB"

    and just type the following command :

	$ VTB


	Copyright 	ESME-Sudria
			4, rue Blaise Desgoffe
			75006  PARIS
			FRANCE
	
	This software is delivered "as is". ESME-Sudria is not
	responsible for any problems or damage caused by the use of
	this software.

	Permission to use, copy and distribute this software for
	non-commercial purposes, is hereby granted without fee, 
	providing that the above copyright notice appears in all
	copies and that both the copyright notice and this permission
	notice appear in any supporting documentation.

	No cost can be charged for distribution, installation or
	support of this software.
