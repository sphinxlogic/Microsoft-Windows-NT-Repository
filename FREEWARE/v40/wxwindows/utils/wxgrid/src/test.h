#define TOTAL_ROWS         20
#define TOTAL_COLUMNS      5
#define DATA_LENGTH        20

#define LEFT_OF_SHEET      0
#define TOP_OF_SHEET       31
#define TOTAL_HORZ_LINES   21
#define TOTAL_VERT_LINES   5
#define CELL_HEIGHT        20
#define CELL_WIDTH         80
#define CLIENT_HEIGHT      490
#define CLIENT_WIDTH       440
#define NUM_COLUMN_WIDTH   40
#define TOTAL_CELL_ROWS    200

//#define LEFT_OF_EDIT       100
#define LEFT_OF_EDIT       5
#define TOP_OF_EDIT        1
#define EDIT_WIDTH         180
#define EDIT_HEIGHT        27

#define TOP_OF_POSITION    -1
#define LEFT_OF_POSITION   -1
#define POSITION_HEIGHT    25
#define POSITION_WIDTH     50

#define EDIT_BUFFER_SIZE   200

void FAR PASCAL DrawDots (int X, int Y, LPSTR lphDC);

class wxTableWindow: public wxPanel
{
 public:
  HWND hEdit;
//  HWND hStatic;
  wxText *textItem;
  WORD wPresentRow;
  WORD wPresentColumn;
  RECT CurrentRect;
  wxString ***Cell;
  BOOL bEditCreated;
  char szRowNumber[3];
  char szEdit[EDIT_BUFFER_SIZE+1];

  int totalRows;
  int totalCols;

  wxTableWindow(wxWindow *parent, int x, int y, int width, int height, long style = 0, char *name = "tableWindow");
  ~wxTableWindow(void);

  void OnPaint(void);
  void OnEvent(wxMouseEvent& ev);
//  void PaintGrid (HWND hWnd);
  void SetCurrentRect (WORD Row, WORD Column);
  void HighlightCell (HDC hDC);

  wxString *GetCell(int row, int col);
  void CreateTable(int nRows, int nCols);
  void ClearTable(void);
};
