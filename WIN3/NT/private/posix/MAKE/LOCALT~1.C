#include <time.h>

/*
	referenced C language routine not yet finished by microsoft
*/
struct tm *localtime(arg) {
	static struct tm 
		t;

	return &t;
}
