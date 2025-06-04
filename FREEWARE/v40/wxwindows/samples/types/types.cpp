/*
 * File:	types.cc
 * Purpose:	Date sample
 * Author:	Julian Smart
 * Created:	1995
 * Updated:	
 * Copyright:	(c) 1995, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "%W% %G%"; */

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx.h"
#endif

#include <time.h>
#include "wx_time.h"
#include "wx_date.h"
#include "wx_frac.h"

void DoDateDemo(wxTextWindow& textWin);
void DoTimeDemo(wxTextWindow& textWin);
void DoFractionDemo(wxTextWindow& textWin);

wxTextWindow *textWin = NULL;

// Define a new application type
class MyApp: public wxApp
{ public:
    wxFrame *OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame
{ public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    void OnMenuCommand(int id);
    Bool OnClose(void) { return TRUE; }
};

// ID for the menu quit command
#define TYPES_QUIT 1
#define TYPES_DATE 2
#define TYPES_TIME 3
#define TYPES_FRACTION 4

// This statement initializes the whole application and calls OnInit
MyApp myApp;

// A macro needed for some compilers (AIX) that need 'main' to be defined
// in the application itself.
IMPLEMENT_WXWIN_MAIN

// `Main program' equivalent, creating windows and returning main app frame
wxFrame *MyApp::OnInit(void)
{
  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "Types demo", 50, 50, 400, 300);

  // Give it an icon
#ifdef wx_msw
  frame->SetIcon(new wxIcon("mondrian"));
#endif
#ifdef wx_x
  frame->SetIcon(new wxIcon("aiai.xbm"));
#endif

  // Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(TYPES_DATE, "&Date");
  file_menu->Append(TYPES_TIME, "&Time");
  file_menu->Append(TYPES_FRACTION, "&Fraction");
  file_menu->Append(TYPES_QUIT, "E&xit");
  wxMenuBar *menu_bar = new wxMenuBar;
  menu_bar->Append(file_menu, "&File");
  frame->SetMenuBar(menu_bar);

  textWin = new wxTextWindow(frame, 0, 0, 400, 400);

  // Show the frame
  frame->Show(TRUE);

  // Return the main frame window
  return frame;
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h):
  wxFrame(frame, title, x, y, w, h)
{}

// Intercept menu commands
void MyFrame::OnMenuCommand(int id)
{
  switch (id)
  {
    case TYPES_QUIT:
    {
      if (OnClose())
        delete this;
      break;
    }
    case TYPES_DATE:
    {
      DoDateDemo(*textWin);
      break;
    }
    case TYPES_TIME:
    {
      DoTimeDemo(*textWin);
      break;
    }
    case TYPES_FRACTION:
    {
#if USE_FRACTION
      DoFractionDemo(*textWin);
#endif
      break;
    }
    break;
  }
}

void DoTimeDemo(wxTextWindow& textWin)
{
  textWin.Clear();
  cout << "\nTest class wxTime" << endl;
  wxTime now;
  int hr,mn,sc;
  textWin << "It is now " << now << "\n";
}

void DoFractionDemo(wxTextWindow& textWin)
{
#if USE_FRACTION
  textWin.Clear();
  textWin << "\nTest class wxFraction" << "\n";
  wxFraction a(40902L,24140), b(7,66), c(17,12);
  textWin << "a = " << a << "\n";
  textWin << "b = " << b << "\n";
  textWin << "c = " << c << "\n";
  textWin << "7/66 + 17/12 = " << (b+c) << "\n";
  textWin << "7/66 - 17/12 = " << (b-c) << "\n";
  textWin << "7/66 * 17/12 = " << (b*c) << "\n";
  textWin << "7/66 / 17/12 = " << (b/c) << "\n";
  textWin << "7/66 < 11/100 = " << (b<wxFraction(11,100)) << "\n";
  textWin << "wxFraction(0.5) = " << wxFraction(0.5) << "\n";
  textWin << "wxFraction(0.333333e6) = " << wxFraction(0.333333e6) << "\n";
  textWin << "wxFraction(0.333333e-6) = " << wxFraction(0.333333e-6) << "\n";
  textWin << "(double)wxFraction(1,2) = " << (double)wxFraction(1,2) << "\n";
#endif
}

void DoDateDemo(wxTextWindow& textWin)
{
    textWin.Clear();
    textWin << "\nTest class wxDate" << "\n";

    // Various versions of the constructors
    // and various output

    wxDate x(10,20,1962);

    textWin << x.FormatDate(wxFULL) << "  (full)\n";

    // constuctor with a string, just printing the day of the week
    wxDate y("8/8/1988");

    textWin << y.FormatDate(wxDAY) << "  (just day)\n";

    // constructor with a julian
    wxDate z( 2450000L );
    textWin << z.FormatDate(wxFULL) << "  (full)\n";

    // using date addition and subtraction
    wxDate a = x + 10;
    textWin << a.FormatDate(wxFULL) << "  (full)\n";
    a = a - 25;
    textWin << a.FormatDate(wxEUROPEAN) << "  (European)\n";

    
    // Using subtraction of two date objects
    wxDate a1 = "7/13/1991";
    wxDate a2 = a1 + 14;
    textWin << (a1-a2) << "\n";
    textWin << (a2+=10) << "\n";

    a1++;
    textWin << "Tomorrow= " << a1.FormatDate(wxFULL) << "\n";

    wxDate tmpDate1("08/01/1991");
    wxDate tmpDate2("07/14/1991");
    textWin << "a1 (7-14-91) < 8-01-91 ? ==> " << ((a1 < tmpDate1) ? "TRUE" : "FALSE") << "\n";
    textWin << "a1 (7-14-91) > 8-01-91 ? ==> " << ((a1 > tmpDate1) ? "TRUE" : "FALSE") << "\n";
    textWin << "a1 (7-14-91)== 7-14-91 ? ==> " << ((a1==tmpDate2) ? "TRUE" : "FALSE") << "\n";

    wxDate a3 = a1;
    textWin << "a1 (7-14-91)== a3 (7-14-91) ? ==> " << ((a1==a3) ? "TRUE" : "FALSE") << "\n";
    wxDate a4 = a1;
    textWin << "a1 (7-14-91)== a4 (7-15-91) ? ==> " << ((a1==++a4) ? "TRUE" : "FALSE") << "\n";

    wxDate a5 = "today";
    textWin << "Today is: " << a5 << "\n";
    a4 = "TODAY";
    textWin << "Today (a4) is: " << a4 << "\n";

    textWin << "Today + 4 is: " << (a4+=4) << "\n";
    a4 = "TODAY";
    textWin << "Today - 4 is: " << (a4-=4) << "\n";

    textWin << "=========== Leap Year Test ===========\n";
    a1 = "1/15/1992";
    textWin << a1.FormatDate(wxFULL) << "  " << ((a1.IsLeapYear()) ? "Leap" : "non-Leap");
    textWin << "  " << "day of year:  " << a1.GetDayOfYear() << "\n";

    a1 = "2/16/1993";
    textWin << a1.FormatDate(wxFULL) << "  " << ((a1.IsLeapYear()) ? "Leap" : "non-Leap");
    textWin << "  " << "day of year:  " << a1.GetDayOfYear() << "\n";

    textWin << "================== string assignment test ====================\n";
    char *date_string=a1;
    textWin << "a1 as a string (s/b 2/16/1993) ==> " << date_string << "\n";

    textWin << "================== SetFormat test ============================\n";
    a1.SetFormat(wxFULL);
    textWin << "a1 (s/b FULL format) ==> " << a1 << "\n";
    a1.SetFormat(wxEUROPEAN);
    textWin << "a1 (s/b EUROPEAN format) ==> " << a1 << "\n";

    textWin << "================== SetOption test ============================\n";
    textWin << "Date abbreviation ON\n";

    a1.SetOption(wxDATE_ABBR);
    a1.SetFormat(wxMONTH);
    textWin << "a1 (s/b MONTH format) ==> " << a1 << "\n";
    a1.SetFormat(wxDAY);
    textWin << "a1 (s/b DAY format) ==> " << a1 << "\n";
    a1.SetFormat(wxFULL);
    textWin << "a1 (s/b FULL format) ==> " << a1 << "\n";
    a1.SetFormat(wxEUROPEAN);
    textWin << "a1 (s/b EUROPEAN format) ==> " << a1 << "\n";
    textWin << "Century suppression ON\n";
    a1.SetOption(wxNO_CENTURY);
    a1.SetFormat(wxMDY);
    textWin << "a1 (s/b MDY format) ==> " << a1 << "\n";
    textWin << "Century suppression OFF\n";
    a1.SetOption(wxNO_CENTURY,FALSE);
    textWin << "a1 (s/b MDY format) ==> " << a1 << "\n";
    textWin << "Century suppression ON\n";
    a1.SetOption(wxNO_CENTURY);
    textWin << "a1 (s/b MDY format) ==> " << a1 << "\n";
    a1.SetFormat(wxFULL);
    textWin << "a1 (s/b FULL format) ==> " << a1 << "\n";

    textWin << "\n=============== Version 4.0 Enhancement Test =================\n";
    
    wxDate v4("11/26/1966");
    textWin << "\n---------- Set Stuff -----------\n";
    textWin << "First, 'Set' to today..." << "\n";
    textWin << "Before 'Set' => " << v4 << "\n";
    textWin << "After  'Set' => " << v4.Set() << "\n\n";

    textWin << "Set to 11/26/66 => " << v4.Set(11,26,1966) << "\n";
    textWin << "Current Julian  => " << v4.GetJulianDate() << "\n";
    textWin << "Set to Julian 2450000L => " << v4.Set(2450000L) << "\n";
    textWin << "See! => " << v4.GetJulianDate() << "\n";

    textWin << "---------- Add Stuff -----------\n";
    textWin << "Start => " << v4 << "\n";
    textWin << "Add 4 Weeks => " << v4.AddWeeks(4) << "\n";
    textWin << "Sub 1 Month => " << v4.AddMonths(-1) << "\n";
    textWin << "Add 2 Years => " << v4.AddYears(2) << "\n";

    textWin << "---------- Misc Stuff -----------\n";
    textWin << "The date aboves' day of the month is => " << v4.GetDay() << "\n";
    textWin << "There are " << v4.GetDaysInMonth() << " days in this month.\n";
    textWin << "The first day of this month lands on " << v4.GetFirstDayOfMonth() << "\n";
    textWin << "This day happens to be " << v4.GetDayOfWeekName() << "\n";
    textWin << "the " << v4.GetDayOfWeek() << " day of the week," << "\n";
    textWin << "on the " << v4.GetWeekOfYear() << " week of the year," << "\n";
    textWin << "on the " << v4.GetWeekOfMonth() << " week of the month, " << "\n";
    textWin << "(which is " << v4.GetMonthName() << ")\n";
    textWin << "the "<< v4.GetMonth() << "th month in the year.\n";
    textWin << "The year alone is " << v4.GetYear() << "\n";

    textWin << "---------- First and Last Stuff -----------\n";
    v4.Set();
    textWin << "The first date of this month is " << v4.GetMonthStart() << "\n";
    textWin << "The last date of this month is " << v4.GetMonthEnd() << "\n";
    textWin << "The first date of this year is " << v4.GetYearStart() << "\n";
    textWin << "The last date of this year is " << v4.GetYearEnd() << "\n";
}

