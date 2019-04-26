#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>
#include "mproc.h"


bool if_descendant(pid_t pid) {

  pid_t my_pid = mp->mp_pid; // get id of curr proc

  struct mproc *lpid = find_proc(pid); // ptr to mproc of pid from parameter

  while(lpid) {

    printf("my pid %d   lpid %d\n", my_pid, lpid->mp_pid);
    if (lpid->mp_pid == my_pid) {
      return true;
    }

    lpid = &mproc[lpid->mp_parent];
  }
  return false;
}

bool if_ancestor(pid_t pid) {

  return false;
}

int do_distort_time() {

  pid_t pid = m_in.m_distort_time.pid;
  uint8_t scale = m_in.m_distort_time.scale;

  if (if_descendant(pid)) {
    printf("is descendant\n");
  } else if (if_ancestor(pid)) {
    printf ("is ancesrot\n");
  } else {
    printf("is neither\n");
  }

  printf("Hello from PM %d %d\n", pid, scale);
  return 0;
}
