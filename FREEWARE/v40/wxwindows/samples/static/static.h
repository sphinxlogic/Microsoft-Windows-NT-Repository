
#define EXIT_BUTTON 0
#define SHOW_BUTTON 1
#define DRAW_BUTTON 2
#define REMOVE_BUTTON 3
#define DELETE_BUTTON 4

class assa : public wxApp
   {
     public :
         wxFrame *frame;
         wxPanel *panel;
         wxFrame * OnInit();
   };
