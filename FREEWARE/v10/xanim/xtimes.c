 /* xtimes.c */

#include "xtimes.h"

#define HZ 60

u_int xtimes(struct tms buffer) {

  struct timeb t_struct, *t_ptr;
  tbuffer_t tm_buf, *tb_ptr;
  u_int return_time;

  t_ptr = &t_struct;
  tb_ptr = &tm_buf;

  ftime(t_ptr);
  time(tb_ptr);

  buffer.tms_utime = tb_ptr->proc_user_time;
  buffer.tms_stime = tb_ptr->proc_system_time;
  buffer.tms_cutime = tb_ptr->child_user_time;
  buffer.tms_cstime = tb_ptr->child_system_time;

  return_time = t_ptr->time*60 + (t_ptr->millitm*60)/1000;
  return return_time;
}
