#ifndef Z_CORE_F_MEMORY_POOL_H_
#define Z_CORE_F_MEMORY_POOL_H_

#include"internal/drive.h"

//Shows the use of the memory pool, includes the memory piece left when the 
// program emds, the total memory pieced appplyed and the peak situation of
// the memory pool. Using this test will slightly reduce the performance of 
// the program.
#define USE_MEMORY_POOL_TEST
//Wheather the memory pool is thread safe.
#define MEMORY_POOL_THREAD_SAFE true

namespace zengine {

/*
    The namespace contains the function that controls the heap memory.
    It has it's own garbage collection.
*/
namespace memory_pool {

/*
    Applys memory by the size given and returns the address of the memory.
*/
CORE_DLLAPI extern const Address ApplyMemory(const MemoryType size) noexcept;

/*
    Applys memory by the size given and returns the address of the memory.
    Returns the memory size of the memory piece.
*/
CORE_DLLAPI extern const Address ApplyMemory(const MemoryType size, MemoryType* memory_size_ptr) noexcept;

 /*
    Release the memory by address, it's a value parameter,
    so remember to set the pointer to null.
    nullptr is allowed to release.
*/
CORE_DLLAPI extern Void ReleaseMemory(const Address address) noexcept;

/*
    Check if the memory can be extended to the given size without changing address,
    if can then the memeory will extend and return true.
*/
CORE_DLLAPI NODISCARD extern const Bool CheckMemory(const Address address, const MemoryType size) noexcept;

}//system_memory
}//zengine

#endif // !Z_CORE_F_MEMORY_POOL_H_
