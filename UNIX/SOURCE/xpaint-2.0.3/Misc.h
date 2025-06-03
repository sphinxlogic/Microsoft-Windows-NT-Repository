/*
************************************************************************
**  Misc.c
************************************************************************
*/

/*
**  Widget heirarchy
*/
Widget		GetToplevel(Widget);
Widget		GetShell(Widget);

/*
**  Cursor maniupulation.
*/
void 		SetCrossHairCursor(Widget);
void 		SetPencilCursor(Widget);
void 		SetWatchCursor(Widget);

XRectangle 	*RectUnion(XRectangle *, XRectangle *);
XRectangle 	*RectIntersect(XRectangle *, XRectangle *);

/*
************************************************************************
**  Dialog.c
************************************************************************
*/
/*
**  Alert boxes
*/
void 	AlertBox(Widget, char *, XtCallbackProc, XtCallbackProc, void *);
void 	Notice(Widget, ...);

/*
************************************************************************
**  Protocol.c
************************************************************************
*/
/*
**  WM_DESTROY  callback
*/
void AddDestroyCallback(Widget, void (*func)(Widget, void *, XEvent *), void *data);

/*
************************************************************************
**  Graphic.c
************************************************************************
*/
void	GraphicAdd(Widget);
void	GraphicRemove(Widget, XtPointer, XtPointer);
void	GraphicAll(void (*)(Widget, void *), void *);
void	GraphicSetOp(void (*)(Widget, void *), void *(*)(Widget));

/*
************************************************************************
**  FileName.c
************************************************************************
*/
void    StdSaveFile(Widget, XtPointer, XtPointer);
void    StdSaveRegionFile(Widget, XtPointer, XtPointer);
void 	GetFileName(Widget, int, char *, XtCallbackProc);

/*
************************************************************************
**  FileName.c
************************************************************************
*/
void ZoomSelect(Widget, Widget);
void SizeSelect(Widget, Widget, void (*)(Widget, int, int, int));

/*
************************************************************************
**  SelectOp.c
************************************************************************
*/
void SelectPasteSave(Widget w, void *);
void SelectPasteStart(Widget w, void *, XRectangle *);
