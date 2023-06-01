#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLINE 1024

void copy(char **p1, char *p2)
{
}

int readLine(char **params, int args_index)
{
  // read the input from stdin
  int i = 0;
  char buf[MAXLINE];
  while (read(0, &buf[i], 1) > 0)
  {
    if (buf[i] == '\n')
    {
      buf[i] = '\0';
      break;
    }
    i++;
    if (i >= MAXLINE)
    {
      printf("xargs: input line too long\n");
      exit(1);
    }
  }

  if (i == 0)
    return -1;

  int length = strlen(buf);
  // build the args
  int k = 0;
  while (k < length)
  {
    if (args_index > MAXARG)
    {
      printf("xargs: too many arguments\n");
      exit(1);
    }
    // skip the white space
    while (k < length && buf[k] == ' ')
      k++;

    char *arg = (char *)malloc(MAXLINE);
    int j = 0;
    while (k < length && buf[k] != ' ')
    {
      arg[j++] = buf[k++];
    }
    arg[j] = '\0';
    params[args_index++] = arg;
    k++;
  }
  params[args_index] = 0;
  return args_index;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("xargs: missing arguments\n");
    exit(1);
  }

  // build the cmd and args
  int args_index = 0;
  char *cmd = argv[1];
  char *params[MAXARG];
  for (int i = 1; i < argc; i++)
    params[args_index++] = argv[i];

  int end;
  while ((end = readLine(params, args_index)) != -1)
  {
    // execute the cmd
    int pid = fork();
    if (pid < 0)
    {
      printf("xargs: fork error\n");
      exit(1);
    }
    else if (pid == 0)
    {
      exec(cmd, params);
      printf("xargs: exec error\n");
      exit(1);
    }
    else
    {
      wait(0);
    }
  }

  exit(0);
}
