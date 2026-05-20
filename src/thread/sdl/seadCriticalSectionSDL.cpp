#include <thread/seadCriticalSection.h>

namespace sead {

CriticalSection::CriticalSection()
    : IDisposer()
    , mMutexInner(nullptr)
{
    mMutexInner = SDL_CreateMutex();
}

CriticalSection::CriticalSection(Heap* disposerHeap)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mMutexInner(nullptr)
{
    mMutexInner = SDL_CreateMutex();
}

CriticalSection::~CriticalSection()
{
    SDL_DestroyMutex(mMutexInner);
}

void CriticalSection::lock()
{
    SDL_LockMutex(mMutexInner);
}

bool CriticalSection::tryLock()
{
    return SDL_TryLockMutex(mMutexInner) == 1;
}

void CriticalSection::unlock()
{
    SDL_UnlockMutex(mMutexInner);
}

} // namespace sead
