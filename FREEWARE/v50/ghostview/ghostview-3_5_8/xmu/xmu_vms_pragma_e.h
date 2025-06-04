/*pragma ###jp### 06/18/95 */

#ifndef _XMU_VMS_PRAGMA_START_
### XMU_VMS_PRAGMA_E.H: Error, _XMU_VMS_PRAGMA_START_ not defined
#endif

#if defined(__DECCXX) || defined(__DECC)
#   pragma __extern_model __restore
#endif

#if defined(_EXTERN_)
#   undef _EXTERN_
#endif

#undef _XMU_VMS_PRAGMA_START_
