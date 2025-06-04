/* Cache.c v1.2   Caching control routines */

/*
        This is part of ODS2 written by Paul Nankervis,
        email address:  Paulnank@au1.ibm.com

        ODS2 is distributed freely for all members of the
        VMS community to use. However all derived works
        must maintain comments in their source to acknowledge
        the contibution of the original author.
*/

/*  Caching seems to have a big impact on performance!!!
    This version based on number of objects - probably
    should change it to use space occupied by objects.
    Currently use vanilla binary trees - could either
    use hashing or balanced trees for better performance.  */

/*  The theory is that all cachable objects share a common
    cache pool. Any object with a reference count of zero
    is a candidate for destruction. All cacheable objects
    have a 'struct CACHE' as the first item of the object
    so that cache pointers and object pointers are interchangeable.
    All cache objects are also part of a binary tree so that
    they can be located. There are many instances of these
    binary trees: for files on a volume, for chunks (segments)
    of files, etc. Also each object can have an object manager:
    a routine to handle special object deletion requirements
    (for example to remove all file chunks before removing a
    file), or to flush objects (write modified chunks to disk).  */


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ssdef.h"
#include "cache.h"


#define DEBUG on

/* Set limits for number of unaccessed cache entries... */

#define CACHELIM  64
#define CACHEGOAL 25

int cachesearches = 0;
int cachecreated = 0;
int cachedeleted = 0;
int cachepeak = 0;
int cachecount = 0;
int cachefreecount = 0;

struct CACHE cachelist = {NULL,NULL,NULL,&cachelist,&cachelist,NULL,0,0,1};


/* cacheshow - to print cache statistics */

void cacheshow(void)
{
    printf("CACHESHOW Searches: %d Created: %d Peak: %d Count: %d Free: %d\n",
           cachesearches,cachecreated,cachepeak,cachecount,cachefreecount);
}


void cachedump(void)
{
    register struct CACHE *cacheobj = cachelist.lstcache;
    printf("%8.8s %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s cachelist\n","Object",
           "Parent","Left","Right","Value","Status","Count");
    while (cacheobj != &cachelist) {
        printf("%8x %8x %8x %8x %8x %8x %8d\n",
               cacheobj,cacheobj->parent,cacheobj->left,cacheobj->right,
               cacheobj->keyval,cacheobj->status,cacheobj->refcount);
        cacheobj = cacheobj->lstcache;
    }
}


/* cacheprint - debugging tool to print out a cache subtree... */

void cacheprint(struct CACHE *cacheobj,int level)
{
    if (cacheobj != NULL) {
        int i;
        cacheprint(cacheobj->left,level + 1);
        for (i = 0; i < level; i++) printf("    ");
        printf("%8x %8x %8x %d\n",
               cacheobj,cacheobj->keyval,cacheobj->status,cacheobj->refcount);
        cacheprint(cacheobj->right,level + 1);
    }
}


/* cacherefcount - compute reference count for cache subtree... */

int cacherefcount(struct CACHE *cacheobj)
{
    register int refcount = 0;
    if (cacheobj != NULL) {
        if (cacheobj->left != NULL) refcount += cacherefcount(cacheobj->left);
        if (cacheobj->right != NULL) refcount += cacherefcount(cacheobj->right);
        refcount += cacheobj->refcount;
    }
    return refcount;
}


/* cachefree - blow away item from cache - allow item to select a proxy (!)
   and adjust 'the tree' containing the item. */

int freeactive = 0;             /* In case object managers re-call cachefree... :-( */

struct CACHE *cachefree(struct CACHE *cacheobj)
{
    if (cacheobj->refcount != 0) {
        char str[100];
        printf("cachelist, object to delete still inuse?\n");
        printf("%8x %8x %8x %8x %8x %8x %8d\n",
               cacheobj,cacheobj->parent,cacheobj->left,cacheobj->right,
               cacheobj->keyval,cacheobj->status,cacheobj->refcount);
        printf("Type RETURN: ");
        gets(str);
        return NULL;
    } else {
        register struct CACHE *proxyobj;
        while (cacheobj->objmanager != NULL) {
            freeactive = 1;
            proxyobj = (*cacheobj->objmanager) (cacheobj);
            freeactive = 0;
            if (proxyobj == NULL) return NULL;
            if (proxyobj == cacheobj) break;
            cacheobj = proxyobj;
        }
        if ((cacheobj->status & CACHE_MODIFIED) && cacheobj->objmanager == NULL)
            printf("cachelist No manager to write modified cache\n");
        cacheobj->lstcache->nxtcache = cacheobj->nxtcache;
        cacheobj->nxtcache->lstcache = cacheobj->lstcache;
        if (cacheobj->parent != NULL) { /* Check if in tree... */
            if (cacheobj->left == NULL) {
                if (cacheobj->right == NULL) {
                    *(cacheobj->parent) = NULL;
                } else {
                    cacheobj->right->parent = cacheobj->parent;
                    *(cacheobj->parent) = cacheobj->right;
                }
            } else {
                if (cacheobj->right == NULL) {
                    cacheobj->left->parent = cacheobj->parent;
                    *(cacheobj->parent) = cacheobj->left;
                } else {
                    register struct CACHE *leftpath = cacheobj->left;
                    register struct CACHE *rightpath = cacheobj->right;
                    while (1) {
                        if (leftpath->right == NULL) {
                            leftpath->right = cacheobj->right;
                            cacheobj->right->parent = &leftpath->right;
                            cacheobj->left->parent = cacheobj->parent;
                            *(cacheobj->parent) = cacheobj->left;
                            break;
                        } else {
                            if (rightpath->left == NULL) {
                                rightpath->left = cacheobj->left;
                                cacheobj->left->parent = &rightpath->left;
                                cacheobj->right->parent = cacheobj->parent;
                                *(cacheobj->parent) = cacheobj->right;
                                break;
                            } else {
                                leftpath = leftpath->right;
                                rightpath = rightpath->left;
                            }
                        }
                    }
                }
            }
        }
        if (--cachecount < 0) printf("cachelist, cache current too small\n");
        cachefreecount--;
        cachedeleted--;
#ifdef DEBUG
        cacheobj->parent = NULL;
        cacheobj->left = NULL;
        cacheobj->right = NULL;
        cacheobj->nxtcache = NULL;
        cacheobj->lstcache = NULL;
        cacheobj->objmanager = NULL;
        cacheobj->keyval = 0;
        cacheobj->status = 0;
        cacheobj->refcount = 0;
#endif
        free(cacheobj);
        return cacheobj;
    }
}


/* cachepurge - trim size of free list */

void cachepurge()
{
    register struct CACHE *cacheobj = cachelist.lstcache;
    while (cachefreecount > CACHEGOAL && cacheobj != &cachelist) {
        register struct CACHE *lastobj = cacheobj->lstcache;
#ifdef DEBUG
        if (cacheobj->lstcache->nxtcache != cacheobj ||
            cacheobj->nxtcache->lstcache != cacheobj ||
            *(cacheobj->parent) != cacheobj) {
            printf("CACHE pointers in bad shape! %x %x %x - %x\n",
                   cacheobj->lstcache->nxtcache,cacheobj,cacheobj->nxtcache->lstcache,*(cacheobj->parent));
        }
#endif
        if (cacheobj->refcount == 0) {
            if (cachefree(cacheobj) != lastobj) cacheobj = lastobj;
        } else {
            cacheobj = lastobj;
        }
    }
}



/* cacheflush - flush modified entries in cache */

void cacheflush()
{
    register struct CACHE *cacheobj = cachelist.lstcache;
    while (cacheobj != &cachelist) {
        if (cacheobj->status & CACHE_MODIFIED) {
            if (cacheobj->objmanager != NULL) {
                (*cacheobj->objmanager) (cacheobj);
            } else {
                printf("CACHEFLUSH No manager to write modified cache\n");
            }
        }
        cacheobj = cacheobj->lstcache;
    }
}


/* cachedelete - delete an object from cache */

struct CACHE *cachedelete(struct CACHE *cacheobj)
{
    if (cacheobj != NULL) {
        register struct CACHE *cachedel;
        do {
            cachedel = cachefree(cacheobj);
        } while (cachedel != NULL && cachedel != cacheobj);
    }
    return cacheobj;
}



/* cachedeltree: delete cache subtree */

void cachedeltree(struct CACHE *cacheobj)
{
    if (cacheobj != NULL) {
        if (cacheobj->left != NULL) cachedeltree(cacheobj->left);
        if (cacheobj->right != NULL) cachedeltree(cacheobj->right);
        if (cacheobj->refcount == 0) cachedelete(cacheobj);
    }
}




/* cacheuntouch - decrement object reference count */

unsigned cacheuntouch(struct CACHE *cacheobj,unsigned reuse,unsigned modflg)
{
    if (cacheobj->refcount < 1) {
        char str[100];
        printf("%8x %8x %8x %8x %8x %8x %8d\n",
               cacheobj,cacheobj->parent,cacheobj->left,cacheobj->right,
               cacheobj->keyval,cacheobj->status,cacheobj->refcount);
        printf("CACHE untouch TOO FAR!\n");
        printf("Type RETURN: ");
        gets(str);
        return 64;
    } else {
        if (modflg) {
            if ((cacheobj->status & CACHE_WRITE) == 0) return SS$_WRITLCK;
            cacheobj->status |= CACHE_MODIFIED;
        }
        if (--cacheobj->refcount == 0) {
            /* Move to new list position... */
            if (reuse == 0 && cacheobj != cachelist.lstcache) {
                cacheobj->lstcache->nxtcache = cacheobj->nxtcache;
                cacheobj->nxtcache->lstcache = cacheobj->lstcache;
                cacheobj->nxtcache = &cachelist;
                cacheobj->lstcache = cachelist.lstcache;
                cachelist.lstcache->nxtcache = cacheobj;
                cachelist.lstcache = cacheobj;
            }
            cacheobj->status &= ~CACHE_WRITE;
            if (++cachefreecount > CACHELIM && freeactive == 0) cachepurge();
        }
    }
    return 1;
}


/* cachetouch - add one to reference count */

void cachetouch(struct CACHE *cacheobj)
{
    if (cacheobj->refcount++ < 1) {
#ifdef DEBUG
        if (cacheobj->lstcache->nxtcache != cacheobj ||
            cacheobj->nxtcache->lstcache != cacheobj ||
            *(cacheobj->parent) != cacheobj) {
            printf("CACHE pointers in bad shape! %x %x %x - %x\n",
                   cacheobj->lstcache->nxtcache,cacheobj,cacheobj->nxtcache->lstcache,*(cacheobj->parent));
        }
#endif
        cachefreecount--;
    }
    /* Move object to head of list... */
    if (cacheobj != cachelist.nxtcache) {
        cacheobj->lstcache->nxtcache = cacheobj->nxtcache;
        cacheobj->nxtcache->lstcache = cacheobj->lstcache;
        cacheobj->lstcache = &cachelist;
        cacheobj->nxtcache = cachelist.nxtcache;
        cachelist.nxtcache->lstcache = cacheobj;
        cachelist.nxtcache = cacheobj;
    }
}


/* cachesearch - to find or create cache entries...

        The grand plan here was to use a hash code as a quick key
        and call a compare function for duplicates. So far no data
        type actually works like this - they either have a unique binary
        key, or all records rely on the compare function - sigh!
        Never mind, the potential is there!  :-)   */

void *cachesearch(void **root,unsigned keyval,unsigned keylen,void *key,
                  int (*cmpfunc) (unsigned keylen,void *key,void *node),
                  unsigned *createsize)
{
    register struct CACHE *parentobj = NULL;
    register struct CACHE *cacheobj,**parent = (struct CACHE **) root;
    cachesearches++;
    while ((cacheobj = *parent) != NULL) {
        parentobj = cacheobj;
        if (cacheobj->keyval == keyval) {
            register int cmp = 0;
            if (cmpfunc != NULL)
                cmp = (*cmpfunc) (keylen,key,(void *) cacheobj);
            if (cmp == 0) {
                cachetouch(cacheobj);
                return cacheobj;
            } else {
                if (cmp < 0) {
                    parent = &cacheobj->left;
                } else {
                    parent = &cacheobj->right;
                }
            }
        } else {
            if (cacheobj->keyval < keyval) {
                parent = &cacheobj->left;
            } else {
                parent = &cacheobj->right;
            }
        }
    }
    if (*createsize > sizeof(struct CACHE)) {
        cacheobj = (struct CACHE *) malloc(*createsize);
        if (cacheobj != NULL) {
            cacheobj->parent = parent;
            cacheobj->left = NULL;
            cacheobj->right = NULL;
            cacheobj->objmanager = NULL;
            cacheobj->keyval = keyval;
            cacheobj->status = 0;
            cacheobj->refcount = 1;
            *parent = cacheobj;
            *createsize = 0;
            cachecreated++;
            /* Add to cache list... */
            cacheobj->lstcache = &cachelist;
            cacheobj->nxtcache = cachelist.nxtcache;
            cachelist.nxtcache->lstcache = cacheobj;
            cachelist.nxtcache = cacheobj;
            if (parentobj != NULL) {
                /* Attempt to mix up the tree a little... */
                if (parentobj->left == NULL) {
                    *parentobj->parent = cacheobj;
                    cacheobj->parent = parentobj->parent;
                    parentobj->parent = &cacheobj->left;
                    cacheobj->left = parentobj;
                    parentobj->right = NULL;
                } else {
                    if (parentobj->right == NULL) {
                        *parentobj->parent = cacheobj;
                        cacheobj->parent = parentobj->parent;
                        parentobj->parent = &cacheobj->right;
                        cacheobj->right = parentobj;
                        parentobj->left = NULL;
                    }
                }
            }
            if (cachecount++ >= cachepeak) cachepeak = cachecount;
        }
    }
    return cacheobj;
}
