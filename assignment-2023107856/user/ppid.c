#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
    printf("My student ID is 2023017856\n");
    
    printf("My pid is ");
    int pid = getpid();
    printf("%d\n",pid);

    printf("My ppid is ");
    int ppid = getppid();
    printf("%d\n",ppid);
}   

