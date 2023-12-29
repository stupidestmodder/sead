#pragma once

#include <heap/seadHeapMgr.h>

namespace sead {

struct HeapPolicy
{
    HeapPolicy()
        : parent(nullptr)
        , size(0)
        , create_slack(0)
        , adjust_slack(0)
        , adjust(0)
        , temporary(0)
        , dont_create(0)
    {
    }

    Heap* parent;
    u32 size;
    u32 create_slack;
    u32 adjust_slack;
    u8 adjust;
    u8 temporary;
    u8 dont_create;
};

class HeapPolicies
{
public:
    HeapPolicies()
        : mPolicies()
        , mPrimaryIndex(0)
    {
    }

    void useOnly(s32 idx)
    {
        s32 rootHeapNum = HeapMgr::getRootHeapNum();
        if (idx >= rootHeapNum)
        {
            SEAD_ASSERT_MSG(false, "illegal idx: %d", idx);
            return;
        }

        for (s32 i = 0; i < rootHeapNum; i++)
        {
            mPolicies[i].dont_create = idx != i;
        }

        mPrimaryIndex = idx;
    }

    void useOnlyPrimaryHeap()
    {
        useOnly(mPrimaryIndex);
    }

    void setAdjustAll(bool b)
    {
        s32 rootHeapNum = HeapMgr::getRootHeapNum();
        for (s32 i = 0; i < rootHeapNum; i++)
        {
            mPolicies[i].adjust = b;
        }
    }

    const HeapPolicy& operator[](s32 idx) const
    {
        if (idx >= HeapMgr::getRootHeapNum())
        {
            SEAD_ASSERT_MSG(false, "illegal idx: %d", idx);
            return mPolicies[0];
        }

        return mPolicies[idx];
    }

    HeapPolicy& operator[](s32 idx)
    {
        //* Yeah...
        return const_cast<HeapPolicy&>(const_cast<const HeapPolicies*>(this)->operator[](idx));
    }

    s32 getPrimaryHeapIndex() const
    {
        return mPrimaryIndex;
    }

private:
    HeapPolicy mPolicies[NUM_ROOT_HEAPS_MAX];
    s32 mPrimaryIndex;
};

class HeapArray
{
public:
    HeapArray()
        : mPrimaryIndex(0)
    {
        for (s32 i = 0; i < NUM_ROOT_HEAPS_MAX; i++)
        {
            mHeaps[i] = nullptr;
            mAdjusted[i] = false;
        }
    }

    Heap* getHeap(s32 idx) const
    {
        if (idx >= HeapMgr::getRootHeapNum())
        {
            SEAD_ASSERT_MSG(false, "illegal idx: %d", idx);
            return mHeaps[0];
        }

        return mHeaps[idx];
    }

    Heap* getHeap(s32 idx)
    {
        //* Yeah...
        return const_cast<const HeapArray*>(this)->getHeap(idx);
    }

    Heap* getPrimaryHeap()
    {
        return mHeaps[mPrimaryIndex];
    }

    Heap* getPrimaryHeap() const
    {
        return mHeaps[mPrimaryIndex];
    }

    s32 getPrimaryHeapIndex() const
    {
        return mPrimaryIndex;
    }

private:
    Heap* mHeaps[NUM_ROOT_HEAPS_MAX];
    bool mAdjusted[NUM_ROOT_HEAPS_MAX];
    s32 mPrimaryIndex;

    friend class TaskMgr;
    friend class TaskBase;
};

} // namespace sead
