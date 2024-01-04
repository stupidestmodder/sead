#include <gfx/seadDrawLockContext.h>

namespace sead {

DrawLockContext::DrawLockContext()
    : mCriticalSection()
{
}

DrawLockContext::~DrawLockContext()
{
}

void DrawLockContext::initialize(Heap*)
{
}

void DrawLockContext::lock()
{
    mCriticalSection.lock();
}

void DrawLockContext::unlock()
{
    mCriticalSection.unlock();
}

} // namespace sead
