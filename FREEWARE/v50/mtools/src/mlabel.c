/*
 * mlabel.c
 * Make an MSDOS volume label
 */

#include "sysincludes.h"
#include "msdos.h"
#include "mainloop.h"
#include "vfat.h"
#include "mtools.h"
#include "nameclash.h"

char *label_name(char *filename, int verbose, 
		 int *mangled, char *ans)
{
	int len;
	int i;
	int have_lower, have_upper;

	strcpy(ans,"           ");
	len = strlen(filename);
	if(len > 11){
		*mangled = 1;
		len = 11;
	} else
		*mangled = 0;
	strncpy(ans, filename, len);
	have_lower = have_upper = 0;
	for(i=0; i<11; i++){
		if(islower(ans[i]))
			have_lower = 1;
		if(isupper(ans[i]))
			have_upper = 1;
		ans[i] = toupper(ans[i]);

		if(strchr("^+=/[]:,?*\\<>|\".", ans[i])){
			*mangled = 1;
			ans[i] = '~';
		}
	}
	if (have_lower && have_upper)
		*mangled = 1;
	return ans;
}

struct directory *labelit(char *dosname,
			  char *longname,
			  void *arg0,
			  struct directory *dir)
{
	time_t now;

	/* find out current time */
	time(&now);
	mk_entry(dosname, 0x8, 0, 0, now, dir);
	return dir;
}

static void usage(void)
{
	fprintf(stderr, "Mtools version %s, dated %s\n",
		mversion, mdate);
	fprintf(stderr, "Usage: %s [-vscV] drive:\n", progname);
	exit(1);
}


void mlabel(int argc, char **argv, int type)
{
	int entry, verbose, clear, interactive, show, open_mode;
	struct directory dir;
	int result=0;
	char longname[VBUFSIZE],filename[VBUFSIZE];
	char shortname[13];
	ClashHandling_t ch;
	struct MainParam_t mp;
	Stream_t *RootDir;
	int c;
	int mangled;
	
	init_clash_handling(&ch);
	ch.name_converter = label_name;
	ch.ignore_entry = -2;

	verbose = 0;
	clear = 0;
	show = 0;

	while ((c = getopt(argc, argv, "vcs")) != EOF) {
		switch (c) {
			case 'v':
				verbose = 1;
				break;
			case 'c':
				clear = 1;
				break;
			case 's':
				show = 1;
				break;
			default:
				usage();
			}
	}

	if (argc - optind != 1 ||
	    !argv[optind][0] || argv[optind][1] != ':') 
		usage();

	init_mp(&mp);
	get_name(argv[optind], filename, mp.mcwd);
	interactive = !filename[0] && !show && !clear;
	if (filename[0] || clear || interactive)
		open_mode = O_RDWR;
	else
		open_mode = O_RDONLY;

	RootDir = open_subdir(&mp, argv[optind], open_mode, 0, 0);
	if(!RootDir && open_mode == O_RDWR && !clear && !filename[0] &&
	   ( errno == EACCES || errno == EPERM) ) {
		show = 1;
		interactive = 0;
		RootDir = open_subdir(&mp, argv[optind], O_RDONLY, 0, 0);
	}	    
	if(!RootDir) {
		fprintf(stderr, "%s: Cannot initialize drive\n", argv[0]);
		exit(1);
	}

	entry = 0;
	vfat_lookup(RootDir, &dir, &entry, 0, 0, ACCEPT_LABEL | MATCH_ANY,
		    NULL, shortname, longname);
	
	if(show || interactive){
		if(entry == -1)
			printf(" Volume has no label\n");
		else if (*longname)
			printf(" Volume label is %s (abbr=%s)\n",
			       longname, shortname);
		else
			printf(" Volume label is %s\n", shortname);

	}

	/* ask for new label */
	if(interactive){
		fprintf(stderr,"Enter the new volume label : ");
		fgets(filename, VBUFSIZE, stdin);
		if(filename[0])
			filename[strlen(filename)-1] = '\0';
	}

	if(!show && entry != -1){
		/* if we have a label, wipe it out before putting new one */
		if(interactive && filename[0] == '\0')
			if(ask_confirmation("Delete volume label (y/n): ",0,0)){
				FREE(&RootDir);
				exit(0);
			}
		dir.name[0] = DELMARK;
		dir.attr = 0; /* for old mlabel */
		dir_write(RootDir, entry-1, &dir);
	}

	if (!show && filename[0] != '\0') {
		ch.ignore_entry = 1;
		result = mwrite_one(RootDir,filename,0,labelit,NULL,&ch) ? 
		  0 : 1;
	}

	if(!show){
		struct bootsector boot;
		Stream_t *Fs = GetFs(RootDir);

		if(!filename[0])
			strncpy(shortname, "NO NAME    ",11);
		else
			label_name(filename, verbose, &mangled, shortname);

		if(force_read(Fs,(char *)&boot,0,sizeof(boot)) == 
		   sizeof(boot) &&
		   boot.descr >= 0xf0 &&
		   boot.ext.old.dos4 == 0x29 &&
		   _WORD(boot.fatlen)){
			strncpy(boot.ext.old.label, shortname, 11);
			force_write(Fs, (char *)&boot, 0, sizeof(boot));
		}
	}

	FREE(&RootDir);
	exit(result);
}
