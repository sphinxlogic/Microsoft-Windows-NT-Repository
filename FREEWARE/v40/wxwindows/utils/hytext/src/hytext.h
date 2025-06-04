/*
 * File:	hytext.h
 * Purpose:	Hypertext library for wxWindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* sccsid[] = "%W% %G%" */

/*
 * hytext.h
 * Hypertext Window implementation.
 * This is a text window with highlighted text which can respond
 * to mouse clicks. Blocks are stored in ordinary text files as:
 * \hy-A{B}{C}  where A is an integer representing the type of
 * block, B is an integer identifier for this block, and C is the
 * text enclosed by the block. Blocks may be nested.
 * Mouse input member functions are provided for overriding by an application,
 * e.g. for displaying further information.
 */

#ifndef hypertexth
#define hypertexth
#include <stdio.h>
#include <wx.h>
#include "wx_hash.h"

#ifdef __GNUG__
#pragma interface
#endif

// Translation events
#define HYTEXT_EVENT_START_BLOCK    1
#define HYTEXT_EVENT_END_BLOCK      2
#define HYTEXT_EVENT_START_FILE     3
#define HYTEXT_EVENT_END_FILE       4
#define HYTEXT_EVENT_DOUBLE_NEWLINE 5

/*
 * Stored in the text file are only block types, not font information.
 * This mapping class is necessary to establish what types should
 * be depicted with what font/colour.
 */

#define wxHYPER_NONE    0
#define wxHYPER_SECTION 1

#define MAX_DISPLAYED_LINES        500

#ifndef DEFAULT_MOUSE_TOLERANCE
#define DEFAULT_MOUSE_TOLERANCE 3
#endif

class wxHTMappingStructure: public wxObject
{
  DECLARE_CLASS(wxHTMappingStructure)
 public:
  char *name;
  int block_type;
  int text_size;
  int text_family;
  int text_style;
  int text_weight;
  int special_attribute; // e.g. section
  Bool visibility;
  char *text_colour;

  // These are only for internal use
  int logical_op;
  wxColour *background_colour;
  
  wxHTMappingStructure(void);
  wxHTMappingStructure(int the_block_type, int the_text_size, int the_text_family,
                  int the_text_style, int the_text_weight,
                  char *the_text_colour, char *the_name, int the_attribute = wxHYPER_NONE,
                  Bool visibility = TRUE);
  ~wxHTMappingStructure(void);

  wxFont *GetFont(void);
  wxHTMappingStructure *Copy(void);
};

class wxHyperTextMapping: public wxList
{
  DECLARE_CLASS(wxHyperTextMapping)
 public:
  wxNode *current_mapping;  // For GetFirst/NextType

  wxHyperTextMapping(void);
  ~wxHyperTextMapping(void);
  void ClearMapping(void);
  void AddMapping(int block_type, int text_size, int text_family,
                  int text_style, int text_weight, char *text_colour,
                  char *name, int the_attribute = -1, Bool visibility = -1);
  Bool GetMapping(int block_type, int *text_size, int *text_family,
                  int *text_style, int *text_weight, char **text_colour,
                  char **name, int *the_attribute, Bool *visibility);

  wxHTMappingStructure *FindByName(char *name);

  // Pop up dialog box for editing mappings
  void Edit(wxWindow *parent = NULL);

  // -1 if no more types
  int GetFirstType(void);
  int GetNextType(void);
};

class wxTextChunk;
class wxHyperTextWindow: public wxCanvas
{
  DECLARE_CLASS(wxHyperTextWindow)
 public:
  Bool modified;
  Bool edit_mode;
  wxList current_selections;
  wxNode *selection_current;
  wxList sections;         // List of chunks
  wxNode *current_section; // Node points into 'sections'
  char *Title;
  Bool indexWriting;

  int hyleft_margin;
  int hytop_margin;

  int DragState;
  long DraggedBlock;
  float old_drag_x, old_drag_y;  // Previous drag coordinates
  int mouseTolerance;
  float first_drag_x, first_drag_y; // INITIAL drag coordinates  

  float OutlineStartX;
  float OutlineStartY;

  wxList text_chunks;
  wxHyperTextMapping *mapping;
  wxHashTable *LinkTable;
  wxNode *current_block;  // For GetFirst/NextBlock
  int no_lines;
                          // Contains the default text attributes,
                          // found on SetMapping, or default default (sic) used
  wxHTMappingStructure *default_mapping_structure;

  int no_displayed_lines;
  float displayed_lines[MAX_DISPLAYED_LINES];
  int section_start_line;
  int section_end_line;
  long saved_block_id;  // For SaveSection and RestoreSection
  int saved_y;
  int saved_x;

  // Members for translation of currently-loaded file
  FILE *transFile1;
  FILE *transFile2;
  
  /* PUBLIC MEMBER FUNCTIONS */
  wxHyperTextWindow(wxFrame *parent, int x, int y, int w, int h, long style);
  ~wxHyperTextWindow(void);

  void OnPaint(void);
  void OnEvent(wxMouseEvent& event);

  void SetMapping(wxHyperTextMapping *mapping);

  Bool ReadIndex(char *filename, FILE *fd);
  Bool LoadFile(char *filename);
  Bool SaveFile(char *filename);
  Bool ClearFile(void);

  /* Add a block, giving:
   * - start and end coordinates in character/line positions
   * - block type
   * - integer block ID (arbitrary ID)
   */
  Bool AddBlock(int xstart, int ystart, int xend, int yend,
                int block_type, long id);

  Bool ClearBlock(long id);

  // Get blocks in this file (returns -1 for no more)
  long GetFirstBlock(void);
  long GetNextBlock(void);

  void SetBlockType(long block_id, int block_type);
  int GetBlockType(long block_id);    // Gets block type
  void GetBlockText(char *buffer, int max_size, long block_id);  // Gets plain text
  void GetBlockText(char *buffer, int max_size, wxNode *node, long block_id); // Gets plain text
  // Get the text between the given points, optionally converting newlines to spaces
  void GetSpanText(char *buffer, int bufSize, int line1, int char1,
                   int line2, int char2, Bool convertNewLines = FALSE, wxNode *startNode = NULL);

  Bool StringSearch(char *searchString, int *linePos, int *charPos,
                    Bool ignoreCase = TRUE);

  // Select or deselect given block
  void SelectBlock(wxTextChunk *chunk, Bool select = TRUE);
  void SelectBlock(long block_id, Bool select = TRUE);

  long GetFirstSelection(void);
  long GetNextSelection(void);

  inline char *GetTitle(void) { return Title; }
  inline void SetTitle(char *title) { Title = copystring(title); }

  inline wxHashTable *GetLinkTable(void) { return LinkTable; }

  void SetMargins(int left, int top);

  Bool GetEditMode(void);
  void SetEditMode(Bool);
  inline void SetIndexWriting(Bool indexWritingMode) { indexWriting = indexWritingMode; }

  Bool Modified(void);
  void DiscardEdits(void);
  virtual long GenerateId(void);

  /*
   * Finds x, y character position from mouse click position,
   * returning block id if any.
   * Returns FALSE if fails.
   */
  Bool FindPosition(float mouse_x, float mouse_y, 
                    int *char_pos, int *line_pos,
                    long *block_id);

//  void SetCurrentBlockType(int type);

  virtual void OnLeftClick(float x, float y, int char_pos, int line_pos, long block_id, int keys);
  virtual void OnRightClick(float x, float y, int char_pos, int line_pos, long block_id, int keys);

  // Eliminate compiler warnings
  virtual void OnLeftClick(int x, int y, int keys) { wxCanvas::OnLeftClick(x, y, keys); }
  virtual void OnRightClick(int x, int y, int keys) { wxCanvas::OnRightClick(x, y, keys); }

  virtual void OnBeginDragLeft(float x, float y, long block_id, int keys);
  virtual void OnDragLeft(Bool draw, float x, float y, long block_id, int keys);
  virtual void OnEndDragLeft(float x, float y, long block_id, int keys);

  virtual void OnBeginDragRight(float x, float y, long block_id, int keys);
  virtual void OnDragRight(Bool draw, float x, float y, long block_id, int keys);
  virtual void OnEndDragRight(float x, float y, long block_id, int keys);

  virtual void OnSelectBlock(long block_id, Bool select);

  void DrawOutline(float x1, float y1, float x2, float y2);

  // If there are more than zero sections, always display at top of section
  // containing this block (for the time being).
  // Search back for a section start.
  void DisplayFile(void);
  void DisplayFileAt(long block_id, Bool refresh = TRUE);
  void DisplayFileAtTop(void);

  void DisplayNextSection(void);
  void DisplayPreviousSection(void);
  int  GetCurrentSectionNumber(void);

  // Save line number at start of section
  // to be restored after a Compile()
  void SaveSection(void);
  void RestoreSection(void);

  // Traverses chunks assigning fonts/colours using a stack
  Bool Compile(void);

  // Find first chunk at given line (returns a node so can traverse
  // forward from here)
  // If startNode is given, starts looking from here to save time.
  wxNode *FindChunkAtLine(int line_no, wxNode *startNode = NULL);

  // Finds first line-start chunk containing the given block
  // (NOT the actual chunk containing the block)
  wxNode *FindChunkAtBlock(long block_id);

  // Finds text chunk at start of given block
  wxTextChunk *FindBlock(long block_id);
  long FindBlockForSection(wxNode *node);

  // Get the number of lines
  int NoLines(void);

  // Returns the line length in characters, starting from line zero
  int LineLength(int line, wxNode *startNode = NULL);

  // Returns the line number and character position for a character offset.
  // Returns FALSE if did not succeed.
  Bool GetOffsetPosition(int line1, int char1, int offset, int *line2, int *char2, wxNode *startNode = NULL);

  Bool InsertText(int line1, int char1, int *line2, int *char2, char *text);
  Bool DeleteText(int line1, int char1, int line2, int char2);

  // New 2/12/94 Translator functions, for helping to translate
  // hyText files into other forms, e.g. HTML
  virtual void OnTranslate(long eventType, long blockType, long blockId) { };

  // Opening/closing, outputting
  Bool OpenTransFile(int which, char *filename);
  void CloseTransFile(int which);
  void OutputTrans(int which, char *buf);

  // Starts the translation process
  void Translate(void);
  
/*
 * PRIVATE MEMBERS
 *
 */
  void ParseLine(char *line);
};

// Types of chunk

// Start a hypertext block
#define CHUNK_START_BLOCK 1

// Start an unrecognized (maybe Latex) block
#define CHUNK_START_UNRECOGNIZED_BLOCK 2

// End a hypertext or Latex block
#define CHUNK_END_BLOCK   3

// Start a line
#define CHUNK_START_LINE  4

// A chunk is not a block, since it may signify the start of a
// line as well as a block boundary.
class wxTextChunk: public wxObject
{
  DECLARE_CLASS(wxTextChunk)
 public:
  int chunk_type;
  int line_no;      // -1 for no line start
  int block_type;   // -1 for no block
  long block_id;    // -1 for no block
  long end_id;      // If chunk ends a block, this is the id of
                    // the block which has ended.
                    // block_id remains the id for the block currently
                    // in scope.
  Bool selected;
  char *text;
  wxFont *font;     // Always present
  wxColour *colour; // Always present
  wxColour *background_colour; // Internal use only
  int logical_op;             // Internal use only
  Bool visibility;
  int special_attribute; // E.g. new section

  wxTextChunk(int the_chunk_type, int the_line_no, char *the_text,
              wxFont *the_font, wxColour *the_colour,
              int the_block_type, long the_block_id, int the_attribute, Bool the_vis);
  ~wxTextChunk(void);
};

class HypertextItem: public wxObject
{
  DECLARE_CLASS(HypertextItem)
 public:
  char *filename;
  long block_id;
  HypertextItem(char *new_filename, long new_block_id);
  ~HypertextItem(void);
};

// Read an integer from line, starting at next
int wxReadInteger(char *line, int next, long *value);

// Find or create a font
wxFont *wxFindOrCreateFont(int PointSize, int Family, int Style, int Weight);

#ifndef KEY_SHIFT
#define KEY_SHIFT 1
#define KEY_CTRL  2
#endif

#define BLOCK_TYPE_SELECTION 9999

#endif // hypertexth
