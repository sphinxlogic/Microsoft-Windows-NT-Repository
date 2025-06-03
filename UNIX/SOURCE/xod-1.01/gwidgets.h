#include <stdio.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <Dialog.h>

#ifdef XOD_MAIN
#	define Dextern
#else
#	define Dextern extern
#endif

Dextern Widget HelpDialog, ModeSelect, GrepText;
Dextern Widget ByteAddress, ctwWidget, ByteOffsetWidget;
Dextern Widget StatusWidget, ErrorDialog, DecodeWidget;
Dextern Dialog GotoByteDialog;
