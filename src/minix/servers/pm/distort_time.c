#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <sys/time.h>
#include "mproc.h"


int related(pid_t ppid, pid_t pid) {

  struct mproc *p = find_proc(pid);

  while (p->mp_pid != 1) {

    if (p->mp_pid == ppid) {
      return OK;
    }

    p = &mproc[p->mp_parent];
  }
  return EPERM;
}

int do_distort_time() {

  pid_t pid = m_in.m_distort_time.pid;
  uint8_t scl = m_in.m_distort_time.scale;

  if (find_proc(pid) == NULL) {
    return (EINVAL);
  }

  if (mp->mp_pid == find_proc(pid)->mp_pid) {
    return (EPERM);
  }

  struct mproc *p = find_proc(pid);

  if (related(mp->mp_pid, pid) == OK) {

    p->distorted = 1;
    p->scale = scl;

  } else if (related(pid, mp->mp_pid) == OK) {

    p->distorted = 2;
    p->scale = scl;

  } else {
    return (EPERM);
  }
  return OK;
}
