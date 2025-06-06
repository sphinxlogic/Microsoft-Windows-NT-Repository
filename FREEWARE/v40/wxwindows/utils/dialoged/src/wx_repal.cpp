/*
 * File:	wx_repal.cc
 * Purpose:	Resource editor palette
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if defined(wx_msw) && !defined(GNUWIN32)
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "wx_resed.h"
#include "wx_repal.h"

// Bitmaps for palette
wxBitmap *PaletteFrameBitmap = NULL;
wxBitmap *PaletteDialogBoxBitmap = NULL;
wxBitmap *PalettePanelBitmap = NULL;
wxBitmap *PaletteCanvasBitmap = NULL;
wxBitmap *PaletteTextWindowBitmap = NULL;
wxBitmap *PaletteMessageBitmap = NULL;
wxBitmap *PaletteButtonBitmap = NULL;
wxBitmap *PaletteCheckBoxBitmap = NULL;
wxBitmap *PaletteListBoxBitmap = NULL;
wxBitmap *PaletteRadioBoxBitmap = NULL;
wxBitmap *PaletteChoiceBitmap = NULL;
wxBitmap *PaletteTextBitmap = NULL;
wxBitmap *PaletteMultiTextBitmap = NULL;
wxBitmap *PaletteSliderBitmap = NULL;
wxBitmap *PaletteArrowBitmap = NULL;
wxBitmap *PaletteGroupBitmap = NULL;
wxBitmap *PaletteGaugeBitmap = NULL;
wxBitmap *PalettePictureBitmap = NULL;
wxBitmap *PaletteBitmapButtonBitmap = NULL;
wxBitmap *PaletteScrollbarBitmap = NULL;

#ifdef wx_x
#include "bitmaps/frame.xbm"
#include "bitmaps/dialog.xbm"
#include "bitmaps/panel.xbm"
#include "bitmaps/canvas.xbm"
#include "bitmaps/textsw.xbm"
#include "bitmaps/message.xbm"
#include "bitmaps/button.xbm"
#include "bitmaps/check.xbm"
#include "bitmaps/listbox.xbm"
#include "bitmaps/radio.xbm"
#include "bitmaps/choice.xbm"
#include "bitmaps/text.xbm"
#include "bitmaps/mtext.xbm"
#include "bitmaps/slider.xbm"
#include "bitmaps/arrow.xbm"
#include "bitmaps/group.xbm"
#include "bitmaps/gauge.xbm"
#include "bitmaps/scroll.xbm"
#include "bitmaps/picture.xbm"
#include "bitmaps/bmpbuttn.xbm"
#endif

/*
 * Object editor tool palette
 *
 */

EditorToolPalette::EditorToolPalette(wxResourceManager *manager, wxFrame *frame, int x, int y, int w, int h,
            long style, int direction, int RowsOrColumns):
  TOOLPALETTECLASS(frame, x, y, w, h, style, direction, RowsOrColumns)
{
  currentlySelected = -1;
  resourceManager = manager;
}

Bool EditorToolPalette::OnLeftClick(int toolIndex, Bool toggled)
{
  // BEGIN mutual exclusivity code
  if (toggled && (currentlySelected != -1) && (toolIndex != currentlySelected))
    ToggleTool(currentlySelected, FALSE);

  if (toggled)
    currentlySelected = toolIndex;
  else if (currentlySelected == toolIndex)
    currentlySelected = -1;
  //  END mutual exclusivity code
/*
  if (MainFrame)
  {
    if (toggled && (toolIndex != PALETTE_ARROW))
      MainFrame->canvas->SetCursor(crossCursor);
    else
      MainFrame->canvas->SetCursor(handCursor);
  }
*/

  return TRUE;
}

void EditorToolPalette::OnMouseEnter(int toolIndex)
{
  if (!resourceManager) return;
  wxFrame *managerFrame = resourceManager->GetEditorFrame();
  
  if (toolIndex > -1)
  {
      switch (toolIndex)
      {
        case PALETTE_FRAME:
          managerFrame->SetStatusText("wxFrame");
          break;
        case PALETTE_DIALOG_BOX:
          managerFrame->SetStatusText("wxDialogBox");
          break;
        case PALETTE_PANEL:
          managerFrame->SetStatusText("wxPanel");
          break;
        case PALETTE_CANVAS:
          managerFrame->SetStatusText("wxCanvas");
          break;
        case PALETTE_TEXT_WINDOW:
          managerFrame->SetStatusText("wxTextWindow");
          break;
        case PALETTE_BUTTON:
          managerFrame->SetStatusText("wxButton");
          break;
        case PALETTE_MESSAGE:
          managerFrame->SetStatusText("wxMessage");
          break;
        case PALETTE_TEXT:
          managerFrame->SetStatusText("wxText");
          break;
        case PALETTE_MULTITEXT:
          managerFrame->SetStatusText("wxMultiText");
          break;
        case PALETTE_CHOICE:
          managerFrame->SetStatusText("wxChoice");
          break;
        case PALETTE_CHECKBOX:
          managerFrame->SetStatusText("wxCheckBox");
          break;
        case PALETTE_RADIOBOX:
          managerFrame->SetStatusText("wxRadioBox");
          break;
        case PALETTE_LISTBOX:
          managerFrame->SetStatusText("wxListBox");
          break;
        case PALETTE_SLIDER:
          managerFrame->SetStatusText("wxSlider");
          break;
        case PALETTE_GROUPBOX:
          managerFrame->SetStatusText("wxGroupBox");
          break;
        case PALETTE_GAUGE:
          managerFrame->SetStatusText("wxGauge");
          break;
        case PALETTE_BITMAP_MESSAGE:
          managerFrame->SetStatusText("Bitmap wxMessage");
          break;
        case PALETTE_BITMAP_BUTTON:
          managerFrame->SetStatusText("Bitmap wxButton");
          break;
        case PALETTE_SCROLLBAR:
          managerFrame->SetStatusText("wxScrollBar");
          break;
        case PALETTE_ARROW:
          managerFrame->SetStatusText("Pointer");
          break;
      }
  }
  else managerFrame->SetStatusText("");
}

void EditorToolPalette::OnPaint(void)
{
  BeginDrawing();
  
  TOOLPALETTECLASS::OnPaint();

  int w, h;
  GetSize(&w, &h);
  wxDC *dc = GetDC();
  dc->SetPen(wxBLACK_PEN);
  dc->SetBrush(wxTRANSPARENT_BRUSH);
  DrawLine(0, h-1, w, h-1);
  
  EndDrawing();
}

void EditorToolPalette::SetSize(int x, int y, int width, int height, int sizeFlags)
{
  TOOLPALETTECLASS::SetSize(x, y, width, height, sizeFlags);
}

Bool wxResourceManager::InitializeTools(void)
{
  // Load palette bitmaps
#ifdef wx_msw
  PaletteFrameBitmap = new wxBitmap("FRAMETOOL");
  PaletteDialogBoxBitmap = new wxBitmap("DIALOGBOXTOOL");
  PalettePanelBitmap = new wxBitmap("PANELTOOL");
  PaletteCanvasBitmap = new wxBitmap("CANVASTOOL");
  PaletteTextWindowBitmap = new wxBitmap("TEXTWINDOWTOOL");
  PaletteMessageBitmap = new wxBitmap("MESSAGETOOL");
  PaletteButtonBitmap = new wxBitmap("BUTTONTOOL");
  PaletteCheckBoxBitmap = new wxBitmap("CHECKBOXTOOL");
  PaletteListBoxBitmap = new wxBitmap("LISTBOXTOOL");
  PaletteRadioBoxBitmap = new wxBitmap("RADIOBOXTOOL");
  PaletteChoiceBitmap = new wxBitmap("CHOICETOOL");
  PaletteTextBitmap = new wxBitmap("TEXTTOOL");
  PaletteMultiTextBitmap = new wxBitmap("MULTITEXTTOOL");
  PaletteSliderBitmap = new wxBitmap("SLIDERTOOL");
  PaletteArrowBitmap = new wxBitmap("ARROWTOOL");
  PaletteGroupBitmap = new wxBitmap("GROUPTOOL");
  PaletteGaugeBitmap = new wxBitmap("GAUGETOOL");
  PalettePictureBitmap = new wxBitmap("PICTURETOOL");
  PaletteBitmapButtonBitmap = new wxBitmap("BMPBUTTONTOOL");
  PaletteScrollbarBitmap = new wxBitmap("SCROLLBARTOOL");
#endif
#ifdef wx_x
  PaletteFrameBitmap = new wxBitmap(frame_bits, frame_width, frame_height);
  PaletteDialogBoxBitmap = new wxBitmap(dialog_bits, dialog_width, dialog_height);
  PalettePanelBitmap = new wxBitmap(panel_bits, panel_width, panel_height);
  PaletteCanvasBitmap = new wxBitmap(canvas_bits, canvas_width, canvas_height);
  PaletteTextWindowBitmap = new wxBitmap(textsw_bits, textsw_width, textsw_height);
  PaletteMessageBitmap = new wxBitmap(message_bits, message_width, message_height);
  PaletteButtonBitmap = new wxBitmap(button_bits, button_width, button_height);
  PaletteCheckBoxBitmap = new wxBitmap(check_bits, check_width, check_height);
  PaletteListBoxBitmap = new wxBitmap(listbox_bits, listbox_width, listbox_height);
  PaletteRadioBoxBitmap = new wxBitmap(radio_bits, radio_width, radio_height);
  PaletteChoiceBitmap = new wxBitmap(choice_bits, choice_width, choice_height);
  PaletteTextBitmap = new wxBitmap(text_bits, text_width, text_height);
  PaletteMultiTextBitmap = new wxBitmap(mtext_bits, mtext_width, mtext_height);
  PaletteSliderBitmap = new wxBitmap(slider_bits, slider_width, slider_height);
  PaletteArrowBitmap = new wxBitmap(arrow_bits, arrow_width, arrow_height);
  PaletteGroupBitmap = new wxBitmap(group_bits, group_width, group_height);
  PaletteGaugeBitmap = new wxBitmap(gauge_bits, gauge_width, gauge_height);
  PalettePictureBitmap = new wxBitmap(picture_bits, picture_width, picture_height);
  PaletteBitmapButtonBitmap = new wxBitmap(bmpbuttn_bits, bmpbuttn_width, bmpbuttn_height);
  PaletteScrollbarBitmap = new wxBitmap(scroll_bits, scroll_width, scroll_height);
#endif
  return TRUE;
}

EditorToolPalette *wxResourceManager::OnCreatePalette(wxFrame *parent)
{
  EditorToolPalette *palette = new EditorToolPalette(this, parent, 0, 0, -1, -1, 0, // wxTB_3DBUTTONS,
                                        wxVERTICAL, 1);
  
  palette->SetMargins(2, 2);
#ifdef wx_msw
  if (palette->IsKindOf(CLASSINFO(wxButtonBar)))
    ((wxButtonBar *)palette)->SetDefaultSize(22, 22);
#endif
  palette->GetDC()->SetBackground(wxLIGHT_GREY_BRUSH);
  palette->AddTool(PALETTE_ARROW, PaletteArrowBitmap, NULL, TRUE, 0, -1, NULL, "Pointer");
/*
  palette->AddTool(PALETTE_FRAME, PaletteFrameBitmap, NULL, TRUE, 0, -1, NULL, "wxFrame");
  palette->AddTool(PALETTE_DIALOG_BOX, PaletteDialogBoxBitmap, NULL, TRUE, 0, -1, NULL, "wxDialogBox");
  palette->AddTool(PALETTE_PANEL, PalettePanelBitmap, NULL, TRUE, 0, -1, NULL, "wxPanel");
  palette->AddTool(PALETTE_CANVAS, PaletteCanvasBitmap, NULL, TRUE, 0, -1, NULL, "wxCanvas");
  palette->AddTool(PALETTE_TEXT_WINDOW, PaletteTextWindowBitmap, NULL, TRUE, 0, -1, NULL, "wxTextWindow");

*/
  palette->AddTool(PALETTE_MESSAGE, PaletteMessageBitmap, NULL, TRUE, 0, -1, NULL, "wxMessage");
  palette->AddTool(PALETTE_BITMAP_MESSAGE, PalettePictureBitmap, NULL, TRUE, 0, -1, NULL, "Picture wxMessage");
  palette->AddTool(PALETTE_BUTTON, PaletteButtonBitmap, NULL, TRUE, 0, -1, NULL, "Button");
  palette->AddTool(PALETTE_BITMAP_BUTTON, PaletteBitmapButtonBitmap, NULL, TRUE, 0, -1, NULL, "Picture wxButton");
  palette->AddTool(PALETTE_CHECKBOX, PaletteCheckBoxBitmap, NULL, TRUE, 0, -1, NULL, "wxCheckBox");
  palette->AddTool(PALETTE_RADIOBOX, PaletteRadioBoxBitmap, NULL, TRUE, 0, -1, NULL, "wxRadioBox");
  palette->AddTool(PALETTE_LISTBOX, PaletteListBoxBitmap, NULL, TRUE, 0, -1, NULL, "wxListBox");
  palette->AddTool(PALETTE_CHOICE, PaletteChoiceBitmap, NULL, TRUE, 0, -1, NULL, "wxChoice");
  palette->AddTool(PALETTE_TEXT, PaletteTextBitmap, NULL, TRUE, 0, -1, NULL, "wxText");
  palette->AddTool(PALETTE_MULTITEXT, PaletteMultiTextBitmap, NULL, TRUE, 0, -1, NULL, "wxMultiText");
  palette->AddTool(PALETTE_SLIDER, PaletteSliderBitmap, NULL, TRUE, 0, -1, NULL, "wxSlider");
  palette->AddTool(PALETTE_GROUPBOX, PaletteGroupBitmap, NULL, TRUE, 0, -1, NULL, "wxGroupBox");
  palette->AddTool(PALETTE_GAUGE, PaletteGaugeBitmap, NULL, TRUE, 0, -1, NULL, "wxGauge");
  palette->AddTool(PALETTE_SCROLLBAR, PaletteScrollbarBitmap, NULL, TRUE, 0, -1, NULL, "wxScrollBar");

  palette->Layout();
  palette->CreateTools();
  
  palette->ToggleTool(PALETTE_ARROW, TRUE);
  palette->currentlySelected = PALETTE_ARROW;
  return palette;
}

