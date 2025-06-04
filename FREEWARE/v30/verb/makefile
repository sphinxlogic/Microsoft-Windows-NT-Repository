CC = /optimize\=noinline/nodebug
LINK = /notraceback

VERB = verb verb_cld verb_command verb_disallows verb_entity \
       verb_file verb_type verb_find_entity init_cli

verb.exe : $(VERB).obj
	link$(LINK) $?, c/opt, sys\$system:sys.stb/selective

$(VERB).c : verb.h

.c.obj:
	cc$(CC) $*

.cld.obj:
	set command/object $*
