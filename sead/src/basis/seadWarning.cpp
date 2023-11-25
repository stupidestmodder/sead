#include <basis/seadWarning.h>

#include <basis/seadRawPrint.h>

static bool sIsWarningEnable = true;

namespace sead { namespace system {

void Warning(const char* pos, s32 line, const char* format, ...)
{
    if (!sIsWarningEnable)
        return;

    std::va_list list;
    va_start(list, format);

    Print("%s(%d) Warning: ", pos, line);
    PrintV(format, list);
    PrintString("\n", 1);

    va_end(list);
}

void SetWarningEnable(bool enable)
{
    sIsWarningEnable = enable;
}

} } // namespace sead::system
