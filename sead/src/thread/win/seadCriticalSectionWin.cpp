#include <thread/seadCriticalSection.h>

namespace sead {

CriticalSection::CriticalSection()
    : IDisposer()
    , mMutexInner()
{
    InitializeCriticalSection(&mMutexInner);
}

CriticalSection::CriticalSection(Heap* disposerHeap)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mMutexInner()
{
    InitializeCriticalSection(&mMutexInner);
}

CriticalSection::~CriticalSection()
{
    DeleteCriticalSection(&mMutexInner);
}

void CriticalSection::lock()
{
    EnterCriticalSection(&mMutexInner);
}

bool CriticalSection::tryLock()
{
    return TryEnterCriticalSection(&mMutexInner) != 0;
}

void CriticalSection::unlock()
{
    LeaveCriticalSection(&mMutexInner);
}

} // namespace sead
