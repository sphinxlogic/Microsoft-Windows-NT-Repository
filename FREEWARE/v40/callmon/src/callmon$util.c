/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$UTIL.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: CALLMON Utility Routines.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  This routine allocates memory. Exit application in case of error.
 */

void* callmon$$alloc (size_t size)
{
    void* ptr;
    int32 alloc_size = size;
    uint32 status = lib$get_vm (&alloc_size, &ptr);

    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_GETVM, 1, alloc_size, status);
        sys$exit (status | STS$M_INHIB_MSG);
    }

    return ptr;
}


/*******************************************************************************
 *
 *  This routine frees memory previously allocated by callmon$$alloc.
 *  Returns a status code.
 */

uint32 callmon$$free (void* ptr, size_t size)
{
    int32 alloc_size = size;
    return lib$free_vm (&alloc_size, &ptr);
}


/*******************************************************************************
 *
 *  This routine prints a status array using a variable argument list.
 */

void callmon$$putmsg (uint32 status, ...)
{
    int count;
    va_list ap;
    uint32 vector [MAX_ARG + 1];
    uint32* v = vector;

    va_count (count);
    if (count > MAX_ARG)
        count = MAX_ARG;

    if (count > 0) {
        *v++ = count;
        *v++ = status;
        va_start (ap, status);
        while (--count > 0)
            *v++ = va_arg (ap, uint32);
        va_end (ap);
        sys$putmsg (vector, 0, 0, 0);
    }
}


/*******************************************************************************
 *
 *  This routine returns the number of translations of a logical name.
 *  If table is NULL, LNM$FILE_DEV is used.
 *  In case of error, returns zero.
 */

int callmon$$translation_count (char* name, char* table)
{
    uint32 status;
    int32 max_index;
    desc_t name_d, table_d;
    item_t* itm;
    item_t itmlst [5];

    set_str_desc (&name_d, name);
    set_str_desc (&table_d, table != NULL ? table : "LNM$FILE_DEV");

    itm = itmlst;
    set_data_item (itm++, LNM$_MAX_INDEX, max_index);
    set_final_item (itm++);

    status = sys$trnlnm (0, &table_d, &name_d, 0, itmlst);

    return $VMS_STATUS_SUCCESS (status) ? max_index + 1 : 0;
}


/*******************************************************************************
 *
 *  This routine translates a logical name.
 *  If table is NULL, LNM$FILE_DEV is used.
 *  In case of error, returns a status code and set value to "".
 */

uint32 callmon$$translate (
    char*  name,
    char*  value,
    size_t size,
    char*  table,
    int    index)
{
    uint32 status;
    uint16 length;
    uint32 index_val;
    desc_t name_d, table_d;
    item_t* itm;
    item_t itmlst [5];

    set_str_desc (&name_d, name);
    set_str_desc (&table_d, table != NULL ? table : "LNM$FILE_DEV");
    index_val = index;

    itm = itmlst;
    set_data_item (itm++, LNM$_INDEX, index_val);
    set_item (itm++, LNM$_STRING, value, size - 1, &length);
    set_final_item (itm++);

    status = sys$trnlnm (0, &table_d, &name_d, 0, itmlst);

    value [$VMS_STATUS_SUCCESS (status) ? length : 0] = '\0';

    return status;
}


/*******************************************************************************
 *
 *  This routine translates a search list and return an allocated
 *  array of nul-terminated string. The end of the pointer array
 *  is NULL.
 */

char** callmon$$search_list (char* name, char* table)
{
    uint32 status;
    int count;
    int index;
    char* i_table;
    char** list;
    char value [256];

    if ((count = callmon$$translation_count (name, table)) <= 0)
        return NULL;

    list = callmon$$alloc ((count + 1) * sizeof (char*));

    for (index = 0; index < count; index++) {

        status = callmon$$translate (name, value, sizeof (value), table, index);

        if (!$VMS_STATUS_SUCCESS (status))
            list [index] = "";
        else {
            list [index] = callmon$$alloc (strlen (value) + 1);
            strcpy (list [index], value);
        }
    }

    list [count] = NULL;
    return list;
}


/*******************************************************************************
 *
 *  This routine returns the page size of the system.
 *  Exit image on error (should not occur...).
 */

uint32 callmon$$page_size (void)
{
    uint32 status;
    uint32 size;
    iosb_t iosb;
    item_t itmlst [5];
    item_t* itm = itmlst;

    set_data_item (itm++, SYI$_PAGE_SIZE, size);
    set_final_item (itm++);

    status = sys$getsyiw (0, 0, 0, itmlst, &iosb, 0, 0);

    if ($VMS_STATUS_SUCCESS (status))
        status = iosb.status;
    if (!$VMS_STATUS_SUCCESS (status)) {
        callmon$$putmsg (CALLMON$_GETSYI, 0, status);
        sys$exit (status | STS$M_INHIB_MSG);
    }

    return size;
}


/*******************************************************************************
 *
 *  This routine is an "allocation routine" for LIB$INSERT_TREE on any
 *  kind of tree. It assumes that the structure has already been allocated
 *  and initialized by the caller. The address of the pre-allocated structure
 *  must be passed in the user_data argument.
 */

uint32 callmon$$tree_allocate_from_user_data (
    void*  symbol,
    void** node,
    void*  user_data)
{
    *node = user_data;
    return SS$_NORMAL;
}


/*******************************************************************************
 *
 *  This routine looks up a name/value table and search the specified name.
 *  If the name is found, its value is returned, otherwise return the
 *  default value. The last element of the table must have a NULL name.
 */

int32 callmon$$name_to_value (name_value_t* table, char* name, int32 defvalue)
{
    name_value_t* p;

    for (p = table; p->name != NULL; p++)
        if (strcmp (name, p->name) == 0)
            return p->value;

    return defvalue;
}


/*******************************************************************************
 *
 *  This routine disables any kind of threading and saves the previous state.
 */

uint32 callmon$$disable_threading (thread_state_t* state)
{
    /* Disable AST delivery and save previous state */

    state->ast_state = sys$setast (0);

    /* After OpenVMS V7.0, should also disable upcalls and kernel threads.
     * But I don't know how to do that... */
     
    return SS$_NORMAL;
}


/*******************************************************************************
 *
 *  This routine restores the previous threading state.
 */

uint32 callmon$$restore_threading (thread_state_t* state)
{
    sys$setast (state->ast_state == SS$_WASSET ? 1 : 0);
    return SS$_NORMAL;
}
