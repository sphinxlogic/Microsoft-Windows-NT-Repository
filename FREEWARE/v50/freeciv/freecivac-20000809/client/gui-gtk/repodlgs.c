/********************************************************************** 
 Freeciv - Copyright (C) 1996 - A Kjeldberg, L Gregersen, P Unold
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(__VMS)
#  include <memory.h>
#  include <string.h>
#endif

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "fcintl.h"
#include "game.h"
#include "government.h"
#include "packets.h"
#include "shared.h"
#include "support.h"

#include "cityrep.h"
#include "civclient.h"
#include "dialogs.h"
#include "gui_stuff.h"
#include "helpdlg.h"
#include "optiondlg.h"
#include "log.h"

#include "repodlgs.h"

extern GtkWidget *toplevel;
extern GdkWindow *root_window;

extern struct connection aconnection;

/******************************************************************/


void create_science_dialog(int make_modal);
void science_close_callback(GtkWidget *widget, gpointer data);
void science_help_callback(GtkWidget *w, gint row, gint column);
void science_change_callback(GtkWidget *widget, gpointer data);
void science_goal_callback(GtkWidget *widget, gpointer data);


/******************************************************************/
GtkWidget *science_dialog_shell=NULL;
GtkWidget *science_label;
GtkWidget *science_current_label, *science_goal_label;
GtkWidget *science_change_menu_button, *science_goal_menu_button;
GtkWidget *science_list[4], *science_help_toggle;
int science_dialog_shell_is_modal;
int science_dialog_popupmenu;
GtkWidget *popupmenu, *goalmenu;


/******************************************************************/
void create_trade_report_dialog(int make_modal);
void trade_close_callback(GtkWidget *widget, gpointer data);
void trade_selloff_callback(GtkWidget *widget, gpointer data);
void trade_list_callback(GtkWidget *w, gint row, gint column);
void trade_list_ucallback(GtkWidget *w, gint row, gint column);
int trade_improvement_type[100];	/* FIXME: Make me dynamic */

GtkWidget *trade_dialog_shell=NULL;
GtkWidget *trade_label, *trade_label2;
GtkWidget *trade_list, *trade_list_label;
GtkWidget *sellall_command, *sellobsolete_command;
int trade_dialog_shell_is_modal;

/******************************************************************/
void create_activeunits_report_dialog(int make_modal);
void activeunits_close_callback(GtkWidget *widget, gpointer data);
void activeunits_upgrade_callback(GtkWidget *widget, gpointer data);
void activeunits_refresh_callback(GtkWidget *widget, gpointer data);
void activeunits_list_callback(GtkWidget *w, gint row, gint column);
void activeunits_list_ucallback(GtkWidget *w, gint row, gint column);
int activeunits_type[U_LAST];

GtkWidget *activeunits_dialog_shell=NULL;
GtkWidget *activeunits_label, *activeunits_label2;
GtkWidget *activeunits_list, *activeunits_list_label;
GtkWidget *upgrade_command;

int activeunits_dialog_shell_is_modal;
/******************************************************************/

int delay_report_update=0;

/******************************************************************
 Turn off updating of reports
*******************************************************************/
void report_update_delay_on(void)
{
  delay_report_update=1;
}

/******************************************************************
 Turn on updating of reports
*******************************************************************/
void report_update_delay_off(void)
{
  delay_report_update=0;
}

/******************************************************************
...
*******************************************************************/
void update_report_dialogs(void)
{
  if(delay_report_update) return;
  activeunits_report_dialog_update();
  trade_report_dialog_update();
  city_report_dialog_update(); 
  science_dialog_update();
}

/****************************************************************
...
****************************************************************/
char *get_report_title(char *report_name)
{
  static char buf[512];

  my_snprintf(buf, sizeof(buf), _("%s\n%s of the %s\n%s %s: %s"),
	  report_name,
	  get_government_name(game.player_ptr->government),
	  get_nation_name_plural(game.player_ptr->nation),
	  get_ruler_title(game.player_ptr->government,
			  game.player_ptr->is_male, game.player_ptr->nation),
	  game.player_ptr->name,
	  textyear(game.year));

  return buf;
}


/****************************************************************
...
*****************************************************************/
void popup_science_dialog(int make_modal)
{
  if(!science_dialog_shell) {
    science_dialog_shell_is_modal=make_modal;
    
    if(make_modal)
      gtk_widget_set_sensitive(toplevel, FALSE);
    
    create_science_dialog(make_modal);
    gtk_set_relative_position(toplevel, science_dialog_shell, 10, 10);

    gtk_widget_show( science_dialog_shell );
  }
}


/****************************************************************
...
*****************************************************************/
void create_science_dialog(int make_modal)
{
  GtkWidget *close_command;
  char *report_title;
  GtkWidget *frame, *hbox, *w;
  GtkAccelGroup *accel=gtk_accel_group_new();
  int i;
  char text[512], rate[128];

  science_dialog_shell = gtk_dialog_new();
  gtk_signal_connect( GTK_OBJECT(science_dialog_shell),"delete_event",
        GTK_SIGNAL_FUNC(science_close_callback),NULL );
  gtk_accel_group_attach(accel, GTK_OBJECT(science_dialog_shell));

  gtk_window_set_title (GTK_WINDOW(science_dialog_shell), _("Science Report"));

  report_title=get_report_title(_("Science Advisor"));
  sz_strlcpy(text, report_title);
  my_snprintf(rate, sizeof(rate), _("\n(%d turns/advance)"),
	      tech_turns_to_advance(game.player_ptr));
  sz_strlcat(text, rate);
  science_label = gtk_label_new(text);

  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(science_dialog_shell)->vbox ),
        science_label, FALSE, FALSE, 0 );

  frame = gtk_frame_new(_("Researching"));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(science_dialog_shell)->vbox ),
        frame, FALSE, FALSE, 0 );

  hbox = gtk_hbox_new( TRUE, 5 );
  gtk_container_add(GTK_CONTAINER(frame), hbox);

  science_change_menu_button = gtk_option_menu_new();
  gtk_box_pack_start( GTK_BOX( hbox ), science_change_menu_button,TRUE, TRUE, 0 );

  popupmenu = gtk_menu_new();
  gtk_widget_show_all(popupmenu);

  science_current_label = gtk_label_new("");
  gtk_box_pack_start( GTK_BOX( hbox ), science_current_label,TRUE, FALSE, 0 );
  gtk_widget_set_usize(science_current_label, 0,25);

  science_help_toggle = gtk_check_button_new_with_label (_("Help"));
  gtk_box_pack_start( GTK_BOX( hbox ), science_help_toggle, TRUE, FALSE, 0 );

  frame = gtk_frame_new( _("Goal"));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(science_dialog_shell)->vbox ),
        frame, FALSE, FALSE, 0 );

  hbox = gtk_hbox_new( TRUE, 5 );
  gtk_container_add(GTK_CONTAINER(frame),hbox);

  science_goal_menu_button = gtk_option_menu_new();
  gtk_box_pack_start( GTK_BOX( hbox ), science_goal_menu_button,TRUE, TRUE, 0 );

  goalmenu = gtk_menu_new();
  gtk_widget_show_all(goalmenu);

  science_goal_label = gtk_label_new("");
  gtk_box_pack_start( GTK_BOX( hbox ), science_goal_label, TRUE, FALSE, 0 );
  gtk_widget_set_usize(science_goal_label, 0,25);

  w = gtk_label_new("");
  gtk_box_pack_start( GTK_BOX( hbox ), w,TRUE, FALSE, 0 );

  hbox = gtk_hbox_new(TRUE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(science_dialog_shell)->vbox),
		     hbox, TRUE, TRUE, 0);

  for (i=0; i<4; i++) {
    science_list[i] = gtk_clist_new(1);
    gtk_box_pack_start(GTK_BOX(hbox), science_list[i], TRUE, TRUE, 0);
    gtk_clist_set_column_auto_resize (GTK_CLIST (science_list[i]), 0, TRUE);
    gtk_signal_connect(GTK_OBJECT(science_list[i]), "select_row",
		       GTK_SIGNAL_FUNC(science_help_callback), NULL);
  }

  close_command = gtk_button_new_with_label(_("Close"));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(science_dialog_shell)->action_area ),
        close_command, TRUE, TRUE, 0 );
  GTK_WIDGET_SET_FLAGS( close_command, GTK_CAN_DEFAULT );
  gtk_widget_grab_default( close_command );

  gtk_signal_connect( GTK_OBJECT( close_command ), "clicked",
        GTK_SIGNAL_FUNC( science_close_callback ), NULL);

  gtk_widget_show_all( GTK_DIALOG(science_dialog_shell)->vbox );
  gtk_widget_show_all( GTK_DIALOG(science_dialog_shell)->action_area );

  gtk_widget_add_accelerator(close_command, "clicked",
	accel, GDK_Escape, 0, GTK_ACCEL_VISIBLE);

  science_dialog_update();
}

/****************************************************************
...
*****************************************************************/
void science_change_callback(GtkWidget *widget, gpointer data)
{
  char text[512];
  struct packet_player_request packet;
  size_t to;

  to=(size_t)data;

  if (GTK_TOGGLE_BUTTON(science_help_toggle)->active) {
    popup_help_dialog_typed(advances[to].name, HELP_TECH);
    /* Following is to make the menu go back to the current research;
     * there may be a better way to do this?  --dwp */
    science_dialog_update();
  } else {
    my_snprintf(text, sizeof(text), "%d/%d",
		game.player_ptr->research.researched,
		research_time(game.player_ptr));

    gtk_set_label(science_current_label,text);
    
    packet.tech=to;
    send_packet_player_request(&aconnection, &packet, PACKET_PLAYER_RESEARCH);
  }
}

/****************************************************************
...
*****************************************************************/
void science_goal_callback(GtkWidget *widget, gpointer data)
{
  char text[512];
  struct packet_player_request packet;
  size_t to;

  to=(size_t)data;

  if (GTK_TOGGLE_BUTTON(science_help_toggle)->active) {
    popup_help_dialog_typed(advances[to].name, HELP_TECH);
    /* Following is to make the menu go back to the current goal;
     * there may be a better way to do this?  --dwp */
    science_dialog_update();
  }
  else {  
    my_snprintf(text, sizeof(text), _("(%d steps)"),
	    tech_goal_turns(game.player_ptr, to));
    gtk_set_label(science_goal_label,text);

    packet.tech=to;
    send_packet_player_request(&aconnection, &packet, PACKET_PLAYER_TECH_GOAL);
  }
}

/****************************************************************
...
*****************************************************************/
void science_close_callback(GtkWidget *widget, gpointer data)
{

  if(science_dialog_shell_is_modal)
    gtk_widget_set_sensitive(toplevel, TRUE);
  gtk_widget_destroy(science_dialog_shell);
  science_dialog_shell=NULL;
}

/****************************************************************
...
*****************************************************************/
void science_help_callback(GtkWidget *w, gint row, gint column)
{
  gtk_clist_unselect_row(GTK_CLIST(w), row, column);

  if (GTK_TOGGLE_BUTTON(science_help_toggle)->active)
  {
    char *s;

    gtk_clist_get_text(GTK_CLIST(w), row, column, &s);
    if (*s != '\0')
      popup_help_dialog_typed(s, HELP_TECH);
    else
      popup_help_dialog_string(HELP_TECHS_ITEM);
  }
}

/****************************************************************
...
*****************************************************************/
static gint cmp_func(gconstpointer a_p, gconstpointer b_p)
{
  gchar *a_str, *b_str;
  gchar text_a[512], text_b[512];
  gint a=(gint)a_p, b=(gint)b_p;

  if(a < game.num_tech_types) {
    a_str=advances[a].name;
  } else {
    my_snprintf(text_a,sizeof(text_a), _("Researching Future Tech. %d"),a);
    a_str=text_a;
  }

  if(b < game.num_tech_types) {
    b_str=advances[b].name;
  } else {
    my_snprintf(text_b,sizeof(text_b), _("Researching Future Tech. %d"),b);
    b_str=text_b;
  }

  return strcmp(a_str,b_str);
}

/****************************************************************
...
*****************************************************************/
void science_dialog_update(void)
{
  if(science_dialog_shell) {
  char text[512], rate[128];
  int i, j, hist;
  char *report_title;
  static char *row	[1];
  GtkWidget *item;
  GList *sorting_list = NULL;

  if(delay_report_update) return;
  report_title=get_report_title(_("Science Advisor"));
  sz_strlcpy(text, report_title);
  my_snprintf(rate, sizeof(rate), _("\n(%d turns/advance)"),
	      tech_turns_to_advance(game.player_ptr));
  sz_strlcat(text, rate);
  gtk_set_label(science_label, text);

  for (i=0; i<4; i++) {
    gtk_clist_freeze(GTK_CLIST(science_list[i]));
    gtk_clist_clear(GTK_CLIST(science_list[i]));
  }

  /* collect all researched techs in sorting_list */
  for(i=A_FIRST; i<game.num_tech_types; i++) {
    if ((get_invention(game.player_ptr, i)==TECH_KNOWN)) {
      sorting_list = g_list_append(sorting_list,(gpointer)i);
    }
  }

  /* sort them, and install them in the list */
  sorting_list = g_list_sort(sorting_list, cmp_func);
  for(i=0; i<g_list_length(sorting_list); i++) {
    j = (gint)g_list_nth_data(sorting_list, i);
    row[0] = advances[j].name;
    gtk_clist_append(GTK_CLIST(science_list[i%4]), row);
  }
  g_list_free(sorting_list);
  sorting_list = NULL;

  for (i=0; i<4; i++) {
    gtk_clist_thaw(GTK_CLIST(science_list[i]));
  }

  gtk_widget_destroy(popupmenu);
  popupmenu = gtk_menu_new();

  my_snprintf(text, sizeof(text), "%d/%d",
	      game.player_ptr->research.researched,
	      research_time(game.player_ptr));

  gtk_set_label(science_current_label,text);

  /* collect all techs which are reachable in the next step
   * hist will hold afterwards the techid of the current choice
   */
  hist=0;
  if (game.player_ptr->research.researching!=A_NONE) {
    for(i=A_FIRST; i<game.num_tech_types; i++) {
      if(get_invention(game.player_ptr, i)!=TECH_REACHABLE)
	continue;

      if (i==game.player_ptr->research.researching)
	hist=i;
      sorting_list = g_list_append(sorting_list,(gpointer)i);
    }
  } else {
    sorting_list = g_list_append(sorting_list, 
				 (gpointer)(game.num_tech_types+
					    (game.player_ptr->future_tech)
					    +1));
  }

  /* sort the list and build from it the menu */
  sorting_list = g_list_sort(sorting_list, cmp_func);
  for (i = 0; i < g_list_length(sorting_list); i++) {
    gchar *data;

    if((gint)g_list_nth_data(sorting_list, i) < game.num_tech_types) {
      data=advances[(gint)g_list_nth_data(sorting_list, i)].name;
    } else {
      my_snprintf(text, sizeof(text), _("Researching Future Tech. %d"),
		  (gint)g_list_nth_data(sorting_list, i));
      data=text;
    }

    item = gtk_menu_item_new_with_label(data);
    gtk_menu_append(GTK_MENU(popupmenu), item);
    if (strlen(data) > 0)
      gtk_signal_connect(GTK_OBJECT(item), "activate",
			 GTK_SIGNAL_FUNC(science_change_callback),
			 (gpointer) g_list_nth_data(sorting_list, i));
  }

  gtk_widget_show_all(popupmenu);
  gtk_menu_set_active(GTK_MENU(popupmenu),
		      g_list_index(sorting_list,(gpointer)hist));
  g_list_free(sorting_list);
  sorting_list = NULL;

  gtk_option_menu_set_menu(GTK_OPTION_MENU(science_change_menu_button), 
			   popupmenu);

  gtk_widget_destroy(goalmenu);
  goalmenu = gtk_menu_new();

  my_snprintf(text, sizeof(text), _("(%d steps)"),
	      tech_goal_turns(game.player_ptr, game.player_ptr->ai.tech_goal));
  gtk_set_label(science_goal_label,text);

  if (game.player_ptr->ai.tech_goal==A_NONE) {
    item = gtk_menu_item_new_with_label(advances[A_NONE].name);
    gtk_menu_append(GTK_MENU(goalmenu), item);
  }

  /* collect all techs which are reachable in under 11 steps
   * hist will hold afterwards the techid of the current choice
   */
  hist=0;
  for(i=A_FIRST; i<game.num_tech_types; i++) {
    if(get_invention(game.player_ptr, i) != TECH_KNOWN &&
       advances[i].req[0] != A_LAST && advances[i].req[1] != A_LAST &&
       tech_goal_turns(game.player_ptr, i) < 11) {
      if (i==game.player_ptr->ai.tech_goal)
	hist=i;
      sorting_list = g_list_append(sorting_list,(gpointer)i);
    }
  }

  /* sort the list and build from it the menu */
  sorting_list = g_list_sort(sorting_list, cmp_func);
  for (i = 0; i < g_list_length(sorting_list); i++) {
    gchar *data=advances[(gint)g_list_nth_data(sorting_list, i)].name;

    item = gtk_menu_item_new_with_label(data);
    gtk_menu_append(GTK_MENU(goalmenu), item);
    gtk_signal_connect(GTK_OBJECT(item), "activate",
		       GTK_SIGNAL_FUNC(science_goal_callback),
		       (gpointer) g_list_nth_data(sorting_list, i));
  }

  gtk_widget_show_all(goalmenu);
  gtk_menu_set_active(GTK_MENU(goalmenu), 
		      g_list_index(sorting_list,(gpointer)hist));
  g_list_free(sorting_list);
  sorting_list = NULL;

  gtk_option_menu_set_menu(GTK_OPTION_MENU(science_goal_menu_button), 
			   goalmenu);
  }
}


/****************************************************************

                      TRADE REPORT DIALOG
 
****************************************************************/

/****************************************************************
...
****************************************************************/
void popup_trade_report_dialog(int make_modal)
{
  if(!trade_dialog_shell) {
      trade_dialog_shell_is_modal=make_modal;
    
      if(make_modal)
	gtk_widget_set_sensitive(toplevel, FALSE);
      
      create_trade_report_dialog(make_modal);
      gtk_set_relative_position(toplevel, trade_dialog_shell, 10, 10);

      gtk_widget_show(trade_dialog_shell);
   }
}


/****************************************************************
...
*****************************************************************/
void create_trade_report_dialog(int make_modal)
{
  GtkWidget *close_command, *scrolled;
  char *report_title;
  static gchar *titles_[4] = { N_("Building Name"), N_("Count"),
			      N_("Cost"), N_("U Total") };
  static gchar **titles;
  int    i;
  GtkAccelGroup *accel=gtk_accel_group_new();

  if (!titles) titles = intl_slist(4, titles_);
  
  trade_dialog_shell = gtk_dialog_new();
  gtk_signal_connect( GTK_OBJECT(trade_dialog_shell),"delete_event",
        GTK_SIGNAL_FUNC(trade_close_callback),NULL );
  gtk_accel_group_attach(accel, GTK_OBJECT(trade_dialog_shell));

  gtk_window_set_title(GTK_WINDOW(trade_dialog_shell),_("Trade Report"));

  report_title=get_report_title(_("Trade Advisor"));
  trade_label = gtk_label_new(report_title);
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(trade_dialog_shell)->vbox ),
        trade_label, FALSE, FALSE, 0 );

  trade_list = gtk_clist_new_with_titles( 4, titles );
  gtk_clist_column_titles_passive(GTK_CLIST(trade_list));
  scrolled = gtk_scrolled_window_new(NULL,NULL);
  gtk_clist_set_selection_mode(GTK_CLIST (trade_list), GTK_SELECTION_EXTENDED);
  gtk_container_add(GTK_CONTAINER(scrolled), trade_list);
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolled ),
  			  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
  gtk_widget_set_usize(trade_list, 300, 150);

  for ( i = 0; i < 4; i++ )
    gtk_clist_set_column_auto_resize(GTK_CLIST(trade_list),i,TRUE);

  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(trade_dialog_shell)->vbox ),
        scrolled, TRUE, TRUE, 0 );

  trade_label2 = gtk_label_new(_("Total Cost:"));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(trade_dialog_shell)->vbox ),
        trade_label2, FALSE, FALSE, 0 );

  close_command = gtk_button_new_with_label(_("Close"));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(trade_dialog_shell)->action_area ),
        close_command, TRUE, TRUE, 0 );
  GTK_WIDGET_SET_FLAGS( close_command, GTK_CAN_DEFAULT );
  gtk_widget_grab_default( close_command );

  sellobsolete_command = gtk_button_new_with_label(_("Sell Obsolete"));
  gtk_widget_set_sensitive(sellobsolete_command, FALSE);
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(trade_dialog_shell)->action_area ),
        sellobsolete_command, TRUE, TRUE, 0 );
  GTK_WIDGET_SET_FLAGS( sellobsolete_command, GTK_CAN_DEFAULT );

  sellall_command  = gtk_button_new_with_label(_("Sell All"));
  gtk_widget_set_sensitive(sellall_command, FALSE);
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(trade_dialog_shell)->action_area ),
        sellall_command, TRUE, TRUE, 0 );
  GTK_WIDGET_SET_FLAGS( sellall_command, GTK_CAN_DEFAULT );

  gtk_signal_connect(GTK_OBJECT(trade_list), "select_row",
	GTK_SIGNAL_FUNC(trade_list_callback), NULL);
  gtk_signal_connect(GTK_OBJECT(trade_list), "unselect_row",
	GTK_SIGNAL_FUNC(trade_list_ucallback), NULL);
  gtk_signal_connect(GTK_OBJECT(close_command), "clicked",
	GTK_SIGNAL_FUNC(trade_close_callback), NULL);
  gtk_signal_connect(GTK_OBJECT(sellobsolete_command), "clicked",
	GTK_SIGNAL_FUNC(trade_selloff_callback), (gpointer)0);
  gtk_signal_connect(GTK_OBJECT(sellall_command), "clicked",
	GTK_SIGNAL_FUNC(trade_selloff_callback), (gpointer)1);

  gtk_widget_show_all( GTK_DIALOG(trade_dialog_shell)->vbox );
  gtk_widget_show_all( GTK_DIALOG(trade_dialog_shell)->action_area );

  gtk_widget_add_accelerator(close_command, "clicked",
	accel, GDK_Escape, 0, GTK_ACCEL_VISIBLE);

  trade_report_dialog_update();
}



/****************************************************************
...
*****************************************************************/
void trade_list_callback(GtkWidget *w, gint row, gint column)
{
  int i;

  i=trade_improvement_type[row];
  if(i>=0 && i<game.num_improvement_types && !is_wonder(i))
    gtk_widget_set_sensitive(sellobsolete_command, TRUE);
    gtk_widget_set_sensitive(sellall_command, TRUE);
  return;
}

/****************************************************************
...
*****************************************************************/
void trade_list_ucallback(GtkWidget *w, gint row, gint column)
{
  gtk_widget_set_sensitive(sellobsolete_command, FALSE);
  gtk_widget_set_sensitive(sellall_command, FALSE);
}

/****************************************************************
...
*****************************************************************/
void trade_close_callback(GtkWidget *w, gpointer data)
{

  if(trade_dialog_shell_is_modal)
     gtk_widget_set_sensitive(toplevel, TRUE);
  gtk_widget_destroy(trade_dialog_shell);
  trade_dialog_shell=NULL;
}

/****************************************************************
...
*****************************************************************/
void trade_selloff_callback(GtkWidget *w, gpointer data)
{
  int i,count=0,gold=0;
  struct genlist_iterator myiter;
  struct city *pcity;
  struct packet_city_request packet;
  char str[64];
  GList              *selection;
  gint                row;

  while((selection = GTK_CLIST(trade_list)->selection) != NULL)
  {
  row = (gint)selection->data;

  i=trade_improvement_type[row];

  genlist_iterator_init(&myiter, &game.player_ptr->cities.list, 0);
  for(; ITERATOR_PTR(myiter);ITERATOR_NEXT(myiter)) {
    pcity=(struct city *)ITERATOR_PTR(myiter);
    if(!pcity->did_sell && city_got_building(pcity, i) && 
       (data ||
	improvement_obsolete(game.player_ptr,i) ||
        wonder_replacement(pcity, i) ))  {
	count++; gold+=improvement_value(i);
        packet.city_id=pcity->id;
        packet.build_id=i;
        packet.name[0]='\0';
	packet.worklist.name[0] = '\0';
        send_packet_city_request(&aconnection, &packet, PACKET_CITY_SELL);
    }
  }
  if(count)  {
    my_snprintf(str, sizeof(str), _("Sold %d %s for %d gold"),
		count, get_improvement_name(i), gold);
  } else {
    my_snprintf(str, sizeof(str), _("No %s could be sold"),
		get_improvement_name(i));
  }
  gtk_clist_unselect_row(GTK_CLIST(trade_list),row,0);
  popup_notify_dialog(_("Sell-Off:"),_("Results"),str);
  }
  return;
}

/****************************************************************
...
*****************************************************************/
void trade_report_dialog_update(void)
{
  if(delay_report_update) return;
  if(trade_dialog_shell) {
    int j, k, count, tax, cost, total;
    char  *report_title;
    char   buf0 [64];
    char   buf1 [64];
    char   buf2 [64];
    char   buf3 [64];
    gchar *row  [4];
    char trade_total[48];
    struct city *pcity;
    
    report_title=get_report_title(_("Trade Advisor"));
    gtk_set_label(trade_label, report_title);

    gtk_clist_freeze(GTK_CLIST(trade_list));
    gtk_clist_clear(GTK_CLIST(trade_list));

    total = 0;
    tax=0;
    k = 0;
    row[0] = buf0;
    row[1] = buf1;
    row[2] = buf2;
    row[3] = buf3;

    pcity = city_list_get(&game.player_ptr->cities,0);
    if(pcity)  {
      for (j=0;j<game.num_improvement_types;j++)
      if(!is_wonder(j)) {
       count = 0; 
       city_list_iterate(game.player_ptr->cities,pcity)
	 if (city_got_building(pcity, j)) count++;
       city_list_iterate_end;
       if (!count) continue;
	cost = count * improvement_upkeep(pcity, j);
	my_snprintf( buf0, sizeof(buf0), "%-20s", get_improvement_name(j) );
	my_snprintf( buf1, sizeof(buf1), "%5d", count );
	my_snprintf( buf2, sizeof(buf2), "%5d", improvement_upkeep(pcity, j) );
	my_snprintf( buf3, sizeof(buf3), "%6d", cost );

	gtk_clist_append( GTK_CLIST( trade_list ), row );

	total+=cost;
	trade_improvement_type[k]=j;
	k++;
      }
      city_list_iterate(game.player_ptr->cities,pcity) {
       tax+=pcity->tax_total;
       if (!pcity->is_building_unit && 
	   pcity->currently_building==B_CAPITAL)
	 tax+=pcity->shield_surplus;
      } city_list_iterate_end;
    }
    my_snprintf(trade_total, sizeof(trade_total),
		_("Income:%6d    Total Costs: %6d"), tax, total); 
    gtk_set_label(trade_label2, trade_total); 

    gtk_widget_show_all(trade_list);
    gtk_clist_thaw(GTK_CLIST(trade_list));
  }
  
}

/****************************************************************

                      ACTIVE UNITS REPORT DIALOG
 
****************************************************************/

#define AU_COL 6

/****************************************************************
...
****************************************************************/
void popup_activeunits_report_dialog(int make_modal)
{
  if(!activeunits_dialog_shell) {
      activeunits_dialog_shell_is_modal=make_modal;
    
      if(make_modal)
	gtk_widget_set_sensitive(toplevel, FALSE);
      
      create_activeunits_report_dialog(make_modal);
      gtk_set_relative_position(toplevel, activeunits_dialog_shell, 10, 10);

      gtk_widget_show(activeunits_dialog_shell);
   }
}


/****************************************************************
...
*****************************************************************/
void create_activeunits_report_dialog(int make_modal)
{
  GtkWidget *close_command, *refresh_command;
  char *report_title;
  static gchar *titles_[AU_COL]
    = { N_("Unit Type"), N_("U"), N_("In-Prog"), N_("Active"),
	N_("Shield"), N_("Food") };
  static gchar **titles;
  int    i;
  GtkAccelGroup *accel=gtk_accel_group_new();

  if (!titles) titles = intl_slist(AU_COL, titles_);

  activeunits_dialog_shell = gtk_dialog_new();
  gtk_signal_connect( GTK_OBJECT(activeunits_dialog_shell),"delete_event",
        GTK_SIGNAL_FUNC(activeunits_close_callback),NULL );
  gtk_accel_group_attach(accel, GTK_OBJECT(activeunits_dialog_shell));

  gtk_window_set_title(GTK_WINDOW(activeunits_dialog_shell),_("Military Report"));

  report_title=get_report_title(_("Military Report"));
  activeunits_label = gtk_label_new(report_title);
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(activeunits_dialog_shell)->vbox ),
        activeunits_label, FALSE, FALSE, 0 );

  activeunits_list = gtk_clist_new_with_titles( AU_COL, titles );
  gtk_clist_column_titles_passive(GTK_CLIST(activeunits_list));
  for ( i = 0; i < AU_COL; i++ ) {
    gtk_clist_set_column_auto_resize(GTK_CLIST(activeunits_list),i,TRUE);
    if (i > 1) {
      gtk_clist_set_column_justification (GTK_CLIST(activeunits_list),
					  i, GTK_JUSTIFY_RIGHT);
    }
  }

  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(activeunits_dialog_shell)->vbox ),
        activeunits_list, TRUE, TRUE, 0 );

  activeunits_label2 = gtk_label_new(_("Totals: ..."));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(activeunits_dialog_shell)->vbox ),
        activeunits_label2, FALSE, FALSE, 0 );

  close_command = gtk_button_new_with_label(_("Close"));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(activeunits_dialog_shell)->action_area ),
        close_command, TRUE, TRUE, 0 );
  GTK_WIDGET_SET_FLAGS( close_command, GTK_CAN_DEFAULT );
  gtk_widget_grab_default( close_command );

  upgrade_command = gtk_button_new_with_label(_("Upgrade"));
  gtk_widget_set_sensitive(upgrade_command, FALSE);
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(activeunits_dialog_shell)->action_area ),
        upgrade_command, TRUE, TRUE, 0 );
  GTK_WIDGET_SET_FLAGS( upgrade_command, GTK_CAN_DEFAULT );

  refresh_command = gtk_button_new_with_label(_("Refresh"));
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG(activeunits_dialog_shell)->action_area ),
        refresh_command, TRUE, TRUE, 0 );
  GTK_WIDGET_SET_FLAGS( refresh_command, GTK_CAN_DEFAULT );

  gtk_signal_connect(GTK_OBJECT(activeunits_list), "select_row",
	GTK_SIGNAL_FUNC(activeunits_list_callback), NULL);
  gtk_signal_connect(GTK_OBJECT(activeunits_list), "unselect_row",
	GTK_SIGNAL_FUNC(activeunits_list_ucallback), NULL);
  gtk_signal_connect(GTK_OBJECT(close_command), "clicked",
	GTK_SIGNAL_FUNC(activeunits_close_callback), NULL);
  gtk_signal_connect(GTK_OBJECT(upgrade_command), "clicked",
	GTK_SIGNAL_FUNC(activeunits_upgrade_callback), NULL);
  gtk_signal_connect(GTK_OBJECT(refresh_command), "clicked",
	GTK_SIGNAL_FUNC(activeunits_refresh_callback), NULL);

  gtk_widget_show_all( GTK_DIALOG(activeunits_dialog_shell)->vbox );
  gtk_widget_show_all( GTK_DIALOG(activeunits_dialog_shell)->action_area );

  gtk_widget_add_accelerator(close_command, "clicked",
	accel, GDK_Escape, 0, GTK_ACCEL_VISIBLE);

  activeunits_report_dialog_update();
}

/****************************************************************
...
*****************************************************************/
void activeunits_list_callback(GtkWidget *w, gint row, gint column)
{
  if ((unit_type_exists(activeunits_type[row])) &&
      (can_upgrade_unittype(game.player_ptr, activeunits_type[row]) != -1))
    gtk_widget_set_sensitive(upgrade_command, TRUE);
}

/****************************************************************
...
*****************************************************************/
void activeunits_list_ucallback(GtkWidget *w, gint row, gint column)
{
  gtk_widget_set_sensitive(upgrade_command, FALSE);
}

/****************************************************************
...
*****************************************************************/
static void upgrade_callback_yes(GtkWidget *w, gpointer data)
{
  send_packet_unittype_info(&aconnection, (size_t)data,PACKET_UNITTYPE_UPGRADE);
  destroy_message_dialog(w);
}

/****************************************************************
...
*****************************************************************/
static void upgrade_callback_no(GtkWidget *w, gpointer data)
{
  destroy_message_dialog(w);
}

/****************************************************************
...
*****************************************************************/
void activeunits_upgrade_callback(GtkWidget *w, gpointer data)
{
  char buf[512];
  int ut1,ut2;
  GList              *selection;
  gint                row;

  if ( !( selection = GTK_CLIST( activeunits_list )->selection ) )
      return;

  row = (gint)selection->data;

  ut1 = activeunits_type[row];
  if (!(unit_type_exists (ut1)))
    return;
  /* puts(unit_types[ut1].name); */

  ut2 = can_upgrade_unittype(game.player_ptr, activeunits_type[row]);

  my_snprintf(buf, sizeof(buf),
	  _("Upgrade as many %s to %s as possible for %d gold each?\n"
	    "Treasury contains %d gold."),
	  unit_types[ut1].name, unit_types[ut2].name,
	  unit_upgrade_price(game.player_ptr, ut1, ut2),
	  game.player_ptr->economic.gold);

  popup_message_dialog(toplevel, /*"upgradedialog"*/_("Upgrade Obsolete Units"), buf,
        	       _("Yes"), upgrade_callback_yes, (gpointer)(activeunits_type[row]),
        	       _("No"), upgrade_callback_no, 0, 0);
}

/****************************************************************
...
*****************************************************************/
void activeunits_close_callback(GtkWidget *w, gpointer data)
{

  if(activeunits_dialog_shell_is_modal)
     gtk_widget_set_sensitive(toplevel, TRUE);
   gtk_widget_destroy(activeunits_dialog_shell);
   activeunits_dialog_shell=0;
}

/****************************************************************
...
*****************************************************************/
void activeunits_refresh_callback(GtkWidget *w, gpointer data)
{
  activeunits_report_dialog_update();
}

/****************************************************************
...
*****************************************************************/
void activeunits_report_dialog_update(void)
{
  struct repoinfo {
    int active_count;
    int upkeep_shield;
    int upkeep_food;
    /* int upkeep_gold;   FIXME: add gold when gold is implemented --jjm */
    int building_count;
  };
  if(delay_report_update) return;
  if(activeunits_dialog_shell) {
    int    i, k, can;
    struct repoinfo unitarray[U_LAST];
    struct repoinfo unittotals;
    char  *report_title;
    char   activeunits_total[100];
    gchar *row[AU_COL];
    char   buf[AU_COL][64];

    report_title=get_report_title(_("Military Report"));
    gtk_set_label(activeunits_label, report_title);

    gtk_clist_freeze(GTK_CLIST(activeunits_list));
    gtk_clist_clear(GTK_CLIST(activeunits_list));

    for(i=0;i<(sizeof(row)/sizeof(row[0]));i++)
      row[i] = buf[i];

    memset(unitarray, '\0', sizeof(unitarray));
    unit_list_iterate(game.player_ptr->units, punit) {
      (unitarray[punit->type].active_count)++;
      if (punit->homecity) {
	unitarray[punit->type].upkeep_shield += punit->upkeep;
	unitarray[punit->type].upkeep_food += punit->upkeep_food;
      }
    }
    unit_list_iterate_end;
    city_list_iterate(game.player_ptr->cities,pcity) {
      if (pcity->is_building_unit &&
	  (unit_type_exists (pcity->currently_building)))
	(unitarray[pcity->currently_building].building_count)++;
    }
    city_list_iterate_end;

    k = 0;
    memset(&unittotals, '\0', sizeof(unittotals));
    for (i=0;i<game.num_unit_types;i++) {
      if ((unitarray[i].active_count > 0) || (unitarray[i].building_count > 0)) {
	can = (can_upgrade_unittype(game.player_ptr, i) != -1);
        my_snprintf(buf[0], sizeof(buf[0]), "%-27s", unit_name(i));
	my_snprintf(buf[1], sizeof(buf[1]), "%c", can ? '*': '-');
        my_snprintf(buf[2], sizeof(buf[2]), "%9d", unitarray[i].building_count);
        my_snprintf(buf[3], sizeof(buf[3]), "%9d", unitarray[i].active_count);
        my_snprintf(buf[4], sizeof(buf[4]), "%9d", unitarray[i].upkeep_shield);
        my_snprintf(buf[5], sizeof(buf[5]), "%9d", unitarray[i].upkeep_food);

	gtk_clist_append( GTK_CLIST( activeunits_list ), row );

	activeunits_type[k]=(unitarray[i].active_count > 0) ? i : U_LAST;
	k++;
	unittotals.active_count += unitarray[i].active_count;
	unittotals.upkeep_shield += unitarray[i].upkeep_shield;
	unittotals.upkeep_food += unitarray[i].upkeep_food;
	unittotals.building_count += unitarray[i].building_count;
      }
    }

    /* horrible kluge, but I can't get gtk_label_set_justify() to work --jjm */
    my_snprintf(activeunits_total, sizeof(activeunits_total),
	    _("Totals:                     %s%9d%s%9d%s%9d%s%9d"),
	    "        ", unittotals.building_count,
	    " ", unittotals.active_count,
	    " ", unittotals.upkeep_shield,
	    " ", unittotals.upkeep_food);
    gtk_set_label(activeunits_label2, activeunits_total); 

    gtk_widget_show_all(activeunits_list);
    gtk_clist_thaw(GTK_CLIST(activeunits_list));

    activeunits_list_ucallback(NULL, 0, 0);
  }
}
