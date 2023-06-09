#ifndef Z_CORE_Z_OBJECT_H_
#define Z_CORE_Z_OBJECT_H_

#pragma warning(disable : 26439)

#include"internal/drive.h"
#include"f_memory_pool.h"

namespace zengine {

/*
    The base class of almost everything(except the constant class).
    Inheriting from this class allows the instance to apply memory from the memorypool,
    instead of applying memory directly from the system.
*/
class ZObject{
public:
    /*
        Overwrite the new and delete operator to use the memory from the memorypool.
    */
    __forceinline static void* operator new(size_t size) {
        return memory_pool::ApplyMemory(static_cast<MemoryType>(size));
    }
    __forceinline static void operator delete(void* address) {
        memory_pool::ReleaseMemory(reinterpret_cast<Address>(address));
    }

protected:
    __forceinline constexpr ZObject() {}
    __forceinline constexpr ZObject(const ZObject& object) {}
    __forceinline constexpr ZObject(ZObject&& object) { object.MoveDestroy(); }

    __forceinline constexpr const ZObject& operator=(const ZObject& object) { return *this; }
    __forceinline constexpr const ZObject& operator=(ZObject&& object) {
        object.MoveDestroy();
        return *this;
    }

    __forceinline constexpr ~ZObject() {}

private:

    /*
        Reset the member to null when moved.
    */
    __forceinline constexpr Void MoveDestroy() {}
};

}//zengine

#endif // !Z_CORE_Z_OBJECT_H_
