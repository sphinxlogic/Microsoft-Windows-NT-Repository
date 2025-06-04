! this MMS procedure uses the following symbols defined either externally
! ot via via MMS /MACRO=<tag>=<value>

! tag		values
! ---		------
! cpu		vax, axp
! opt_file	tcp_compiler_cpu (e.g. multinet_decc_vax)
! opt_flags	(cc flags like /define)
! apitv		apitv.obj or null

exes = [-]qi.exe [-]qi_api.exe [-]qi_make.exe [-]qi_build.exe \
	[-]qi_add_alias.exe [-]qi_add_nickname.exe [-]qi_add_soundex.exe \
	[-]apitest.exe

server = gopher_root:[server]

servers = $(server)qi.exe $(server)qi_api.exe $(server)qi_build.exe \
	$(server)qi_add_alias.exe $(server)qi_add_nickname.exe \
	$(server)qi_add_soundex.exe $(server)qi_make.exe

all : $(exes)
	!done

[-]qi.exe : qi_main.obj qi_util.obj qi_query.obj qi_soundex.obj \
	qi_$(socket).obj crypt.obj cryptit.obj strcase.obj
	$(link) $(linkflags) $(mms$source_list), $(opt_file)/opt

[-]qi_api.exe : qi_api.obj $(apitv) qi_util.obj qi_query.obj qi_soundex.obj \
	crypt.obj cryptit.obj strcase.obj
	$(link) /share=[-]qi_api $(mms$source_list), \
	qi_api_$(cpu).opt/opt, $(opt_file)/opt

[-]qi_make.exe : qi_make.obj
	$(link) $(linkflags) $(mms$source_list), $(opt_file)/opt

[-]qi_build.exe : qi_build.obj qi_buildcld.obj strcase.obj
	$(link) $(linkflags) $(mms$source_list), $(opt_file)/opt

[-]qi_add_soundex.exe : qi_add_soundex.obj qi_soundex.obj
	$(link) $(linkflags) $(mms$source_list), $(opt_file)/opt

[-]qi_add_alias.exe : qi_add_alias.obj
	$(link) $(linkflags) $(mms$source_list), $(opt_file)/opt

[-]qi_add_nickname.exe : qi_add_nickname.obj
	$(link) $(linkflags) $(mms$source_list), $(opt_file)/opt

[-]apitest.exe : apitest.obj
	$(link) $(linkflags) $(mms$source_list), api.opt/opt, $(opt_file)/opt

qi_main.obj : qi_main.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_api.obj : qi_api.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_util.obj : qi_util.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_query.obj : qi_query.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_$(socket).obj : qi_$(socket).c
	$(cc) $(opt_flags) $(cflags) $(mms$source)
                                      
qi_build.obj : qi_build.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_make.obj : qi_make.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_add_alias.obj : qi_add_alias.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_add_nickname.obj : qi_add_nickname.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

qi_add_soundex.obj : qi_add_soundex.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

apitest.obj : apitest.c qi.h
	$(cc) $(opt_flags) $(cflags) $(mms$source)

deb : 
	cflags = "/noopt/debug"
	linkflags = "/debug"
	mms /override /descrip=qi

install : $(servers)
	!done

$(server)qi.exe : [-]qi.exe
	copy [-]qi.exe $(server)
        if f$file("$(server)qi.exe","known") -
        then install replace $(server)qi.exe
        if .not. f$file("$(server)qi.exe","known") -
        then install add $(server)qi.exe /open/header/share

$(server)qi_api.exe : [-]qi_api.exe
	copy [-]qi_api.exe $(server)
        if f$file("$(server)qi_api.exe","known") -
        then install replace $(server)qi_api.exe
        if .not. f$file("$(server)qi_api.exe","known") -
        then install add $(server)qi_api.exe /open/header/share
        define/sys/exec cso_apishr $(server)qi_api.exe

$(server)qi_build.exe : [-]qi_build.exe
	copy [-]qi_build.exe $(server)

$(server)qi_add_soundex.exe : [-]qi_add_soundex.exe
	copy [-]qi_add_soundex.exe $(server)

$(server)qi_add_alias.exe : [-]qi_add_alias.exe
	copy [-]qi_add_alias.exe $(server)

$(server)qi_add_nickname.exe : [-]qi_add_nickname.exe
	copy [-]qi_add_nickname.exe $(server)

$(server)qi_make.exe : [-]qi_make.exe
	copy [-]qi_make.exe $(server)

