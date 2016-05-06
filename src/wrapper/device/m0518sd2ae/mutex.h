#ifndef __MUTEX_H__
#define __MUTEX_H__ 1

typedef char mutex;

void Mutex_Lock(mutex* mtx);
void Mutex_Unlock(mutex* mtx);
int Mutex_Try_Lock(mutex* mtx);     // zero to success, none zero to failure

#endif