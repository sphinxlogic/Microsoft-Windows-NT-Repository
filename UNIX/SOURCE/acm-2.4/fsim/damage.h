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
#define isFunctioning(c, sys)	((c->damageBits & sys) ? 1 : 0)

/*
 *  Each of these bits are true if the corresponding system is
 *  functioning.
 */

#define	SYS_RADAR	0x0001	/* set if radar works */
#define SYS_TEWS	0x0002	/* set if TEWS works */
#define SYS_FLAPS	0x0004	/* set if flaps work */
#define SYS_SPEEDBRAKE	0x0008
#define SYS_HYD1	0x0010
#define SYS_HYD2	0x0020
#define SYS_GEN1	0x0040
#define SYS_GEN2	0x0080
#define SYS_ENGINE1	0x0100	/* set if engine #1 works */
#define SYS_ENGINE2	0x0200	/* set if engine #2 works */
#define SYS_HUD		0x0400	/* set if HUD works */
#define SYS_FUEL	0x0800	/* set if there isn't a fuel leak */
#define SYS_NOSEGEAR	0x1000
#define SYS_LEFTMAIN	0x2000
#define SYS_RIGHTMAIN	0x4000
#define SYS_NODAMAGE	0x7fff

#define FLAG_LOWFUEL	0x8000	/* set when fuel < 1000 lbs */
#define FLAG_SPEEDBRAKE	0x10000	/* set when speedbrake is deployed */
#define FLAG_WHEELBRAKE	0x20000	/* set when wheel brakes are applied */

