Sorry for the brief installation instructions. A more complete set will
be created for the distribution.


	To Install SAS

	1) copy the appropriate space-*, i_*, n_*, s_* from the CONFIG
	   directory to Space.c, Init, Node, and System respectivly.

	2) Type make install
		this will compile the driver and idinstall it.

	3) Edit /etc/ap/chan.ap and add the SAS nodes to autopush
	   the ldterm and ttcompat modules for line that will
	   be used for dial in/out.

	4) Reboot your system.

	We have tested most modes of operation on this driver, however
	VPIX code remains UNTESTED (We don't have vpix).

Read the include README.FAS for notes on minor node numbers.  Send bug
report to:

roe@rapfast.Unibase.SK.CA

Thanks....
