#pragma once

#include <basis/seadTypes.h>

#ifdef SEAD_DEBUG
#define SEAD_WARNING(format, ...)                                                                  \
    do                                                                                             \
        sead::system::Warning(__FILE__, __LINE__, format, ##__VA_ARGS__);                          \
    while (0)
#else
#define SEAD_WARNING(format, ...)                                                                  \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
            sead::system::Warning(nullptr, 0, format, ##__VA_ARGS__);                              \
    } while (0)
#endif // SEAD_DEBUG

namespace sead { namespace system {

void Warning(const char* pos, s32 line, const char* format, ...);
void SetWarningEnable(bool enable);

} } // namespace sead::system
