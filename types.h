/* date = April 25th 2021 0:44 pm */

#ifndef SL_TYPES_H
#define SL_TYPES_H

#include <stdint.h>
#include <stdlib.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

#define internal static
#define global   static
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t i64;
typedef int32_t i32;
typedef float f32;

#endif // COMMON_H
