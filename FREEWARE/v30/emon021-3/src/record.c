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
#include "monitor"
#include "record.h"

#include <stdio.h>

boolean
start_record (filename)
char * filename;
{
	frecord= fopen (filename,"wb","dna=emon.rec","rfm=var");
	if (frecord==NULL) {
		/* on pourrait faire mieux */
		emon$put_error("error creating record file");
	}
	recording= frecord!=NULL;
	return recording;
}

void
stop_record()
{
	if (fclose(frecord) == 0) frecord=NULL;
}

void
write_record (void * buf, int bufsize)
{
	if (fwrite (buf, bufsize, 1, frecord)<=0) {
		abort_monitor ("error writing record file");
	}
}
