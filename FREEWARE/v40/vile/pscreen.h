/* The following set of definitions comprise the interface to pscreen
 * in display.c.
 */

#ifndef PSCREEN_H
#define PSCREEN_H 1

extern VIDEO **pscreen;

#define IS_DIRTY_LINE(r)	(pscreen[(r)]->v_flag & VFCHG)
#define IS_DIRTY(r,c)		(pscreen[(r)]->v_attrs[(c)] & VADIRTY)
#define IS_REVERSED(r,c)	((pscreen[(r)]->v_attrs[(c)] & (VAREV|VASEL)) != 0)
#define MARK_LINE_DIRTY(r)	(pscreen[(r)]->v_flag |= VFCHG)
#define MARK_CELL_DIRTY(r,c)	(pscreen[(r)]->v_attrs[(c)] |= VADIRTY)
#define CLEAR_LINE_DIRTY(r)	(pscreen[(r)]->v_flag &= ~VFCHG)
#define CLEAR_CELL_DIRTY(r,c)	(pscreen[(r)]->v_attrs[(c)] &= ~VADIRTY)
#define CELL_TEXT(r,c)		(pscreen[(r)]->v_text[(c)])
#define CELL_ATTR(r,c)		(pscreen[(r)]->v_attrs[(c)])

#endif /* PSCREEN_H */
