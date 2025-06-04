MESA-GL, GRAPHICS, 3D OPENGL compatible library

Mesa is a 3-D graphics library with an API which is very similar to that
of OpenGL*.  To the extent that Mesa utilizes the OpenGL command syntax
or state machine, it is being used with authorization from Silicon Graphics,
Inc.  However, the author makes no claim that Mesa is in any way a
compatible replacement for OpenGL or associated with Silicon Graphics, Inc.
Those who want a licensed implementation of OpenGL should contact a licensed
vendor.  This software is distributed under the terms of the GNU Library
General Public License, see the LICENSE file for details.

* OpenGL(R) is a registered trademark of Silicon Graphics, Inc.



Author
======

Brian Paul
Avid Technology
6400 EnterPrise Lane, Suite 201
Madison, WI  53719

brianp@elastic.avid.com

MESA-GL 2.2 under OpenVMS                         May 1997
=========================

I've made a few modifications into MMS makefiles to build MESA-GL with older
DEC C versions (5.0 and 5.2) under OpenVMS 6.1 and 6.2

Original makefiles are kept (DESCRIP.MMS_ORI).

To build with MMS, just type :

$ MMS

To build with MMK, just tyme:

$ MMS :== 'MMK
$ MMS

Into this kit, you have all sources plus demos obj files ([.demo]VAXOBJS.ZIP
and [.demo]AXPOBJS.ZIP) and olb libraries ([.lib]VAXOLB.ZIP and
[.lib]AXPOLB.ZIP)

Enjoy !!

Patrick Moreau

pmoreau@cena.dgac.fr
moreau_p@decus.fr
