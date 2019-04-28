#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char **argv) {

  pid_t ppid = getpid();
  pid_t pid;
  assert(distort_time(getpid(), 2) == EPERM);

  switch (pid = fork()) {                     /* powstaje nowy proces */

    case -1:                                  /* błąd funkcji fork() */
      return -1;

    case 0:                                   /* proces potomny */

      printf("I am a child and my pid is %d\n", getpid());
      printf("I am a child and fork() return value is %d\n", pid);
      distort_time(ppid, 2);

      return 0;                               /* proces potomny kończy */

    default:                                  /* proces macierzysty */

      printf("I am a parent and my pid is %d\n", getpid());
      printf("I am a parent and fork() return value is %d\n", pid);
//      distort_time(pid, 2);


      if (wait(0) == -1) return -1;
      /* czeka na zakończenie procesu potomnego */

      return 0;
  }

}