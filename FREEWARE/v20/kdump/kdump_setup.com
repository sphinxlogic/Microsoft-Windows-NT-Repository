$ cur_dev = f$parse(f$environment("PROCEDURE"),,,"DEVICE","SYNTAX_ONLY")
$ cur_dir = f$parse(f$environment("PROCEDURE"),,,"DIRECTORY","SYNTAX_ONLY")
$ define kdump_loc 'cur_dev''cur_dir'
$ define kdump kdump_loc:kdump.exe
$ set command 'cur_dev''cur_dir'jsy_kdump.cld
$ exit
