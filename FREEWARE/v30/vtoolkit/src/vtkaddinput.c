/* vtkaddinput.c - VToolkit AddInput routines

*******************************************************************************
* VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
* (c) Steve Klein							      *
* Unpublished rights reserved under the copyright laws of the United States.  *
*******************************************************************************

This module contains routines that turn VMS ASTs into toolkit-compatible
routine callbacks.  It contains two externally visible routines to be
used in place of the ASTADR and ASTPRM arguments to asynchronous VMS
system services.

For a usage example, see btrap.c, the broadcast message trapper sample program.

MODIFICATION HISTORY:

03-Sep-1993 (sjk/nfc) VToolkit V2.

10-Dec-1991 (sjk) Bugfix - initialized flag was never being set.

06-Sep-1990 (sjk) Initial entry.
*/

#include <starlet.h>
#include <lib$routines.h>
#include "vtoolkit.h"
#include "vtkaddinput.h"

#define LIB$_QUEWASEMP	1409772
#define ADD_INPUT_EFN	3

typedef struct {
    unsigned long	queueEntryA[2];		/* must be first in struct */
    char		*mallocP;		/* address actually malloc-ed */
    VtkAddInputProc	*routineP;		/* thread resumption routine */
    Opaque		closure;		/* thread closure */
} AddInputRec;

static _align(quadword) unsigned long addInputQueueHeaderA[2];

static int initialized;

static void AddInputCallback(void)
{
    unsigned long status;
    AddInputRec *addInputRecP;

    sys$clref (ADD_INPUT_EFN);

    while (lib$remqhi (addInputQueueHeaderA, &addInputRecP, 0) != LIB$_QUEWASEMP) {
	(*addInputRecP->routineP) (addInputRecP->closure);
	XtFree (addInputRecP->mallocP);
    }
}

/* Use VtkCompletionAst as the ASTADR parameter on asynchronous system service
 * calls.  This routine must not be called directly from the application.
 * It adds an application callback to the pending callback list. */

void VtkCompletionAst(Opaque closure)
{
    AddInputRec *addInputRecP = (AddInputRec *)closure;

    lib$insqti (addInputRecP, addInputQueueHeaderA, 0);
    sys$setef (ADD_INPUT_EFN);
}

/* Use VtkAllocateAddInputRec as the ASTPRM parameter on asynchronous system
 * service calls.  Arguments to this routine are the application callback
 * routine to be called when the system service completes and the parameter
 * to be passed to that callback.  VtkAllocateAddInputRec allocates and
 * initializes an application callback record to be passed to the
 * VtkCompletionAst routine at AST level when the system service completes. */

Opaque VtkAllocateAddInputRec(VtkAddInputProc *routineP, Opaque closure)
{
    char *mallocP;
    AddInputRec *addInputRecP;

    if (!initialized) {
	XtAddInput (ADD_INPUT_EFN, 0, AddInputCallback, 0);
	initialized = 1;
    }

    mallocP = XtMalloc (sizeof (AddInputRec) + 7);
    addInputRecP = (AddInputRec *)(((int)(mallocP) + 7) & (-8));
    addInputRecP->mallocP  = mallocP;
    addInputRecP->routineP = routineP;
    addInputRecP->closure  = closure;

    return ((Opaque)addInputRecP);
}
