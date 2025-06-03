#include <stdio.h>

int main() {
    int status;
    status = printf("content-type: text/html\n\n");
    while ( status>0 ) printf(
	"I am a misbehaving CGI program, I never stop my output\n" );
    return 1;
}
