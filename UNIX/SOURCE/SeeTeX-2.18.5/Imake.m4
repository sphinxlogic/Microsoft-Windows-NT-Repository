undefine(`include')

define(MakeSubdirs,
all::
	for dir in $1 ; do (echo Build in $$dir; cd $$dir ; make ) ; done
clean::
	for dir in $1 ; do (echo Clean in $$dir; cd $$dir ; make  clean) ; done
install::
	for dir in $1; do (echo Install in $$dir; cd $$dir; make install); done
)

define(DependSubdirs,
depend::
	for dir in $1 ; do (cd $$dir ; make depend) ; done
)

define(NormalLibraryObjectRule,)

define(NormalLibraryTarget,
lib$1.a: $2
	rm -f lib$1.a
	ar r lib$1.a $2
	ranlib lib$1.a
clean::
	rm -f lib$1.a $2
)

define(LintLibraryTarget,)

define(CleanTarget,)

define(TagsTarget,
tags::
	etags *.c *.h
)

define(NormalLintTarget,)

define(NormalProgramTarget,
$1:	$2
	${CC} ${CFLAGS} -o $1 $2 $4 $5
clean::
	rm -f $1 $2
)

define(SingleProgramTarget,
$1:	$(OBJS)
	${CC} ${CFLAGS} -o $1 $(OBJS) $(LOCAL_LIBRARIES) $(SYSLIBS)
clean::
	rm -f $1 $2
install::
	install -c -s $1 $(BININDIR)
)

define(ComplexProgramTarget,
$1:	$(OBJS)
	${CC} ${CFLAGS} -o $1 $(OBJS) $(LOCAL_LIBRARIES) $(SYSLIBS)
clean::
	rm -f $1 $2
install::
	install -c -s $1 $(BININDIR)
)

define(InstallProgram,
install:: $1
	install -c -s $1 $2
)

define(InstallNonExec,
install:: $1
	install -c $1 $2
)

define(InstallScript,
install:: $1
	install -c -m 555 $1.script $2/$1
)

define(InstallManPage,
install:: $1.man
	install -c $1.man $2/$1.1
)

define(InstallAppDefaults,
install:: $1.ad
	install -c $1.ad $RESOURCES/$1
)

define(DependTarget,)
