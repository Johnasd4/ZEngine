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
#ifndef Z_CORE_INTERNAL_D_CONSTANT_H_
#define Z_CORE_INTERNAL_D_CONSTANT_H_

#include "d_lib.h"
#include "d_type.h"

namespace zengine {

static constexpr Float32 kFloat32Max = 3.402823466e+38F;
static constexpr Float32 kFloat32Min = -3.402823466e+38F;

static constexpr MemoryType kB = 1;
static constexpr MemoryType kKB = 1024;
static constexpr MemoryType kMB = 1048576;
static constexpr MemoryType kGB = 1073741824;

//The unit size when applying memory.
static constexpr MemoryType kHeapMemoryUnitSize = 4 * kKB;

}//zengine

#endif // !Z_CORE_INTERNAL_D_CONSTANT_H_