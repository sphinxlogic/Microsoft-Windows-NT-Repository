$! 'f$verify(0)
$!			VMSIndex.COM
$!
$!	27-APR-1994	F.Macrides (macrides@sci.wfeb.edu)
$!			Example htbin script interface to the
$!			VMSIndex search engine (WWWquery).
$!			For use with CERN v216-1betavms httpd.
$!			Should be placed where your htbin scripts
$!			have been mapped.
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
$!	An example HTML anchor and an example gopher tuple for invoking this
$!	script are included in BUILD_INDEX.DOC.
$!	
$!	This example script is for use with an index of all HTML documents
$!	(i.e., which have extension .html) in your WWW_Root:[000000...]
$!	data tree.  The index should have been created with the example
$!	HTTP_INDEXTREE.COM in this distribution, and reside in the
$!	WWW_Root:[Index] subdirectory.  If instead of one, "whole tree"
$!	index, you built a series of indexes for components of your data
$!	tree, you could call WWWquery with a wildcarded index spec to search
$!	them all, or particular combinations which match the wildcarding.
$!
$!	If no query was included in the client's request, a "database cover
$!	page" is returned.  Modify that section, below, to indicate yourself
$!	and your host as the "database maintainer".  The cover page assumes
$!	that the searchwords.html HELP file resides in the WWW_Root:[doc]
$!	subdirectory.
$!
$!	The symbol WWW_SEND_RAW_FILE must be made non-NULL (we set it equal
$!	to "TRUE", below, but any string will do, e.g., "YES"), so that
$!	WWWquery will not add html "packaging", as the .html file should
$!	already have a <HEAD> section with <TITLE>.
$!
$!	Note that the symbol WWW_OMIT_PRE_TAG, if defined, would cause
$!	the <PRE> tag to be omitted by WWWquery, but not the other html
$!	"packaging".  You want it all omitted, since you presumably
$!	indexed sets of complete, normally independent foo.html files.
$!
$!	Upon completion of field testing and incorporation of enhancements,
$!	WWWquery.c and query.c will be combined, with a test along the lines
$!	of  if (getenv("WWW_SCRIPT_NAME" ...)   used to set a flag for using
$!	code sections for the Gopher versus HTTP Servers, so keep that in
$!	mind when you hack the code, and don't get rid of anything we'll want
$!	to put back.
$!
$!	Set up the foreign command definition for WWWquery, below, or define
$!	it in the HTTP Server's SpawnInit.com.
$!
$!----------------------------------------------------------------------------
$! WWWquery := "$device:[directory]WWWquery"
$!------------------------------------------
$ say = "write WWW_OUT"
$ IF WWW_KEY_COUNT .eq. 0
$ THEN
$!
$!  Send Cover Page
$!
$   say "Content-Type: text/html"
$   say ""
$   say "<HTML><HEAD>"
$!***** Change the next five lines, appropriately for your site. *****
$   say "<TITLE>Search all html files on sci.wfeb.edu</TITLE>"
$   say "</HEAD><BODY>"
$   say "<H1>Search all html files on sci.wfeb.edu</H1>"
$   say "This database is maintained by macrides@sci.wfeb.edu"
$   say "on host sci.wfeb.edu."
$   say "<p>"
$   say "The search matches your words against full document text."
$   say "Access to this index is provided by the WWW to VMSIndex gateway."
$   say "Please specify <A HREF=""/www/doc/searchwords.html"""
$   say ">search words</A> to find documents."
$   say "Click <A HREF=""/htbin/vmsindex?title"""
$   say ">here</A> or use title as the search word to fetch"
$   say "a menu for all documents in this database."
$   say "</BODY><iSINDEX></HTML>"
$ ELSE
$!
$!  Do Search or Send Document Section
$!
$   WWW_SEND_RAW_FILE = "TRUE"
$   query = WWW_KEY_1
$   num = 2
$   Loop:
$   If (WWW_KEY_COUNT - num) .ge. 0 
$   Then
$     query = query + " " + WWW_KEY_'num'
$     num = num + 1
$     goto Loop
$   EndIf
$   WWWquery "WWW_Root:[Index]VMSindex" "''query'"
$ ENDIF
$exit
