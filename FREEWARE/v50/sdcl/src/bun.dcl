$ !/*  Bun -- VMS DCL command procedure to bundle files into     distribution package which can then be unbundled   */
$ !/*         using UNIX shell. The output will be placed on the */
$ !/*         on the file given as the arg to this procedure     */
IF( p1 .eqs. "" ){
    write sys$output\
    	"Usage: bundle outfile (outfile will receive bundle)"
    exit    /* DCL exit */
}
/* if the file exists, open it, otherwise create it */
open/write/err=out_err fout 'p1'
exist := "TRUE"
out_err:
If( exist .nes. "TRUE" ){
    create 'p1'
    open/write/err=give_up fout 'p1'
}
q := "'"
for( rc = 0; ; ){    /* no condition, no reinit */
    inquire infile "File? "
    if( infile .eqs. "" )
	break        /* time to wrapup */
    open/read/err=infile_err inf 'infile'
    write fout "echo ''infile' 1>&2"
    write fout "cat >''infile' <<''q'END OF ''infile'''q'"
    rc = rc + 2  
    done = 0
    while( done .eq. 0 ){
	read/end=eof inf line
	write       fout line
	rc = rc + 1
    }
    eof: close inf
    write fout "END OF ''infile'"
    rc = rc + 1
    next
    /*
     come here if trouble opening 'infile'
    */
    infile_err: write sys$output \
		   "error opening ''infile'"
}
if( rc .gt. 0 ){
    write sys$output "''rc' records written to ''p1'"
    close fout
}
else
    write sys$output "0 records written out"
exit
