#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <sys/time.h>
#include "mproc.h"

int do_distort_time() {

  pid_t pid = m_in.m_distort_time.pid;
  uint8_t scale = m_in.m_distort_time.scale;

  printf("Hello from PM %d %d\n", pid, scale);
  return 0;
}
