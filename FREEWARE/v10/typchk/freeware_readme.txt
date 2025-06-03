TYPE CHECKER, TOOLS, MACRO-32 Type checking tool for VAX and AXP systems

This DCL tool applies type checking rules to MACRO-32 code. For example, the
embedded data type in MACRO-32 field names can be checked against the MACRO-32
opcode's expectations for each argument. This tool can isolate bugs without any
debugging and can be used on both VAX and Alpha MACRO-32 listing files. It is
especially useful for MACRO-32 code which has been ported to Alpha, where byte and
word fields may have expanded to longwords. The tool helps assure all MACRO-32
opcodes use these changed fields in a longword fashion.

The tool consists of the following files:

TYPCHK.COM - the MACRO-32 Type Checker which is a DCL command file.
TYPCHK.DEFINITIONS - a text file containing all the type checking rules which
	you can tailor to your needs.
TYPCHK.PS - a postscript file containing a user guide.
TYPCHK_DIFF.COM - a DCL command file to perform a specialized difference 
	between two Type Checked listing files. This allows one to quickly
	access new Type Check warnings added as a result of a code change
	or to verify that Type Check warnings have been eliminated by a
	code change.
