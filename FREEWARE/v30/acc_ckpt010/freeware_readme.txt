ACC_CKPT V1.0-3 , TOOLS, Accounting checkpoint utility

	This tool forces a process to write an accounting record, resetting
	all accounting counters.


files:	acc_ckpt.exe,.mar,.obj,.map	loadable_image for acc_ckpt
	acc_ckpt$main.exe,.mar,.obj	main procedure
		install as a foreign command, and
		$ acc_ckpt$main <PID> 
		will checkpoint the specified process and load the image
		if necessary
	acc_chkpt$unl.exe,.mar,.obj	(VAX only)
		unloads the loadable image
	install.com [ADD/REMOVE] adds or removes the loadable image
	test.com	tests the software


©	Ecole Nationale supérieure des Télécommunications

	Guillaume Gérard

	    gege@cal.enst.fr
	    CO:FR; AD:ATLAS; PD:TELECPARIS
	    0208075041285021::gege
