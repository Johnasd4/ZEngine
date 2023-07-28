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
#ifndef Z_CORE_MEMORY_POOL_Z_MEMORY_BLOCK_BASE_H_
#define Z_CORE_MEMORY_POOL_Z_MEMORY_BLOCK_BASE_H_

#include "internal/drive.h"

namespace zengine {
namespace memory_pool {

/*
    The base type of all memory block.
*/
struct ZMemoryBlockBase {
    FORCEINLINE Void InitializeP(Void* pool_ptr) {}
};

}//memory_pool
}//zengine

#endif // !Z_CORE_MEMORY_POOL_Z_MEMORY_BLOCK_BASE_H_
