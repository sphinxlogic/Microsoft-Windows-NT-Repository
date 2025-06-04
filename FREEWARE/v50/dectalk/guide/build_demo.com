$ basic basic_dtdemo
$ cobol cobol_dtdemo
$ cc    c_dtdemo
$ link basic_dtdemo
$ link cobol_dtdemo
$ link c_dtdemo, sys$library:vaxcrtl.olb
