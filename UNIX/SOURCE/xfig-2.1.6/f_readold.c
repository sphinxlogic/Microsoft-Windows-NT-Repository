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

/*******************************************************************/
/***************       Read version 1.3 format	     ***************/
/*******************************************************************/
#include "fig.h"
#include "object.h"
#include "u_create.h"

/*******    Fig 1.3 subtype of objects	  *******/
#define			DRAW_ELLIPSE_BY_RAD	1
#define			DRAW_ELLIPSE_BY_DIA	2
#define			DRAW_CIRCLE_BY_RAD	3
#define			DRAW_CIRCLE_BY_DIA	4
#define			DRAW_CIRCULAR_ARC	5
#define			DRAW_POLYLINE		6
#define			DRAW_BOX		7
#define			DRAW_POLYGON		8
#define			DRAW_TEXT		9
#define			DRAW_SPLINE		10
#define			DRAW_CLOSEDSPLINE	11
#define			DRAW_COMPOUND		13

static F_ellipse *read_1_3_ellipseobject();
static F_line  *read_1_3_lineobject();
static F_text  *read_1_3_textobject();
static F_spline *read_1_3_splineobject();
static F_arc   *read_1_3_arcobject();
static F_compound *read_1_3_compoundobject();

extern int	line_no;
extern int	num_object;

int
read_1_3_objects(fp, obj)
    FILE	   *fp;
    F_compound	   *obj;
{
    F_ellipse	   *e, *le = NULL;
    F_line	   *l, *ll = NULL;
    F_text	   *t, *lt = NULL;
    F_spline	   *s, *ls = NULL;
    F_arc	   *a, *la = NULL;
    F_compound	   *c, *lc = NULL;
    int		    n;
    int		    object, pixperinch, canvaswid, canvasht, coord_sys;

    n = fscanf(fp, "%d%d%d%d\n", &pixperinch, &coord_sys, &canvaswid, &canvasht);
    if (n != 4) {
	file_msg("Incorrect format in the first line in input file");
	return (-1);
    }
    obj->nwcorner.x = pixperinch;
    obj->nwcorner.y = coord_sys;
    while (fscanf(fp, "%d", &object) == 1) {
	switch (object) {
	case O_POLYLINE:
	    if ((l = read_1_3_lineobject(fp)) == NULL)
		return (-1);
	    if (ll)
		ll = (ll->next = l);
	    else
		ll = obj->lines = l;
	    num_object++;
	    break;
	case O_SPLINE:
	    if ((s = read_1_3_splineobject(fp)) == NULL)
		return (-1);
	    if (ls)
		ls = (ls->next = s);
	    else
		ls = obj->splines = s;
	    num_object++;
	    break;
	case O_ELLIPSE:
	    if ((e = read_1_3_ellipseobject(fp)) == NULL)
		return (-1);
	    if (le)
		le = (le->next = e);
	    else
		le = obj->ellipses = e;
	    num_object++;
	    break;
	case O_ARC:
	    if ((a = read_1_3_arcobject(fp)) == NULL)
		return (-1);
	    if (la)
		la = (la->next = a);
	    else
		la = obj->arcs = a;
	    num_object++;
	    break;
	case O_TEXT:
	    if ((t = read_1_3_textobject(fp)) == NULL)
		return (-1);
	    if (lt)
		lt = (lt->next = t);
	    else
		lt = obj->texts = t;
	    num_object++;
	    break;
	case O_COMPOUND:
	    if ((c = read_1_3_compoundobject(fp)) == NULL)
		return (-1);
	    if (lc)
		lc = (lc->next = c);
	    else
		lc = obj->compounds = c;
	    num_object++;
	    break;
	default:
	    file_msg("Incorrect object code %d", object);
	    return (-1);
	}			/* switch */
    }				/* while */
    if (feof(fp))
	return (0);
    else
	return (errno);
}

static F_arc   *
read_1_3_arcobject(fp)
    FILE	   *fp;
{
    F_arc	   *a;
    int		    f, b, h, w, n;

    if ((a = create_arc()) == NULL)
	return (NULL);

    a->type = T_3_POINTS_ARC;
    a->color = BLACK;
    a->depth = 0;
    a->pen = 0;
    a->for_arrow = NULL;
    a->back_arrow = NULL;
    a->next = NULL;
    n = fscanf(fp, " %d %d %d %f %d %d %d %d %d %f %f %d %d %d %d %d %d\n",
	       &a->type, &a->style, &a->thickness,
	       &a->style_val, &a->direction, &f, &b,
	       &h, &w, &a->center.x, &a->center.y,
	       &a->point[0].x, &a->point[0].y,
	       &a->point[1].x, &a->point[1].y,
	       &a->point[2].x, &a->point[2].y);
    if (n != 17) {
	file_msg("Incomplete arc data");
	free((char *) a);
	return (NULL);
    }
    if (f) {
	a->for_arrow = forward_arrow();
	a->for_arrow->wid = w;
	a->for_arrow->ht = h;
    }
    if (b) {
	a->back_arrow = backward_arrow();
	a->back_arrow->wid = w;
	a->back_arrow->ht = h;
    }
    return (a);
}

static F_compound *
read_1_3_compoundobject(fp)
    FILE	   *fp;
{
    F_arc	   *a, *la = NULL;
    F_ellipse	   *e, *le = NULL;
    F_line	   *l, *ll = NULL;
    F_spline	   *s, *ls = NULL;
    F_text	   *t, *lt = NULL;
    F_compound	   *com, *c, *lc = NULL;
    int		    n, object;

    if ((com = create_compound()) == NULL)
	return (NULL);

    com->arcs = NULL;
    com->ellipses = NULL;
    com->lines = NULL;
    com->splines = NULL;
    com->texts = NULL;
    com->compounds = NULL;
    com->next = NULL;
    n = fscanf(fp, " %d %d %d %d\n", &com->nwcorner.x, &com->nwcorner.y,
	       &com->secorner.x, &com->secorner.y);
    if (n != 4) {
	file_msg("Incorrect compound object format");
	return (NULL);
    }
    while (fscanf(fp, "%d", &object) == 1) {
	switch (object) {
	case O_POLYLINE:
	    if ((l = read_1_3_lineobject(fp)) == NULL) {
		free_line(&l);
		return (NULL);
	    }
	    if (ll)
		ll = (ll->next = l);
	    else
		ll = com->lines = l;
	    break;
	case O_SPLINE:
	    if ((s = read_1_3_splineobject(fp)) == NULL) {
		free_spline(&s);
		return (NULL);
	    }
	    if (ls)
		ls = (ls->next = s);
	    else
		ls = com->splines = s;
	    break;
	case O_ELLIPSE:
	    if ((e = read_1_3_ellipseobject(fp)) == NULL) {
		free_ellipse(&e);
		return (NULL);
	    }
	    if (le)
		le = (le->next = e);
	    else
		le = com->ellipses = e;
	    break;
	case O_ARC:
	    if ((a = read_1_3_arcobject(fp)) == NULL) {
		free_arc(&a);
		return (NULL);
	    }
	    if (la)
		la = (la->next = a);
	    else
		la = com->arcs = a;
	    break;
	case O_TEXT:
	    if ((t = read_1_3_textobject(fp)) == NULL) {
		free_text(&t);
		return (NULL);
	    }
	    if (lt)
		lt = (lt->next = t);
	    else
		lt = com->texts = t;
	    break;
	case O_COMPOUND:
	    if ((c = read_1_3_compoundobject(fp)) == NULL) {
		free_compound(&c);
		return (NULL);
	    }
	    if (lc)
		lc = (lc->next = c);
	    else
		lc = com->compounds = c;
	    break;
	case O_END_COMPOUND:
	    return (com);
	}			/* switch */
    }
    if (feof(fp))
	return (com);
    else {
	file_msg("Format error: %s", sys_errlist[errno]);
	return (NULL);
    }
}

static F_ellipse *
read_1_3_ellipseobject(fp)
    FILE	   *fp;
{
    F_ellipse	   *e;
    int		    n, t;

    if ((e = create_ellipse()) == NULL)
	return (NULL);

    e->color = BLACK;
    e->angle = 0.0;
    e->depth = 0;
    e->pen = 0;
    e->fill_style = 0;
    e->next = NULL;
    n = fscanf(fp, " %d %d %d %f %d %d %d %d %d %d %d %d %d\n",
	       &t, &e->style,
	       &e->thickness, &e->style_val, &e->direction,
	       &e->center.x, &e->center.y,
	       &e->radiuses.x, &e->radiuses.y,
	       &e->start.x, &e->start.y,
	       &e->end.x, &e->end.y);
    if (n != 13) {
	file_msg("Incomplete ellipse data");
	free((char *) e);
	return (NULL);
    }
    if (t == DRAW_ELLIPSE_BY_RAD)
	e->type = T_ELLIPSE_BY_RAD;
    else if (t == DRAW_ELLIPSE_BY_DIA)
	e->type = T_ELLIPSE_BY_DIA;
    else if (t == DRAW_CIRCLE_BY_RAD)
	e->type = T_CIRCLE_BY_RAD;
    else
	e->type = T_CIRCLE_BY_DIA;
    return (e);
}

static F_line  *
read_1_3_lineobject(fp)
    FILE	   *fp;
{
    F_line	   *l;
    F_point	   *p, *q;
    int		    f, b, h, w, n, t, x, y;

    if ((l = create_line()) == NULL)
	return (NULL);

    l->color = BLACK;
    l->depth = 0;
    l->pen = 0;
    l->fill_style = 0;
    l->for_arrow = NULL;
    l->back_arrow = NULL;
    l->next = NULL;
    if ((p = create_point()) == NULL) {
	free((char *) l);
	return (NULL);
    }
    l->points = p;
    n = fscanf(fp, " %d %d %d %f %d %d %d %d %d %d", &t,
	       &l->style, &l->thickness, &l->style_val,
	       &f, &b, &h, &w, &p->x, &p->y);
    if (n != 10) {
	file_msg("Incomplete line data");
	free((char *) l);
	return (NULL);
    }
    if (t == DRAW_POLYLINE)
	l->type = T_POLYLINE;
    else if (t == DRAW_POLYGON)
	l->type = T_POLYGON;
    else
	l->type = T_BOX;
    if (f) {
	l->for_arrow = forward_arrow();
	l->for_arrow->wid = w;
	l->for_arrow->ht = h;
    }
    if (b) {
	l->back_arrow = backward_arrow();
	l->back_arrow->wid = w;
	l->back_arrow->ht = h;
    }
    for (;;) {
	if (fscanf(fp, " %d %d", &x, &y) != 2) {
	    file_msg("Incomplete line object");
	    free_linestorage(l);
	    return (NULL);
	}
	if (x == 9999)
	    break;
	if ((q = create_point()) == NULL)
	    return (NULL);
	q->x = x;
	q->y = y;
	q->next = NULL;
	p->next = q;
	p = q;
    }
    return (l);
}

static F_spline *
read_1_3_splineobject(fp)
    FILE	   *fp;
{
    F_spline	   *s;
    F_point	   *p, *q;
    int		    f, b, h, w, n, t, x, y;

    if ((s = create_spline()) == NULL)
	return (NULL);

    s->color = BLACK;
    s->depth = 0;
    s->pen = 0;
    s->fill_style = 0;
    s->for_arrow = NULL;
    s->back_arrow = NULL;
    s->controls = NULL;
    s->next = NULL;
    if ((p = create_point()) == NULL) {
	free((char *) s);
	return (NULL);
    }
    s->points = p;
    n = fscanf(fp, " %d %d %d %f %d %d %d %d %d %d",
	       &t, &s->style, &s->thickness, &s->style_val,
	       &f, &b,
	       &h, &w, &p->x, &p->y);
    if (n != 10) {
	file_msg("Incomplete spline data");
	free((char *) s);
	return (NULL);
    }
    if (t == DRAW_CLOSEDSPLINE)
	s->type = T_CLOSED_NORMAL;
    else
	s->type = T_OPEN_NORMAL;
    if (f) {
	s->for_arrow = forward_arrow();
	s->for_arrow->wid = w;
	s->for_arrow->ht = h;
    }
    if (b) {
	s->back_arrow = backward_arrow();
	s->back_arrow->wid = w;
	s->back_arrow->ht = h;
    }
    for (;;) {
	if (fscanf(fp, " %d %d", &x, &y) != 2) {
	    file_msg("Incomplete spline object");
	    free_splinestorage(s);
	    return (NULL);
	};
	if (x == 9999)
	    break;
	if ((q = create_point()) == NULL) {
	    free_splinestorage(s);
	    return (NULL);
	}
	q->x = x;
	q->y = y;
	q->next = NULL;
	p->next = q;
	p = q;
    }
    return (s);
}

static F_text  *
read_1_3_textobject(fp)
    FILE	   *fp;
{
    F_text	   *t;
    int		    n;
    char	    buf[128];

    if ((t = create_text()) == NULL)
	return (NULL);

    t->type = T_LEFT_JUSTIFIED;
    t->flags = RIGID_TEXT;
    t->color = BLACK;
    t->depth = 0;
    t->pen = 0;
    t->angle = 0.0;
    t->next = NULL;
    n = fscanf(fp, " %d %d %d %d %d %d %d %[^\n]", &t->font,
	       &t->size, &t->flags, &t->height, &t->length,
	       &t->base_x, &t->base_y, buf);
    if (n != 8) {
	file_msg("Incomplete text data");
	free((char *) t);
	return (NULL);
    }
    if ((t->cstring = new_string(strlen(buf) + 1)) == NULL) {
	free((char *) t);
	return (NULL);
    }
    strcpy(t->cstring, buf);
    if (t->size == 0)
	t->size = 18;
    return (t);
}
