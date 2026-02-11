#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_LOOP 100000
#define NUM_SLEEP 50

#define NUM_THREAD 8
#define MAX_LEVEL 3
int parent;

int fork_children()
{
  int i, p;
  for (i = 0; i < NUM_THREAD; i++)
    if ((p = fork()) == 0)
    {
      sleep(0);
      return getpid();
    }
  return parent;
}

int fork_children2()
{
  int i, p;
  for (i = 0; i < NUM_THREAD; i++)
  {
    if ((p = fork()) == 0)
    {
      sleep(0);
      return getpid();
    }
    else
    {
      int r = setpriority(p, i % 4);  // 0~3
      if (r < 0)
      {
        printf("setpriority returned %d\n", r);
        exit(1);
      }
    }
  }
  return parent;
}

void exit_children()
{
  if (getpid() != parent)
    exit(0);
  while (wait(0) != -1);
}

int main(int argc, char *argv[])
{
  int i, pid;
  int count[MAX_LEVEL] = {0};
  mlfqmode();

  parent = getpid();

  printf("MLFQ test start\n");

  printf("[Test 1] default\n");
  pid = fork_children();

  if (pid != parent)
  {
    for (i = 0; i < NUM_LOOP; i++)
    {
      int x = getlev();
      if (x < 0 || x >= MAX_LEVEL)
      {
        printf("Wrong level: %d\n", x);
        exit(1);
      }
      count[x]++;
    }
    printf("Process %d\n", pid);
    for (i = 0; i < MAX_LEVEL; i++)
      printf("L%d: %d\n", i, count[i]);
  }
  exit_children();
  printf("[Test 1] finished\n");

  printf("[Test 2] priorities\n");
  pid = fork_children2();

  if (pid != parent)
  {
    for (i = 0; i < NUM_LOOP; i++)
    {
      int x = getlev();
      if (x < 0 || x >= MAX_LEVEL)
      {
        printf("Wrong level: %d\n", x);
        exit(1);
      }
      count[x]++;
    }
    printf("Process %d\n", pid);
    for (i = 0; i < MAX_LEVEL; i++)
      printf("L%d: %d\n", i, count[i]);
  }
  exit_children();
  printf("[Test 2] finished\n");

  printf("[Test 3] sleep\n");
  pid = fork_children2();

  if (pid != parent)
  {
    for (i = 0; i < NUM_SLEEP; i++)
    {
      int x = getlev();
      if (x < 0 || x >= MAX_LEVEL)
      {
        printf("Wrong level: %d\n", x);
        exit(1);
      }
      count[x]++;
      sleep(1);
    }
    printf("Process %d\n", pid);
    for (i = 0; i < MAX_LEVEL; i++)
      printf("L%d: %d\n", i, count[i]);
  }
  exit_children();
  printf("[Test 3] finished\n");

  printf("MLFQ test finished\n");
  exit(0);
}
