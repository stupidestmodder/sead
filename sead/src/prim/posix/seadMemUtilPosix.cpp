#include <prim/seadMemUtil.h>

#include <pthread.h>

namespace sead {

bool MemUtil::isStack(const void* addr)
{
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
}

} // namespace sead
