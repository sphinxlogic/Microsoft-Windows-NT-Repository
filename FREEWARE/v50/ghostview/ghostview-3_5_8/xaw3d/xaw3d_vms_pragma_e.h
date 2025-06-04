/*pragma ###jp### 06/18/95 */

#ifndef _XAW3D_VMS_PRAGMA_START_
### XAW3D_VMS_PRAGMA_E.H: Error, _XAW3D_VMS_PRAGMA_START_ not defined
#endif

#if defined(__DECCXX) || defined(__DECC)
#   pragma __extern_model __restore
#endif

#ifdef _EXTERN_
#   undef _EXTERN_
#endif

#ifdef _INTERN_
#   undef _INTERN_
#endif

#undef _XAW3D_VMS_PRAGMA_START_
