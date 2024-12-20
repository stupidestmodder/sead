#include <basis/seadWarning.h>

#include <basis/seadRawPrint.h>

static bool sIsWarningEnable = true;

namespace sead { namespace system {

void Warning(const char* pos, s32 line, const char* format, ...)
{
    if (sIsWarningEnable)
    {
        Print("%s(%d) Warning: ", pos, line);

        std::va_list list;
        va_start(list, format);
        PrintV(format, list);
        va_end(list);

        PrintString("\n", 1);
    }
}

void SetWarningEnable(bool enable)
{
    sIsWarningEnable = enable;
}

} } // namespace sead::system
