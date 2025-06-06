
HICON    ReadIconFile(    char *szFileName, HINSTANCE hInst,
                          int *W = 0, int *H = 0);
HCURSOR  ReadCursorFile(  char *szFileName, HINSTANCE hInst,
                          int *W = 0, int *H = 0, int *XHot = 0, int *YHot = 0);
HCURSOR  IconToCursor(    char *szFileName, HINSTANCE hInst, int XHot, int YHot,
                          int *W = 0, int *H = 0);
HICON    CursorToIcon(    char *szFileName, HINSTANCE hInst,
                          int *W = 0, int *H = 0);

HCURSOR MakeCursorFromBitmap(HINSTANCE hInst, HBITMAP hBitmap, POINT *pPoint);
HICON MakeIconFromBitmap(HINSTANCE hInst, HBITMAP hBitmap);
