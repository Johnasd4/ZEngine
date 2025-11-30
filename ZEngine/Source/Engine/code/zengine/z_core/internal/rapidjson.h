/*
    Copyright (c) YuLin Zhu

    This code file is licensed under the Creative Commons
    Attribution-NonCommercial 4.0 International License.

    You may obtain a copy of the License at
    https://creativecommons.org/licenses/by-nc/4.0/

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    Author: YuLin Zhu
    Contact: 1152325286@qq.com
*/
#pragma once

#include "../drive.h"

namespace rapidjson {
    template<typename _EncodingType, typename _AllocatorType>
    class GenericValue;
    template<typename _AllocatorType>
    class MemoryPoolAllocator;
    template<typename _CharType = zengine::Char>
    struct UTF8;
}

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/allocators.h"
#include "rapidjson/prettywriter.h"

#include "../f_memory_pool.h"

namespace zengine {
namespace internal {
    
class JsonAllocatorP {
public:
    static constexpr Bool kNeedFree = true;

    FORCEINLINE Void* Malloc(SizeType _size) noexcept {
        if (_size == 0U) {
            return nullptr;
        }
        return memory_pool::ApplyMemory(static_cast<SizeType>(_size));
    }

    FORCEINLINE Void* Realloc(Void* _old_ptr, SizeType _old_size, SizeType _new_size) noexcept {
        if (_new_size == 0U) {
            memory_pool::ReleaseMemory(_old_ptr);
            return nullptr;
        }
        return memory_pool::ReapplyMemory(_old_ptr, static_cast<SizeType>(_new_size));
    }

    FORCEINLINE static Void Free(Void* _mem_ptr) noexcept {
        memory_pool::ReleaseMemory(_mem_ptr);
    }
};
using JsonAllocator = rapidjson::MemoryPoolAllocator<internal::JsonAllocatorP>;
using JsonDocument = rapidjson::GenericDocument<rapidjson::UTF8<>, internal::JsonAllocator, internal::JsonAllocatorP>;
using JsonValue = rapidjson::GenericValue<rapidjson::UTF8<>, JsonAllocator>;

}//internal
}//zengine

