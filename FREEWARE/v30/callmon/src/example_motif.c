/*  CALLMON Examples
 *
 *  File:     EXAMPLE_MOTIF.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: DECwindows/Motif program using SVN widgets. Adapted from
 *            DECwindows examples SvnMSample.c and SvnMSampleSource.c.
 *            It is linked with file EXAMPLE_MOTIF_VM.C were all kinds
 *            of memory allocations are performed.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/PushB.h>
#include <DXm/DXmSvn.h>

extern void init_call_monitor (int argc, char** argv);

/* Description of pixmaps used in SVN */

#define pixmap_width 13
#define pixmap_height 13

static char parent_pixmap_bits [] = {
   0x40, 0x00, 0xe0, 0x00, 0xf0, 0x01, 0x08, 0x02, 0xf4, 0x05, 0x16, 0x0d,
   0x57, 0x1d, 0x16, 0x0d, 0xf4, 0x05, 0x08, 0x02, 0xf0, 0x01, 0xe0, 0x00,
   0x40, 0x00};

static char child_pixmap_bits [] = {
   0x40, 0x00, 0xa0, 0x00, 0x10, 0x01, 0x08, 0x02, 0x04, 0x04, 0x02, 0x08,
   0x41, 0x10, 0x02, 0x08, 0x04, 0x04, 0x08, 0x02, 0x10, 0x01, 0xa0, 0x00,
   0x40, 0x00};


/* Hierarchy storage structure */

typedef struct node {
    int level;             /* level number of children    */
    int number;            /* number of children          */
    XmString text;         /* entry text                  */
    struct node *sibling;  /* pointer to sibling or NULL  */
    struct node *children; /* pointer to children or NULL */
    Boolean opened;        /* children are showing        */
} NodeData, *NodePtr;


/* Description of text hierarchy */

typedef struct {
    int level;           /* level number of entry */
    String text;         /* entry text */
} NodeText, *NodeTextPtr;

NodeText text_list [] = {
    {0, "OSF/Motif Style Guide V1.1"},
    {1, "User Interface Design Principles"},
    {2, "Adopt the User's Perspective"},
    {2, "Give the User Control"},
    {3, "Keep Interfaces Flexible"},
    {3, "Use Progressive Disclosure"},
    {2, "User Real-World Metaphors"},
    {3, "Allow Direct Manipulation"},
    {3, "Provide Rapid Response"},
    {3, "Provide Output as Input"},
    {2, "Keep Interfaces Natural"},
    {3, "Make Navigation Easy"},
    {3, "Provide Natural Shades and Colors"},
    {2, "Keep Interfaces Consistent"},
    {2, "Communicate Application Actions to the User"},
    {3, "Give the User Feedback"},
    {3, "Anticipate Errors"},
    {3, "Use Explicit Destruction"},
    {2, "Avoid Common Design Pitfalls"},
    {1, "Input and Navigation Models"},
    {2, "The Keyboard Focus Model"},
    {3, "Implicit Focus"},
    {3, "Explicit Focus"},
    {2, "The Input Device Model"},
    {3, "Pointing Devices"},
    {3, "Pointer Shapes"},
    {3, "Warp Pointer Only If Explicitly Enabled"},
    {2, "The Navigation Model"},
    {3, "Mouse-Based Navigation"},
    {3, "Keyboard-Based Navigation"},
    {3, "Menu Traversal"},
    {3, "Scrollable Component Navigation"},
    {1, "Selection and Component Activation"},
    {2, "Selection Models"},
    {3, "Mouse-Based Single Selection"},
    {3, "Mouse-Based Browse Selection"},
    {3, "Mouse-Based Multiple Selection"},
    {3, "Mouse-Based Range Selection"},
    {3, "Mouse-Based Discontiguous Selection"},
    {3, "Keyboard Selection"},
    {3, "Canceling a Selection"},
    {3, "Selecting and Deselecting All Elements"},
    {3, "Using Mnemonics for Elements"},
    {2, "Selection Actions"},
    {3, "the Drag-and-Drop Model"},
    {3, "Using Primary Selection"},
    {3, "Using Quick Transfer"},
    {3, "Using Keyboard Clipboard Selection Actions and Deletion"},
    {2, "Component Activation"},
    {3, "Basic Activation"},
    {3, "Accelerators"},
    {3, "Mnemonics"},
    {3, "Help Activation"},
    {3, "Default Activation"},
    {3, "Expert Activation"},
    {3, "Previewing and Autorepeat"},
    {1, "Application Design Principles"},
    {2, "Choosing Components"},
    {3, "Guidelines for Choosing a Main Component Group"},
    {3, "Guidelines for Choosing Interactive Methods"},
    {2, "Layout"},
    {3, "Common Client Areas"},
    {3, "Grouping Components"},
    {3, "Menu Design"},
    {3, "DialogBox Design"},
    {2, "Interaction"},
    {3, "Supplying Indications of Actions"},
    {3, "Providing Feedback"},
    {3, "Allowing User Flexibility"},
    {2, "Component Design"},
    {1, "Window Manager Design Principles"},
    {2, "Configurability"},
    {2, "Window Support"},
    {3, "Primary Window"},
    {3, "Secondary Windows (Dialog)"},
    {3, "Menu Windows"},
    {2, "Window Decorations"},
    {3, "Client Area"},
    {3, "Title Area"},
    {3, "Maximize Button"},
    {3, "Minimize Button"},
    {3, "Other Buttons"},
    {3, "Resize Buttons"},
    {3, "Window Menu"},
    {2, "Window Navigation"},
    {2, "Icons"},
    {3, "Icon Decoration"},
    {3, "Icon Menu"},
    {3, "Icon Box"},
    {1, "Designing for International Markets"},
    {2, "Collating Sequences"},
    {2, "Country-Specific Data Formats"},
    {3, "Thousands Separators"},
    {3, "Decimal Separators"},
    {3, "Grouping Separators"},
    {3, "Positive and Negative Values"},
    {3, "Currency"},
    {3, "Date Formats"},
    {3, "Time Formats"},
    {3, "Telephone Numbers"},
    {3, "Proper Names and Addresses"},
    {2, "Icons, Symbols, and Pointer Shapes"},
    {2, "Scanning Direction"},
    {2, "Designing Modularized Software"},
    {2, "Translation Screen Text"},
    {1, "Controls, Groups, and Models Reference Pages"},
    {0, NULL},
};


/* Routine that creates a node tree */

NodePtr BuildTree (NodeTextPtr *text, String prefix, int *child_count)
{
    NodePtr node;
    char *line, buffer [256], new_prefix [40];

    node = XtNew (NodeData);
    *child_count += 1;
    node->level = (*text)->level + 1;
    node->number = 0;
    node->children = node->sibling = NULL;
    node->opened = False;

    if ((*text)->level > 0) {
        sprintf (new_prefix, "%s%s%d", prefix, *prefix != '\0' ? "." : "",
            *child_count);
        sprintf (line = buffer, "%s %s", new_prefix, (*text)->text);
    }
    else {
        new_prefix [0] = '\0';
        line = (*text)->text;
    }
    node->text = XmStringCreateSimple (line);

    if ((++*text)->text != NULL && (*text)->level >= node->level)
        node->children = BuildTree (text, new_prefix, &node->number);
    if ((*text)->text != NULL && (*text)->level == node->level - 1)
        node->sibling = BuildTree (text, prefix, child_count);

    return node;
}


/* AttachToSource Callback */

void Attach (Widget svn, XtPointer tag, XtPointer user_data)
{
    XtPointer root_tag;
    DXmSvnCallbackStruct* data = user_data;
    NodeTextPtr node = text_list;
    int topnum = 0;

    printf ("AttachToSource Callback\n");

    /* Build the data tree */
    root_tag = BuildTree (&node, "", &topnum);

    /* Add the top level entry */
    DXmSvnAddEntries (svn, 0, 1, 0, &root_tag, True);
}


/* GetEntry Callback */

void GetEntry (Widget svn, XtPointer tag, XtPointer user_data)
{
    DXmSvnCallbackStruct* data = user_data;
    NodePtr node = (NodePtr) data->entry_tag;
    Pixel background_pixel, foreground_pixel;
    Display *display;
    Screen *screen;
    Widget shell;

    static Pixmap parent_pixmap = None;
    static Pixmap child_pixmap = None;
    static XmFontList fontlist;

    printf ("GetEntry Callback: entry = %d, level = %d, cursor entry = %d\n", 
        data->entry_number, data->entry_level, data->loc_cursor_entry_number);

    /* Set up the pixmaps the first time */
    if (parent_pixmap == None) {

        /* Get background and foreground pixel */
        XtVaGetValues (svn,
            XmNforeground, (XtArgVal)&foreground_pixel,
            XmNbackground, (XtArgVal)&background_pixel,
            NULL);

        /* Create the pixmaps */
        screen = XtScreen (svn);
        display = DisplayOfScreen (screen);
        parent_pixmap = XCreatePixmapFromBitmapData (display,
            XDefaultRootWindow(display), parent_pixmap_bits,
            pixmap_width, pixmap_height, foreground_pixel, background_pixel,
            DefaultDepthOfScreen (screen));
        child_pixmap = XCreatePixmapFromBitmapData (display,
            XDefaultRootWindow(display), child_pixmap_bits,
            pixmap_width, pixmap_height, foreground_pixel, background_pixel,
            DefaultDepthOfScreen (screen));

        /* Get the shell widget of the SVN */
        for (shell = svn; !XtIsShell (shell); shell = XtParent (shell));

        /* Get the font-list for the SVN texts */
        XtVaGetValues (shell, XmNdefaultFontList, (XtArgVal)&fontlist, NULL);
    }

    /* Set the entry information */
    DXmSvnSetEntryNumComponents (svn, data->entry_number, 2);

    /* Parent nodes are put in the index window */
    if (node->number != 0)
       DXmSvnSetEntryIndexWindow (svn, data->entry_number, True);

    /* The first component is different in parent/child nodes */
    DXmSvnSetComponentPixmap (svn, data->entry_number, 1, 0, 0,
        (node->number == 0) ? child_pixmap : parent_pixmap,
        pixmap_width, pixmap_height);

    /* The second component is the same in parent/child nodes */
    DXmSvnSetComponentText (svn, data->entry_number, 2, pixmap_width + 4, 0,
        node->text, fontlist);
}


/* Routine that opens a node, given the node number. */
/* If all levels required, recursively opens all children and return the */
/* number of last entry in the subtree. */
/* Otherwise, returns the opened entry number. */

int OpenNode (Widget svn, int entry, NodePtr node, Boolean all_levels)
{
    NodePtr child;
    XtPointer* tags;
    XtPointer* t;

    /* If node has no children, then return. */
    if (node->number == 0)
        return entry;

    /* If not already opened, open it now */
    if (!node->opened) {
        node->opened = True;

        /* Build an array of children's tags */
        t = tags = (XtPointer*) XtMalloc (node->number * sizeof (XtPointer));
        for (child = node->children; child != NULL; child = child->sibling)
            *t++ = child;

        /* Add the entries. */
        DXmSvnAddEntries (svn, entry, node->number, node->level, tags, False);
        XtFree ((char*) tags);
    }

    /* If all levels required, open the children */
    if (all_levels)
        for (child = node->children; child != NULL; child = child->sibling)
            entry = OpenNode (svn, entry + 1, child, True);

    return entry;
}


/* Recursively close a node and its descendants */

void CloseNode (Widget svn, int entry, NodePtr node)
{
    NodePtr child;
    int child_entry = entry;

    /* If the current node is not opened, then return */
    if (!node->opened)
       return;

    /* Call CloseNode on each child */
    for (child = node->children; child != NULL; child = child->sibling)
        CloseNode (svn, ++child_entry, child);

    /* Tell SVN to remove its children */
    DXmSvnDeleteEntries (svn, entry, node->number);

    /* Mark the node closed */
    node->opened = False;
}


/* SelectAndConfirm Callback */

void SelectAndConfirm (Widget svn, XtPointer tag, XtPointer user_data)
{
    DXmSvnCallbackStruct* data = user_data;
    NodePtr node = (NodePtr) data->entry_tag;

    printf ("SelectAndConfirm Callback: %s = %d, %s = %d, %s = %d, %s = %d\n",
        "entry", data->entry_number, "component", data->component_number,
        "level", data->entry_level, "cursor entry",
        data->loc_cursor_entry_number);

    if (node->opened)
        CloseNode (svn, data->entry_number, node);
    else
        OpenNode (svn, data->entry_number, node, False);

    if (node->number > 0)
        DXmSvnClearSelection (svn, data->entry_number);
}


/* ExtendConfirm Callback */

void ExtendConfirm (Widget svn, XtPointer tag, XtPointer user_data)
{
    DXmSvnCallbackStruct* data = user_data;
    NodePtr node;
    int numsel, n, *entries;
    XtPointer* tags;

    printf ("ExtendConfirm Callback: entry = %d, cursor entry = %d\n",
        data->entry_number, data->loc_cursor_entry_number);

    /* If no entry selected, then return */
    if ((numsel = DXmSvnGetNumSelections (svn)) == 0)
        return;

    /* Get the list of all selected entries */
    entries = (int*) XtMalloc (numsel * sizeof (int));
    tags = (XtPointer*) XtMalloc (numsel * sizeof (XtPointer));
    DXmSvnGetSelections (svn, entries, NULL, tags, numsel);

    /* Toggle state of all selected entries */
    for (n = numsel - 1; n >= 0; n--) {
        node = tags [n];
        if (node->opened)
            CloseNode (svn, entries [n], node);
        else
            OpenNode (svn, entries [n], node, False);
        if (node->number > 0)
            DXmSvnClearSelection (svn, entries [n]);
    }

    XtFree ((char*) entries);
    XtFree ((char*) tags);
}


/* Quit push button callback */

void Quit (Widget w, XtPointer tag, XtPointer user_data)
{
    Widget shell = tag;
    printf ("Quit\n");
    XtDestroyWidget (shell);
    exit (1);
}


/* Expand All push button callback */

void ExpandAll (Widget w, XtPointer tag, XtPointer user_data)
{
    Widget svn = tag;
    DXmSvnCallbackStruct* data = user_data;
    int entry, entry_number;
    NodePtr node;

    printf ("ExpandAll\n");

    DXmSvnDisableDisplay (svn);
    XtVaGetValues (svn, DXmSvnNnumberOfEntries, (XtArgVal)&entry_number, NULL);
    for (entry = entry_number; entry > 0; entry--) {
        node = (NodePtr) DXmSvnGetEntryTag (svn, entry);
        if (node->number > 0)
            OpenNode (svn, entry, node, True);
    }
    DXmSvnEnableDisplay (svn);
}


/* View Textual / View Tree push button callback */

void ToggleView (Widget w, XtPointer tag, XtPointer user_data)
{
    Widget svn = tag;
    DXmSvnCallbackStruct* data = user_data;
    short mode;
    String label;
    XmString cs;

    printf ("ToggleView\n");

    DXmSvnDisableDisplay (svn);

    XtVaGetValues (svn, DXmSvnNdisplayMode, (XtArgVal)&mode, NULL);
    if (mode == DXmSvnKdisplayOutline) {
        mode = DXmSvnKdisplayTree;
        label = "View Textual";
    }
    else {
        mode = DXmSvnKdisplayOutline;
        label = "View Tree";
    }
    XtVaSetValues (svn, DXmSvnNdisplayMode, (XtArgVal)mode, NULL);

    cs = XmStringCreateSimple (label);
    XtVaSetValues (w, XmNlabelString, (XtArgVal)cs, NULL);
    XmStringFree (cs);

    DXmSvnEnableDisplay (svn);
}


/* This routine creates a pulldown menu in a menu bar */

Widget AddMenu (Widget menu_bar, String label)
{
    Widget menu, button;
    XmString cs;
    Arg args [2];

    menu = XmCreatePulldownMenu (menu_bar, "", NULL, 0);
    cs = XmStringCreateSimple (label);

    XtSetArg (args [0], XmNlabelString, cs);
    XtSetArg (args [1], XmNsubMenuId, menu);
    button = XmCreateCascadeButton (menu_bar, "", args, 2);

    XtManageChild (button);
    XmStringFree (cs);
    return menu;
}


/* This routine adds a button to a menu */

Widget AddButton (Widget menu, String label, XtCallbackProc cb, XtPointer data)
{
    Widget button;
    XmString cs;
    Arg arg;

    cs = XmStringCreateSimple (label);
    XtSetArg (arg, XmNlabelString, cs);
    button = XmCreatePushButton (menu, "", &arg, 1);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, cb, data);
    XmStringFree (cs);

    return button;
}


/* Main routine */

int main (int argc, char **argv)
{
    XtAppContext appctx;
    Display *display;
    Widget shell, main, menu_bar, file_menu, svn;
    XmString cs;
    Arg al [40];
    int ac;

    /* The Attach Callbacks must be set at SVN creation time */

    static XtCallbackRec AttachCB [] = {{Attach, NULL}, {NULL, NULL}};

    /* Initialize CALLMON and trace all images */

    init_call_monitor (argc, argv);

    /* Application initialization */

    XtToolkitInitialize ();

    appctx = XtCreateApplicationContext ();

    display = XtOpenDisplay (appctx, NULL, "verySimpleSvn", "VerySimpleSvn",
        NULL, 0, &argc, argv);
    if (display == NULL)
        XtAppError (appctx, "Can't open display");

    /* Create the application shell */

    shell = XtVaAppCreateShell ("verySimpleSvn", "VerySimpleSvn",
        applicationShellWidgetClass, display,
        XtNx, (XtArgVal)100,
        XtNy, (XtArgVal)100,
        XtNargc, (XtArgVal)argc,
        XtNargv, (XtArgVal)argv,
        XtNtitle, (XtArgVal)"Very Simple SVN Test",
        XtNiconName, (XtArgVal)"VerySimpleSvn",
        XtNallowShellResize, (XtArgVal)True,
        NULL);

    /* Create the main window */

    ac = 0;
    XtSetArg (al[ac], XmNx, 0); ac++;
    XtSetArg (al[ac], XmNy, 0); ac++;
    XtSetArg (al[ac], XmNwidth, 0); ac++;
    XtSetArg (al[ac], XmNheight, 0); ac++;
    main = XmCreateMainWindow (shell, "main", al, ac);
    XtManageChild (main);

    /* Create the SVN widget */

    cs = XmStringCreateSimple ("Default Tree Title");

    ac = 0;
    XtSetArg (al [ac], XmNwidth, 500); ac++;
    XtSetArg (al [ac], XmNheight, 400); ac++;
    XtSetArg (al [ac], DXmSvnNattachToSourceCallback, AttachCB); ac++;
    XtSetArg (al [ac], DXmSvnNexpectHighlighting, True); ac++;
    XtSetArg (al [ac], DXmSvnNstartLocationCursor, 4); ac++;
    XtSetArg (al [ac], DXmSvnNdisplayMode, DXmSvnKdisplayOutline); ac++;
    XtSetArg (al [ac], DXmSvnNnavWindowTitle, cs); ac++;
    XtSetArg (al [ac], DXmSvnNshowPathToRoot, False); ac++;
    XtSetArg (al [ac], DXmSvnNstartColumnComponent, 3); ac++;
    XtSetArg (al [ac], DXmSvnNcolumnLines, True); ac++;
    XtManageChild (svn = DXmCreateSvn (main, "Svn", al, ac));

    XmStringFree (cs);

    /* Set additional SVN callbacks */

    XtAddCallback (svn, DXmSvnNgetEntryCallback, GetEntry, NULL);
    XtAddCallback (svn, DXmSvnNselectAndConfirmCallback, SelectAndConfirm,NULL);
    XtAddCallback (svn, DXmSvnNextendConfirmCallback, ExtendConfirm, NULL);

    /* Create the menu bar and the menu */

    XtManageChild (menu_bar = XmCreateMenuBar (main, "", NULL, 0));
    file_menu = AddMenu (menu_bar, "File");
    AddButton (file_menu, "Expand All", ExpandAll, (XtPointer)svn);
    AddButton (file_menu, "View Tree", ToggleView, (XtPointer)svn);
    AddButton (file_menu, "Quit", Quit, (XtPointer)shell);

    /* Set main window areas */

    XmMainWindowSetAreas (main, menu_bar, NULL, NULL, NULL, svn);

    /* Application loop */

    XtRealizeWidget (shell);
    XtAppMainLoop (appctx);
}
