/* child progrm for fork test, include display of stat. */
#include <stdio.h>
#include <string.h>
#ifdef VMS
#include <unixlib.h>
#include <stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
char **environ;
#endif
int main(int argc, char **argv )
{
    int i; char line[512];
    char **envp;
    struct stat statblk;
    printf ( "Content-type: text/plain\n\n" );
    printf( "arg count to child is %d\n", argc );
    for ( i = 0; i < argc; i++ ) printf("argv[%d] = '%s'\n", i, argv[i]);
    envp = environ;
    printf("environment pointer: %d\n", envp );
    while ( *envp ) {
	printf("  %s\n", *envp ); envp++;
    }
    printf("\nOpen file descriptors:\n");
    for ( i = 0; i < 20; i++ ) if ( 0 == fstat ( i, &statblk ) ) {
	printf("   %2d: mode: %9o, owner: %d, size: %d\n", i, statblk.st_mode,
		statblk.st_uid, statblk.st_size );
    }
    return 1;
}
