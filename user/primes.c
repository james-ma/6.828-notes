#include "kernel/types.h"
#include "user/user.h"

int main() {
  int p[2];
  pipe(p);
  int pid;
  if ((pid = fork()) < 0) {
    exit(1);
  } else if (pid == 0) {
    int primes(int p[]);
    primes(p);
    exit(0);
  } else {
    // write the all numbers
    close(p[0]);
    for (int i = 2; i <= 35; i++) {
      write(p[1], &i, sizeof(i));
    }
    close(p[1]);
    wait((void *)0);
  }
  exit(0);
}

int primes(int p[]) {
  int prime;
  int pid;
  close(p[1]);
  if (read(p[0], &prime, sizeof(prime)) == 0) {
    exit(0);
  }
  printf("prime %d\n", prime);
  int p2[2];
  pipe(p2);
  if ((pid = fork()) < 0) {
    printf("fork error\n");
    exit(1);
  } else if (pid == 0) {
    primes(p2);
    exit(0);
  } else {
    int num;
    close(p2[0]);
    while (read(p[0], &num, sizeof(num)) != 0) {
      if (num % prime != 0) {
        write(p2[1], &num, sizeof(num));
      }
    }
    close(p2[1]);
    close(p[0]);

    wait((void *)0);
  }
  exit(0);
}