#include <hostio/seadHostIOEventListener.h>

#include <hostio/seadHostIOThreadLock.h>

namespace sead { namespace hostio {

u32 LifeCheckable::sCurrentCreateID = 1;
LifeCheckable* LifeCheckable::sTopInstance = nullptr;

LifeCheckable::DisposeHostIOCaller::~DisposeHostIOCaller()
{
    if (mInstance)
    {
        mInstance->disposeHostIO();
        mInstance = nullptr;
    }
}

LifeCheckable* LifeCheckable::searchInstanceFromCreateID(u32 createID)
{
    ThreadLock lock;

    for (LifeCheckable* instance = sTopInstance; instance; instance = instance->mNext)
    {
        if (instance->mCreateID == createID)
        {
            return instance;
        }
    }

    return nullptr;
}

void LifeCheckable::initialize_()
{
    ThreadLock lock;

    mCreateID = sCurrentCreateID;

    sCurrentCreateID++;
    if (sCurrentCreateID == 0)
    {
        sCurrentCreateID = 1;
    }

    if (sTopInstance)
    {
        mNext = sTopInstance;
        sTopInstance->mPrev = this;
    }

    sTopInstance = this;
}

void LifeCheckable::disposeHostIOImpl_()
{
    ThreadLock lock;

    if (sTopInstance == this)
    {
        SEAD_ASSERT(mPrev == nullptr);

        if (mNext)
        {
            sTopInstance = mNext;
            sTopInstance->mPrev = nullptr;
        }
        else
        {
            sTopInstance = nullptr;
        }
    }
    else
    {
        SEAD_ASSERT(mPrev != nullptr);
        mPrev->mNext = mNext;

        if (mNext)
        {
            SEAD_ASSERT(mNext->mPrev == this);
            mNext->mPrev = mPrev;
        }
    }
}

} } // namespace sead::hostio
