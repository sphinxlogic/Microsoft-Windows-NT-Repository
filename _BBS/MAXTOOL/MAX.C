/*
  
	Maximus CDROM Mount :
  
	Creates FILES.BBS files and CDAREAS.CTL from indexfile on 
	CD-ROM from Walnut Creek CD-ROM.
  
	Usage :
  
	MAX <CD-ROMdrive> <Directory for FILES.BBS files> [starting area]
  
	Example :
  
	MAX G: C:\MAX\CDAREAS
	MAX G: C:\MAX\CDAREAS 100
  
*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void 
main(int argc, char **argv) {
	FILE *dosindex;
	FILE *areasfile;
	FILE *filesbbsfile = NULL;

	char name[255];

	char indexfname[80];
	char filesbbsname[80];
	char dirname[80];
	char areadesc[80];
	char areaname[15];

	char buf[512];
	char tmps[512];

	long totdirs = 0;

	char *p, *r;

	printf("ษอออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออธ\n");
	printf("บ                                                                 ณ\n");
	printf("บ                   ฎฎฎ Maximus CDROM Mount ฏฏฏ                  ณ\n");
	printf("บ                                                                 ณ\n");
	printf("บ Creates Files.Bbs files and Hcdareas.Ctl from indexfiles on the ณ\n");
	printf("บ CD-ROM from Walnut Creek CD-ROM.                                ณ\n");
	printf("บ                                                                 ณ\n");
	printf("บ (C) Copyright 1993 Koos van den Hout /                          ณ\n");
	printf("บ                            Van den Hout Creative Communications ณ\n");
	printf("บ (C) copy right 1993 Walnut Creek CDROM                          ณ\n");
	printf("บ This program is Freeware. Feel free to share the executable /   ณ\n");
	printf("บ source with anyone. Read MaxHmnt.Doc for license.               ณ\n");
	printf("บ                                                                 ณ\n");
	printf("ำฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู\n\n");

	if (argc < 3) {
		printf("Usage :\n\n");
		printf("MAX <CD-ROMdrive> <Directory for FILES.BBS files> [starting area number]\n\n");
		printf("Example:\n");
		printf("MAX G: C:\\MAX\\CDAREAS\n");
		printf("MAX G: C:\\MAX\\CDAREAS 100\n\n");
		printf("Read MAXHMNT.DOC for more info.\n");
		exit(1);
	}
	strupr(argv[1]);
	strupr(argv[2]);

	if (argc == 4) {
		if (0 == (totdirs = atoi(argv[3]))) {
			fprintf(stderr, "''%s'' is not a starting area number\n", argv[3]);
			exit(1);
		}
		--totdirs;
	}

	strcpy(indexfname, argv[1]);
	strcat(indexfname, "\\_bbs\\dirs.txt");

	if ((dosindex = fopen(indexfname, "rt")) == NULL) {
		printf("Can't open index file %s\n", indexfname);
		exit(1);
	}
	if ((areasfile = fopen("HCDAREAS.CTL", "wt")) == NULL) {
		printf("Can't write area file\n");
		exit(1);
	}
	
	fprintf(areasfile, "% Areas file of Walnut Creek CDROM.\n");
	fprintf(areasfile, "% Created by Max\n\n");
	while (NULL != fgets(buf, 512, dosindex)) {

		buf[strlen(buf) - 1] = 0;

		if (0 == strlen(buf))
			continue;
		
		if (0 == strncmp(buf, "---", 3))
			break;

		p = strchr(buf, ' ');
		r = strchr(buf, '\t');
		if (r && r < p)
			p = r;
		*p = 0;
		strcpy(dirname, buf);
		*p = ' ';
		while (isspace(*p))
			++p;

		/* p now at description */
				
		++totdirs;

		strcpy(areadesc, p);

		sprintf(areaname, "HC%03li", totdirs);
		strupr(areaname);

		printf("[%03li] %s%s >> %s\n",
				  totdirs, argv[1], dirname, areadesc);

		strcpy(filesbbsname, argv[2]);

		if (filesbbsname[strlen(filesbbsname) - 1] != '\\')
			strcat(filesbbsname, "\\");
		
		sprintf(tmps, "HC%03li.BBS", totdirs);
		strcat(filesbbsname, tmps);

		sprintf(buf, "copy %s%s\\files.bbs %s", argv[1], dirname, filesbbsname);

		fprintf(stderr, "executing ``copy %s\\files.bbs %s''\n",
				  dirname, filesbbsname);

		system(buf);

		fprintf(areasfile, "Area %s\n", areaname);
		fprintf(areasfile, "        Access           Disgrace/S\n");
		fprintf(areasfile, "        FileInfo         %s\n", areadesc);
		fprintf(areasfile, "        Download         %s%s\n", argv[1], dirname);
		fprintf(areasfile, "        FileList         %s\n", filesbbsname);
		fprintf(areasfile, "End Area\n\n");
	}

	fclose(filesbbsfile);
	fclose(areasfile);
	fclose(dosindex);
}
