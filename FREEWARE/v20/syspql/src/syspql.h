static const unsigned short VERS_MAJ=2;
static const unsigned short VERS_MIN=0;
#define PARAM_SIZE 32

#pragma member_alignment save
#pragma nomember_alignment

typedef unsigned int u32;
typedef unsigned short u16;
typedef void * p32;

typedef struct {u32 low, high;} vmsvers;

typedef struct {unsigned char typ; u32 val;} pqlent; 
typedef struct {u16 versmin, versmaj;
		u32 map;		/* 1 bit per specified parameter */
		u32 mapdef;		/* 1 bit per system default parameter */
		u32 map0inf;		/* 1 bit for each parameter for */
					   /* which 0 means infinity */
		vmsvers	sysver;		/* vms version of the defvalues */
		u32 nb_param; 		/* size of parameter list */
		u32 values[PARAM_SIZE];	/* new values */
		u32 defvalues[PARAM_SIZE];/* default system values */
					  /* copied at boot time */
		} parambuf;
#pragma member_alignment restore

static int
okassumes()
{
	return (sizeof (u32)==4) && (sizeof (u16)==4) && (sizeof (p32) == 4);
}
