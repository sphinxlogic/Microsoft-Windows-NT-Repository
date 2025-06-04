# XEphem v3.2.3
#   Top level MMS description file.  Nothing fancy.
#   This build and set of description files in the sub-directories have
#   only been tested on Alpha/OpenVMS v6.2 or greater systems, with
#   DEC C v5.7-004 and at least Motif v1.2-3960215 (v1.2-3 15 Feb 1996).
#
#   Written by: Rick Dyson rick-dyson@uiowa.edu
#                4-NOV-1999
#
#   usage:
#       $ MMS
#

all :	LibAstro.olb XEphem.exe
	@ Continue

XEphem.exe :	LibAstro.olb
	@ Set Default [.XEphem]
	- $(MMS) /Ignore = Warning
	@ Set Default [-]

LibAstro.olb :
	@ Set Default [.LibAstro]
	- $(MMS)
	@ Set Default [-]

clobber :
	@ Set Default [.XEphem]
	- $(MMS) clobber
	@ Set Default [-.LibAstro]
	- $(MMS) clobber
	@ Set Default [-]
        - Purge /NoConfirm
