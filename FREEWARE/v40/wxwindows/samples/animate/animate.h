/*
 * File:        anim.h
 * Purpose:     wxWindow animation demo
 * Author:      Markus Franz Meisinger
 * e-mail:      Markus.Meisinger@risc.uni-linz.ac.at
 * Created:     June 1994
 * Updated:     
 * Copyright:   
 */

#ifdef __GNUG__
#pragma interface
#endif

#include "tool3d.h"

class baseObject: public object3D
{ public:
    wxBrush    *faceBrush[10];

    baseObject(void);
    ~baseObject(void);
    void Draw(wxDC &dc);
};

class cube: public baseObject
{ public:
    cube(void);
};

class ufo: public baseObject
{ public:
    ufo(void);
};

// Define a new application type
class AnimApp: public wxApp
{ public:
    AnimApp(void);
    wxFrame *OnInit(void);
};

// Define a new canvas
class objCanvas: public wxCanvas
{ private:
    float      fXMax, fYMax;
    int        xOfs, yOfs;
    wxBitmap   *memBmp;

  public:
    wxMemoryDC memDC;
    int        xRot, yRot, zRot;
    int        xInc, yInc, zInc;
    int        zDist;
    cube       myCube;
    ufo        myUfo;

    objCanvas(wxFrame *frame,
              int x, int y, int w, int h, int style = wxRETAINED,
              float fMaxX = 0.0, float fMaxY = 0.0);
    ~objCanvas(void);
    void OnPaint(void);
    void Draw(wxDC &dc);
    void AnimationStep(void);
    void SetMaxXY(float fMaxX, float fMaxY);
};

// Define an own timer for animation
class animationTimer: public wxTimer
{ public:
    void Notify(void);
};

// Define a new main frame type
class AnimFrame: public wxFrame
{ public:
    wxPanel          *panel;
    objCanvas        *drawArea;
    animationTimer   animation;

    AnimFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    ~AnimFrame(void);
    void OnMenuCommand(int id);
    void OnSize(int w, int h);
    Bool OnClose(void);
};

// IDs for the menu commands
#define ANIMATION_QUIT  101
