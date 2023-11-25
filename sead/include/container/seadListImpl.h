#pragma once

#include <basis/seadTypes.h>

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

    void insertBack_(ListNode* node);
    void insertFront_(ListNode* node);
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
    using CompareCallbackImpl = s32 (*)(const void*, const void*);

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
    void shuffle();
    void shuffle(Random* random);
    bool checkLinks() const;

protected:
    void sort(s32 offset, CompareCallbackImpl cmp);
    void mergeSort(s32 offset, CompareCallbackImpl cmp);

    void pushBack(ListNode* node)
    {
        mStartEnd.insertFront_(node);
        mCount++;
    }

    void pushFront(ListNode* node)
    {
        mStartEnd.insertBack_(node);
        mCount++;
    }

    ListNode* popBack();
    ListNode* popFront();

    void insertBefore(ListNode* basis, ListNode* node)
    {
        basis->insertFront_(node);
        mCount++;
    }

    void insertAfter(ListNode* basis, ListNode* node)
    {
        basis->insertBack_(node);
        mCount++;
    }

    void erase(ListNode* node)
    {
        node->erase_();
        mCount--;
    }

    ListNode* front() const { return mCount > 0 ? mStartEnd.mNext : nullptr; }
    ListNode* back() const { return mCount > 0 ? mStartEnd.mPrev : nullptr; }

    ListNode* nth(s32 index) const;
    s32 indexOf(const ListNode* node) const;
    void swap(ListNode* n1, ListNode* n2);
    void moveAfter(ListNode* basis, ListNode* node);
    void moveBefore(ListNode* basis, ListNode* node);
    ListNode* find(const void* ptr, s32 offset, CompareCallbackImpl cmp) const;
    void uniq(s32 offset, CompareCallbackImpl cmp);
    void clear();
    void unsafeClear();

    static void mergeSortImpl(ListNode* front, ListNode* back, s32 num, s32 offset, CompareCallbackImpl cmp);

protected:
    ListNode mStartEnd;
    s32 mCount;
};

} // namespace sead
