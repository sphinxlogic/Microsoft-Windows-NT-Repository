' -- VGA background color demo program --
 DEFINT A-Z: SCREEN 12: CLS
 PRINT "Press RrGgBb to set colors   ESC to quit"
 DO:
  X$ = INPUT$(1)
  IF X$ = CHR$(27) THEN CLS : END
  IF X$ = "R" THEN RED = RED + 1
  IF X$ = "r" THEN RED = RED - 1
  IF X$ = "G" THEN GREEN = GREEN + 1
  IF X$ = "g" THEN GREEN = GREEN - 1
  IF X$ = "B" THEN BLUE = BLUE + 1
  IF X$ = "b" THEN BLUE = BLUE - 1
  IF RED > 63 THEN RED = 0
  IF RED < 0 THEN RED = 63
  IF GREEN > 63 THEN GREEN = 0
  IF GREEN < 0 THEN GREEN = 63
  IF BLUE > 63 THEN BLUE = 0
  IF BLUE < 0 THEN BLUE = 63
  BACKCOLOR& = 65536 * BLUE + 256 * GREEN + RED
  PALETTE 0, BACKCOLOR&
  LOCATE 30, 1
  PRINT "RED:"; RED; "  BLUE:"; BLUE; "  GREEN:"; GREEN;
  PRINT "  COLOR NUMBER:"; BACKCOLOR&;
 LOOP

