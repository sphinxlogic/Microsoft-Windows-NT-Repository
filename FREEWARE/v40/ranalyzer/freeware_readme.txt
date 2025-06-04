RTN ANALYZER, UTILITIES, Routine call analyzer for C and BLISS source code 

The Routine Analyzer parses C and BLISS source code to identify routine
definitions and calls. It compiles basic statistics and call trees from this
information and generates SDML or HTML output for presentation. It also
generates annotated source code with line numbers and indices of defined
routines. The SDML format is suitable for development/support documentation.
The HTML format is heavily linked to provide rapid navigation between source
modules, routines, and cross-reference information.

The files USERS_GUIDE.TXT and USERS_GUIDE.HTML contain plain text and HTML
versions of the user's guide, respectively. The guide explains the purpose of
the analyzer and provides instructions on how to use it. The [.SRC] directory
contains complete source. The [.EXAMPLE] directory contains an example HTML
output of the analyzer run over its own source code; the file
[.EXAMPLE]RANALYZER_FILES.HTML is the "home page" for the example.

The Routine Analyzer was written by Steve Branam, Digital Equipment
Corporation.
