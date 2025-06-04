$ v = 'f$verify (0)'
$!------------------------------------------------------------------------------
$!
$!  Simply show available Global Pages and Sections.
$!
$!  Author:  Jonathan Ridler,
$!	     Information Technology Services,
$!	     The University of Melbourne,
$!	     Victoria, Australia.
$!
$!  Email: jonathan@unimelb.edu.au
$!
$!  History:
$!	24-Jun-1993	JER	Original Version.
$!------------------------------------------------------------------------------
$
$ say := write SYS$OUTPUT
$
$ gpf = f$getsyi ("free_gblpages")
$ gpt = f$getsyi ("gblpages")
$ gpu = gpt - gpf
$ gsf = f$getsyi ("free_gblsects")
$ gst = f$getsyi ("gblsections")
$ gsu = gst - gsf
$
$ say "Global Resource		Free	Used	Total"
$ say "---------------		----	----	-----"
$ say "Global Pages		''gpf'	''gpu'	''gpt'"
$ say "Global Sections		''gsf'	''gsu'	''gst'"
$
$ EXIT 1 + 0 * f$verify (v)
