# Don't edit makefile, edit makefile.bas instead.
#

RM=  rm -f

ALL	=	os-a32.c os-aix.c os-bsd.c os-bsi.c os-cvx.c os-dos.c \
		os-dyn.c os-gen.c os-hpp.c os-isc.c os-lnx.c os-nxt.c \
		os-osf.c os-ptx.c os-s40.c os-sgi.c \
		os-sun.c os-sv4.c os-ult.c

.SUFFIXES: .ic

.ic.c:
		./includer < $*.ic > $*.c

all:		includer $(ALL)

includer:	includer.c
		$(CC) -o includer includer.c

clean:
		$(RM) $(ALL) includer

# You don't have to run this unless you change a .ic file.
depend:
		./makedep

# Makedep only catches 1-level deep includes.  If something depends on a
# 2nd-level include, put it here.
os-a32.c: sunquota
os-dyn.c: sunquota
os-hpp.c: sunquota
os-osf.c: sunquota
os-ptx.c: sunquota
os-s40.c: sunquota
os-sgi.c: sunquota
os-sun.c: sunquota
os-sv4.c: sunquota

os-a32.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.a32 domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename srandom.dum tempfile tempnam writ_dir os-a32.ic
os-aix.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-aix.ic
os-bsd.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot domnames err_desc expnfldr fgetpos.non filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-bsd.ic
os-bsi.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.non domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-bsi.ic
os-cvx.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.non domnames err_desc expnfldr fgetpos.non filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-cvx.ic
os-dos.c: bld_path.dos canacces canonicl.dos chnge_pw.dos coredump creatdir.dos diskquot.non domnames.dos dosextra err_desc.dos expnfldr.dos fgetpos filesize fltrname.dos fnexpand.dos header hostname jobcntrl.dos lstcmpnt.dos mimedisp.dos play_snd.non pw_stuff.dos readfile.dos rename.dos tempfile tempnam.dos writ_dir os-dos.ic
os-dyn.c: bld_path canacces canonicl chnge_pw coredump.fpe creatdir diskquot.sun domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile.non tempnam writ_dir os-dyn.ic
os-gen.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename srandom.dum tempfile tempnam writ_dir os-gen.ic
os-hpp.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.hpp domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname.una jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename srandom.dum tempfile tempnam writ_dir os-hpp.ic
os-isc.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.non domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname iscextra jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename srandom.dum tempfile tempnam writ_dir os-isc.ic
os-lnx.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.non domnames err_desc expnfldr fgetpos.non filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-lnx.ic
os-nxt.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.non domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-nxt.ic
os-osf.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.sun domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-osf.ic
os-ptx.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.ptx domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname.una jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename srandom.dum tempfile tempnam writ_dir os-ptx.ic
os-s40.c: bld_path canacces canonicl chnge_pw coredump.fpe creatdir diskquot.sun domnames err_desc expnfldr fgetpos.non filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp.sun play_snd.sun pw_stuff readfile rename tempfile tempnam writ_dir os-s40.ic
os-sgi.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.sun domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-sgi.ic
os-sun.c: bld_path canacces canonicl chnge_pw coredump.fpe creatdir diskquot.sun domnames err_desc expnfldr fgetpos.non filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp.sun play_snd.sun pw_stuff readfile rename tempfile tempnam writ_dir os-sun.ic
os-sv4.c: bld_path canacces canonicl chnge_pw coredump creatdir diskquot.sv4 domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname.una jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename srandom.dum tempfile tempnam writ_dir os-sv4.ic
os-ult.c: bld_path canacces canonicl chnge_pw coredump.fpe creatdir diskquot domnames err_desc expnfldr fgetpos filesize fltrname fnexpand header hostname jobcntrl lstcmpnt mimedisp play_snd.non pw_stuff readfile rename tempfile tempnam writ_dir os-ult.ic
