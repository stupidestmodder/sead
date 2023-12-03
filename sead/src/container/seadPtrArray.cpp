#include <container/seadPtrArray.h>

#include <basis/seadNew.h>
#include <prim/seadMemUtil.h>
#include <random/seadRandom.h>

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
        s32 end = mPtrNum - i - 1;

        void* ptr = mPtrs[end];
        mPtrs[end] = mPtrs[i];
        mPtrs[i] = ptr;
    }
}

void PtrArrayImpl::shuffle()
{
    Random random;
    shuffle(&random);
}

void PtrArrayImpl::shuffle(Random* random)
{
    SEAD_ASSERT(random);

    for (s32 i = mPtrNum - 1; i > 0; i--)
    {
        u32 j = random->getU32(i + 1);

        void* ptr = mPtrs[i];
        mPtrs[i] = mPtrs[j];
        mPtrs[j] = ptr;
    }
}

void PtrArrayImpl::insert(s32 pos, void* ptr)
{
    if (!checkInsert(pos, 1))
        return;

    createVacancy(pos, 1);
    mPtrs[pos] = ptr;
    mPtrNum++;
}

void PtrArrayImpl::insertArray(s32 pos, void* array, s32 arrayLength, s32 elemSize)
{
    if (!checkInsert(pos, arrayLength))
        return;

    createVacancy(pos, arrayLength);

    for (s32 i = 0; i < arrayLength; i++)
        mPtrs[pos + i] = PtrUtil::addOffset(array, i * elemSize);

    mPtrNum += arrayLength;
}

bool PtrArrayImpl::checkInsert(s32 pos, s32 num)
{
    if (pos < 0)
    {
        SEAD_ASSERT_MSG(false, "illegal pos[%d]", pos);
        return false;
    }

    if (mPtrNum + num > mPtrNumMax)
    {
        SEAD_ASSERT_MSG(false, "list is full.");
        return false;
    }

    if (pos > mPtrNum)
    {
        SEAD_ASSERT_MSG(false, "pos[%d] exceed size[%d]", pos, mPtrNum);
        return false;
    }

    return true;
}

// TODO: Refactor
s32 PtrArrayImpl::compare(const PtrArrayImpl& o, CompareCallbackImpl cmp) const
{
    s32 i = 0;
    while (true)
    {
        if (i >= mPtrNum)
        {
            if (mPtrNum == o.mPtrNum)
                return 0;
            else
                return -1;
        }

        if (i >= o.mPtrNum)
            return 1;

        s32 result = cmp(mPtrs[i], o.mPtrs[i]);
        if (result != 0)
            return result;

        i++;
    }
}

void PtrArrayImpl::uniq(CompareCallbackImpl cmp)
{
    for (s32 i = 0; i < mPtrNum - 1; i++)
    {
        s32 nextPos = i + 1;
        while (nextPos < mPtrNum)
        {
            if (cmp(mPtrs[i], mPtrs[nextPos]) == 0)
                erase(nextPos);
            else
                nextPos++;
        }
    }
}

s32 PtrArrayImpl::binarySearch(const void* ptr, CompareCallbackImpl cmp) const
{
    if (mPtrNum == 0)
        return -1;

    s32 left = 0;
    s32 right = mPtrNum - 1;

    while (left < right)
    {
        const s32 mid = (left + right) / 2;
        const s32 cmpResult = cmp(mPtrs[mid], ptr);

        if (cmpResult == 0)
            return mid;

        if (cmpResult < 0)
            left = mid + 1;
        else
            right = mid;
    }

    if (cmp(mPtrs[left], ptr) == 0)
        return left;

    return -1;
}

} // namespace sead
