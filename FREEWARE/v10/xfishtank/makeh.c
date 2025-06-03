
#include <stdio.h>

main(argc, argv)
int	argc;
char	*argv[];
{
	FILE *fp;
	FILE *op;
	int	numfish;
	char	prefix[BUFSIZ];
	char **fishlist;
	int i;

#ifdef vax11c
	fp = fopen("FishList.", "r");
#else
	fp = fopen("FishList", "r");
#endif /* vax11c */
	if (fp == NULL)
	{
		fprintf(stderr, "ERROR: cannot open FishList for read!\n");
		exit(1);
	}
#ifdef vax11c
	if (argc <= 1) {
	   fprintf(stderr, "ERROR: VMS versions requires the output file specified\n");
	   exit(1);
	}
	op = fopen(argv[1], "w");
	if (op == NULL)
	{
		fprintf(stderr, "ERROR: cannot open \"%s\" for write!\n",
			argv[1]);
		exit(1);
	}
#else
	op = stdout;
#endif /* vax11c */
	fscanf(fp, "%d\n", &numfish);

	fprintf(op, "/*\n * Warning, this header file is automatically generated\n */\n");

	fprintf(op, "\n#define NUM_FISH\t%d\n\n", numfish);
	fishlist = (char **)malloc(numfish * sizeof(char *));

	for (i=0; i<numfish; i++)
	{
		fscanf(fp, "%s\n", prefix);
		fishlist[i] = (char *)malloc(strlen(prefix) + 1);
		strcpy(fishlist[i], prefix);
	}
	fclose(fp);

	for (i=0; i<numfish; i++)
	{
		fprintf(op, "#include \"%s.h\"\n", fishlist[i]);
	}
	fprintf(op, "\n");

	fprintf(op, "int rwidth[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_width,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "int rheight[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_height,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "int rcolors[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_colors,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "int rback[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_back,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "int *rreds[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_reds,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "int *rgreens[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_greens,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "int *rblues[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_blues,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "unsigned char *xfishRasterA[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_rasterA,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");

	fprintf(op, "unsigned char *xfishRasterB[] = {\n");
	for (i=0; i<numfish; i++)
	{
		fprintf(op, "\t%s_rasterB,\n", fishlist[i]);
	}
	fprintf(op, "};\n\n");
	exit(0);
}
