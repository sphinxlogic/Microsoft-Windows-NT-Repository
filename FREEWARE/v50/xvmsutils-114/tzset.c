/* Taken from the gawk distribution [.src.vms]vms_misc.c                      */
/* 960306 mpjz */

    /*
     * VMS has no timezone support.
     */

/* dummy to satisfy linker */
void tzset()
{
    return;
}
