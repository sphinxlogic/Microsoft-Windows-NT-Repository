#include <buttonndx.h>

typedef struct {
  int             id;
  String          name;
  Boolean         trap;
  Widget          widget;
} ButtonRec, *ButtonPtr;

/* for the File menu */
static ButtonRec file_menu[] = {
  {New, "new", True},
  {Quit, "quit", True},
};

static ButtonRec buttonsMode[] = {
  {Hex, "Hex", False},
  {Octal, "Octal", False},
  {Decimal, "Decimal", False},
  {Ascii, "ASCII", False},
};

static ButtonRec buttonsDecode[] = {
  {Short, "Short", False},
  {uShort, "uShort", False},
  {Long, "Long", False},
  {uLong, "uLong", False},
  {Float, "Float", False},
  {Double, "Double", False},
  {HPR4, "HpR4", False},
  {HPR6, "HpR6", False},
};
