# Build fork-based scripts for HTTP server

scripts : makefile testfork indirect chatter
	chmod a+x ../cgi-bin/man2html

testfork : testfork.c
	cc -o testfork testfork.c
	cp testfork ../cgi-bin/testfork

indirect : indirect.c
	cc -o indirect indirect.c
	cp indirect ../cgi-bin/indirect

chatter : chatter.c
	cc -o chatter chatter.c
	cp chatter ../cgi-bin/chatter
