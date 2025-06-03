/*
 *  acm : an aerial combat simulator for X
 *  Copyright (C) 1991  Riley Rainey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 dated June, 1991.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program;  if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave., Cambridge, MA 02139, USA.
 */
 
#include "pm.h"

void placeCraft (obj, poly, cnt)
craft	 *obj;
VPolygon **poly;
int	 *cnt; {

	int	 i, j, k, n;
	VPoint	 *q, tmp;
	VMatrix	 mtx;
	VPolygon **p;

	j = *cnt;

	VIdentMatrix (&mtx);
	if (obj->curRoll != 0.0)
		VRotate (&mtx, XRotation, obj->curRoll);
	if (obj->curPitch != 0.0)
		VRotate (&mtx, YRotation, - obj->curPitch);
	if (obj->curHeading != 0.0)
		VRotate (&mtx, ZRotation, obj->curHeading);
	VTranslatePoint (&mtx, obj->Sg);

	if (obj->cinfo->placeProc != NULL) {
		(*obj->cinfo->placeProc)(obj, &mtx, poly, cnt);
		return;
	}

	n = obj->cinfo->object->numPolys;
	p = obj->cinfo->object->polygon;
	for (i=0; i<n; ++i) {

		poly[j] = VCopyPolygon(p[i]);
		for ((k=0, q=poly[j]->vertex); k<poly[j]->numVtces; (++k, ++q)) {
			VTransform(q, &mtx, &tmp);
			*q = tmp;
		}
		++j;
	}

	*cnt = j;
}
