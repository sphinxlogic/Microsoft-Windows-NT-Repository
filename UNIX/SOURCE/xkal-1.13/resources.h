/*
 *	resources.h: Resource loading code and definitions.
 *
 *	Most other modules should include app-resources.h for access to
 *	the resources after they've been loaded.
 *
 *	George Ferguson, ferguson@cs.rochester.edu, 27 Feb 1991.
 *
 *	$Id: resources.h,v 2.2 91/03/13 13:31:35 ferguson Exp $
 *
 */
#ifndef RESOURCES_H
#define RESOURCES_H

extern XrmOptionDescRec xkalOptions[15];	/* cmd line options */
extern XtResource xkalResources[53];		/* applications resources */

extern void getResources();		/* load resources without display */

#endif /* RESOURCES_H */
