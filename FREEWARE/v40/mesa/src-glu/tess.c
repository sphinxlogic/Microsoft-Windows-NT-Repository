/* $Id: tess.c,v 1.3 1996/11/12 01:23:02 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  2.0
 * Copyright (C) 1995-1996  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * $Log: tess.c,v $
 * Revision 1.3  1996/11/12 01:23:02  brianp
 * added test to prevent free(vertex) when vertex==NULL in delete_contours()
 *
 * Revision 1.2  1996/10/22 22:57:19  brianp
 * better error handling in gluBegin/EndPolygon() from Erich Eder
 *
 * Revision 1.1  1996/09/27 01:19:39  brianp
 * Initial revision
 *
 */


/*
 * This file is part of the polygon tesselation code contributed by
 * Bogdan Sikorski
 */


#include <math.h>
#include <stdlib.h>
#include "tess.h"



extern void tess_test_polygon(GLUtriangulatorObj *);
extern void tess_find_contour_hierarchies(GLUtriangulatorObj *);
extern void tess_handle_holes(GLUtriangulatorObj *);
extern void tess_tesselate(GLUtriangulatorObj *);
extern void tess_tesselate_with_edge_flag(GLUtriangulatorObj *);
static void delete_contours(GLUtriangulatorObj *);

void init_callbacks(tess_callbacks *callbacks)
{
   callbacks->begin = ( void (*)(GLenum) ) 0;
   callbacks->edgeFlag = ( void (*)(GLboolean) ) 0;
   callbacks->vertex = ( void (*)(void*) ) 0;
   callbacks->end = ( void (*)(void) ) 0;
   callbacks->error = ( void (*)(GLenum) ) 0;
}

void tess_call_user_error(GLUtriangulatorObj *tobj,
	GLenum gluerr)
{
	if(tobj->error==GLU_NO_ERROR)
		tobj->error=gluerr;
	if(tobj->callbacks.error!=NULL)
		(tobj->callbacks.error)(gluerr);
}

GLUtriangulatorObj* gluNewTess( void )
{
   GLUtriangulatorObj *tobj;

	if((tobj=(GLUtriangulatorObj *)
		malloc(sizeof(struct GLUtriangulatorObj)))==NULL)
		return NULL;
	tobj->contours=tobj->last_contour=NULL;
	init_callbacks(&tobj->callbacks);
	tobj->error=GLU_NO_ERROR;
	tobj->current_polygon=NULL;
	tobj->contour_cnt=0;
	return tobj;
}


void gluTessCallback( GLUtriangulatorObj *tobj, GLenum which,
		      void (*fn)() )
{
	switch(which)
	{
		case GLU_BEGIN:
			tobj->callbacks.begin = (void (*)(GLenum)) fn;
			break;
		case GLU_EDGE_FLAG:
			tobj->callbacks.edgeFlag = (void (*)(GLboolean)) fn;
			break;
		case GLU_VERTEX:
			tobj->callbacks.vertex = (void (*)(void *)) fn;
			break;
		case GLU_END:
			tobj->callbacks.end=fn = (void (*)(void)) fn;
			break;
		case GLU_ERROR:
			tobj->callbacks.error = (void (*)(GLenum)) fn;
			break;
		default:
			tobj->error=GLU_INVALID_ENUM;
			break;
	}
}



void gluDeleteTess( GLUtriangulatorObj *tobj )
{
	if(tobj->error==GLU_NO_ERROR && tobj->contour_cnt)
		/* was gluEndPolygon called? */
		tess_call_user_error(tobj,GLU_TESS_ERROR1);
	/* delete all internal structures */
	delete_contours(tobj);
	free(tobj);
}


void gluBeginPolygon( GLUtriangulatorObj *tobj )
{
/*
	if(tobj->error!=GLU_NO_ERROR)
		return;
*/
        tobj->error = GLU_NO_ERROR;
	if(tobj->current_polygon!=NULL)
	{
		/* gluEndPolygon was not called */
		tess_call_user_error(tobj,GLU_TESS_ERROR1);
		/* delete all internal structures */
		delete_contours(tobj);
	}
	else
	{
		if((tobj->current_polygon=
			(tess_polygon *)malloc(sizeof(tess_polygon)))==NULL)
		{
			tess_call_user_error(tobj,GLU_OUT_OF_MEMORY);
			return;
		}
		tobj->current_polygon->vertex_cnt=0;
		tobj->current_polygon->vertices=
			tobj->current_polygon->last_vertex=NULL;
	}
}


void gluEndPolygon( GLUtriangulatorObj *tobj )
{
	/*tess_contour *contour_ptr;*/

	/* there was an error */
	if(tobj->error!=GLU_NO_ERROR) goto end;

	/* check if gluBeginPolygon was called */
	if(tobj->current_polygon==NULL)
	{
		tess_call_user_error(tobj,GLU_TESS_ERROR2);
		return;
	}
	tess_test_polygon(tobj);
	/* there was an error */
	if(tobj->error!=GLU_NO_ERROR) goto end;

	/* any real contours? */
	if(tobj->contour_cnt==0)
	{
		/* delete all internal structures */
		delete_contours(tobj);
		return;
	}
	tess_find_contour_hierarchies(tobj);
	/* there was an error */
	if(tobj->error!=GLU_NO_ERROR) goto end;

	tess_handle_holes(tobj);
	/* there was an error */
	if(tobj->error!=GLU_NO_ERROR) goto end;

	/* if no callbacks, nothing to do */
	if(tobj->callbacks.begin!=NULL && tobj->callbacks.vertex!=NULL &&
		tobj->callbacks.end!=NULL)
	{
		if(tobj->callbacks.edgeFlag==NULL)
			tess_tesselate(tobj);
		else
			tess_tesselate_with_edge_flag(tobj);
	}

end:
	/* delete all internal structures */
	delete_contours(tobj);
}


void gluNextContour( GLUtriangulatorObj *tobj, GLenum type )
{
	if(tobj->error!=GLU_NO_ERROR)
		return;
	if(tobj->current_polygon==NULL)
	{
		tess_call_user_error(tobj,GLU_TESS_ERROR2);
		return;
	}
	/* first contour? */
	if(tobj->current_polygon->vertex_cnt)
		tess_test_polygon(tobj);
}


void gluTessVertex( GLUtriangulatorObj *tobj, GLdouble v[3], void *data )
{
	tess_polygon *polygon=tobj->current_polygon;
	tess_vertex *last_vertex_ptr;

	if(tobj->error!=GLU_NO_ERROR)
		return;
	if(polygon==NULL)
	{
		tess_call_user_error(tobj,GLU_TESS_ERROR2);
		return;
	}
	last_vertex_ptr=polygon->last_vertex;
	if(last_vertex_ptr==NULL)
	{
		if((last_vertex_ptr=(tess_vertex *)
			malloc(sizeof(tess_vertex)))==NULL)
		{
			tess_call_user_error(tobj,GLU_OUT_OF_MEMORY);
			return;
		}
		polygon->vertices=last_vertex_ptr;
		polygon->last_vertex=last_vertex_ptr;
		last_vertex_ptr->data=data;
		last_vertex_ptr->location[0]=v[0];
		last_vertex_ptr->location[1]=v[1];
		last_vertex_ptr->location[2]=v[2];
		last_vertex_ptr->next=NULL;
		last_vertex_ptr->previous=NULL;
		++(polygon->vertex_cnt);
	}
	else
	{
		tess_vertex *vertex_ptr;

		/* same point twice? */
		if(fabs(last_vertex_ptr->location[0]-v[0]) < EPSILON &&
			fabs(last_vertex_ptr->location[1]-v[1]) < EPSILON &&
			fabs(last_vertex_ptr->location[2]-v[2]) < EPSILON)
		{
			tess_call_user_error(tobj,GLU_TESS_ERROR6);
			return;
		}
		if((vertex_ptr=(tess_vertex *)
			malloc(sizeof(tess_vertex)))==NULL)
		{
			tess_call_user_error(tobj,GLU_OUT_OF_MEMORY);
			return;
		}
		vertex_ptr->data=data;
		vertex_ptr->location[0]=v[0];
		vertex_ptr->location[1]=v[1];
		vertex_ptr->location[2]=v[2];
		vertex_ptr->next=NULL;
		vertex_ptr->previous=last_vertex_ptr;
		++(polygon->vertex_cnt);
		last_vertex_ptr->next=vertex_ptr;
		polygon->last_vertex=vertex_ptr;
	}
}


static void delete_contours(GLUtriangulatorObj *tobj)
{
	tess_polygon *polygon=tobj->current_polygon;
	tess_contour *contour,*contour_tmp;
	tess_vertex *vertex,*vertex_tmp;

	/* remove current_polygon list - if exists due to detected error */
	if(polygon!=NULL)
	{
		if (polygon->vertices)
		{
			for(vertex=polygon->vertices;vertex!=polygon->last_vertex;)
			{
				vertex_tmp=vertex->next;
				free(vertex);
				vertex=vertex_tmp;
			}
			free(vertex);
		}
		free(polygon);
		tobj->current_polygon=NULL;
	}
	/* remove all contour data */
	for(contour=tobj->contours;contour!=NULL;)
	{
		for(vertex=contour->vertices;vertex!=contour->last_vertex;)
		{
			vertex_tmp=vertex->next;
			free(vertex);
			vertex=vertex_tmp;
		}
		free(vertex);
		contour_tmp=contour->next;
		free(contour);
		contour=contour_tmp;
	}
	tobj->contours=tobj->last_contour=NULL;
	tobj->contour_cnt=0;
}



