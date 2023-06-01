#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int p1[2], p2[2];
  pipe(p1);
  pipe(p2);
  int pid;
  if ((pid = fork()) < 0)
  {
    exit(1);
  }
  else if (pid > 0)
  // parent process
  {
    close(p1[0]);
    close(p2[1]);
    char buf[1];
    // write to child process
    int len = write(p1[1], buf, 1);
    if (len < 0)
    {
      printf("write error\n");
      exit(1);
    }
    close(p1[1]);
    // read from child process
    len = read(p2[0], buf, 1);
    if (len < 0)
    {
      printf("read error\n");
      exit(1);
    }
    printf("%d: received pong\n", getpid());
    close(p2[0]);
  }
  else
  {
    // child process
    close(p1[1]);
    close(p2[0]);
    char buf[1];
    // read from parent process
    read(p1[0], buf, 1);
    printf("%d: received ping\n", getpid());
    close(p1[0]);
    // write to parent process
    write(p2[1], buf, 1);
    close(p2[1]);
  }

  exit(0);
}