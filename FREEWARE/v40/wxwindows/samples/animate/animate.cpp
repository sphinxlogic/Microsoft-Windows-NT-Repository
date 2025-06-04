/*
 * File:        anim.cc
 * Purpose:     wxWindow animation demo
 * Author:      Markus Franz Meisinger
 * Created:     June 1994
 * Updated:     
 * Copyright:   
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include <stdlib.h>
#include <wx_timer.h>
#include <stdio.h>
#ifdef wx_msw
#include <malloc.h>
#endif

// include class definitions
#include "animate.h"

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// ----------------------------------------------------------------------------
// data declaration 
// ----------------------------------------------------------------------------

// global application variables used in different objects
AnimFrame *frame = NULL;          // main frame

// ----------------------------------------------------------------------------
// method implementation part of defined classes
// ----------------------------------------------------------------------------

// anim main frame constructor; some initialization will be done
AnimFrame::AnimFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{
  panel    = NULL;
  drawArea = NULL;
  animation.Start(100);
}

// anim main frame destructor
AnimFrame::~AnimFrame(void)
{
}

// each menu selection invoces this main frame method
void AnimFrame::OnMenuCommand(int id)
{
  switch (id) {
    case ANIMATION_QUIT:
      OnClose();
      delete this;
    break;
  }
}

// Size the subwindowssss when the frame is resized
void AnimFrame::OnSize(int w, int h)
{
  if (panel && drawArea) {
    int width, height;
    GetClientSize(&width, &height);
    panel->SetSize(0, 0, width, (int)(height/2));
    drawArea->SetSize(0, (int)(height/2), width, (int)(height/2));
    drawArea->SetMaxXY((float)width, (float)(height/2));
  }
}

Bool AnimFrame::OnClose(void)
{
  animation.Stop();
  drawArea->SetBrush(NULL);
  drawArea->SetPen(NULL);
  drawArea->memDC.SetPen(NULL);
  drawArea->memDC.SetBrush(NULL);
  drawArea->memDC.SelectObject(NULL);
  return (TRUE);
}

// Define a constructor for my canvas
objCanvas::objCanvas(wxFrame *frame, 
                   int x, int y, int w, int h, int style,
                   float fMaxX, float fMaxY):
 wxCanvas(frame, x, y, w, h, style)
{
  fXMax = fMaxX;
  fYMax = fMaxY;

  // create a bitmap of canvas size and select it into the memory dc
  memBmp = new wxBitmap((int)(fXMax+1), (int)(fYMax+1));
  memDC.SelectObject(memBmp);

  xOfs = (int)(fXMax / 2);
  yOfs = (int)(fYMax / 2);
  xRot = yRot = zRot = 0;
  xInc = 6;
  yInc = -9;
  zInc = 12;
  zDist = -130;
}

// Define the destructor; delete all dynamic data
objCanvas::~objCanvas(void)
{
  // *** CAUTION *** CAUTION *** CAUTION ***
  // Never delete a bitmap which is selected into a DC if the DC will be deleted!
  // The bitmap will be deleted by the DC!
  // But i DID it !!! :) and it resulted in a system wide crash of MS-WINDOWS
  // (in Motif it works fine (why?))
  // The funny thing with such a crash is that the system doesn4t immediately
  // have problems, instead it works fine for a few minutes until ANY other
  // program gets into some memory conflict. After this application error 
  // each program still running complains problems with the memory and stops. 8-O
  // The last thing you see is the standard windows background color (if you
  // are lucky:)
  // I never saw such a system wide crash before! WOU :) in some sense it 
  // was funny :)

  // if you have saved all your data and want some fun ;) uncomment the next line
  //delete memBmp;
}

// Define the repainting behaviour
void objCanvas::OnPaint(void)
{
  BeginDrawing();

  Draw(memDC);                                // first, draw in memory dc
  (*GetDC()).Blit(0.0, 0.0, fXMax, fYMax,     // second, copy memory dc to
		  &memDC, 0.0 ,0.0, wxCOPY);   // display (to avoid flickering)

  EndDrawing();
}

void objCanvas::Draw(wxDC &dc)
{
  dc.Clear();
  myCube.Rotate(xRot, yRot, zRot);
  myCube.Translate(0, 0, zDist+60);
  myCube.Project(xOfs, yOfs, 10, 10, 500);
  myCube.Draw(dc);
  myUfo.Rotate(xRot, yRot, zRot);
  myUfo.Translate(0, 0, zDist);
  myUfo.Project(xOfs, yOfs, 10, 10, 500);
  myUfo.Draw(dc);
}

void objCanvas::AnimationStep(void)
{
  xRot = (xRot + xInc) % 360;
  if (xRot < 0) { xRot += 360; }
  yRot = (yRot + yInc) % 360;
  if (yRot < 0) { yRot += 360; }
  zRot = (zRot + zInc) % 360;
  if (zRot < 0) { zRot += 360; }
  //printf("alpha: %d, beta: %d, gamma: %d\n", xRot, yRot, zRot);
}

void objCanvas::SetMaxXY(float fMaxX, float fMaxY)
{
  fXMax = fMaxX;
  fYMax = fMaxY;

  memDC.SelectObject(NULL);
  delete memBmp;                                         // resize memory bitmap

  memBmp = new wxBitmap((int)(fXMax+1), (int)(fYMax+1));
  memDC.SelectObject(memBmp);

  xOfs = (int)(fXMax / 2);
  yOfs = (int)(fYMax / 2);
}

// Implemention of the notification method
void animationTimer::Notify(void)
{
  frame->drawArea->AnimationStep();
  frame->drawArea->OnPaint();
}

baseObject::baseObject(void)
{
  faceBrush[0] = new wxBrush("RED", wxSOLID);
  faceBrush[1] = new wxBrush("GREEN", wxSOLID);
  faceBrush[2] = new wxBrush("BLUE", wxSOLID);
  faceBrush[3] = new wxBrush("WHITE", wxSOLID);
  faceBrush[4] = new wxBrush("YELLOW", wxSOLID);
  faceBrush[5] = new wxBrush("CYAN", wxSOLID);
  faceBrush[6] = new wxBrush("BROWN", wxSOLID);
  faceBrush[7] = new wxBrush("VIOLET", wxSOLID);
  faceBrush[8] = new wxBrush("PURPLE", wxSOLID);
  faceBrush[9] = new wxBrush("CORAL", wxSOLID);
  
}

baseObject::~baseObject(void)
{
  int i;
  
  for (i = 0; i < 10; i++) {
    delete faceBrush[i];
  }
}
void baseObject::Draw(wxDC &dc)
{
  int i, j, noPts;
  struct faceStruct *face;
  struct point2D pt;
  wxPoint points[8];

  for (i = 0; i < faces.no; i++) {                         // for all faces
    face = (faces.points+i);
    noPts = face->noPts;
    if (face->visible) {                                   //   if visible
      //printf("Normal %d: (%d,%d,%d)\n", i+1, face->normal.x, face->normal.y, face->normal.z);
      for (j = 0; j < noPts; j++) {                
        pt  = *((face->projected)+j);
        points[j].x = (float)pt.x;
        points[j].y = (float)pt.y;
      }
      dc.SetBrush(faceBrush[i]);
      dc.DrawPolygon(noPts, points, 0, 0, wxODDEVEN_RULE); //     draw face
    }
  }
}

ufo::ufo(void)
{
  int x1Pts[8] = { 0, -17, -25, -17, 0, 17, 25, 17 };
  int x2Pts[8] = { 0, -25, -35, -25, 0, 25, 35, 25 };
  struct point3D ufoPt[16], *pt, p, s, w, ps, sw;
  int i, bytes;
  struct faceStruct *face;

  faces.no = 10;
  bytes = sizeof(struct faceStruct) * faces.no;
  faces.points = (struct faceStruct *)malloc(bytes);

  for (i = 0; i < 8; i++) {
    ufoPt[i].x = x1Pts[i];
    ufoPt[i].y = x1Pts[(i+6) % 8];
    ufoPt[i].z = -10;
    ufoPt[i+8].x = x2Pts[i];
    ufoPt[i+8].y = x2Pts[(i+6) % 8];
    ufoPt[i+8].z = 10;
  }

  for (i = 0; i < 7; i++) {
    face = (faces.points+i);
    face->noPts = 4;
    face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);
    
    pt  = (face->origPoint+0);
    *pt = ufoPt[i];
    pt  = (face->origPoint+1);
    *pt = ufoPt[i+8];
    pt  = (face->origPoint+2);
    *pt = ufoPt[(i+8) + 1];
    pt  = (face->origPoint+3);
    *pt = ufoPt[i+1];
  }

  face = (faces.points+i);
  face->noPts = 4;
  face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
  face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
  face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);
  pt  = (face->origPoint+0);
  *pt = ufoPt[7];
  pt  = (face->origPoint+1);
  *pt = ufoPt[15];
  pt  = (face->origPoint+2);
  *pt = ufoPt[8];
  pt  = (face->origPoint+3);
  *pt = ufoPt[0];

  face = (faces.points+8);
  face->noPts = 8;
  face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 8);
  face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 8);
  face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 8);
  for (i=0; i < 8; i++) {
    pt = (face->origPoint+i);
    *pt = ufoPt[i];
  }
  face = (faces.points+9);
  face->noPts = 8;
  face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 8);
  face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 8);
  face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 8);
  for (i=0; i < 8; i++) {
    pt = (face->origPoint+i);
    *pt = ufoPt[15-i];
  }

  for (i = 0; i < faces.no; i++) {
    face = (faces.points+i);
    p = *(face->origPoint+0);
    s = *(face->origPoint+1);
    w = *(face->origPoint+2);
    ps.x = s.x - p.x;
    ps.y = s.y - p.y;
    ps.z = s.z - p.z;
    sw.x = w.x - s.x;
    sw.y = w.y - s.y;
    sw.z = w.z - s.z;
    face->origNormal.x = sw.y * ps.z - sw.z * ps.y;
    face->origNormal.y = sw.z * ps.x - sw.x * ps.z;
    face->origNormal.z = sw.x * ps.y - sw.y * ps.x;
  }
}

cube::cube(void)
{
  int bytes;
  struct faceStruct *face;
  struct point3D    *pt;

  faces.no = 6;
  bytes = sizeof(struct faceStruct) * faces.no;
  faces.points = (struct faceStruct *)malloc(bytes);

//  for (i = 0; i < faces.no; i++) {

    face = (faces.points+0);
    face->noPts = 4;
    face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);

    face->origNormal.x = 0;
    face->origNormal.y = 0;
    face->origNormal.z = -16384;

    pt  = (face->origPoint+0);
    pt->x = -50; pt->y =  50; pt->z = -50; // P
    pt  = (face->origPoint+1);
    pt->x = -50; pt->y = -50; pt->z = -50; // Q
    pt  = (face->origPoint+2);
    pt->x =  50; pt->y = -50; pt->z = -50; // R
    pt  = (face->origPoint+3);
    pt->x =  50; pt->y =  50; pt->z = -50; // S

    face = (faces.points+1);
    face->noPts = 4;
    face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);

    face->origNormal.x = 0;
    face->origNormal.y = 16384;
    face->origNormal.z = 0;

    pt  = (face->origPoint+0);
    pt->x = -50; pt->y =  50; pt->z = -50; // P
    pt  = (face->origPoint+1);
    pt->x =  50; pt->y =  50; pt->z = -50; // S
    pt  = (face->origPoint+2);
    pt->x =  50; pt->y =  50; pt->z =  50; // W
    pt  = (face->origPoint+3);
    pt->x = -50; pt->y =  50; pt->z =  50; // T

    face = (faces.points+2);
    face->noPts = 4;
    face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);

    face->origNormal.x = 16384;
    face->origNormal.y = 0;
    face->origNormal.z = 0;

    pt  = (face->origPoint+0);
    pt->x =  50; pt->y =  50; pt->z = -50; // S
    pt  = (face->origPoint+1);
    pt->x =  50; pt->y = -50; pt->z = -50; // R
    pt  = (face->origPoint+2);
    pt->x =  50; pt->y = -50; pt->z =  50; // V
    pt  = (face->origPoint+3);
    pt->x =  50; pt->y =  50; pt->z =  50; // W

    face = (faces.points+3);
    face->noPts = 4;
    face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);

    face->origNormal.x = 0;
    face->origNormal.y = -16384;
    face->origNormal.z = 0;

    pt  = (face->origPoint+0);
    pt->x = -50; pt->y = -50; pt->z = -50; // Q
    pt  = (face->origPoint+1);
    pt->x = -50; pt->y = -50; pt->z =  50; // U
    pt  = (face->origPoint+2);
    pt->x =  50; pt->y = -50; pt->z =  50; // V
    pt  = (face->origPoint+3);
    pt->x =  50; pt->y = -50; pt->z = -50; // R

    face = (faces.points+4);
    face->noPts = 4;
    face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);

    face->origNormal.x = -16384;
    face->origNormal.y = 0;
    face->origNormal.z = 0;

    pt  = (face->origPoint+0);
    pt->x = -50; pt->y =  50; pt->z = -50; // P
    pt  = (face->origPoint+1);
    pt->x = -50; pt->y =  50; pt->z =  50; // T
    pt  = (face->origPoint+2);
    pt->x = -50; pt->y = -50; pt->z =  50; // U
    pt  = (face->origPoint+3);
    pt->x = -50; pt->y = -50; pt->z = -50; // Q

    face = (faces.points+5);
    face->noPts = 4;
    face->origPoint = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->point     = (struct point3D *)malloc(sizeof(struct point3D) * 4);
    face->projected = (struct point2D *)malloc(sizeof(struct point2D) * 4);

    face->origNormal.x = 0;
    face->origNormal.y = 0;
    face->origNormal.z = 16384;

    pt  = (face->origPoint+0);
    pt->x = -50; pt->y =  50; pt->z =  50; // T
    pt  = (face->origPoint+1);
    pt->x =  50; pt->y =  50; pt->z =  50; // W
    pt  = (face->origPoint+2);
    pt->x =  50; pt->y = -50; pt->z =  50; // V
    pt  = (face->origPoint+3);
    pt->x = -50; pt->y = -50; pt->z =  50; // U

//  }
}


void ObjDistance(wxSlider &slider, wxCommandEvent &event)
{
  frame->drawArea->zDist = slider.GetValue();
}

void RotateX(wxSlider &slider, wxCommandEvent &event)
{
  frame->drawArea->xInc = slider.GetValue();
}

void RotateY(wxSlider &slider, wxCommandEvent &event)
{
  frame->drawArea->yInc = slider.GetValue();
}

void RotateZ(wxSlider &slider, wxCommandEvent &event)
{
  frame->drawArea->zInc = slider.GetValue();
}

// ----------------------------------------------------------------------------
// entry point of the whole application
// ----------------------------------------------------------------------------

// this statement initializes the whole application and calls OnInit
AnimApp animApp;

// application contructor
AnimApp::AnimApp(void)
{
  // testing of ressources, this forces the program to look for
  // an application default file called "Anim"
  wx_class = copystring("Animation");
}

// `Main program' equivalent, creating windows and returning main frame
wxFrame *AnimApp::OnInit(void)
{
  // used variables
  wxMenuBar *animMenu   = new wxMenuBar;
  wxMenu    *projectMenu = new wxMenu;
  int       width, height;

  // Create the main frame window
  frame = new AnimFrame(NULL, "wxWindows Animation Demo", 50, 50, 390, 480);

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("aiai.ico"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("aiai.xbm"));
#endif

  // Give it a status line
  frame->CreateStatusLine(1);
  frame->SetStatusText("wxWindows Animation Demo");

  // Make the menubar
  projectMenu->Append(ANIMATION_QUIT, "&Quit");

  animMenu->Append(projectMenu, "&File");

  frame->SetMenuBar(animMenu);

  // Fetch client size of main frame
  frame->GetClientSize(&width, &height);

  // Make a panel with a message just half the size of the whole main frame
  frame->panel = new wxPanel(frame, 0, 0, width, (int)(height/2));
  //(void)new wxMessage(frame->panel, "Hello! A message outside of a drawing area!");
  (void)new wxSlider(frame->panel, (wxFunction)RotateX, "rotation speed x:",
                     6, -15, 15, 350);

  frame->panel->NewLine();
  (void)new wxSlider(frame->panel, (wxFunction)RotateY, "rotation speed y:",
                     -9, -15, 15, 350);
  frame->panel->NewLine();
  (void)new wxSlider(frame->panel, (wxFunction)RotateZ, "rotation speed z:",
                     12, -15, 15, 350);

  frame->panel->NewLine();
  (void)new wxSlider(frame->panel, (wxFunction)ObjDistance, "object distance:",
		     -130, -1000, 250, 350);

  // Make a canvas with a rectangle and put in on the left space in the main frame
  frame->drawArea = new objCanvas(frame,
				 0, (int)(height/2), width, (int)(height/2),
				 wxBORDER | wxRETAINED,
                                 (float)width, (float)(height/2));

  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}
