/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "resources.h"
#include "object.h"
#include "paintop.h"
#include "u_draw.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_undo.h"
#include "mode.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_zoom.h"

static int	place_line(), cancel_line();
static int	place_arc(), cancel_arc();
static int	place_spline(), cancel_spline();
static int	place_ellipse(), cancel_ellipse();
static int	place_text(), cancel_text();
static int	place_compound(), cancel_compound();

extern int	copy_selected();

/***************************** ellipse section ************************/

init_ellipsedragging(e, x, y)
    F_ellipse	   *e;
    int		    x, y;
{
    new_e = e;
    fix_x = cur_x = x;
    fix_y = cur_y = y;
    cur_angle = e->angle;
    x1off = (e->center.x - e->radiuses.x) - cur_x;
    x2off = (e->center.x + e->radiuses.x) - cur_x;
    y1off = (e->center.y - e->radiuses.y) - cur_y;
    y2off = (e->center.y + e->radiuses.y) - cur_y;
    canvas_locmove_proc = moving_ellipse;
    canvas_leftbut_proc = place_ellipse;
    canvas_rightbut_proc = cancel_ellipse;
    set_action_on();
    elastic_moveellipse();
}

static
cancel_ellipse()
{
    elastic_moveellipse();
    if (return_proc == copy_selected) {
	free_ellipse(&new_e);
    } else {
	list_add_ellipse(&objects.ellipses, new_e);
	redisplay_ellipse(new_e);
    }
    (*return_proc) ();
    draw_mousefun_canvas();
}

static
place_ellipse(x, y)
    int		    x, y;
{
    elastic_moveellipse();
    adjust_pos(x, y, fix_x, fix_y, &x, &y);
    translate_ellipse(new_e, x - fix_x, y - fix_y);
    if (return_proc == copy_selected) {
	add_ellipse(new_e);
    } else {
	list_add_ellipse(&objects.ellipses, new_e);
	clean_up();
	set_lastposition(fix_x, fix_y);
	set_newposition(x, y);
	set_action_object(F_MOVE, O_ELLIPSE);
	set_latestellipse(new_e);
	set_modifiedflag();
    }
    redisplay_ellipse(new_e);
    (*return_proc) ();
    draw_mousefun_canvas();
}

/*****************************	arc  section  *******************/

init_arcdragging(a, x, y)
    F_arc	   *a;
    int		    x, y;
{
    new_a = a;
    fix_x = cur_x = x;
    fix_y = cur_y = y;
    canvas_locmove_proc = moving_arc;
    canvas_leftbut_proc = place_arc;
    canvas_rightbut_proc = cancel_arc;
    set_action_on();
    elastic_movearc(new_a);
}

static
cancel_arc()
{
    elastic_movearc(new_a);
    if (return_proc == copy_selected) {
	free_arc(&new_a);
    } else {
	list_add_arc(&objects.arcs, new_a);
	redisplay_arc(new_a);
    }
    (*return_proc) ();
    draw_mousefun_canvas();
}

static
place_arc(x, y)
    int		    x, y;
{
    elastic_movearc(new_a);
    adjust_pos(x, y, fix_x, fix_y, &x, &y);
    translate_arc(new_a, x - fix_x, y - fix_y);
    if (return_proc == copy_selected) {
	add_arc(new_a);
    } else {
	list_add_arc(&objects.arcs, new_a);
	clean_up();
	set_lastposition(fix_x, fix_y);
	set_newposition(x, y);
	set_action_object(F_MOVE, O_ARC);
	set_latestarc(new_a);
	set_modifiedflag();
    }
    redisplay_arc(new_a);
    (*return_proc) ();
    draw_mousefun_canvas();
}

/*************************  line  section  **********************/

init_linedragging(l, x, y)
    F_line	   *l;
    int		    x, y;
{
    int		    xmin, ymin, xmax, ymax;

    new_l = l;
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    canvas_locmove_proc = moving_line;
    canvas_leftbut_proc = place_line;
    canvas_rightbut_proc = cancel_line;
    set_action_on();
    if (l->type == T_BOX || l->type == T_ARC_BOX || l->type == T_EPS_BOX) {
	line_bound(l, &xmin, &ymin, &xmax, &ymax);
	get_links(xmin, ymin, xmax, ymax);
    }
    elastic_moveline(new_l->points);
}

static
cancel_line()
{
    elastic_moveline(new_l->points);
    free_linkinfo(&cur_links);
    if (return_proc == copy_selected) {
	free_line(&new_l);
    } else {
	list_add_line(&objects.lines, new_l);
	redisplay_line(new_l);
    }
    (*return_proc) ();
    draw_mousefun_canvas();
}

static
place_line(x, y)
    int		    x, y;
{
    int		    dx, dy;

    elastic_moveline(new_l->points);
    adjust_pos(x, y, fix_x, fix_y, &x, &y);
    dx = x - fix_x;
    dy = y - fix_y;
    translate_line(new_l, dx, dy);
    clean_up();
    set_latestline(new_l);
    if (return_proc == copy_selected) {
	adjust_links(cur_linkmode, cur_links, dx, dy, 0, 0, 1.0, 1.0, 1);
	tail(&objects, &object_tails);
	append_objects(&objects, &saved_objects, &object_tails);
	set_action_object(F_ADD, O_ALL_OBJECT);
	free_linkinfo(&cur_links);
    } else {
	list_add_line(&objects.lines, new_l);
	adjust_links(cur_linkmode, cur_links, dx, dy, 0, 0, 1.0, 1.0, 0);
	set_lastposition(fix_x, fix_y);
	set_newposition(x, y);
	set_lastlinkinfo(cur_linkmode, cur_links);
	cur_links = NULL;
	set_action_object(F_MOVE, O_POLYLINE);
    }
    set_modifiedflag();
    redisplay_line(new_l);
    (*return_proc) ();
    draw_mousefun_canvas();
}

/************************  text section	 **************************/

static PR_SIZE	txsize;

init_textdragging(t, x, y)
    F_text	   *t;
    int		    x, y;
{

    new_t = t;
    fix_x = cur_x = x;
    fix_y = cur_y = y;
    x1off = t->base_x - x;
    y1off = t->base_y - y;
    if (t->type == T_CENTER_JUSTIFIED || t->type == T_RIGHT_JUSTIFIED) {
	txsize = pf_textwidth(t->font, psfont_text(t), t->size,
			      strlen(t->cstring), t->cstring);
	if (t->type == T_CENTER_JUSTIFIED)
	    x1off -= round(txsize.x / 2 / zoomscale);
	else
	    x1off -= round(txsize.x / zoomscale);
    }
    canvas_locmove_proc = moving_text;
    canvas_leftbut_proc = place_text;
    canvas_rightbut_proc = cancel_text;
    elastic_movetext();
    set_action_on();
}

static
cancel_text()
{
    elastic_movetext();
    if (return_proc == copy_selected) {
	free_text(&new_t);
    } else {
	list_add_text(&objects.texts, new_t);
	redisplay_text(new_t);
    }
    (*return_proc) ();
    draw_mousefun_canvas();
}

static
place_text(x, y)
    int		    x, y;
{
    elastic_movetext();
    adjust_pos(x, y, fix_x, fix_y, &x, &y);
    translate_text(new_t, x - fix_x, y - fix_y);
    if (return_proc == copy_selected) {
	add_text(new_t);
    } else {
	list_add_text(&objects.texts, new_t);
	clean_up();
	set_lastposition(fix_x, fix_y);
	set_newposition(x, y);
	set_action_object(F_MOVE, O_TEXT);
	set_latesttext(new_t);
	set_modifiedflag();
    }
    redisplay_text(new_t);
    (*return_proc) ();
    draw_mousefun_canvas();
}

/*************************  spline  section  **********************/

init_splinedragging(s, x, y)
    F_spline	   *s;
    int		    x, y;
{
    new_s = s;
    cur_x = fix_x = x;
    cur_y = fix_y = y;
    canvas_locmove_proc = moving_spline;
    canvas_leftbut_proc = place_spline;
    canvas_rightbut_proc = cancel_spline;
    set_action_on();
    elastic_moveline(new_s->points);
}

static
cancel_spline()
{
    elastic_moveline(new_s->points);
    if (return_proc == copy_selected) {
	free_spline(&new_s);
    } else {
	list_add_spline(&objects.splines, new_s);
	redisplay_spline(new_s);
    }
    (*return_proc) ();
    draw_mousefun_canvas();
}

static
place_spline(x, y)
    int		    x, y;
{
    elastic_moveline(new_s->points);
    adjust_pos(x, y, fix_x, fix_y, &x, &y);
    translate_spline(new_s, x - fix_x, y - fix_y);
    if (return_proc == copy_selected) {
	add_spline(new_s);
    } else {
	list_add_spline(&objects.splines, new_s);
	clean_up();
	set_lastposition(fix_x, fix_y);
	set_newposition(x, y);
	set_action_object(F_MOVE, O_SPLINE);
	set_latestspline(new_s);
	set_modifiedflag();
    }
    redisplay_spline(new_s);
    (*return_proc) ();
    draw_mousefun_canvas();
}

/*****************************	Compound section  *******************/

init_compounddragging(c, x, y)
    F_compound	   *c;
    int		    x, y;
{
    new_c = c;
    fix_x = cur_x = x;
    fix_y = cur_y = y;
    x1off = c->nwcorner.x - x;
    x2off = c->secorner.x - x;
    y1off = c->nwcorner.y - y;
    y2off = c->secorner.y - y;
    canvas_locmove_proc = moving_box;
    canvas_leftbut_proc = place_compound;
    canvas_rightbut_proc = cancel_compound;
    set_action_on();
    get_links(c->nwcorner.x, c->nwcorner.y, c->secorner.x, c->secorner.y);
    elastic_movebox();
}

static
cancel_compound()
{
    elastic_movebox();
    free_linkinfo(&cur_links);
    if (return_proc == copy_selected) {
	free_compound(&new_c);
    } else {
	list_add_compound(&objects.compounds, new_c);
	redisplay_compound(new_c);
    }
    (*return_proc) ();
    draw_mousefun_canvas();
}

static
place_compound(x, y)
    int		    x, y;
{
    int		    dx, dy;

    elastic_movebox();
    adjust_pos(x, y, fix_x, fix_y, &x, &y);
    dx = x - fix_x;
    dy = y - fix_y;
    translate_compound(new_c, dx, dy);
    clean_up();
    set_latestcompound(new_c);
    if (return_proc == copy_selected) {
	adjust_links(cur_linkmode, cur_links, dx, dy, 0, 0, 1.0, 1.0, 1);
	tail(&objects, &object_tails);
	append_objects(&objects, &saved_objects, &object_tails);
	set_action_object(F_ADD, O_ALL_OBJECT);
	free_linkinfo(&cur_links);
    } else {
	list_add_compound(&objects.compounds, new_c);
	adjust_links(cur_linkmode, cur_links, dx, dy, 0, 0, 1.0, 1.0, 0);
	set_lastposition(fix_x, fix_y);
	set_newposition(x, y);
	set_lastlinkinfo(cur_linkmode, cur_links);
	cur_links = NULL;
	set_action_object(F_MOVE, O_COMPOUND);
    }
    set_modifiedflag();
    redisplay_compound(new_c);
    (*return_proc) ();
    draw_mousefun_canvas();
}
