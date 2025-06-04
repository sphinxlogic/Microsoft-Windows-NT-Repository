$ set ver
$ !
$ ! 
$ link/exe=pan.exe 'p1' imp-note,add_mem_file,report_link,-
dir,mail_dir,help,cld,-
search,-
batch_cld,-
GET_BATCH_LOG_SPEC,-
rebuild,-
set_member,-
list,-
copy_max_uid,-
keyword,-
do_batch,-
enotes,-
emsg,-
sys$input/opt/notrace
sys$share:MAILSHR/share
sys$library:notes$share/share
disk$user4:[bailey.build]tpushr/share
name = PAN
