#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

#define O_RDONLY 0x000
#define STDERR_FILENO 2

void find(char *path, char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0)
  {
    fprintf(STDERR_FILENO, "find open %s error\n", path);
    exit(1);
  }

  if (fstat(fd, &st) < 0)
  {
    fprintf(STDERR_FILENO, "fstat %s error\n", path);
    close(fd);
    exit(1);
  }

  switch (st.type)
  {
  case T_FILE:
    break;

  case T_DIR: // if directory recursive call find
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
    {
      fprintf(STDERR_FILENO, "find: path too long\n");
      break;
    }

    // add '/'
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      // inum == 0 means invalid directory entry
      if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
          strcmp(de.name, "..") == 0)
        continue;

      // add de.name to path
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if (stat(buf, &st) < 0)
      {
        fprintf(STDERR_FILENO, "stat %s error\n", buf);
        exit(1);
      }
      switch (st.type)
      {
      case T_FILE:
        if (0 == strcmp(de.name, name))
          printf("%s\n", buf);
        break;
      case T_DIR:
        find(buf, name);
        break;
      }
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(STDERR_FILENO, "usage: find <path> <name>\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}