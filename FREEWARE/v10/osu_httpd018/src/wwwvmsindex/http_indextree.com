$! v = 'f$verify(0)'
$!
$!			HTTP_INDEXTREE.COM for the CERN server.
$!
$!	30-APR-1994	F.Macrides (macrides@sci.wfeb.edu)
$!			Example command procedure for indexing
$!			all .html files in the httpd data tree.
$!			Assumes that the program BUILD_INDEX.EXE,
$!			and the files SEQIDX.FDL, SEQSEL.FDL and
$!			NOISE_WORDS.DAT are in your current default
$!			directory, and that the subdirectory
$!			WWW_Root:[Index] has been created in your
$!			data tree for holding .IDX and .SEL files.
$!			The .html files must have <title> at the
$!			beginning of a line (case doesn't matter),
$!			e.g.:
$!
$!			<HTML>
$!			<HEAD>
$!			<TITLE>Welcome to the sci.wfeb.edu http server</TITLE>
$!			<link rev=made href="mailto:macrides@sci.wfeb.edu">
$!			</HEAD>
$!
$!		Note that instead of building one index via a full data tree
$!		wildcard, one could create a series of of indexes, e.g., for
$!		files in individual subdirectories, or with a common
$!		characteristics and correspondingly common elements in their
$!		filenames to permit wildcarding in the filename field.  Then,
$!		in the htbin script, one could call WWWquery or DTquery with
$!		wildcarded index specs, to combine them in the search.
$!
$!		The "/nodefault_topic" qualifier will cause any files which
$!		do not have "<TITLE>" at the beginning of a line to be
$!		excluded from indexing.  You can use this "feature" as the
$!		equivalent of an exclusion switch in this script, i.e., to
$!		exclude particular files from a "total tree" index (e.g.,
$!		because you're still working on them, or they have material
$!		you can't offer publically and you don't want them to appear
$!		in a hit list and then tell the user to get lost if he/she
$!		clicks on them), put <HEAD><TITLE>Blah ... in the those files.
$!		You can then index them separately by changing the "/topic"
$!		qualifier, below, to: /topic=(text="<HEAD><TITLE>", exclude) -
$!		and regulate access to that index by restricting access to
$!		its cover_page/search/fetch script.
$!
$ v = f$verify(1)
$!
$! Create the Sequential files
$!
$ THIS_PATH = F$Element (0, "]", F$Environment ("PROCEDURE")) + "]"
$ index := $'THIS_PATH'build_index.exe
$ index /sequential/whole/noversion/noise=noise_words.dat -
	/nodefault_topic -
        /topic=(text="<TITLE>", exclude) -
	/output=VMSindex -
	WWW_Root:[000000...]*.html;0
$!
$! Sort and Convert the SEQIDX to an IDX file
$!
$ sort VMSindex.seqidx VMSindex.seqidx
$ convert/fdl=seqidx VMSindex.seqidx WWW_Root:[Index]VMSindex.idx
$!
$! Sort and Convert the SEQSEL to an SEL file
$!
$ sort VMSindex.seqsel VMSindex.seqsel
$ convert/fdl=seqsel VMSindex.seqsel WWW_Root:[Index]VMSindex.sel
$!
$! Clean up
$!
$ purge /nolog/noconfirm  WWW_Root:[Index]VMSindex.idx
$ rename/nolog/noconfirm  WWW_Root:[Index]VMSindex.idx ;1
$ delete/nolog/noconfirm  VMSindex.seqidx;*
$ purge /nolog/noconfirm  WWW_Root:[Index]VMSindex.sel
$ rename/nolog/noconfirm  WWW_Root:[Index]VMSindex.sel ;1
$ delete/nolog/noconfirm  VMSindex.seqsel;*
$ v = 'f$verify(0)'
$ WRITE SYS$OUTPUT ""
$exit
