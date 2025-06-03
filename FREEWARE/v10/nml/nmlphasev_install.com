$ arch= f$getsyi ("Arch_name")
$ copy:= copy/log
$ exefile= f$search ("nmlphasev_nml.exe")
$ if exefile.eqs."" then exefile= f$search ("nmlphasev_nml_''arch'.exe")
$ if exefile.eqs."" then exefile:= [.'arch']nmlphasev_nml.exe
$ copy 'exefile' sys$common:[sysexe]nmlphasev_nml;/prot=w:Re
$ copy nmlphasev_startup.com sys$common:[sys$startup]/prot=w:re
$ copy nmlphasev_cml.com sys$common:[sysexe]/prot=w:Re
$ @ sys$startup:nmlphasev_startup.com
$ type sys$input

 Pensez a ajouter dans vos 	Remember to add in your 
   fichiers de startup 	             startup files

 		@ sys$startup:NMLPHASEV_startup

Apres le demarrage de decnet	After DecNet is started
