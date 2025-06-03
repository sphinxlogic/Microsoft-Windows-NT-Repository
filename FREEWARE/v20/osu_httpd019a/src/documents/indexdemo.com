$!
$! Build index files for HTTP demo.  Add /nodefault to exclude files
$! missing a <TITLE>
$!
$ index = "$" + f$parse("build_index.exe;",f$environment("PROCEDURE"))
$ index /sequential/whole/noversion/noise=noise_words.dat -
        /topic=(text="<TITLE>", exclude) -
	/output=indexdemo -
	WWW_Root:[DOCUMENTS...]*.html;0
$!
$ sort indexdemo.seqidx indexdemo.seqidx
$ convert/fdl=seqidx indexdemo.seqidx www_root:[documents]indexdemo.idx
$ sort indexdemo.seqsel indexdemo.seqsel
$ convert/fdl=seqsel indexdemo.seqsel www_root:[documents]indexdemo.sel
