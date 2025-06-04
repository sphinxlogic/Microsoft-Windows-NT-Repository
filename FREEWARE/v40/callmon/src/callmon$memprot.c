/*  CALLMON - A Call Monitor for OpenVMS Alpha
 *
 *  File:     CALLMON$MEMPROT.C
 *  Author:   Thierry Lelegard
 *  Version:  1.0
 *  Date:     24-JUL-1996
 *
 *  Abstract: This module deals with memory protection.
 */


#include "callmon$private.h"


/*******************************************************************************
 *
 *  This routine unprotects a memeory range for write access.
 *  Return 1 on success, 0 on error.
 */

int callmon$$writeable (void* addr, size_t size)
{
    uint32 status;
    range_t page;
    char buffer [MAX_ALPHA_PAGE];

    /* If already writeable, return */

    if (__PAL_PROBEW (addr, size, PSL$C_USER))
        return 1;

    /* Unprotect each page */

    page.base = (char*) ((long)addr & ~(get_page_size () - 1));
    page.end = page.base + get_page_size () - 1;

    do {

        /* First, attempt to simply change the page protection */

        status = sys$setprt (&page, 0, PSL$C_USER, PRT$C_UW, 0);

        if (callmon$$own.trace_flags & TRACE_MEMORY)
            callmon$$putmsg (CALLMON$_SETPAGWRT, 1, page.base, status);

        /* If page not owned, replace it with a new one */

        if (status == SS$_NOPRIV || status == SS$_PAGOWNVIO) {

            /* Save content of the page */

            memcpy (buffer, page.base, get_page_size ());

            /* Delete and recreate the page. Don't make any RTL call
             * (like memcpy) here! Maybe we are replacing it. */

            status = sys$deltva (&page, 0, 0);

            if ($VMS_STATUS_SUCCESS (status)) {

                status = sys$cretva (&page, 0, 0);

                if (!$VMS_STATUS_SUCCESS (status)) {

                    /* If we reach this point, we are in a pretty bad state:
                     * a code page has been deleted but cannot be recreated.
                     * No other solution than abort. */

                    callmon$$putmsg (CALLMON$_CRETVA, 0, status);
                    sys$exit (status | STS$M_INHIB_MSG);
                }
                else {
                    int64* end = (int64*) (page.end + 1);
                    int64* p1 = (int64*) page.base;
                    int64* p2 = (int64*) buffer;
                    while (p1 < end)
                        *p1++ = *p2++;
                }
            }
        }

        /* If still no luck, report error */

        if (!$VMS_STATUS_SUCCESS (status)) {
            callmon$$putmsg (CALLMON$_MEMPROT, 2, page.base, page.end, status);
            return 0;
        }

        /* Loop on next page */

        page.base += get_page_size ();
        page.end += get_page_size ();

    } while (page.base < (char*)addr + size);

    return 1;
}
