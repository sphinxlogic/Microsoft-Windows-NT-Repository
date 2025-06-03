$! 'f$verify(0)
$!			VMSGopherServer.COM
$!
$!	27-APR-1994	F.Macrides (macrides@sci.wfeb.edu)
$!			Example htbin script interface to the
$!			VMSIndex search engine (WWWquery).
$!			For use with CERN httpd.
$!			Should be placed where your htbin scripts
$!			have been mapped.
$!
$!	22-AUG-1994	Modified to return complete URL's in cover page. - FM
$!	16-OCT-1994	Added lots of symbol options (see below). - FM
$!	17-JAN-1995	Minor mods of cover page. - FM
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
$!	This example script is for use with the HTML version of the VMS
$!	Gopher Server documentation, VMSGopherServer.doc, and assumes that
$!	it has been placed in the WWW_Root:[doc.gopher] subdirectory of your
$!	httpd data tree, and has been indexed there via the HTTP_INDEXDOC.COM
$!	interface to the BUILD_INDEX.EXE indexing program.
$!
$!	If no query was included in the client's request, a "database cover
$!	page" is returned.  Modify that section, below, to indicate yourself
$!	and your host as the "database maintainer".  The cover page assumes
$!	that the searchwords.html HELP file resides in the WWW_Root:[doc]
$!	subdirectory.
$!
$!	VMSGopherServer.doc has an anchor pointing to egrep.html, which also
$!	is assumed to reside in the WWW_Root:[doc] subdirectory.
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
$   access = "http://" + f$edit(WWW_SERVER_NAME, "COLLAPSE,LOWERCASE")
$   if WWW_SERVER_PORT .ne. 80 then access = access + ":" + WWW_SERVER_PORT
$   say "Content-Type: text/html"
$   say ""
$   say "<HTML>"
$   say "<HEAD>"
$   say "<TITLE>Search the VMSGopherServer v1.2VMS-1 Documentation</TITLE>"
$   say "<ISINDEX>"
$   say "</HEAD>"
$   say "<BODY>"
$   say "<H1>VMSGopherServer</H1>"
$!***** Change the next two lines, appropriately for your site. *****
$   say "This database is maintained by macrides@sci.wfeb.edu"
$   say "on host sci.wfeb.edu."
$   say "<p>"
$   say "The search matches your words against full document text."
$   say "Access to this index is provided by the WWW to VMSIndex gateway."
$   say "Please specify <A HREF=""''access'/www/doc/searchwords.html"""
$   say ">search words</A> to find documents."
$   say "Click <A HREF=""''access'/htbin/vmsgopherserver?gopher"""
$   say ">here</A> or use gopher as the search word to fetch"
$   say "a menu for all 33 sections of this database."
$   say "</BODY>"
$   SAY "</HTML>"
$ ELSE
$!
$!  Do Search or Send Document Section
$!
$!  Hit list options:
$!  =================
$!  WWW_SHOW_FILETYPE = "YES"	! All text, so don't bother
$!  WWW_MAXIMUM_HITS = 250	! Only 33 sections, so don't bother
$   WWW_SHOW_SIZE = "YES"	! Bytes or KB
$!  WWW_SHOW_DATE = "YES"	! DD-MMM-YY, not worthwhile for databases
$!  WWW_SHOW_TIME = "YES"	! Replace -YY with HH:MM for current year
$!  WWW_USE_MDATE = "YES"	! Use last modified date (default is creation)
$!
$   query = WWW_KEY_1
$   num = 2
$   Loop:
$   If (WWW_KEY_COUNT - num) .ge. 0 
$   Then
$     query = query + " " + WWW_KEY_'num'
$     num = num + 1
$     goto Loop
$   EndIf
$   WWWquery "WWW_Root:[Index]VMSGopherServer" "''query'"
$ ENDIF
$exit
