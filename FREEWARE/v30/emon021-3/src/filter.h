/*									*/
/*	Copyright (�) Ecole Nationale Sup�rieure des T�l�communications */
/*									*/
/*	EMON: moniteur ethernet	(V2.1-3)				*/
/*									*/
/*	20-fev-1994:	Guillaume g�rard				*/
/*									*/
/*	Ce logiciel est fourni gratuitement et ne peut faire		*/
/*		l'objet d'aucune commercialisation			*/
/*									*/
/*	Aucune garantie d'utilisation, ni implicite,			*/
/*		ni explicite, n'est fournie avec ce logiciel.		*/
/*		Utilisez-le � vos risques et p�rils			*/
/*									*/
/*	This freeware is given freely and cannot be sold		*/
/*	No warranty is given for reliability of this software		*/
/*	Use at your own risk						*/
/*									*/
boolean	get_levelfilter (struct levelfilter_st *f1, int level);
void	free_levelfilter (struct levelfilter_st *f1);
void	get_common_trace_options(void);
void	emon$dump_options (void (*rtn)(), int width);
