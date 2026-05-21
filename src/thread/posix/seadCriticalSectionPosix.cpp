#include <thread/seadCriticalSection.h>

#include <cerrno>

namespace sead {

CriticalSection::CriticalSection()
    : IDisposer()
    , mMutexInner(PTHREAD_MUTEX_INITIALIZER)
{
    initialize_pthread(&mMutexInner);
}

CriticalSection::CriticalSection(Heap* disposerHeap)
    : IDisposer(disposerHeap, HeapNullOption::eFindContainHeap)
    , mMutexInner(PTHREAD_MUTEX_INITIALIZER)
{
    initialize_pthread(&mMutexInner);
}

CriticalSection::~CriticalSection()
{
    pthread_mutex_destroy(&mMutexInner);
}

void CriticalSection::lock()
{
    pthread_mutex_lock(&mMutexInner);
}

bool CriticalSection::tryLock()
{
    return pthread_mutex_trylock(&mMutexInner) != EBUSY; //? Why not == 0 ?
}

void CriticalSection::unlock()
{
    pthread_mutex_unlock(&mMutexInner);
}

void CriticalSection::initialize_pthread(pthread_mutex_t* pthread)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    pthread_mutex_init(pthread, &attr);

    pthread_mutexattr_destroy(&attr);
}

} // namespace sead
