$! SETUP_GOPHER.COM 
$!	This setup file for GOPHER gives a single, central location
$!	for the definition of various logicals that are needed for
$!	the gopher server.
$!
$!	To modify this for your own installation, change all the 
$!	dev:[dir] specifications to the actual location for each
$!	item.
$!
$!  19931116	D.Sherman		dennis_sherman@unc.edu
$!		Original version created for 1_2vms1 distribution
$!
$! ** GOPHER_ROOT: the top level of your gopher data tree.  Note
$!	the trailing . -- it is important!
$!
$  define/system/tran=conceal/name=no_alias/nolog -
	GOPHER_ROOT dev:[dir.]
$!
$! If you have a need for more than one tree of data, for example
$! you want to serve data on more than one device, simply define
$! additional GOPHER_ROOTx logicals:
$! $  define/system/tran=conceal/name=no_alias/nolog -
$!	   GOPHER_ROOT1 dev:[dir.]
$!
$!
$! ** GOPHER_EXE: the location of all the executable files needed
$!	for the VMSGopherServer.  This will usually be the [.EXE]
$!	directory below the directory in which this file resides,
$!	but you can change it to somewhere else, as long as the
$!	*.exe files actually are there.  If you define GOPHER_EXE
$!	to be something else, make sure you copy RUN_GOPHERD_DAEMON.COM
$!	there as well as all the *.EXE files.
$!
$  define/system/nolog GOPHER_EXE dev:[dir]
$!
$  exit_prog:
$   exit
