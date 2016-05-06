#include "mutex.h"

void Mutex_Lock(mutex* mtx) {
    while (*mtx != 0);
    *mtx = 1;
}
void Mutex_Unlock(mutex* mtx) {
    while (*mtx == 0);
    *mtx = 0;
}
int Mutex_Try_Lock(mutex* mtx) {
    if (*mtx == 0) {
        *mtx = 1;
        return 0;
    } else {
        return -1;
    }
}