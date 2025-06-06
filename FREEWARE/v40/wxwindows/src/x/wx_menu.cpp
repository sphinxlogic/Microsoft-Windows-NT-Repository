/*
 * File:        wx_menu.cc
 * Purpose:     Menu and menu bar implementation (X version)
 * Author:      Julian Smart
 * Created:     1993
 * Updated:	April 1995
 * Copyright:   (c) 1995, AIAI, University of Edinburgh
 */


/* 
   The followng define implements a fix for the Motif popup menu problems
   by Nick Liebmann (badboy@jungle.uk.eu.org).  26/02/97

   Re-fixed by NL on 02/03/97
   Fixed incorrect behaviour on separators and menu titles, and menu border
*/

#define USE_NL_POPUP_MENU_FIX

/*
Brief summary of fix:

  Problem -  Sometimes motif popups were appearing only once and then not 
	     again.

  Cause   -  Popup menus seem to grab the right mouse button for the duration
	     of their existance, a scheme was setup whereby popup menus
	     would be deleted (and thus release the right button) the 
	     next time X was waiting for input (thats what a WorkProc does
	     apparently). Unfortunately a program that uses wxYield() in 
	     a loop, to keep the interface alive, may never give X a chance 
	     to wait for input, and so the WorkProc may never be called.

  Solution - The immediate solution is to delete the menu earlier.
	     If the user clicks on a menu item it is deleted in 
	     wxMenuItemCallback otherwise it is deleted in 
	     wxMenuPopdownCallback. 
	     Unfortunately I have to calculate whether a menu item is selected
	     or not by looking at the co-ordinates given in the XEvent, and
	     using wxMenu::GetSize()/ GetPosition(). This may prove to be 
	     unreliable on systems where popup menus have decrative borders - 
	     I dont know if any do.
 
	     A better solution might be to force WorkProcs to be executed
	     from wxYield() - I could not find a way of doing this, but 
	     I'm not an Xpert.....
*/


#ifdef __GNUG__
#pragma implementation
#pragma implementation "wx_menu.h"
#pragma implementation "wx_mnuit.h"
#endif

#include <stdlib.h>

//Added by JOREL Jean-Charles <jjorel@silr.ireste.fr>
#include "wx_main.h"

#include "wx_defs.h"
#include "wx_utils.h"
#include "wx_frame.h"
#include "wx_privt.h"
#include "wx_menu.h"

#ifdef wx_motif
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/CascadeBG.h>
#include <Xm/CascadeB.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/RowColumn.h>

// Convenience dialogs, only needed for FakePopupMenu
#include "wx_dialg.h"

void wxMenuItemCallback (Widget w, XtPointer clientData,
			 XtPointer ptr);
void wxMenuItemArmCallback (Widget w, XtPointer clientData,
			    XtPointer ptr);
void wxMenuItemDisarmCallback (Widget w, XtPointer clientData,
			       XtPointer ptr);
#endif

#ifdef wx_xview
#include <xview/openmenu.h>

// Menu notifier proc
void wxMenuProc (Menu x_menu, Menu_item menu_item);
void wxMenuBarProc (Menu x_menu, Menu_item menu_item);
#endif

// Menus

// Construct a menu with optional title (then use append)
// IMPLEMENT_DYNAMIC_CLASS(wxMenu, wxWindow)
// IMPLEMENT_DYNAMIC_CLASS(wxMenuItem, wxObject)

wxMenu::wxMenu (char *Title, wxFunction func): wxbMenu (Title, func)
{
  no_items = 0;
  menu_bar = NULL;
  menuItems.DeleteContents (TRUE);
#ifdef wx_motif
  ownedByMenuBar = FALSE;
  numColumns = 1;
  buttonWidget = NULL;
  handle = NULL;
  top_menu = NULL;
  WXGC_IGNORE(top_menu);
  menuId = 0;

  if (title)
    {
      Append (-2, title);
      AppendSeparator ();
    }
#endif
#ifdef wx_xview
  panelItem = NULL;
  Menu x_menu = (Menu) xv_create ((Xv_opaque) NULL, MENU, MENU_CLIENT_DATA, (char *) this, NULL);
  if (title)
    {
      title = copystring (title);
      xv_set (x_menu, MENU_TITLE_ITEM, title, NULL);
    }
  handle = (char *) x_menu;
  top_level_menu = this;
#endif // wx_xview
  Callback (func);
}

// The wxWindow destructor will take care of deleting the submenus.

wxMenu::~wxMenu (void)
{
#ifdef wx_motif
  if (handle) {
//    if (!window_parent)
//      DestroyMenu();
      if (window_parent)
        DestroyMenu(TRUE);
      else
        DestroyMenu(FALSE);
    handle = NULL;
  }

  DestroyChildren();

//  if (window_parent && wxSubType(window_parent->__type, wxTYPE_MENU_BAR))
//    window_parent = NULL;
  if (window_parent && ownedByMenuBar)
    window_parent = NULL;
#endif

#ifdef wx_xview
  /* Uninstalled menu needs to be destroyed */
  if (!window_parent && !panelItem) {
    /* Need to delete self */
    xv_destroy_safe((Xv_opaque)handle);
  }
#endif
}

void wxMenu:: Break (void)
{
#ifdef wx_motif
  numColumns++;
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;
  xv_set (menu, MENU_NCOLS, xv_get (menu, MENU_NCOLS) + 1, NULL);
#endif
}

// Ordinary menu item
void wxMenu:: Append (int Id, char *Label, char *helpString, Bool checkable)
{
  wxMenuItem *item = new wxMenuItem;
  item->checkable = checkable;
  item->itemId = Id;
  item->itemName = copystring (Label);
  item->subMenu = NULL;
  if (helpString)
    item->helpString = copystring (helpString);

  menuItems.Append (item);

#ifdef wx_motif
  item->buttonWidget = NULL;
  if (handle)
    item->CreateItem ((Widget) handle, menu_bar, top_menu);	// this is a dynamic Append
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;
  wxStripMenuCodes (Label, wxBuffer);
  item->uncheckedString = copystring (wxBuffer);

  Menu_item mi = (Menu_item) xv_create ((Xv_opaque) NULL, MENUITEM,
					MENU_RELEASE,
					MENU_STRING, item->uncheckedString,
					MENU_NOTIFY_PROC, wxMenuProc,
					MENU_CLIENT_DATA, (char *) item,
					NULL);

  if (mi)
    xv_set (menu, MENU_APPEND_ITEM, mi, NULL);
  if (mi && checkable)
    {
      char *checked = new char[strlen (item->uncheckedString) + 2];
      sprintf (checked, "%s*", item->uncheckedString);
      item->checkedString = checked;
    }
#endif // wx_xview
  no_items++;
}

void wxMenu:: AppendSeparator (void)
{
  wxMenuItem *item = new wxMenuItem;
  item->itemId = -1;
  item->checkable = FALSE;
  menuItems.Append (item);

#ifdef wx_motif
  item->buttonWidget = NULL;
  if (handle)
    item->CreateItem ((Widget) handle, menu_bar, top_menu);	// this is a dynamic Append
#endif
#ifdef wx_xview
/* HOW DO WE GET A SEPARATOR IN XVIEW?!
 * This makes far too much space.
 */

  Menu menu = (Menu) handle;
  Menu_item mi = (Menu_item) xv_create ((Xv_opaque) NULL,
					MENUITEM_SPACE,
					NULL);

  if (mi)
    xv_set (menu, MENU_APPEND_ITEM, mi, NULL);

/***
This was a try to get a line separator, using private attributes.
But that doesn't works...
  Menu menu = (Menu)handle ;
  int n = xv_get(menu,MENU_NITEMS) ;
  Menu_item mi = xv_get(menu,MENU_NTH_ITEM,n) ;
  xv_set(mi,MENU_LINE_AFTER_ITEM,MENU_HORIZONTAL_LINE,NULL) ;
***/

#endif
}

// Pullright item
void wxMenu:: Append (int Id, char *Label, wxMenu * SubMenu, char *helpString)
{
  if (SubMenu->window_parent)
    return;

  SubMenu->top_level_menu = top_level_menu;
  SubMenu->window_parent = this;
  children->Append (SubMenu);	// Store submenu for later deletion

  wxMenuItem *item = new wxMenuItem;
  item->checkable = FALSE;
  item->itemId = Id;
  item->itemName = copystring (Label);
  if (helpString)
    item->helpString = copystring (helpString);

  item->subMenu = SubMenu;
  menuItems.Append (item);

#ifdef wx_motif
  item->buttonWidget = NULL;
  if (handle)
    item->CreateItem ((Widget) handle, menu_bar, top_menu);	// this is a dynamic Append
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;
  wxStripMenuCodes (Label, wxBuffer);

  Menu x_submenu = (Menu) (SubMenu->GetHandle ());
  item->uncheckedString = copystring (wxBuffer);
  Menu_item mi = (Menu_item) xv_create ((Xv_opaque) NULL, MENUITEM,
					MENU_RELEASE,
					MENU_STRING, item->uncheckedString,
					MENU_NOTIFY_PROC, wxMenuProc,
					MENU_PULLRIGHT, x_submenu,
					MENU_CLIENT_DATA, (char *) item,
					NULL);
  if (mi)
    xv_set (menu, MENU_APPEND_ITEM, mi, NULL);

#endif // wx_xview

  no_items++;
}

void wxMenu::Delete(int Id)
{
  wxMenuItem *item = NULL;
  wxNode *node;
  int pos;

  for (pos = 0, node = menuItems.First(); node; node = node->Next(), pos++) {
    item = (wxMenuItem *)node->Data();
    if (item->itemId == Id)
      break;
  }

  if (!node)
    return;

#ifdef wx_motif
  item->DestroyItem(TRUE);
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;

  xv_set(menu, MENU_REMOVE, pos + 1, NULL);
#endif

  if (item->subMenu) {
    item->subMenu->top_level_menu = item->subMenu;
    item->subMenu->window_parent = NULL;
    children->DeleteObject(item->subMenu);
  }

  menuItems.DeleteObject(item);
}

void wxMenu:: Enable (int Id, Bool Flag)
{
#ifdef wx_motif
  wxMenuItem *it = NULL;
  Widget w = FindMenuItem (Id, &it);

  if (it)
    it->isEnabled = Flag;

  if (w)
    {
      if (w)
	XtSetSensitive (w, Flag);
      return;
    }
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;
  int n = (int) xv_get (menu, MENU_NITEMS);
  int i;
  for (i = 1; i <= n; i++)
    {
      Menu_item item = FindMenuItem (Id);
      if (item)
	{
	  xv_set (item, MENU_INACTIVE, !Flag, NULL);
	}
    }
#endif
}

void wxMenu:: Check (int Id, Bool Flag)
{
#ifdef wx_motif
  wxMenuItem *it = NULL;
  Widget w = FindMenuItem (Id, &it);
  if (it)
    it->isChecked = Flag;

  if (w && XtIsSubclass (w, xmToggleButtonGadgetClass))
    {
      XtVaSetValues (w, XmNset, (Boolean) Flag, NULL);
    }
#endif
#ifdef wx_xview
  // Simulate checking in XView
  Menu menu = (Menu) handle;
  int n = (int) xv_get (menu, MENU_NITEMS);
  int i;
  for (i = 1; i <= n; i++)
    {
      Menu_item item = FindMenuItem (Id);
      if (item)
	{
          wxMenuItem *menuItem = (wxMenuItem *) xv_get(item, MENU_CLIENT_DATA);
          if (menuItem && menuItem->checkable)
          {
            menuItem->isChecked = Flag;
            char *str = (Flag ? menuItem->checkedString : menuItem->uncheckedString);
	      
            xv_set (item, MENU_STRING, str, NULL);
	  }
          return;
	}
    }
#endif
}

Bool wxMenu:: Checked (int Id)
{
  wxMenuItem *it = FindItemForId (Id);
  if (it)
    return it->isChecked;
  else
    return FALSE;
}

void wxMenu:: SetTitle (char *label)
{
  if (title)
    delete[] title;
  if (label)
    title = copystring (label);
  else
    title = copystring (" ");

#ifdef wx_motif
  wxNode *node = menuItems.First ();
  if (!node)
    return;
  wxMenuItem *item = (wxMenuItem *) node->Data ();
  Widget widget = item->buttonWidget;
  if (!widget)
    return;

  XmString title_str = XmStringCreateSimple (title);
  XtVaSetValues (widget,
                 XmNlabelString, title_str,
                 NULL);
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;
  xv_set (menu, MENU_TITLE_ITEM, title, NULL);
#endif
}

char *wxMenu:: GetTitle ()
{
  return title;
}

void wxMenu:: SetLabel (int Id, char *label)
{
#ifdef wx_motif
  char mnem = wxFindMnemonic (label);
#endif
  wxStripMenuCodes (label, wxBuffer);
#ifdef wx_motif
  wxMenuItem *it = NULL;
  Widget w = FindMenuItem (Id, &it);
  if (w)
    {
      XmString label_str = XmStringCreateSimple (wxBuffer);
      XtVaSetValues (w,
                     XmNlabelString, label_str,
                     NULL);
      XmStringFree (label_str);
      if (mnem != 0)
        XtVaSetValues (w, XmNmnemonic, mnem, NULL);
      char *accel = wxFindAccelerator (label);
      if (accel)
        XtVaSetValues (w, XmNaccelerator, accel, NULL);

      XmString accel_str = wxFindAcceleratorText (label);
      if (accel_str)
        {
          XtVaSetValues (w, XmNacceleratorText, accel_str, NULL);
          XmStringFree (accel_str);
        }
    }
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;
  int n = (int) xv_get (menu, MENU_NITEMS);
  int i;
  for (i = 1; i <= n; i++)
    {
      Menu_item item = FindMenuItem (Id);
      if (item)
        {
          wxMenuItem *menuItem = (wxMenuItem *)xv_get(item, MENU_CLIENT_DATA);
          
          if (menuItem && menuItem->checkable)
            {
              if (menuItem->checkedString)
                delete[] menuItem->checkedString;
              if (menuItem->uncheckedString)
                delete[] menuItem->uncheckedString;

              menuItem->uncheckedString = copystring (wxBuffer);
              menuItem->checkedString = strcpy (new char[strlen (wxBuffer) + 2], wxBuffer);
              strcat (menuItem->checkedString, "*");

              if (menuItem->isChecked)
                xv_set (item, MENU_STRING, menuItem->checkedString, NULL);
              else
                xv_set (item, MENU_STRING, menuItem->uncheckedString, NULL);
            }
          else if (menuItem && !menuItem->checkable)
            {
              if (menuItem->uncheckedString)
                delete[] menuItem->uncheckedString;
                
              menuItem->uncheckedString = copystring (wxBuffer);
	      xv_set (item, MENU_STRING, menuItem->uncheckedString, NULL);
	    }
	}
    }
#endif
}

char *wxMenu:: GetLabel (int Id)
{
#ifdef wx_motif
  wxMenuItem *it = NULL;
  Widget w = FindMenuItem (Id, &it);
  if (w)
    {
      XmString text;
      char *s;
      XtVaGetValues (w,
		     XmNlabelString, &text,
		     NULL);

      if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
	{
	  strcpy (wxBuffer, s);
	  XtFree (s);
	  return wxBuffer;
	}
      else
	{
	  XmStringFree (text);
	  return NULL;
	}
    }
  else
    return (NULL);
#endif
#ifdef wx_xview
  Menu menu = (Menu) handle;
  int n = (int) xv_get (menu, MENU_NITEMS);
  int i;
  for (i = 1; i <= n; i++)
    {
      Menu_item item = FindMenuItem (Id);
      if (item)
	{
          wxMenuItem *menuItem = (wxMenuItem *)xv_get(item, MENU_CLIENT_DATA);
          if (menuItem)
            return menuItem->uncheckedString;
	}
    }
  return (NULL);
#endif
}

#ifdef wx_xview
Menu_item wxMenu:: FindMenuItem (int Id)
{
  Menu x_menu = (Menu) handle;

  int num = (int) xv_get (x_menu, MENU_NITEMS);

  int i;
  for (i = 1; i <= num; i++)
    {
      Menu_item item = (Menu_item) xv_get (x_menu, MENU_NTH_ITEM, i);
      wxMenuItem *menuItem = (wxMenuItem *) xv_get (item, MENU_CLIENT_DATA);
      if (menuItem && (Id == menuItem->itemId))
	return item;
    }

  for (wxNode * node = children->First (); node; node = node->Next ())
    {
      wxMenu *child = (wxMenu *) node->Data ();
      Menu_item item = child->FindMenuItem (Id);
      if (item)
	return item;
    }
  return 0;
}
#endif

#ifdef wx_motif
Widget wxMenu:: FindMenuItem (int Id, wxMenuItem ** it)
{
  if (Id == menuId)
    {
      if (it)
	*it = NULL;
      return buttonWidget;
    }

  for (wxNode * node = menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      if (item->itemId == Id)
	{
	  if (it)
	    *it = item;
	  return item->buttonWidget;
	}

      if (item->subMenu)
	{
	  Widget w = item->subMenu->FindMenuItem (Id, it);
	  if (w)
	    {
	      return w;
	    }
	}
    }				// for()

  if (it)
    *it = NULL;
  return NULL;
}
#endif


#ifdef wx_xview
void 
wxMenuProc (Menu x_menu, Menu_item menu_item)
{

  wxMenu *menu = (wxMenu *) xv_get (x_menu, MENU_CLIENT_DATA);
  wxMenuItem *menuItem = (wxMenuItem *) xv_get (menu_item, MENU_CLIENT_DATA);

  // Should find top-level menu and use ITS callback
  wxMenu *top_level_menu = menu->top_level_menu;

  wxFrame *frame = NULL;

  if (menuItem && menuItem->checkable)
    {
      menu->Check (menuItem->itemId, !menu->Checked (menuItem->itemId));
    }

  // If a menu bar, send a message to the frame
  if (top_level_menu && top_level_menu->menu_bar)
    {
      frame = top_level_menu->menu_bar->menu_bar_frame;
      frame->GetEventHandler()->OnMenuCommand (menuItem->itemId);
    }
  else if (top_level_menu && top_level_menu->callback)
    {
      wxCommandEvent event (wxEVENT_TYPE_MENU_COMMAND);
      event.eventObject = menu;
      event.commandInt = menuItem->itemId;
      (void) (*(top_level_menu->callback)) (*menu, event);
    }
}

#endif

// Menu Bar

// IMPLEMENT_DYNAMIC_CLASS(wxMenuBar, wxWindow)

wxMenuBar::wxMenuBar (void)
{
}

wxMenuBar::wxMenuBar (int N, wxMenu * Menus[], char *Titles[]):
  wxbMenuBar (N, Menus, Titles)
{
}

wxMenuBar::~wxMenuBar (void)
{
#ifdef wx_motif
#endif
#ifdef wx_xview
  wxFrame *frame = menu_bar_frame;
  if (frame)
    {
      frame->wx_menu_bar = 0;
      delete frame->menu_bar_panel;
      frame->y_offset = 0;
    }
#endif
  int i;
  for (i = 0; i < n; i++)
    {
      delete menus[i];
      delete[]titles[i];
    }
  delete[]menus;
  delete[]titles;
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus to enable/disable items
void wxMenuBar::Enable (int Id, Bool Flag)
{
#ifdef wx_motif
  int j;
  for (j = 0; j < n; j++)
    {
      wxMenuItem *it = NULL;
      Widget w = menus[j]->FindMenuItem (Id, &it);
      if (w)
	{
	  XtSetSensitive (w, Flag);
	  return;
	}
      else if (it)
	{
	  it->isEnabled = Flag;
	  return;
	}
    }
#endif
#ifdef wx_xview
  int j;
  for (j = 0; j < n; j++)
    {
      Menu_item item = menus[j]->FindMenuItem (Id);
      if (item)
	{
	  xv_set (item, MENU_INACTIVE, !Flag, NULL);
	  return;
	}
    }
#endif
}

// Must only be used AFTER menu has been attached to frame,
// otherwise use individual menus
void wxMenuBar::Check (int Id, Bool Flag)
{
#ifdef wx_motif
  int j;
  for (j = 0; j < n; j++)
    {
      wxMenuItem *it = NULL;
      Widget w = menus[j]->FindMenuItem (Id, &it);
      if (w && XtIsSubclass (w, xmToggleButtonGadgetClass))
	{
	  XtVaSetValues (w, XmNset, (Boolean) Flag, NULL);
	  return;
	}
      else if (it)
	{
	  it->isChecked = Flag;
	  return;
	}
    }
#endif
#ifdef wx_xview
  int j;
  for (j = 0; j < n; j++)
    {
      Menu_item item = menus[j]->FindMenuItem (Id);
      if (item)
      {
        wxMenuItem *menuItem = (wxMenuItem *)xv_get(item, MENU_CLIENT_DATA);
        if (menuItem && menuItem->checkable)
        {
          menuItem->isChecked = Flag;
          char *str = Flag ? menuItem->checkedString : menuItem->uncheckedString;
          xv_set (item, MENU_STRING, str, NULL);
        }
        return;
      }
    }
#endif
}

Bool wxMenuBar::Checked (int Id)
{
#ifdef wx_motif
  Boolean Flag;
  int j;
  for (j = 0; j < n; j++)
    {
      Widget w = menus[j]->FindMenuItem (Id);
      if (w && XtIsSubclass (w, xmToggleButtonGadgetClass))
	{
	  XtVaGetValues (w,
			 XmNset, &Flag,
			 NULL);
	  return (Bool) Flag;
	}
    }
  return FALSE;
#endif
#ifdef wx_xview
  int j;
  for (j = 0; j < n; j++)
    {
      Menu_item item = menus[j]->FindMenuItem (Id);
      if (item)
      {
        wxMenuItem *menuItem = (wxMenuItem *)xv_get(item, MENU_CLIENT_DATA);
        if (menuItem)
          return (menuItem->checkable && menuItem->isChecked);
        else
          return FALSE;
      }
    }
  return FALSE;
#endif
}

void wxMenuBar::SetLabel (int Id, char *label)
{
#ifdef wx_motif
  char mnem = wxFindMnemonic (label);
#endif
  wxStripMenuCodes (label, wxBuffer);
#ifdef wx_motif
  int j;
  for (j = 0; j < n; j++)
    {
      Widget w = menus[j]->FindMenuItem (Id);
      if (w)
	{
	  XmString label_str = XmStringCreateSimple (wxBuffer);
	  XtVaSetValues (w,
			 XmNlabelString, label_str,
			 NULL);
	  XmStringFree (label_str);
	  if (mnem != 0)
	    XtVaSetValues (w, XmNmnemonic, mnem, NULL);
	  char *accel = wxFindAccelerator (label);
	  if (accel)
	    XtVaSetValues (w, XmNaccelerator, accel, NULL);

	  XmString accel_str = wxFindAcceleratorText (label);
	  if (accel_str)
	    {
	      XtVaSetValues (w, XmNacceleratorText, accel_str, NULL);
	      XmStringFree (accel_str);
	    }
	  return;
	}
    }
#endif
#ifdef wx_xview
  int i;
  for (i = 0; i < n; i++)
    {
      Menu_item item = menus[i]->FindMenuItem (Id);
      if (item)
      {
        menus[i]->SetLabel(Id, label);
        return;
      }
    }
#endif
}

char *wxMenuBar::GetLabel (int Id)
{
#ifdef wx_motif
  int j;
  for (j = 0; j < n; j++)
    {
      Widget w = menus[j]->FindMenuItem (Id);
      if (w)
	{
	  XmString text;
	  char *s;
	  XtVaGetValues (w,
			 XmNlabelString, &text,
			 NULL);

	  if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
	    {
	      strcpy (wxBuffer, s);
	      XtFree (s);
	      return wxBuffer;
	    }
	  else
	    {
	      return NULL;
	    }
	}
    }
  return (NULL);
#endif
#ifdef wx_xview
  int i;
  for (i = 0; i < n; i++)
    {
      Menu_item item = menus[i]->FindMenuItem (Id);
      if (item)
        return menus[i]->GetLabel(Id);
    }
  return NULL;
#endif
}

void wxMenuBar::SetLabelTop (int pos, char *label)
{
#ifdef wx_motif
  Widget w = menus[pos]->buttonWidget;
  if (w)
    {
      XmString label_str = XmStringCreateSimple (label);
      XtVaSetValues (w,
		     XmNlabelString, label_str,
		     NULL);
      XmStringFree (label_str);
      return;
    }
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) (menus[pos]->panelItem);
  char *p = copystring (label);
  xv_set (item, PANEL_LABEL_STRING, p, NULL);
#endif
}

char *wxMenuBar::GetLabelTop (int pos)
{
#ifdef wx_motif
  Widget w = menus[pos]->buttonWidget;
  if (w)
    {
      XmString text;
      char *s;
      XtVaGetValues (w,
		     XmNlabelString, &text,
		     NULL);

      if (XmStringGetLtoR (text, XmSTRING_DEFAULT_CHARSET, &s))
	{
	  strcpy (wxBuffer, s);
	  XtFree (s);
	  return wxBuffer;
	}
      else
	{
	  return NULL;
	}
    }
  else
    return (NULL);
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) (menus[pos]->panelItem);
  char *p = (char *) xv_get (item, PANEL_LABEL_STRING);
  return p;
#endif
}

void wxMenuBar::EnableTop (int pos, Bool flag)
{
  if (pos < 0 || (pos >= n))
    return;

#ifdef wx_motif
  Widget w = menus[pos]->buttonWidget;
  if (w)
    {
      XtSetSensitive (w, flag);
      return;
    }
#endif
#ifdef wx_xview
  Panel_item item = (Panel_item) (menus[pos]->panelItem);
  xv_set (item, PANEL_INACTIVE, !flag, NULL);
#endif
}

Bool wxMenuBar::OnAppend(wxMenu *menu, char *title)
{
  if (!menu_bar_frame)
    return TRUE;
  if (menu->GetParent())
    return FALSE;

#ifdef wx_motif
  menu->ownedByMenuBar = TRUE;

  if (menu->buttonWidget)
    return FALSE;

  menu->buttonWidget = menu->CreateMenu(this, (Widget)handle, menu, 
					title, TRUE);
#else
  if (menu->panelItem)
    return FALSE;

  wxStripMenuCodes (title, wxBuffer);
  menu->panelItem =
    (void *) xv_create ((Panel)menu_bar_frame->menu_bar_panel->handle, 
			PANEL_BUTTON,
			PANEL_LABEL_STRING, wxBuffer,
			PANEL_ITEM_MENU, (Menu)menu->handle,
			NULL);
#endif

  return TRUE;
}

Bool wxMenuBar::OnDelete(wxMenu *menu, int position)
{
  if (!menu_bar_frame)
    return TRUE;

#ifdef wx_motif
  menu->DestroyMenu(TRUE);
#else
  xv_set((Panel)menu->panelItem, PANEL_ITEM_MENU, NULL, NULL);
  xv_destroy_safe((Xv_opaque)menu->panelItem);
  menu->panelItem = NULL;
#endif

  return TRUE;
}

#ifdef wx_motif
// Added by JOREL Jean-Charles <jjorel@silr.ireste.fr>
extern wxApp *wxTheApp;
static XtWorkProcId WorkProcMenuId;
/* Since PopupMenu under Motif stills grab right mouse button events
 * after it was closed, we need to delete the associated widgets to
 * allow next PopUpMenu to appear...
 */

#ifdef USE_NL_POPUP_MENU_FIX
int PostDeletionOfMenu( wxMenu *menu )
{
#else
int PostDeletionOfMenu( XtPointer* clientData )
{
  XtRemoveWorkProc(WorkProcMenuId);
  wxMenu *menu = (wxMenu *)clientData;
#endif //USE_NL_POPUP_MENU_FIX

  if (menu->handle) {
    wxList *list = menu->GetParent()->GetChildren();
    int dd = list->destroy_data;

    list->DeleteContents(FALSE);
    list->DeleteObject(menu);
    list->DeleteContents(dd);

    menu->DestroyMenu(TRUE);
  }
  /* Mark as no longer popped up */
  menu->menuId = -1;
  return TRUE;
}

void 
wxMenuPopdownCallback(Widget w, XtPointer clientData,
		      XtPointer ptr)
{
  wxMenu *menu = (wxMenu *)clientData;

  // Added by JOREL Jean-Charles <jjorel@silr.ireste.fr>
  /* Since Callbacks of MenuItems are not yet processed, we put a
   * background job which will be done when system will be idle.
   * What awful hack!! :(
   */

#ifdef USE_NL_POPUP_MENU_FIX
   XButtonEvent *Ev=(XButtonEvent*)((XmAnyCallbackStruct*)ptr)->event;

   Dimension MenuX, MenuY, MenuW, MenuH;
   int MainX, MainY;
   menu->GetPosition(&MainX, &MainY);

   wxMenuItem *Item;
   wxNode *Node = menu->menuItems.First();

   while(Node)
   {
      Item=(wxMenuItem *)Node->Data();

      XtVaGetValues(Item->buttonWidget, XmNx, &MenuX, 
					XmNy, &MenuY,
					XmNwidth, &MenuW,
					XmNheight, &MenuH, NULL);

      //Translate to root-window relative points
      MenuX+=MainX;
      MenuY+=MainY;

      //Check to see if the mouse is within the menu window
      if((Ev->x_root > MenuX) && (Ev->y_root > MenuY) &&
         (Ev->x_root < MenuX + MenuW) && (Ev->y_root < MenuY + MenuH))
      {
         if(Item->itemId < 0)
	 {   //It is in a title or separator so cancel it now
	     PostDeletionOfMenu(menu);
	 }
	 //The pointer is on a menu item - thats all we needed to know
	 return;
      }
      Node=Node->Next();
   }
   //The pointer is elsewhere
   PostDeletionOfMenu(menu);
#else
  WorkProcMenuId = XtAppAddWorkProc( 
		(XtAppContext) wxTheApp->appContext, 
		(XtWorkProc) PostDeletionOfMenu,
		(XtPointer) menu );
  // Apparently not found in Motif headers
  //  XtVaSetValues( w, XmNpopupEnabled, XmPOPUP_DISABLED, NULL );
#endif  //USE_NL_POPUP_MENU_FIX
}

void 
wxMenuItemCallback (Widget w, XtPointer clientData,
		    XtPointer ptr)
{
  wxMenuItem *item = (wxMenuItem *) clientData;
  if (item)
    {
      if (item->menuBar && item->menuBar->menu_bar_frame)
	{
  	  // cout << "Id = " << item->itemId << "\n";
	  item->menuBar->menu_bar_frame->GetEventHandler()->OnMenuCommand (item->itemId);
	}
      else if (item->topMenu)
	{
	  wxCommandEvent event (wxEVENT_TYPE_MENU_COMMAND);
	  event.eventObject = item->topMenu;
	  event.commandInt = item->itemId;
	  item->topMenu->ProcessCommand (event);
	}
    }
#ifdef USE_NL_POPUP_MENU_FIX
	if(!item->menuBar)
		PostDeletionOfMenu(item->topMenu);
#endif //USE_NL_POPUP_MENU_FIX
}

void 
wxMenuItemArmCallback (Widget w, XtPointer clientData,
		       XtPointer ptr)
{
  wxMenuItem *item = (wxMenuItem *) clientData;
  if (item)
    {
      if (item->menuBar && item->menuBar->menu_bar_frame)
	{
	  item->menuBar->menu_bar_frame->GetEventHandler()->OnMenuSelect (item->itemId);
	}
    }
}

void 
wxMenuItemDisarmCallback (Widget w, XtPointer clientData,
			  XtPointer ptr)
{
  wxMenuItem *item = (wxMenuItem *) clientData;
  if (item)
    {
      if (item->menuBar && item->menuBar->menu_bar_frame)
	{
	  item->menuBar->menu_bar_frame->GetEventHandler()->OnMenuSelect (-1);
	}
    }
}

/*
 * Create a popup or pulldown menu.
 * Submenus of a popup will be pulldown.
 *
 */

Widget wxMenu::CreateMenu (wxMenuBar * menuBar, Widget parent, wxMenu * topMenu, char *Title, Bool pullDown)
{
  Widget menu;
  Widget ButtonWidget = 0;
  Arg args[5];
  XtSetArg (args[0], XmNnumColumns, numColumns);
  XtSetArg (args[1], XmNpacking, XmPACK_COLUMN);

  if (!pullDown)
    {
      menu = XmCreatePopupMenu (parent, "popup", args, 2);
      XtAddCallback(menu,
		    XmNunmapCallback, 
		    (XtCallbackProc)wxMenuPopdownCallback,
		    (XtPointer)this);
    }
  else
    {
      char mnem = wxFindMnemonic (Title);
      wxStripMenuCodes (Title, wxBuffer);

      menu = XmCreatePulldownMenu (parent, "pulldown", args, 2);

      XmString label_str = XmStringCreateSimple (wxBuffer);
      ButtonWidget = XtVaCreateManagedWidget (wxBuffer,
#if USE_GADGETS
					 xmCascadeButtonGadgetClass, parent,
#else
					 xmCascadeButtonWidgetClass, parent,
#endif
					      XmNlabelString, label_str,
					      XmNsubMenuId, menu,
					      NULL);

      if (mnem != 0)
	XtVaSetValues (ButtonWidget, XmNmnemonic, mnem, NULL);

      XmStringFree (label_str);
    }

  handle = (char *) menu;

  menu_bar = menuBar;
  top_menu = topMenu;

  for (wxNode * node = menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      item->CreateItem (menu, menu_bar, top_menu);
    }

  return ButtonWidget;
}

// Destroys the Motif implementation of the menu,
// but maintains the wxWindows data structures so we can
// do a CreateMenu again. 
void wxMenu::DestroyMenu (Bool full)
{
  for (wxNode * node = menuItems.First (); node; node = node->Next ())
    {
      wxMenuItem *item = (wxMenuItem *) node->Data ();
      item->menuBar = NULL;

      item->DestroyItem(full);
    }				// for()

  if (buttonWidget)
    {
      if (full)
      {
        XtVaSetValues(buttonWidget, XmNsubMenuId, NULL, NULL);
        XtDestroyWidget (buttonWidget);
        buttonWidget = 0;
      }
    }
  if (handle && full)
    {
      XtDestroyWidget((Widget)handle);
      handle = NULL;
    }
}

void wxMenuItem::CreateItem (Widget menu, wxMenuBar * menu_bar, wxMenu * top_menu)
{
  menuBar = menu_bar;
  topMenu = top_menu;

  if (itemId == -2)
    {
      // Id=-2 identifies a Title item.
      wxStripMenuCodes (itemName, wxBuffer);
      buttonWidget = XtVaCreateManagedWidget (wxBuffer,
					    xmLabelGadgetClass, menu, NULL);
    }
  else if (itemName && (!subMenu))
    {
      wxStripMenuCodes (itemName, wxBuffer);
      if (checkable)
	{
	  buttonWidget = XtVaCreateManagedWidget (wxBuffer,
					    xmToggleButtonGadgetClass, menu,
						  NULL);
	  XtVaSetValues (buttonWidget, XmNset, (Boolean) isChecked, NULL);
	}
      else
	buttonWidget = XtVaCreateManagedWidget (wxBuffer,
					      xmPushButtonGadgetClass, menu,
						NULL);
      char mnem = wxFindMnemonic (itemName);
      if (mnem != 0)
	XtVaSetValues (buttonWidget, XmNmnemonic, mnem, NULL);

      char *accel = wxFindAccelerator (itemName);
      if (accel)
	XtVaSetValues (buttonWidget, XmNaccelerator, accel, NULL);

      XmString accel_str = wxFindAcceleratorText (itemName);
      if (accel_str)
	{
	  XtVaSetValues (buttonWidget, XmNacceleratorText, accel_str, NULL);
	  XmStringFree (accel_str);
	}

      if (checkable)
	XtAddCallback (buttonWidget,
		       XmNvalueChangedCallback,
		       (XtCallbackProc) wxMenuItemCallback,
		       (XtPointer) this);
      else
	XtAddCallback (buttonWidget,
		       XmNactivateCallback,
		       (XtCallbackProc) wxMenuItemCallback,
		       (XtPointer) this);
      XtAddCallback (buttonWidget,
		     XmNarmCallback,
		     (XtCallbackProc) wxMenuItemArmCallback,
		     (XtPointer) this);
      XtAddCallback (buttonWidget,
		     XmNdisarmCallback,
		     (XtCallbackProc) wxMenuItemDisarmCallback,
		     (XtPointer) this);
    }
  else if (itemId == -1)
    {
      buttonWidget = XtVaCreateManagedWidget ("separator",
					xmSeparatorGadgetClass, menu, NULL);
    }
  else if (subMenu)
    {
      buttonWidget = subMenu->CreateMenu (menu_bar, menu, topMenu, itemName, TRUE);
      subMenu->buttonWidget = buttonWidget;
      XtAddCallback (buttonWidget,
		     XmNcascadingCallback,
		     (XtCallbackProc) wxMenuItemArmCallback,
		     (XtPointer) this);
    }
  if (buttonWidget)
    XtSetSensitive (buttonWidget, isEnabled);
}

void wxMenuItem::DestroyItem(Bool full)
{
  if (itemId == -2)
    {
      ;			// Nothing
      
    }
  else if (itemName && !subMenu)
    {
      if (buttonWidget)
	{
	  if (checkable)
	    XtRemoveCallback (buttonWidget, XmNvalueChangedCallback,
			      wxMenuItemCallback, (XtPointer) this);
	  else
	    XtRemoveCallback (buttonWidget, XmNactivateCallback,
			      wxMenuItemCallback, (XtPointer) this);
	  XtRemoveCallback (buttonWidget, XmNarmCallback,
			    wxMenuItemArmCallback, (XtPointer) this);
	  XtRemoveCallback (buttonWidget, XmNdisarmCallback,
			    wxMenuItemDisarmCallback, (XtPointer) this);
	}
    }
  else if (itemId == -1)
    {
      ;			// Nothing
      
    }
  else if (subMenu)
    {
      if (buttonWidget)
	{
	  XtRemoveCallback (buttonWidget, XmNcascadingCallback,
			    wxMenuItemArmCallback, (XtPointer) this);
	}
      subMenu->DestroyMenu(full);
      if (full)
        buttonWidget = NULL;
    }

  if (buttonWidget && full)
    {
      XtDestroyWidget (buttonWidget);
      buttonWidget = 0;
    }
}

#endif
