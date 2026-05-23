#include <prim/seadMemUtil.h>

#include <pthread.h>

namespace sead {

bool MemUtil::isStack(const void* addr)
{
#if defined(SEAD_PLATFORM_MACOSX)
    pthread_t thread = pthread_self();

    void* stackTop = pthread_get_stackaddr_np(thread);
    size_t stackSize = pthread_get_stacksize_np(thread);

    return PtrUtil::isInclude(addr, PtrUtil::addOffset(stackTop, -static_cast<intptr_t>(stackSize)), stackTop);
#else
    pthread_attr_t attr;
    if (pthread_getattr_np(pthread_self(), &attr) != 0)
    {
        return false;
    }

    void* stackAddr;
    size_t stackSize;
    pthread_attr_getstack(&attr, &stackAddr, &stackSize);
    pthread_attr_destroy(&attr);

    return PtrUtil::isInclude(addr, stackAddr, PtrUtil::addOffset(stackAddr, stackSize));
#endif // SEAD_PLATFORM
}

} // namespace sead
