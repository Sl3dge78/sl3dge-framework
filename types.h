/* date = April 25th 2021 0:44 pm */

#ifndef SL_TYPES_H
#define SL_TYPES_H

#include <stdint.h>
#include <stdlib.h>

#define DLL_EXPORT extern "C" __declspec(dllexport)

#define internal static;
#define global   static;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t i64;
typedef int32_t i32;
typedef float f32;

#ifdef SDL_h_
typedef SDL_Rect Rect;
#else
typedef struct Rect {
    i32 x;
    i32 y;
    i32 w;
    i32 h;
} Rect;
#endif

typedef struct frect {
    f32 x;
    f32 y;
    f32 w;
    f32 h;
} frect;

#endif // COMMON_H
