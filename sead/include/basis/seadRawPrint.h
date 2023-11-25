#pragma once

#include <basis/seadTypes.h>

#include <cstdarg>

#ifdef SEAD_DEBUG
#define SEAD_PRINT(format, ...)                                                                    \
    do                                                                                             \
        sead::system::Print(format, ##__VA_ARGS__);                                                \
    while (0)
#else
#define SEAD_PRINT(format, ...)                                                                    \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
            static_cast<void>(format);                                                             \
    } while (0)
#endif // SEAD_DEBUG

namespace sead { namespace system {

void Print(const char* format, ...);
void PrintV(const char* format, std::va_list list);
void PrintString(const char* str, s32 len);

void PrintStringImpl(const char* str, s32 len);
void FlushPrint();

} } // namespace sead::system
