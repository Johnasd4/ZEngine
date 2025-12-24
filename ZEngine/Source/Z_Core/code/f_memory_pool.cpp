/*
    Copyright (c) YuLin Zhu

    ** ZEngine Proprietary License **

    This software is provided "as-is", without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.

    Usage Rights:
    1. Non-Commercial Use: You may use, modify, and distribute this software
       for non-commercial purposes (e.g., education, personal projects, open-source
       projects that do not generate revenue) free of charge.

    2. Commercial Use: Commercial use of this software is STRICTLY PROHIBITED
       without a valid commercial license agreement with the author.
       "Commercial use" includes, but is not limited to:
       - Incorporating this software into a product that is sold.
       - Using this software in a paid service.
       - Using this software for internal business operations in a for-profit entity.

    To obtain a Commercial License, please contact the author.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#define CORE_DLLFILE
#include "drive/d_pch.h"

#include "f_memory_pool.h"

#include "m_log.h"

#include "f_memory_pool/c_memory_pool.h"
#include "f_memory_pool/z_generic_global_small_memory_pool.h"
#include "f_memory_pool/z_generic_global_medium_memory_pool.h"
#include "f_memory_pool/z_generic_thread_local_small_memory_pool.h"
#include "f_memory_pool/z_generic_thread_local_medium_memory_pool.h"
#include "f_memory_pool/z_generic_large_memory_pool.h"
#include "f_memory_pool/z_generic_system_memory_pool.h"
#include "f_memory_pool/z_specialized_smart_pointer_control_block_memory_pool.h"
#include "f_memory_pool/z_specialized_log_string_memory_pool.h"
#include "f_memory_pool/z_specialized_error_log_memory_pool.h"
#include "f_memory_pool/z_specialized_info_log_memory_pool.h"
#include "f_memory_pool/z_specialized_trace_log_memory_pool.h"

namespace zengine {
namespace memory_pool {

CORE_DLLAPI NODISCARD Void* ApplyThreadLocalMemory(SizeType _size) noexcept {
    //small memory pool
    if (_size <= ZGenericThreadLocalSmallMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericThreadLocalSmallMemoryPoolGroup::Instance().ApplyMemory(_size);
    }
    //medium memory pool
    else if (_size <= ZGenericThreadLocalMediumMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericThreadLocalMediumMemoryPoolGroup::Instance().ApplyMemory(_size);
    }
    //large memory pool
    else if (_size <= ZGenericLargeMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericLargeMemoryPoolGroup::Instance().ApplyMemory(_size);
    }
    //system memory
    else {
        return ZGenericSystemMemoryPool::Instance().ApplyMemory(_size);
    }
}

CORE_DLLAPI NODISCARD Void* ApplyThreadLocalMemory(SizeType _size, SizeType* _memory_size_ptr) noexcept {
    //small memory pool
    if (_size <= ZGenericThreadLocalSmallMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericThreadLocalSmallMemoryPoolGroup::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
    //medium memory pool
    else if (_size <= ZGenericThreadLocalMediumMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericThreadLocalMediumMemoryPoolGroup::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
    //large memory pool
    else if (_size <= ZGenericLargeMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericLargeMemoryPoolGroup::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
    //system memory
    else {
        return ZGenericSystemMemoryPool::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
}

CORE_DLLAPI NODISCARD Void* ApplyGlobalMemory(SizeType _size) noexcept {
    //small memory pool
    if (_size <= ZGenericGlobalSmallMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericGlobalSmallMemoryPoolGroup::Instance().ApplyMemory(_size);
    }
    //medium memory pool
    else if (_size <= ZGenericGlobalMediumMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericGlobalMediumMemoryPoolGroup::Instance().ApplyMemory(_size);
    }
    //large memory pool
    else if (_size <= ZGenericLargeMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericLargeMemoryPoolGroup::Instance().ApplyMemory(_size);
    }
    //system memory
    else {
        return ZGenericSystemMemoryPool::Instance().ApplyMemory(_size);
    }
}

CORE_DLLAPI NODISCARD Void* ApplyGlobalMemory(SizeType _size, SizeType* _memory_size_ptr) noexcept {
    //small memory pool
    if (_size <= ZGenericGlobalSmallMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericGlobalSmallMemoryPoolGroup::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
    //medium memory pool
    else if (_size <= ZGenericGlobalMediumMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericGlobalMediumMemoryPoolGroup::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
    //large memory pool
    else if (_size <= ZGenericLargeMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericLargeMemoryPoolGroup::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
    //system memory
    else {
        return ZGenericSystemMemoryPool::Instance().ApplyMemory(_size, _memory_size_ptr);
    }
}

CORE_DLLAPI NODISCARD Void* ReapplyThreadLocalMemory(Void* _old_memory_ptr, SizeType _size) noexcept {
    if (_old_memory_ptr == nullptr) {
        return ApplyThreadLocalMemory(_size);
    }

    //check for small memory pool address range
    PointerType memory_pool_address = *reinterpret_cast<PointerType*>(
        reinterpret_cast<PointerType>(_old_memory_ptr) & ~(static_cast<PointerType>(kMemoryPageAlignmentMask))
    );
    PointerType small_pool_group_address =
        reinterpret_cast<PointerType>(&ZGenericThreadLocalSmallMemoryPoolGroup::Instance());
    if (
        memory_pool_address >= small_pool_group_address &&
        memory_pool_address < small_pool_group_address + sizeof(ZGenericThreadLocalSmallMemoryPoolGroup)
    ) {
        ZGenericThreadLocalSmallMemoryPool* memory_pool_ptr = 
            reinterpret_cast<ZGenericThreadLocalSmallMemoryPool*>(memory_pool_address);

        //if reapply size is same as old memory block size
        if (_size <= memory_pool_ptr->GetMemoryBlockSize()) {
            return _old_memory_ptr;
        }
        else {
            //apply new memory block
            Void* new_memory_ptr = ApplyThreadLocalMemory(_size);
            //copy old memory block data to new memory block
            Copy(
                new_memory_ptr, _old_memory_ptr, 
                memory_pool_ptr->GetMemoryBlockSize()
            );

            //release old memory block
            memory_pool_ptr->ReleaseMemory(_old_memory_ptr);

            return new_memory_ptr;
        }
    }
    //other memory pools
    else {
        //other memory pools memory block pool address is 16 Bytes before the memory block pointer
        PointerType memory_pool_address = *reinterpret_cast<PointerType*>(
            reinterpret_cast<PointerType>(_old_memory_ptr) - sizeof(ZGenericMemoryPool::MemoryPageHead_)
        );

        //check for medium memory pool address range
        PointerType medium_pool_group_address = 
            reinterpret_cast<PointerType>(&ZGenericThreadLocalMediumMemoryPoolGroup::Instance());
        if (
            memory_pool_address >= medium_pool_group_address &&
            memory_pool_address < medium_pool_group_address + sizeof(ZGenericThreadLocalMediumMemoryPoolGroup)
        ) {
            ZGenericThreadLocalMediumMemoryPool* memory_pool_ptr = 
                reinterpret_cast<ZGenericThreadLocalMediumMemoryPool*>(memory_pool_address);

            //if reapply size is same as old memory block size
            if (_size <= memory_pool_ptr->GetMemoryBlockSize()) {
                return _old_memory_ptr;
            }
            else {
                //apply new memory block
                Void* new_memory_ptr = ApplyThreadLocalMemory(_size);
                //copy old memory block data to new memory block
                Copy(
                    new_memory_ptr, _old_memory_ptr,
                    memory_pool_ptr->GetMemoryBlockSize()
                );

                //release old memory block
                memory_pool_ptr->ReleaseMemory(_old_memory_ptr);

                return new_memory_ptr;
            }
        }

        //check for large memory pool address range
        PointerType large_pool_group_address = 
            reinterpret_cast<PointerType>(&ZGenericLargeMemoryPoolGroup::Instance());
        if (
            memory_pool_address >= large_pool_group_address &&
            memory_pool_address < large_pool_group_address + sizeof(ZGenericLargeMemoryPoolGroup)
        ) {
            ZGenericLargeMemoryPool* memory_pool_ptr = reinterpret_cast<ZGenericLargeMemoryPool*>(
                memory_pool_address
            );

            //if reapply size is same as old memory block size
            if (_size <= memory_pool_ptr->GetMemoryBlockSize()) {
                return _old_memory_ptr;
            }
            else {
                //apply new memory block
                Void* new_memory_ptr = ApplyThreadLocalMemory(_size);
                //copy old memory block data to new memory block
                Copy(
                    new_memory_ptr, _old_memory_ptr,
                    memory_pool_ptr->GetMemoryBlockSize()
                );

                //release old memory block
                memory_pool_ptr->ReleaseMemory(_old_memory_ptr);

                return new_memory_ptr;
            }
        }

        //check for system address range
        if (memory_pool_address == reinterpret_cast<PointerType>(&ZGenericSystemMemoryPool::Instance())) {

            //apply new memory block
            ZGenericSystemMemoryPool::MemoryBlockHead_* memory_block_head_ptr =
                reinterpret_cast<ZGenericSystemMemoryPool::MemoryBlockHead_*>(_old_memory_ptr) - 1ULL;

            if (_size <= memory_block_head_ptr->memory_block_size_) {
                return _old_memory_ptr;
            }
            else {
                Void* new_memory_ptr = ZGenericSystemMemoryPool::Instance().ApplyMemory(_size);
                //copy old memory block data to new memory block
                Copy(
                    new_memory_ptr, _old_memory_ptr,
                    memory_block_head_ptr->memory_block_size_
                );

                //release old memory block
                ZGenericSystemMemoryPool::Instance().ReleaseMemory(_old_memory_ptr);
                return new_memory_ptr;
            }
        }
    }

    //not thread local memory pool
    Z_LOG_ERROR(
        error_code::kFMemoryPoolErrorCode_NotCurrentThreadMemory, 0,
        "Memory not from thread local memory pool, exit program!"
    );

    Exit(error_code::kFMemoryPoolErrorCode_NotCurrentThreadMemory);
    return nullptr;
}

CORE_DLLAPI NODISCARD Void* ReapplyGlobalMemory(Void* _old_memory_ptr, SizeType _size) noexcept {
    if (_old_memory_ptr == nullptr) {
        return ApplyGlobalMemory(_size);
    }

    //check for small memory pool address range
    PointerType memory_pool_address = *reinterpret_cast<PointerType*>(
        reinterpret_cast<PointerType>(_old_memory_ptr) & ~(static_cast<PointerType>(kMemoryPageAlignmentMask))
    );
    PointerType small_pool_group_address =
        reinterpret_cast<PointerType>(&ZGenericGlobalSmallMemoryPoolGroup::Instance());
    if (
        memory_pool_address >= small_pool_group_address &&
        memory_pool_address < small_pool_group_address + sizeof(ZGenericGlobalSmallMemoryPoolGroup)
    ) {
        ZGenericGlobalSmallMemoryPool* memory_pool_ptr = 
            reinterpret_cast<ZGenericGlobalSmallMemoryPool*>(memory_pool_address);

        //if reapply size is same as old memory block size
        if (_size <= memory_pool_ptr->GetMemoryBlockSize()) {
            return _old_memory_ptr;
        }
        else {
            //apply new memory block
            Void* new_memory_ptr = ApplyGlobalMemory(_size);
            //copy old memory block data to new memory block
            Copy(
                new_memory_ptr, _old_memory_ptr, 
                memory_pool_ptr->GetMemoryBlockSize()
            );

            //release old memory block
            memory_pool_ptr->ReleaseMemory(_old_memory_ptr);

            return new_memory_ptr;
        }
    }
    //other memory pools
    else {
        //other memory pools memory block pool address is 16 Bytes before the memory block pointer
        PointerType memory_pool_address = *reinterpret_cast<PointerType*>(
            reinterpret_cast<PointerType>(_old_memory_ptr) - sizeof(ZGenericMemoryPool::MemoryPageHead_)
        );

        //check for medium memory pool address range
        PointerType medium_pool_group_address = 
            reinterpret_cast<PointerType>(&ZGenericGlobalMediumMemoryPoolGroup::Instance());
        if (
            memory_pool_address >= medium_pool_group_address &&
            memory_pool_address < medium_pool_group_address + sizeof(ZGenericGlobalMediumMemoryPoolGroup)
        ) {
            ZGenericGlobalMediumMemoryPool* memory_pool_ptr = reinterpret_cast<ZGenericGlobalMediumMemoryPool*>(
                memory_pool_address
            );

            //if reapply size is same as old memory block size
            if (_size <= memory_pool_ptr->GetMemoryBlockSize()) {
                return _old_memory_ptr;
            }
            else {
                //apply new memory block
                Void* new_memory_ptr = ApplyGlobalMemory(_size);
                //copy old memory block data to new memory block
                Copy(
                    new_memory_ptr, _old_memory_ptr,
                    memory_pool_ptr->GetMemoryBlockSize()
                );

                //release old memory block
                memory_pool_ptr->ReleaseMemory(_old_memory_ptr);

                return new_memory_ptr;
            }
        }

        //check for large memory pool address range
        PointerType large_pool_group_address = 
            reinterpret_cast<PointerType>(&ZGenericLargeMemoryPoolGroup::Instance());
        if (
            memory_pool_address >= large_pool_group_address &&
            memory_pool_address < large_pool_group_address + sizeof(ZGenericLargeMemoryPoolGroup)
        ) {
            ZGenericLargeMemoryPool* memory_pool_ptr = reinterpret_cast<ZGenericLargeMemoryPool*>(
                memory_pool_address
            );

            //if reapply size is same as old memory block size
            if (_size <= memory_pool_ptr->GetMemoryBlockSize()) {
                return _old_memory_ptr;
            }
            else {
                //apply new memory block
                Void* new_memory_ptr = ApplyGlobalMemory(_size);
                //copy old memory block data to new memory block
                Copy(
                    new_memory_ptr, _old_memory_ptr,
                    memory_pool_ptr->GetMemoryBlockSize()
                );

                //release old memory block
                memory_pool_ptr->ReleaseMemory(_old_memory_ptr);

                return new_memory_ptr;
            }
        }

        //check for system address range
        if (memory_pool_address == reinterpret_cast<PointerType>(&ZGenericSystemMemoryPool::Instance())) {

            //apply new memory block
            ZGenericSystemMemoryPool::MemoryBlockHead_* memory_block_head_ptr =
                reinterpret_cast<ZGenericSystemMemoryPool::MemoryBlockHead_*>(_old_memory_ptr) - 1ULL;

            if (_size <= memory_block_head_ptr->memory_block_size_) {
                return _old_memory_ptr;
            }
            else {
                Void* new_memory_ptr = ZGenericSystemMemoryPool::Instance().ApplyMemory(_size);
                //copy old memory block data to new memory block
                Copy(
                    new_memory_ptr, _old_memory_ptr,
                    memory_block_head_ptr->memory_block_size_
                );

                //release old memory block
                ZGenericSystemMemoryPool::Instance().ReleaseMemory(_old_memory_ptr);
                return new_memory_ptr;
            }
        }
    }

    //not thread local memory pool
    Z_LOG_ERROR(
        error_code::kFMemoryPoolErrorCode_NotGlobalMemory, 0,
        "Memory not from global memory pool, exit program!"
    );

    Exit(error_code::kFMemoryPoolErrorCode_NotGlobalMemory);
    return nullptr;
}

CORE_DLLAPI NODISCARD SizeType CalculateMemory(SizeType _size) noexcept {
    //small memory pool
    if (_size <= ZGenericThreadLocalSmallMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericThreadLocalSmallMemoryPoolGroup::Instance().CalculateMemory(_size);
    }
    //medium memory pool
    else if (_size <= ZGenericThreadLocalMediumMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericThreadLocalMediumMemoryPoolGroup::Instance().CalculateMemory(_size);
    }
    //large memory pool
    else if (_size <= ZGenericLargeMemoryPoolGroup::kMemoryBlockSizeMax) {
        return ZGenericLargeMemoryPoolGroup::Instance().CalculateMemory(_size);
    }
    //system memory
    else {
        return ZGenericSystemMemoryPool::Instance().CalculateMemory(_size);
    }
}

CORE_DLLAPI Void ReleaseThreadLocalMemory(Void* _memory_ptr) noexcept {
    if (_memory_ptr == nullptr) {
        return;
    }

    //check for small memory pool address range
    PointerType memory_pool_address = *reinterpret_cast<PointerType*>(
        reinterpret_cast<PointerType>(_memory_ptr) & ~(static_cast<PointerType>(kMemoryPageAlignmentMask))
    );
    PointerType small_pool_group_address =
        reinterpret_cast<PointerType>(&ZGenericThreadLocalSmallMemoryPoolGroup::Instance());
    if (
        memory_pool_address >= small_pool_group_address &&
        memory_pool_address < small_pool_group_address + sizeof(ZGenericThreadLocalSmallMemoryPoolGroup)
    ) {
        reinterpret_cast<ZGenericThreadLocalSmallMemoryPool*>(memory_pool_address)->ReleaseMemory(_memory_ptr);
        return;
    }

    //other memory pools memory block pool address is 16 Bytes before the memory block pointer
    memory_pool_address = *reinterpret_cast<PointerType*>(
        reinterpret_cast<PointerType>(_memory_ptr) - sizeof(ZGenericMemoryPool::MemoryPageHead_)
    );

    //check for medium memory pool address range
    PointerType medium_pool_group_address = 
        reinterpret_cast<PointerType>(&ZGenericThreadLocalMediumMemoryPoolGroup::Instance());
    if (
        memory_pool_address >= medium_pool_group_address &&
        memory_pool_address < medium_pool_group_address + sizeof(ZGenericThreadLocalMediumMemoryPoolGroup)
    ) {
        reinterpret_cast<ZGenericThreadLocalMediumMemoryPool*>(memory_pool_address)->ReleaseMemory(_memory_ptr);
        return;
    }

    //check for large memory pool address range
    PointerType large_pool_group_address = reinterpret_cast<PointerType>(&ZGenericLargeMemoryPoolGroup::Instance());
    if (
        memory_pool_address >= large_pool_group_address &&
        memory_pool_address < large_pool_group_address + sizeof(ZGenericLargeMemoryPoolGroup)
        ) {
        reinterpret_cast<ZGenericLargeMemoryPool*>(memory_pool_address)->ReleaseMemory(_memory_ptr);
        return;
    }
    
    //system memory
    if (memory_pool_address == reinterpret_cast<PointerType>(&ZGenericSystemMemoryPool::Instance())) {
        ZGenericSystemMemoryPool::Instance().ReleaseMemory(_memory_ptr);
        return;
    }

    //not thread local memory pool
    Z_LOG_ERROR(
        error_code::kFMemoryPoolErrorCode_NotGlobalMemory, 0,
        "Memory not from thread local memory pool, memory leak!"
    );
}

CORE_DLLAPI Void ReleaseGlobalMemory(Void* _memory_ptr) noexcept {
    if (_memory_ptr == nullptr) {
        return;
    }

    //check for small memory pool address range
    PointerType memory_pool_address = *reinterpret_cast<PointerType*>(
        reinterpret_cast<PointerType>(_memory_ptr) & ~(static_cast<PointerType>(kMemoryPageAlignmentMask))
    );
    PointerType small_pool_group_address =
        reinterpret_cast<PointerType>(&ZGenericGlobalSmallMemoryPoolGroup::Instance());

    //check for small memory pool address range
    if (
        memory_pool_address >= small_pool_group_address &&
        memory_pool_address < small_pool_group_address + sizeof(ZGenericGlobalSmallMemoryPoolGroup)
    ) {
        reinterpret_cast<ZGenericGlobalSmallMemoryPool*>(memory_pool_address)->ReleaseMemory(_memory_ptr);
        return;
    }

    //other memory pools memory block pool address is 16 Bytes before the memory block pointer
    memory_pool_address = *reinterpret_cast<PointerType*>(
        reinterpret_cast<PointerType>(_memory_ptr) - sizeof(ZGenericMemoryPool::MemoryPageHead_)
    );

    //check for medium memory pool address range
    PointerType medium_pool_group_address = 
        reinterpret_cast<PointerType>(&ZGenericGlobalMediumMemoryPoolGroup::Instance());
    if (
        memory_pool_address >= medium_pool_group_address &&
        memory_pool_address < medium_pool_group_address + sizeof(ZGenericGlobalMediumMemoryPoolGroup)
    ) {
        reinterpret_cast<ZGenericGlobalMediumMemoryPool*>(memory_pool_address)->ReleaseMemory(_memory_ptr);
        return;
    }

    //check for large memory pool address range
    PointerType large_pool_group_address = reinterpret_cast<PointerType>(&ZGenericLargeMemoryPoolGroup::Instance());
    if (
        memory_pool_address >= large_pool_group_address &&
        memory_pool_address < large_pool_group_address + sizeof(ZGenericLargeMemoryPoolGroup)
    ) {
        reinterpret_cast<ZGenericLargeMemoryPool*>(memory_pool_address)->ReleaseMemory(_memory_ptr);
        return;
    }

    //system memory
    if (memory_pool_address == reinterpret_cast<PointerType>(&ZGenericSystemMemoryPool::Instance())) {
        ZGenericSystemMemoryPool::Instance().ReleaseMemory(_memory_ptr);
        return;
    }

    //not global memory pool
    Z_LOG_ERROR(
        error_code::kFMemoryPoolErrorCode_NotGlobalMemory, 0,
        "Memory not from global memory pool, memory leak!"
    );
}

CORE_DLLAPI NODISCARD Void* ApplySmartPointerControlBlockMemory() noexcept {
    return ZSpecializedSmartPointerControlBlockMemoryPool::Instance().ApplyMemory();
}

CORE_DLLAPI Void ReleaseSmartPointerControlBlockMemory(Void* _memory_ptr) noexcept {
    ZSpecializedSmartPointerControlBlockMemoryPool::Instance().ReleaseMemory(_memory_ptr);
}

CORE_DLLAPI NODISCARD Void* ApplyLogStringMemory() noexcept {
    return ZSpecializedLogStringMemoryPool::Instance().ApplyMemory();
}

CORE_DLLAPI Void ReleaseLogStringMemory(Void* _memory_ptr) noexcept {
    ZSpecializedLogStringMemoryPool::Instance().ReleaseMemory(_memory_ptr);
}

CORE_DLLAPI NODISCARD Void* ApplyErrorLogMemory() noexcept {
    return ZSpecializedErrorLogMemoryPool::Instance().ApplyMemory();
}

CORE_DLLAPI Void ReleaseErrorLogMemory(Void* _memory_ptr) noexcept {
    ZSpecializedErrorLogMemoryPool::Instance().ReleaseMemory(_memory_ptr);
}

CORE_DLLAPI NODISCARD Void* ApplyInfoLogMemory() noexcept {
    return ZSpecializedInfoLogMemoryPool::Instance().ApplyMemory();
}

CORE_DLLAPI Void ReleaseInfoLogMemory(Void* _memory_ptr) noexcept {
    ZSpecializedInfoLogMemoryPool::Instance().ReleaseMemory(_memory_ptr);
}

CORE_DLLAPI NODISCARD Void* ApplyTraceLogMemory() noexcept {
    return ZSpecializedTraceLogMemoryPool::Instance().ApplyMemory();
}

CORE_DLLAPI Void ReleaseTraceLogMemory(Void* _memory_ptr) noexcept {
    ZSpecializedTraceLogMemoryPool::Instance().ReleaseMemory(_memory_ptr);
}

}//memory_pool
}//zengine
