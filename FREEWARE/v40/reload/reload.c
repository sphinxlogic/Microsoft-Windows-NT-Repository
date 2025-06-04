/*
 * Reload Xdefaults onto root window
 *
 * Dave Porter  7-Mar-1989
 * -----------------------
 *
 * This program is based on the observation that the DECwindows
 * session manager stores the contents of the Xdefaults file, merged
 * with some more resource definitions from session mananger data files,  
 * as a string-valued property on the root window.
 *
 * This program fetches the old value of the property and the new 
 * contents of Xdefaults, merges the two together, and rewrites
 * the property value.
 *
 * Restrictions:
 *
 * Resource values can be changed, or new resources added.  They
 * cannot be deleted by this program (because the new Xdefaults is
 * merged into the previous stored list of resources).
 *
 * The total length of the resource list is limited by the constant
 * defined as MAXLEN (as some sort of sanity check).  Recompile from
 * source if you have a huge Xdefaults database.
 *
 * Bugs:
 *
 * When the session manager stores the data on the root window, it
 * doesn't include and resources that are explicitly for it (resources
 * with names starting with "sm." or "sm*", as far as I can tell).
 * This program doesn't bother with such trifles, so some extraneous
 * session manager resource definitions end up being stored.  This appears
 * to be harmless.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <decw$include/Xlib.h>
#include <decw$include/Xutil.h>
#include <decw$include/Xatom.h>
#include <decw$include/Xresource.h>

#define XDEFAULTS "sys$login:decw$xdefaults.dat"
#define MAXLEN 4096L	/* 4K chars */

main()
{
    Display *dpy;
    Screen *scr;
    Window root;
    Atom ptype;
    int pform;
    unsigned long nitems, nafter;
    unsigned char *prop;
    XrmDatabase old, new;
    char *temp, *ptr;
    FILE *file;
    int len;

    dpy = XOpenDisplay(0);
    if (dpy==0) {
	printf("can't open display\n");
	exit(-1);
	}

    scr = XDefaultScreenOfDisplay(dpy);
    root = XRootWindowOfScreen(scr);

    XrmInitialize();

    /*
     * get current database, stored as a string-valued
     * property on the root window
     */
    XGetWindowProperty(
		      dpy, root, 
		      XA_RESOURCE_MANAGER,
		      0L, MAXLEN/4, 0, AnyPropertyType,
		      &ptype, &pform, &nitems, &nafter, &prop);
    if (ptype==None) {
	printf("property not found on root\n");
	exit(-1);
	}
    if (ptype!=XA_STRING || pform!=8) {
	printf("unexpected property data read\n");
	exit(-1);
	}
    if (nafter) {
	printf("property too big (%ld bytes)\n", nitems+nafter);
	exit(-1);
	}
    printf("old: %ld bytes\n", nitems);

    /*
     * convert into resource manager database
     *
     */
    old = XrmGetStringDatabase(prop);
    if (old==NULL) {
	printf("error reconverting old property value\n");
	exit(-1);
	}

    /*
     * be tidy and free up the Xlib memory used for the
     * property string
     */
     Xfree(prop);

    /*
     * read in new file and convert to database
     * format
     */
    new = XrmGetFileDatabase(XDEFAULTS);
    if (new==NULL) {
	printf("can't open %s\n", XDEFAULTS);
	exit(-1);
	}

    /*
     * merge new database into the old.  the 'new' resources
     * overwrite the old, and 'new' is itself destroyed.
     */
    XrmMergeDatabases(new, &old);

    /*
     * now we want to convert the merged database back to
     * a string.  there doesn't seem to be any way to do this,
     * so we resort to writing it out to a file and then
     * reading the file in.  yecch.
     */
    temp = tmpnam(NULL);
    XrmPutFileDatabase(old, temp);
    file = fopen(temp,"r");
    if (file==NULL) {
	printf("can't open %s\n", temp);
	exit(-1);
	}
    prop = malloc(MAXLEN);
    if (prop==0) {
	printf("malloc failed\n");
	exit(-1);
	}
    ptr = prop;
    len = MAXLEN;
    nitems = 0;
    while (len>0 && fgets(ptr, len, file)!=NULL) {
	int n = strlen(ptr);
	ptr += n;
	len -= n;
	nitems += n;
	}
    if(ferror(file))
	{
	printf("error %d reading %s\n", ferror(file), temp);
	exit(-1);
	}
    if(!feof(file))
	{
	printf("internal storage exceeded (%ld bytes)\n", MAXLEN);
	exit(-1);
	}
    fclose(file);
    remove(temp);

    /*
     * Ok, now we can rewrite the root property
     */    
    printf("new: %ld bytes\n", nitems);
    XChangeProperty(
		   dpy, root, XA_RESOURCE_MANAGER, ptype, pform,
		   PropModeReplace, prop, nitems
		   );
    XSync(dpy, 0);

}
