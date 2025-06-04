#include <pqldef.h>
#include <stdio.h>

typedef struct {unsigned char typ; unsigned long val;} pqlent; 
extern pqlent * PQL$AR_SYSPQL;
extern unsigned long PQL$GL_SYSPQLLEN;

main()
{
	pqlent * pe;
	int sz;

	printf ("System PQL length is %d\n", PQL$GL_SYSPQLLEN);
	for (pe= PQL$AR_SYSPQL, sz=PQL$GL_SYSPQLLEN; sz>0; 
			pe++, sz-= sizeof (pqlent)) {
		char unkcode[20], *cp;

		switch (pe->typ) {
		case PQL$_ASTLM:	cp= "ASTLM"; break;
		case PQL$_BIOLM:	cp= "BIOLM"; break;
		case PQL$_BYTLM:	cp= "BYTLM"; break;
		case PQL$_CPULM:	cp= "CPULIM"; break;
		case PQL$_DIOLM:	cp= "DIOLM"; break;
		case PQL$_FILLM:	cp= "FILLM"; break;
		case PQL$_PGFLQUOTA:	cp= "PGFLQUOTA"; break;
		case PQL$_PRCLM:	cp= "PRCLM"; break;
		case PQL$_TQELM:	cp= "TQELM"; break;
		case PQL$_WSQUOTA:	cp= "WSQUOTA"; break;
		case PQL$_WSDEFAULT:	cp= "WSDEFAULT"; break;
		case PQL$_ENQLM:	cp= "ENQLM"; break;
		case PQL$_WSEXTENT:	cp= "WSEXTENT"; break;
		case PQL$_JTQUOTA:	cp= "JTQUOTA"; break;
		default:	sprintf (cp=unkcode, "Code: %d", pe->typ);
		}
		printf ("Code: %s: value: %u\n", cp, pe->val);
	}
	
}
