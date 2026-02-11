#ifndef _THREAD_H_
#define _THREAD_H_

int thread_create(void (*start_routine)(void*, void*), void *arg1, void *arg2);
int thread_join();

#endif
