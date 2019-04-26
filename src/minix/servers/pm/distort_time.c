#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>
#include "mproc.h"


bool is_descendant(pid_t pid) {

  pid_t my_pid = mp->mp_pid; // get id of curr proc

  struct mproc *lpid = find_proc(pid); // ptr to mproc of pid from parameter

  while(lpid->mp_pid != 1) {

    printf("my pid %d   lpid %d\n", my_pid, lpid->mp_pid);
    if (lpid->mp_pid == my_pid) {
      return true;
    }

    lpid = &mproc[lpid->mp_parent];
  }
  return false;
}

bool is_ancestor(pid_t pid) {

  pid_t my_pid = mp->mp_pid; // get id of curr proc

  struct mproc *lpid = find_proc(my_pid); // ptr to mproc of curr proc

  while(lpid->mp_pid != 1) {

    printf("lpid %d   pid %d\n", lpid->mp_pid, pid);
    if (lpid->mp_pid == pid) {
      return true;
    }

    lpid = &mproc[lpid->mp_parent];
  }
  return false;
}

int do_distort_time() {

  /*
   * if pid is DESCENDANT of current process: time *= scale
   * if pid is ANCESTOR of current process: time /= scale
   */

  // TODO przypadki szczególne

  pid_t pid = m_in.m_distort_time.pid;
  uint8_t scale = m_in.m_distort_time.scale;

  if (is_descendant(pid)) {

    // przyspiesz
    printf("is descendant\n");
  } else if (is_ancestor(pid)) {

    // spowolnij
    printf ("is ancestor\n");
  } else {

    printf("is neither\n");
    return EPERM;
  }

  printf("Hello from PM %d %d\n", pid, scale);
  return 0;
}
