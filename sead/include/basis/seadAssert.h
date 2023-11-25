#pragma once

#include <basis/seadTypes.h>

#ifdef SEAD_DEBUG
#define SEAD_ASSERT(condition)                                                                     \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
            sead::system::HaltWithDetailNoFormat(__FILE__, __LINE__, #condition);                  \
    } while (0)
#define SEAD_ASSERT_MSG(condition, format, ...)                                                    \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
            sead::system::HaltWithDetail(__FILE__, __LINE__, format, ##__VA_ARGS__);               \
    } while (0)
#else
#define SEAD_ASSERT(condition)                                                                     \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
            static_cast<void>(condition);                                                          \
    } while (0)
#define SEAD_ASSERT_MSG(condition, format, ...)                                                    \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
        {                                                                                          \
            static_cast<void>(condition);                                                          \
            static_cast<void>(format);                                                             \
        }                                                                                          \
    } while (0)
#endif // SEAD_DEBUG

namespace sead { namespace system {

void DebugBreak();
void Halt();
void HaltWithDetail(const char* pos, s32 line, const char* format, ...);
void HaltWithDetailNoFormat(const char* pos, s32 line, const char* str);

#ifdef SEAD_PLATFORM_WINDOWS
void SetEnableExceptionOnHalt(bool enable);
#endif // SEAD_PLATFORM_WINDOWS

} } // namespace sead::system
