/*
 * File:	hytext.cc
 * Purpose:	Hypertext library for wxWindows
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

static const char sccsid[] = "%W% %G%";

#ifdef __GNUG__
#pragma implementation "hytext.h"
#endif

/*
 * hytext.cc
 * Hypertext Window implementation.
 * This is a text window with highlighted text which can respond
 * to mouse clicks. Blocks are stored in ordinary text files as:
 * \hy-A{B}{C}  where A is an integer representing the type of
 * block, B is an integer identifier for this block, and C is the
 * text enclosed by the block. Blocks may be nested.
 * Mouse input member functions are provided for overriding by an application,
 * e.g. for displaying further information.
 */

// For compilers that support precompilation, includes "wx.h".
#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "hytext.h"

// Drag states
#define NoDragging             0
#define StartDraggingLeft      1
#define ContinueDraggingLeft   2
#define StartDraggingRight     3
#define ContinueDraggingRight  4

wxBrush *wx_transparent_brush = NULL;
wxPen *wx_black_dashed_pen = NULL;

#define HYTEXT_DEFAULT_LEFT_MARGIN 10
#define HYTEXT_DEFAULT_TOP_MARGIN  5
#define LINE_BUFFER_SIZE           400

IMPLEMENT_CLASS(wxHTMappingStructure, wxObject)
IMPLEMENT_CLASS(wxHyperTextMapping, wxList)
IMPLEMENT_CLASS(wxHyperTextWindow, wxCanvas)
IMPLEMENT_CLASS(wxTextChunk, wxObject)
IMPLEMENT_CLASS(HypertextItem, wxObject)

/*
 * HyperText Mapping Structure
 *
 */

wxHTMappingStructure::wxHTMappingStructure(void)
{
  block_type = -1;
  text_size = -1;
  text_family = -1;
  text_style = -1;
  text_weight = -1;
  text_colour = NULL;
  name = NULL;
  special_attribute = -1;
  visibility = -1;
  logical_op = wxCOPY;
  background_colour = NULL;
}

wxHTMappingStructure::wxHTMappingStructure(int the_block_type,
                  int the_text_size, int the_text_family,
                  int the_text_style, int the_text_weight,
                  char *the_text_colour, char *the_name,
                  int the_attribute, Bool the_visibility)
{
  block_type = the_block_type;
  text_size = the_text_size;
  text_family = the_text_family;
  text_style = the_text_style;
  text_weight = the_text_weight;
  if (the_text_colour)
    text_colour = copystring(the_text_colour);
  else text_colour = NULL;

  if (the_name)
    name = copystring(the_name);
  else name = NULL;

  logical_op = wxCOPY;
  background_colour = NULL;
  special_attribute = the_attribute;
  visibility = the_visibility;
}

wxHTMappingStructure::~wxHTMappingStructure(void)
{
  if (name) delete[] name;
  if (text_colour) delete[] text_colour;
}

wxFont *wxHTMappingStructure::GetFont(void)
{
  wxFont *the_font = wxFindOrCreateFont(text_size, text_family, text_style, text_weight);
  return the_font;
}

wxHTMappingStructure *wxHTMappingStructure::Copy(void)
{
  wxHTMappingStructure *the_copy = new wxHTMappingStructure(block_type,
                  text_size, text_family,
                  text_style, text_weight,
                  text_colour, name, special_attribute, visibility);
  the_copy->logical_op = logical_op;
  the_copy->background_colour = background_colour;
  return the_copy;  
}

/*
 * HyperText Mapping List
 *
 */

wxHyperTextMapping::wxHyperTextMapping(void):wxList(wxKEY_INTEGER)
{
  current_mapping = NULL;

  AddMapping(BLOCK_TYPE_SELECTION, -1, -1, -1, -1, NULL, "RESERVED", -1, -1);
}

wxHyperTextMapping::~wxHyperTextMapping(void)
{
}

void wxHyperTextMapping::ClearMapping(void)
{
  wxNode *node = First();
  while (node)
  {
    wxHTMappingStructure *struc = (wxHTMappingStructure *)node->Data();
    wxNode *next_node = node->Next();
    if (strcmp(struc->name, "RESERVED") != 0)
    {
      delete struc;
      delete node;
    }
    node = next_node;
  }
}

void wxHyperTextMapping::AddMapping(int block_type, int text_size, int text_family,
                  int text_style, int text_weight, char *text_colour, 
                  char *name, int attribute, Bool visibility)
{
  wxHTMappingStructure *ms =
    new wxHTMappingStructure(block_type, text_size, text_family, text_style,
                             text_weight, text_colour, name, attribute, visibility);
  Append((long)block_type, ms);
}

Bool wxHyperTextMapping::GetMapping(int block_type, int *text_size, int *text_family,
                  int *text_style, int *text_weight, char **text_colour,
                  char **name, int *attribute, Bool *visibility)
{
  wxNode *node = Find((long)block_type);
  if (node)
  {
    wxHTMappingStructure *ms = (wxHTMappingStructure *)node->Data();
    *text_size = ms->text_size;
    *text_family = ms->text_family;
    *text_style = ms->text_style;
    *text_colour = ms->text_colour;
    *text_weight = ms->text_weight;
    *name = ms->name;
    *attribute = ms->special_attribute;
    *visibility = ms->visibility;
    return TRUE;
  }
  else return FALSE;
}

// Find a mapping structure given a name
wxHTMappingStructure *wxHyperTextMapping::FindByName(char *name)
{
  wxNode *node = First();
  wxHTMappingStructure *found = NULL;
  while (node && !found)
  {
    wxHTMappingStructure *thing = (wxHTMappingStructure *)node->Data();
    if (strcmp(name, thing->name) == 0)
      found = thing;
    else node = node->Next();
  }
  return found;
}


// Pop up dialog box for editing mappings
// TO BE IMPLEMENTED
void wxHyperTextMapping::Edit(wxWindow *parent)
{
}

// -1 if no more types
int wxHyperTextMapping::GetFirstType(void)
{
  current_mapping = First();
  if (current_mapping)
  {
    wxHTMappingStructure *ms = (wxHTMappingStructure *)current_mapping->Data();
    return ms->block_type;
  }
  else return -1;
}

int wxHyperTextMapping::GetNextType(void)
{
  if (current_mapping)
    current_mapping = current_mapping->Next();
  if (current_mapping)
  {
    wxHTMappingStructure *ms = (wxHTMappingStructure *)current_mapping->Data();
    return ms->block_type;
  }
  else return -1;
}

/*
 * wxHyperTextWindow - based on a wxCanvas: make sure you make it
 * NOT retained!
 *
 */

/* PUBLIC MEMBER FUNCTIONS */

wxHyperTextWindow::wxHyperTextWindow(wxFrame *parent, 
                                     int x, int y, int w, int h, long style):
  wxCanvas(parent, x, y, w, h, style)
{
  Title = NULL;
  indexWriting = FALSE;
  mapping = NULL;
  current_block = NULL;
  selection_current = NULL;
  no_lines = 0;
  text_chunks.DeleteContents(TRUE);
  no_displayed_lines = 0;
  default_mapping_structure = NULL;
  DragState = NoDragging;
  DraggedBlock = -1;
  old_drag_x = 0;
  old_drag_y = 0;
  first_drag_x = 0.0;
  first_drag_y = 0.0;
  mouseTolerance = DEFAULT_MOUSE_TOLERANCE;
  OutlineStartX = 0.0;
  OutlineStartY = 0.0;
  modified = FALSE;
  edit_mode = TRUE;
  current_section = NULL;
  section_start_line = 0;
  section_end_line = 0;
  LinkTable = new wxHashTable(wxKEY_INTEGER);

  hyleft_margin = HYTEXT_DEFAULT_LEFT_MARGIN;
  hytop_margin = HYTEXT_DEFAULT_TOP_MARGIN;

  transFile1 = NULL;
  transFile2 = NULL;

  EnableScrolling(TRUE, FALSE);
}

wxHyperTextWindow::~wxHyperTextWindow(void)
{
  delete LinkTable;
}

void wxHyperTextWindow::SetMapping(wxHyperTextMapping *the_mapping)
{
  mapping = the_mapping;

  default_mapping_structure = mapping->FindByName("Default");
}

Bool wxReadALine(FILE *fd, char *buf)
{
  int ch = -2;
  int i = 0;
  buf[0] = 0;
  while (ch != EOF && ch != '\n')
  {
    ch = getc(fd);
//    if (ch == 10) { }
   if (ch != EOF && ch != '\n')
    {
      buf[i] = ch;
      i ++;
    }
  }
  buf[i] = 0;
  return (ch == EOF);
}

Bool wxHyperTextWindow::ReadIndex(char *filename, FILE *fd)
{
  static char line_buffer[LINE_BUFFER_SIZE];
  Bool eof = FALSE;
  // Get title
  eof = wxReadALine(fd, line_buffer);
  for (int i = 1; i < LINE_BUFFER_SIZE-1; i ++)
    if (line_buffer[i] == '"')
      { line_buffer[i] = 0; break; }

  if (strlen(line_buffer+1) > 0)
    Title = copystring(line_buffer+1);
  while (!eof)
  {
    eof = wxReadALine(fd, line_buffer);
    if (line_buffer[0] == '}')
      return eof;
    else
    {
      long block_from, block_to;
      int next = wxReadInteger(line_buffer, 0, &block_from);
      next = wxReadInteger(line_buffer, next+1, &block_to);
      char *file = filename;
      if (strlen(line_buffer + next) >  0)
        file = line_buffer + next + 1;
      LinkTable->Put(block_from, (wxObject *)new HypertextItem(file, block_to));
    }
  }
  return eof;
}

Bool wxHyperTextWindow::LoadFile(char *filename)
{
  if (!filename)
    return FALSE;

  wxCursor *old_cursor = SetCursor(wxHOURGLASS_CURSOR);
  ::wxSetCursor(wxHOURGLASS_CURSOR);

  // Update to delete all HypertextItems!!!
  LinkTable->Clear();
  static char line_buffer[300];
  FILE *fd = fopen(filename, "r");
  if (fd)
  {
    ClearFile();
    Bool eof = FALSE;
    while (!eof)
    {
      eof = wxReadALine(fd, line_buffer);
      if ((strlen(line_buffer) > 8) && strncmp(line_buffer,"\\hyindex{", 9) == 0)
        eof = ReadIndex(filename, fd);
      else
      {
        ParseLine(line_buffer);
        no_lines ++;
      }
    }
    fclose(fd);
    Compile();
    current_section = NULL;
    section_start_line = 0;
//    section_end_line = no_lines-1;
    section_end_line = 0;

    SetCursor(old_cursor);
    ::wxSetCursor(wxSTANDARD_CURSOR);

    return TRUE;
  }
  else
  {
    SetCursor(old_cursor);
    ::wxSetCursor(wxSTANDARD_CURSOR);

    return FALSE;
  }
}

void wxHyperTextWindow::ParseLine(char *line)
{
  static char temp_buffer[300];
  int len = strlen(line);
  int i = 0;
  int ptr = 0;  // How much of temp_buffer we've used up
  char ch;
  wxTextChunk *current_chunk =
    new wxTextChunk(CHUNK_START_LINE, no_lines, NULL, NULL, NULL, -1, -1, -1, -1);
  while (i < len)
  {
    ch = line[i];
    switch (ch)
    {
      case '\\':
      {
        // Trying to parse something like \hy-123{456}{
        Bool success = FALSE;

        if ((len > i + 5) &&
            line[i+1] == 'h' && line[i+2] == 'y' && line[i+3] == '-')
        {
          int next = i + 4;
          long hy_type = 0;
          long hy_id = 0;
          next = wxReadInteger(line, next, &hy_type);
          if (next > -1 && line[next] == '{')
          {
            next = wxReadInteger(line, next+1, &hy_id);
            if (next > -1 && line[next + 1] == '{')
            {
              RegisterId(hy_id); // Ensure GetId() returns a unique Id
              i = next + 2;
              temp_buffer[ptr] = 0;
              ptr = 0;
              current_chunk->text = copystring(temp_buffer);
              text_chunks.Append(current_chunk);

              current_chunk = new wxTextChunk(CHUNK_START_BLOCK, -1, NULL, NULL, NULL,
                                              (int)hy_type, hy_id, -1, FALSE);
              success = TRUE;
            }
          }
        }
        if (!success)
        {
          temp_buffer[ptr] = '\\';
          ptr ++;
          i ++;
        }
        break;
      }
      case '{':
      {
        temp_buffer[ptr] = 0;
        ptr = 0;
        current_chunk->text = copystring(temp_buffer);
        text_chunks.Append(current_chunk);

        current_chunk = new wxTextChunk(CHUNK_START_UNRECOGNIZED_BLOCK, -1,
                                        NULL, NULL, NULL,
                                        -1, -1, -1, -1);

        temp_buffer[ptr] = '{';
        ptr ++;
        i ++;

        break;
      }
      case '}':
      {
        temp_buffer[ptr] = 0;
        ptr = 0;
        current_chunk->text = copystring(temp_buffer);
        text_chunks.Append(current_chunk);

        current_chunk = new wxTextChunk(CHUNK_END_BLOCK, -1,
                                        NULL, NULL, NULL,
                                        -1, -1, -1, -1);
//        temp_buffer[ptr] = '}';
//        ptr ++;
        i ++;

        break;
      }
      default:
      {
        temp_buffer[ptr] = ch;
        ptr ++;
        i ++;
        break;
      }
    }
  }

  temp_buffer[ptr] = 0;
  current_chunk->text = copystring(temp_buffer);
  text_chunks.Append(current_chunk);
}

void wxHyperTextWindow::DisplayFileAtTop(void)
{
  long block_id = -1;
  wxNode *node = sections.First();
  if (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    block_id = chunk->block_id;
  }
  DisplayFileAt(block_id, TRUE);
}

void wxHyperTextWindow::DisplayFileAt(long block_id, Bool refresh)
{
  wxTextChunk *first_chunk;
  wxNode *node;

  if (block_id == -1)
  {
    node = text_chunks.First();
    if (!node)
      return;
    first_chunk = (wxTextChunk *)node->Data();
  }
  else
  {
    node = FindChunkAtBlock(block_id);
    if (!node)
      return;
    first_chunk = (wxTextChunk *)node->Data();
  }

  int start_line = first_chunk->line_no;
  section_start_line = 0;   // Default
  section_end_line = no_lines - 1;   // Default
  current_section = NULL;            // Default

  // Try to find section info
  if (sections.Number() > 0)
  {
    wxNode *temp_node = node;
    wxTextChunk *found = NULL;
    // FIND SECTION START

    // May have a start of line just before the section block
    if (first_chunk->chunk_type == CHUNK_START_LINE)
      temp_node = node->Next();

    while (temp_node && !found)
    {
      wxTextChunk *chunk = (wxTextChunk *)temp_node->Data();
      if (sections.Member(chunk))
        found = chunk;
      else temp_node = temp_node->Previous();
    }
    if (!found)
    {
      temp_node = text_chunks.First();
      found = (wxTextChunk *)temp_node->Data();
    }
    if (found)
    {
      section_start_line = found->line_no;
      current_section = sections.Member(found);

      // FIND SECTION END
      if (current_section)
      {
        wxNode *end_node = current_section->Next();
        if (end_node)
        {
          wxTextChunk *chunk = (wxTextChunk *)end_node->Data();
          section_end_line = chunk->line_no - 1;
        }
        else section_end_line = no_lines - 1;
      }
      else
      {
        wxNode *next_node = sections.First();
        wxTextChunk *chunk = (wxTextChunk *)next_node->Data();
        section_end_line = chunk->line_no - 1;
      }
    }
  }
  int no_section_lines = section_end_line - section_start_line + 1;

  // Set scrollbars for this section
  SetScrollbars(10, 10, 200, no_section_lines+50, 20, 20);

  // Now scroll because we may not be viewing at this position
  if (refresh)
  {
    Scroll(0, (int)(start_line - section_start_line));
    DisplayFile();
  }
}

void wxHyperTextWindow::DisplayFile(void)
{
  if (section_end_line == 0)
    return;

  wxDC *dc = GetDC();
  dc->Clear();
  no_displayed_lines = 0;

  float max_line_height = 0.0;

  int start_x = 0;
  int start_y = 0;
  int width, height;

  ViewStart(&start_x, &start_y);
  GetClientSize(&width, &height);

  float current_x = 0.0;
  float current_y = (float)(start_y*10.0);

  int start_line_no = (int)(start_y + section_start_line);
//  float max_y = (float)(current_y + height);

  if (start_line_no > section_end_line) start_line_no = section_end_line;

  wxNode *node = FindChunkAtLine(start_line_no, NULL);

  current_y += (float)hytop_margin;

  int i = start_line_no;
//  while (node && (current_y <= max_y) && (i <= section_end_line))
  while (node && (i <= section_end_line) && (no_displayed_lines < MAX_DISPLAYED_LINES))
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    dc->SetTextForeground(chunk->colour);
    float extent_x = 0.0;
    float extent_y = 0.0;

    if (chunk->chunk_type == CHUNK_START_LINE)
    {
      i = chunk->line_no;

      // Find max. line height
      max_line_height = 0.0;
      wxNode *node1 = node;
      while (node1)
      {
        wxTextChunk *chunk1 = (wxTextChunk *)node1->Data();
        if (chunk1 != chunk && chunk1->chunk_type == CHUNK_START_LINE)
          node1 = NULL;
        else
	{
          if (chunk1->visibility || edit_mode)
	  {
            dc->SetFont(chunk1->font);
            dc->GetTextExtent(chunk1->text, &extent_x, &extent_y, NULL, NULL);
            if (extent_y > max_line_height) max_line_height = extent_y;
	  }
          node1 = node1->Next();
	}
      }
      if (max_line_height < 0.1)
      {
        dc->GetTextExtent("X", &extent_x, &max_line_height, NULL, NULL);
      }

      current_y += max_line_height;
      current_x = (float)hyleft_margin;
      displayed_lines[no_displayed_lines] = current_y;
      no_displayed_lines ++;
    }

    if (chunk->visibility || edit_mode)
    {
      dc->SetFont(chunk->font);
      if (!dc->Colour)
        dc->SetLogicalFunction(chunk->logical_op);
      if (chunk->background_colour)
        dc->SetTextBackground(chunk->background_colour);
      else
        dc->SetTextBackground(wxWHITE);

      dc->GetTextExtent(chunk->text, &extent_x, &extent_y, NULL, NULL);
      dc->DrawText(chunk->text, current_x, current_y-extent_y);
      current_x += extent_x;
    }

    node = node->Next();
  }
}

Bool wxHyperTextWindow::SaveFile(char *filename)
{
  wxCursor *old_cursor = SetCursor(wxHOURGLASS_CURSOR);
  ::wxSetCursor(wxHOURGLASS_CURSOR);

  ofstream output(filename);
  if (!output.bad())
  {
    wxNode *node = text_chunks.First();
    while (node)
    {
      wxTextChunk *chunk = (wxTextChunk *)node->Data();
      switch (chunk->chunk_type)
      {
        case CHUNK_START_LINE:
        {
          if (node != text_chunks.First())
            output << '\n';
          output << chunk->text;
          break;
        }
        case CHUNK_START_BLOCK:
        {
          output << "\\hy-" << chunk->block_type << "{"
                 << chunk->block_id << "}{" << chunk->text;
          break;
        }
        case CHUNK_END_BLOCK:
        {
          if (chunk->end_id > -1)
            output << "}";
          output << chunk->text;
          break;
        }
        case CHUNK_START_UNRECOGNIZED_BLOCK:
        default:
        {
          output << chunk->text;
          break;
        }
      }
      node = node->Next();
    }

    if (indexWriting)
    {
      // Now output link table, if more than 0
      LinkTable->BeginFind();
      if (LinkTable->Next() || Title)
      {
        output << "\n\\hyindex{\n";
        output << '"';
        if (Title) output << Title;
        output << "\"\n";
        LinkTable->BeginFind();
        wxNode *node;
        while (node = LinkTable->Next())
        {
          HypertextItem *item = (HypertextItem *)node->Data();
          output << node->key.integer << " " << item->block_id;
          if (item->filename && (strcmp(item->filename, filename) != 0))
            output << " " << item->filename;
          output << "\n";
        }
        output << "}\n";
      }
    }

    modified = FALSE;
    SetCursor(old_cursor);
    ::wxSetCursor(wxSTANDARD_CURSOR);

    return TRUE;
  }
  else
  {
    SetCursor(old_cursor);
    ::wxSetCursor(wxSTANDARD_CURSOR);

    return FALSE;
  }
}

Bool wxHyperTextWindow::ClearFile(void)
{
  current_selections.Clear();
  text_chunks.Clear();
  selection_current = NULL;
  no_lines = 0;
  Clear();
  // Should reset scrollbar
  return TRUE;
}

Bool wxHyperTextWindow::GetEditMode(void)
{
  return edit_mode;
}

void wxHyperTextWindow::SetEditMode(Bool the_edit_mode)
{
  edit_mode = the_edit_mode;
}

Bool wxHyperTextWindow::Modified(void)
{
  return modified;
}

void wxHyperTextWindow::DiscardEdits(void)
{
  modified = FALSE;
}

long wxHyperTextWindow::GenerateId(void)
{
  return NewId();
}

void wxHyperTextWindow::OnPaint(void)
{
  DisplayFile();
}

/*
 * Finds x, y character position from mouse click position,
 * returning block id if any.
 * Returns FALSE if fails.
 */
Bool wxHyperTextWindow::FindPosition(float mouse_x, float mouse_y, 
                                     int *char_pos, int *line_pos,
                                     long *block_id)
{
  int current_x, current_y;
  ViewStart(&current_x, &current_y);
//  if (current_y > no_displayed_lines - 1)
//    return FALSE;

  float current_y_pixels = (float)(current_y*10);
  int line_found = -1;

  // Find line position
  for (int i = 0; i < no_displayed_lines; i++)
  {
    if (mouse_y <= displayed_lines[i] && mouse_y > current_y_pixels)
    {
      line_found = current_y + i + section_start_line;
      break;
    } else current_y_pixels = (float)displayed_lines[i];
  }

  if (line_found == -1)
    return FALSE;

  // Now find character position and block id (if any)
  wxNode *chunk_node = FindChunkAtLine(line_found, NULL);
  int char_found = -1;
  long block_found = -1;
  current_x = (int)hyleft_margin;
  wxNode *node = chunk_node;
  wxDC *dc = GetDC();
  int char_pos1 = 0;
  while (node && char_found == -1)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    // Next line so give up
    if (chunk->chunk_type == CHUNK_START_LINE && node != chunk_node)
      node = NULL;
    else
    {
      dc->SetFont(chunk->font);
      int len = strlen(chunk->text);
      float x_extent, y_extent;
      for (int i = 0; i < len; i++)
      {
        char buf[2];
        buf[0] = chunk->text[i];
        buf[1] = 0;
        dc->GetTextExtent(buf, &x_extent, &y_extent, NULL, NULL);
        if (mouse_x >= current_x && mouse_x < (current_x + x_extent))
	{
          char_found = char_pos1;
          block_found = chunk->block_id;
          break;
	}
        else
	{
          current_x += (int)x_extent;
          char_pos1 ++;
	}
      }
      node = node->Next();
    }
  }
  if (char_found == -1)
    return FALSE;

  *char_pos = char_found;
  *line_pos = line_found;
  *block_id = block_found;
  return TRUE;
}

void wxHyperTextWindow::OnLeftClick(float x, float y, int char_pos, int line_pos, long block_id, int keys)
{
//  cout << "Left click at " << char_pos << ", " << line_pos << ": block id = " << block_id << "\n";
  SetFocus();  // Capture keyboard focus
  
  if (!edit_mode)
    return;

  if ((keys & KEY_SHIFT) && block_id > -1)
  {
    wxTextChunk *chunk = FindBlock(block_id);
    if (chunk)
    {
      SaveSection();
      SelectBlock(chunk, !chunk->selected);
      Compile();
      RestoreSection();
    }
  }
}

void wxHyperTextWindow::OnRightClick(float x, float y, int char_pos, int line_pos, long block_id, int keys)
{
  SetFocus();  // Capture keyboard focus
//  cout << "Right click at " << char_pos << ", " << line_pos << ": block id = " << block_id << "\n";
}

void wxHyperTextWindow::OnBeginDragLeft(float x, float y, long block_id, int keys)
{
  SetFocus();  // Capture keyboard focus

  if (!edit_mode)
    return;

  OutlineStartX = x;
  OutlineStartY = y;

  GetDC()->SetLogicalFunction(wxXOR);
  DrawOutline(OutlineStartX, OutlineStartY, x, y);
}

void wxHyperTextWindow::OnDragLeft(Bool draw, float x, float y, long block_id, int keys)
{
  if (!edit_mode)
    return;

  DrawOutline(OutlineStartX, OutlineStartY, x, y);
}

void wxHyperTextWindow::OnEndDragLeft(float x, float y, long block_id, int keys)
{
  if (!edit_mode)
    return;

  GetDC()->SetLogicalFunction(wxCOPY);

  int char_pos1, line_pos1;
  int char_pos2, line_pos2;
  long block_id1;

  float min_x, max_x, min_y, max_y;
  min_x = wxMin(x, OutlineStartX);
  max_x = wxMax(x, OutlineStartX);
  min_y = wxMin(y, OutlineStartY);
  max_y = wxMax(y, OutlineStartY);

  if (!FindPosition(min_x, min_y, &char_pos1, &line_pos1, &block_id1))
    return;
  if (!FindPosition(max_x, max_y, &char_pos2, &line_pos2, &block_id1))
    return;

  long id = GenerateId();
  if (!AddBlock(char_pos1, line_pos1, char_pos2, line_pos2,
                BLOCK_TYPE_SELECTION, id))
    return;

  SelectBlock(id, TRUE);

//  cout << "Added block " << id << "\n";
  SaveSection();
  Compile();
  RestoreSection();
//  DisplayFile();
}

void wxHyperTextWindow::OnBeginDragRight(float x, float y, long block_id, int keys)
{
//  cout << "Begin right drag: block id = " << block_id << "\n";
}

void wxHyperTextWindow::OnDragRight(Bool draw, float x, float y, long block_id, int keys)
{
//  cout << "On right drag: block id = " << block_id << "\n";
}

void wxHyperTextWindow::OnEndDragRight(float x, float y, long block_id, int keys)
{
//  cout << "End right drag: block id = " << block_id << "\n";
}

void wxHyperTextWindow::OnEvent(wxMouseEvent& event)
{
  float x, y;
  event.Position(&x, &y);
  int keys = 0;
  if (event.ShiftDown())
    keys = keys | KEY_SHIFT;
  if (event.ControlDown())
    keys = keys | KEY_CTRL;

  Bool dragging = event.Dragging();

  // Check if we're within the tolerance for mouse movements.
  // If we're very close to the position we started dragging
  // from, this may not be an intentional drag at all.
  if (dragging)
  {
    wxDC *dc = GetDC();
    int dx = abs(dc->LogicalToDeviceX(x - first_drag_x));
    int dy = abs(dc->LogicalToDeviceY(y - first_drag_y));
    if ((dx <= mouseTolerance) && (dy <= mouseTolerance))
      return;
  }

  int char_pos, line_pos;
  long block_id;

  // All following events sent to canvas
  if (dragging && DragState == StartDraggingLeft)
  {
    DragState = ContinueDraggingLeft;
    OnBeginDragLeft((float)x, (float)y, DraggedBlock, keys);
    old_drag_x = x; old_drag_y = y;
  }
  else if (dragging && DragState == ContinueDraggingLeft)
  { 
    // Continue dragging
    OnDragLeft(FALSE, old_drag_x, old_drag_y, DraggedBlock, keys);
    OnDragLeft(TRUE, (float)x, (float)y, DraggedBlock, keys);
    old_drag_x = x; old_drag_y = y;
  }
  else if (event.LeftUp() && DragState == ContinueDraggingLeft)
  {
    DragState = NoDragging;
    OnDragLeft(FALSE, old_drag_x, old_drag_y, DraggedBlock, keys);
    OnEndDragLeft((float)x, (float)y, DraggedBlock, keys);
//    DraggedBlock = -1;
  }
  else if (dragging && DragState == StartDraggingRight)
  {
    DragState = ContinueDraggingRight;
    OnBeginDragRight((float)x, (float)y, DraggedBlock, keys);
    old_drag_x = x; old_drag_y = y;
  }
  else if (dragging && DragState == ContinueDraggingRight)
  { 
    // Continue dragging
    OnDragRight(FALSE, old_drag_x, old_drag_y, DraggedBlock, keys);
    OnDragRight(TRUE, (float)x, (float)y, DraggedBlock, keys);
    old_drag_x = x; old_drag_y = y;
  }
  else if (event.RightUp() &&  DragState == ContinueDraggingRight)
  {
    DragState = NoDragging;

    OnDragRight(FALSE, old_drag_x, old_drag_y, DraggedBlock, keys);
    OnEndDragRight((float)x, (float)y, DraggedBlock, keys);
  }

  // Non-dragging events
  else if (event.IsButton())
  {
    // Find block (if any)
    if (!FindPosition(x, y, &char_pos, &line_pos, &block_id))
      return;

    if (event.LeftDown())
    {
      DraggedBlock = block_id;
      DragState = StartDraggingLeft;
      first_drag_x = x;
      first_drag_y = y;
    }
    else if (event.LeftUp())
    {
      // N.B. Only register a click if the same object was
      // identified for down *and* up.
      if (block_id == DraggedBlock)
        OnLeftClick((float)x, (float)y, char_pos, line_pos, block_id, keys);

      DraggedBlock = -1;
      DragState = NoDragging;
    }
    else if (event.RightDown())
    {
      DraggedBlock = block_id;
      DragState = StartDraggingRight;
      first_drag_x = x;
      first_drag_y = y;
    }
    else if (event.RightUp())
    {
      if (block_id == DraggedBlock)
        OnRightClick((float)x, (float)y, char_pos, line_pos, block_id, keys);

      DraggedBlock = -1;
      DragState = NoDragging;
    }
  }
}


/* Add a block, giving:
 * - start and end coordinates in character/line positions
 * - block type (arbitrary ID)
 * - integer block ID
 * Insert START_BLOCK before first char, insert END_BLOCK after
 * last char.
 */
Bool wxHyperTextWindow::AddBlock(int xstart, int ystart, int xend, int yend,
              int block_type, long id)
{
  wxNode *start_node = FindChunkAtLine(ystart, NULL);
  wxNode *end_node = FindChunkAtLine(yend, NULL);
  if (!(start_node && end_node))
    return FALSE;

  // Find which existing chunk we have to carve up
  wxNode *node = start_node;
  int char_pos = 0;
  static char buffer[300];
  Bool success = FALSE;
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    char *text = chunk->text;
    int len = strlen(text);
    if (xstart < (char_pos + len))
    {
      // Break up this chunk
      int split_point = xstart - char_pos;
      strncpy(buffer, text, split_point);
      buffer[split_point] = 0;
      char *new_text1 = copystring(buffer);
      chunk->text = new_text1;

      strncpy(buffer, text + split_point, len - split_point);
      buffer[len-split_point] = 0;
      delete[] text;

      wxTextChunk *new_chunk =
         new wxTextChunk(CHUNK_START_BLOCK, -1, buffer, NULL, NULL,
                         block_type, id, -1, -1);
      wxNode *next_node = node->Next();
      if (!next_node)
        text_chunks.Append(new_chunk);
      else text_chunks.Insert(next_node, new_chunk);

      success = TRUE;
      node = NULL;
    }
    else
    {
      char_pos += len;
      node = node->Next();
    }
  }
  if (!success)
    return FALSE;

  // Insert end block chunk
  node = end_node;
  char_pos = 0;
  success = FALSE;
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    char *text = chunk->text;
    int len = strlen(text);
    if (xend < (char_pos + len))
    {
      // Break up this chunk
      int split_point = xend - char_pos + 1;  // Add 1 since we inserting
                                                // AFTER this character
      strncpy(buffer, text, split_point);
      buffer[split_point] = 0;
      char *new_text1 = copystring(buffer);
      chunk->text = new_text1;

      strncpy(buffer, text + split_point, len - split_point);
      buffer[len-split_point] = 0;
      delete[] text;

      wxTextChunk *new_chunk =
         new wxTextChunk(CHUNK_END_BLOCK, -1, buffer, NULL, NULL,
                         block_type, id, -1, -1);
      wxNode *next_node = node->Next();
      if (!next_node)
        text_chunks.Append(new_chunk);
      else text_chunks.Insert(next_node, new_chunk);

      success = TRUE;
      node = NULL;
    }
    else
    {
      char_pos += len;
      node = node->Next();
    }
  }
  return success;
}

Bool wxHyperTextWindow::ClearBlock(long id)
{
  static char buffer[400];

  wxNode *first_node = FindChunkAtBlock(id);
  if (!first_node)
    return FALSE;

//  SelectBlock(id, FALSE); // Would make SelectBlock recurse if selection block type!

  // First find start of block and delete it.
  wxNode *block_node = NULL;
  while (!block_node && first_node)
  {
    wxTextChunk *chunk = (wxTextChunk *)first_node->Data();
    if (chunk->chunk_type == CHUNK_START_BLOCK && chunk->block_id == id)
    {
      block_node = first_node;
    }
    else first_node = first_node->Next();
  }
  if (!block_node)
    return FALSE;

  wxTextChunk *first_chunk = (wxTextChunk *)block_node->Data();
  wxNode *previous_node = block_node->Previous();
  if (!previous_node)
    return FALSE;

  wxTextChunk *previous_chunk = (wxTextChunk *)previous_node->Data();

  strcpy(buffer, previous_chunk->text);
  strcat(buffer, first_chunk->text);
  delete[] previous_chunk->text;
  previous_chunk->text = copystring(buffer);

  // Before we delete the chunk, make sure we're not displaying
  // this section!
  if (current_section && (first_chunk == (wxTextChunk *)current_section->Data()))
  {
    current_section = current_section->Previous();
  }

  text_chunks.DeleteObject(first_chunk);

  // Now find end of block and delete that too.
  block_node = NULL;
  first_node = previous_node;
  while (!block_node && first_node)
  {
    wxTextChunk *chunk = (wxTextChunk *)first_node->Data();
    if (chunk->chunk_type == CHUNK_END_BLOCK && chunk->end_id == id)
    {
      block_node = first_node;
    }
    else first_node = first_node->Next();
  }

  wxTextChunk *last_chunk = (wxTextChunk *)block_node->Data();
  previous_node = block_node->Previous();
  if (!previous_node)
    return FALSE;

  previous_chunk = (wxTextChunk *)previous_node->Data();

  strcpy(buffer, previous_chunk->text);
  strcat(buffer, last_chunk->text);
  delete[] previous_chunk->text;
  previous_chunk->text = copystring(buffer);

  text_chunks.DeleteObject(last_chunk);
  modified = TRUE;
  return TRUE;
}

// Get blocks in this file (returns -1 for no more)
long wxHyperTextWindow::GetFirstBlock(void)
{
  current_block = text_chunks.First();
  wxTextChunk *found = NULL;

  while (current_block && !found)
  {
    wxTextChunk *chunk = (wxTextChunk *)current_block->Data();
    if (chunk->chunk_type == CHUNK_START_BLOCK)
      found = chunk;
    else current_block = current_block->Next();
  }
  if (found)
    return found->block_id;
  else return -1;
}

long wxHyperTextWindow::GetNextBlock(void)
{
  if (!current_block)
    return -1;

  current_block = current_block->Next();
  wxTextChunk *found = NULL;

  while (current_block && !found)
  {
    wxTextChunk *chunk = (wxTextChunk *)current_block->Data();
    if (chunk->chunk_type == CHUNK_START_BLOCK)
      found = chunk;
    else current_block = current_block->Next();
  }
  if (found)
    return found->block_id;
  else return -1;
}


void wxHyperTextWindow::DrawOutline(float x1, float y1, float x2, float y2)
{
  wxDC *dc = GetDC();

/*
  if (!wx_black_dashed_pen)
    wx_black_dashed_pen = new wxPen("BLACK", 1, wxSHORT_DASH);
  dc->SetPen(wx_black_dashed_pen);
*/
  dc->SetPen(wxBLACK_PEN);

  if (!wx_transparent_brush)
    wx_transparent_brush = new wxBrush("BLACK", wxTRANSPARENT);
  dc->SetBrush(wx_transparent_brush);

  dc->DrawLine(x1, y1, x2, y1);
  dc->DrawLine(x2, y1, x2, y2);
  dc->DrawLine(x2, y2, x1, y2);
  dc->DrawLine(x1, y2, x1, y1);
}

/*
void wxHyperTextWindow::SetCurrentBlockType(int type)
{
  current_block_type = type;
}
*/

void wxHyperTextWindow::SetBlockType(long block_id, int block_type)
{
  wxTextChunk *chunk = FindBlock(block_id);
  if (chunk) chunk->block_type = block_type;
  modified = TRUE;
}

int wxHyperTextWindow::GetBlockType(long block_id)    // Gets block type
{
  wxTextChunk *chunk = FindBlock(block_id);
  if (chunk) return chunk->block_type;
  return -1;
}

void wxHyperTextWindow::GetBlockText(char *buffer, int max_size, long block_id) // Gets plain text
{
  wxNode *node = FindChunkAtBlock(block_id);
  if (!node)
  {
    buffer[0] = 0;
    return;
  }
  GetBlockText(buffer, max_size, node, block_id);
}

void wxHyperTextWindow::GetBlockText(char *buffer, int max_size, wxNode *node, long block_id) // Gets plain text
{
  int i = 0;
  buffer[0] = 0;
  if (!node)
    return;

  Bool started = FALSE;
  while (node && i <= max_size)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if (!started && (chunk->chunk_type == CHUNK_START_BLOCK && chunk->block_id == block_id))
    {
      started = TRUE;
    }
    if (started && (chunk->chunk_type == CHUNK_END_BLOCK && chunk->end_id == block_id))
      node = NULL;
    else if (started)
    {
      int len = strlen(chunk->text);
      if ((i + len) <= max_size)
      {
        strcat(buffer, chunk->text);
        i += len;
        node = node->Next();
      }
      else node = NULL;
    } else node = node->Next();
  }
  buffer[i] = 0;
}

void wxHyperTextWindow::SetMargins(int left, int top)
{
  hyleft_margin = left;
  hytop_margin = top;
}

// Traverses chunks assigning fonts/colours based on mapping.
// Uses a stack to determine the scope of the attributes,
// so when a block scope ends (with a }) the previous scope's attributes
// come into play.
// This scoping scheme is 'compiled' into the chunks so all the
// repaint proc has to do is scan through the chunks setting the
// font and colour according to the info in each chunk.

/*
 * NEED TO have a structure holding block id and wxHTMappingStructure,
 * so can assign id to end of block.
 */

class wxHTStackThing: public wxObject
{
 public:
  long block_id;
  wxHTMappingStructure *mapping;
};

Bool wxHyperTextWindow::Compile(void)
{
  long currentSectionId = -1;
  current_section = NULL;
  sections.Clear();

  wxList stack;

  // Put first item on stack, for default font.
  wxHTMappingStructure *default_mapping = NULL;

  wxHTMappingStructure *first_mapping;

  if (default_mapping_structure)
    first_mapping = default_mapping_structure->Copy();
  else
  {
    default_mapping = new wxHTMappingStructure(-1,
                  12, wxSWISS, wxNORMAL, wxNORMAL, "BLACK", "Default");
    first_mapping = default_mapping->Copy();
  }

  wxHTStackThing *first_thing = new wxHTStackThing;
  first_thing->block_id = -1;
  first_thing->mapping = first_mapping;

  stack.Append(first_thing);

  wxHTMappingStructure *current_mapping = first_mapping;

  int line_no = 0;
  wxNode *node = text_chunks.First();
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    switch (chunk->chunk_type)
    {
      case CHUNK_START_BLOCK:
      {
        wxNode *node1 = mapping->Find(chunk->block_type);
        if (node1)
	{
          wxHTMappingStructure *mapping1 =
            (wxHTMappingStructure *)node1->Data();
          if (mapping1)
	  {
            // Must make a copy of the mapping so we don't overwrite
            // defaults. This COPY has to be put on the stack so
            // as we unwind, we can set the attributes back to previous values.
            int old_text_size = current_mapping->text_size;
            int old_text_style = current_mapping->text_style;
            int old_text_weight = current_mapping->text_weight;
            int old_text_family = current_mapping->text_family;
            int old_logical_op = current_mapping->logical_op;
            Bool old_visibility = current_mapping->visibility;
//            int old_attribute = current_mapping->special_attribute;

            char *old_text_colour = current_mapping->text_colour;
            wxColour *old_background_colour = current_mapping->background_colour;

            current_mapping = mapping1->Copy();

            // Fill in any missing values
            if (current_mapping->text_size < 0)
              current_mapping->text_size = old_text_size;
            if (current_mapping->text_style < 0)
              current_mapping->text_style = old_text_style;
            if (current_mapping->text_weight < 0)
              current_mapping->text_weight = old_text_weight;
            if (current_mapping->text_family < 0)
              current_mapping->text_family = old_text_family;
            if (current_mapping->visibility < 0)
              current_mapping->visibility = old_visibility;
//            if (current_mapping->special_attribute < 0)
//              current_mapping->special_attribute = old_attribute;
            if (!current_mapping->text_colour && old_text_colour)
              current_mapping->text_colour = copystring(old_text_colour);

            // Only this chunk should get a special attribute
            chunk->special_attribute = mapping1->special_attribute;

            // Starting a selected block will cause everything after it
            // to have a background, since no other block has one
            if (chunk->selected)
            {
              if (!GetDC()->Colour)
                current_mapping->background_colour = wxWHITE;
              else current_mapping->background_colour = wxCYAN;

              current_mapping->logical_op = wxINVERT;
            }
            else
            {
              current_mapping->background_colour = old_background_colour;
              current_mapping->logical_op = old_logical_op;
            }
	  }
	}

        wxHTStackThing *thing = new wxHTStackThing;
        thing->block_id = chunk->block_id;
        thing->mapping = current_mapping;

        stack.Append(thing);
        break;
      }

      case CHUNK_START_UNRECOGNIZED_BLOCK:
      {
        wxHTStackThing *thing = new wxHTStackThing;
        thing->block_id = -1;
        thing->mapping = current_mapping->Copy();

        stack.Append(thing);
        break;
      }

      case CHUNK_END_BLOCK:
      {
        if (stack.Number() > 1)
        {
          wxNode *node1 = stack.Last();
          // Assign the block id from the starting chunk to
          // this end chunk, so we can recognize both start and end
          // chunks. Also, end braces from unrecognized constructs
          // can be displayed, and recognized block braces NOT.
          wxHTStackThing *thing = (wxHTStackThing *)node1->Data();
          chunk->end_id = thing->block_id;

          // If unrecognised block, stick right brace back into text
          if (chunk->end_id == -1)
          {
            if ((strlen(chunk->text) == 0) || (chunk->text[0] != '}'))
            {
              static char buf[301];
              buf[0] = '}'; buf[1] = 0;
              strncat(buf, chunk->text, 300);
              delete[] chunk->text;
              chunk->text = copystring(buf);
            }
          }

          delete node1;
          if (thing->mapping != first_mapping)
            delete thing->mapping;
          delete thing;

          node1 = stack.Last();
          thing = (wxHTStackThing *)node1->Data();
          chunk->block_id = thing->block_id;
          current_mapping = thing->mapping;
        }
        break;
      }

      case CHUNK_START_LINE:
      default:
      {
        line_no = chunk->line_no;

        wxNode *node1 = stack.Last();
        wxHTStackThing *thing = (wxHTStackThing *)node1->Data();
        chunk->block_id = thing->block_id;

        // In case we're recompiling because a section has changed,
        // and will redisplay at same section after this.
        if (currentSectionId == saved_block_id)
	{
          section_end_line = line_no;
	}

        break;
      }
    }
    if (current_mapping->text_colour)
      chunk->colour = wxTheColourDatabase->FindColour(current_mapping->text_colour);
    chunk->background_colour = current_mapping->background_colour;
    chunk->font = current_mapping->GetFont();
    chunk->logical_op = current_mapping->logical_op;
//    chunk->special_attribute = current_mapping->special_attribute;
    chunk->visibility = current_mapping->visibility;
    chunk->line_no = line_no;
    if (chunk->special_attribute == wxHYPER_SECTION)
    {
      currentSectionId = chunk->block_id;
      sections.Append(chunk);
    }

    node = node->Next();
  }
  if (default_mapping) delete default_mapping;

  delete first_mapping;
  return TRUE;
}

// Find first chunk at given line (returns a node so can traverse
// forward from here)
wxNode *wxHyperTextWindow::FindChunkAtLine(int line_no, wxNode *startNode)
{
  wxNode *node = startNode ? startNode : text_chunks.First();
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if (chunk->line_no == line_no)
      return node;
    else node = node->Next();
  }
  return NULL;
}

// Finds first line-start chunk containing the given block
// (NOT the actual chunk containing the block)
wxNode *wxHyperTextWindow::FindChunkAtBlock(long block_id)
{
  wxNode *node = text_chunks.First();
  wxNode *found = NULL;
  wxNode *last_starting_chunk = NULL;
  while (node && !found)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if (chunk->chunk_type == CHUNK_START_LINE)
      last_starting_chunk = node;

    if ((chunk->chunk_type == CHUNK_START_BLOCK) && (chunk->block_id == block_id))
      found = last_starting_chunk;
    else node = node->Next();
  }
  return found;
}

// Finds chunk at given block
wxTextChunk *wxHyperTextWindow::FindBlock(long block_id)
{
  wxNode *node = text_chunks.First();
  wxTextChunk *found = NULL;
  while (node && !found)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if ((chunk->chunk_type == CHUNK_START_BLOCK) && (chunk->block_id == block_id))
      found = chunk;
    else node = node->Next();
  }
  return found;
}

long wxHyperTextWindow::FindBlockForSection(wxNode *node)
{
  if (!node)
    return -1;
  wxTextChunk *chunk = (wxTextChunk *)node->Data();
  return chunk->block_id;
}

// Select/deselect block.
// Need to call DisplayFile explicitly before screen is updated.
void wxHyperTextWindow::SelectBlock(wxTextChunk *chunk, Bool select)
{
  if (chunk)
  {
    chunk->selected = select;
    if (select)
    {
      current_selections.Append((wxObject *)chunk->block_id);
      OnSelectBlock(chunk->block_id, select);
    }
    else
    {
      if (chunk->block_type == BLOCK_TYPE_SELECTION)
        ClearBlock(chunk->block_id);
      current_selections.DeleteObject((wxObject *)chunk->block_id);
      OnSelectBlock(chunk->block_id, select);
    }
  }
}

void wxHyperTextWindow::SelectBlock(long block_id, Bool select)
{
  wxTextChunk *chunk = FindBlock(block_id);
  if (chunk) SelectBlock(chunk, select);
}

void wxHyperTextWindow::OnSelectBlock(long block_id, Bool select)
{
}

long wxHyperTextWindow::GetFirstSelection(void)
{
  selection_current = current_selections.First();
  if (selection_current)
  {
    long id = (long)selection_current->Data();
    selection_current = selection_current->Next();
    return id;
  } else return -1;
}

long wxHyperTextWindow::GetNextSelection(void)
{
  if (selection_current)
  {
    long id = (long)selection_current->Data();
    selection_current = selection_current->Next();
    return id;
  }  else return -1;
}

void wxHyperTextWindow::DisplayNextSection(void)
{
  if (current_section)
  {
    wxNode *next = current_section->Next();
    if (!next)
      return;
    current_section = next;
    wxTextChunk *chunk = (wxTextChunk *)current_section->Data();
    DisplayFileAt(chunk->block_id, TRUE);
  } else if (sections.Number() > 0)
  {
    current_section = sections.First();
    wxTextChunk *chunk = (wxTextChunk *)current_section->Data();
    DisplayFileAt(chunk->block_id, TRUE);
  }
}

void wxHyperTextWindow::DisplayPreviousSection(void)
{
  if (current_section)
  {
    wxNode *prev = current_section->Previous();
    if (prev)
    {
      current_section = prev;
      wxTextChunk *chunk = (wxTextChunk *)current_section->Data();
      DisplayFileAt(chunk->block_id, TRUE);
    }
    else DisplayFileAt(-1, TRUE);
  }
}

int wxHyperTextWindow::GetCurrentSectionNumber(void)
{
  if (current_section)
  {
    int i = 1;
    wxNode *node = sections.First();
    while (node)
    {
      if (node == current_section)
        return i;
      else
        node = node->Next();
      i ++;
    }
    return 0;
  }
  return 0;
}

// Save the block id of the current section, since current_section
// is always set to NULL during a compile, and the display must be
// restored.
void wxHyperTextWindow::SaveSection(void)
{
  ViewStart(&saved_x, &saved_y);

  if (current_section)
  {
    wxTextChunk *chunk = (wxTextChunk *)current_section->Data();
    saved_block_id = chunk->block_id;
  } else saved_block_id = -1;
}

void wxHyperTextWindow::RestoreSection(void)
{
  // See if the section is still there
  wxNode *node = sections.First();
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if (chunk->block_id == saved_block_id)
    {
      DisplayFileAt(chunk->block_id, FALSE);
      node = NULL;
    }
    if (node) node = node->Next();
  }
  Scroll(saved_x, saved_y);
  DisplayFile();

//  DisplayFileAtTop();
}

// A chunk is not a block, since it may signify the start of a
// line as well as a block boundary.
wxTextChunk::wxTextChunk(int the_chunk_type, int the_line_no, char *the_text,
                         wxFont *the_font, wxColour *the_colour,
                         int the_block_type, long the_block_id,
                         int the_attribute, Bool the_visibility)
{
  chunk_type = the_chunk_type;
  line_no = the_line_no;
  if (the_text) text = copystring(the_text);
    else text = NULL;
  font = the_font;
  colour = the_colour;
  block_type = the_block_type;
  block_id = the_block_id;

  logical_op = wxCOPY;
  background_colour = NULL;
  selected = FALSE;
  special_attribute = the_attribute;
  visibility = the_visibility;
}

wxTextChunk::~wxTextChunk(void)
{
  if (text) delete[] text;
}

HypertextItem::HypertextItem(char *new_filename, long new_block)
{
  if (new_filename)
    filename = copystring(new_filename);
  else filename = NULL;
  block_id = new_block;
}

HypertextItem::~HypertextItem(void)
{
  if (filename)
    delete[] filename;
}

// Read an integer from the buffer, returning next index if
// successful, -1 if not.
int wxReadInteger(char *line, int next, long *value)
{
  long the_value = 0;
  char digits[10];
  int no_digits = 0;
  int next1 = next;
  while ((next1 < next + 10) && isdigit(line[next1]))
  {

    digits[no_digits] = line[next1];
    no_digits ++;
    next1 ++;
  }
  if (no_digits == 0)
    return -1;
  else
  {
    for (int i = 0; i < no_digits; i++)
      the_value += (long)(((int)digits[i] - 48) * pow(10, no_digits - i - 1));
    *value = the_value;
    return next1;
  }
}

/*
 * Font stuff
 *
 */

wxFont *wxFindOrCreateFont(int PointSize, int Family, int Style, int Weight)
{
  wxNode *node = wxTheFontList->First();
  wxFont *font = NULL;
  while (node && !font)
  {
    wxFont *the_font = (wxFont *)node->Data();
    if (the_font->GetFamily() == Family && the_font->GetPointSize() == PointSize
        && the_font->GetStyle() == Style && the_font->GetWeight() == Weight)
      font = the_font;
    else node = node->Next();
  }
  if (font)
    return font;
  else
    return new wxFont(PointSize, Family, Style, Weight);
}

/*
 * Various accessors/query functions
 *
 */

// Get the number of lines
int wxHyperTextWindow::NoLines(void)
{
  return no_lines;
}

// Returns the line length in characters, starting from line zero
int wxHyperTextWindow::LineLength(int line, wxNode *startNode)
{
  wxNode *node = FindChunkAtLine(line, startNode);
  if (!node)
    return -1;

  int lineLength = 0;

  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if ((chunk->line_no != -1) && (chunk->line_no != line))
      node = NULL;
    else
    {
      lineLength += strlen(chunk->text);
      node = node->Next();
    } 
  }
  return lineLength;
}

// Returns the line number and character position for a character offset from
// a line/character position.
// Returns FALSE if did not succeed.
Bool wxHyperTextWindow::GetOffsetPosition(int line1, int char1, int offset,
                                          int *line2, int *char2, wxNode *startNode)
{
  wxNode *node = FindChunkAtLine(line1, startNode);
  if (!node)
    return FALSE;

  int currentLine = line1;
  int currentChar = char1;
  Bool started = FALSE;
  int currentOffset = offset;
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if (currentOffset <= 0)
      node = NULL;
    else if (!started)
    {
      if (chunk->line_no > -1)
        currentLine = chunk->line_no;

      int len = strlen(chunk->text);

      int charsToCopy = len;

      if (char1 <= (len + currentChar - 1))
      {
        started = TRUE;

        // See if we're on the end-line already
        if (currentOffset < (len - (char1 - currentChar)))
          charsToCopy = currentOffset;
      }
      currentChar += charsToCopy;
      currentOffset -= charsToCopy;
    }
    else
    {
      // Check if we've got a newline, in which case put a space in the buffer
      if (chunk->chunk_type == CHUNK_START_LINE)
      {
        currentLine = chunk->line_no;
        currentChar = 0;
//        currentOffset -= 1;
      }
      int len = strlen(chunk->text);

      // See if there's a limit to the number of chars to copy
      int charsToCopy = len;

      // See if we're on the end-line already
      if (currentOffset < len)
        charsToCopy = currentOffset;

      currentChar += charsToCopy;
      currentOffset -= charsToCopy;
    }
    if (node)
      node = node->Next();
  }
  if (currentChar > 0)
    currentChar --;
  *line2 = currentLine;
  *char2 = currentChar;
  return TRUE;
}

// Get the text between the given points, converting newlines to spaces
void wxHyperTextWindow::GetSpanText(char *buffer, int bufSize, int line1, int char1,
                                  int line2, int char2, Bool convertNewLines, wxNode *startNode)
{
  wxNode *node = FindChunkAtLine(line1, startNode);
  if (!node)
    return;

  Bool started = FALSE;
  int i = 0;  // Pointer to current position in buffer
  int currentChar = 0;
  int currentLine = line1;
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if ((currentLine >= line2) && (currentChar > char2))
      node = NULL;
    else if (!started)
    {
      if (chunk->line_no > -1)
        currentLine = chunk->line_no;

      int len = strlen(chunk->text);
      int charsToCopy = len;
      // See if we can start copying characters to the buffer
      if (char1 <= (len + currentChar - 1))
      {
        started = TRUE;

        // See if there's a limit to the number of chars to copy

        if ((currentLine == line2) && (char2 < (len + currentChar - 1)))
          charsToCopy = char2 - currentChar + 1;
        
        for (int j = (char1 - currentChar); j < charsToCopy; j++)
	{
          buffer[i] = chunk->text[j];
          if (i < (bufSize - 1))
            i ++;
	}
      }
      currentChar += charsToCopy;
    }
    else
    {
      // Check if we've got a newline, in which case put a space in the buffer
      if ((chunk->chunk_type == CHUNK_START_LINE) && (chunk->line_no <= line2))
      {
        currentLine = chunk->line_no;
        currentChar = 0;
        if (i < (bufSize - 1))
        {
          if (convertNewLines)
            buffer[i] = ' ';
          else
            buffer[i] = 10;
          i ++;
        }
      }
      int len = strlen(chunk->text);

      // See if there's a limit to the number of chars to copy
      int charsToCopy = len;
      if ((currentLine == line2) && (char2 < (len + currentChar - 1)))
        charsToCopy = char2 - currentChar + 1;

      for (int j = 0; j < charsToCopy; j++)
      {
        buffer[i] = chunk->text[j];
        if (i < (bufSize - 1))
          i ++;
      }

      currentChar += charsToCopy;
    }
    if (node)
      node = node->Next();
  }
  buffer[i] = 0;
}

// Finds the next match, returning TRUE if successful and returning actual position
// in linePos and charPos. For next match, increment charPos and try again.
// Returns FALSE when no more matches.
Bool wxHyperTextWindow::StringSearch(char *searchString, int *linePos, int *charPos,
                                   Bool ignoreCase)
{
  static char searchBuffer[500];
  int line2, char2;
  wxNode *startNode = NULL;
  while (TRUE)
  {
    startNode = FindChunkAtLine(*linePos, startNode);

    int lineLength = LineLength(*linePos, startNode);
    if (lineLength < 0) return FALSE;
    if (*charPos >= lineLength)
    {
      (*linePos) = (*linePos + 1);
      (*charPos) = 0;
    }
    
    if (GetOffsetPosition(*linePos, *charPos, strlen(searchString), &line2, &char2, startNode))
    {
      GetSpanText(searchBuffer, 500, *linePos, *charPos, line2, char2, TRUE, startNode);
      Bool matchFlag = TRUE;
      int len = strlen(searchBuffer);
      if (len == 0)
        return FALSE;
      else if (ignoreCase)
      {
        for (int j = 0; j < len; j++)
          if (tolower(searchString[j]) != tolower(searchBuffer[j]))
	  {
            matchFlag = FALSE;
            break;
	  }
      }
      else
        matchFlag = (strncmp(searchString, searchBuffer, len) == 0);
      if (matchFlag)
        return TRUE;
    }
    else return FALSE;

    (*charPos) = (*charPos) + 1;
  }
  return FALSE;
}

/*
 * Insert text at line1, char1, returning end position
 *
 */
Bool wxHyperTextWindow::InsertText(int line1, int char1, int *line2, int *char2,
                                   char *textToInsert)
{
  wxNode *start_node = FindChunkAtLine(line1, NULL);
//  Bool appendToEnd = FALSE;
  int linesSoFar;
  int actualStartLine;

  // If can't find start node, assume appending to end.
  if (!start_node)
  {
//    appendToEnd = TRUE;
    wxTextChunk *tempChunk =
      new wxTextChunk(CHUNK_START_LINE, no_lines, "", NULL, NULL, -1, -1, -1, -1);
    no_lines ++;
    start_node = text_chunks.Append(tempChunk);
    linesSoFar = no_lines-1;
    actualStartLine = linesSoFar;
  }
  else
  {
    linesSoFar = line1;
    actualStartLine = linesSoFar;
  }

  // Find which existing chunk we have to carve up
  wxNode *node = start_node;
  int char_pos = 0;
  Bool success = FALSE;
  while (node)
  {
    wxNode *next_node = node->Next();
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    char *oldText = chunk->text;
    int len = strlen(oldText);
    if ((char1 < (char_pos + len)) || !next_node) // If no next node, add to end.
    {
      if (!next_node) // Go straight to end of this chunk
        char_pos += len;

      // Break up this chunk
      int split_point = wxMin(wxMax((char1 - char_pos), 0), len+1);
      strncpy(wxBuffer, oldText, split_point);
      wxBuffer[split_point] = 0;

      // Keep appending lines of new text (new text may have newlines)
      wxTextChunk *currentChunk = chunk;
      int ptr = split_point;
      int i = 0;
      int newCh = -1;
      while (newCh != 0)
      {
        newCh = textToInsert[i];
        if (newCh == '\n')
        {
          wxBuffer[ptr] = 0;
          i ++;
          // End old chunk
          currentChunk->text = copystring(wxBuffer);
          if (currentChunk != chunk) // If old chunk, is already in the list
          {
            if (!next_node)
              text_chunks.Append(currentChunk);
            else text_chunks.Insert(next_node, currentChunk);
	  }

          // Start new chunk
          currentChunk =
           new wxTextChunk(CHUNK_START_LINE, linesSoFar, NULL, NULL, NULL, -1, -1, -1, -1);
          linesSoFar ++;
          ptr = 0;
        }
        else if (newCh == 0)
        {
          wxBuffer[ptr] = 0;
          // Add last segment of text from first chunk found
          strncat(wxBuffer, oldText + split_point, len - split_point);
          wxBuffer[ptr + len-split_point] = 0;
          delete[] oldText;

          // End last chunk
          currentChunk->text = copystring(wxBuffer);
          if (currentChunk != chunk) // If old chunk, is already in the list
          {
            if (!next_node)
              text_chunks.Append(currentChunk);
            else text_chunks.Insert(next_node, currentChunk);
	  }
        }
        else
        {
          wxBuffer[ptr] = newCh;
          ptr ++;
          i ++;
        }
      }
      no_lines += (linesSoFar - actualStartLine);
      if (sections.Number() == 0)
        section_end_line = (no_lines - 1);

      *line2 = linesSoFar;
      *char2 = ptr;
      success = TRUE;
      node = NULL;
    }
    else
    {
      char_pos += len;
      node = node->Next();
    }
  }
  return success;
}

/*
 * DIFFICULT! NOT YET IMPLEMENTED.
 * Problems:
 * -  How to cope with deleting block start/end markers without
 *    deleting the matching marker (could search forward/backward)
 */
 
Bool wxHyperTextWindow::DeleteText(int line1, int char1, int line2, int char2)
{
  wxNode *startNode = FindChunkAtLine(line1);
  wxNode *endNode = FindChunkAtLine(line2);
  if (!startNode || !endNode)
    return FALSE;

  Bool started = FALSE;
  int currentChar = 0;
  int currentLine = line1;
  wxNode *node = startNode;
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    if ((currentLine >= line2) && (currentChar > char2))
      node = NULL;
    else if (!started)
    {
      if (chunk->line_no > -1)
        currentLine = chunk->line_no;

      int len = strlen(chunk->text);
      int charsToCopy = len;
      // See if we can start copying characters to the buffer
      if (char1 <= (len + currentChar - 1))
      {
        started = TRUE;

        // See if there's a limit to the number of chars to copy
/*
        if ((currentLine == line2) && (char2 < (len + currentChar - 1)))
          charsToCopy = char2 - currentChar + 1;
        
        for (int j = (char1 - currentChar); j < charsToCopy; j++)
	{
          buffer[i] = chunk->text[j];
          if (i < (bufSize - 1))
            i ++;
	}
*/
      }
      currentChar += charsToCopy;
    }
    else
    {
/*
      // Check if we've got a newline, in which case put a space in the buffer
      if ((chunk->chunk_type == CHUNK_START_LINE) && (chunk->line_no <= line2))
      {
        currentLine = chunk->line_no;
        currentChar = 0;
        if (i < (bufSize - 1))
        {
          if (convertNewLines)
            buffer[i] = ' ';
          else
            buffer[i] = 10;
          i ++;
        }
      }
      int len = strlen(chunk->text);

      // See if there's a limit to the number of chars to copy
      int charsToCopy = len;
      if ((currentLine == line2) && (char2 < (len + currentChar - 1)))
        charsToCopy = char2 - currentChar + 1;

      for (int j = 0; j < charsToCopy; j++)
      {
        buffer[i] = chunk->text[j];
        if (i < (bufSize - 1))
          i ++;
      }

      currentChar += charsToCopy;
*/
    }
    if (node)
      node = node->Next();
  }
  return FALSE;
}


/*
 * Translation functions
 */

Bool wxHyperTextWindow::OpenTransFile(int which, char *filename)
{
  if (which == 1)
  {
    if (transFile1)
    {
      fclose(transFile1);
      transFile1 = NULL;
    }
    transFile1 = fopen(filename, "w");
    if (transFile1)
      return TRUE;
    else
      return FALSE;
  }
  else if (which == 2)
  {
    if (transFile2)
    {
      fclose(transFile2);
      transFile2 = NULL;
    }
    transFile2 = fopen(filename, "w");
    if (transFile2)
      return TRUE;
    else
      return FALSE;
  }
  return FALSE;
}

void wxHyperTextWindow::CloseTransFile(int which)
{
  if (which == 1 && transFile1)
    fclose(transFile1);
  else if (which == 2 && transFile2)
    fclose(transFile2);
}

void wxHyperTextWindow::OutputTrans(int which, char *buf)
{
  if (((which == 1) || (which == -1)) && transFile1)
    fprintf(transFile1, buf);
  if (((which == 2) || (which == -1)) && transFile2)
    fprintf(transFile2, buf);
}

// Starts the translation process
void wxHyperTextWindow::Translate(void)
{
  OnTranslate(HYTEXT_EVENT_START_FILE, 0, 0);

  Bool newLineEncountered = FALSE;
  wxNode *node = text_chunks.First();
  while (node)
  {
    wxTextChunk *chunk = (wxTextChunk *)node->Data();
    
    switch (chunk->chunk_type)
    {
      case CHUNK_START_LINE:
      {
        if (newLineEncountered)
        {
          OnTranslate(HYTEXT_EVENT_DOUBLE_NEWLINE, chunk->block_type, chunk->block_id);
          newLineEncountered = FALSE;
        }
        else
          newLineEncountered = TRUE;
        
        if (node != text_chunks.First())
          OutputTrans(-1, "\n");
        if (chunk->text && (strlen(chunk->text) > 0))
        {
          newLineEncountered = FALSE;
          OutputTrans(-1, chunk->text);
        }
        break;
      }
      case CHUNK_START_BLOCK:
      case CHUNK_START_UNRECOGNIZED_BLOCK:
      {
        OnTranslate(HYTEXT_EVENT_START_BLOCK, chunk->block_type, chunk->block_id);
        if (chunk->text && (strlen(chunk->text) > 0))
        {
          newLineEncountered = FALSE;
          OutputTrans(-1, chunk->text);
        }
        break;
      }
      case CHUNK_END_BLOCK:
      {
        if (chunk->text && (strlen(chunk->text) > 0))
        {
          newLineEncountered = FALSE;
          OutputTrans(-1, chunk->text);
        }
        OnTranslate(HYTEXT_EVENT_END_BLOCK, chunk->block_type, chunk->end_id);
        break;
      }
      default:
        break;
    }
    node = node->Next();
  }
  
  OnTranslate(HYTEXT_EVENT_END_FILE, 0, 0);
}

