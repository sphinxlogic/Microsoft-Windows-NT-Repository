/* +-------------------------------------------------------------------+ */
/* | Copyright 1992, David Koblas.                                     | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */

/*
**  A Simple useful set of hash routines:
**
**      void	*HashCreate(int (*cmp)(void *, void*), void (*free)(void *), int nelem)
**	  -- Create a hash table with cmp() function, and optional free() function.
**	void	 HashDestroy(HashTable *tbl)
**	  -- Destroy the whole hash table
**	void	*HashFind(HashTable *tbl, int value, void *val)
** 	  -- Find an element in the hash table, returns NULL if not found
**	int	 HashAdd(HashTable *tbl, int value, void *val)
**	  -- Add an element to the table, returns non-zero on failure
**	int	 HashRemove(HashTable *tbl, int value, void *elem)
**	  -- Remove a particular hash entry from the table, pointer compairson
**	int	 HashAll(HashTable *tbl, int (*func)(void *))
**	  -- Apply function to all elements of the table, until func() returns non-zero
*/


#include <stdio.h>
#if 0
#include <malloc.h>
#else
extern void free(void *);
extern void *malloc(unsigned int);
#endif

typedef struct hash_s {
	int		*val;
	struct hash_s	*left, *right, **parentp;
} HashElement;

typedef struct {
	int		(*cmp)(void *, void *);
	void		(*free)(void *);
	HashElement	**table;
	int		nelem;
} HashTable;

/*
**  Create a hashtable, with cmp() compare function, and free() free function
**       with a hash table width of nelem, free can be null in which case the
**       system free function will be used.
*/
void	*HashCreate(int (*cmp)(void *, void *), void (*free)(void *), int nelem)
{
	HashTable	*new;
	int		i;

	new = (HashTable*)malloc(sizeof(HashTable));
	new->nelem = nelem;
	new->cmp   = cmp;
	new->free  = free;
	new->table = (HashElement **)malloc(nelem * sizeof(HashElement*));

	for (i = 0; i < nelem; i++)
		new->table[i] = NULL;

	return (void*)new;
}

static void hashDestory(void (*func)(void *), HashElement *entry)
{
	if (entry->left != NULL) {
		hashDestory(func, entry->left);
		free(entry->left);
	}
	if (entry->right != NULL) {
		hashDestory(func, entry->right);
		free(entry->right);
	}

	if (entry->val)
		func(entry->val);
}

/*
**  Free the entire hash table, and all elements
*/
void	HashDestroy(HashTable *tbl)
{
	int		i;

	if (tbl == NULL)
		return;

	for (i = 0; i < tbl->nelem; i++) {
		if (tbl->table[i] != NULL) {
			hashDestory(tbl->free ? tbl->free : free, tbl->table[i]);
			free(tbl->table[i]);
		}
	}
	free(tbl);
}

/*
**  Find a particular element in the hash table, returns NULL if it isn't found
*/
void	*HashFind(HashTable *tbl, int value, void *val)
{
	HashElement	*cur;
	int		v;

	if (tbl == NULL)
		return NULL;

	cur = tbl->table[value];

	while (cur != NULL) {
		if ((v = tbl->cmp(cur->val, val)) == 0)
			return cur->val;

		if (v < 0)
			cur = cur->left;
		else
			cur = cur->right;
	}

	return NULL;
}

/*
**  And a new element to the has table, returns non-zero on failure
*/
int	HashAdd(HashTable *tbl, int value, void *val)
{
	HashElement	*new = (HashElement*)malloc(sizeof(HashElement));
	HashElement	*cur = tbl->table[value];
	HashElement	**prev;

	if (new == NULL || tbl == NULL)
		return 1;

	new->left    = NULL; 
	new->right   = NULL; 
	new->val     = val;
	new->parentp = NULL;

	prev = &tbl->table[value];
	while (cur != NULL) {
		if (tbl->cmp(cur->val, val) < 0) {
			prev = &cur->left;
			cur  = cur->left;
		} else {
			prev = &cur->right;
			cur  = cur->right;
		}
	}
	*prev = new;
	new->parentp = prev;

	return 0;
}

static HashElement 	*find(HashElement *pos, void *elem)
{
	HashElement	*v;

	if (pos == NULL)
		return NULL;

	if (pos->val == elem)
		return pos;
	if (pos->left != NULL && (v = find(pos->left, elem)) != NULL)
		return v;
	if (pos->right != NULL && (v = find(pos->right, elem)) != NULL)
		return v;
	return NULL;
}

/*
**  Remove a particular element from the hash table, done using pointer compares
*/
int	HashRemove(HashTable *tbl, int value, void *elem)
{
	HashElement	*v;

	if (elem == NULL || tbl == NULL)
		return 1;

	if ((v = find(tbl->table[value], elem)) == NULL) 
		return 0;

	if (v->left != NULL) {
		*v->parentp = v->left;
		v->left->parentp = v->parentp;
		if (v->right != NULL) {
			HashElement	*cur = v->left, **prev;
			while (cur != NULL) {
				if (tbl->cmp(cur->val, v->right->val) < 0) {
					prev = &cur->left;
					cur  =  cur->left;
				} else {
					prev = &cur->right;
					cur  =  cur->right;
				}
			}
			*prev = v->right;
			v->right->parentp = prev;
		}
	} else {
		/* no left side, just attach the right */
		*v->parentp = v->right;
		if (v->right != NULL)
			v->right->parentp = v->parentp;
	}
	free(v);

	return 1;
}

static int overAll(HashElement *elem, int (*func)(void *)) 
{
	if (elem == NULL)
		return 0;
	if (elem->val != NULL)
		func(elem->val);

	return overAll(elem->left, func) || overAll(elem->right, func);
}

int	HashAll(HashTable *tbl, int (*func)(void *))
{
	int	i;

	if (tbl == NULL)
		return 0;

	for (i = 0; i < tbl->nelem; i++)
		if (overAll(tbl->table[i], func))
			return 1;
	return 0;
}
