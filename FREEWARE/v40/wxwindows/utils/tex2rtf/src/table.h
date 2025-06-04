/*
 * File:     table.h
 * Purpose:  Table utilities
 */

/*
 * Table dimensions
 *
 */

struct ColumnData
{
  char justification; // l, r, c
  int width;          // -1 or a width in twips
  int spacing;        // Space between columns in twips
  Bool leftBorder;
  Bool rightBorder;
  Bool absWidth;      // If FALSE (the default), don't use an absolute width if you can help it.
};

extern ColumnData TableData[];
extern Bool inTabular;
extern Bool startRows;
extern Bool tableVerticalLineLeft;
extern Bool tableVerticalLineRight;
extern int noColumns;   // Current number of columns in table
extern int ruleTop;
extern int ruleBottom;
extern int currentRowNumber;
extern Bool ParseTableArgument(char *value);
