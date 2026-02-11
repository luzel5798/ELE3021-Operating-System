#include "user.h"
#include "thread.h"

#define PGSIZE 4096
#define SYS_clone 22
#define SYS_join 23

int thread_create(void (*start_routine)(void*, void*), void *arg1, void *arg2) {
    //Page Size의 2배만큼 할당함.
    //내가 지금 할당받은 범위 내에는 사용하고 있는 addr이 없다는 것이 분명하므로
    //이 범위 내에서 offset bit를 0으로 밀어서 stack이 다른 데이터는 안들고 있고
    //page의 시작 부분을 가리키도록 설정한다. 
    
    void *tmp = sbrk(PGSIZE*2);
    void *stack = (void*)(((uint64)tmp + PGSIZE - 1) & ~(PGSIZE - 1));
    //malloc 으로 공간을 할당하긴 하였으나 실제로 쓰지 않았을 때
    //실제 주소로 할당이 안되어 있을 수 있기에 실제 페이지가 매핑되도록 유도
    memset(stack, 0, PGSIZE);

    if (stack == 0 || (uint64)stack % PGSIZE != 0) {
        free(tmp);
        return -1;
    }
    return clone(start_routine, arg1, arg2, stack);
}

int thread_join() {
    void **stack = malloc(sizeof(void*));
    
    int tid = join(stack);

    if (tid < 0)
        return -1;

    if (stack && *stack) {
        free(*stack);
    }   
    return tid;
}
