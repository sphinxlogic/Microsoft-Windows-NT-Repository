/*									*/
/*	Copyright (©) Ecole Nationale Supérieure des Télécommunications */
/*									*/
/*	EMON: moniteur ethernet	(V2.1-3)				*/
/*									*/
/*	20-fev-1994:	Guillaume gérard				*/
/*									*/
/*	Ce logiciel est fourni gratuitement et ne peut faire		*/
/*		l'objet d'aucune commercialisation			*/
/*									*/
/*	Aucune garantie d'utilisation, ni implicite,			*/
/*		ni explicite, n'est fournie avec ce logiciel.		*/
/*		Utilisez-le à vos risques et périls			*/
/*									*/
/*	This freeware is given freely and cannot be sold		*/
/*	No warranty is given for reliability of this software		*/
/*	Use at your own risk						*/
/*									*/
#include stdio
#include unixio
main()
{
	int fd;
	char buf[512];
	int bufsiz= sizeof buf;
	int mod= 0;

	fd= open ("ip_found.dat", 0, "rb");
	if (fd>=0) {
		int i;
		char * cp;
		read (fd, buf, bufsiz);
		for (cp= buf, i=0; i<bufsiz; i++, cp++) {
			if (*cp != 0) {
				char name[8];
				if (i<256) sprintf (name, "%d", i);
				else sprintf (name, "%d.%d", i/256, i%256);
				if (*cp < 0) printf ("%s ", name);
				else {	char rep;
					printf ("\n>>> %s: ", name);
					scanf  ("%c", &rep);
					scanf ("\n");
					if ((rep=='n') || (rep=='N')) continue;
					mod= 1;
					*cp |= 0x80;
				}
			}
		}
		if (mod) {
			if (close(fd) < 0) perror ("close file");
			fd= creat ("ip_found.dat","wb","ctx=rec","rfm=var");
			if (fd>=0) {
				write (fd, buf, bufsiz);
				close (fd);
			}
			else	perror ("open file for write");
		}
	}	
	else	perror ("open file for read");
}
