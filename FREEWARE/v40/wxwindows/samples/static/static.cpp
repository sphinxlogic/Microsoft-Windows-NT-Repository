
// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#if !USE_EXTENDED_STATICS
#error You must set USE_EXTENDED_STATICS to 1 in wx_setup.h to compile this demo.
#endif

#include <wx_stat.h>

#include "static.h"

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

assa app;
wxStaticBitmap *sbt1, *sbt2, *sbt3, *sbt4;
wxPanel *pann;
wxPanelDC *dc;
wxFont *font;
wxStaticBox *sb1, *sb2, *sb3, *sb4, *sb5, *sb6;
wxStaticBox *sb7, *sb8, *sb9, *sb10, *sb11, *sb12;
wxStaticBox *sb13, *sb14, *sb15, *sb16, *sb17, *sb18;
wxStaticBox *sb19, *sb20, *sb21, *sb22, *sb23, *sb24;
wxStaticSeparator *ss1, *ss2, *ss3, *ss4;

void loholka(wxButton &button,wxEvent *event);

wxFrame *assa::OnInit(void)
   {
     char buff[256];
     sprintf(buff,"(%s) ? : Loholushka",app.argv[0]);
     frame = new wxFrame(NULL,buff,0,0,600,400);

     pann = panel = new wxPanel(frame,0,0,600,400,0);
     panel->Show(TRUE);
     
//<***************************** BOXES                        

     //black solid box with system colors for border and box
     //drawing and box and border
     sb1 = new wxStaticBox(panel, 140,20,60,30, 2, 
                              (COLORREF)-1, (COLORREF)-1, 
                              wxIS_BORDER | wxIS_BOX, wxBLACK_SOLID);
                        
     //black solid box with system colors for border and green box
     //drawing and box and border
     sb2 = new wxStaticBox(panel, 140,60,60,30, 5, 
                              RGB (0,0xcc,0), (COLORREF)-1, 
                              wxIS_BORDER | wxIS_BOX, wxBLACK_SOLID);
                        
     //black solid box with blue border and green box
     //drawing only border
     new wxStaticBox(panel, 140,100,60,30, 2, 
                              RGB (0,0xcc,0), RGB (0,0,0xcc), 
                              wxIS_BORDER, wxBLACK_SOLID);
                        
     //black solid box with system box and green border
     //drawing only border
     sb3 = new wxStaticBox(panel, 140,140,60,30, 2, 
                              (COLORREF)-1, RGB (0,0xcc,0), 
                              wxIS_BORDER, wxBLACK_SOLID);
                        
     //white solid box with system colors for border and box
     //drawing and box and border
     sb4 = new wxStaticBox(panel, 140,180,60,30, 2, 
                              (COLORREF)-1, (COLORREF)-1, 
                              wxIS_BORDER | wxIS_BOX, wxWHITE_SOLID);
                        
     //white solid box with system colors for border and green box
     //drawing only border
     new wxStaticBox(panel, 140,220,60,30, 2, 
                              RGB (0,0xcc,0), (COLORREF)-1, 
                              wxIS_BORDER, wxWHITE_SOLID);
                        
     //white solid box with blue border and green box
     //drawing border and box
     new wxStaticBox(panel, 140,260,60,30, 2, 
                              RGB (0,0xcc,0), RGB (0,0,0xcc), 
                              wxIS_BORDER | wxIS_BOX, wxWHITE_SOLID);
                        
     //black solid box with system colors for border and green box
     //drawing only border
     new wxStaticBox(panel, 140,300,60,30, 2, 
                              (COLORREF)-1, RGB (0,0xcc,0), 
                              wxIS_BORDER, wxWHITE_SOLID);

     //Rized solid box with red border and green box
     //drawing border and box
     // No more comments .... 08)--
     new wxStaticBox(panel, 220,20,60,30, 1, 
                              RGB (0,0xcc,0), RGB(0xcc,0,0), 
                              wxIS_BOX | wxIS_BORDER, wxRIZED_SOLID);
     new wxStaticBox(panel, 220,60,60,30, 2, 
                              (COLORREF)-1, (COLORREF)-1,
                              wxIS_BOX | wxIS_BORDER, wxRECESSED_SOLID);
     new wxStaticBox(panel, 220,100,60,30, 3, 
                              RGB (0,0xcc,0), RGB(0xcc,0xcc,0), 
                              wxIS_BOX | wxIS_BORDER, wxRIZED_SOLID);
     new wxStaticBox(panel, 220,140,60,30, 4, 
                              RGB (0,0xcc,0), (COLORREF)-1, 
                              wxIS_BOX | wxIS_BORDER, wxRIZED_SOLID);

     new wxStaticBox(panel, 300,20,60,30, 1, 
                              RGB (0,0xcc,0), RGB(0xcc,0,0), 
                              wxIS_BOX | wxIS_BORDER, wxBUMPED);
     new wxStaticBox(panel, 300,60,60,30, 2, 
                              (COLORREF)-1, (COLORREF)-1,
                              wxIS_BOX | wxIS_BORDER, wxBUMPED);
     new wxStaticBox(panel, 300,100,60,30, 3, 
                              RGB (0,0xcc,0), RGB(0xcc,0xcc,0), 
                              wxIS_BOX | wxIS_BORDER, wxDIPPED);
     new wxStaticBox(panel, 300,140,80,30, 4, 
                              (COLORREF)-1, (COLORREF)-1, 
                              wxIS_BOX | wxIS_BORDER, wxDIPPED); 

     new wxStaticBox(panel, 220,180,60,30, 1, 
                              RGB (0,0xcc,0), RGB(0xcc,0,0), 
                              wxIS_BOX, wxWHITE_ROUNDED);
     new wxStaticBox(panel, 220,220,60,30, 2, 
                              (COLORREF)-1, (COLORREF)-1,
                              wxIS_BOX | wxIS_BORDER, wxWHITE_ROUNDED);
     new wxStaticBox(panel, 220,260,60,30, 3, 
                              RGB (0,0xcc,0), RGB(0xcc,0xcc,0), 
                              wxIS_BOX | wxIS_BORDER, wxBLACK_ROUNDED);
     new wxStaticBox(panel, 220,300,60,30, 4, 
                              RGB (0,0xcc,0), (COLORREF)-1, 
                              wxIS_BOX | wxIS_BORDER, wxBLACK_ROUNDED); 

     new wxStaticBox(panel, 300,180,60,30, 2, 
                              (COLORREF)-1, (COLORREF)-1,
                              wxIS_BOX | wxIS_BORDER, wxRIZED_ROUNDED);
     new wxStaticBox(panel, 300,220,60,30, 3, 
                              RGB (0,0xcc,0), RGB(0xcc,0xcc,0), 
                              wxIS_BOX | wxIS_BORDER, wxRECESSED_ROUNDED);
     new wxStaticBox(panel, 300,260,60,30, 4, 
                              RGB (0,0xcc,0), (COLORREF)-1, 
                              wxIS_BOX | wxIS_BORDER, wxRECESSED_ROUNDED); 


//<********************************* Separators
     ss1 = new wxStaticSeparator(panel, 10,200,70);
     ss2 = new wxStaticSeparator(panel, 90,200,60,
                                    wxVERT_SEPARATOR, 2,
                                    RGB(0xcc,0,0), wxWHITE_SOLID);
     ss3 = new wxStaticSeparator(panel, 10,300,70,
                                    wxHORIZ_SEPARATOR, 3,
                                    COLORREF(-1), wxBLACK_SOLID);
     ss4 = new wxStaticSeparator(panel, 90,300,80,
                                    wxVERT_SEPARATOR, 2,
                                    RGB(0xcc,0xcc,0), wxDIPPED);

//<************************************ Bitmaps

     sbt1 = new wxStaticBitmap(panel,440,10, new wxBitmap("b1.bmp",wxBITMAP_TYPE_BMP));

     if (wxDisplayDepth() > 8)
     {
       sbt2 = new wxStaticBitmap(panel,440,80, new wxBitmap("b3.bmp",wxBITMAP_TYPE_BMP));
       sbt4 = new wxStaticBitmap(panel,440,180,  new wxBitmap("b4.bmp",wxBITMAP_TYPE_BMP));
     }
     else
     {
       sbt2 = NULL;
       sbt4 = NULL;
     }

//************************************** Buttons 

     wxButton *button = new wxButton(panel, (wxFunction)&loholka, "Exit", 20, 20);


     button->SetClientData((char *) EXIT_BUTTON);
     button->Show(TRUE); 
                        
     button = new wxButton(panel, (wxFunction)&loholka, "Show", 20, 50);
     button->SetClientData((char *) SHOW_BUTTON);
     button->Show(TRUE); 
                        
     button = new wxButton(panel, (wxFunction)&loholka, "Draw", 20, 80);
     button->SetClientData((char *) DRAW_BUTTON);
     button->Show(TRUE); 

     wxCheckBox *chk =
     new wxCheckBox(panel, NULL, "Check", 305, 145);

     frame->Show(TRUE);
     return frame;
   }

void loholka(wxButton &button,wxEvent *event)
   {
     long tag = (long) button.GetClientData();

     switch(tag)
        {
          case EXIT_BUTTON :
                             delete (wxFrame *) app.frame;
                             app.frame = NULL;
                             break;
          case SHOW_BUTTON :
                             sb1 -> Show(!sb1 -> IsShow());
                             ss2 -> Show(!ss2 -> IsShow());
                             if(sbt2)sbt2 -> Show(!sbt2 -> IsShow());
                             sbt4 -> Show(!sbt4 -> IsShow());
                             break;
          case DRAW_BUTTON :
                             sb2 -> Show(!sb2 -> IsShow());
                             ss1 -> Show(!ss1 -> IsShow());
                             sbt1 -> Show(!sbt1 -> IsShow());
                             break;
          case REMOVE_BUTTON :
                     {
                       static int flg = 0;
                       if (!flg)
                          {
                           pann -> RemoveStaticItem(sb4);
                           pann -> RemoveStaticItem(ss3);
                           flg++;
                          }
                       else
                          {
                           pann -> AddStaticItem(sb4);
                      sb4 -> Show(FALSE);
                      sb4 -> Show(TRUE);
                           pann -> AddStaticItem(ss3);
                      ss3 -> Show(FALSE);
                      ss3 -> Show(TRUE);
                           flg = 0;
                          }
                     }
                             break;
          case DELETE_BUTTON :
                 if (sb3)
                    {
                      delete sb3;
                      sb3 = NULL;
                    }
                 else
                    {
     sb3 = new wxStaticBox(pann, 140,140,60,30, 2, 
                              (COLORREF)-1, RGB (0,0xcc,0), 
                              wxIS_BORDER, wxBLACK_SOLID);
                      sb3 -> Show(FALSE);
                      sb3 -> Show(TRUE);
                    }

                 if (ss4)
                    {
                      delete ss4;
                      ss4 = NULL;
                    }
                 else
                    {
     ss4 = new wxStaticSeparator(pann, 400,90,80,
                                    wxVERT_SEPARATOR, 2,
                                    RGB(0xcc,0xcc,0), wxDIPPED);
                      ss4 -> Show(FALSE);
                      ss4 -> Show(TRUE);
                    }

                 if (sbt2)
                    {
                      delete sbt2;
                      sbt2 = NULL;
                    }
                 else
                    {
     sbt2 = new wxStaticBitmap(pann,100,140, new wxBitmap("b3.bmp",wxBITMAP_TYPE_BMP));
                      sbt2 -> Show(FALSE);
                      sbt2 -> Show(TRUE);
                    }
                             break;

        }
   }
