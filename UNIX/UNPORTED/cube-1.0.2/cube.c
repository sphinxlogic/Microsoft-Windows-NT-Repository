/*
 * cube.c: This is a cube solver and model of magic cube
 *The following references were used:
   "The X Window System Programming And Applications with Xt
   OSF/MOTIF EDITION"
   by Douglas A Young 
   Prentice-Hall, 1990.
   ISBN 0-13-642786-3

   "Mastering Rubik's Cube"
   by Don Taylor
   An Owl Book; Holt, Rinehart and Winston, New York, 1980
   ISBN 0-03-059941-5

-------------------------------------------------------------------
Copyright (C) 1993 by Pierre A. Fleurant
Permission is granted to copy and distribute this program
without charge, provided this copyright notice is included
in the copy.
This Software is distributed on an as-is basis. There will be
ABSOLUTELY NO WARRANTY for any part of this software to work
correct. In no case will the author be liable to you for damages
caused by the usage of this software.
-------------------------------------------------------------------
 */

#include "cube.h"
#include "common.h"

main(argc, argv)
   int   argc;
   char *argv[];
{
  Widget        toplevel, canvas, framework, command, menubar;
  Widget        help_button,init_cubik, pristine_button, quit_button;
  Widget        solve_it_button, load_file_button;
  graphics_data data;
  int           n;
  Arg           wargs[10];
  widget_data   wdata;

  /* init toolkit, create application context,
   * open display, create shell. This could be replaced 
   * by XtInitialize but we need appContext for other functions.
   */
  XtToolkitInitialize();
  appContext = XtCreateApplicationContext();
  xDisplay = XtOpenDisplay(appContext, "", argv[0], "MagicCb", NULL,0,
			   &argc, argv);
  toplevel = XtAppCreateShell(appContext, "MagicCube",
			      applicationShellWidgetClass,
			      xDisplay,NULL,0);
  /* put title into arg list */
  /* set title */
  n=0;
  XtSetArg(wargs[n], XtNtitle,"magicCube"); n++;
  XtSetArg(wargs[n], XtNiconName,"magicCube"); n++;
  XtSetValues(toplevel,wargs, n);
  
  /* Create and Manage widgets */
  framework = XtCreateManagedWidget("framework", 
                                    xmFormWidgetClass, 
                                    toplevel, NULL, 0);
  /*
   * Create the column to hold the commands.
   */
  command = XtCreateManagedWidget("command", 
                                  xmRowColumnWidgetClass, 
                                  framework, NULL, 0);
  
  /* add help button and help callbacks */
  help_button = XtCreateManagedWidget("Help",
                                      xmPushButtonWidgetClass,
                                      command, NULL, 0);
  XtAddCallback(help_button, XmNactivateCallback,
                xs_help_callback, tophelp_str);
  
  /* add init_cubik button callback */
  init_cubik = XtCreateManagedWidget("Init Cube",
                                      xmPushButtonWidgetClass,
                                      command, NULL, 0);
  XtAddCallback(init_cubik, XmNactivateCallback,
                xcube_init_cubik, &wdata);

  /*
   * Create and manage a menubar.
   */
  menubar = XmCreateMenuBar(command, "menubar", NULL, 0);
  XtManageChild(menubar); 

  /* add solve it button and solve it button  callbacks */
  solve_it_button = XtCreateManagedWidget("Solve",
                                      xmPushButtonWidgetClass,
                                      command, NULL, 0);
  XtAddCallback(solve_it_button, XmNactivateCallback,
                taylor, &wdata);
  
  /* add load_file_button and load_file_button callbacks */
  load_file_button = XtCreateManagedWidget("Load File",
                                      xmPushButtonWidgetClass,
                                      command, NULL, 0);
  XtAddCallback(load_file_button, XmNactivateCallback,
                ldfile, &wdata);   /* load  file */
  
  /* add pristine and quit buttons */
  /* This should be the last twoo buttons */
  pristine_button = XtCreateManagedWidget("Pristine",
					  xmPushButtonWidgetClass,
					  command, NULL, 0);
  XtAddCallback(pristine_button, XmNactivateCallback,
                init_cube, &wdata);
  
  quit_button = XtCreateManagedWidget("Quit",
                                      xmPushButtonWidgetClass,
                                      command, NULL, 0);
  XtAddCallback(quit_button, XmNactivateCallback,
                quit_it, &wdata);
  
  
  /*
   * Create the drawing surface
   */
  canvas = XtCreateManagedWidget("canvas",
                                 xmDrawingAreaWidgetClass, 
                                 framework, NULL, 0);

  XtAddEventHandler(canvas, ButtonPressMask, FALSE,
                    init_subplane_pattern, &wdata);
  /*
   * Create the menu from the description.
   */
  xss_create_menu_buttons(NULL, menubar, PulldownData,
                         XtNumber(PulldownData),&wdata);
  /* add the call backs for the canvas */
  XtAddCallback(canvas, XmNexposeCallback, refresh_cube, &wdata);

  /* specify form */
  n = 0;
  XtSetArg(wargs[n], XmNtopAttachment,    XmATTACH_FORM);n++;
  XtSetArg(wargs[n], XmNbottomAttachment, XmATTACH_FORM);n++;
  XtSetArg(wargs[n], XmNleftAttachment,   XmATTACH_FORM);n++;
  XtSetValues(command, wargs, n);

  n = 0;
  XtSetArg(wargs[n], XmNtopAttachment,   XmATTACH_FORM);  n++;
  XtSetArg(wargs[n], XmNbottomAttachment,XmATTACH_FORM);  n++;
  XtSetArg(wargs[n], XmNleftAttachment,  XmATTACH_WIDGET);n++;
  XtSetArg(wargs[n], XmNleftWidget,     command);         n++;
  XtSetArg(wargs[n], XmNrightAttachment,  XmATTACH_FORM); n++;
  XtSetValues(canvas, wargs, n);

  /*
   * Initialize the graphics buffer and other data.
   */
  /* init wdata stuff */
  wdata.toplevel = toplevel;
  wdata.canvas = canvas;
  wdata.graph_pointer = &data;
  init_data(canvas, &data, &wdata);

  /*
   * Register the seed colors as new patterns 
   * in preparation for pixmap browser
   */
  for(n=0; n < XtNumber(seed_color); n++)
    xs_register_pattern(toplevel, seed_color[n], fg_bitmap,
			fg_width, fg_height);
  
  XtRealizeWidget(toplevel);
  init_cube(canvas, &wdata, NULL);

  /*
   * Establish a passive grab on the drawing canvas window.
   */
  XGrabButton(XtDisplay(canvas), AnyButton, AnyModifier, 
              XtWindow(canvas), TRUE, 
              ButtonPressMask | ButtonMotionMask | 
              ButtonReleaseMask,
              GrabModeAsync, GrabModeAsync,
              XtWindow(canvas), 
              XCreateFontCursor(XtDisplay(canvas),
                                XC_crosshair));
  XtInstallAllAccelerators(canvas,framework);
  XtAppMainLoop(appContext);
}

init_data(w, data, wdata)
     Widget          w;
     graphics_data  *data;
     widget_data    *wdata;
{
  XGCValues       values;
  Arg             wargs[5];
  int             i,j,n;
  XPoint          short_face[NUM_SUBPLANES][5];
  double_XPoint   tmp0_face[NUM_SUBPLANES][5];
  
  /*
   * Get the colors the user has set for the widget. 
   */
  XtSetArg(wargs[0], XtNforeground, &data->foreground);
  XtSetArg(wargs[1], XtNbackground, &data->background);
  XtGetValues(w, wargs,2);
  /*
   * Fill in the values structure
   */  
  values.foreground = data->foreground;
  values.background = data->background;
  values.fill_style = FillTiled;
  /*
   * Get the GC used for lines in the cube
   */
  data->gc_line = XtGetGC(w, 
			  GCForeground | GCBackground | GCFillStyle,
			  &values);
  /* 
   * Get a gc for each subface_color. This will be used
   * when using XFillPolygon.
   */
  for(i=0;i<9;i++){
    for(j=0;j<6;j++){
      values.foreground = xss_get_pixel_by_name(w,seed_color[j]);
      values.background = xss_get_pixel_by_name(w,seed_color[j]);
      values.fill_style = FillTiled;
      wdata->subface_gc[i][j] =	XtGetGC(w,
					GCForeground | GCBackground | GCFillStyle,
					&values);
    }
  }
  /* init pixmap stuff */
  /*
   * Get the size of the drawing area.
   */
  n=0;
  XtSetArg(wargs[n], XtNwidth,  &wdata->width); n++;
  XtSetArg(wargs[n], XtNheight, &wdata->height); n++;
  XtGetValues(w, wargs, n); 
  
  /*
   * Create a default, writable, graphics context.
   */
  wdata->gc = XCreateGC(XtDisplay(w),
			DefaultRootWindow(XtDisplay(w)),
			NULL, NULL); 
  /*
   *  Initialize the pixmap to NULL.
   */
  wdata->pix = NULL;

  /* convert seed_face from CoordModePrevious to CoordModeOrigin */
  prev_to_org(seed_face);
  facecpy(cubik_face,seed_face); /* init cubik face: prestine */
  /* init */
  cubikcpy(front,seed_front);
  cubikcpy(left,seed_left);
  cubikcpy(right,seed_right);
  cubikcpy(back,seed_back);
  cubikcpy(top,seed_top);
  cubikcpy(bottom,seed_bottom);
  update_cubik(wdata);

  /* The following are used for align_subfaces in init_cubik */
  init_cornermap();
  init_edgemap(); 

  /* init regions for init_subplane_pattern callback */
  /* generate a prestine short_face for 
  /* values to regular XPoint array */
  facecpy(rot_face,seed_face);
  projface(tmp0_face,rot_face);
  norm_face(tmp0_face);
  shortface(short_face,tmp0_face);
  for(i=0; i<9; i++) 
    wdata->init_cubik_region[i] = XPolygonRegion(short_face[i],XtNumber(short_face[i]),WindingRule);
}

void quit_it(w, client_data, call_data)
     Widget   w;
     char    *client_data;
     caddr_t  call_data;
{
  XtCloseDisplay(XtDisplay(w));
  exit(0);
}

void clear_draw(w, wdata, call_data)
     Widget       w;
     widget_data *wdata;
     caddr_t      call_data;
{
  /* Clear the canvas */
  XClearArea(XtDisplay(wdata->toplevel), XtWindow(wdata->canvas), 0, 0, 0, 0, FALSE);
}

void xs_rot_face(rotated_face,src_face,size_rot_face,degrees)
     double_XPoint rotated_face[],*src_face;
     int size_rot_face;
     double degrees; 
{
  /* rotation of axis with fixed origin 
     x' = x cos(r) + y sin(r)
     y' = y cos(r) - x sin(r)
     
     180degree = PI radians

     degrees:

     /----------
     |     ^
     |     |
     |    /
     |   / degrees,      
     |  /  
     |--         
     |         
     |         

     
   */

  double_XPoint *tmp_xpoint;
  int i;
  double radians;

  tmp_xpoint = src_face;
  radians = degrees * M_PI / 180.0;
  for(i=0; i < size_rot_face; i++) {
    rotated_face[i].x = (tmp_xpoint->x * cos(radians) + tmp_xpoint->y * sin(radians));
    rotated_face[i].y = (tmp_xpoint->y * cos(radians) - tmp_xpoint->x * sin(radians));
    rotated_face[i].z =  tmp_xpoint->z; 
    tmp_xpoint++;
  };

}

void xs_spin_face(spin_face,src_face,size_spin_face,degrees)
     double_XPoint spin_face[],*src_face;
     int size_spin_face;
     double degrees; 
{
  /* rotation of z-axis with fixed origin 
   */

  double_XPoint *tmp_xpoint;
  int i;
  double radians;

  tmp_xpoint = src_face;
  radians = degrees * M_PI / 180.0;
  for(i=0; i < size_spin_face; i++) {
    spin_face[i].x = (tmp_xpoint->x * cos(radians) + tmp_xpoint->z * sin(radians));
    spin_face[i].y =  tmp_xpoint->y;
    spin_face[i].z = (tmp_xpoint->z * cos(radians) - tmp_xpoint->x * sin(radians));
    tmp_xpoint++;
  };

}

void xs_flip_face(flip_face,src_face,size_flip_face,degrees)
     double_XPoint flip_face[],*src_face;
     int size_flip_face;
     double degrees; 
{
  /* rotation of x-axis with fixed origin 
   */

  double_XPoint *tmp_xpoint;
  int i;
  double radians;

  tmp_xpoint = src_face;
  radians = degrees * M_PI / 180.0;
  for(i=0; i < size_flip_face; i++) {
    flip_face[i].x =  tmp_xpoint->x;
    flip_face[i].y = (tmp_xpoint->y * cos(radians) + tmp_xpoint->z * sin(radians));
    flip_face[i].z = (tmp_xpoint->z * cos(radians) - tmp_xpoint->y * sin(radians));
    tmp_xpoint++;
  };

}

void norm_face(dest_face)
     double_XPoint dest_face[NUM_SUBPLANES][5];
{
  /*old way: Normalize the 'rotated face' */
  /*new: just shift over into 'center' of window */
  int i,j;

  for (i=0;i<=NUM_SUBPLANES-1;i++)
    for(j=0;j<=4;j++){
      dest_face[i][j].x += WINDOW_CENTER_X;
      dest_face[i][j].y += WINDOW_CENTER_Y;
    }
  
}

void facecpy(dest_face,src_face)
     double_XPoint dest_face[NUM_SUBPLANES][5],src_face[NUM_SUBPLANES][5];
{
  int i,j;
  
  for(i=0;i<=NUM_SUBPLANES-1;i++)
    for(j=0;j<=4;j++){
      dest_face[i][j].x = src_face[i][j].x;
      dest_face[i][j].y = src_face[i][j].y;
      dest_face[i][j].z = src_face[i][j].z;
    }
}

void cubikcpy(dest_cubik,src_cubik)
     int dest_cubik[NUM_CUBIKPLANES],src_cubik[NUM_CUBIKPLANES];
{
  int i;
  
  for(i=0;i<=NUM_CUBIKPLANES-1;i++)
    dest_cubik[i] = src_cubik[i];
}

void lefthand_twist(src_cubik)
     int src_cubik[NUM_CUBIKPLANES];
{
  int temp_subplanes[NUM_CUBIKPLANES];

  temp_subplanes[0]  = src_cubik[2];
  temp_subplanes[1]  = src_cubik[5];
  temp_subplanes[2]  = src_cubik[8];
  temp_subplanes[3]  = src_cubik[1];
  temp_subplanes[4]  = src_cubik[4];
  temp_subplanes[5]  = src_cubik[7];
  temp_subplanes[6]  = src_cubik[0];
  temp_subplanes[7]  = src_cubik[3];
  temp_subplanes[8]  = src_cubik[6];
  cubikcpy(src_cubik,temp_subplanes); 

}

void righthand_twist(src_cubik)
     int src_cubik[NUM_CUBIKPLANES];
{
  int temp_subplanes[NUM_CUBIKPLANES];

  temp_subplanes[0]  = src_cubik[6];
  temp_subplanes[1]  = src_cubik[3];
  temp_subplanes[2]  = src_cubik[0];
  temp_subplanes[3]  = src_cubik[7];
  temp_subplanes[4]  = src_cubik[4];
  temp_subplanes[5]  = src_cubik[1];
  temp_subplanes[6]  = src_cubik[8];
  temp_subplanes[7]  = src_cubik[5];
  temp_subplanes[8]  = src_cubik[2];
  cubikcpy(src_cubik,temp_subplanes); 

}

void update_cubik(wdata)
     widget_data   *wdata;
{
  /* This updates xxx[9,10,...20]
   * interms of xxx[0,1,...8].
   * This is called after cubik() in cubik_xxxx(), and in align_subfaces
   *      
   */

  int i;
  
  front[9]  = top[0];
  front[10] = top[1];
  front[11] = top[2];
 
  front[12] = right[0];
  front[13] = right[3];
  front[14] = right[6];

  front[15] = bottom[0];
  front[16] = bottom[1];
  front[17] = bottom[2];

  front[18] = left[0];
  front[19] = left[3];
  front[20] = left[6];

  
  left[9]  = top[0];
  left[10] = top[3];
  left[11] = top[6];

  left[12] = front[0];
  left[13] = front[3];
  left[14] = front[6];

  left[15] = bottom[0];
  left[16] = bottom[3];
  left[17] = bottom[6];

  left[18] = back[0];
  left[19] = back[3];
  left[20] = back[6];
  

  right[9]  = top[2];
  right[10] = top[5];
  right[11] = top[8];

  right[12] = front[2];
  right[13] = front[5];
  right[14] = front[8];

  right[15] = bottom[2];
  right[16] = bottom[5];
  right[17] = bottom[8];

  right[18] = back[2];
  right[19] = back[5];
  right[20] = back[8];
  

  back[9]  = top[6];
  back[10] = top[7];
  back[11] = top[8];

  back[12] = right[2];
  back[13] = right[5];
  back[14] = right[8];

  back[15] = left[2];
  back[16] = left[5];
  back[17] = left[8];

  back[18] = bottom[6];
  back[19] = bottom[7];
  back[20] = bottom[8];
  

  top[9]  = front[0];
  top[10] = front[1];
  top[11] = front[2];

  top[12] = right[0];
  top[13] = right[1];
  top[14] = right[2];

  top[15] = left[0];
  top[16] = left[1];
  top[17] = left[2];

  top[18] = back[0];
  top[19] = back[1];
  top[20] = back[2];
  

  bottom[9]  = front[6];
  bottom[10] = front[7];
  bottom[11] = front[8];

  bottom[12] = right[6];
  bottom[13] = right[7];
  bottom[14] = right[8];

  bottom[15] = left[6];
  bottom[16] = left[7];
  bottom[17] = left[8];

  bottom[18] = back[6];
  bottom[19] = back[7];
  bottom[20] = back[8];

  /* update subface_side[][] */
  for(i=0; i<9; i++){
    wdata->subface_side[i][0] = front[i]    / 9;
    wdata->subface_side[i][1] = left[i]     / 9;
    wdata->subface_side[i][2] = right[i]    / 9;
    wdata->subface_side[i][3] = back[i]     / 9;
    wdata->subface_side[i][4] = top[i]      / 9;
    wdata->subface_side[i][5] = bottom[i]   / 9;
  }
}

void prev_to_org(seed_data)
     double_XPoint seed_data[NUM_SUBPLANES][5];
{
  double_XPoint tmp_data[NUM_SUBPLANES][5];
  int i,j;

  for(i=0;i<=NUM_SUBPLANES-1;i++){
    tmp_data[i][0].x = seed_data[i][0].x;
    tmp_data[i][1].x = seed_data[i][0].x + seed_data[i][1].x;
    tmp_data[i][2].x = seed_data[i][0].x + seed_data[i][1].x + seed_data[i][2].x;
    tmp_data[i][3].x = seed_data[i][0].x + seed_data[i][1].x + seed_data[i][2].x + seed_data[i][3].x;
    tmp_data[i][4].x = seed_data[i][0].x + seed_data[i][1].x + seed_data[i][2].x + seed_data[i][3].x + seed_data[i][4].x;
    
    tmp_data[i][0].y = seed_data[i][0].y;
    tmp_data[i][1].y = seed_data[i][0].y + seed_data[i][1].y;
    tmp_data[i][2].y = seed_data[i][0].y + seed_data[i][1].y + seed_data[i][2].y;
    tmp_data[i][3].y = seed_data[i][0].y + seed_data[i][1].y + seed_data[i][2].y + seed_data[i][3].y;
    tmp_data[i][4].y = seed_data[i][0].y + seed_data[i][1].y + seed_data[i][2].y + seed_data[i][3].y + seed_data[i][4].y;
    
    tmp_data[i][0].z = seed_data[i][0].z;
    tmp_data[i][1].z = seed_data[i][0].z + seed_data[i][1].z;
    tmp_data[i][2].z = seed_data[i][0].z + seed_data[i][1].z + seed_data[i][2].z;
    tmp_data[i][3].z = seed_data[i][0].z + seed_data[i][1].z + seed_data[i][2].z + seed_data[i][3].z;
    tmp_data[i][4].z = seed_data[i][0].z + seed_data[i][1].z + seed_data[i][2].z + seed_data[i][3].z + seed_data[i][4].z;
  }
  facecpy(seed_data,tmp_data);
}
  
void projface(dest_face,src_face)
     double_XPoint dest_face[NUM_SUBPLANES][5];
     double_XPoint src_face[NUM_SUBPLANES][5];
{
  double_XPoint tmp_face[NUM_SUBPLANES][5];
  int i,j;
  
  /* Project src_face onto the z=PROJECTION_DEPTH plane.
   */
  for(i=0;i<=NUM_SUBPLANES-1;i++)
    for(j=0;j<=4;j++){
      /* project onto z=PROJECTION_DEPTH plane */
      /* project with cube at origin, then shift back */
      dest_face[i][j].x = (src_face[i][j].x-MR_X-FACE0_WIDTH*1.5)/(src_face[i][j].z/PROJECTION_DEPTH +1)
                                          +(MR_X+FACE0_WIDTH*1.5);
      dest_face[i][j].y = (src_face[i][j].y-MR_Y-FACE0_HEIGHT*1.5)/(src_face[i][j].z/PROJECTION_DEPTH +1)
                                          +(MR_Y+FACE0_HEIGHT*1.5);

      dest_face[i][j].z = src_face[i][j].z;
    }
}

void shortface(dest_face,src_face)
     XPoint dest_face[NUM_SUBPLANES][5];
     double_XPoint src_face[NUM_SUBPLANES][5];
{
  int i,j;
  
  /* Convert the double_XPoint array to a plain XPoint array.
   */
  for(i=0;i<=NUM_SUBPLANES-1;i++)
    for(j=0;j<=4;j++){
      /* Ignore the z plane */
      dest_face[i][j].x = (short) floor(src_face[i][j].x + .5);
      dest_face[i][j].y = (short) floor(src_face[i][j].y + .5);
    }
}

void depth_sort(draw_order,src_face)
     int draw_order[NUM_SUBPLANES];
     double_XPoint src_face[NUM_SUBPLANES][5];
{
  struct {
    double z_value;
    int    subplane_number;
  } sort_array[NUM_SUBPLANES];
  int gap,i,j,subplane_number_temp;
  double z_temp;
  
  /* Depth-sort algorthm for determining which faces are 
     to be drawn. 
     Draw_order gives the order of how the subplanes are to be drawn,
     given the subplane number. 0 is last, NUM_SUBPLANES-1 is first. The 
     side with the deepest z value gets drawn first.
     
     The z value of the center subface of each side is 
     sorted into asending order. 
     Then draw_order is set accordingly.
     */
  
  /* init sort_array
   */
  for(i=0;i<=NUM_SUBPLANES-1;i++){
    sort_array[i].z_value = (src_face[i][0].z + src_face[i][2].z)/2; /* center point subplane */
    sort_array[i].subplane_number = i;
  }
  
  /* use Shell sort method to sort it out */
  for(gap = NUM_SUBPLANES/2; gap > 0; gap /= 2)
    for(i = gap; i < NUM_SUBPLANES; i++)
      for(j = i-gap;j >= 0 && sort_array[j].z_value > sort_array[j+gap].z_value;j -= gap){
	/* swap `em */
	z_temp = sort_array[j].z_value;
	sort_array[j].z_value = sort_array[j+gap].z_value;
	sort_array[j+gap].z_value = z_temp;
	subplane_number_temp = sort_array[j].subplane_number;
	sort_array[j].subplane_number = sort_array[j+gap].subplane_number;
	sort_array[j+gap].subplane_number = subplane_number_temp;
      }
  /* Copy the ascending order into draw_order.
   */
  for(i=0;i<NUM_SUBPLANES;i++)
    draw_order[i]= sort_array[i].subplane_number;
}

void draw_it(dpy, win, wdata, short_face,draw_order)
     Display *dpy;
     Window   win;
     widget_data *wdata;
     XPoint short_face[NUM_SUBPLANES][5];
     int draw_order[NUM_SUBPLANES];
{
  int i,k,l;
  Arg wargs[10];

  /* process other events first */
  check_events();

  /*
   *  Get the new window size.
   */
  XtSetArg(wargs[0], XtNwidth,  &wdata->width); 
  XtSetArg(wargs[1], XtNheight, &wdata->height);
  XtGetValues(wdata->canvas, wargs, 2);

  /*
   *  Free the old pixmap and create a new pixmap 
   *  the size of the window.
   */
  if(wdata->pix)
    XFreePixmap(XtDisplay(wdata->canvas), wdata->pix);
  wdata->pix= XCreatePixmap(XtDisplay(wdata->canvas),
			    DefaultRootWindow(XtDisplay(wdata->canvas)),
			    (Dimension)wdata->width, (Dimension)wdata->height, 
			    DefaultDepthOfScreen(XtScreen(wdata->canvas)));
  XSetForeground(XtDisplay(wdata->canvas), wdata->gc,
                 WhitePixelOfScreen(XtScreen(wdata->canvas)));
  XFillRectangle(XtDisplay(wdata->canvas), wdata->pix, wdata->gc, 0, 0, 
                 (Dimension)wdata->width,  (Dimension)wdata->height);
  /*
   * draw the new cube
   */
  /* Draw all the subplanes in the draw order */
  for(i=NUM_SUBPLANES-1;i>=0;i--){
    k = draw_order[i] % 9;
    l = draw_order[i] / 9;
    XFillPolygon(dpy, wdata->pix, wdata->subface_gc[k][l],
		 short_face[draw_order[i]], XtNumber(short_face[draw_order[i]]),
		 Nonconvex,CoordModeOrigin);
    XDrawLines(dpy, wdata->pix, wdata->graph_pointer->gc_line,
	       short_face[draw_order[i]], XtNumber(short_face[draw_order[i]]),
	       CoordModeOrigin);
  }
  if(XtIsRealized(wdata->canvas) && Draw_Enable)
    XCopyArea(XtDisplay(wdata->canvas), wdata->pix, XtWindow(wdata->canvas), wdata->gc, 
	      0, 0, wdata->width, wdata->height, 
	      0, 0);
}

void rot_it(w, wdata, call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  Display *dpy = XtDisplay(wdata->canvas);
  Window   win = XtWindow(wdata->canvas);
  double_XPoint   tmp0_face[NUM_SUBPLANES][5];
  XPoint short_face[NUM_SUBPLANES][5];
  int draw_order[NUM_SUBPLANES];
  int i;

  rot_angle += Delta_Angle;
  rot_angle %= 360;
/*  printf("rot=%d;  spin=%d;  flip=%d;\n",rot_angle,spin_angle,flip_angle);*/

  for(i=0;i<=NUM_SUBPLANES-1;i++)  /* NUM_SUBPLANES subplanes  */ 
    xs_rot_face(tmp0_face[i],rot_face[i],XtNumber(tmp0_face[i]),Delta_Angle);
  facecpy(rot_face,tmp0_face);
  projface(tmp0_face,rot_face);
  norm_face(tmp0_face); 
  depth_sort(draw_order,tmp0_face);
  shortface(short_face,tmp0_face);
  draw_it(dpy, win, wdata, short_face,draw_order);
}

void flip_it(w, wdata, call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  Display *dpy = XtDisplay(wdata->canvas);
  Window   win = XtWindow(wdata->canvas);
  double_XPoint   tmp0_face[NUM_SUBPLANES][5];
  XPoint short_face[NUM_SUBPLANES][5];
  int draw_order[NUM_SUBPLANES];
  int i;

  flip_angle += Delta_Angle;
  flip_angle %= 360;
/*  printf("rot=%d;  spin=%d;  flip=%d;\n",rot_angle,spin_angle,flip_angle);*/

  for(i=0;i<=NUM_SUBPLANES-1;i++)  /* NUM_SUBPLANES subplanes  */ 
    xs_flip_face(tmp0_face[i],rot_face[i],XtNumber(tmp0_face[i]),Delta_Angle);
  facecpy(rot_face,tmp0_face);
  projface(tmp0_face,rot_face);
  norm_face(tmp0_face);
  depth_sort(draw_order,tmp0_face);
  shortface(short_face,tmp0_face);
  draw_it(dpy, win, wdata, short_face,draw_order);
}

void spin_it(w, wdata, call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  Display *dpy = XtDisplay(wdata->canvas);
  Window   win = XtWindow(wdata->canvas);
  double_XPoint   tmp0_face[NUM_SUBPLANES][5];
  XPoint short_face[NUM_SUBPLANES][5];
  int draw_order[NUM_SUBPLANES];
  int i;

  spin_angle += Delta_Angle;
  spin_angle %= 360;
/*  printf("rot=%d;  spin=%d;  flip=%d;\n",rot_angle,spin_angle,flip_angle);*/

  for(i=0;i<=NUM_SUBPLANES-1;i++)  /* NUM_SUBPLANES subplanes  */ 
    xs_spin_face(tmp0_face[i],rot_face[i],XtNumber(tmp0_face[i]),Delta_Angle);
  facecpy(rot_face,tmp0_face);
  projface(tmp0_face,rot_face);
  norm_face(tmp0_face);
  depth_sort(draw_order,tmp0_face);
  shortface(short_face,tmp0_face);
  draw_it(dpy, win, wdata, short_face,draw_order);
}

void init_cube(w, wdata, call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  Display *dpy = XtDisplay(wdata->canvas);
  Window   win = XtWindow(wdata->canvas);
  XPoint short_face[NUM_SUBPLANES][5];
  double_XPoint tmp0_face[NUM_SUBPLANES][5];
  int draw_order[NUM_SUBPLANES];
  XGCValues       values;
  int i,j;
  
  /* Init subplane_side[][] */
  /* This is used for init_cubik. */
  for(i=0; i < 6; i++)
    for(j=0; j < 9; j++)
      wdata->subface_side[j][i] = i;
  
  /* reset angles */
  rot_angle = 0;
  spin_angle = 0;
  flip_angle = 0;
  /* re-init subplanes' gc */
  /* 
   * Change gc for each subface_color. This will be used
   * when using XFillPolygon.
   */
  for(i=0;i<9;i++){
    for(j=0;j<6;j++){
      values.foreground = xss_get_pixel_by_name(w,seed_color[j]);
      values.background = xss_get_pixel_by_name(w,seed_color[j]);
      values.fill_style = FillTiled;
      wdata->subface_gc[i][j] = XtGetGC(w, 
					GCForeground | GCBackground | GCFillStyle,
					&values);
    }
  }
  
  /* init cubik face: prestine */
  facecpy(cubik_face,seed_face); 
  /* init */
  cubikcpy(front,seed_front);
  cubikcpy(left,seed_left);
  cubikcpy(right,seed_right);
  cubikcpy(back,seed_back);
  cubikcpy(top,seed_top);
  cubikcpy(bottom,seed_bottom);
  update_cubik(wdata);
  /* display the seed face only */
  /* values to regular XPoint array */
  facecpy(rot_face,seed_face);
  projface(tmp0_face,rot_face);
  norm_face(tmp0_face);
  depth_sort(draw_order,tmp0_face);
  shortface(short_face,tmp0_face);
  draw_it(dpy, win, wdata, short_face,draw_order);
}

void refresh_cube(w, wdata, call_data)
     Widget          w;
     widget_data     *wdata;
     XmDrawingAreaCallbackStruct    *call_data;
{
  XExposeEvent  *event = (XExposeEvent *) call_data->event;
  /*
   * Extract the exposed area from the event and copy
   * from the saved pixmap to the window.
   */
  if(XtIsRealized(wdata->canvas) && Draw_Enable)
    XCopyArea(XtDisplay(w), wdata->pix, XtWindow(w), wdata->gc, 
	      event->x, event->y, event->width, event->height, 
	      event->x, event->y);
}

/*    cubik_front, cubik_left,...callbacks 
 *    These operate on the cube to rotate only the
 *    1/3 of the cube specified. 
 *    Cubik is called 3 times for 3-30degree turns.
 */
void cubik_front(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  int i;
  int temp[NUM_CUBIKPLANES];

  for(i=1;i<=3;i++)
    cubik(w,wdata,call_data,front,xs_rot_face);

  /* Now points in plane 0 are in old plane 6,
   * points in plane 1, are in old plane 3 etc....
   * so the data must be swapped so that another 
   * op starts with the correct 'inited' cube.
   * Also the ->subface_gc[k][l] graphic contexts 
   * must be swapped.
   *  Instead of actually moving double_Xpoint and 
   * ->subface_gc[k][l] data 
   * around, we just transform front,
   * left etc.
   * After a cubik_front operation, front 
   * and the OTHER xxxx_subplane arrays that are affected
   * are updated-transformed.
   */
  lefthand_twist(front);

  /* left, top, right, and bottom are affected */
  temp[0] = left[0];
  temp[3] = left[3];
  temp[6] = left[6];

  left[0] = top[2];
  left[3] = top[1];
  left[6] = top[0];

  top[0] = right[0]; 
  top[1] = right[3];
  top[2] = right[6];

  right[0] = bottom[2];
  right[3] = bottom[1];
  right[6] = bottom[0];

  bottom[0] = temp[0];
  bottom[1] = temp[3];
  bottom[2] = temp[6];

  update_cubik(wdata);
}

void cubik_left(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  int i;
  int temp[NUM_CUBIKPLANES];

  for(i=1;i<=3;i++)
    cubik(w,wdata,call_data,left,xs_flip_face);
  
  /* Now points in plane 0 are in old plane 6,
   * points in plane 1, are in old plane 3 etc....
   */
  righthand_twist(left);

  /* other planes affected */
  temp[0] = front[0];
  temp[3] = front[3];
  temp[6] = front[6];

  front[0] = bottom[0];
  front[3] = bottom[3];
  front[6] = bottom[6];

  bottom[0] = back[6]; 
  bottom[3] = back[3];
  bottom[6] = back[0];

  back[0] = top[0];
  back[3] = top[3];
  back[6] = top[6];

  top[0] = temp[6];
  top[3] = temp[3];
  top[6] = temp[0];

  update_cubik(wdata);

}

void cubik_right(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  int i;
  int temp[NUM_CUBIKPLANES];

  for(i=1;i<=3;i++)
    cubik(w,wdata,call_data,right,xs_flip_face);
  righthand_twist(right);
  /* other planes affected */
  temp[2] = front[2];
  temp[5] = front[5];
  temp[8] = front[8];

  front[2] = bottom[2];
  front[5] = bottom[5];
  front[8] = bottom[8];

  bottom[2] = back[8]; 
  bottom[5] = back[5];
  bottom[8] = back[2];

  back[2] = top[2];
  back[5] = top[5];
  back[8] = top[8];

  top[2] = temp[8];
  top[5] = temp[5];
  top[8] = temp[2];

  update_cubik(wdata);
}

void cubik_back(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  int i;
  int temp[NUM_CUBIKPLANES];

  for(i=1;i<=3;i++)
    cubik(w,wdata,call_data,back,xs_rot_face);
  lefthand_twist(back);
  /* other planes affected */
  temp[2] = right[2];
  temp[5] = right[5];
  temp[8] = right[8];

  right[2] = bottom[8];
  right[5] = bottom[7];
  right[8] = bottom[6];

  bottom[6] = left[2]; 
  bottom[7] = left[5];
  bottom[8] = left[8];

  left[2] = top[8];
  left[5] = top[7];
  left[8] = top[6];

  top[6] = temp[2];
  top[7] = temp[5];
  top[8] = temp[8];

  update_cubik(wdata);
}

void cubik_top(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  int i;
  int temp[NUM_CUBIKPLANES];

  for(i=1;i<=3;i++)
    cubik(w,wdata,call_data,top,xs_spin_face);
  lefthand_twist(top);
  /* other planes affected */
  temp[0] = front[0];
  temp[1] = front[1];
  temp[2] = front[2];

  front[0] = right[0];
  front[1] = right[1];
  front[2] = right[2];

  right[0] = back[2]; 
  right[1] = back[1];
  right[2] = back[0];

  back[0] = left[0];
  back[1] = left[1];
  back[2] = left[2];

  left[0] = temp[2];
  left[1] = temp[1];
  left[2] = temp[0];

  update_cubik(wdata);
}

void cubik_bottom(w,wdata,call_data)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
{
  int i;
  int temp[NUM_CUBIKPLANES];

  for(i=1;i<=3;i++)
    cubik(w,wdata,call_data,bottom,xs_spin_face);
  lefthand_twist(bottom);
  /* other planes affected */
  temp[6] = front[6];
  temp[7] = front[7];
  temp[8] = front[8];

  front[6] = right[6];
  front[7] = right[7];
  front[8] = right[8];

  right[6] = back[8]; 
  right[7] = back[7];
  right[8] = back[6];

  back[6] = left[6];
  back[7] = left[7];
  back[8] = left[8];

  left[6] = temp[8];
  left[7] = temp[7];
  left[8] = temp[6];

  update_cubik(wdata);
}


void cubik(w, wdata, call_data, subplanes, xs_op_face)
     Widget   w;
     widget_data *wdata;
     caddr_t  call_data;
     int subplanes[21];
     void   (*xs_op_face)();
{
  Display *dpy = XtDisplay(wdata->canvas);
  Window   win = XtWindow(wdata->canvas);
  double_XPoint   tmp0_face[NUM_SUBPLANES][5];
  XPoint short_face[NUM_SUBPLANES][5];
  int draw_order[NUM_SUBPLANES];
  int i,j;

  /* Rotate all the planes and normalize, 
   * but only copy the xxxx to the
   * the rot_face[] array.
   * 
   *  Start from cubiked face, (cubik was inited to seed_face in init_data).
   * Then after cubikking, return to 
   * to the latest rot, spin, and flip angles.
   * Update cubik with new rot face
   */


  /* rotate all of cubik */
  for(i=0;i<=NUM_SUBPLANES-1;i++)
    xs_op_face(tmp0_face[i],cubik_face[i],XtNumber(tmp0_face[i]),(double)30.0);

  /* init rot face back to cubik. This will get roted, spinned, and flipped later */
  facecpy(rot_face,cubik_face);
  
  /* only copy over the planes we want */
  for(i=0;i<=NUM_CUBIKPLANES-1;i++) 
    for(j=0;j<=4;j++){
      rot_face[subplanes[i]][j].x = tmp0_face[subplanes[i]][j].x;
      rot_face[subplanes[i]][j].y = tmp0_face[subplanes[i]][j].y;
      rot_face[subplanes[i]][j].z = tmp0_face[subplanes[i]][j].z;
    }
  
  /* update cubik */
  facecpy(cubik_face,rot_face);


  /* now rot, spin, and flip back to the latest angles.
   * Note: the order of the rot, spin, and flip angles makes a difference...
   * but that info is not available...so cube may not go back to exact position.
   */
  for(i=0;i<=NUM_SUBPLANES-1;i++)  
    xs_rot_face(tmp0_face[i],rot_face[i],XtNumber(tmp0_face[i]),(double)rot_angle);
  facecpy(rot_face,tmp0_face);
  for(i=0;i<=NUM_SUBPLANES-1;i++)  
    xs_spin_face(tmp0_face[i],rot_face[i],XtNumber(tmp0_face[i]),(double)spin_angle);
  facecpy(rot_face,tmp0_face);
  for(i=0;i<=NUM_SUBPLANES-1;i++)  
    xs_flip_face(tmp0_face[i],rot_face[i],XtNumber(tmp0_face[i]),(double)flip_angle);
  facecpy(rot_face,tmp0_face);    

  /* prep to draw, then draw */
  facecpy(tmp0_face,rot_face); /* copy it back to tmp0 */
  projface(tmp0_face,rot_face);
  norm_face(tmp0_face);
  depth_sort(draw_order,tmp0_face);
  shortface(short_face,tmp0_face);
  draw_it(dpy, win, wdata, short_face,draw_order);
}

void set_fill_pattern(w, data, call_data)
     Widget         w;
     graphics_data *data;
     XmToggleButtonCallbackStruct  *call_data;
{
  Pixmap    tile;
  int       i,j;
  XGCValues values;
  Arg       wargs[1];
  
  /* If not called for the right reason, return */
  if(call_data->reason != XmCR_VALUE_CHANGED ||
     !call_data->set)
    return;

  /* userData is the index into seed_color;
   * index is same as button number.
   */
  XtSetArg(wargs[0], XmNuserData, &j);
  XtGetValues(w, wargs, 1);
  data->init_cubik_color = j;
  /* indicate that a fill pattern has been chosen */
  Fill_Pattern_Chosen = TRUE;
  
}

void set_cube_side(w, wdata, call_data)
     Widget         w;
     widget_data    *wdata;
     XmToggleButtonCallbackStruct  *call_data;
{
  XmString  xmstr;
  int       i;
  Arg       wargs[1];
  Cardinal  side_number;
  
  /* If not called for the right reason, return */
  if(call_data->reason != XmCR_VALUE_CHANGED || 
     !call_data->set)
    return;
  
  XtSetArg(wargs[0], XmNuserData, &side_number);
  XtGetValues(w, wargs, 1);
  
  wdata->graph_pointer->init_cubik_number = side_number;
  /* init */
  wdata->graph_pointer->init_cubik_mirror = FALSE;
  
  /* Crack side picked, then rot, spin, or flip accordingly */
  /* orient cube */
  Draw_Enable = FALSE;
   Delta_Angle = -rot_angle;
   rot_it(wdata->canvas, wdata, call_data);
   Delta_Angle = -spin_angle;
   spin_it(wdata->canvas, wdata, call_data);
   Delta_Angle = -flip_angle;
   flip_it(wdata->canvas, wdata, call_data);
  Draw_Enable = TRUE;
  switch(side_number){
  case 0: 
    /* front */
    /* oriented already */
    Delta_Angle = 0;
    spin_it(wdata->canvas, wdata, call_data);
    break;
  case 1:
    /* left */
    Delta_Angle = 270;
    spin_it(wdata->canvas, wdata, call_data);
    wdata->graph_pointer->init_cubik_mirror = TRUE;
    break;
  case 2:
    /* right */
    Delta_Angle = 90;
    spin_it(wdata->canvas, wdata, call_data);
    break;
  case 3:
    /* back */
    Delta_Angle = 180;
    spin_it(wdata->canvas, wdata, call_data);
    wdata->graph_pointer->init_cubik_mirror = TRUE;
    break;
  case 4:
    /* top */
    Draw_Enable = FALSE;
    Delta_Angle = 270;
    flip_it(wdata->canvas, wdata, call_data);
    Draw_Enable = TRUE;
    Delta_Angle = 180;
    rot_it(wdata->canvas, wdata, call_data);
    wdata->graph_pointer->init_cubik_mirror = TRUE;
    break;
  case 5:
    /* bottom */
    Delta_Angle = 90;
    flip_it(wdata->canvas, wdata, call_data);
    break;
  }
  /* restore */
  Delta_Angle = DEFAULT_DELTA_ANGLE;
  /* indicate that a side has be chosen */
  Cube_Side_Chosen = TRUE;
}

void init_subplane_pattern(w, wdata, event)
     Widget          w;
     widget_data     *wdata;
     XEvent          *event;
{
  int i;
  XGCValues values;
  int mirror[9];
  
  /* If not called for the right reason, return */
  if(!Cube_Side_Chosen ||
     !Fill_Pattern_Chosen)
    return;
  
  /* init mirror */
  if(wdata->graph_pointer->init_cubik_mirror){
    mirror[0]=2;
    mirror[1]=1;
    mirror[2]=0;
    mirror[3]=5;
    mirror[4]=4;
    mirror[5]=3;
    mirror[6]=8;
    mirror[7]=7;
    mirror[8]=6;
  }
  else
    for(i=0; i<9; i++)
      mirror[i]=i;
  
  for(i=0; i<9; i++){
    /* Can't change the center cube (subplane) */
    if(i == 4)
      continue;
    if(XPointInRegion(wdata->init_cubik_region[i],event->xbutton.x,event->xbutton.y)){
      /* Note: To change color of a face after a click, we just brute force change the subface_gc[][]
       * of the cube. This is effectively just re-initing the gc to a new color for that face.
       *    In align_subfaces, when cubik_face is determined the subface_gc[][]'s get re-inited 
       * to their original colors.
       */
      values.foreground = xss_get_pixel_by_name(w,seed_color[wdata->graph_pointer->init_cubik_color]);
      values.background = xss_get_pixel_by_name(w,seed_color[wdata->graph_pointer->init_cubik_color]);
      values.fill_style = FillTiled;
      wdata->subface_gc[mirror[i]][wdata->graph_pointer->init_cubik_number] = 
	XtGetGC(w, 
		GCForeground | GCBackground | GCFillStyle,
		&values);
      /* update subface_side */
      wdata->subface_side[mirror[i]][wdata->graph_pointer->init_cubik_number] = wdata->graph_pointer->init_cubik_color;
      /* dummy draw */
      Delta_Angle = 0;
      spin_it(wdata->canvas, wdata, NULL);
      /* restore */
      Delta_Angle = DEFAULT_DELTA_ANGLE;
    }
  }
}

void align_subfaces(w, wdata, call_data)
     Widget               w; 
     widget_data     *wdata;
     XmAnyCallbackStruct *call_data; 
{
  /* This is called by xcic_done_callback in init_cubik.
    
     Given the updated wdata->subface_side[][] 
     the front[], left[], right[], back[].....
     arrays must be `aligned'. If a corner cube 
     face changes color, then its other 2 faces
     may have to change front[], left[], right[], back[].....
     values even though it does not change color.
   */

  /* There are 8 corner cubes that must have 3
     different color faces.
     Each corner cube is represented by 3 wdata->subface_side[][]
     values.
      Using the macros we have:
             Front(0),Top(0),Left(0)
	     Front(2),Top(2),Right(0)
	     Front(6),Bottom(0),Left(6)
	     Front(8),Bottom(2), Right(6)
	     Back(0),Top(6),Left(2)
	     Back(2),Top(8),Right(2)
	     Back(6),Bottom(6),Left(8)
	     Back(8),Bottom(8),Right(8)

      These values index to an array which gives the values for the front[], left[]... arrays.

     The array is generated in init_data and is called 
     struct Corner cornermap[6][6][6]. A warning is printed if values are illegal
     (e.g.,  all sides of a corner having the same color.)

     cornermap values

     indices                      cornermap[][][].
     []      []      []          a                b                c
     0       0       0           -1(illegal)
     0       0       1           -1
             .                    .
             .                    .
             .                    .
     FRONT   TOP     LEFT        seed_front[0]    seed_top[0]      seed_left[0]
     TOP     FRONT   LEFT        -1
     FRONT   LEFT    TOP         -1
     TOP     LEFT    FRONT       seed_top[0]      seed_left[0]     seed_front[0]
     LEFT    TOP     FRONT       -1
     LEFT    FRONT   TOP         seed_left[0]     seed_front[0]    seed_top[0]
             .                    .
             .                    .
             .                    .
     BACK    BOTTOM  RIGHT       seed_back[8]     seed_bottom[8]   seed_right[8]
             .                    .  
             .                    .  
             .                    .  

     So we can update front[], left[]... arrays in the following manner:

          front[0] = cornermap[Front(0)][Top(0)][Left(0)].a
	  front[2] = cornermap[Front(2)][Top(2)][Left(0)].a
	           .
	           .
	           .
	  top[0]   = cornermap[Front(0)][Top(0)][Left(0)].b

     A test is done for cornermap[][][].a == -1 which indicates an illegal
     initialization.

   */  
     
  /* There are 12 edge cubes that must have 2
     different color faces.
     Each edge cube is represented by 2 wdata->subface_side[][]
     values.
      Using the macros we have:
             Front(1),Top(1)
	     Front(3),Left(3)
	     Front(5),Right(3)
	     Front(7),Bottom(1)
	     Back(1),Top(7)
	     Back(3),Left(5)
	     Back(5),Right(5)
	     Back(7),Bottom(7)
	     Right(1),Top(5)
	     Right(7),Bottom(5)
	     Bottom(3),Left(7)
	     Left(1),Top(3)
	     

      These values index to an array which gives the values for the front[], left[]... arrays.

     The array is generated in init_data and is called 
     struct Edge edgemap[6][6]. A warning is printed if values are illegal
     (e.g.,  both sides of an edge cube having the same color.)

     edgemap values

     indices                  edgemap[][].
     []      []               a                b
     0       0                -1(illegal)
     0       0                -1
             .                    .
             .                    .
             .                    .
     FRONT   TOP              seed_front[1]    seed_top[1]     
     TOP     FRONT            seed_top[1]      seed_front[1]

             .                    .
     BACK    BOTTOM           seed_back[7]     seed_bottom[7]  
             .                    .  
             .                    .  
             .                    .  

     So we can update front[], left[]... arrays in the following manner:

          front[1] = edgemap[Front(1)][Top(1)].a
	  front[5] = edgemap[Front(5)][Right(3)].a
	           .
	           .
	           .
	  top[1]   = edgemap[Front(1)][Top(1)].b

     A test is done for edgemap[][].a == -1 which indicates an illegal
     initialization.



     This takes care of the front[], left[]... arrays. 
     Next, we have to fix the 
          double_XPoint cubik_face[NUM_SUBPLANES][5] array which has the old 
     pristine coordinates. The coordinates of the subplanes that have moved
     by initialization must also move. 
        Using inverse_funct which is the inverse of the front[], left[]... arrays,
     the new coordinates in cubik_face[][] are determinesd by the following.
        First make a copy of cubik_face[][].
                     facecpy(cubikface_cpy,cubik_face);
	Proceed to replace old coordinates.
	             cubik_face[0][] = cubikface_cpy[inverse_funct[0]][];
	             cubik_face[1][] = cubikface_cpy[inverse_funct[1]][];
	             cubik_face[2][] = cubikface_cpy[inverse_funct[2]][];
		                     .
		                     .
		                     .
	             cubik_face[53][] = cubikface_cpy[inverse_funct[53][];

	Inverse_funct is created as follows:
	             First make one large array:
		         large_array[0] = front[0]
		         large_array[1] = front[1]
		         large_array[2] = front[2]
		                     .
		                     .
		                     .
		         large_array[9] = front[8]
		         large_array[10] = left[0]
		         large_array[11] = left[1]
		         large_array[12] = left[2]
				     .
				     .
				     .
			 large_array[53] = bottom[8]

                     Then load inverse_funct with inverse values.
		         for(i=0;i<NUM_SUBPLANES-1;i++)
			      inverse_funct[large_array[i]] = i; 

     Lastly, the subface_gc[][]'s get re-initied.
     
   */  


  int i,j;
  XGCValues       values;
  double_XPoint cubikface_cpy[NUM_SUBPLANES][5];

  int inverse_funct[NUM_SUBPLANES];
  int large_array[NUM_SUBPLANES];

  /* corners */
  
  front[0] = cornermap[Front(0)][Top(0)][Left(0)].a;
  top[0] =   cornermap[Front(0)][Top(0)][Left(0)].b;
  left[0] =  cornermap[Front(0)][Top(0)][Left(0)].c;
  
  front[2] = cornermap[Front(2)][Top(2)][Right(0)].a;
  top[2] =   cornermap[Front(2)][Top(2)][Right(0)].b;
  right[0] = cornermap[Front(2)][Top(2)][Right(0)].c;
  
  
  front[6] = cornermap[Front(6)][Bottom(0)][Left(6)].a;
  bottom[0]= cornermap[Front(6)][Bottom(0)][Left(6)].b;
  left[6] =  cornermap[Front(6)][Bottom(0)][Left(6)].c;
  
  
  front[8] = cornermap[Front(8)][Bottom(2)][Right(6)].a;
  bottom[2]= cornermap[Front(8)][Bottom(2)][Right(6)].b;
  right[6] = cornermap[Front(8)][Bottom(2)][Right(6)].c;
  
  
  back[0] =  cornermap[Back(0)][Top(6)][Left(2)].a;
  top[6] =   cornermap[Back(0)][Top(6)][Left(2)].b;
  left[2] =  cornermap[Back(0)][Top(6)][Left(2)].c;
  
  
  back[2] =  cornermap[Back(2)][Top(8)][Right(2)].a;
  top[8] =   cornermap[Back(2)][Top(8)][Right(2)].b;
  right[2] = cornermap[Back(2)][Top(8)][Right(2)].c;
  
  
  back[6] =  cornermap[Back(6)][Bottom(6)][Left(8)].a;
  bottom[6]= cornermap[Back(6)][Bottom(6)][Left(8)].b;
  left[8] =  cornermap[Back(6)][Bottom(6)][Left(8)].c;
  
  
  back[8] =  cornermap[Back(8)][Bottom(8)][Right(8)].a;
  bottom[8]= cornermap[Back(8)][Bottom(8)][Right(8)].b;
  right[8] = cornermap[Back(8)][Bottom(8)][Right(8)].c;


  /* edges */

  front[1] = edgemap[Front(1)][Top(1)].a;
  top[1] =   edgemap[Front(1)][Top(1)].b;
  
  front[3] = edgemap[Front(3)][Left(3)].a;
  left[3] =  edgemap[Front(3)][Left(3)].b;
  
  front[5] = edgemap[Front(5)][Right(3)].a;
  right[3] = edgemap[Front(5)][Right(3)].b;
  
  front[7] = edgemap[Front(7)][Bottom(1)].a;
  bottom[1]= edgemap[Front(7)][Bottom(1)].b;
  
  back[1] =  edgemap[Back(1)][Top(7)].a;
  top[7] =   edgemap[Back(1)][Top(7)].b;
  
  back[3] =  edgemap[Back(3)][Left(5)].a;
  left[5] =  edgemap[Back(3)][Left(5)].b;
  
  back[5] =  edgemap[Back(5)][Right(5)].a;
  right[5] = edgemap[Back(5)][Right(5)].b;
  
  back[7] =  edgemap[Back(7)][Bottom(7)].a;
  bottom[7]= edgemap[Back(7)][Bottom(7)].b;
  
  right[1] = edgemap[Right(1)][Top(5)].a;
  top[5] =   edgemap[Right(1)][Top(5)].b;
  
  
  right[7] = edgemap[Right(7)][Bottom(5)].a;
  bottom[5]= edgemap[Right(7)][Bottom(5)].b;
  
  bottom[3]= edgemap[Bottom(3)][Left(7)].a;
  left[7] =  edgemap[Bottom(3)][Left(7)].b;
  
  left[1] =  edgemap[Left(1)][Top(3)].a;
  top[3] =   edgemap[Left(1)][Top(3)].b;

  /* check for bad alignment */
  for(i=0;i<=8;i++)
    if(front[i]  == -1 ||
       left[i]   == -1 ||
       right[i]  == -1 ||
       back[i]   == -1 ||
       top[i]    == -1 ||
       bottom[i] == -1){
      printf("ERROR BAD INITIALED CUBE\nPREPARE FOR STRANGE CUBE\n");
      return;
    }
  update_cubik(wdata);

  /* Create large_array fron front[],left[],... array */
  for(i=0;i<9;i++){
    large_array[i] = front[i];
    large_array[i+9] = left[i];
    large_array[i+18] = right[i];
    large_array[i+27] = back[i];
    large_array[i+36] = top[i];
    large_array[i+45] = bottom[i];
  }
  
  /* Create the inverse of large array */
  /* (This step could be incorporated into the next step but is kept
   *  here for clarity.)
   */
  for(i=0;i<=NUM_SUBPLANES-1;i++)
    inverse_funct[large_array[i]] = i;
  
  /* save old cubik_face values  */
  facecpy(cubikface_cpy,cubik_face);
  
  /* swap arround the values */
  for(i=0;i<=NUM_SUBPLANES-1;i++)
    for(j=0;j<5;j++){
      cubik_face[i][j].x = cubikface_cpy[inverse_funct[i]][j].x;
      cubik_face[i][j].y = cubikface_cpy[inverse_funct[i]][j].y;
      cubik_face[i][j].z = cubikface_cpy[inverse_funct[i]][j].z;
    }

  /* update rot_face  */
  facecpy(rot_face,cubik_face);

  /* re-init subface_gc's */
  /* 
   * Get a gc for each subface_color
   */
  for(i=0;i<9;i++){
    for(j=0;j<6;j++){
      values.foreground = xss_get_pixel_by_name(w,seed_color[j]);
      values.background = xss_get_pixel_by_name(w,seed_color[j]);
      values.fill_style = FillTiled;
      wdata->subface_gc[i][j] =	XtGetGC(w,
					GCForeground | GCBackground | GCFillStyle,
					&values);
    }
  }
}


void init_cornermap()
{
  int  i,j,k;
  
  /* See align_subfaces in cube.c for more info on cornermap[][][] */
  /* init all to illegal value */
  for(i=0;i<6;i++)
    for(j=0;j<6;j++)
      for(k=0;k<6;k++)
	cornermap[i][j][k].a = -1;
	
  /* brute force initialization ... ugh! */
  /* 1 */
  cornermap[FRONT][TOP][LEFT].a = seed_front[0];
  cornermap[FRONT][TOP][LEFT].b = seed_top[0];
  cornermap[FRONT][TOP][LEFT].c = seed_left[0];

  cornermap[TOP][LEFT][FRONT].a = seed_top[0];
  cornermap[TOP][LEFT][FRONT].b = seed_left[0];
  cornermap[TOP][LEFT][FRONT].c = seed_front[0];

  cornermap[LEFT][FRONT][TOP].a = seed_left[0];
  cornermap[LEFT][FRONT][TOP].b = seed_front[0];
  cornermap[LEFT][FRONT][TOP].c = seed_top[0];


  cornermap[FRONT][LEFT][TOP].a = seed_front[0];
  cornermap[FRONT][LEFT][TOP].c = seed_top[0];
  cornermap[FRONT][LEFT][TOP].b = seed_left[0];

  cornermap[TOP][FRONT][LEFT].a = seed_top[0];
  cornermap[TOP][FRONT][LEFT].c = seed_left[0];
  cornermap[TOP][FRONT][LEFT].b = seed_front[0];

  cornermap[LEFT][TOP][FRONT].a = seed_left[0];
  cornermap[LEFT][TOP][FRONT].c = seed_front[0];
  cornermap[LEFT][TOP][FRONT].b = seed_top[0];

  /* 2 */
  cornermap[FRONT][TOP][RIGHT].a = seed_front[2];
  cornermap[FRONT][TOP][RIGHT].b = seed_top[2];
  cornermap[FRONT][TOP][RIGHT].c = seed_right[0];

  cornermap[TOP][RIGHT][FRONT].a = seed_top[2];
  cornermap[TOP][RIGHT][FRONT].b = seed_right[0];
  cornermap[TOP][RIGHT][FRONT].c = seed_front[2];

  cornermap[RIGHT][FRONT][TOP].a = seed_right[0];
  cornermap[RIGHT][FRONT][TOP].b = seed_front[2];
  cornermap[RIGHT][FRONT][TOP].c = seed_top[2];


  cornermap[FRONT][RIGHT][TOP].a = seed_front[2];
  cornermap[FRONT][RIGHT][TOP].c = seed_top[2];
  cornermap[FRONT][RIGHT][TOP].b = seed_right[0];

  cornermap[TOP][FRONT][RIGHT].a = seed_top[2];
  cornermap[TOP][FRONT][RIGHT].c = seed_right[0];
  cornermap[TOP][FRONT][RIGHT].b = seed_front[2];

  cornermap[RIGHT][TOP][FRONT].a = seed_right[0];
  cornermap[RIGHT][TOP][FRONT].c = seed_front[2];
  cornermap[RIGHT][TOP][FRONT].b = seed_top[2];

  /* 3 */
  cornermap[FRONT][BOTTOM][LEFT].a = seed_front[6];
  cornermap[FRONT][BOTTOM][LEFT].b = seed_bottom[0];
  cornermap[FRONT][BOTTOM][LEFT].c = seed_left[6];

  cornermap[BOTTOM][LEFT][FRONT].a = seed_bottom[0];
  cornermap[BOTTOM][LEFT][FRONT].b = seed_left[6];
  cornermap[BOTTOM][LEFT][FRONT].c = seed_front[6];

  cornermap[LEFT][FRONT][BOTTOM].a = seed_left[6];
  cornermap[LEFT][FRONT][BOTTOM].b = seed_front[6];
  cornermap[LEFT][FRONT][BOTTOM].c = seed_bottom[0];


  cornermap[FRONT][LEFT][BOTTOM].a = seed_front[6];
  cornermap[FRONT][LEFT][BOTTOM].c = seed_bottom[0];
  cornermap[FRONT][LEFT][BOTTOM].b = seed_left[6];

  cornermap[BOTTOM][FRONT][LEFT].a = seed_bottom[0];
  cornermap[BOTTOM][FRONT][LEFT].c = seed_left[6];
  cornermap[BOTTOM][FRONT][LEFT].b = seed_front[6];

  cornermap[LEFT][BOTTOM][FRONT].a = seed_left[6];
  cornermap[LEFT][BOTTOM][FRONT].c = seed_front[6];
  cornermap[LEFT][BOTTOM][FRONT].b = seed_bottom[0];

  /* 4 */
  cornermap[FRONT][BOTTOM][RIGHT].a = seed_front[8];
  cornermap[FRONT][BOTTOM][RIGHT].b = seed_bottom[2];
  cornermap[FRONT][BOTTOM][RIGHT].c = seed_right[6];

  cornermap[BOTTOM][RIGHT][FRONT].a = seed_bottom[2];
  cornermap[BOTTOM][RIGHT][FRONT].b = seed_right[6];
  cornermap[BOTTOM][RIGHT][FRONT].c = seed_front[8];

  cornermap[RIGHT][FRONT][BOTTOM].a = seed_right[6];
  cornermap[RIGHT][FRONT][BOTTOM].b = seed_front[8];
  cornermap[RIGHT][FRONT][BOTTOM].c = seed_bottom[2];


  cornermap[FRONT][RIGHT][BOTTOM].a = seed_front[8];
  cornermap[FRONT][RIGHT][BOTTOM].c = seed_bottom[2];
  cornermap[FRONT][RIGHT][BOTTOM].b = seed_right[6];

  cornermap[BOTTOM][FRONT][RIGHT].a = seed_bottom[2];
  cornermap[BOTTOM][FRONT][RIGHT].c = seed_right[6];
  cornermap[BOTTOM][FRONT][RIGHT].b = seed_front[8];

  cornermap[RIGHT][BOTTOM][FRONT].a = seed_right[6];
  cornermap[RIGHT][BOTTOM][FRONT].c = seed_front[8];
  cornermap[RIGHT][BOTTOM][FRONT].b = seed_bottom[2];


  /* 5 */
  cornermap[BACK][TOP][LEFT].a = seed_back[0];
  cornermap[BACK][TOP][LEFT].b = seed_top[6];
  cornermap[BACK][TOP][LEFT].c = seed_left[2];

  cornermap[TOP][LEFT][BACK].a = seed_top[6];
  cornermap[TOP][LEFT][BACK].b = seed_left[2];
  cornermap[TOP][LEFT][BACK].c = seed_back[0];

  cornermap[LEFT][BACK][TOP].a = seed_left[2];
  cornermap[LEFT][BACK][TOP].b = seed_back[0];
  cornermap[LEFT][BACK][TOP].c = seed_top[6];


  cornermap[BACK][LEFT][TOP].a = seed_back[0];
  cornermap[BACK][LEFT][TOP].c = seed_top[6];
  cornermap[BACK][LEFT][TOP].b = seed_left[2];

  cornermap[TOP][BACK][LEFT].a = seed_top[6];
  cornermap[TOP][BACK][LEFT].c = seed_left[2];
  cornermap[TOP][BACK][LEFT].b = seed_back[0];

  cornermap[LEFT][TOP][BACK].a = seed_left[2];
  cornermap[LEFT][TOP][BACK].c = seed_back[0];
  cornermap[LEFT][TOP][BACK].b = seed_top[6];

  /* 6 */
  cornermap[BACK][TOP][RIGHT].a = seed_back[2];
  cornermap[BACK][TOP][RIGHT].b = seed_top[8];
  cornermap[BACK][TOP][RIGHT].c = seed_right[2];

  cornermap[TOP][RIGHT][BACK].a = seed_top[8];
  cornermap[TOP][RIGHT][BACK].b = seed_right[2];
  cornermap[TOP][RIGHT][BACK].c = seed_back[2];

  cornermap[RIGHT][BACK][TOP].a = seed_right[2];
  cornermap[RIGHT][BACK][TOP].b = seed_back[2];
  cornermap[RIGHT][BACK][TOP].c = seed_top[8];


  cornermap[BACK][RIGHT][TOP].a = seed_back[2];
  cornermap[BACK][RIGHT][TOP].c = seed_top[8];
  cornermap[BACK][RIGHT][TOP].b = seed_right[2];

  cornermap[TOP][BACK][RIGHT].a = seed_top[8];
  cornermap[TOP][BACK][RIGHT].c = seed_right[2];
  cornermap[TOP][BACK][RIGHT].b = seed_back[2];

  cornermap[RIGHT][TOP][BACK].a = seed_right[2];
  cornermap[RIGHT][TOP][BACK].c = seed_back[2];
  cornermap[RIGHT][TOP][BACK].b = seed_top[8];

  /* 7 */
  cornermap[BACK][BOTTOM][LEFT].a = seed_back[6];
  cornermap[BACK][BOTTOM][LEFT].b = seed_bottom[6];
  cornermap[BACK][BOTTOM][LEFT].c = seed_left[8];

  cornermap[BOTTOM][LEFT][BACK].a = seed_bottom[6];
  cornermap[BOTTOM][LEFT][BACK].b = seed_left[8];
  cornermap[BOTTOM][LEFT][BACK].c = seed_back[6];

  cornermap[LEFT][BACK][BOTTOM].a = seed_left[8];
  cornermap[LEFT][BACK][BOTTOM].b = seed_back[6];
  cornermap[LEFT][BACK][BOTTOM].c = seed_bottom[6];


  cornermap[BACK][LEFT][BOTTOM].a = seed_back[6];
  cornermap[BACK][LEFT][BOTTOM].c = seed_bottom[6];
  cornermap[BACK][LEFT][BOTTOM].b = seed_left[8];

  cornermap[BOTTOM][BACK][LEFT].a = seed_bottom[6];
  cornermap[BOTTOM][BACK][LEFT].c = seed_left[8];
  cornermap[BOTTOM][BACK][LEFT].b = seed_back[6];

  cornermap[LEFT][BOTTOM][BACK].a = seed_left[8];
  cornermap[LEFT][BOTTOM][BACK].c = seed_back[6];
  cornermap[LEFT][BOTTOM][BACK].b = seed_bottom[6];

  /* 8 */
  cornermap[BACK][BOTTOM][RIGHT].a = seed_back[8];
  cornermap[BACK][BOTTOM][RIGHT].b = seed_bottom[8];
  cornermap[BACK][BOTTOM][RIGHT].c = seed_right[8];

  cornermap[BOTTOM][RIGHT][BACK].a = seed_bottom[8];
  cornermap[BOTTOM][RIGHT][BACK].b = seed_right[8];
  cornermap[BOTTOM][RIGHT][BACK].c = seed_back[8];

  cornermap[RIGHT][BACK][BOTTOM].a = seed_right[8];
  cornermap[RIGHT][BACK][BOTTOM].b = seed_back[8];
  cornermap[RIGHT][BACK][BOTTOM].c = seed_bottom[8];


  cornermap[BACK][RIGHT][BOTTOM].a = seed_back[8];
  cornermap[BACK][RIGHT][BOTTOM].c = seed_bottom[8];
  cornermap[BACK][RIGHT][BOTTOM].b = seed_right[8];

  cornermap[BOTTOM][BACK][RIGHT].a = seed_bottom[8];
  cornermap[BOTTOM][BACK][RIGHT].c = seed_right[8];
  cornermap[BOTTOM][BACK][RIGHT].b = seed_back[8];

  cornermap[RIGHT][BOTTOM][BACK].a = seed_right[8];
  cornermap[RIGHT][BOTTOM][BACK].c = seed_back[8];
  cornermap[RIGHT][BOTTOM][BACK].b = seed_bottom[8];

}

void init_edgemap()
{
  int  i,j;
  
  /* See align_subfaces in cube.c for more info on edgemap[][] */
  /* init all to illegal value */
  for(i=0;i<6;i++)
    for(j=0;j<6;j++)
      edgemap[i][j].a = -1;
	
  /* brute force initialization ... ugh! */
  /* 1 */
  edgemap[FRONT][TOP].a = seed_front[1];
  edgemap[FRONT][TOP].b = seed_top[1];

  edgemap[TOP][FRONT].a = seed_top[1];
  edgemap[TOP][FRONT].b = seed_front[1];

  /* 2 */
  edgemap[FRONT][LEFT].a = seed_front[3];
  edgemap[FRONT][LEFT].b = seed_left[3];

  edgemap[LEFT][FRONT].a = seed_left[3];
  edgemap[LEFT][FRONT].b = seed_front[3];

  /* 3 */
  edgemap[FRONT][RIGHT].a = seed_front[5];
  edgemap[FRONT][RIGHT].b = seed_right[3];

  edgemap[RIGHT][FRONT].a = seed_right[3];
  edgemap[RIGHT][FRONT].b = seed_front[5];

  /* 4 */
  edgemap[FRONT][BOTTOM].a = seed_front[7];
  edgemap[FRONT][BOTTOM].b = seed_bottom[1];

  edgemap[BOTTOM][FRONT].a = seed_bottom[1];
  edgemap[BOTTOM][FRONT].b = seed_front[7];

  /* 5 */
  edgemap[BACK][TOP].a = seed_back[1];
  edgemap[BACK][TOP].b = seed_top[7];

  edgemap[TOP][BACK].a = seed_top[7];
  edgemap[TOP][BACK].b = seed_back[1];

  /* 6 */
  edgemap[BACK][LEFT].a = seed_back[3];
  edgemap[BACK][LEFT].b = seed_left[5];

  edgemap[LEFT][BACK].a = seed_left[5];
  edgemap[LEFT][BACK].b = seed_back[3];

  /* 7 */
  edgemap[BACK][RIGHT].a = seed_back[5];
  edgemap[BACK][RIGHT].b = seed_right[5];

  edgemap[RIGHT][BACK].a = seed_right[5];
  edgemap[RIGHT][BACK].b = seed_back[5];

  /* 8 */
  edgemap[BACK][BOTTOM].a = seed_back[7];
  edgemap[BACK][BOTTOM].b = seed_bottom[7];

  edgemap[BOTTOM][BACK].a = seed_bottom[7];
  edgemap[BOTTOM][BACK].b = seed_back[7];

  /* 9 */
  edgemap[RIGHT][TOP].a = seed_right[1];
  edgemap[RIGHT][TOP].b = seed_top[5];

  edgemap[TOP][RIGHT].a = seed_top[5];
  edgemap[TOP][RIGHT].b = seed_right[1];

  /* 10 */
  edgemap[RIGHT][BOTTOM].a = seed_right[7];
  edgemap[RIGHT][BOTTOM].b = seed_bottom[5];

  edgemap[BOTTOM][RIGHT].a = seed_bottom[5];
  edgemap[BOTTOM][RIGHT].b = seed_right[7];

  /* 11 */
  edgemap[LEFT][BOTTOM].a = seed_left[7];
  edgemap[LEFT][BOTTOM].b = seed_bottom[3];

  edgemap[BOTTOM][LEFT].a = seed_bottom[3];
  edgemap[BOTTOM][LEFT].b = seed_left[7];

  /* 12 */
  edgemap[LEFT][TOP].a = seed_left[1];
  edgemap[LEFT][TOP].b = seed_top[3];
  
  edgemap[TOP][LEFT].a = seed_top[3];
  edgemap[TOP][LEFT].b = seed_left[1];
  
}
/* check_events:
 * This checks for Expose events and processes them.
 * Check_events is used in draw_it. 
 * When cube is being solved, expose events are checked for
 * and processed so that the buttons can be visible after
 * the the buttons have been 'exposed'. This could happen 
 * when windows get circulated through a stack during
 * the time the cube is solved.
 * 
 * test_for_event is a predicate procedure used by XCheckIfEvent.
 */

Bool test_for_event(display,event,event_type)
     Display *display;
     XEvent *event;
     int    event_type;
{
  if(event->type == event_type)
    return(True);
  else
    return(False);
}

void check_events()
{
  if(XtAppPending(appContext) & XtIMXEvent){
    XEvent event;
    if(XCheckIfEvent(xDisplay,&event,test_for_event,Expose))
      XtDispatchEvent(&event);
  }
}

#ifdef SVR4
/* Some X libraries contain references to undefined routines regex and regcmp.
 * Hope they don't get used, just put these in to keep the linker happy.
 * Greg Lehey, LEMIS, 6 February 1994 */
int regex ()
{
  printf ("Something called regex (). Goodbye\n");
  exit (1);
  }

int regcmp ()
{
  printf ("Something called regex (). Goodbye\n");
  exit (1);
  }
#endif
