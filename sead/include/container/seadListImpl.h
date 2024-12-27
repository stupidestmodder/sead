#pragma once

#include <random/seadRandom.h>

namespace sead {

class Random;

class ListNode
{
    SEAD_NO_COPY(ListNode);

public:
    ListNode()
        : mPrev(nullptr)
        , mNext(nullptr)
    {
    }

    ListNode* prev() const { return mPrev; }
    ListNode* next() const { return mNext; }

    bool isLinked() const { return mPrev || mNext; }

private:
    void init_()
    {
        mPrev = nullptr;
        mNext = nullptr;
    }

    void insertBack_(ListNode* n);
    void insertFront_(ListNode* n);
    void erase_();

private:
    ListNode* mPrev;
    ListNode* mNext;

    friend class ListImpl;
};

class ListImpl
{
    SEAD_NO_COPY(ListImpl);

protected:
    using CompareCallbackImpl = s32 (*)(const void* a, const void* b);

public:
    ListImpl()
        : mStartEnd()
        , mCount(0)
    {
        mStartEnd.mPrev = &mStartEnd;
        mStartEnd.mNext = &mStartEnd;
    }

    bool isEmpty() const { return mCount == 0; }

    s32 size() const { return mCount; }
    s32 getSize() const { return mCount; }

    void reverse();

    void shuffle()
    {
        Random random;
        shuffle(&random);
    }

    void shuffle(Random* random);
    bool checkLinks() const;

protected:
    void sort(s32 offset, CompareCallbackImpl cmp);
    void mergeSort(s32 offset, CompareCallbackImpl cmp);

    void pushBack(ListNode* n)
    {
        mStartEnd.insertFront_(n);
        mCount++;
    }

    void pushFront(ListNode* n)
    {
        mStartEnd.insertBack_(n);
        mCount++;
    }

    ListNode* popBack();
    ListNode* popFront();

    void insertBefore(ListNode* basis, ListNode* n)
    {
        basis->insertFront_(n);
        mCount++;
    }

    void insertAfter(ListNode* basis, ListNode* n)
    {
        basis->insertBack_(n);
        mCount++;
    }

    void erase(ListNode* n)
    {
        n->erase_();
        mCount--;
    }

    ListNode* front() const { return mCount > 0 ? mStartEnd.mNext : nullptr; }
    ListNode* back() const { return mCount > 0 ? mStartEnd.mPrev : nullptr; }

    ListNode* nth(s32 index) const;
    s32 indexOf(const ListNode* n) const;

    void swap(ListNode* n1, ListNode* n2);
    void moveAfter(ListNode* basis, ListNode* n);
    void moveBefore(ListNode* basis, ListNode* n);

    ListNode* find(const void* ptr, s32 offset, CompareCallbackImpl cmp) const;
    void uniq(s32 offset, CompareCallbackImpl cmp);

    void clear();

    void unsafeClear()
    {
        mCount = 0;
        mStartEnd.mPrev = &mStartEnd;
        mStartEnd.mNext = &mStartEnd;
    }

    static void mergeSortImpl(ListNode* front, ListNode* back, s32 num, s32 offset, CompareCallbackImpl cmp);

protected:
    ListNode mStartEnd;
    s32 mCount;
};

} // namespace sead
