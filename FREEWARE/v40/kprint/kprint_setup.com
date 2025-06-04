$ cur_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE","SYNTAX_ONLY")
$ cur_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY","SYNTAX_ONLY")
$ cpu = f$edit(f$getsyi("arch_name"),"UPCASE, TRIM")
$ define kprint_loc 'cur_dev''cur_dir'
$ define kpri kprint_loc:kpri_'cpu'.exe
$ set command 'cur_dev''cur_dir'jsy_kprint_'cpu'.cld
$ exit
