#include "sysincludes.h"
#include "msdos.h"
#include "mtools.h"
#include "patchlevel.h"
#include "partition.h"
#include "vfat.h"

/*
** 13-Mar-2000 Sergey Tikhonov
**	Added CLI interface to OpenVMS port
*/

const char *mversion = VERSION;
const char *mdate = DATE;
const char *progname;

static const struct dispatch {
	const char *cmd;
	void (*fn)(int, char **, int);
	int type;
} dispatch[] = {
	{"mattrib",mattrib, 0},
	{"mbadblocks",mbadblocks, 0},
	{"mcd",mcd, 0},
	{"mcopy",mcopy, 0},
	{"mdel",mdel, 0},
	{"mdeltree",mdel, 2},
	{"mdir",mdir, 0},
	{"mformat",mformat, 0},
	{"minfo", minfo, 0},
	{"mlabel",mlabel, 0},
	{"mmd",mmd, 0},
	{"mmount",mmount, 0},
	{"mpartition",mpartition, 0},
	{"mrd",mdel, 1},
	{"mread",mcopy, 0},
	{"mmove",mmove, 0},
	{"mren",mmove, 1},
	{"mtoolstest", mtoolstest, 0},
	{"mtype",mcopy, 1},
	{"mwrite",mcopy, 0},
	{"mzip", mzip, 0}
};
#define NDISPATCH (sizeof dispatch / sizeof dispatch[0])

#ifdef VMS
#ifdef USE_CLI

#include <clidef.h>
#include <climsgdef.h>
#include <cli$routines.h>

#include <descrip.h>

typedef struct
{
    void    *ParamDsc;
    char    *C_subs;
    char    *C_subs_NO;
    int	    Value;
} PARAM_S;

int MyErrorHandler(unsigned long int *sigarg, void *mecharg)
{
    return 1;
}

$DESCRIPTOR (Q_ToolsDsc, "tool");
$DESCRIPTOR (Q_VerboseDsc, "verbose");
$DESCRIPTOR (Q_WideDsc, "wide");
$DESCRIPTOR (Q_AllDsc, "all");
$DESCRIPTOR (Q_ConciseDsc, "concise");
$DESCRIPTOR (Q_FastDsc, "fast");
$DESCRIPTOR (Q_ClearDsc, "clear");
$DESCRIPTOR (Q_ShowDsc, "show");
$DESCRIPTOR (Q_ArchiveDsc, "archive");
$DESCRIPTOR (Q_HiddenDsc, "hidden");
$DESCRIPTOR (Q_SystemDsc, "system");
$DESCRIPTOR (Q_ReadonlyDsc, "readonly");
$DESCRIPTOR (Q_InteractiveDsc, "interactive");
$DESCRIPTOR (Q_NowarningDsc, "nowarning");
$DESCRIPTOR (Q_SilentDsc, "silent");
$DESCRIPTOR (Q_TracksDsc, "tracks");
$DESCRIPTOR (Q_HeadsDsc, "heads");
$DESCRIPTOR (Q_SectorsDsc, "sectors");
$DESCRIPTOR (Q_LabelDsc, "label");
$DESCRIPTOR (Q_SerialnumberDsc, "serialnumber");
$DESCRIPTOR (Q_HardssizeDsc, "hardssize");
$DESCRIPTOR (Q_SoftssizeDsc, "softssize");
$DESCRIPTOR (Q_PreserveDsc, "preserve");

$DESCRIPTOR (P_DriveDsc, "drive");
$DESCRIPTOR (P_DestDsc, "dest");

$DESCRIPTOR (RetDsc, NULL);

#define	MAX_C_PARAMS	16
#define	MAX_VALUE	32

static char *FakeArgv[MAX_C_PARAMS];
static char FakeParamBuf[256];
static char *FakeParamPtr;
static int  FakeArgc;

static PARAM_S	MInfoPar[] = {
    { &Q_VerboseDsc, "-v", NULL, 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MDirPar[] = {
    { &Q_WideDsc, "-w", NULL, 0 },
    { &Q_AllDsc, "-a", NULL, 0 },
    { &Q_ConciseDsc, "-X", NULL, 0 },
    { &Q_FastDsc, "-f", NULL, 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MCdPar[] = {
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MDelPar[] = {
    { &Q_VerboseDsc, "-v", NULL, 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MLabelPar[] = {
    { &Q_VerboseDsc, "-v", NULL, 0 },
    { &Q_ClearDsc, "-c", NULL, 0 },
    { &Q_ShowDsc, "-s", NULL, 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MCopyPar[] = {
    { &Q_VerboseDsc, "-v", NULL, 0 },
    { &Q_PreserveDsc, "-m", NULL, 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { &P_DestDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MAttribPar[] = {
    { &Q_ArchiveDsc, "+a", "-a", 0 },
    { &Q_HiddenDsc, "+h", "-h", 0 },
    { &Q_SystemDsc, "+s", "-s", 0 },
    { &Q_ReadonlyDsc, "+r", "-r", 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MMdPar[] = {
    { &Q_VerboseDsc, "-v", NULL, 0 },
    { &Q_InteractiveDsc, "-i", NULL, 0 },
    { &Q_NowarningDsc, "-n", NULL, 0 },
    { &Q_SilentDsc, "-X", NULL, 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MMovePar[] = {
    { &Q_VerboseDsc, "-v", NULL, 0 },
    { &Q_InteractiveDsc, "-i", NULL, 0 },
    { &Q_NowarningDsc, "-n", NULL, 0 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { &P_DestDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MBadblocksPar[] = {
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static PARAM_S	MFormatPar[] = {
    { &Q_TracksDsc, "-t", NULL, 1 },
    { &Q_HeadsDsc, "-h", NULL, 1 },
    { &Q_SectorsDsc, "-s", NULL, 1 },
    { &Q_LabelDsc, "-l", NULL, 1 },
    { &Q_SerialnumberDsc, "-n", NULL, 1 },
    { &Q_HardssizeDsc, "-S", NULL, 1 },
    { &Q_SoftssizeDsc, "-M", NULL, 1 },
    { &P_DriveDsc, NULL, NULL, 1 },
    { NULL, NULL, NULL, 0 }
};

static void CliParseParams(PARAM_S *pParams)
{
    int	    i = 1;
    char    Value[MAX_VALUE];
    unsigned short  RetLen;
    long    Result;

    RetDsc.dsc$a_pointer = Value;
    RetDsc.dsc$w_length = MAX_VALUE;

    while (pParams->ParamDsc)
    {
	if ((Result = cli$present(pParams->ParamDsc)) & 1)
	{
	    if (pParams->C_subs)
		FakeArgv[FakeArgc++] = pParams->C_subs;

	    if (pParams->Value)
	    {
		if (cli$get_value(pParams->ParamDsc, &RetDsc, &RetLen) & 1)
		{
		    FakeArgv[FakeArgc++] = FakeParamPtr;
		    memcpy(FakeParamPtr, Value, RetLen);
		    FakeParamPtr += RetLen;
		    *FakeParamPtr++ = 0;
		}
	    }
	}
	else
	    if (Result == CLI$_NEGATED)
	    {
		if (pParams->C_subs_NO)
		    FakeArgv[FakeArgc++] = pParams->C_subs_NO;
	    }
	    
	pParams++;
    }
}

static int CliParseCommand()
{
    long    Result;
    char    Value[MAX_VALUE];
    unsigned short  RetLen;

    VAXC$ESTABLISH(MyErrorHandler);

    FakeArgv[0] = "mtools";
    FakeArgc = 1;
    FakeParamPtr = FakeParamBuf;

    RetDsc.dsc$a_pointer = Value;
    RetDsc.dsc$w_length = MAX_VALUE;

    if (cli$present(&Q_ToolsDsc) & 1)
    {
	if (cli$get_value(&Q_ToolsDsc, &RetDsc, &RetLen) & 1)
	{
	    Value[RetLen] = 0;

	    if (!strcmp(Value, "MTOOLS"))
	    {
	        FakeArgv[0] = "mtools";
		FakeArgv[FakeArgc++] = " ";
	    }

	    if (!strcmp(Value, "MINFO"))
	    {
	        FakeArgv[0] = "minfo";
		CliParseParams(MInfoPar);
	    }

	    if (!strcmp(Value, "MDIR"))
	    {
	        FakeArgv[0] = "mdir";
		CliParseParams(MDirPar);
	    }

	    if (!strcmp(Value, "MCD"))
	    {
	        FakeArgv[0] = "mcd";
		CliParseParams(MCdPar);
	    }

	    if (!strcmp(Value, "MDEL"))
	    {
	        FakeArgv[0] = "mdel";
		CliParseParams(MDelPar);
	    }

	    if (!strcmp(Value, "MLABEL"))
	    {
	        FakeArgv[0] = "mlabel";
		CliParseParams(MLabelPar);
	    }

	    if (!strcmp(Value, "MREAD"))
	    {
	        FakeArgv[0] = "mread";
		CliParseParams(MCopyPar);
	    }

	    if (!strcmp(Value, "MWRITE"))
	    {
	        FakeArgv[0] = "mwrite";
		CliParseParams(MCopyPar);
	    }

	    if (!strcmp(Value, "MCOPY"))
	    {
	        FakeArgv[0] = "mcopy";
		CliParseParams(MCopyPar);
	    }

	    if (!strcmp(Value, "MATTRIB"))
	    {
	        FakeArgv[0] = "mattrib";
		CliParseParams(MAttribPar);
	    }

	    if (!strcmp(Value, "MMD"))
	    {
	        FakeArgv[0] = "mmd";
		CliParseParams(MMdPar);
	    }

	    if (!strcmp(Value, "MRD"))
	    {
	        FakeArgv[0] = "mrd";
		CliParseParams(MDelPar);
	    }

	    if (!strcmp(Value, "MMOVE"))
	    {
	        FakeArgv[0] = "mmove";
		CliParseParams(MMovePar);
	    }

	    if (!strcmp(Value, "MREN"))
	    {
	        FakeArgv[0] = "mren";
		CliParseParams(MMovePar);
	    }

	    if (!strcmp(Value, "MBADBLOCKS"))
	    {
	        FakeArgv[0] = "mbadblocks";
		CliParseParams(MBadblocksPar);
	    }

	    if (!strcmp(Value, "MFORMAT"))
	    {
	        FakeArgv[0] = "mformat";
		CliParseParams(MFormatPar);
	    }
	}
	return 1;
    }
    return 0;
}

#endif
#endif

void main(int argc, char **argv)
{
	char *name;
	int i;

//	init_privs();

/*#define PRIV_TEST*/

#ifdef PRIV_TEST
	{ 
		int euid;
		char command[100];
	
		printf("INIT: %d %d\n", getuid(), geteuid());
		drop_privs();
		printf("DROP: %d %d\n", getuid(), geteuid());
		reclaim_privs();
		printf("RECLAIM: %d %d\n", getuid(), geteuid());
		euid = geteuid();
		if(argc & 1) {
			drop_privs();
			printf("DROP: %d %d\n", getuid(), geteuid());
		}
		if(!((argc-1) & 2)) {
			destroy_privs();
			printf("DESTROY: %d %d\n", getuid(), geteuid());
		}
		sprintf(command, "a.out %d", euid);
		system(command);
		exit(1);
	}
#endif

#ifdef VMS
#ifdef USE_CLI
	if (CliParseCommand())
	{
	    for (i=0; i<FakeArgc; i++)
		argv[i] = FakeArgv[i];

	    argc = FakeArgc;
	}
#endif
#endif

	/* print the version */
	if(argc >= 2 && strcmp(argv[1], "-V") == 0) {
		printf("Mtools version %s, dated %s\n", mversion, mdate);
		printf("configured with the following options: ");
#ifdef USE_XDF
		printf("enable-xdf ");
#else
		printf("disable-xdf ");
#endif
#ifdef USING_VOLD
		printf("enable-vold ");
#else
		printf("disable-vold ");
#endif
#ifdef USING_NEW_VOLD
		printf("enable-new-vold ");
#else
		printf("disable-new-vold ");
#endif
#ifdef DEBUG
		printf("enable-debug ");
#else
		printf("disable-debug ");
#endif
#ifdef USE_RAWTERM
		printf("enable-raw-term ");
#else
		printf("disable-raw-term ");
#endif
		printf("\n");
		exit(0);
	}

	/* check whether the compiler lays out structures in a sane way */
	if(sizeof(struct partition) != 16 ||
	   sizeof(struct directory) != 32 ||
	   sizeof(struct vfat_subentry) !=32) {
		fprintf(stderr,"Mtools has not been correctly compiled\n");
		fprintf(stderr,"Recompile it using a more recent compiler\n");
		exit(137);
	}

	if ((name = strrchr(argv[0],'/')))
		name++;
	else
		name = argv[0];
	progname = argv[0];

	/* this allows the different tools to be called as "mtools -c <command>"
	** where <command> is mdir, mdel, mcopy etcetera
	** Mainly done for the BeOS, which doesn't support links yet.
	*/

	if(argc >= 3 && 
	   !strcmp(argv[1], "-c") &&
	   !strcmp(name, "mtools")) {
		argc-=2;
		argv+=2;
		name = argv[0];
	}

	argv[0] = name;
	
	read_config();
	setup_signal();
	for (i = 0; i < NDISPATCH; i++) {
		if (strstr(name, dispatch[i].cmd))
//		if (!strcmp(name,dispatch[i].cmd))
			dispatch[i].fn(argc, argv, dispatch[i].type);
	}
	if (strcmp(name,"mtools"))
		fprintf(stderr,"Unknown mtools command '%s'\n",name);
	fprintf(stderr,"Supported commands:");
	for (i = 0; i < NDISPATCH; i++) {
		if (i%8 == 0) putc('\n', stderr);
		else fprintf(stderr, ", ");
		fprintf(stderr, "%s", dispatch[i].cmd);
	}
	putc('\n', stderr);

	exit(1);
}
