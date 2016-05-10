#include <i2c-rpi.h>
#include <mutex>

static std::mutex& get_status_mutex() {
    static std::mutex _mtx;
    return _mtx;
}

void set_get_status_lock() {
    get_status_mutex().lock();
}
void set_get_status_unlock() {
    get_status_mutex().unlock();
}
