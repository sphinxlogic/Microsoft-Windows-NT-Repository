/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Define the version number, name, etc.
 * The patchlevel is in included_patches[], in version.c.
 *
 * This doesn't use string contatenation, some compilers don't support it.
 */

#define VIM_VERSION_MAJOR		 5
#define VIM_VERSION_MAJOR_STR		"5"
#define VIM_VERSION_MINOR		 6
#define VIM_VERSION_MINOR_STR		"6"
#define VIM_VERSION_BUILD		 62
#define VIM_VERSION_BUILD_STR		"62"
#define VIM_VERSION_PATCHLEVEL		 0
#define VIM_VERSION_PATCHLEVEL_STR	"0"

/*
 * VIM_VERSION_NODOT is used for the runtime directory name.
 * VIM_VERSION_SHORT is copied into the swap file (max. length is 6 chars).
 * VIM_VERSION_MEDIUM is used for the startup-screen.
 * VIM_VERSION_LONG is used for the ":version" command and "Vim -h".
 */
#define VIM_VERSION_NODOT	"vim56"
#define VIM_VERSION_SHORT	"5.6"
#define VIM_VERSION_MEDIUM	"5.6"
#define VIM_VERSION_LONG	"VIM - Vi IMproved 5.6 (2000 Jan 16)"
#define VIM_VERSION_LONG_DATE	"VIM - Vi IMproved 5.6 (2000 Jan 16, compiled "
