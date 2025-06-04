This is a VMS qi server for the UIUC CCSO nameserver protocol.

The sources for qi and its utilities are in the SRC directory.

The directory CCSO_DOCS contain the documents for the Unix CCSO nameserver.
They contain a description of how the server and clients should work.

The directory DOC contains sample configuration files and a document
(WHAT.TXT) describing the VMS implementation of the server.  WHAT.TXT also
contains a short troubleshooting section.  The DOC directory also contains
a history of changes (QI.RELEASE_NOTES).

The programs can be built with MAKE_QI.COM for MultiNet, Wollongong, UCX or
NETLIB.  NETLIB 2.0 is available separately and supports CMU TCP/IP, UCX,
Pathway, TCPware and MultiNet.


Distribution contents:

APITEST.EXE		Interactive tool to test QI_API.EXE linkage
QI.EXE			The server
QI_ADD_ALIAS.EXE	Filter to add alias field.  See [.DOC]
QI_ADD_NICKNAME.EXE	Filter to add nickname field.  See [.DOC]
QI_ADD_SOUNDEX.EXE	Filter to add soundex field.  See [.DOC]
QI_API.EXE		Sharable image for programs using the qi API
QI_BUILD.EXE		Utility to create/update the database files. See [.DOC]
QI_MAKE.EXE		Utility to reformat your source data files.  See [.DOC]
README.TXT		This file

[.CCSO_DOCS]

Original documentation form the UIUC CCSO qi distribution

[.DOC]
CSO_FAQ.TXT		Frequently asked questions
CSO_STARTUP.COM		Sample startup script
DATABASE.CNF		Sample database configuration file
EM411.TXT		Sample input file used in MAKE_DATABASE.COM
EM411.QI_MAKE		Sample QI_MAKE definition used in MAKE_DATABASE.COM
MAKE_DATABASE.COM	Script to build a sample database
NICKNAMES.TXT		Sample set of nicknames used by QI_ADD_NICKNAMES
OPTIMIZE.COM		Generic script to optimize RMS indexed files
PH.DIF			Changes to enable some commands in the LLNL ph
QI.RELEASE_NOTES	Version changes and release notes
QI_ADD_ALIAS.DOC	Documentation on QI_ADD_ALIAS
QI_ADD_NICKNAME.DOC	Documentation on QI_ADD_NICKNAME
QI_ADD_SOUNDEX.DOC	Documentation on QI_ADD_SOUNDEX
QI_MAKE.DOC		Documentation on QI_MAKE
QI_UAFDUMP.DOC		Documentation on QI_UAFDUMP
SAMPLE_BUILD.TXT	Describes the steps in building and testing a database
SITEINFO.TXT		Sample site info file
TODO.TXT		Projects on the list to do
UCX_QI_SERVER.COM	Sample COM file for UCX service
UCX_SETUP_QI.COM	Sample UCX service setup
UPDATE_DATABASE.COM	Sample showing how we update an existing database
WHAT.TXT		Describe the VMS qi implementation.  See also WHY.TXT

[.HELP.NATIVE]

Various files from UIUC describing the qi commands and errors

[.HELP.PH]

Various files from UIUC describing the ph commands

[.SRC]
API.OPT			Link option file for using the qi API
APITEST.C		C language interface to API
APITEST.PAS		Pascal language interface to API
APITV.MAR		VAX transfer vector for QI_API
CRYPT.C			Utility routines
CRYPTIT.C		Utility routines
MAKE_QI.COM		Procedure to build everything
MULTINET_AXP_DECC.OPT	Link option file
MULTINET_VAX_DECC.OPT	Link option file
MULTINET_VAX_VAXC.OPT	Link option file
NETLIB_AXP_DECC.OPT	Link option file
NETLIB_VAX_DECC.OPT	Link option file
NETLIB_VAX_VAXC.OPT	Link option file
PHTOOL.C		Batch query client
QI.H			Symbolic constants for program suite
QI.MMS			MMS definitions used with MAKE_QI.COM
QI_ADD_ALIAS.C		Utility to add alias records
QI_ADD_NICKNAME.C	Utility to add nickname records
QI_ADD_SOUNDEX.C	Utility to add soundex records
QI_API.C		API routines
QI_API_AXP.OPT		Link option file
QI_API_VAX.OPT		Link option file
QI_BUILD.C		Utility to create the nameserver database
QI_BUILDCLD.CLD		CLD description for QI_BUILD
QI_MAIN.C		Main routine for nameserver
QI_MAKE.C		Utility to convert sequential files to QI_BUILD format
QI_NETLIB.C		NETLIB network routines
QI_QUERY.C		Query routines for nameserver
QI_SOCKET.C		Non NETLIB network routines
QI_SOUNDEX.C		Soundex utility routines
QI_UTIL.C		Misc utility routines for nameserver
STRCASE.C		String comparison utility routines
UCX_AXP_DECC.OPT	Link option file
UCX_VAX_DECC.OPT	Link option file
UCX_VAX_VAXC.OPT	Link option file
WOLLONGONG_AXP_DECC.OPT	Link option file
WOLLONGONG_VAX_DECC.OPT	Link option file
WOLLONGONG_VAX_VAXC.OPT	Link option file

[.TOOLS]

ADD_NS_SERVERS.COM	Script to add the servers found with NS_SERVERS.COM
NS_SERVERS.COM		Converts the NWU server listing to a sequential file
QI_UAFDUMP.EXE		Dumps SYSUAF to a sequential file (UAFDATA.TXT)
QI_UAFDUMP.PAS		Source
UAFDATA.QI_MAKE		Description of UAFDATA.TXT for QI_MAKE


Bruce Tanner
Cerritos College
Computer Services
11110 Alondra Blvd.
Norwalk, CA 90650

TANNER@Cerritos.edu


