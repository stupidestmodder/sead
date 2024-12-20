#pragma once

#include <basis/seadTypes.h>

#include <cstdarg>

#if UINTPTR_MAX == UINT32_MAX
    #define SEAD_FMT_SIZE_T "%u"
    #define SEAD_FMT_UINTPTR "0x%08X"
#elif UINTPTR_MAX == UINT64_MAX
    #define SEAD_FMT_SIZE_T "%llu"
    #define SEAD_FMT_UINTPTR "0x%016llX"
#else
    #error "Unsupported platform"
#endif

#if defined(SEAD_TARGET_DEBUG)
#define SEAD_PRINT(format, ...)                                                                    \
    do                                                                                             \
    {                                                                                              \
        sead::system::Print(format, ##__VA_ARGS__);                                                \
    }                                                                                              \
    while (0)
#else
#define SEAD_PRINT(format, ...)                                                                    \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
        {                                                                                          \
            sead::system::Print(format, ##__VA_ARGS__);                                            \
        }                                                                                          \
    } while (0)
#endif // SEAD_TARGET_DEBUG

namespace sead { namespace system {

void Print(const char* format, ...);
void PrintV(const char* format, std::va_list list);
void PrintString(const char* str, s32 len);

void PrintStringImpl(const char* str, s32 len);
void FlushPrint();

} } // namespace sead::system
