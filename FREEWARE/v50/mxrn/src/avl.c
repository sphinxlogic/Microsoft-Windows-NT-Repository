
#if !defined(lint) && !defined(SABER)
static char XRNrcsid[] = "$Header: /net/objy27/wrld/mnt11/ricks/src/master/xrn/avl.c,v 1.5 1993/01/11 02:14:13 ricks Exp $";
#endif

/*
 * avl package
 *
 * Copyright (c) 1988-1993, The Regents of the University of California.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "copyright.h"
#include "config.h"
#ifndef VMS
#include <X11/Intrinsic.h>
#else
#include <decw$include/Intrinsic.h>
#endif
#include <stdio.h>
#include "utils.h"
#include "avl.h"
#include <assert.h>

#ifdef ALLOC
#undef ALLOC
#endif
#ifdef NIL
#undef NIL
#endif
#ifdef FREE
#undef FREE
#endif
#define ALLOC(type, number)  (type *) XtMalloc((unsigned) sizeof(type) * number)
#define NIL(type)            (type *) 0
#define FREE(item)	     (void) XtFree((void *)item)

#define XRNMAX(a,b)             ((a) > (b) ? (a) : (b))

/* LINTLIBRARY */


#define HEIGHT(node) (node == NIL(avl_node) ? -1 : (node)->height)
#define BALANCE(node) (HEIGHT((node)->right) - HEIGHT((node)->left))

#define compute_height(node) {				\
    int x=HEIGHT(node->left), y=HEIGHT(node->right);	\
    (node)->height = XRNMAX(x,y) + 1;			\
}

#define COMPARE(key, nodekey, compare)	 		\
    ((compare == avl_numcmp) ? 				\
	(int) key - (int) nodekey : 			\
	(*compare)(key, nodekey))

static avl_node *new_node();
static avl_node *find_rightmost();
static void do_rebalance(); 
static void rotate_left();
static void rotate_right();
static int do_check_tree();


avl_tree * avl_init_table
#if defined(__STDC__) && __STDC__
(int (*compar)(const char *,const char *))
#else
(compar)
    int (*compar)();
#endif
{
    avl_tree *tree;

    tree = ALLOC(avl_tree, 1);
    if (! tree) {
	 return 0;
    }
    tree->root = NIL(avl_node);
    tree->compar = compar;
    tree->num_entries = 0;
    return tree;
}

int avl_lookup
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree, register char *key, char **value_p)
#else
(tree, key, value_p)
    avl_tree *tree;
    register char *key;
    char **value_p;
#endif
{
    register avl_node *node;
    register int (*compare)() = tree->compar, diff;

    if (key == NIL(char) || value_p == NIL(char *))
	return 0;
    node = tree->root;
    while (node != NIL(avl_node)) {
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) {
	    /* got a match, give the user a 'value' only if non-null */
	    if (value_p != NIL(char *)) *value_p = node->value;
	    return 1;
	}
	node = (diff < 0) ? node->left : node->right;
    }
    return 0;
}

int avl_insert
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree, char *key, char *value)
#else
(tree, key, value)
    avl_tree *tree;
    char *key;
    char *value;
#endif
{
    register avl_node **node_p, *node;
    register int stack_n = 0;
    register int (*compare)() = tree->compar;
    avl_node **stack_nodep[32];
    int diff, status;

    node_p = &tree->root;

    /* walk down the tree (saving the path); stop at insertion point */
    status = 0;
    while ((node = *node_p) != NIL(avl_node)) {
	stack_nodep[stack_n++] = node_p;
	assert(stack_n < 32);
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) status = 1;
	node_p = (diff < 0) ? &node->left : &node->right;
    }

    /* insert the item and re-balance the tree */
    *node_p = new_node(key, value);
    if (! *node_p) {
	 return -1;
    }
    do_rebalance(stack_nodep, stack_n);
    tree->num_entries++;
    tree->modified = 1;
    return status;
}
/*
 * Insert a node with the key string being the value.
 * Assumes a static key input.
 */

int avl_insert_str
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree, char *nkey)
#else
(tree, nkey)
    avl_tree *tree;
    char *nkey;
#endif
{
    register avl_node **node_p, *node;
    register int stack_n = 0;
    register int (*compare)() = tree->compar;
    avl_node **stack_nodep[32];
    int diff, status;
    char *key;

    key = (char *) XtNewString(nkey);
    node_p = &tree->root;

    /* walk down the tree (saving the path); stop at insertion point */
    status = 0;
    while ((node = *node_p) != NIL(avl_node)) {
	stack_nodep[stack_n++] = node_p;
	assert(stack_n < 32);
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) status = 1;
	node_p = (diff < 0) ? &node->left : &node->right;
    }

    /* insert the item and re-balance the tree */
    *node_p = new_node(key, key);
    if (! *node_p) {
	 return -1;
    }
    do_rebalance(stack_nodep, stack_n);
    tree->num_entries++;
    tree->modified = 1;
    return status;
}

int avl_delete
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree, char **key_p, char **value_p)
#else
(tree, key_p, value_p)
    avl_tree *tree;
    char **key_p;
    char **value_p;
#endif
{
    register avl_node **node_p, *node, *rightmost;
    register int stack_n = 0;
    char *key = *key_p;
    int (*compare)() = tree->compar, diff;
    avl_node **stack_nodep[32];
    
    node_p = &tree->root;

    /* Walk down the tree saving the path; return if not found */
    while ((node = *node_p) != NIL(avl_node)) {
	diff = COMPARE(key, node->key, compare);
	if (diff == 0) goto delete_item;
	stack_nodep[stack_n++] = node_p;
	assert(stack_n < 32);
	node_p = (diff < 0) ? &node->left : &node->right;
    }
    return 0;		/* not found */

    /* prepare to delete node and replace it with rightmost of left tree */
delete_item:
    *key_p = node->key;
    if (value_p != 0) *value_p = node->value;
    if (node->left == NIL(avl_node)) {
	*node_p = node->right;
    } else {
	rightmost = find_rightmost(&node->left);
	rightmost->left = node->left;
	rightmost->right = node->right;
	rightmost->height = -2; 	/* mark bogus height for do_rebal */
	*node_p = rightmost;
	stack_nodep[stack_n++] = node_p;
    }
    FREE(node);

    /* work our way back up, re-balancing the tree */
    do_rebalance(stack_nodep, stack_n);
    tree->num_entries--;
    tree->modified = 1;
    return 1;
}

static void avl_record_gen_forward
#if defined(STDC) && !defined(_NO_PROTO)
(avl_node *node, avl_generator *gen)
#else
(node, gen)
    avl_node *node;
    avl_generator *gen;
#endif
{
    if (node != NIL(avl_node)) {
	avl_record_gen_forward(node->left, gen);
	gen->nodelist[gen->count++] = node;
	avl_record_gen_forward(node->right, gen);
    }
}

static void avl_record_gen_backward
#if defined(STDC) && !defined(_NO_PROTO)
(avl_node *node, avl_generator *gen)
#else
(node, gen)
    avl_node *node;
    avl_generator *gen;
#endif
{
    if (node != NIL(avl_node)) {
	avl_record_gen_backward(node->right, gen);
	gen->nodelist[gen->count++] = node;
	avl_record_gen_backward(node->left, gen);
    }
}

avl_generator * avl_init_gen
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree, int dir)
#else
(tree, dir)
    avl_tree *tree;
    int dir;
#endif
{
    avl_generator *gen;

    /* what a hack */
    gen = ALLOC(avl_generator, 1);
    if (! gen) {
	 return 0;
    }
    gen->tree = tree;
    gen->nodelist = ALLOC(avl_node *, avl_count(tree));
    if (! gen->nodelist) {
	 return 0;
    }
    gen->count = 0;
    if (dir == AVL_FORWARD) {
	avl_record_gen_forward(tree->root, gen);
    } else {
	avl_record_gen_backward(tree->root, gen);
    }
    gen->count = 0;

    /* catch any attempt to modify the tree while we generate */
    tree->modified = 0;
    return gen;
}

int avl_gen
#if defined(STDC) && !defined(_NO_PROTO)
(avl_generator *gen, char **key_p, char **value_p)
#else
(gen, key_p, value_p)
    avl_generator *gen;
    char **key_p;
    char **value_p;
#endif
{
    avl_node *node;

    if (gen->count == gen->tree->num_entries) {
	return 0;
    } else {
	node = gen->nodelist[gen->count++];
	if (key_p != NIL(char *)) *key_p = node->key;
	if (value_p != NIL(char *)) *value_p = node->value;
	return 1;
    }
}

void avl_free_gen
#if defined(STDC) && !defined(_NO_PROTO)
(avl_generator *gen)
#else
(gen)
    avl_generator *gen;
#endif
{
    FREE(gen->nodelist);
    FREE(gen);
}

static avl_node * find_rightmost
#if defined(STDC) && !defined(_NO_PROTO)
(register avl_node **node_p)
#else
(node_p)
    register avl_node **node_p;
#endif
{
    register avl_node *node;
    register int stack_n = 0;
    avl_node **stack_nodep[32];

    node = *node_p;
    while (node->right != NIL(avl_node)) {
	stack_nodep[stack_n++] = node_p;
	assert(stack_n < 32);
	node_p = &node->right;
	node = *node_p;
    }
    *node_p = node->left;

    do_rebalance(stack_nodep, stack_n);
    return node;
}

static void do_rebalance
#if defined(STDC) && !defined(_NO_PROTO)
(register avl_node ***stack_nodep, register int stack_n)
#else
(stack_nodep, stack_n)
    register avl_node ***stack_nodep;
    register int stack_n;
#endif
{
    register avl_node **node_p, *node;
    register int hl, hr;
    int height;

    /* work our way back up, re-balancing the tree */
    while (--stack_n >= 0) {
	node_p = stack_nodep[stack_n];
	node = *node_p;
	hl = HEIGHT(node->left);		/* watch for NIL */
	hr = HEIGHT(node->right);		/* watch for NIL */
	if ((hr - hl) < -1) {
	    rotate_right(node_p);
	} else if ((hr - hl) > 1) {
	    rotate_left(node_p);
	} else {
	    height = XRNMAX(hl, hr) + 1;
	    if (height == node->height) break;
	    node->height = height;
	}
    }
}

static void rotate_left
#if defined(STDC) && !defined(_NO_PROTO)
(register avl_node **node_p)
#else
(node_p)
    register avl_node **node_p;
#endif
{
    register avl_node *old_root = *node_p, *new_root, *new_right;

    if (BALANCE(old_root->right) >= 0) {
	*node_p = new_root = old_root->right;
	old_root->right = new_root->left;
	new_root->left = old_root;
    } else {
	new_right = old_root->right;
	*node_p = new_root = new_right->left;
	old_root->right = new_root->left;
	new_right->left = new_root->right;
	new_root->right = new_right;
	new_root->left = old_root;
	compute_height(new_right);
    }
    compute_height(old_root);
    compute_height(new_root);
}

static void rotate_right
#if defined(STDC) && !defined(_NO_PROTO)
(avl_node **node_p)
#else
(node_p)
    avl_node **node_p;
#endif
{
    register avl_node *old_root = *node_p, *new_root, *new_left;

    if (BALANCE(old_root->left) <= 0) {
	*node_p = new_root = old_root->left;
	old_root->left = new_root->right;
	new_root->right = old_root;
    } else {
	new_left = old_root->left;
	*node_p = new_root = new_left->right;
	old_root->left = new_root->right;
	new_left->right = new_root->left;
	new_root->left = new_left;
	new_root->right = old_root;
	compute_height(new_left);
    }
    compute_height(old_root);
    compute_height(new_root);
}

static void avl_walk_forward
#if defined(STDC) && !defined(_NO_PROTO)
(avl_node *node, void (*func)(char *,char *))
#else
(node, func)
    avl_node *node;
    void (*func)();
#endif
{
    if (node != NIL(avl_node)) {
	avl_walk_forward(node->left, func);
	(*func)(node->key, node->value);
	avl_walk_forward(node->right, func);
    }
}

static void avl_walk_backward
#if defined(STDC) && !defined(_NO_PROTO)
(avl_node *node, void (*func)(char *,char *))
#else
(node, func)
    avl_node *node;
    void (*func)();
#endif
{
    if (node != NIL(avl_node)) {
	avl_walk_backward(node->right, func);
	(*func)(node->key, node->value);
	avl_walk_backward(node->left, func);
    }
}

void avl_foreach
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree, void (*func)(char *,char *), int direction)
#else
(tree, func, direction)
    avl_tree *tree;
    void (*func)();
    int direction;
#endif
{
    if (direction == AVL_FORWARD) {
	avl_walk_forward(tree->root, func);
    } else {
	avl_walk_backward(tree->root, func);
    }
}

static void free_entry
#if defined(STDC) && !defined(_NO_PROTO)
(avl_node *node, void (*key_free)(char *), void (*value_free)(char *))
#else
(node, key_free, value_free)
    avl_node *node;
    void (*key_free)();
    void (*value_free)();
#endif
{
    if (node != NIL(avl_node)) {
	free_entry(node->left, key_free, value_free);
	free_entry(node->right, key_free, value_free);
	if (key_free != 0) (*key_free)(node->key);
	if (value_free != 0) (*value_free)(node->value);
	FREE(node);
    }
}
    
void avl_free_table
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree, void (*key_free)(char *), void (*value_free)(char *))
#else
(tree, key_free, value_free)
    avl_tree *tree;
    void (*key_free)();
    void (*value_free)();
#endif
{
    free_entry(tree->root, key_free, value_free);
    FREE(tree);
}


int avl_count
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree)
#else
(tree)
    avl_tree *tree;
#endif
{
    return tree->num_entries;
}

static avl_node * new_node
#if defined(STDC) && !defined(_NO_PROTO)
(char *key, char *value)
#else
(key, value)
    char *key;
    char *value;
#endif
{
    register avl_node *new;

    new = ALLOC(avl_node, 1);
    if (! new) {
	 return 0;
    }
    new->key = key;
    new->value = value;
    new->height = 0;
    new->left = new->right = NIL(avl_node);
    return new;
}

int avl_numcmp
#if defined(STDC) && !defined(_NO_PROTO)
(char *x, char *y)
#else
(x, y)
    char *x, *y; 
#endif
{
    return (int) x - (int) y;
}

int avl_check_tree
#if defined(STDC) && !defined(_NO_PROTO)
(avl_tree *tree)
#else
(tree)
    avl_tree *tree;
#endif
{
    int error = 0;
    (void) do_check_tree(tree->root, tree->compar, &error);
    return error;
}

static int do_check_tree
#if defined(STDC) && !defined(_NO_PROTO)
(avl_node *node, int (*compar)(char *,char *), int *error)
#else
(node, compar, error)
    avl_node *node;
    int (*compar)();
    int *error;
#endif
{
    int l_height, r_height, comp_height, bal;
    
    if (node == NIL(avl_node)) {
	return -1;
    }

    r_height = do_check_tree(node->right, compar, error);
    l_height = do_check_tree(node->left, compar, error);

    comp_height = XRNMAX(l_height, r_height) + 1;
    bal = r_height - l_height;
    
    if (comp_height != node->height) {
	(void) printf("Bad height for 0x%08x: computed=%d stored=%d\n",
	    node, comp_height, node->height);
	++*error;
    }

    if (bal > 1 || bal < -1) {
	(void) printf("Out of balance at node 0x%08x, balance = %d\n", 
	    node, bal);
	++*error;
    }

    if (node->left != NIL(avl_node) && 
		    (*compar)(node->left->key, node->key) > 0) {
	(void) printf("Bad ordering between 0x%08x and 0x%08x", 
	    node, node->left);
	++*error;
    }
    
    if (node->right != NIL(avl_node) && 
		    (*compar)(node->key, node->right->key) > 0) {
	(void) printf("Bad ordering between 0x%08x and 0x%08x", 
	    node, node->right);
	++*error;
    }

    return comp_height;
}
