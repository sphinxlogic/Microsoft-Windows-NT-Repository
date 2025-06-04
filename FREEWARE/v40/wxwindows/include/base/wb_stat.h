
#ifndef wxb_stath
#define wxb_stath

#ifdef __GNUG__
#pragma interface
#endif

#include "wx_defs.h"
#include "wx_item.h"

#ifdef IN_CPROTO
typedef void *wxbStaticItem
#else

// WXDLLEXPORT extern const char *wxStaticNameStr;

class wxObject;
class wxPanel;
class wxPanelDC;
class WXDLLEXPORT wxbStaticItem : public wxItem
   {
     protected :
          int xpos, ypos;           // The common coordinates
          int width, height;        // The common coordinates
          int x_draw, y_draw;       // The coordinates for redrawing
          int w_draw, h_draw;       // The coordinates for redrawing
          wxPanel *panel;
          wxPanelDC *dc;
          Bool isShow;
     public :
          wxbStaticItem(void);
          wxbStaticItem(wxPanel *the_panel,int x,int y,int w,int h);
          ~wxbStaticItem(void);
          virtual Bool Show(Bool WXUNUSED(show)) { return TRUE; } ;
          virtual Bool IsShow(void) const { return isShow; } ;
          virtual void Draw(void) {} ;

          // Statndart Features :-)
          virtual wxWindow *GetParent(void)
                          {
                            return (wxWindow *)panel;
                          };

          virtual void GetSize(int *x, int *y, int *w, int *h)
                          {
                            *x = xpos; *y = ypos;
                            *w = width; *h = height;
                          };

          void GetSize(int *w, int *h)
                          {
                            *w = width; *h = height;
                          };

          virtual void GetPosition(int *x, int *y)
                          {
                            *x = xpos; *y = ypos;
                          };

          virtual void GetClientSize(int *w, int *h)
                          {
                            *w = width; *h = height;
                          };

          virtual void SetSize(int x, int y, int w, int h, int sizeFlags = wxSIZE_AUTO);
          virtual void SetClientSize(int w, int h)
                          {
                            wxbWindow::SetSize(w,h);
                          };
          void SetPosition(int x,int y)
                          {
                            Move(x,y);
                          };

          // Only for redrawing !!!
          virtual void GetDrawingSize(int *x, int *y, int *w, int *h)
                          {
                            *x = x_draw; *y = y_draw;
                            *w = w_draw; *h = h_draw;
                          };
   };

#endif //IN_CPROTO
#endif
