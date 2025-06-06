/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Cimarron D. Taylor of the University of California, Berkeley.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static char sccsid[] = "@(#)operator.c	5.4 (Berkeley) 5/24/91";
#endif /* not lint */

#if DF_POSIX
# include <misc.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "find.h"

#if WIN_NT
extern int f_expr (PLAN *, FTSENT *);
#endif
    
/*
 * yanknode --
 *	destructively removes the top from the plan
 */
static PLAN *
#if __STDC__
yanknode (PLAN **planp)    
#else
yanknode(planp)    
	PLAN **planp;		/* pointer to top of plan (modified) */
#endif
{
	PLAN *node;		/* top node removed from the plan */
    
	if ((node = (*planp)) == NULL)
		return(NULL);
	(*planp) = (*planp)->next;
	node->next = NULL;
	return(node);
}
 
/*
 * yankexpr --
 *	Removes one expression from the plan.  This is used mainly by
 *	paren_squish.  In comments below, an expression is either a
 *	simple node or a N_EXPR node containing a list of simple nodes.
 */
static PLAN *
#if __STDC__
yankexpr (PLAN **planp)    
#else
yankexpr(planp)    
	PLAN **planp;		/* pointer to top of plan (modified) */
#endif
{
	register PLAN *next;	/* temp node holding subexpression results */
	PLAN *node;		/* pointer to returned node or expression */
	PLAN *tail;		/* pointer to tail of subplan */
	PLAN *subplan;		/* pointer to head of ( ) expression */
#if !WIN_NT
	int f_expr();
#endif
    
	/* first pull the top node from the plan */
	if ((node = yanknode(planp)) == NULL)
		return(NULL);
    
	/*
	 * If the node is an '(' then we recursively slurp up expressions
	 * until we find its associated ')'.  If it's a closing paren we
	 * just return it and unwind our recursion; all other nodes are
	 * complete expressions, so just return them.
	 */
	if (node->type == N_OPENPAREN)
		for (tail = subplan = NULL;;) {
			if ((next = yankexpr(planp)) == NULL)
				err("%s: %s", "(", "missing closing ')'");
			/*
			 * If we find a closing ')' we store the collected
			 * subplan in our '(' node and convert the node to
			 * a N_EXPR.  The ')' we found is ignored.  Otherwise,
			 * we just continue to add whatever we get to our
			 * subplan.
			 */
			if (next->type == N_CLOSEPAREN) {
				if (subplan == NULL)
					err("%s: %s",
					    "()", "empty inner expression");
				node->p_data[0] = subplan;
				node->type = N_EXPR;
				node->eval = f_expr;
				break;
			} else {
				if (subplan == NULL)
					tail = subplan = next;
				else {
					tail->next = next;
					tail = next;
				}
				tail->next = NULL;
			}
		}
	return(node);
}
 
/*
 * paren_squish --
 *	replaces "parentheisized" plans in our search plan with "expr" nodes.
 */
PLAN *
#if __STDC__
paren_squish (PLAN *plan)
#else
paren_squish(plan)
	PLAN *plan;		/* plan with ( ) nodes */
#endif
{
	register PLAN *expr;	/* pointer to next expression */
	register PLAN *tail;	/* pointer to tail of result plan */
	PLAN *result;		/* pointer to head of result plan */
    
	result = tail = NULL;

	/*
	 * the basic idea is to have yankexpr do all our work and just
	 * collect it's results together.
	 */
	while ((expr = yankexpr(&plan)) != NULL) {
		/*
		 * if we find an unclaimed ')' it means there is a missing
		 * '(' someplace.
		 */
		if (expr->type == N_CLOSEPAREN)
			err("%s: %s", ")", "no beginning '('");

		/* add the expression to our result plan */
		if (result == NULL)
			tail = result = expr;
		else {
			tail->next = expr;
			tail = expr;
		}
		tail->next = NULL;
	}
	return(result);
}
 
/*
 * not_squish --
 *	compresses "!" expressions in our search plan.
 */
PLAN *
#if __STDC__
not_squish (PLAN *plan)
#else
not_squish(plan)
	PLAN *plan;		/* plan to process */
#endif
{
	register PLAN *next;	/* next node being processed */
	register PLAN *node;	/* temporary node used in N_NOT processing */
	register PLAN *tail;	/* pointer to tail of result plan */
	PLAN *result;		/* pointer to head of result plan */
    
	tail = result = next = NULL;
    
	while ((next = yanknode(&plan)) != NULL) {
		/*
		 * if we encounter a ( expression ) then look for nots in
		 * the expr subplan.
		 */
		if (next->type == N_EXPR)
			next->p_data[0] = not_squish(next->p_data[0]);

		/*
		 * if we encounter a not, then snag the next node and place
		 * it in the not's subplan.  As an optimization we compress
		 * several not's to zero or one not.
		 */
		if (next->type == N_NOT) {
			int notlevel = 1;

			node = yanknode(&plan);
			while (node->type == N_NOT) {
				++notlevel;
				node = yanknode(&plan);
			}
			if (node == NULL)
				err("%s: %s", "!", "no following expression");
			if (node->type == N_OR)
				err("%s: %s", "!", "nothing between ! and -o");
			if (notlevel % 2 != 1)
				next = node;
			else
				next->p_data[0] = node;
		}

		/* add the node to our result plan */
		if (result == NULL)
			tail = result = next;
		else {
			tail->next = next;
			tail = next;
		}
		tail->next = NULL;
	}
	return(result);
}
 
/*
 * or_squish --
 *	compresses -o expressions in our search plan.
 */
PLAN *
#if __STDC__
or_squish (PLAN *plan)
#else
or_squish(plan)
	PLAN *plan;		/* plan with ors to be squished */
#endif
{
	register PLAN *next;	/* next node being processed */
	register PLAN *tail;	/* pointer to tail of result plan */
	PLAN *result;		/* pointer to head of result plan */
    
	tail = result = next = NULL;
    
	while ((next = yanknode(&plan)) != NULL) {
		/*
		 * if we encounter a ( expression ) then look for or's in
		 * the expr subplan.
		 */
		if (next->type == N_EXPR)
			next->p_data[0] = or_squish(next->p_data[0]);

		/* if we encounter a not then look for not's in the subplan */
		if (next->type == N_NOT)
			next->p_data[0] = or_squish(next->p_data[0]);

		/*
		 * if we encounter an or, then place our collected plan in the
		 * or's first subplan and then recursively collect the
		 * remaining stuff into the second subplan and return the or.
		 */
		if (next->type == N_OR) {
			if (result == NULL)
				err("%s: %s", "-o", "no expression before -o");
			next->p_data[0] = result;
			next->p_data[1] = or_squish(plan);
			if (next->p_data[1] == NULL)
				err("%s: %s", "-o", "no expression after -o");
			return(next);
		}

		/* add the node to our result plan */
		if (result == NULL)
			tail = result = next;
		else {
			tail->next = next;
			tail = next;
		}
		tail->next = NULL;
	}
	return(result);
}
