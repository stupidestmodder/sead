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
    const size_t cBufSize = 512;
    char tmp[cBufSize];

    s32 len = StringUtil::vsnprintf(tmp, cBufSize, format, list);
    PrintString(tmp, len);
}

void PrintString(const char* str, s32 len)
{
    PrintConfig::execCallbacks(PrintConfig::PrintEventArg(str, len));
}

} } // namespace sead::system
