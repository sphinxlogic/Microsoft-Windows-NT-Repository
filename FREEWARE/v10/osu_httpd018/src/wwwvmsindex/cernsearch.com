$! 'f$verify(0)
$!			CERNsearch.COM
$!
$!	17-JAN-1994	F.Macrides (macrides@sci.wfeb.edu)
$!			ISINDEX search script interface to the
$!			VMSIndex search engine (WWWquery).
$!			For use with CERN httpd.
$!
$!			Should be placed where your ISINDEX search script
$!			has been mapped in the server's configuration
$!			file, e.g.:  Search	/HTTPD_Dir/CERNsearch.COM
$!
$!			It allows you to specify a location of
$!			indexes (.IDX and .SEL files), and set up
$!			symbols for features of the hit lists, in
$!			calls to WWWquery.exe.  You can further
$!			elaborate this model script so that it
$!			tests the "database" symbol and sets the
$!			hit list features as a function of the index
$!			(e.g., don't set WWW_SEND_RAW_FILES if the
$!			index is for a sectional database), and/or
$!			substitutes databases whose names differ from
$!			the root of the ISINDEX document.
$!
$!----------------------------------------------------------------------------
$!
$!	WWWquery.c is my modification of Bruce Tanner's query.c for accessing
$!	foo.doc;*, foo.IDX;*, foo.SEL;* filesets.  The doc file(s) is(are)
$!	indexed with BUILD_INDEX.EXE, yielding the IDX and SEL filesets.
$!	This search engine was develeped by Bruce and colleagues associated
$!	with the VMSGopher-L@trln.lib.unc.edu list for use with the
$!	VMSGopherServer.  It is similar to WAIS, but uses native indexing
$!	facilities of the VMS Record Management System (RMS).
$!
$!	Build BUILD_INDEX.EXE, WWWquery.EXE (and QUERY.EXE, for the Gopher
$!	Server), by executing MAKE.COM.  It will check out your system and
$!	then use MMS or command files, and the appropriate foo.OPT files.
$!
$!	Read BUILD_INDEX.DOC for information about the indexer.  The IDX and
$!	SEL files built by the indexer are the same whether they're intended
$!	for use by the Gopher or HTTP Server, or from the DCL command line.
$!
$!	The example indexdemo.html ISINDEX document is for use with this
$!	script and an index of all HTML documents (i.e., which have extension
$!	.html) in your WWW_Root:[000000...] data tree.  The index should have
$!	been created with the example HTTP_INDEXTREE.COM in this distribution,
$!	and reside in the WWW_Root:[Index] subdirectory.  If instead of one,
$!	"whole tree" index, you built a series of indexes for components of
$!	your data tree, you could call WWWquery with a wildcarded index spec
$!	to search them all, or particular combinations which match the
$!	wildcarding.  Do that substitution where this script presently
$!	substitutes "VMSINDEX" for "INDEXDEMO" as the database symbol.
$!
$!	The symbol WWW_SEND_RAW_FILE must be made non-NULL (we set it equal
$!	to "YES", below, but any string will do, e.g., "TRUE"), so that
$!	WWWquery will not add html "packaging", as the .html file should
$!	already have a <HEAD> section with <TITLE>.
$!
$!	Note that the symbol WWW_OMIT_PRE_TAG, if defined, would cause
$!	the <PRE> tag to be omitted by WWWquery, but not the other html
$!	"packaging".  You want it all omitted, since you presumably
$!	indexed sets of complete, normally independent foo.html files.
$!
$!	The symbol WWW_SHOW_FILETYPE can be made non-NULL (e.g., "YES") to
$!	have icons (or ALT text) indicating the file types to be inserted
$!	into the hit lists returned from searches.
$!
$!	If the symbol WWW_MAXIMUM_HITS is made non-NULL (i.e., set equal to
$!	a number), then the hit lists returned by searches will be abridged,
$!	if necessary, so as not to exceed that number.
$!
$!	The symbol WWW_SHOW_SIZE can be made non-NULL (e.g., "YES") to have
$!	the size of each file or database section indicated in the hit lists.
$!
$!	The symbol WWW_SHOW_DATE can be made non-NULL to have the date of the
$!	database indicated in the hit lists (DD-MMM-YY).  If WWW_SHOW_TIME
$!	also is made non-NULL, for the current year the hour and minute will
$!	be indicated instead of the year (DD-MMM HH:MM).  If WWW_USE_MDATE is
$!	made non-NULL, the date of the last modification (e.g., from an
$!	APPEND) will be used.  The default is the file creation date (i.e.,
$!	of the highest version).
$!
$!	Set up the foreign command definition for WWWquery, below, or define
$!	it in the HTTP Server's SpawnInit.com.
$!
$!----------------------------------------------------------------------------
$! WWWquery := "$device:[directory]WWWquery"
$!------------------------------------------
$ INDEX_DIR = "WWW_Root:[Index]"	! Location of indexes for WWWquery.exe.
$!----------------------------------------------------------------------------
$ If P1 .eqs. "HEAD"			! Request for header only.
$ Then
$   say = "write WWW_OUT"
$   say "Content-type: text/html"	! CGI header.
$   say ""				! Required blank line.
$   goto done
$ Endif
$!----------------------------------------------------------------------------
$!
$! WWW_PATH_INFO has the ISINDEX URL.
$!
$ ISINDEX_URL = f$edit(WWW_PATH_INFO,"COLLAPSE,UPCASE")
$ gosub EXTRACT_DATABASE		! Strip it to the database root.
$!----------------------------------------------------------------------------
$!
$! Check the database root symbol for ones we want to change (Otherwise,
$! we assume the database has the same root as the ISINDEX document).
$!
$ if database .eqs. "INDEXDEMO" then database = "VMSINDEX"
$!
$! Add the location of indexes.
$!
$ database = INDEX_DIR + database
$!
$! Hit list options:
$! =================
$  WWW_SEND_RAW_FILE = "YES"	! Send direct URL's without a Range
$  WWW_SHOW_FILETYPE = "YES"	! Show filetype icons or ALT strings
$  WWW_MAXIMUM_HITS = 250	! Limit number of hits to return
$  WWW_SHOW_SIZE = "YES"	! Bytes or KB
$! WWW_SHOW_DATE = "YES"	! DD-MMM-YY
$! WWW_SHOW_TIME = "YES"	! Replace -YY with HH:MM for current year
$! WWW_USE_MDATE = "YES"	! Use last modified date (default is creation)
$!----------------------------------------------------------------------------
$!
$! Reconstruct the query (unescaped).
$!
$ query = WWW_KEY_1
$ num = 2
$ Loop:
$ If (WWW_KEY_COUNT - num) .ge. 0 
$ Then
$   query = query + " " + WWW_KEY_'num'
$   num = num + 1
$   goto Loop
$ EndIf
$!----------------------------------------------------------------------------
$!
$! Do the search.
$!
$ WWWquery "''database'" "''query'"
$!
$done:
$ exit
$!
$!***************** EXTRACT_DATABASE SUBROUTINE ******************************
$! Extract database symbol from ISINDEX_URL symbol
$!
$ EXTRACT_DATABASE:
$ database = ISINDEX_URL
$ trimming_loop:
$ offset = f$locate("/",database)+1
$ If offset .le. f$length(database)
$ Then
$   database = f$extract(offset,f$length(database)-offset,database)
$   goto trimming_loop
$ Else
    database = f$extract(0,f$locate(".",database),database)
$ Endif
$ RETURN   
$!************************ END EXTRACT_DATABASE ******************************
