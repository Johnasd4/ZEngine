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
    NODISCARD FORCEINLINE static Void* operator new(SizeType size) {
        return memory_pool::ApplyMemory(static_cast<MemoryType>(size));
    }
    NODISCARD FORCEINLINE static Void* operator new(SizeType size, Void* memory_ptr) {
        return memory_ptr;
    }
    NODISCARD FORCEINLINE static Void* operator new[](SizeType size) {
        return memory_pool::ApplyMemory(static_cast<MemoryType>(size));
    }
    NODISCARD FORCEINLINE static Void* operator new[](SizeType size, Void* memory_ptr) {
        return memory_ptr;
    }
    NODISCARD FORCEINLINE static Void operator delete(Void* const memory_ptr) {
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(memory_ptr));
    }
    NODISCARD FORCEINLINE static Void operator delete[](Void* const memory_ptr) {
        memory_pool::ReleaseMemory(reinterpret_cast<Void*>(memory_ptr));
    }

protected:
    FORCEINLINE constexpr ZObject() {}
    FORCEINLINE constexpr ZObject(const ZObject& object) {}
    FORCEINLINE constexpr ZObject(ZObject&& object) { object.MoveDestroy(); }

    FORCEINLINE constexpr const ZObject& operator=(const ZObject& object) { return *this; }
    FORCEINLINE constexpr const ZObject& operator=(ZObject&& object) {
        object.MoveDestroy();
        return *this;
    }

    FORCEINLINE constexpr ~ZObject() {}

private:

    /*
        Reset the member to null when moved.
    */
    FORCEINLINE constexpr Void MoveDestroy() {}
};

}//zengine

#endif // !Z_CORE_Z_OBJECT_H_
