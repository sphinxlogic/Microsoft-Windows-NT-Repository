
	.Title		DELTREE error messages
	.Facility	DELTREE,1016/prefix=DELTREE__
	.Ident		'Version 1.016'

	.Severity	fatal
		BUGCHK		<bug Check - Null pointer returned by runtime>
		BADCOUNT	<bad internal counter. Please report>
		SLOGFAIL	<setting logical failed. Please report>
		CHDIR		<chdir() failed while returning to original tree>
		INSFMEM         <insufficient dynamic memory>
		NOROOT		<startup root-directory no longer present> 
		CREATFAIL	<process creation failed>

	.Severity	error
		UGLIX		<first character '-' ? Is that an Uglix command line option ??>
		NOTATREE	<not all tree(s) deleted>
		MISSING		<argument syntax error. The path may be missing either [ or ] OR the path is illegal>
		CLKINIT		<could not initialize timer>
		PRV		<insufficient privilege or file protection violation>
		IOERROR		<IO error on tempfile>

	.Severity	warning
		NODIR		<no directory specified on command line>
		TOODEEP		<directory tree is too deep !AZ>/FAO=1
		DEEPFAIL	<deep tree deletion failed>
		NOPROT		<protection not set !AZ>/FAO=1
		NODELE		<could not delete   !AZ>/FAO=1
		NOTSET		<will not elevate priority above authorized priority = !SL>/FAO=1
		CHECKFAIL	<cannot check !AZ. Continue>/FAO=1
		

	.Severity	information
		DPRCS		<process !AZ detached, pid=!XL, priority=!SL>/FAO=3
		VERSION         <DELTREE Version !AZ. !AZ>/FAO=2
		NOTFND		<directory not found !AZ!AZ>/FAO=2
		CHDFAIL		<could not change directory to !AZ>/FAO=1
		DEEPDIR		<dealing with deep directory !AZ>/FAO=1
		GETENV		<getenv() returns !AZ>/FAO=1
		PRSRVD		<top directory !AZ preserved>/FAO=1
		NOTEXISTS	<some directories did not exist>
		IGNORE		</!AZ usage is enabled if /!AZ specified. Ignoring>/FAO=2
		

	.Severity	success
		DEBUG		<!AZ !AZ>/FAO=2
		BEGIN		<tree to remove !AZ>/FAO=1
		DEEPOK		<deep tree deletion is a success>
		DELETED		<!AZ>/FAO=1
		STAT		<!UL file!%S deleted in !AZ seconds>/FAO=1

	.End

