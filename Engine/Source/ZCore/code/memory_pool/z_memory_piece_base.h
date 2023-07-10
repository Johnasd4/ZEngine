#ifndef Z_CORE_MEMORY_POOL_Z_MEMORY_PIECE_BASE_H_
#define Z_CORE_MEMORY_POOL_Z_MEMORY_PIECE_BASE_H_

#include"internal/drive.h"

namespace zengine {
namespace memory_pool {

/*
    The base type of all memory piece.
*/
struct ZMemoryPieceBase {
    FORCEINLINE Void Initialize(Void* pool_ptr) {}
};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_MEMORY_PIECE_BASE_H_
