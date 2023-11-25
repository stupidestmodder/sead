#include <container/seadPtrArray.h>

#include <basis/seadNew.h>
#include <prim/seadMemUtil.h>

namespace sead {

void PtrArrayImpl::allocBuffer(s32 ptrNumMax, s32 alignment)
{
    if (!tryAllocBuffer(ptrNumMax, alignment))
        AllocFailAssert(nullptr, ptrNumMax * sizeof(void*), alignment);
}

void PtrArrayImpl::allocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment)
{
    if (!tryAllocBuffer(ptrNumMax, heap, alignment))
        AllocFailAssert(heap, ptrNumMax * sizeof(void*), alignment);
}

bool PtrArrayImpl::tryAllocBuffer(s32 ptrNumMax, s32 alignment)
{
    SEAD_ASSERT(mPtrs == nullptr);

    if (ptrNumMax <= 0)
    {
        SEAD_ASSERT_MSG(false, "ptrNumMax[%d] must be larger than zero", ptrNumMax);
        return false;
    }

    void* buffer = new(alignment, std::nothrow) u8[ptrNumMax * sizeof(void*)];
    if (!buffer)
        return false;

    setBuffer(ptrNumMax, buffer);
    return true;
}

bool PtrArrayImpl::tryAllocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment)
{
    SEAD_ASSERT(mPtrs == nullptr);

    if (ptrNumMax <= 0)
    {
        SEAD_ASSERT_MSG(false, "ptrNumMax[%d] must be larger than zero", ptrNumMax);
        return false;
    }

    void* buffer = new(heap, alignment, std::nothrow) u8[ptrNumMax * sizeof(void*)];
    if (!buffer)
        return false;

    setBuffer(ptrNumMax, buffer);
    return true;
}

void PtrArrayImpl::freeBuffer()
{
    if (isBufferReady())
    {
        delete[] mPtrs;
        mPtrs = nullptr;
        mPtrNum = 0;
        mPtrNumMax = 0;
    }
}

void PtrArrayImpl::setBuffer(s32 ptrNumMax, void* buffer)
{
    if (ptrNumMax <= 0)
    {
        SEAD_ASSERT_MSG(false, "ptrNumMax[%d] must be larger than zero", ptrNumMax);
        return;
    }

    if (!buffer)
    {
        SEAD_ASSERT_MSG(false, "buffer is null");
        return;
    }

    mPtrs = static_cast<void**>(buffer);
    mPtrNumMax = ptrNumMax;
    mPtrNum = 0;
}

void PtrArrayImpl::erase(s32 pos, s32 num)
{
    if (pos < 0)
    {
        SEAD_ASSERT_MSG(false, "illegal position[%d]", pos);
        return;
    }

    if (num < 0)
    {
        SEAD_ASSERT_MSG(false, "illegal number[%d]", num);
        return;
    }

    const s32 endPos = pos + num;
    if (endPos > mPtrNum)
    {
        SEAD_ASSERT_MSG(false, "pos[%d] + num[%d] exceed size[%d]", pos, num, mPtrNum);
        return;
    }

    if (endPos < mPtrNum)
        MemUtil::copyOverlap(mPtrs + pos, mPtrs + endPos, (mPtrNum - endPos) * sizeof(void*));

    mPtrNum -= num;
}

void PtrArrayImpl::reverse()
{
    for (s32 i = 0; i < mPtrNum / 2; i++)
    {
        
    }
}

} // namespace sead
