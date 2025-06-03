HSCLOAD010, system management, HSC and HSJ disk load balancing tool

HSCLOAD010 is a useful tool for system managers concerned with the performance 
impact of data channel contention on HSC's and cache imbalances on HSJ's. This
tool uses a list of how disks are cabled (which CAN be obtained from HSC's
during the install) and the actual disk usage obtained from DECps or F$GETDVI to
determine the optimal loading given the wiring constraints. This information
can be used to perform either manual loading or automatic loading using PREFER.
Reports that are produced include a list of disks sorted by actual i/o's
performed and a list of HSC requestors showing the number of disks connected
and the before/after effects of load balancing on the total requestor load.
Command files to automatic set/clear/force the preferred paths are created when
the procedure is run. This kit is installable using vmsinstal. See the 
Postscript Install Guide and Users Guide for details.

Listing of save set(s)
Save set:          HSCLOAD010.A

BUILD_DISK_CABLE.COM	Procedure to automaticly build HSC cabling file
BUILD_DISK_CABLE.FOR	Fortran source to edit the cabling file
BUILD_DISK_CABLE.OBJ	Object code for the editing function
DCLHSC.FOR		Fortran source to allow dcl to hsc communication
DCLHSC.OBJ		Object code for dcl to hsc communication
HSCLOAD010_IG.PS	Postscript Install Guide
HSCLOAD010_UG.PS	Postscript User's Guide
HSC_LOADING_DECPS.FOR	Fortran source for the DECps version of the loading code
HSC_LOADING_NOVPA.FOR	Fortran source for the F$getdvi version of the code
KITINSTAL.COM		Vmsinstal required (this file does the install)
PREFER.CLD		Prefer command language definition
PREFER.EXE		Prefer executable
PREFER.MAR		Prefer macro source
PREFER.OBJ		Prefer object code
PREFER.TXT		Description of how prefer works

Save set:          HSCLOAD010.B	(DECps version of the code)

HSCLOADINGIVP.COM	Installation verification procedure
HSCLOADINGIVP_MI.COM	Installation verification procedure for MI clusters
HSC_LOADING_DECPS.CLD	Command language definition for the loading code
HSC_LOADING_DECPS.OBJ	Object code for the loading code
MANUAL_HSC_LOADING.COM	Procedure to do manual hsc loading
PREFERRED_PATH.COM	Procedure to do automatic hsc loading
PSPA$LIB.FOR		DECps fortran library

Save set:          HSCLOAD010.C (F$GETDVI version of the code)

HSCLOADINGIVP.COM	Installation verification procedure
HSCLOADINGIVP_MI.COM	Installation verification procedure for MI clusters
HSC_LOADING_NOVPA.CLD	Command language definition for the loading code
HSC_LOADING_NOVPA.OBJ	Object code for the loading code
MANUAL_HSC_LOADING.COM	Procedure to do manual hsc loading
PREFERRED_PATH.COM	Procedure to do automatic hsc loading

