#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <sys/param.h>
#include <AxeEditor.h>
#include <AxeiiText.h>

extern char *getenv();

#include "Language.h"

static void InterpFocus(), Execute();

static XtActionsRec actions[] = {
    "interp-focus", InterpFocus,
    "execute",      Execute,
};

/*ARGSUSED*/
static void
InterpFocus(widget, event, params, num_params)
     Widget widget;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
    if (!XtIsSubclass(widget, axeEditorWidgetClass))
    {
	return;
    }

    interpreter.SetBuffer(AxeEditorEdWidget(widget));
}

/*ARGSUSED*/
static void
Execute(widget, event, params, num_params)
     Widget widget;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
    Tcl_CmdBuf assembley;
    Cardinal param;
    char *cmd;

    if (*num_params == 0)
    {
        return;
    }

    assembley = Tcl_CreateCmdBuf();
    for (param = 0;  param < *num_params;  ++param)
    {
        cmd = Tcl_AssembleCmd(assembley, params[param]);
        cmd = Tcl_AssembleCmd(assembley, " ");
    }
    cmd = Tcl_AssembleCmd(assembley, "");
    
    (void) Tcl_Eval(interpreter.interp, cmd, 0, NULL);

    Tcl_DeleteCmdBuf(assembley);
}

/*ARGSUSED*/
static int 
Action(clientData, interp, argc, argv)
    ClientData clientData;
    int argc;
    char *argv[];
{
    Window root, child;
    unsigned int mask;
    XEvent event;

    if (argc < 2)
    {
        return TCL_ERROR;
    }

    XQueryPointer(XtDisplay(interpreter.buffer), XtWindow(interpreter.buffer),
		  &root, &child,
                  &event.xbutton.x_root, &event.xbutton.y_root,
                  &event.xbutton.x, &event.xbutton.y, &mask);
    event.type = ButtonPress;

    XtCallActionProc(interpreter.buffer, argv[1], &event, &argv[2], argc - 2);

    return TCL_OK;
}

/*ARGSUSED*/
static int 
GetPos(clientData, interp, argc, argv)
    ClientData clientData;
    int argc;
    char *argv[];
{
    char result[32];

    Tcl_ResetResult(interpreter.interp);

    sprintf(result, "%d", XawTextGetInsertionPoint(interpreter.buffer));

    Tcl_SetResult(interpreter.interp, result, TCL_VOLATILE);

    return TCL_OK;
}

/*ARGSUSED*/
static int 
SetPos(clientData, interp, argc, argv)
    ClientData clientData;
    int argc;
    char *argv[];
{
    if (argc < 2)
    {
	return TCL_ERROR;
    }

    XawTextSetInsertionPoint(interpreter.buffer,
			                      (XawTextPosition) atoi(argv[1]));
    return TCL_OK;
}

/*ARGSUSED*/
static int 
GetSelection(clientData, interp, argc, argv)
    ClientData clientData;
    int argc;
    char *argv[];
{
    char result[64];
    XawTextPosition start, finish, eol;
    int lines;

    XawTextGetSelectionPos(interpreter.buffer, &start, &finish);
    for (lines = 0, eol = start;  eol < finish;  ++lines)
    {
	eol = XawTextSourceScan(XawTextGetSource(interpreter.buffer),
				           eol, XawstEOL, XawsdRight, 1, True);
    }

    Tcl_ResetResult(interpreter.interp);

    sprintf(result, "%d %d %d", start, finish, lines);

    Tcl_SetResult(interpreter.interp, result, TCL_VOLATILE);

    return TCL_OK;
}

static void 
SetBuffer(buffer)
     Widget buffer;
{
    interpreter.buffer = buffer;
}

static XtResource resources[] = {
    { "axeLibDir", "AxeLibDir", XtRString, sizeof(String),
      (Cardinal) 0, XtRImmediate, (XtPointer) 0
    }
};

static void
Initialize(widget)
     Widget widget;
{
    XtAppContext app = XtWidgetToApplicationContext(widget);
    char tclFile[MAXPATHLEN], *home, *tcl;

    Tcl_Interp *interp = interpreter.interp = Tcl_CreateInterp();

    Tcl_CreateCommand(interp, "action", Action, (ClientData) 0,
                                                   (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "getSelection", GetSelection, (ClientData) 0,
                                                   (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "getPos", GetPos, (ClientData) 0,
                                                   (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "setPos", SetPos, (ClientData) 0,
                                                   (Tcl_CmdDeleteProc *) NULL);

    XtVaGetApplicationResources(widget, (XtPointer) &tcl,
                                resources, XtNumber(resources),
                                NULL);
    strcpy(tclFile, tcl);
    strcat(tclFile, "/axe.tcl");
    Tcl_EvalFile(interp, tclFile);

    home = getenv("HOME");
    strcpy(tclFile, home);
    strcat(tclFile, "/axe.tcl");
    Tcl_EvalFile(interp, tclFile);

    XtAppAddActions(app, actions, XtNumber(actions));
}

InterpRec interpreter = {
    Initialize,
    (Tcl_Interp *) 0,
    SetBuffer,
    (Widget) 0,
};
