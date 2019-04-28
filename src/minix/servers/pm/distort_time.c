#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <sys/time.h>
#include "mproc.h"

int is_descendant(pid_t pid) {

  pid_t my_pid = mp->mp_pid; // get id of curr proc

  struct mproc *lpid = find_proc(pid); // ptr to mproc of pid from parameter

  while (lpid->mp_pid != 1) {

    if (lpid->mp_pid == my_pid) {
      return 0;
    }

    lpid = &mproc[lpid->mp_parent];
  }
  return 1;
}

int is_ancestor(pid_t pid) {

  pid_t my_pid = mp->mp_pid; // get id of curr proc

  struct mproc *lpid = find_proc(my_pid); // ptr to mproc of curr proc

  while (lpid->mp_pid != 1) {

    if (lpid->mp_pid == pid) {
      return OK;
    }

    lpid = &mproc[lpid->mp_parent];
  }
  return EPERM;
}


void print_debug(pid_t pid) {

  printf("*** DEBUG ***\n");

  printf("distortion: %d\n", distortion[pid]);
  printf("scale: %d\n", scale[pid]);
  printf("set_base_time: %d\n", set_base_time[pid]);
  printf("base_time: %llu\n", base_time[pid]);

  printf("*** END DEBUG ***\n");

}

int do_distort_time() {

  pid_t pid = m_in.m_distort_time.pid;
  uint8_t
  scl = m_in.m_distort_time.scale;

  if (find_proc(pid) == NULL) {
    return (EINVAL);
  }

  if (mp->mp_pid == find_proc(pid)->mp_pid) {
    return (EPERM);
  }

//  which_pid[mp->mp_pid] = pid;

  if (is_descendant(pid) == OK) {

    printf("descendant\n");

    distortion[pid] = 1;
    scale[pid] = scl;
//    print_debug(pid);

  } else if (is_ancestor(pid) == OK) {

    printf("ancestor\n");

    distortion[pid] = 2;
    scale[pid] = scl;
//    print_debug(pid);

  } else {
//    which_pid[mp->mp_pid] = 0;

    return (EPERM);
  }

  return 0;
}
