/*
 * Cpus.c - popup an X window showing details of CPU usage
 *          on a set of machines.
 *
 * Terry Jones (terry@santafe.edu)
 * May 4, 1993.
 *
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xaw/Label.h>
#include <Xaw/Form.h>

#define BUF_SZ            2048
#define HEIGHT_MULTIPLIER    4

extern void error();
extern char *strdup();
extern char *set_name();

typedef struct _USER {
    char *name;
    double share;
    Widget label;
    struct _USER *next;
} USER;

typedef struct {
    char *name;
    int nusers;
    USER *users;
    double total_share;
    Widget label;
} MACHINE;

char *myname;
int sysdie = TRUE;
MACHINE *machines;
int nmachines;
Dimension column_width;

int
main(argc, argv)
int argc;
char **argv;
{
    
    void graphics();
    char line[BUF_SZ];
    register int i;
    
    myname = set_name(argv[0]);
    
    if (!fgets(line, BUF_SZ, stdin)){
	error("Could not read first input line.");
    }
    
    nmachines = atoi(line);
    
    if (nmachines <= 0){
	error("You must give a positive number of machines.");
    }

    machines = (MACHINE *) malloc(nmachines * sizeof(MACHINE));

    for (i = 0; i < nmachines; i++){
	machines[i].users = NULL;
	machines[i].nusers = 0;
	machines[i].total_share = 0.0;
    }
    
    for (i = 0; i < nmachines; i++){
	USER *tmp;
	char *machine_name;
	char *user_name;
	double share;
	
	if (!fgets(line, BUF_SZ, stdin)){
	    error("Could not machine input line %d.", i);
	}

	machine_name = strtok(line, " \n");

	if (!machine_name){
	    error("Could not get machine name from machine line %d.", i);
	}

	machines[i].name = strdup(machine_name);

	while (user_name = strtok(NULL, " \n")){
	    char *equals = strchr(user_name, '=');

	    if (!equals){
		error("Could not find = sign in user info for machine '%s'. line is '%s'", machine_name, user_name);
	    }

	    *equals = '\0';

	    share = atof(equals + 1);

	    tmp = machines[i].users;
	    machines[i].users = (USER *) malloc(sizeof(USER));
	    machines[i].users->next = tmp;
	    machines[i].users->name = strdup(user_name);
	    machines[i].users->share = share;
	    machines[i].nusers++;
	    machines[i].total_share += share;
	}

	if (machines[i].total_share > 100.0){
	    double scale = 100.0 / machines[i].total_share;
	    USER *tmp = machines[i].users;

	    machines[i].total_share = 100.0;

	    while (tmp){
		tmp->share *= scale;
		tmp = tmp->next;
	    }
	}
	else {
	    tmp = machines[i].users;
	    machines[i].users = (USER *) malloc(sizeof(USER));
	    machines[i].users->next = tmp;
	    machines[i].users->name = strdup("idle");
	    machines[i].users->share = 100.0 - machines[i].total_share;
	    machines[i].nusers++;
	    machines[i].total_share = 100.0;
	}
    }

    /* Debugging - if you need it.
    for (i = 0; i < nmachines; i++){

	USER *tmp = machines[i].users;
	
	printf("MACHINE NAME: %s\n", machines[i].name);
	printf("\tTotal Users: %d\n", machines[i].nusers);
	printf("\tTotal Share: %f\n", machines[i].total_share);

	while (tmp){
	    printf("\t\tUser: %s Share %f\n", tmp->name, tmp->share);
	    tmp = tmp->next;
	}
    }
    */
    

    graphics(argc, argv);
}

void
error(u, v, w, x, y, z)
char *u, *v, *w, *x, *y, *z;
{
    extern void perror();
    extern int fflush();

    fprintf(stderr, "%s: ", myname);
    fprintf(stderr, u, v, w, x, y, z);
    fprintf(stderr, "\n");
    if (fflush(stderr) == EOF){
        perror("fflush");
    }
    if (sysdie == TRUE){
	exit(1);
    }
    return;
}

char *
set_name(s)
char *s;
{
    extern char *strrchr();
    char *slash = strrchr(s, '/');
    
    return slash ? slash + 1 : s;
}

#if defined(ultrix) || defined(NeXT)
char *strdup(s)
char *s;
{
    return strcpy(malloc(strlen(s) + 1), s);
}
#endif

void
graphics(argc, argv)
int argc;
char **argv;
{
    XtAppContext context;
    Widget toplevel;
    Widget form;
    Arg args[10];
    register int i;
    
    void create_machine_labels();
    void create_user_boxes();
    void set_widths();
    
    toplevel = XtAppInitialize(&context, "Cpus", (XrmOptionDescList) NULL, 0, &argc, argv, NULL, NULL, 0);

    form = XtCreateManagedWidget("form", formWidgetClass, toplevel, NULL, 0);

    create_machine_labels(form);
    create_user_boxes(form);
    set_widths();

    XtRealizeWidget(toplevel);
    XtAppMainLoop(context);
}

void 
create_machine_labels(parent)
Widget parent;
{
    int i;
    Dimension width;

    column_width = 0;
    
    for (i = 0; i < nmachines; i++) {
	machines[i].label = XtCreateManagedWidget(machines[i].name, labelWidgetClass, parent, NULL, 0);

	if (i){
	    XtVaSetValues(machines[i].label, "fromHoriz", machines[i - 1].label, NULL);
	}
	
	XtVaGetValues(machines[i].label, XtNwidth, &width, NULL);
	    
	if (column_width < width){
	    column_width = width;
	}
    }

    return;
}

void 
create_user_boxes(parent)
Widget parent;
{
    int i;
    Dimension height;
    Dimension width;
    
    for (i = 0; i < nmachines; i++) {

	USER *tmp = machines[i].users;
	Widget prev;
	int first = 1;

	while (tmp){

	    height = HEIGHT_MULTIPLIER * (Dimension)tmp->share;
	    
	    tmp->label = XtCreateManagedWidget(tmp->name, labelWidgetClass, parent, NULL, 0);
       
	    
	    if (first){
		XtVaSetValues(tmp->label, "fromVert", machines[i].label, NULL);
		first = 0;
	    }
	    else {
		XtVaSetValues(tmp->label, "fromVert", prev, NULL);
	    }
	    
	    if (i){
		XtVaSetValues(tmp->label, "fromHoriz", machines[i - 1].label, NULL);
	    }

	    XtVaGetValues(tmp->label, XtNwidth, &width, NULL);
	    
	    if (column_width < width){
		column_width = width;
	    }
	    
	    XtVaSetValues(tmp->label, XtNheight, height, NULL);

	    prev = tmp->label;
	    tmp = tmp->next;
	}
    }
    
    return;
}

void
set_widths()
{
    register int i;
    
    for (i = 0; i < nmachines; i++) {

	USER *tmp = machines[i].users;

	XtVaSetValues(machines[i].label, XtNwidth, column_width, NULL);

	while (tmp){
	    XtVaSetValues(tmp->label, XtNwidth, column_width, NULL);
	    tmp = tmp->next;
	}
    }

    return;
}
