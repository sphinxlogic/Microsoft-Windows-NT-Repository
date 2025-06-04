
/*
 *			    XGetHost
 *
 *  Get the list of hosts enabled on the server
 */

#include    <stdio.h>
#include    "X.h"
#include    "Xlib.h"

main(
    int	    argc,
    char    *argv[]
    )
{

    char	    line[132];
    int		    i, j,
		    numHosts;
    Display	    *dsp;
    XHostAddress    *hostList;
    Bool	    onOrOff;

    if ( (dsp = XOpenDisplay( (argc>1?argv[1]:(char *) NULL) ) ) == (Display *) NULL ) {
	printf( "[Can't open display]\n" );
	exit();
    }
    hostList = XListHosts( dsp, &numHosts, &onOrOff );
    if ( onOrOff == True ) printf( "[Access checks enabled]\n" );
    else printf( "[Access checks disabled]\n" );
    if ( numHosts > 0 ) {
	for ( i=0; i<numHosts; i++ ) {
	    switch ( hostList[i].family) {

case FamilyInternet:
		if ( hostList[i].length != 4 ) printf( "[Protocol Internet, Address length incorrect]\n" );
		else printf( "[Protocol Internet, Address: <%d.%d.%d.%d>]\n",
		    *(unsigned char *) hostList[i].address,
		    *(unsigned char *) hostList[i].address+1,
		    *(unsigned char *) hostList[i].address+2,
		    *(unsigned char *) hostList[i].address+3 );
		break;

case FamilyDECnet:
		if ( hostList[i].length != 2 ) printf( "[Protocol DECnet, Address length incorrect]\n" );
		else printf( "[Protocol DECnet, Address: <%d.%d>]\n",
		    *(unsigned char *) hostList[i].address,
		    *(unsigned char *) hostList[i].address+1 );
		break;

case FamilyChaos:
		printf( "[Protocol Chaos, Address: <" );
		for (j=0; j<hostList[i].length; j++) printf( " %d", *(unsigned char *) hostList[i].address+j );
		printf( ">]\n" );
		break;

case FamilyGeneric:
		printf( "[Protocol Sneaky DECprivate, Address: <" );
		for (j=0; j<hostList[i].length; j++) printf( "%c", hostList[i].address[j] );
		printf( ">]\n" );
		break;

default:	printf( "[Protocol Unknown, Address: <" );
		for (j=0; j<hostList[i].length; j++) printf( " %d", *(unsigned char *) hostList[i].address+j );
		printf( ">]\n" );
		break;

	    }
	}
	XFree( hostList );
    }
    else {
	printf( "[Empty host list returned by server]\n" );
    }
    XCloseDisplay( dsp );
    exit();
}
