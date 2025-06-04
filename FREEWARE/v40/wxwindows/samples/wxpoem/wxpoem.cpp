/**************************************************************************
 * wxPoem: wxpoem.cc                                                      *
 *                                                                        *
 * A small C++ program which displays a random poem on execution.         *
 * It also allows search for poems containing a string.                   *
 * It requires winpoem.dat and creates winpoem.idx.                       *
 *                                                                        *
 * Converted from WinPoem source.                                         *
 *                                                                        *
 * Copyright 1994 Julian Smart, email J.Smart@ed.ac.uk                    *
 *                                                                        *
 * Filename: wxpoem.cc                                                    *
 * Purpose:  Main source file for wxPoem.                                 *
 * Version:  1.0                                                          *
 *************************************************************************/

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include "wx_help.h"

#include "wxpoem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define         buf_size 10000
#define         DEFAULT_POETRY_DAT "wxpoem"
#define         DEFAULT_POETRY_IND "wxpoem"
#define         DEFAULT_CHAR_HEIGHT 18
#define         DEFAULT_FONT "Swiss"
#define         DEFAULT_X_POS 0
#define         DEFAULT_Y_POS 0
#define         BORDER_SIZE 30
#define         THIN_LINE_BORDER 10
#define         THICK_LINE_BORDER 16
#define         THICK_LINE_WIDTH 2
#define         SHADOW_OFFSET 1
#define         X_SIZE 30
#define         Y_SIZE 20

static char     *poem_buffer;          // Storage for each poem
static char     line[150];                      // Storage for a line
static char     title[150];                     // Remember the title
static char     *search_string = NULL;          // The search string
static int      pages[30];                      // For multipage poems -
                                                // store the start of each page
static long     last_poem_start = 0;            // Start of last found poem
static long     last_find = -1;                 // Point in file of last found
                                                // search string
static Bool     search_ok = FALSE;              // Search was successful
static Bool     same_search = FALSE;            // Searching on same string

static long     poem_index[600];                     // Index of poem starts
static long     nitems = 0;                     // Number of poems
static int      desired_char_height = DEFAULT_CHAR_HEIGHT; // Desired height
static char     DesiredFont[64];                // Chosen font
static int      char_height = DEFAULT_CHAR_HEIGHT; // Actual height
static int      index_ptr = -1;                 // Pointer into index
static int      poem_height, poem_width;        // Size of poem
static int      XPos;                           // Startup X position
static int      YPos;                           // Startup Y position
static int      pointSize = 12;                 // Font size

static char     *index_filename = NULL;            // Index filename
static char     *data_filename = NULL;             // Data filename
static char     error_buf[300];                 // Error message buffer
static Bool     loaded_ok = FALSE;              // Poem loaded ok
static Bool     index_ok = FALSE;               // Index loaded ok

static Bool     paging = FALSE;                 // Are we paging?
static int      current_page = 0;               // Currently viewed page

wxIcon          *Corner1 = NULL;
wxIcon          *Corner2 = NULL;
wxIcon          *Corner3 = NULL;
wxIcon          *Corner4 = NULL;

// Fonts
wxFont          *NormalFont = NULL;
wxFont          *BoldFont = NULL;
wxFont          *ItalicFont = NULL;

// Pens
wxPen           *GreyPen = NULL;
wxPen           *DarkGreyPen = NULL;
wxPen           *WhitePen = NULL;

// Backing bitmap
wxBitmap        *backingBitmap = NULL;

void            PoetryError(char *, char *caption="wxPoem Error");
void            PoetryNotify(char *Msg, char *caption="wxPoem");
void            TryLoadIndex();
Bool            LoadPoem(char *, long);
int             GetIndex();
int             LoadIndex(char *);
Bool            Compile(void);
void            WritePreferences();
void            ReadPreferences();
void            FindMax(int *max_thing, int thing);
void            CreateFonts();
#ifdef wx_msw
void            CopyToClipboard(HWND, char *);
#endif

wxMenu    *popupMenu = NULL;
void PopupFunction(wxMenu& menu, wxCommandEvent& event);

wxHelpInstance *HelpInstance = NULL;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

MyApp WinPoemApp;
MainWindow *TheMainWindow = NULL;

// Callbacks
void button_proc(wxButton& but, wxCommandEvent& event);
void list_proc(wxListBox& list, wxCommandEvent& event);
void GenericOk(wxButton& but, wxCommandEvent& event);

// Create the fonts
void CreateFonts()
{
  NormalFont = wxTheFontList->FindOrCreateFont(pointSize, wxSWISS, wxNORMAL, wxNORMAL);
  BoldFont =   wxTheFontList->FindOrCreateFont(pointSize, wxSWISS, wxNORMAL, wxBOLD);
  ItalicFont = wxTheFontList->FindOrCreateFont(pointSize, wxSWISS, wxITALIC, wxNORMAL);
}

MainWindow::MainWindow(wxFrame *frame, char *title, int x, int y, int w, int h, long style):
  wxFrame(frame, title, x, y, w, h, style)
{
}

// Read the poetry buffer, either for finding the size
// or for writing to a bitmap (not to the window directly,
// since that displays messily)
// If DrawIt is true, we draw, otherwise we just determine the
// size the window should be.
void MainWindow::ScanBuffer(wxDC *dc, Bool DrawIt, int *max_x, int *max_y)
{
    int i = pages[current_page];
    int ch = -1;
    float x = 10.0;
    float y = 0.0;
    int j;
    char *line_ptr;
    int curr_width = 0;
    Bool page_break = FALSE;

    int width = 0;
    int height = 0;

    if (DrawIt)
    {
      y = (*max_y - poem_height)/2;
      width = *max_x;
      height = *max_y;
    }

    if (DrawIt && wxColourDisplay())
    {
      dc->SetBrush(wxLIGHT_GREY_BRUSH);
      dc->SetPen(GreyPen);
      dc->DrawRectangle(0, 0, width, height);
      dc->SetBackgroundMode(wxTRANSPARENT);
    }

    // See what ACTUAL char height is
    dc->SetFont(NormalFont);
    float xx;
    float yy;
    dc->GetTextExtent("X", &xx, &yy);
    char_height = (int)yy;

    if (current_page == 0)
      title[0] = 0;
    else if (title[0] != 0)
    {
       dc->SetFont(BoldFont);
       dc->GetTextExtent(title, &xx, &yy);
       FindMax(&curr_width, (int)xx);

       if (DrawIt)
       {
         x = (width - xx)/2;
         dc->SetFont(BoldFont);

         // Change text to BLACK!
         dc->SetTextForeground(wxBLACK);
         dc->DrawText(title, x, y);
         // Change text to WHITE!
         dc->SetTextForeground(wxWHITE);
         dc->DrawText(title, x-SHADOW_OFFSET, y-SHADOW_OFFSET);
       }
       y += char_height;
       y += char_height;
    }

    while (ch != 0 && !page_break)
    {
        j = 0;
#ifdef wx_msw
        while (((ch = poem_buffer[i]) != 13) && (ch != 0))
#else
        while (((ch = poem_buffer[i]) != 10) && (ch != 0))
#endif
        {
            line[j] = ch;
            j ++;
            i ++;
        }

#ifdef wx_msw
        if (ch == 13)
#else
        if (ch == 10)
#endif
        {
            ch = -1;
            i ++;
#ifdef wx_msw
            // Add another to skip the linefeed
            i ++;
#endif
            // If a single newline on its own, put a space in
            if (j == 0)
            {
              line[j] = ' ';
              j ++;
              line[j] = 0;
            }
        }

        if (j > 0)
        {
          line[j] = 0;
          if (line[0] == '@')
          {
            switch (line[1])
            {
              case 'P':
                paging = TRUE;
                page_break = TRUE;
                break;

              case 'T':
                dc->SetFont(BoldFont);
                line_ptr = line+3;

                strcpy(title, line_ptr);
                strcat(title, " (cont'd)");

                dc->GetTextExtent(line_ptr, &xx, &yy);
                FindMax(&curr_width, (int)xx);

                if (DrawIt)
                {
                  x = (width - xx)/2;
                  dc->SetFont(BoldFont);

                  // Change text to BLACK!
                  dc->SetTextForeground(wxBLACK);
                  dc->DrawText(line_ptr, x, y);

                  // Change text to WHITE!
                  dc->SetTextForeground(wxWHITE);
                  dc->DrawText(line_ptr, x-SHADOW_OFFSET, y-SHADOW_OFFSET);
                  dc->SetTextForeground(wxWHITE);
                }
                break;

              case 'A':
                line_ptr = line+3;
                dc->SetFont(ItalicFont);

                dc->GetTextExtent(line_ptr, &xx, &yy);
                FindMax(&curr_width, (int)xx);

                if (DrawIt)
                {
                  x = (width - xx)/2;
                  dc->SetTextForeground(wxBLACK);
                  dc->DrawText(line_ptr, x, y);
                }
                break;

              // Default: just ignore this line
              default:
                y -= char_height;
            }
           }
           else
           {
              dc->SetFont(NormalFont);

              dc->GetTextExtent(line, &xx, &yy);
              FindMax(&curr_width, (int)xx);
    
              if (DrawIt)
              {
                int x = (int)((width - xx)/2.0);
                dc->SetFont(NormalFont);
                dc->SetTextForeground(wxBLACK);
                dc->DrawText(line, (float)x, (float)y);
              }
           }
        }
        y += char_height;
    }

    // Write (cont'd)
    if (page_break)
    {
       char *cont = "(cont'd)";

       dc->SetFont(NormalFont);

       dc->GetTextExtent(cont, &xx, &yy);
       FindMax(&curr_width, (int)xx);
       if (DrawIt)
       {
         int x = (int)((width - xx)/2.0);
         dc->SetFont(NormalFont);
         dc->SetTextForeground(wxBLACK);
         dc->DrawText(cont, (float)x, (float)y);
       }
       y += 2*char_height;
    }

    *max_x = (int)curr_width;
    *max_y = (int)(y-char_height);

    if (page_break)
      pages[current_page+1] = i;
    else
      paging = FALSE;

    if (DrawIt)
    {
      // Draw dark grey thick border
      if (wxColourDisplay())
      {
        dc->SetBrush(wxGREY_BRUSH);
        dc->SetPen(wxGREY_PEN);

        // Left side
        dc->DrawRectangle(0, 0, THIN_LINE_BORDER, height);
        // Top side
        dc->DrawRectangle(THIN_LINE_BORDER, 0, width-THIN_LINE_BORDER, THIN_LINE_BORDER);
        // Right side
        dc->DrawRectangle(width-THIN_LINE_BORDER, THIN_LINE_BORDER, width, height-THIN_LINE_BORDER);
        // Bottom side
        dc->DrawRectangle(THIN_LINE_BORDER, height-THIN_LINE_BORDER, width-THIN_LINE_BORDER, height);
      }
      // Draw border
      // Have grey background, plus 3-d border -
      // One black rectangle.
      // Inside this, left and top sides - dark grey. Bottom and right -
      // white.

      // Change pen to black
      dc->SetPen(wxBLACK_PEN);
      dc->DrawLine(THIN_LINE_BORDER, THIN_LINE_BORDER, width-THIN_LINE_BORDER, THIN_LINE_BORDER);
      dc->DrawLine(width-THIN_LINE_BORDER, THIN_LINE_BORDER, width-THIN_LINE_BORDER, height-THIN_LINE_BORDER);
      dc->DrawLine(width-THIN_LINE_BORDER, height-THIN_LINE_BORDER, THIN_LINE_BORDER, height-THIN_LINE_BORDER);
      dc->DrawLine(THIN_LINE_BORDER, height-THIN_LINE_BORDER, THIN_LINE_BORDER, THIN_LINE_BORDER);
  
      // Right and bottom white lines - 'grey' (black!) if
      // we're running on a mono display.
      if (wxColourDisplay())
        dc->SetPen(WhitePen);
      else
        dc->SetPen(DarkGreyPen);

      dc->DrawLine(width-THICK_LINE_BORDER, THICK_LINE_BORDER,
                   width-THICK_LINE_BORDER, height-THICK_LINE_BORDER);
      dc->DrawLine(width-THICK_LINE_BORDER, height-THICK_LINE_BORDER,
                   THICK_LINE_BORDER, height-THICK_LINE_BORDER);

      // Left and top grey lines
      dc->SetPen(DarkGreyPen);
      dc->DrawLine(THICK_LINE_BORDER, height-THICK_LINE_BORDER,
                   THICK_LINE_BORDER, THICK_LINE_BORDER);
      dc->DrawLine(THICK_LINE_BORDER, THICK_LINE_BORDER,
                   width-THICK_LINE_BORDER, THICK_LINE_BORDER);

#ifdef wx_msw
      // Draw icons
      dc->DrawIcon(Corner1, 0.0, 0.0);
      dc->DrawIcon(Corner2, (float)(width-32.0), 0.0);

      int y2 = height - 32;
      int x2 = (width-32);
      dc->DrawIcon(Corner3, 0.0, (float)y2);
      dc->DrawIcon(Corner4, (float)x2, (float)y2);
#endif
    }
}

// Get an index (randomly generated) and load the poem
void MainWindow::GetIndexLoadPoem(void)
{
    if (index_ok)
      index_ptr = GetIndex();

    if (index_ptr > -1)
      loaded_ok = LoadPoem(data_filename, -1);
}

// Find the size of the poem and resize the window accordingly
void MainWindow::Resize(void)
{
  // Get the poem size
  ScanBuffer(canvas->GetDC(), FALSE, &poem_width, &poem_height);
  int x = poem_width + (2*BORDER_SIZE);
  int y = poem_height + (2*BORDER_SIZE);

  SetClientSize(x, y);

  // In case client size isn't what we set it to...
  int xx, yy;
  GetClientSize(&xx, &yy);

  wxMemoryDC memDC(canvas->GetDC());
  if (backingBitmap) delete backingBitmap;
  backingBitmap = new wxBitmap(x, yy);
  memDC.SelectObject(backingBitmap);

  memDC.Clear();
  TheMainWindow->ScanBuffer(&memDC, TRUE, &xx, &yy);
}

// Which is more?
void FindMax(int *max_thing, int thing)
{
  if (thing > *max_thing)
    *max_thing = thing;
}

// Process menu commands
// Return TRUE if processed one
void MainWindow::OnMenuCommand(int Item)
{
}

// Next page/poem
void MainWindow::NextPage(void)
{
  if (paging)
    current_page ++;
  else
  {
    current_page = 0;
    GetIndexLoadPoem();
  }
  Resize();
}

// Previous page
void MainWindow::PreviousPage(void)
{
  if (current_page > 0)
  {
    current_page --;
    Resize();
  }
}

// Search for a string
void MainWindow::Search(Bool ask)
{
  long position;

  if (ask || !search_string)
  {
    char *s = wxGetTextFromUser("Enter search string", "Search", search_string);
    if (s)
    {
      if (search_string) delete[] search_string;
      search_string = copystring(s);
      search_ok = TRUE;
    } else search_ok = FALSE;
  }
  else
  {
    same_search = TRUE;
    search_ok = TRUE;
  }

  if (search_string && search_ok)
  {
    position = DoSearch();
    if (position > -1)
    {
       loaded_ok = LoadPoem(data_filename, position);
       Resize();
    }
    else
    {
      last_poem_start = 0;
      PoetryNotify("Search string not found.");
    }
  }
}

// Process characters
void MyCanvas::OnChar(wxKeyEvent& event)
{
  switch (event.KeyCode())
  {
    case 'n':
    case 'N':
      // Next match
      TheMainWindow->Search(FALSE);
      break;
    case 's':
    case 'S':
      // New search
      TheMainWindow->Search(TRUE);
      break;
    case WXK_SPACE:
      // Another poem
      TheMainWindow->NextPage();
      break;
    case 27:
      if (TheMainWindow->OnClose())
        delete TheMainWindow;
    default:
       break;
   }
 }

// Copy a string to the clipboard
#ifdef wx_msw
void CopyToClipboard(HWND handle, char *s)
{
  int length = strlen(s);
  HANDLE hGlobalMemory = GlobalAlloc(GHND, (DWORD) length + 1);
  if (hGlobalMemory)
  {
#ifdef __WINDOWS_386__
    LPSTR lpGlobalMemory = MK_FP32(GlobalLock(hGlobalMemory));
#else
    LPSTR lpGlobalMemory = (LPSTR)GlobalLock(hGlobalMemory);
#endif
    int i, j = 0;
    for (i = 0; i < length; i ++)
    {
      if (s[i] == '@')
      {
        i++;
        switch (s[i])
        {
          case 'P':
            break;
          case 'T':
          case 'A':
          default:
            i ++;
            break;
        }
      }
      else
      {
        lpGlobalMemory[j] = s[i];
        j ++;
      }
    }

    GlobalUnlock(hGlobalMemory);
    OpenClipboard(handle);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hGlobalMemory);
    CloseClipboard();
  }
}
#endif

wxFrame *MyApp::OnInit(void)
{
  poem_buffer = new char[buf_size];
  
  GreyPen = new wxPen("LIGHT GREY", THICK_LINE_WIDTH, wxSOLID);
  DarkGreyPen = new wxPen("GREY", THICK_LINE_WIDTH, wxSOLID);
  WhitePen = new wxPen("WHITE", THICK_LINE_WIDTH, wxSOLID);

  HelpInstance = new wxHelpInstance(TRUE);
  HelpInstance->Initialize("wxpoem");

  CreateFonts();

  ReadPreferences();

  // Seed the random number generator
  time_t current_time;

  (void)time(&current_time);
  srand((unsigned int)current_time);

//    randomize();
  pages[0] = 0;

  TheMainWindow = new MainWindow(NULL, "wxPoem", XPos, YPos, 100, 100,
  // Different GUI on Win95 so old Windows 3.1-style tiny caption looks a bit naff.
#ifdef WIN32
    wxSDI // wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCAPTION
#else
   wxSDI // | wxTINY_CAPTION_HORIZ | wxMINIMIZE_BOX
#endif
#ifdef wx_x
        | wxTHICK_FRAME
#endif
   );

#ifdef wx_x
  wxIcon *poemIcon = new wxIcon("wxpoem");
  TheMainWindow->SetIcon(poemIcon);
#endif

  TheMainWindow->canvas = new MyCanvas(TheMainWindow);

  popupMenu = new wxMenu(NULL, (wxFunction)PopupFunction);
  popupMenu->Append(POEM_NEXT, "Next poem/page");
  popupMenu->Append(POEM_PREVIOUS, "Previous page");
  popupMenu->AppendSeparator();
  popupMenu->Append(POEM_SEARCH, "Search");
  popupMenu->Append(POEM_NEXT_MATCH, "Next match");
  popupMenu->Append(POEM_COPY, "Copy to clipboard");
  popupMenu->Append(POEM_MINIMIZE, "Minimize");
  popupMenu->AppendSeparator();
  popupMenu->Append(POEM_BIGGER_TEXT, "Bigger text");
  popupMenu->Append(POEM_SMALLER_TEXT, "Smaller text");
  popupMenu->AppendSeparator();
  popupMenu->Append(POEM_ABOUT, "About wxPoem");
  popupMenu->AppendSeparator();
  popupMenu->Append(POEM_EXIT, "Exit");

  if (argc > 1)
  {
    index_filename = copystring(argv[1]);
    data_filename = copystring(argv[1]);
  }
  else
  {
    index_filename = DEFAULT_POETRY_IND;
    data_filename = DEFAULT_POETRY_DAT;
  }
  TryLoadIndex();

#ifdef wx_msw
  Corner1 = new wxIcon("icon_1");
  Corner2 = new wxIcon("icon_2");
  Corner3 = new wxIcon("icon_3");
  Corner4 = new wxIcon("icon_4");
#endif

  TheMainWindow->GetIndexLoadPoem();
  TheMainWindow->Resize();
  TheMainWindow->Show(TRUE);

  return TheMainWindow;
}

Bool MainWindow::OnClose(void)
{
  WritePreferences();
  return TRUE;
}

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxFrame *frame, int x, int y, int w, int h):
 wxCanvas(frame, x, y, w, h, 0)
{
}

// Define the repainting behaviour
void MyCanvas::OnPaint(void)
{
  if (backingBitmap)
  {
    int xx, yy;
    TheMainWindow->GetClientSize(&xx, &yy);
    
    BeginDrawing();

    wxMemoryDC memDC(GetDC());
    memDC.SelectObject(backingBitmap);
    GetDC()->Blit(0, 0,
      backingBitmap->GetWidth(), backingBitmap->GetHeight(), &memDC, 0, 0);
      
    EndDrawing();
  }
}

void MyCanvas::OnEvent(wxMouseEvent& event)
{
  float x, y;
  event.Position(&x, &y);
  static int startPosX, startPosY, startFrameX, startFrameY;

  event.Position(&x, &y);

  if (event.RightDown())
  {
    // Versions from wxWin 1.67 are probably OK
#if 0 // wx_motif
    FakePopupMenu(popupMenu, x, y);
#else
    PopupMenu(popupMenu, x, y);
#endif
  }
  else if (event.LeftDown())
  {
    this->CaptureMouse();
    int x1 = (int)x;
    int y1 = (int)y;
    ClientToScreen(&x1, &y1);
    startPosX = x1;
    startPosY = y1;
    GetParent()->GetPosition(&startFrameX, &startFrameY);
  }
  else if (event.LeftUp())
    this->ReleaseMouse();
  else if (event.Dragging() && event.LeftIsDown())
  {
    int x1 = (int)x;
    int y1 = (int)y;
    ClientToScreen(&x1, &y1);
    
    int dX = x1 - startPosX;
    int dY = y1 - startPosY;
    GetParent()->Move(startFrameX + dX, startFrameY + dY);
  }
}

// Load index file
int LoadIndex(char *file_name)
{
    long data;
    FILE *index_file;

    int i = 0;
    char buf[100];

    if (file_name)
      sprintf(buf, "%s.idx", file_name);
    if (! (file_name && (index_file = fopen(buf, "r"))))
      return 0;
    else
    {
      fscanf(index_file, "%ld", &nitems);

      for (i = 0; i < nitems; i++)
      {
        fscanf(index_file, "%ld", &data);
        poem_index[i] = data;
      }
      fclose(index_file);

      return 1;
    }
}

// Get index
int GetIndex()
{
    int indexn = 0;

    indexn = (int)(rand() % nitems);

    if ((indexn < 0) || (indexn > nitems))
    { PoetryError("No such poem!");
      return -1;
    }
    else
      return indexn;
}

// Read preferences
void ReadPreferences()
{
  wxGetResource("wxPoem", "FontSize", &pointSize);
  wxGetResource("wxPoem", "X", &XPos);
  wxGetResource("wxPoem", "Y", &YPos);
}

// Write preferences to disk
void WritePreferences()
{
#ifdef wx_msw
  TheMainWindow->GetPosition(&XPos, &YPos);
  wxWriteResource("wxPoem", "FontSize", pointSize);
  wxWriteResource("wxPoem", "X", XPos);
  wxWriteResource("wxPoem", "Y", YPos);
#endif
}

// Load a poem from given file, at given point in file.
// If position is > -1, use this for the position in the
// file, otherwise use index[index_ptr] to find the correct position.
Bool LoadPoem(char *file_name, long position)
{
    int ch = 0;
    int i = 0;
    int j = 0;
    int indexn = 0;
    char buf[100];
    long data;
    FILE *data_file;

    paging = FALSE;
    current_page = 0;

    if (file_name)
      sprintf(buf, "%s.dat", file_name);

    if (! (file_name && (data_file = fopen(buf, "r"))))
    {
      sprintf(error_buf, "Data file %s not found.", buf);
      PoetryError(error_buf);
      return FALSE;
    }
    else
    {
      if (position > -1)
        data = position;
      else
        data = poem_index[index_ptr];

      fseek(data_file, data, SEEK_SET);

      ch = 0;
      i = 0;
      while ((ch != EOF) && (ch != '#'))
      {
        ch = getc(data_file);
        // Add a linefeed so it will copy to the clipboard ok
        if (ch == 10)
        {
          poem_buffer[i] = 13;
          i++;
        }

        poem_buffer[i] = ch;
        i ++;

        if (i == buf_size)
        {
           sprintf(error_buf, "%s", "Poetry buffer exceeded.");
           PoetryError(error_buf);
           return FALSE;
        }
      }
      fclose(data_file);
      poem_buffer[i-1] = 0;
      return TRUE;
  }
}

// Do the search
long MainWindow::DoSearch(void)
{
    if (!search_string)
      return FALSE;

    FILE *file;
    long i = 0;
    int ch = 0;
    char buf[100];
    long find_start;
    long previous_poem_start;

    Bool found = FALSE;
    int search_length = strlen(search_string);

    if (same_search)
    {
      find_start = last_find + 1;
      previous_poem_start = last_poem_start;
    }
    else
    {
      find_start = 0;
      last_poem_start = 0;
      previous_poem_start = -1;
    }

    if (data_filename)
      sprintf(buf, "%s.dat", data_filename);

    if (! (data_filename && (file = fopen(buf, "r"))))
    {
      sprintf(error_buf, "Poetry data file %s not found\n", buf);
      PoetryError(error_buf);
      return FALSE;
    }

    fseek(file, find_start, SEEK_SET);

    while ((ch != EOF) && !found)
    {
        ch = getc(file);
        ch |= 0x0020;   // Make lower case

        // Only match if we're looking at a different poem
        // (no point in displaying the same poem again)
        if ((ch == search_string[i]) && (last_poem_start != previous_poem_start))
        {
          if (i == 0)
            last_find = ftell(file);
          if (i == search_length-1)
            found = TRUE;
          i ++;
        }
        else
          i = 0;

        if (ch == '#')
        {
            ch = getc(file);
              last_poem_start = ftell(file);
        }
    }
    fclose(file);
    if (ch == EOF)
      last_find = -1;

    if (found)
    {
      return last_poem_start;
    }
    else
      return -1;
}

// Set up poetry filenames, preferences, load the index
// Load index (or compile it if none found)
void TryLoadIndex()
{
  index_ok = LoadIndex(index_filename);
  if (!index_ok || (nitems == 0))
  {
      PoetryError("Index file not found; will compile new one", "wxPoem");
      index_ok = Compile();
  }
}

// Error message
void PoetryError(char *msg, char *caption)
{
  wxMessageBox(msg, caption, wxOK|wxICON_EXCLAMATION);
}

// Notification (change icon to something appropriate!)
void PoetryNotify(char *Msg, char *caption)
{
  wxMessageBox(Msg, caption, wxOK | wxICON_INFORMATION);
}

// Build up and save an index into the poetry data file, for
// rapid random access
Bool Compile(void)
{
    FILE *file;
    long i = 0;
    int j;
    int ch = 0;
    char buf[100];

    if (data_filename)
      sprintf(buf, "%s.dat", data_filename);

    if (! (data_filename && (file = fopen(buf, "r"))))
    {
      sprintf(error_buf, "Poetry data file %s not found\n", buf);
      PoetryError(error_buf);
      return FALSE;
    }

    nitems = 0;

    // Do first one (?)
    poem_index[nitems] = 0;
    nitems ++;

    // Do rest
    while (ch != EOF)
    {
        ch = getc(file);
        i ++;
        if (ch == '#')
        {
            ch = getc(file);
            long data;
            data = ftell(file);
            poem_index[nitems] = data;
            nitems ++;
        }
    }
    fclose(file);

    if (index_filename)
      sprintf(buf, "%s.idx", index_filename);
    if (! (data_filename && (file = fopen(buf, "w"))))
    {
      sprintf(error_buf, "Poetry index file %s cannot be created\n", buf);
      PoetryError(error_buf);
      return FALSE;
    }

    fprintf(file, "%ld\n\n", nitems);
    for (j = 0; j < nitems; j++)
      fprintf(file, "%ld\n", poem_index[j]);
 
    fclose(file);
    PoetryNotify("Poetry index compiled.");
    return TRUE;
}

void PopupFunction(wxMenu& menu, wxCommandEvent& event)
{
  switch (event.commandInt)
  {
     case POEM_NEXT:
       // Another poem/page
       TheMainWindow->NextPage();
       break;
     case POEM_PREVIOUS:
       // Previous page
       TheMainWindow->PreviousPage();
       break;
     case POEM_SEARCH:
       // Search - with dialog
       TheMainWindow->Search(TRUE);
       break;
     case POEM_NEXT_MATCH:
       // Search - without dialog (next match)
       TheMainWindow->Search(FALSE);
       break;
     case POEM_MINIMIZE:
       TheMainWindow->Iconize(TRUE);
       break;
#ifdef wx_msw
     case POEM_COPY:
       // Copy current poem to the clipboard
       CopyToClipboard(TheMainWindow->GetHWND(), poem_buffer);
       break;
#endif
     case POEM_COMPILE:
       // Compile index
       Compile();
       break;
     case POEM_BIGGER_TEXT:
     {
       pointSize ++;
       CreateFonts();
       TheMainWindow->Resize();
       break;
     }
     case POEM_SMALLER_TEXT:
     {
       if (pointSize > 2)
       {
         pointSize --;
         CreateFonts();
         TheMainWindow->Resize();
       }
       break;
     }
     case POEM_HELP_CONTENTS:
     {
       HelpInstance->LoadFile("wxpoem");
       HelpInstance->DisplayContents();
       break;
     }
     case POEM_ABOUT:
     {
       (void)wxMessageBox("wxPoem Version 1.1\nJulian Smart (c) 1995",
                          "About wxPoem", wxOK, TheMainWindow);
       break;
     }
     case POEM_EXIT:
       // Exit
       if (TheMainWindow->OnClose())
         delete TheMainWindow;
       break;
     default:
       break;
  }
}
