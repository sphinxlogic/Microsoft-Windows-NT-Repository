/*
 * File:     test.h
 * Purpose:  Demo for tabbed dialog library
 *
 */

// Define a new application
class MyApp: public wxApp
{
  public:
    wxFrame *OnInit(void);
};

class MyFrame: public wxFrame
{
  public:
    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    Bool OnClose(void);
    void OnMenuCommand(int id);
    
    void TestTabbedDialog(void);
};

void GenericOk(wxButton& but, wxCommandEvent& event);

// File ids
#define TEST_QUIT           1
#define TEST_ABOUT          2
#define TEST_TABBED_DIALOG  3

// Tab ids
#define TEST_TAB_DOG        1
#define TEST_TAB_CAT        2
#define TEST_TAB_GOAT       3
#define TEST_TAB_GUINEAPIG  4
#define TEST_TAB_ANTEATER   5
#define TEST_TAB_HUMMINGBIRD 6
#define TEST_TAB_SHEEP      7
#define TEST_TAB_COW        8
#define TEST_TAB_HORSE      9
#define TEST_TAB_PIG        10
#define TEST_TAB_OSTRICH    11
#define TEST_TAB_AARDVARK   12

