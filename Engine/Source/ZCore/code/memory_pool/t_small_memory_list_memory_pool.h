/*
    Copyright (c) YuLin Zhu (÷Ï”Í¡÷)

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu (÷Ï”Í¡÷)
    Contact: 1152325286@qq.com
*/
#ifndef Z_CORE_MEMORY_POOL_T_SMALL_MEMORY_LIST_MEMORY_POOL_H_
#define Z_CORE_MEMORY_POOL_T_SMALL_MEMORY_LIST_MEMORY_POOL_H_

#include "internal/z_drive.h"

#include "f_console.h"
#include "t_array.h"
#include "t_lookup_table.h"

#include "t_list_memory_pool_base.h"
#include "t_memory_block_base.h"

namespace zengine {
namespace memory_pool {

template<Bool kIsThreadSafe>
struct TSmallMemoryBlock : public TMemoryBlockBase{
    TMemoryPoolBase<kIsThreadSafe>* owner_memory_pool_ptr;

    FORCEINLINE Void Initialize(Void* _pool_ptr) noexcept {
        owner_memory_pool_ptr = reinterpret_cast<TMemoryPoolBase<kIsThreadSafe>*>(_pool_ptr);
    }
};

/*
    The memory pool for small memory. This singleton pattern like class, it has
    a fixed number of instance. All the memory pool will be generated when any
    of the memory pool is used. The instance is private.
    Waring: The program will crash if a nullptr is send in.
    Template Parameters:
    - kIsThreadSafe: Thread safe or not.
*/
template<Bool kIsThreadSafe>
class TSmallMemoryListMemoryPool : public TListMemoryPoolBase<TSmallMemoryBlock<kIsThreadSafe>, sizeof(TSmallMemoryBlock<kIsThreadSafe>), kIsThreadSafe> {
private:
    //The sizes of the memory blocks(includes the memory size).
    static constexpr IndexType kMemoryBlockTypeNum = 10;
    static constexpr MemoryType kMemoryBlockMinSize = 32;
    static constexpr MemoryType kMemoryBlockSizeMultGrowFactor = 2;

public:
    NODISCARD static Void* ApplyMemory(const MemoryType _size) noexcept {
        static TArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        IndexType size_index = (_size + SuperType_::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
        IndexType memory_pool_index = kMemorySize2MemoryPoolTable.At(size_index);
#ifdef USE_MEMORY_POOL_TEST
        memory_pool_array[memory_pool_index].memory_block_used_current_num_ += 1;
        memory_pool_array[memory_pool_index].momory_block_applyed_num_ += 1;
        if (memory_pool_array[memory_pool_index].memory_block_used_current_num_ >
            memory_pool_array[memory_pool_index].momory_block_peak_num_) {
            memory_pool_array[memory_pool_index].momory_block_peak_num_ =
                memory_pool_array[memory_pool_index].memory_block_used_current_num_;
        }
#endif //USE_MEMORY_POOL_TEST
        return memory_pool_array[memory_pool_index].SuperType_::ApplyMemory();
    }
    NODISCARD static Void* ApplyMemory(const MemoryType _size, MemoryType* _memory_size_ptr) noexcept {
        static TArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        IndexType size_index = (_size + SuperType_::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
        IndexType memory_pool_index = kMemorySize2MemoryPoolTable.At(size_index);
#ifdef USE_MEMORY_POOL_TEST
        memory_pool_array[memory_pool_index].memory_block_used_current_num_ += 1;
        memory_pool_array[memory_pool_index].momory_block_applyed_num_ += 1;
        if (memory_pool_array[memory_pool_index].memory_block_used_current_num_ >
            memory_pool_array[memory_pool_index].momory_block_peak_num_) {
            memory_pool_array[memory_pool_index].momory_block_peak_num_ =
                memory_pool_array[memory_pool_index].memory_block_used_current_num_;
        }
#endif //USE_MEMORY_POOL_TEST
        (*_memory_size_ptr) = memory_pool_array[memory_pool_index].SuperType_::MemoryBlockMemorySize();
        return memory_pool_array[memory_pool_index].SuperType_::ApplyMemory();
    }

    /*
        Checks if the memory can extend without moving to a new memory, If can
        then it will auto extend and return true.
    */
    NODISCARD FORCEINLINE static Bool CheckMemory(
        TSmallMemoryListMemoryPool* _memory_pool_ptr, 
        MemoryType _size
    ) noexcept {
        return _memory_pool_ptr->SuperType_::MemoryBlockMemorySize() >= _size;
    }
    NODISCARD FORCEINLINE static Bool CheckMemory(
        TSmallMemoryListMemoryPool* _memory_pool_ptr, 
        MemoryType _size, 
        MemoryType* _memory_size_ptr
    ) noexcept {
        return (*_memory_size_ptr = _memory_pool_ptr->SuperType_::MemoryBlockMemorySize()) >= _size;
    }

    NODISCARD FORCEINLINE static MemoryType CalculateMemory(MemoryType _size) noexcept {
        static TArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& memory_pool_array = InstanceP();
        IndexType size_index = (_size + SuperType_::NodeHeadOffset() - 1) / kMemoryBlockMinSize;
        return memory_pool_array[kMemorySize2MemoryPoolTable.At(size_index)].SuperType_::MemoryBlockMemorySize();
    }

    static Void ReleaseMemory(TSmallMemoryListMemoryPool* _memory_pool_ptr, Void* _memory_ptr) noexcept {
#ifdef USE_MEMORY_POOL_TEST
        _memory_pool_ptr->memory_block_used_current_num_ -= 1;
#endif //USE_MEMORY_POOL_TEST
        _memory_pool_ptr->SuperType_::ReleaseMemory(_memory_ptr);
    }

    NODISCARD static constexpr MemoryType MemoryBlockMemoryMaxSize() noexcept { return kMemoryBlockMemoryMaxSize; }
    NODISCARD static constexpr IndexType MemoryBlockTypeNum() noexcept { return kMemoryBlockTypeNum; }

    FORCEINLINE TSmallMemoryListMemoryPool() : SuperType_() {}
    ~TSmallMemoryListMemoryPool() noexcept {
#if USE_MEMORY_POOL_TEST
        ReturnType link_code = kOK;
        ZFile file;
        TWFixedString<ZFile::kFileNameLength> file_str;
        const ZSystemTime& system_time = ZSystemTime::StartTimeInstance();

        file_str.SetString(
            L"%ls%04d%02d%02d%02d%02d%02d_memory.log", log::ZLog::kPathTString,
            system_time.Year(), system_time.Month(), system_time.Day(),
            system_time.Hour(), system_time.Min(), system_time.Sec());
        link_code = file.OpenSafe(log::ZLog::kPathTString, file_str.DataPtr(), ZFile::kOpenTypeAppendW);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kMLogErrorCodeLinkError, link_code, L"ZFile::OpenSafe() link error!");
        }

        //The first pool realsed.
        if (SuperType_::MemoryBlockSize() == kMemoryBlockMaxSize) {
            link_code = file.Print("\n\n***** small memory pool *****\n\n");
            if (link_code != kOK) {
                Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeLinkError, link_code, L"ZFile::OpenSafe() link error!");
            }

            link_code = file.Print("    size    | usable size |  total num  | applied times | used peak num | unused num\n");
            if (link_code != kOK) {
                Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeLinkError, link_code, L"ZFile::OpenSafe() link error!");
            }
        }

        link_code = file.Print(
            "  %8u  |  %9u  |  %9d  |   %9d   |   %9d   |  %8d\n",
            SuperType_::MemoryBlockSize(),
            SuperType_::MemoryBlockMemorySize(),
            SuperType_::Capacity(),
            momory_block_applyed_num_,
            momory_block_peak_num_,
            memory_block_used_current_num_);
        if (link_code != kOK) {
            Z_LOG_ERROR(error_code::kFMemoryPoolErrorCodeLinkError, link_code, L"ZFile::OpenSafe() link error!");
        }

#endif //USE_MEMORY_POOL_TEST        
    }

protected:
    using SuperType_ = TListMemoryPoolBase<TSmallMemoryBlock<kIsThreadSafe>, sizeof(TSmallMemoryBlock<kIsThreadSafe>), kIsThreadSafe>;

private:
    static constexpr MemoryType kMemoryBlockHeadSize = SuperType_::NodeHeadOffset();
    static constexpr TArray<MemoryType, kMemoryBlockTypeNum> kMemoryBlockSizeArray =
        TArray<MemoryType, kMemoryBlockTypeNum>([](TArray<MemoryType, kMemoryBlockTypeNum>* _array_ptr) {
        (*_array_ptr)[0] = kMemoryBlockMinSize;
            for (IndexType index = 1; index < _array_ptr->Capacity(); ++index) {
                (*_array_ptr)[index] = (*_array_ptr)[index - 1] * kMemoryBlockSizeMultGrowFactor;
            }
        });
    static constexpr MemoryType kMemoryBlockMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1];
    static constexpr MemoryType kMemoryBlockMemoryMaxSize = kMemoryBlockSizeArray[kMemoryBlockTypeNum - 1]
        - kMemoryBlockHeadSize;

    //The sizes of the memorys that can be uesd.
    static constexpr TArray<MemoryType, kMemoryBlockTypeNum> kMemoryBlockMemorySizeArray =
        TArray<MemoryType, kMemoryBlockTypeNum>([](TArray<MemoryType, kMemoryBlockTypeNum>* _array_ptr) {
            for (IndexType index = 0; index < _array_ptr->Capacity(); ++index) {
                (*_array_ptr)[index] = kMemoryBlockSizeArray[index] - SuperType_::NodeHeadOffset();
            }
        });

    //The number of the blocks that the memory pool contains when created.
    static constexpr IndexType kMemoryBlockDefaultNum = 0;
    static constexpr TArray<IndexType, kMemoryBlockTypeNum> kMemoryBlockDefaultNumArray =
        TArray<IndexType, kMemoryBlockTypeNum>([](TArray<IndexType, kMemoryBlockTypeNum>* _array_ptr) {
            for (IndexType index = 0; index < _array_ptr->Capacity(); ++index) {
                (*_array_ptr)[index] = kMemoryBlockDefaultNum;
            }
        });

    //The lookup table that links the memory size to the memory pool index.
    //mamory alignment
    static constexpr IndexType kMemorySize2MemoryPoolTableSize = kMemoryBlockMaxSize / kMemoryBlockMinSize;
    static constexpr TLookupTable<UInt8, kMemorySize2MemoryPoolTableSize> kMemorySize2MemoryPoolTable =
        TLookupTable<UInt8, kMemorySize2MemoryPoolTableSize>(
            [](TLookupTable<UInt8, kMemorySize2MemoryPoolTableSize>* _table_ptr) {
                UInt8 current_pool_index = 0;
                IndexType current_pool_index_max_index = 1;
                for (IndexType index = 0; index < _table_ptr->Size(); ++index) {
                    if (index < current_pool_index_max_index) {
                        (*_table_ptr)[index] = current_pool_index;
                    }
                    else {
                        ++current_pool_index;
                        current_pool_index_max_index *= static_cast<IndexType>(kMemoryBlockSizeMultGrowFactor);
                        (*_table_ptr)[index] = current_pool_index;
                    }
                }
            });

    NODISCARD static TArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>& InstanceP() noexcept {
        static TArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum> memory_pool_array(
            MemoryPoolArrayInitFunction);
        return memory_pool_array;
    }

    static Void MemoryPoolArrayInitFunction(
        TArray<TSmallMemoryListMemoryPool<kIsThreadSafe>, kMemoryBlockTypeNum>* _array_ptr
    ) noexcept {
        for (IndexType index = 0; index < _array_ptr->Capacity(); ++index) {
            (*_array_ptr)[index].InitializeP(kMemoryBlockSizeArray[index], kMemoryBlockMemorySizeArray[index],
                kMemoryBlockDefaultNumArray[index]);
        }
    }

    TSmallMemoryListMemoryPool(const TSmallMemoryListMemoryPool&) = delete;
    TSmallMemoryListMemoryPool(TSmallMemoryListMemoryPool&&) = delete;

    TSmallMemoryListMemoryPool& operator=(const TSmallMemoryListMemoryPool&) = delete;
    TSmallMemoryListMemoryPool& operator=(TSmallMemoryListMemoryPool&&) = delete;

    FORCEINLINE Void InitializeP(
        MemoryType _memory_block_size, 
        MemoryType _memory_block_memory_size, 
        Int32 _capacity
    ) noexcept {
        SuperType_::InitializeP(
            MemoryPoolEnum::kTSmallMemoryListMemoryPool, 
            _memory_block_size,
            _memory_block_memory_size, 
            _capacity);
    }


#ifdef USE_MEMORY_POOL_TEST
    IndexType memory_block_used_current_num_ = 0;
    IndexType momory_block_applyed_num_ = 0;
    IndexType momory_block_peak_num_ = 0;
#endif //USE_MEMORY_POOL_TEST
};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_T_SMALL_MEMORY_LIST_MEMORY_POOL_H_
