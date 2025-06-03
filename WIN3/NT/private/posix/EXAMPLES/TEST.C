#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void a (void);
extern void b (void);

signed int main (signed int argc, char **argv)
{
	char *program_name;
	signed int status;

	if ((program_name = strrchr(*argv, '/')) == NULL)
	{
		program_name = *argv;
	}
	else
	{
		++program_name;
	}
	if (argc != 1)
	{
		(void) fprintf(stderr, "usage: %s\n", program_name);
		status = EXIT_FAILURE;
	}
	else
	{
		(void) printf("Entering \"%s\".\n", program_name);
		a();
		b();
		status = EXIT_SUCCESS;
	}
	return status;
}
