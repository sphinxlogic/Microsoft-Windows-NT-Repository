#include <stdio.h>

#define MAX_INCLUDE 10
int include_cnt;
char include_lines [MAX_INCLUDE][84];

df_mi(register char *line)
{
	char
		buf [84];
	register char
		*c;

	strcpy (buf, line);
	c = strtok (buf, " \r\t\n");
	for ( ; c; c = strtok (0, " \r\t\n") )
		if(!memcmp(c, "-I", 2) && include_cnt < MAX_INCLUDE)
			strcpy(include_lines[include_cnt++], c);
}

dump_includes() {
	FILE 
		*fp;
	register 
		i;

	unlink ("/tmp/cc.inc");
	fp = fopen("/tmp/cc.inc", "w");

	for(i = 0; i < include_cnt; i++)
		fprintf(fp, "%s\n", include_lines[i]);
}


