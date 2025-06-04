/* xmorph.c : Digital Image Warping for X Window System
**
** A graphical user interface to a mesh warping algorithm
**
** Written and Copyright (C) 1994, 1995, 1996 by Michael J. Gourlay
**
** NO WARRANTEES, EXPRESS OR IMPLIED.
*/

#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifndef VMS
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Scrollbar.h>
#else
#include <X11Xaw/Form.h>
#include <X11Xaw/Box.h>
#include <X11Xaw/Paned.h>
#include <X11Xaw/Scrollbar.h>
#endif

#include "diw_map.h"
#include "image.h"
#include "sequence.h"
#include "file_menu.h"
#include "mesh_menu.h"
#include "warp_menu.h"
#include "help_menu.h"




int verbose = FALSE;




/* --------------------------------------------------------------- */

/* actions: action-to-function mappings
** determines the behaviour of this application
*/
static XtActionsRec actions[] = {
  {"refresh", RefreshImage},
  {"draw_meshes", DrawMeshes},
  {"draw_all_meshes", DrawAllMeshes},
  {"redither", ReditherImage},
  {"redither_all", ReditherAllImages},
  {"warp", WarpImage},
  {"change_mesh", ChangeMeshLine},
  {"pick", PickMeshpoint},
  {"unpick", UnpickMeshpoint},
  {"drag", DragMeshpoint},
  {"start_drag", StartDragMeshpoint},
  {"warp_sequence", WarpSequence},
};

/* --------------------------------------------------------------- */

/* fallback_resources: default application resources
** determines the look of this application
*/
String fallback_resources[] = {
  /* Application looks */
  "XDiw*font:                  -*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*",
  "XDiw*foreground:                    MidnightBlue",
  "XDiw*background:                    LightGray",
  "XDiw*borderColor:                   DimGray",

  /* Dialog look and feel */
  "XDiw*Dialog.label.font:       -*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*",
  "XDiw*Dialog.value:                    ",
  "XDiw*Dialog.Command.foreground:       black",
  "XDiw*Dialog.cancel.label:             Cancel",
  "XDiw*Dialog.okay.label:               Okay",
  "XDiw*Dialog.Text.translations:        #override <Key>Return: mjg_okay()",

  /* Menu looks */
  "XDiw*MenuButton.font:       -*-helvetica-bold-o-normal-*-14-*-*-*-*-*-*",
  "XDiw*SimpleMenu*font:       -*-helvetica-bold-o-normal-*-14-*-*-*-*-*-*",
  "XDiw*SimpleMenu*leftMargin:         24",
  "XDiw*SimpleMenu*rightMargin:        24",

  /* File titlebar menu */
  "XDiw*file_menu_button.label:        File",
  "XDiw*file_menu.fm_osi_sme.label:    Open source image...",
  "XDiw*file_menu.fm_odi_sme.label:    Open destination image...",
  "XDiw*file_menu.fm_osm_sme.label:    Open source mesh...",
  "XDiw*file_menu.fm_odm_sme.label:    Open destination mesh...",
  "XDiw*file_menu.fm_ssi_sme.label:    Save source image...",
  "XDiw*file_menu.fm_sdi_sme.label:    Save destination image...",
  "XDiw*file_menu.fm_ssm_sme.label:    Save source mesh...",
  "XDiw*file_menu.fm_sdm_sme.label:    Save destination mesh...",
  "XDiw*file_menu.fm_quit_sme.label:   Quit",
  "XDiw*TransientShell.md_osi.label:     Load source image from filename:",
  "XDiw*TransientShell.md_odi.label:     Load destination image from filename:",
  "XDiw*TransientShell.md_osm.label:     Load source mesh from filename:",
  "XDiw*TransientShell.md_odm.label:     Load destination mesh from filename:",
  "XDiw*TransientShell.md_ssm.label:     Save source mesh to filename:",
  "XDiw*TransientShell.md_sdm.label:     Save destination mesh to filename:",
  "XDiw*TransientShell.md_si.label:      Save image to filename:",

  /* Mesh titlebar menu */
  "XDiw*mesh_menu_button.label:        Mesh",
  "XDiw*mesh_menu.mm_rsm_sme.label:    Reset source mesh",
  "XDiw*mesh_menu.mm_rdm_sme.label:    Reset destination mesh",
  "XDiw*mesh_menu.mm_msm_sme.label:    Functionalize source mesh",
  "XDiw*mesh_menu.mm_mdm_sme.label:    Functionalize destination mesh",

  /* Warp titlebar menu */
  "XDiw*warp_menu_button.label:        Morph Sequence",
  "XDiw*warp_menu.wm_ssfn_sme.label:   Set sequence file name...",
  "XDiw*warp_menu.wm_ssns_sme.label:   Set sequence number of steps...",
  "XDiw*warp_menu.wm_seq_sme.label:    Warp sequence",
  "XDiw*TransientShell.md_ssfn.label:    Warp Sequence filename:",
  "XDiw*TransientShell.md_ssns.label:    Number of warp frames:",
  "XDiw*TransientShell.md_ssfn.value:    warp",
  "XDiw*TransientShell.md_ssns.value:    30",

  /* Help titlebar menu */
  "XDiw*help_menu_button.label:          Help",
  "XDiw*help_menu_button.horizDistance:  100",
  "XDiw*help_menu.hm_about_sme.label:    About...",
  "XDiw*help_menu.hm_mmp_sme.label:      Manipulating the mesh...",
  "XDiw*help_menu.hm_file_sme.label:     File menu help...",
  "XDiw*help_menu.hm_mesh_sme.label:     Mesh menu help...",
  "XDiw*help_menu.hm_warp_sme.label:     Warp menu help...",
  "XDiw*help_menu.hm_dpm_sme.label:      DIW Properties menu help...",
  "XDiw*help_menu.hm_dcm_sme.label:      DIW Commands menu help...",
  "XDiw*help_menu.hm_quit_sme.label:     Quit Xdiw",

  "XDiw*TransientShell.md_h_about.Text*font: -*-helvetica-medium-r-normal-*-18-*-*-*-*-*-*",
  "XDiw*TransientShell.md_h_about.label:                About Xdiw",
  "XDiw*TransientShell.md_h_about.Text.width:           640",
  "XDiw*TransientShell.md_h_about.Text.height:          480",
  "XDiw*TransientShell.md_h_about.Text.scrollHoriontal: whenneeded",
  "XDiw*TransientShell.md_h_about.okay.label:           Dismiss",

  "XDiw*TransientShell.md_h_mmp.Text*font: -*-helvetica-medium-r-normal-*-18-*-*-*-*-*-*",
  "XDiw*TransientShell.md_h_mmp.label:                Manipulating the mesh",
  "XDiw*TransientShell.md_h_mmp.Text.width:           640",
  "XDiw*TransientShell.md_h_mmp.Text.height:          480",
  "XDiw*TransientShell.md_h_mmp.Text.scrollHoriontal: whenneeded",
  "XDiw*TransientShell.md_h_mmp.okay.label:           Dismiss",

  "XDiw*TransientShell.md_h_file.Text*font: -*-helvetica-medium-r-normal-*-18-*-*-*-*-*-*",
  "XDiw*TransientShell.md_h_file.label:                File menu Help",
  "XDiw*TransientShell.md_h_file.Text.width:           640",
  "XDiw*TransientShell.md_h_file.Text.height:          480",
  "XDiw*TransientShell.md_h_file.Text.scrollHoriontal: whenneeded",
  "XDiw*TransientShell.md_h_file.okay.label:           Dismiss",

  "XDiw*TransientShell.md_h_mesh.Text*font: -*-helvetica-medium-r-normal-*-18-*-*-*-*-*-*",
  "XDiw*TransientShell.md_h_mesh.label:                Mesh menu Help",
  "XDiw*TransientShell.md_h_mesh.Text.width:           640",
  "XDiw*TransientShell.md_h_mesh.Text.height:          480",
  "XDiw*TransientShell.md_h_mesh.Text.scrollHoriontal: whenneeded",
  "XDiw*TransientShell.md_h_mesh.okay.label:           Dismiss",

  "XDiw*TransientShell.md_h_morph.Text*font: -*-helvetica-medium-r-normal-*-18-*-*-*-*-*-*",
  "XDiw*TransientShell.md_h_morph.label:              Morph Sequence menu Help",
  "XDiw*TransientShell.md_h_morph.Text.width:         640",
  "XDiw*TransientShell.md_h_morph.Text.height:        480",
  "XDiw*TransientShell.md_h_morph.Text.scrollHoriontal: whenneeded",
  "XDiw*TransientShell.md_h_morph.okay.label:         Dismiss",

  "XDiw*TransientShell.md_h_dpm.Text*font: -*-helvetica-medium-r-normal-*-18-*-*-*-*-*-*",
  "XDiw*TransientShell.md_h_dpm.label:                DIW Properties menu Help",
  "XDiw*TransientShell.md_h_dpm.Text.width:           640",
  "XDiw*TransientShell.md_h_dpm.Text.height:          480",
  "XDiw*TransientShell.md_h_dpm.Text.scrollHoriontal: whenneeded",
  "XDiw*TransientShell.md_h_dpm.okay.label:           Dismiss",

  "XDiw*TransientShell.md_h_dcm.Text*font: -*-helvetica-medium-r-normal-*-18-*-*-*-*-*-*",
  "XDiw*TransientShell.md_h_dcm.label:                DIW Commands menu Help",
  "XDiw*TransientShell.md_h_dcm.Text.width:           640",
  "XDiw*TransientShell.md_h_dcm.Text.height:          480",
  "XDiw*TransientShell.md_h_dcm.Text.scrollHoriontal: whenneeded",
  "XDiw*TransientShell.md_h_dcm.okay.label:           Dismiss",

  /* ----------------- */
  /* DIW box resources */
  "XDiw*diw_box.SimpleMenu.background:         white",

  /* Viewport scrollbars */
  "XDiw*Viewport.allowHoriz:           True",
  "XDiw*Viewport.allowVert:            True",

  /* warp and dissolve scrollbars */
  "XDiw*mesh_label.label:              warp:",
  "XDiw*source_mesh.foreground:        dark green",
  "XDiw*source_mesh.label:             source mesh",
  "XDiw*destination_mesh.foreground:   red",
  "XDiw*destination_mesh.label:        destination mesh",
  "XDiw*scrollbar_mesh.foreground:     yellow",
  "XDiw*scrollbar_mesh.orientation:    horizontal",
  "XDiw*scrollbar_image.orientation:   horizontal",
  "XDiw*Scrollbar.foreground:          LightGray",
  "XDiw*Scrollbar.background:          DimGray",
  "XDiw*image_label.label:             dissolve:",
  "XDiw*source_image.label:            source image",
  "XDiw*destination_image.label:       destination image",

  /* DIW Properties menu */
  "XDiw*diw_prop_menu_button.label:            Properties",
  "XDiw*diw_box.SimpleMenu.smp_sme.label:      Show source mesh points",
  "XDiw*diw_box.SimpleMenu.sml_sme.label:      Show source mesh lines",
  "XDiw*diw_box.SimpleMenu.dmp_sme.label:      Show destination mesh points",
  "XDiw*diw_box.SimpleMenu.dml_sme.label:      Show destination mesh lines",
  "XDiw*diw_box.SimpleMenu.tmp_sme.label:      Show tween mesh points",
  "XDiw*diw_box.SimpleMenu.tml_sme.label:      Show tween mesh lines",
  "XDiw*diw_box.SimpleMenu.dim_sme.label:      Dim Image",

  /* DIW Commands menu */
  "XDiw*diw_cmd_menu_button.label:             Commands",
  "XDiw*diw_box.SimpleMenu.si_sme.label:       Save Image...",
  "XDiw*diw_box.SimpleMenu.warp_sme.label:     Warp Image",
  "XDiw*diw_box.SimpleMenu.redither_sme.label: Redither Image",

  NULL
};

/* --------------------------------------------------------------- */

/* initialize_application: lots of once-only initialization
** Build the widget heirarchy and initializes all GUI variables
*/
XtAppContext
initialize_application(int width, int height, int *argc, char **argv)
{
  XtAppContext app;
  Widget         toplevel;
  Widget           form;
  Widget             file_menu_button;
  Widget             mesh_menu_button;
  Widget             warp_menu_button;
  Widget             help_menu_button;
  Widget             diw_box;
  Widget               diw_widget1, diw_widget2;
  Arg args[25];
  int argn;

  Display *display;
  int     screen_num;

  /* initialize the DIW maps */
  for(argn=0; argn<NUM_DIW_MAPS; argn++) {
    global_diw_map[argn].width = 0;
    global_diw_map[argn].height = 0;
  }

  /* Create the application context */
  toplevel = XtAppInitialize(&app, "XDiw",
    NULL, 0, argc, argv, fallback_resources, NULL, 0);

  XtAppAddActions(app, actions, XtNumber(actions));

  display = XtDisplay(toplevel);
  screen_num = XDefaultScreen(display);

  XSynchronize(display, 1); /* for debugging */

  /* ---------------------------------------------------- */
  /* Create the Form widget for the buttons and diw_map's */
  form = XtVaCreateManagedWidget("form", formWidgetClass, toplevel, NULL);

  /* ------------------------------------------------------------ */
  /* Create the file menu, its button, and register its callbacks */
  file_menu_button = create_file_menu(form, toplevel, NULL);

  /* ------------------------------------------------------------ */
  /* Create the mesh menu, its button, and register its callbacks */
  mesh_menu_button = create_mesh_menu(form, toplevel, file_menu_button);

  /* ------------------------------------------------------------ */
  /* Create the warp menu, its button, and register its callbacks */
  warp_menu_button = create_warp_menu(form, toplevel, mesh_menu_button);

  /* ------------------------------------------------------------ */
  /* Create the help menu, its button, and register its callbacks */
  help_menu_button = create_help_menu(form, toplevel, warp_menu_button);
  XtVaSetValues(help_menu_button, XtNright, XtChainRight, NULL);

  /* ================================== */
  /* Create the box for the DIW widgets */
  argn=0;
  XtSetArg(args[argn], XtNorientation, XtorientHorizontal); argn++;
  XtSetArg(args[argn], XtNfromVert, file_menu_button); argn++;
  /* box changed to paned: WA (MJG 13sep95) */
  diw_box = XtCreateManagedWidget("diw_box", panedWidgetClass, form, args, argn);

  /* Initialize GC's, cursors, bitmaps, palette... */
  init_diw_stuff(toplevel);

    /* === Create the DIW panels === */
    /* --- Make the first diw_map --- */
    global_diw_map[0].mesh_t = 0.0;
    global_diw_map[0].img_t = 0.0; /* for the scrollbar Thumb */
    diw_widget1 = create_diw_widget(diw_box, &global_diw_map[0], width, height);

    /* Turn on the source and tween mesh on the first diw_map */
    dp_menu_cb(global_diw_map[0].sml, (XtPointer)&global_diw_map[0], NULL);
    dp_menu_cb(global_diw_map[0].smp, (XtPointer)&global_diw_map[0], NULL);
    dp_menu_cb(global_diw_map[0].tml, (XtPointer)&global_diw_map[0], NULL);

    /* --- Make the second diw_map --- */
    global_diw_map[1].mesh_t = 1.0;
    global_diw_map[1].img_t = 1.0; /* for the scrollbar Thumb */
    diw_widget2 = create_diw_widget(diw_box, &global_diw_map[1], width, height);

    /* These arrays are not used, so I'll free them */
    my_free(global_diw_map[1].mesh_xs, "initialize_application");
    my_free(global_diw_map[1].mesh_ys, "initialize_application");
    my_free(global_diw_map[1].mesh_xd, "initialize_application");
    my_free(global_diw_map[1].mesh_yd, "initialize_application");

    /* Make the 2 diw_maps share the same meshes */
    global_diw_map[1].mesh_xs = global_diw_map[0].mesh_xs;
    global_diw_map[1].mesh_ys = global_diw_map[0].mesh_ys;
    global_diw_map[1].mesh_xd = global_diw_map[0].mesh_xd;
    global_diw_map[1].mesh_yd = global_diw_map[0].mesh_yd;

    /* Turn on the destination and tween mesh on the second diw_map */
    dp_menu_cb(global_diw_map[1].dml, (XtPointer)&global_diw_map[1], NULL);
    dp_menu_cb(global_diw_map[1].dmp, (XtPointer)&global_diw_map[1], NULL);
    dp_menu_cb(global_diw_map[1].tml, (XtPointer)&global_diw_map[1], NULL);

  /* --------------------- */
  XtRealizeWidget(toplevel);

  /* Copy "original" images into the diw_map image spaces */
  reset_images(1);
  reset_images(2);

  /* Send the diw_map images to the X server */
  ReditherImage(diw_widget1, NULL, NULL, NULL);
  ReditherImage(diw_widget2, NULL, NULL, NULL);

  /* X Toolkit Main Loop (should be in main()) */
  return(app);
}

/* =============================================================== */

#define cl_match_arg(cmd) (!strcmp(argv[apc], cmd) && ((apc+1)<argc))

#define USAGE \
"   xmorph: Digital Image Warp for X Window System\n\
   Copyright (C) 1994 by Michael J. Gourlay\n\
   usage:  xmorph [options] [Xt options]\n\
     options:\n\
       -start  <starting image>\n\
       -finish <finishing image>\n\
       -src    <source mesh>\n\
       -dst    <destination mesh>\n"

int
main(argc, argv)
int argc;
char **argv;
{
  char *infn, *outfn;
  char *srcfn, *dstfn;
  rgba_image_t *src_imgP, *dst_imgP;
  XtAppContext app;

  int apc;

#ifdef SUNOS
  malloc_debug(2);
#endif

  infn=outfn=NULL;
  srcfn=dstfn=NULL;

  for(apc=1; apc<argc; apc++) {
    if(argv[apc][0] != '-') {
    } else {
      if(cl_match_arg("-start")) {
        infn = argv[++apc];
      } else if(cl_match_arg("-finish")) {
        outfn = argv[++apc];
      } else if(cl_match_arg("-src")) {
        srcfn = argv[++apc];
      } else if(cl_match_arg("-dst")) {
        dstfn = argv[++apc];
      } else if(!strcmp(argv[apc], "-verbose")) {
        verbose ++;
        fprintf(stderr, "verbose reporting\n");
      } else if(argv[apc][1] == '?') {
        fprintf(stderr, USAGE);
        exit(1);
      }
    }
  }

  if(infn==NULL && outfn!=NULL) {
    fprintf(stderr, "can't have finish image without start image.\n");
    fprintf(stderr, "swapping start and finish images.\n");
    infn=outfn;
    outfn=NULL;
  }

  /* Load the source image */
  src_imgP = &orig_image[0];
  src_imgP->ri = src_imgP->gi = src_imgP->bi = src_imgP->ai = NULL;
  if(infn!=NULL) {
    if(load_img(infn, src_imgP)) {
      fprintf(stderr, "could not open '%s'\n", infn);
      exit(1);
    }
  } else {
    src_imgP->ncols = 320;
    src_imgP->nrows = 240;
    make_test_image(src_imgP, 2);
  }

  /* Load the destination image */
  dst_imgP = &orig_image[1];
  dst_imgP->ri = dst_imgP->gi = dst_imgP->bi = dst_imgP->ai = NULL;
  if(infn!=NULL && outfn!=NULL) {
    if(load_img(outfn, dst_imgP)) {
      fprintf(stderr, "could not open '%s'\n", infn);
      exit(1);
    }
  } else {
    dst_imgP->ncols = src_imgP->ncols;
    dst_imgP->nrows = src_imgP->nrows;
    make_test_image(dst_imgP, 1);
  }

  app=initialize_application(src_imgP->ncols, src_imgP->nrows, &argc, argv);

  XtAppMainLoop(app);
}
