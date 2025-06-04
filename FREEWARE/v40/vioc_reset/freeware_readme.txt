VIOC_RESET, SYSTEM_MANAGEMENT, Reset Virtual I/O Cache counters
 
The Virtual I/O Cache (VIOC) is a clusterwide, file-oriented disk
cache available to all supported configurations, from single-node
systems to large mixed-interconnect VMScluster systems.  Any base
system can use VIOC, no additional license is required. 

There are applications that cause the VIOC counters to be skewed,
resulting in an incorrect interpretation of the usefullness of the
VIOC.  By resetting the counters, the SHOW MEMORY/CACHE/FULL
display depicts a more accurate use of the VIOC.

The following discussion is relevant to the SHOW MEMORY/CACHE/FULL 
display:

    The BACKUP utility typically causes a large increase to the
    "Read IO Count" while incurring no read hits. Depending on
    the amount of data being backed up, the increase to "Read IO
    Count" can be drastic in a short span of time.  Since the
    "Read Hit Rate" remains somewhat flat for this same period of
    time, the calculated "Hit%" will be skewed.  The skewed
    values will remain until the system is rebooted, or until
    enough time and I/Os, with an equitable hit rate, occur. 
    Typically, "IO Bypassing Cache" will increase in proportion
    to "Read IO Count".


Counters reset by this program:

CACHE$GL_VREAD,   the Read IO counter
CACHE$GL_READHIT, the Read Hit counter
CACHE$GL_VWRITE,  the Write IO counter
CACHE$GL_RRNDMOD, Read IO bypassing VIOC due to function
CACHE$GL_RRNDSIZ, Read IO bypassing VIOC due to size
CACHE$GL_WRNDMOD, Write IO bypassing VIOC due to function
CACHE$GL_WRNDSIZ, Write IO bypassing VIOC due to size

Submitted by: Reg Hunter, Customer Support Center, Colorado Springs
