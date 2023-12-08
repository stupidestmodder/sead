#include <prim/seadMemUtil.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static NT_TIB* getTIB()
{
    return reinterpret_cast<NT_TIB*>(NtCurrentTeb());
}

namespace sead {

bool MemUtil::isStack(const void* addr)
{
    NT_TIB* tib = getTIB();
    return PtrUtil::isInclude(addr, tib->StackLimit, tib->StackBase);
}

} // namespace sead
