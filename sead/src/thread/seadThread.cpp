#include <thread/seadThread.h>

namespace sead {

ThreadMgr::ThreadMgr()
    : mList()
    , mIterateLockCS()
    , mMainThread(nullptr)
    , mThreadPtrTLS()
{
}

} // namespace sead
