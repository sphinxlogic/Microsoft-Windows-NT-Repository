
chksum : chksum.c chksum_help.c md5.c
	cc -o chksum chksum.c md5.c

test :
	sum    chksum.c ; sum -r chksum.c ; ./chksum -r chksum.c
	sum -o chksum.c ; ./chksum -o chksum.c
	echo "The following test will fail under OSF1 unless you have the GNU version of the cksum program"
	cksum  chksum.c ; ./chksum -c chksum.c
	
install : clean chksum 
	strip chksum
	cp -f chksum /usr/local/bin/
	cp -f chksum.man /usr/local/man/man1/chksum.1
	chmod +x /usr/local/bin/chksum
	chmod +r /usr/local/man/man1/chksum.1


clean : 
	rm -f chksum

