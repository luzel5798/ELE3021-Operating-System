#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

extern struct proc proc[];

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_yieldUser(void)
{
  yield();
  return 0;
}

uint64
sys_getlev(void)
{
  if (schedState){
      return 99;
  } 
  else {
    struct proc *p = myproc();
    if (p){
      return myproc()->queueLV;
    }
    else {
      return -1;
    }
  }
}

uint64
sys_setpriority(void)
{
  int pid, priority;
  argint(0, &pid);
  argint(1, &priority);
  if (priority < 0 || priority > 3){
    return -2;
  } 
  struct proc *this;
  this = findProc(pid);
  if (this){
    this->priority = priority;
    return 0;
  }
  else {
    return -1;
  }
}

uint64
sys_mlfqmode(void)
{
  if (!schedState){
      printf("Error: CPU scheduler is already MLFQ mode.\n");
      return -1;
  }
  // for 문으로 모든 proc 접근 후 싹 다 L0 queue에 enqueue해서 넣고자 함.
  // mysyscall.c 에서는 proc[NPROC]에 접근 불가하므로 proc.c 에서 함수를 만들어 이 안에서 처리했음.
  schedState = 0;
  globaltick = 0;
  cleanQueue();
  makeQueue();
  return 0;
}

uint64
sys_fcfsmode(void)
{
    if (schedState){
        printf("Error: CPU scheduler is already FCFS mode.\n");
        return -1; 
    }
    
    for (int i=0; i<NPROC; i++){
      proc[i].queueLV = -1;
      proc[i].tickProc = -1;
      proc[i].priority = -1;
    }
    schedState = 1;
    globaltick = 0;
    return 0;
}

