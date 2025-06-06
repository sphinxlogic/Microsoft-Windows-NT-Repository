DECWRITE_RTF, Utilities, Read and write RTF with DECwrite 

These converters allow DECWRITE to import and export documents
in RTF. They also allow the CONVERT/DOCUMENT command line
interface to convert RTF documents. 


The converters 

	DDIF$READ_RTF.EXE	DDIF$WRITE_RTF.EXE

should be placed in the SYS$COMMON:[SYSLIB] directory.


When converting from RTF format warnings about unknown RTF
control words may be generated. The supported RTF format
is based on Microsoft WORD 2 format so later enhancements
are not available however, the resultant document should
still be usable.

An example of the errors generated by the CONVERT/DOCUMENT 
command is as follows:

$ convert/document TEST.rtf/form=rtf TEST.DDIF/form=ddif
%CONVERT-W-UNKCNTL, unknown RTF-control-word "\ansicpg" encountered.
%CONVERT-W-UNKCNTL, unknown RTF-control-word "\fromtext" encountered.
%CONVERT-W-UNKCNTL, unknown RTF-control-word "\uc" encountered.


When converting to RTF format you may encounter the errors:

%CONVERT-W-STYNOTREADABLE, Can't open style guide file: defstyle
or 
%CONVERT-W-STYNOTREADABLE, Can't open style guide file: dx_times

these can be resolved by defining the following logicals:

     $ define/sys defstyle cda$library:defstyle.ddif
     $ define/sys dx_times cda$library:dx_times.doc_style


The converters are not capable of using rooted logicals in the
paths to files:

	"USER_ROOT" = "DISK1:[ROOT.]"

	USER_ROOT:[USER]FILE.DOC	- will fail

	DISK1:[ROOT.USER]FILE.DOC	- will work

before entering DECWRITE the user is advised to $SET DEFAULT to 
a non-roooted path.
