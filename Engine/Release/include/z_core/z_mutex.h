#ifndef Z_CORE_Z_MUTEX_H_
#define Z_CORE_Z_MUTEX_H_

#include"internal/drive.h"

namespace zengine {

/*
    A simple mutex.
*/
class ZMutex {
public:
    __forceinline ZMutex() :handle_(CreateMutex(nullptr, FALSE, nullptr)) {}
    __forceinline ~ZMutex() { CloseHandle(handle_); }

    __forceinline Void lock() { WaitForSingleObject(handle_, INFINITE); }   
    __forceinline Void unlock() { ReleaseMutex(handle_); }

private:
    Handle handle_;
};

}//zengine

#endif // !Z_CORE_Z_MUTEX_H_
