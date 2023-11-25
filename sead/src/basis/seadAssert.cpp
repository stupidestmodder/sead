#include <basis/seadAssert.h>

namespace sead { namespace system {

void HaltWithDetailNoFormat(const char* pos, s32 line, const char* str)
{
    HaltWithDetail(pos, line, "%s", str);
}

} } // namespace sead::system
