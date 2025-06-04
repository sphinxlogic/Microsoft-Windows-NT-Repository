$ old_default = f$environment("DEFAULT")
$ on error then $ goto exit
$ on control_y then $ goto exit
$ proc = f$environment("PROCEDURE")
$ procdir = f$parse(proc,,,"DEVICE") + f$parse(proc,,,"DIRECTORY")
$ set default 'procdir'
$ define/nolog pqm_obj 'f$env("DEFAULT")'
$ pascal := pascal/debug=traceback/optimize/usage=(all,noperformance)
$ pascal globaldef
$ pascal pqm
$ pascal browser
$ pascal jobs
$ pascal queues
$ set command/object pqm_foreign
$ link/nodebug/exe=pqm.'f$getsyi("ARCH_NAME")'_exe  pqm/opt,version/opt
$ delete *.obj;*,*.pen;*
$ deassign pqm_obj
$ exit
