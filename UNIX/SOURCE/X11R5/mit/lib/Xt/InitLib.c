/* libXt : InitLib.c
 * -----------------
 * This code goes into .init section of the client binary.
 * On archive library references like &widgetClassRec, &compositeClassRec
 * and so on are unresolved. They will resolved during linking. So someone
 * could define its own (e.g.) widgetClassRec and even the library code
 * will use it gracefully.
 * On SVR3 shared lib such references are to fixed addresses. The library
 * would never use a user defined (e.g.) widgetClassRec and more worse
 * the compiler would complain about multiply defined widgetClassRec.
 * So we don't reference to (e.g.) widgetClassRec in the shared lib.
 * Instead putting this code to client binary, which is resolved during
 * linking and writes on runtime the needed addresses to the shared lib.
 *
 * Second, it initializes all References to _XtInherit in the Library.
 * This is necessary in libXt only, because a behavior of the mkshlib(1) tool.
 * It seems to forget to replace function addresses in structures
 * with the new function address from the branch table. But there are
 * several places in the code were pointers are compared with _XtInherit.
 *
 * Copyright (c) 1992, 1993 by Thomas Wolfram, Berlin, Germany
 * (thomas@aeon.in-berlin.de, wolf@prz.tu-berlin.de)
 *
 * $XFree86: mit/lib/Xt/InitLib.c,v 1.3 1993/05/22 06:51:00 dawes Exp $
 */

#include "IntrinsicI.h"
#include "VarargsI.h"
#include "ShellP.h"
#include "VendorP.h"

#ifdef SVR3SHLIB

/* Check whether the named class is really from shared libXt.
 * It could also be user supplied (e.g. vendorShellClassRec from Motif).
 * We don't know with what the user wants to initialize, so let it be in
 * that case.
 * 0xB0C00000 : address of libXt's .data section
 */
#define IsInLibXt(ClassRec) ((unsigned)ClassRec & 0xFFC00000) == 0xB0C00000 

externalref WidgetClass _libXtCore_unNamedObjClass;

void __libXt_init_()
{

  /* Composite.c */
  if(IsInLibXt(compositeWidgetClass)) {
      compositeWidgetClass->core_class.superclass = widgetClass;
      compositeWidgetClass->core_class.realize = XtInheritRealize;
      compositeWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
  }

  /* Constraint.c */
  if(IsInLibXt(constraintWidgetClass)) {
      constraintWidgetClass->core_class.superclass = compositeWidgetClass;
      constraintWidgetClass->core_class.realize = XtInheritRealize;
      constraintWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((ConstraintWidgetClass)constraintWidgetClass)->composite_class.insert_child = XtInheritInsertChild;
      ((ConstraintWidgetClass)constraintWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

  /* Core.c */
  if(IsInLibXt(_libXtCore_unNamedObjClass)) {
      _libXtCore_unNamedObjClass->core_class.superclass = rectObjClass;
      _libXtCore_unNamedObjClass->core_class.realize = XtInheritRealize;
      _libXtCore_unNamedObjClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
  }

  /* RectObj.c */
  if(IsInLibXt(rectObjClass))
      rectObjClass->core_class.superclass = objectClass;
  
  /* Shell.c */
  if(IsInLibXt(shellWidgetClass)) {
      shellWidgetClass->core_class.superclass = compositeWidgetClass;
      shellWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((ShellWidgetClass)shellWidgetClass)->composite_class.insert_child = XtInheritInsertChild;
      ((ShellWidgetClass)shellWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

  if(IsInLibXt(overrideShellWidgetClass)) {
      overrideShellWidgetClass->core_class.realize = XtInheritRealize;
      overrideShellWidgetClass->core_class.resize = XtInheritResize;
      overrideShellWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((OverrideShellWidgetClass)overrideShellWidgetClass)->composite_class.geometry_manager = XtInheritGeometryManager;
      ((OverrideShellWidgetClass)overrideShellWidgetClass)->composite_class.change_managed = XtInheritChangeManaged;
      ((OverrideShellWidgetClass)overrideShellWidgetClass)->composite_class.insert_child = XtInheritInsertChild;
      ((OverrideShellWidgetClass)overrideShellWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

  if(IsInLibXt(wmShellWidgetClass)) {
      wmShellWidgetClass->core_class.realize = XtInheritRealize;
      wmShellWidgetClass->core_class.resize = XtInheritResize;
      wmShellWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((WMShellWidgetClass)wmShellWidgetClass)->composite_class.geometry_manager = XtInheritGeometryManager;
      ((WMShellWidgetClass)wmShellWidgetClass)->composite_class.change_managed = XtInheritChangeManaged;
      ((WMShellWidgetClass)wmShellWidgetClass)->composite_class.insert_child = XtInheritInsertChild;
      ((WMShellWidgetClass)wmShellWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

  if(IsInLibXt(transientShellWidgetClass)) {
      transientShellWidgetClass->core_class.superclass = vendorShellWidgetClass;
      transientShellWidgetClass->core_class.resize = XtInheritResize;
      transientShellWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((TransientShellWidgetClass)transientShellWidgetClass)->composite_class.geometry_manager = XtInheritGeometryManager;
      ((TransientShellWidgetClass)transientShellWidgetClass)->composite_class.change_managed = XtInheritChangeManaged;
      ((TransientShellWidgetClass)transientShellWidgetClass)->composite_class.insert_child = XtInheritInsertChild;
      ((TransientShellWidgetClass)transientShellWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

  if(IsInLibXt(topLevelShellWidgetClass)) {
      topLevelShellWidgetClass->core_class.superclass = vendorShellWidgetClass;
      topLevelShellWidgetClass->core_class.realize = XtInheritRealize;
      topLevelShellWidgetClass->core_class.resize = XtInheritResize;
      topLevelShellWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((TopLevelShellWidgetClass)topLevelShellWidgetClass)->composite_class.geometry_manager = XtInheritGeometryManager;
      ((TopLevelShellWidgetClass)topLevelShellWidgetClass)->composite_class.change_managed = XtInheritChangeManaged;
      ((TopLevelShellWidgetClass)topLevelShellWidgetClass)->composite_class.insert_child = XtInheritInsertChild;
      ((TopLevelShellWidgetClass)topLevelShellWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

  if(IsInLibXt(applicationShellWidgetClass)) {
      applicationShellWidgetClass->core_class.realize = XtInheritRealize;
      applicationShellWidgetClass->core_class.resize = XtInheritResize;
      applicationShellWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((ApplicationShellWidgetClass)applicationShellWidgetClass)->composite_class.geometry_manager = XtInheritGeometryManager;
      ((ApplicationShellWidgetClass)applicationShellWidgetClass)->composite_class.change_managed = XtInheritChangeManaged;
      ((ApplicationShellWidgetClass)applicationShellWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

  /* Vendor.c */
  if(IsInLibXt(vendorShellWidgetClass)) {
      vendorShellWidgetClass->core_class.superclass = wmShellWidgetClass;
      vendorShellWidgetClass->core_class.realize = XtInheritRealize;
      vendorShellWidgetClass->core_class.resize = XtInheritResize;
      vendorShellWidgetClass->core_class.set_values_almost = XtInheritSetValuesAlmost;
      ((VendorShellWidgetClass)vendorShellWidgetClass)->composite_class.geometry_manager = XtInheritGeometryManager;
      ((VendorShellWidgetClass)vendorShellWidgetClass)->composite_class.change_managed = XtInheritChangeManaged;
      ((VendorShellWidgetClass)vendorShellWidgetClass)->composite_class.insert_child = XtInheritInsertChild;
      ((VendorShellWidgetClass)vendorShellWidgetClass)->composite_class.delete_child = XtInheritDeleteChild;
  }

}
#endif /* SVR3SHLIB */


