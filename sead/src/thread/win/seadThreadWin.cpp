#include <thread/seadThread.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(ThreadMgr);

void Thread::sleep(TickSpan span)
{
    s64 ms = span.toMilliSeconds();
    SEAD_ASSERT((ms & 0xFFFFFFFF00000000) == 0);

    timeBeginPeriod(1);
    Sleep(static_cast<DWORD>(ms));
    timeEndPeriod(1);
}

} // namespace sead
