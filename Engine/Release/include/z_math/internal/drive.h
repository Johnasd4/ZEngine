#ifndef Z_MATH_INTERNAL_DRIVE_H_
#define Z_MATH_INTERNAL_DRIVE_H_

#include "z_core/internal/drive.h"

//Define if at the start of the z_math cpp file.
#ifdef MATH_DLLFILE
#define MATH_DLLAPI DLLEXPORT
#else
#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib,"ZMath_x64_Debug.lib")
#else
#pragma comment(lib,"ZMath_x64_Release.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib,"ZMath_Win32_Debug.lib")
#else
#pragma comment(lib,"ZMath_Win32_Release.lib")
#endif
#endif
#define MATH_DLLAPI DLLIMPORT
#endif


namespace zengine {
namespace math {

#pragma region basic constant

//The Float64 version of PI.
constexpr Float64 kPI64 = 3.1415926535897932384626433832795;
//The Float32 version of PI.
constexpr Float32 kPI32 = static_cast<Float32>(kPI64);
//The Float64 version of 2PI.
constexpr Float64 k2PI64 = kPI64 * 2.0;
//The Float32 version of 2PI.
constexpr Float32 k2PI32 = static_cast<Float32>(k2PI64);
//The Float64 version of half PI.
constexpr Float64 kHalfPI64 = kPI64 * 0.5;
//The Float32 version of half PI.
constexpr Float32 kHalfPI32 = static_cast<Float32>(kHalfPI64);

#pragma endregion basic constant

}//math
}//zengine


#endif // !Z_MATH_INTERNAL_DRIVE_H_