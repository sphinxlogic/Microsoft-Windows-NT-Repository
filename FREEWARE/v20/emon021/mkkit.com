$ copy c$include:types.h []
$ copy [.alpha]emon.olb []emon_alpha
$ copy [.vax]emon.olb []emon_vax
$ files:= emon_*.olb,link.com,mkfilter,emon$cfg.cfg,-
net$event_local.ncl,types.h,emon$help.hlb,install.txt,-
[.doc]emon.doc*,-
[.driver_patch]*.*,-
[.src]emon$filter_internet.c,emon$test_record.c,emon.opt,-
record.h,trace,emon_*.opt
$ files= f$edit (files, "COLLAPSE")
$ purge/log 'files'
$ backup 'files' emon021-2.bck/save/block=4096
$ delete mx_root:[fileserv.emon]*.*;*
$ @ [utilitaires.reseau.vms_share]vms_share 'files' mx_root:[fileserv.emon]emon
$ delete types.h;*,emon_vax.olb;*,emon_alpha;*/log
