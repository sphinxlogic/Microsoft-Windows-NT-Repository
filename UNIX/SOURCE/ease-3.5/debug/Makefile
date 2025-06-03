# makefile for doing regression tests on sendmail files
BINDIR=../bin

all: debug
debug:	good.out debug.out sendmail.cf
	diff debug.out good.out 

sendmail.ease: ../sendmail.ease
	@echo getting your ease file from the top directory
	cp ../sendmail.ease .
sendmail.cf:	sendmail.ease
	${BINDIR}/et -C sendmail.ease >sendmail.cf
debug.out:	debug.in sendmail.cf
	showwhere < debug.in	>debug.out
debug.tbl:	debug.out
	echo ".TS"	>debug.tbl
	echo "l l l l l"	>>debug.tbl
	echo "l l l l l." >>debug.tbl
	echo "Ruleset	Input	Mailer	hostname	User" >>debug.tbl
	cat debug.out >>debug.tbl
	echo ".TE" >>debug.tbl
report:	debug.tbl
	tbl debug.tbl | ptroff
	
good.out: 
	@ echo you have to create this yourself by typing
	@ echo "	cp debug.out good.out"
	@ echo do this once you feel your sendmail test suite
	@ echo has results that you feel are correct
	@ echo 
	@ echo do this everytime you feel you want to update
	@ echo your test suite
	@ cp good.out /dev/null


clean:
	rm debug.out debug.tbl 
