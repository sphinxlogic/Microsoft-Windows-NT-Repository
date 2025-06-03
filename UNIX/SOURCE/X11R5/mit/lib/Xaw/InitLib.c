/* libXaw: InitLib.c
 * -----------------
 * This code goes into .init section of the client binary.
 * On archive libraries references like &widgetClassRec, &compositeClassRec
 * and so on are unresolved. They will resolved during linking. So someone
 * could define its own (e.g.) widgetClassRec and even the library code
 * will use it gracefully (one example for this are the vendorShellClassRec's
 * in the Toolkit, in the Athena Widgets and in the Motif widgets).
 * On SVR3 shared lib such references are on fixed addresses. The library
 * would never use a user defined (e.g.) vendorShellClassRec and the compiler
 * would complain about multiply defined vendorShellClassRec.
 * So we don't reference to (e.g.) vendorShellClassRec in the shared lib.
 * Instead putting this code to client binary, which is resolved during
 * linking and writes on runtime the needed addresses to the shared lib.
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xaw/InitLib.c,v 1.3 1993/05/22 06:43:32 dawes Exp $
 */

#include <X11/IntrinsicP.h>
#include <X11/Xmu/WidgetNode.h>
#include <X11/Xaw/XawInit.h>
#include <X11/Xaw/AsciiSinkP.h>
#include <X11/Xaw/AsciiSrcP.h>
#include <X11/Xaw/AsciiTextP.h>
#include <X11/Xaw/BoxP.h>
#include <X11/Xaw/ClockP.h>
#include <X11/Xaw/CommandP.h>
#include <X11/Xaw/DialogP.h>
#include <X11/Xaw/FormP.h>
#include <X11/Xaw/GripP.h>
#include <X11/Xaw/LabelP.h>
#include <X11/Xaw/ListP.h>
#include <X11/Xaw/LogoP.h>
#include <X11/Xaw/MailboxP.h>
#include <X11/Xaw/MenuButtoP.h>
#include <X11/Xaw/PanedP.h>
#include <X11/Xaw/PannerP.h>
#include <X11/Xaw/PortholeP.h>
#include <X11/Xaw/RepeaterP.h>
#include <X11/Xaw/ScrollbarP.h>
#include <X11/Xaw/SimpleP.h>
#include <X11/Xaw/SimpleMenP.h>
#include <X11/Xaw/SmeP.h>
#include <X11/Xaw/SmeBSBP.h>
#include <X11/Xaw/SmeLineP.h>
#include <X11/Xaw/StripCharP.h>
#include <X11/Xaw/TextP.h>
#include <X11/Xaw/TextSinkP.h>
#include <X11/Xaw/TextSrcP.h>
#include <X11/Xaw/ToggleP.h>
#include <X11/Xaw/TreeP.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Vendor.h>
#include <X11/VendorP.h>
#include <X11/Xaw/ViewportP.h>

#ifdef SVR3SHLIB

/* Check whether the named class is really from shared libXaw.
 * It could also be user supplied. We don't know with what the user
 * wants to initialize, so let it be in that case.
 * 0xB1C00000 : address of libXaw's .data section
 */
#define IsInLibXaw(ClassRec) ((unsigned)ClassRec & 0xFFC00000) == 0xB1C00000

extern WidgetClass __futureWidgetClass_0_;
extern WidgetClass __futureWidgetClass_1_;
extern WidgetClass __futureWidgetClass_2_;
extern WidgetClass __futureWidgetClass_3_;
extern WidgetClass __futureWidgetClass_4_;
extern WidgetClass __futureWidgetClass_5_;
extern WidgetClass __futureWidgetClass_6_;
extern WidgetClass __futureWidgetClass_7_;
extern WidgetClass __futureWidgetClass_8_;
extern WidgetClass __futureWidgetClass_9_;
extern WidgetClass __futureWidgetClass_10_;
extern WidgetClass __futureWidgetClass_11_;
extern WidgetClass __futureWidgetClass_12_;
extern WidgetClass __futureWidgetClass_13_;
extern WidgetClass __futureWidgetClass_14_;
extern WidgetClass __futureWidgetClass_15_;
extern WidgetClass __futureWidgetClass_16_;
extern WidgetClass __futureWidgetClass_17_;
extern WidgetClass __futureWidgetClass_18_;
extern WidgetClass __futureWidgetClass_19_;

static WidgetClass *__AllWidgets[] = {
&applicationShellWidgetClass,
&asciiSinkObjectClass,
&asciiSrcObjectClass,
&asciiTextWidgetClass,
&boxWidgetClass,
&clockWidgetClass,
&commandWidgetClass,
&compositeWidgetClass,
&constraintWidgetClass,
&coreWidgetClass,
&dialogWidgetClass,
&formWidgetClass,
&gripWidgetClass,
&labelWidgetClass,
&listWidgetClass,
&logoWidgetClass,
&mailboxWidgetClass,
&menuButtonWidgetClass,
&objectClass,
&overrideShellWidgetClass,
&panedWidgetClass,
&pannerWidgetClass,
&portholeWidgetClass,
&rectObjClass,
&repeaterWidgetClass,
&scrollbarWidgetClass,
&shellWidgetClass,
&simpleMenuWidgetClass,
&simpleWidgetClass,
&smeBSBObjectClass,
&smeLineObjectClass,
&smeObjectClass,
&stripChartWidgetClass,
&textSinkObjectClass,
&textSrcObjectClass,
&textWidgetClass,
&toggleWidgetClass,
&topLevelShellWidgetClass,
&transientShellWidgetClass,
&treeWidgetClass,
&vendorShellWidgetClass,
&viewportWidgetClass,
&wmShellWidgetClass,
/*
 * additional widgetClass's will be on the same address like this
 * dummy pointers, so the client will initialize them in XawWidgetArray
 * too
 */
&__futureWidgetClass_0_,
&__futureWidgetClass_1_,
&__futureWidgetClass_2_,
&__futureWidgetClass_3_,
&__futureWidgetClass_4_,
&__futureWidgetClass_5_,
&__futureWidgetClass_6_,
&__futureWidgetClass_7_,
&__futureWidgetClass_8_,
&__futureWidgetClass_9_,
&__futureWidgetClass_10_,
&__futureWidgetClass_11_,
&__futureWidgetClass_12_,
&__futureWidgetClass_13_,
&__futureWidgetClass_14_,
&__futureWidgetClass_15_,
&__futureWidgetClass_16_,
&__futureWidgetClass_17_,
&__futureWidgetClass_18_,
&__futureWidgetClass_19_
};

extern XmuWidgetNode XawWidgetArray[];
extern int XawWidgetCount;
extern XtActionsRec _XawTextActionsTable[];

void __libXaw_init_()
{
  int i;

  /* initialize XawWidgetArray at first */

  for(i = 0 ; i < (XawWidgetCount+FUTURE_WIDGET_COUNT); i++)
     XawWidgetArray[i].widget_class_ptr = __AllWidgets[i];

  /* AsciiSink.c */
  if(IsInLibXaw(asciiSinkObjectClass))
     asciiSinkObjectClass->core_class.superclass = textSinkObjectClass;

  /* AsciiSrc.c */
  if(IsInLibXaw(asciiSrcObjectClass))
     asciiSrcObjectClass->core_class.superclass = textSrcObjectClass;

  /* AsciiText.c */
  if(IsInLibXaw(asciiTextWidgetClass))
     asciiTextWidgetClass->core_class.superclass = textWidgetClass;

  /* Box.c */
  if(IsInLibXaw(boxWidgetClass))
     boxWidgetClass->core_class.superclass = compositeWidgetClass;

  /* Clock.c */
  if(IsInLibXaw(clockWidgetClass))
     clockWidgetClass->core_class.superclass = simpleWidgetClass;

  /* Command.c */ 
  if(IsInLibXaw(commandWidgetClass))
     commandWidgetClass->core_class.superclass = labelWidgetClass;

  /* Dialog.c */
  if(IsInLibXaw(dialogWidgetClass))
     dialogWidgetClass->core_class.superclass = formWidgetClass;
  
  /* Form.c */
  if(IsInLibXaw(formWidgetClass))
     formWidgetClass->core_class.superclass = constraintWidgetClass;

  /* Grip.c */
  if(IsInLibXaw(gripWidgetClass))
     gripWidgetClass->core_class.superclass = simpleWidgetClass;

  /* Label.c */
  if(IsInLibXaw(labelWidgetClass))
     labelWidgetClass->core_class.superclass = simpleWidgetClass;

  /* List.c */
  if(IsInLibXaw(listWidgetClass))
     listWidgetClass->core_class.superclass = simpleWidgetClass;

  /* Logo.c */
  if(IsInLibXaw(logoWidgetClass))
     logoWidgetClass->core_class.superclass = simpleWidgetClass;

  /* Mailbox.c */
  if(IsInLibXaw(mailboxWidgetClass))
     mailboxWidgetClass->core_class.superclass = simpleWidgetClass;

  /* MenuBotton.c */
  if(IsInLibXaw(menuButtonWidgetClass))
     menuButtonWidgetClass->core_class.superclass = commandWidgetClass;

  /* Paned.c */
  if(IsInLibXaw(panedWidgetClass))
     panedWidgetClass->core_class.superclass = constraintWidgetClass;

  /* Panner.c */
  if(IsInLibXaw(pannerWidgetClass))
     pannerWidgetClass->core_class.superclass = simpleWidgetClass;

  /* Porthole.c */
  if(IsInLibXaw(portholeWidgetClass))
     portholeWidgetClass->core_class.superclass = compositeWidgetClass;

  /* Repeater.c */
  if(IsInLibXaw(repeaterWidgetClass))
     repeaterWidgetClass->core_class.superclass = commandWidgetClass;

  /* Scrollbar.c */
  if(IsInLibXaw(scrollbarWidgetClass))
     scrollbarWidgetClass->core_class.superclass = simpleWidgetClass;

  /* Simple.o */
  if(IsInLibXaw(simpleWidgetClass))
     simpleWidgetClass->core_class.superclass = widgetClass;

  /* SimpleMenu.c */
  if(IsInLibXaw(simpleMenuWidgetClass))
     simpleMenuWidgetClass->core_class.superclass = overrideShellWidgetClass;

  /* Sem.c */
  if(IsInLibXaw(smeObjectClass))
     smeObjectClass->core_class.superclass = rectObjClass;

  /* SmeBSB.c */
  if(IsInLibXaw(smeBSBObjectClass))
     smeBSBObjectClass->core_class.superclass = smeObjectClass;

  /* SmeLine.c */
  if(IsInLibXaw(smeLineObjectClass))
     smeLineObjectClass->core_class.superclass = smeObjectClass;

  /* StripChart.c */
  if(IsInLibXaw(stripChartWidgetClass))
     stripChartWidgetClass->core_class.superclass = simpleWidgetClass;

  /* Text.c */
  if(IsInLibXaw(textWidgetClass)) {
     textWidgetClass->core_class.superclass = simpleWidgetClass;
     textWidgetClass->core_class.actions = _XawTextActionsTable; 
  }
  
  /* TextSink.c */
  if(IsInLibXaw(textSinkObjectClass))
     textSinkObjectClass->core_class.superclass = objectClass;

  /* TextSrc.c */
  if(IsInLibXaw(textSrcObjectClass))
     textSrcObjectClass->core_class.superclass = objectClass;

  /* Toggle.c */
  if(IsInLibXaw(toggleWidgetClass))
     toggleWidgetClass->core_class.superclass = commandWidgetClass;

  /* Tree.c */
  if(IsInLibXaw(treeWidgetClass))
     treeWidgetClass->core_class.superclass = constraintWidgetClass;

  /* Vendor.c */
  if(IsInLibXaw(vendorShellWidgetClass))
     vendorShellWidgetClass->core_class.superclass = wmShellWidgetClass;

  /* Viewport.c */
  if(IsInLibXaw(viewportWidgetClass))
     viewportWidgetClass->core_class.superclass = formWidgetClass;


}
#endif /* SVR3SHLIB */


