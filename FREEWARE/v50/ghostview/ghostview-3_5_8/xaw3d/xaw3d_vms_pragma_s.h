/*pragma ###jp### 06/18/95 */
/*
###jp### 02/29/96:
   Tim Adye (adye@v2.rl.ac.uk) suggested to change the default
   __extern_model from __common_block to __relaxed_refdef to allow
   creation/usage of object instead of shared libraries.
   The old behaviour (__common_block) may be selected by
   /DEF=GLOBALS_COMMON_BLOCK at compile time.
*/
#ifdef _XAW3D_VMS_PRAGMA_START_
### XAW3D_VMS_PRAGMA_S.H: Error, _XAW3D_VMS_PRAGMA_START_ already defined
#endif

#ifdef _XAW3D_VMS_PRAGMA_END_
### XAW3D_VMS_PRAGMA_S.H: Error, _XAW3D_VMS_PRAGMA_END_ defined
#endif  

#define _XAW3D_VMS_PRAGMA_START_

#if defined(__DECCXX) || defined(__DECC)
#   pragma __extern_model __save
#   ifdef GLOBALS_COMMON_BLOCK
#     pragma __extern_model __common_block noshr
#   else
#     pragma __extern_model __relaxed_refdef noshr
#   endif
#   define _EXTERN_ extern
#   define _INTERN_
#else
#   define _EXTERN_ extern noshare
#   define _INTERN_ noshare
#endif
