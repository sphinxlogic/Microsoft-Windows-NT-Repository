/*
 * Paths of the configuration files.
 * This file may be changed by the user as needed.
 * There are three empty lines between each definition.
 * These ensure that "local" patches and official patches have
 * only a very low probability of conflicting.
 */

#define CONF_FILE "mtools.conf"

#define OLD_CONF_FILE "mtools"

#define LOCAL_CONF_FILE "sys$login:mtools.conf"

#define OLD_LOCAL_CONF_FILE "sys$login:mtools"

/* Use this if you like to keep the configuration file in a non-standard
 * place such as /etc/default, /opt/etc, /usr/etc, /usr/local/etc ...
 */

#define SYS_CONF_FILE SYSCONFDIR "mtools.conf"

#define CFG_FILE1 "sys$login:.mtoolsrc"

/* END */
