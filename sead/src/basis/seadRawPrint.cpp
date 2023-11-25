#include <basis/seadRawPrint.h>

#include <devenv/seadPrintConfig.h>
#include <prim/seadStringUtil.h>

namespace sead { namespace system {

void Print(const char* format, ...)
{
    std::va_list list;
    va_start(list, format);

    PrintV(format, list);

    va_end(list);
}

void PrintV(const char* format, std::va_list list)
{
    const size_t cStrSize = 512;
    char str[cStrSize];

    s32 len = StringUtil::vsnprintf(str, cStrSize, format, list);
    PrintString(str, len);
}

void PrintString(const char* str, s32 len)
{
    PrintConfig::PrintEventArg arg(str, len);
    PrintConfig::execCallbacks(arg);
}

} } // namespace sead::system
