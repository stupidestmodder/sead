#pragma once

#include <basis/seadAssert.h>
#include <basis/seadNew.h> 
#include <basis/seadTypes.h>

namespace sead {

class FreeList
{
public:
    FreeList()
        : mFree(nullptr)
        , mWork(nullptr)
    {
    }

    void init(void* work, s32 elemSize, s32 num)
    {
        SEAD_ASSERT(work);
        SEAD_ASSERT(elemSize > 0 && elemSize % cPtrSize == 0);
        SEAD_ASSERT(num > 0);

        void** workAsPtr = static_cast<void**>(work);
        s32 sizeDiv = elemSize / cPtrSize;

        mFree = work;

        for (s32 i = 0; i < num - 1; ++i)
        {
            workAsPtr[sizeDiv * i] = &workAsPtr[sizeDiv * (i + 1)];
        }

        workAsPtr[(num - 1) * sizeDiv] = nullptr;

        mWork = work;
    }

    void cleanup()
    {
        mFree = nullptr;
        mWork = nullptr;
    }

    void* get()
    {
        if (!mFree)
            return nullptr;

        void* ret = mFree;
        mFree = *static_cast<void**>(mFree);
        return ret;
    }

    void put(void* elem)
    {
        *static_cast<void**>(elem) = mFree;
        mFree = elem;
    }

    void* work() const
    {
        return mWork;
    }

    bool isEmpty() const
    {
        return mFree == nullptr;
    }

private:
    void* mFree;
    void* mWork;
};

} // namespace sead
