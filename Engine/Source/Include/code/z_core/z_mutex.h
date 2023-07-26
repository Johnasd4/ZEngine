#ifndef Z_CORE_Z_MUTEX_H_
#define Z_CORE_Z_MUTEX_H_

#include "internal/drive.h"

namespace zengine {

/*
    A simple mutex.
*/
class ZMutex {
public:
    FORCEINLINE ZMutex() :handle_(CreateMutex(nullptr, FALSE, nullptr)) {}
    FORCEINLINE ~ZMutex() { CloseHandle(handle_); }

    FORCEINLINE Void Lock() { WaitForSingleObject(handle_, INFINITE); }   
    FORCEINLINE Void Unlock() { ReleaseMutex(handle_); }

private:
    ZMutex(const ZMutex&) = delete;
    ZMutex(ZMutex&&) = delete;

    ZMutex& operator=(const ZMutex&) = delete;
    ZMutex& operator=(ZMutex&&) = delete;

    Handle handle_;
};

}//zengine

#endif // !Z_CORE_Z_MUTEX_H_
