$! INDEX_SERVER_DOC.COM
$!	Process the Gopher Server documentation  and set it up for GOPHER
$!	indexed access, using the BUILD_INDEX program included in the
$!	VMS Gopher server distribution.
$!
$!	This requires:
$!	     *	running the BUILD_INDEX program against the copied archive
$!		file.
$!
$!	Installation:
$!	     *  where flagged below, change dev:[dir] specifications for
$!		 index_exe and targetdir for your installation.
$!
$! Maintenance Log
$!
$!  19931130	D.Sherman <dennis_sherman@unc.edu>
$!			fixed problem with self-determination of exe paths
$!  19931118	D.Sherman <dennis_sherman@unc.edu>
$!			added self-determination of exe paths
$!  19931112	D.Sherman (dennis_sherman@unc.edu)
$!			generalized and parameterized for easier installation
$!			 at other sites.  Added some comments.
$!  19930920	D.Sherman (dennis_sherman@unc.edu)
$!			moved to gopher_root:[searchidx], modified
$!			 for new gopher paths
$!  19930810	D.Sherman (dennis_sherman@unc.edu)
$!			added code to move index files automatically
$!  19930809	D.Sherman (dennis_sherman@unc.edu)
$!			original version
$!
$!	CHANGE THE FOLLOWING LINE FOR YOUR INSTALLATION
$  targetdir := gopher_root:[searchidx]
$!
$! =========== Nothing below here needs changing ==========
$!
$  say := write sys$output
$!
$  if (P1 .eqs. "") then P1 := server.doc
$  if (f$parse(p1,,,,"syntax_only")) .eqs. "" then goto err_p1
$  if (f$search(p1)) .eqs. "" then goto err_p1
$!
$  exe_path = f$environment("DEFAULT") - ".DOC]" + ".EXE]"
$  index := $'exe_path'build_index.exe
$  indexing_path = f$environment("DEFAULT") - ".DOC]" + ".INDEXING]"
$  define _noise_words 'indexing_path'_noise_words.dat
$!
$!	index the file, breaking sections on ^A (Ctrl-A)
$  index 'P1' /CHARACTER=%x01
$  fn = f$parse(P1,,,"DEVICE") + f$parse(P1,,,"DIRECTORY") -
      + f$parse(P1,,,"NAME")
$  cfn = "''fn'" + ".idx"
$  copy 'cfn' 'targetdir'*.*
$  del 'cfn';*
$  cfn = "''fn'" + ".sel"
$  copy 'cfn' 'targetdir'*.*
$  del 'cfn';*
$  goto fini
$!
$ ERR_P1:
$  say "''p1' is not a valid file name."
$  goto usage
$!
$ USAGE:
$  type sys$input
Usage: $ @INDEX_SERVER_DOC <doc_file>
	where 
		<doc_file>	=	full path of document to index
$  goto fini
$!
$ FINI:
$  deassign _noise_words
$  exit
