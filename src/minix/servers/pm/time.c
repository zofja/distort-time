/* This file takes care of those system calls that deal with time.
 *
 * The entry points into this file are
 *   do_getres:		perform the CLOCK_GETRES system call
 *   do_gettime:	perform the CLOCK_GETTIME system call
 *   do_settime:	perform the CLOCK_SETTIME system call
 *   do_time:		perform the GETTIMEOFDAY system call
 *   do_stime:		perform the STIME system call
 */

#include "pm.h"
#include <minix/callnr.h>
#include <minix/com.h>
#include <signal.h>
#include <sys/time.h>
#include "mproc.h"

/*===========================================================================*
 *				do_gettime				     *
 *===========================================================================*/
int do_gettime() {

  clock_t ticks, realtime, clock;
  time_t boottime;
  int s;

  if ((s = getuptime(&ticks, &realtime, &boottime)) != OK)
    panic("do_time couldn't get uptime: %d", s);

  switch (m_in.m_lc_pm_time.clk_id) {
    case CLOCK_REALTIME:
      clock = realtime;
      break;
    case CLOCK_MONOTONIC:
      clock = ticks;
      break;
    default:
      return EINVAL; /* invalid/unsupported clock_id */
  }

  mp->mp_reply.m_pm_lc_time.sec = boottime + (clock / system_hz);
  mp->mp_reply.m_pm_lc_time.nsec =
          (uint32_t)((clock % system_hz) * 1000000000ULL / system_hz);

  return (OK);
}

/*===========================================================================*
 *				do_getres				     *
 *===========================================================================*/
int do_getres() {
  switch (m_in.m_lc_pm_time.clk_id) {
    case CLOCK_REALTIME:
    case CLOCK_MONOTONIC:
      /* tv_sec is always 0 since system_hz is an int */
      mp->mp_reply.m_pm_lc_time.sec = 0;
      mp->mp_reply.m_pm_lc_time.nsec = 1000000000 / system_hz;
      return (OK);
    default:
      return EINVAL; /* invalid/unsupported clock_id */
  }
}

/*===========================================================================*
 *				do_settime				     *
 *===========================================================================*/
int do_settime() {
  int s;
  struct mproc *proc;

  if (mp->mp_effuid != SUPER_USER) {
    return (EPERM);
  }

  switch (m_in.m_lc_pm_time.clk_id) {
    case CLOCK_REALTIME:

      for (proc = &mproc[0]; proc < &mproc[NR_PROCS]; proc++) {
        if (set_base_time[proc->mp_pid] == 1) {
          set_base_time[proc->mp_pid] = 0;
        }
      }

      s = sys_settime(m_in.m_lc_pm_time.now, m_in.m_lc_pm_time.clk_id,
                      m_in.m_lc_pm_time.sec, m_in.m_lc_pm_time.nsec);
      return (s);
    case CLOCK_MONOTONIC: /* monotonic cannot be changed */
    default:
      return EINVAL; /* invalid/unsupported clock_id */
  }
}

/*===========================================================================*
 *				do_time					     *
 *===========================================================================*/
int do_time() {
/* Perform the time(tp) system call. This returns the time in seconds since 
 * 1.1.1970.  MINIX is an astrophysically naive system that assumes the earth 
 * rotates at a constant rate and that such things as leap seconds do not 
 * exist.
 */
  clock_t
  ticks, realtime;
  time_t boottime;
  int s;

  if ((s = getuptime(&ticks, &realtime, &boottime)) != OK)
    panic("do_time couldn't get uptime: %d", s);

  pid_t pid = mp->mp_pid;

  if (distortion[pid] != 0 && set_base_time[pid] != 0) {

    uint64_t dif = (uint64_t)realtime - base_time[pid];
    uint64_t scl = scale[pid];
    uint64_t base = base_time[pid];
    uint64_t scaled_hz = system_hz * scl;

    if (distortion[pid] == 1 || scl == 0) { // mnożenie lub 0

      printf("multiply or 0\n");
      printf("base %llu  scale %llu\n", base, scl);

      mp->mp_reply.m_pm_lc_time.sec = boottime + ((base_time[pid] + dif * scl) / system_hz);
      mp->mp_reply.m_pm_lc_time.nsec =
              (uint64_t)(((base_time[pid] + dif * scl) % system_hz) * 1000000000ULL / system_hz);

    } else { // dzielenie

      printf("division\n");
      printf("base %llu  scale %llu\n", base, scl);

      mp->mp_reply.m_pm_lc_time.sec = boottime + ((dif + base * scl) / (scaled_hz));
      mp->mp_reply.m_pm_lc_time.nsec =
              (uint64_t)(((dif + base * scl) % (scaled_hz)) * 1000000000ULL / (scaled_hz));

    }
  } else {

    set_base_time[pid] = 1;
    base_time[pid] = realtime;

    mp->mp_reply.m_pm_lc_time.sec = boottime + (realtime / system_hz);
    mp->mp_reply.m_pm_lc_time.nsec =
            (uint32_t)((realtime % system_hz) * 1000000000ULL / system_hz);
  }

  return (OK);
}

/*===========================================================================*
 *				do_stime				     *
 *===========================================================================*/
int do_stime() {
/* Perform the stime(tp) system call. Retrieve the system's uptime (ticks 
 * since boot) and pass the new time in seconds at system boot to the kernel.
 */
  clock_t uptime, realtime;
  time_t boottime;
  int s;

  if (mp->mp_effuid != SUPER_USER) {
    return (EPERM);
  }
  if ((s = getuptime(&uptime, &realtime, &boottime)) != OK)
    panic("do_stime couldn't get uptime: %d", s);
  boottime = m_in.m_lc_pm_time.sec - (realtime / system_hz);

  s = sys_stime(boottime);    /* Tell kernel about boottime */
  if (s != OK)
    panic("pm: sys_stime failed: %d", s);

  return (OK);
}
