/* child progrm for fork test */
#include <stdio.h>
#include <string.h>
#ifdef VMS
#include <unixlib.h>
#else
char **environ;
#endif
int main(int argc, char **argv )
{
    int i; char line[512];
    char **envp;
    printf ( "Content-type: text/plain\n\n" );
    printf( "arg count to child is %d\n", argc );
    for ( i = 0; i < argc; i++ ) printf("argv[%d] = '%s'\n", i, argv[i]);
    envp = environ;
    printf("environment pointer: %d\n", envp );
    while ( *envp ) {
	printf("  %s\n", *envp ); envp++;
    }
    return 1;
}
