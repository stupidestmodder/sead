#pragma once

#include <basis/seadAssert.h>
#include <basis/seadNew.h>
#include <container/seadFreeList.h>
#include <container/seadListImpl.h>
#include <prim/seadPtrUtil.h>

namespace sead {

// TODO: Add all functions
template <typename T>
class ObjList : public ListImpl
{
private:
    struct Node
    {
        Node()
            : item()
            , link()
        {
        }

        T item;
        ListNode link;
    };

public:
    ObjList()
        : ListImpl()
        , mFreeList()
        , mLimitNum(0)
    {
    }

    ObjList(s32 limitNum, void* buf)
        : ListImpl()
        , mFreeList()
        , mLimitNum(0)
    {
        setBuffer(limitNum, buf);
    }

    void allocBuffer(s32 limitNum, s32 alignment = cDefaultAlignment)
    {
        if (!tryAllocBuffer(limitNum, alignment))
            AllocFailAssert(nullptr, limitNum * sizeof(Node), alignment);
    }

    void allocBuffer(s32 limitNum, Heap* heap, s32 alignment = cDefaultAlignment)
    {
        if (!tryAllocBuffer(limitNum, heap, alignment))
            AllocFailAssert(heap, limitNum * sizeof(Node), alignment);
    }

    bool tryAllocBuffer(s32 limitNum, s32 alignment = cDefaultAlignment)
    {
        SEAD_ASSERT(mFreeList.work() == nullptr);

        if (limitNum <= 0)
        {
            SEAD_ASSERT_MSG(false, "limitNum[%d] must be larger than zero", limitNum);
            return false;
        }

        void* buf = new(alignment, std::nothrow) u8[limitNum * sizeof(Node)];
        if (!buf)
            return false;

        setBuffer(limitNum, buf);

        return true;
    }

    bool tryAllocBuffer(s32 limitNum, Heap* heap, s32 alignment = cDefaultAlignment)
    {
        SEAD_ASSERT(mFreeList.work() == nullptr);

        if (limitNum <= 0)
        {
            SEAD_ASSERT_MSG(false, "limitNum[%d] must be larger than zero", limitNum);
            return false;
        }

        void* buf = new(heap, alignment, std::nothrow) u8[limitNum * sizeof(Node)];
        if (!buf)
            return false;

        setBuffer(limitNum, buf);

        return true;
    }

    void freeBuffer()
    {
        if (isBufferReady())
        {
            clear();

            delete[] static_cast<u8*>(mFreeList.work());
            mLimitNum = 0;

            mFreeList.cleanup();
        }
    }

    void setBuffer(s32 limitNum, void* buf)
    {
        if (limitNum <= 0)
        {
            SEAD_ASSERT_MSG(false, "limitNum[%d] must be larger than zero", limitNum);
            return;
        }

        if (!buf)
        {
            SEAD_ASSERT_MSG(false, "buf is null");
            return;
        }

        mFreeList.init(buf, sizeof(Node), limitNum);
        mLimitNum = limitNum;
    }

    bool isBufferReady() const { return mFreeList.work() != nullptr; }
    bool isFull() const { return mCount >= mLimitNum; }

    void clear()
    {
        ListNode* node = mStartEnd.next();

        while (node != &mStartEnd)
        {
            ListNode* temp = node->next();

            ListImpl::erase(node);

            {
                T* item = listNodeToObj(node);
                item->~T();
                mFreeList.put(item);
            }

            node = temp;
        }
    }

    T* birthBack()
    {
        if (isFull())
        {
            SEAD_ASSERT_MSG(false, "buffer full.");
            return nullptr;
        }

        Node* node = new(mFreeList.get()) Node();
        pushBack(objToListNode(&node->item));

        return &node->item;
    }

    T* birthFront()
    {
        if (isFull())
        {
            SEAD_ASSERT_MSG(false, "buffer full.");
            return nullptr;
        }

        Node* node = new(mFreeList.get()) Node();
        pushFront(objToListNode(&node->item));

        return &node->item;
    }

    T* birthBefore(const T* basis)
    {
        if (isFull())
        {
            SEAD_ASSERT_MSG(false, "buffer full.");
            return nullptr;
        }

        Node* node = new(mFreeList.get()) Node();
        insertBefore(objToListNode(basis), objToListNode(&node->item));

        return &node->item;
    }

    T* birthAfter(const T* basis)
    {
        if (isFull())
        {
            SEAD_ASSERT_MSG(false, "buffer full.");
            return nullptr;
        }

        Node* node = new(mFreeList.get()) Node();
        insertAfter(objToListNode(basis), objToListNode(&node->item));

        return &node->item;
    }

    void erase(T* obj)
    {
        ListImpl::erase(objToListNode(obj));
        obj->~T();
        mFreeList.put(obj);
    }

    T* front() { return listNodeToObjWithNullCheck(ListImpl::front()); }
    T* front() const { return listNodeToObjWithNullCheck(ListImpl::front()); }

    T* back() { return listNodeToObjWithNullCheck(ListImpl::back()); }
    T* back() const { return listNodeToObjWithNullCheck(ListImpl::back()); }

    T* prev(const T* obj)
    {
        ListNode* prev = objToListNode(obj)->prev();
        if (prev == &mStartEnd)
            return nullptr;

        return listNodeToObj(prev);
    }

    T* prev(const T* obj) const
    {
        ListNode* prev = objToListNode(obj)->prev();
        if (prev == &mStartEnd)
            return nullptr;

        return listNodeToObj(prev);
    }

    T* next(const T* obj)
    {
        ListNode* next = objToListNode(obj)->next();
        if (next == &mStartEnd)
            return nullptr;

        return listNodeToObj(next);
    }

    T* next(const T* obj) const
    {
        ListNode* next = objToListNode(obj)->next();
        if (next == &mStartEnd)
            return nullptr;

        return listNodeToObj(next);
    }

    // TODO: Iterators

protected:
    ListNode* objToListNode(const T* obj) const
    {
        return static_cast<ListNode*>(PtrUtil::addOffset(obj, offsetof(Node, link)));
    }

    T* listNodeToObj(const ListNode* node) const
    {
        return static_cast<T*>(PtrUtil::addOffset(node, -static_cast<intptr_t>(offsetof(Node, link))));
    }

    T* listNodeToObjWithNullCheck(const ListNode* node) const
    {
        return node ? listNodeToObj(node) : nullptr;
    }

protected:
    FreeList mFreeList;
    s32 mLimitNum;
};

template <typename T, s32 N>
class FixedObjList : public ObjList<T>
{
public:
    FixedObjList()
        : ObjList<T>(N, mWork)
        , mWork()
    {
    }

    void allocBuffer(s32 limitNum, s32 alignment = cDefaultAlignment) = delete;
    void allocBuffer(s32 limitNum, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 limitNum, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 limitNum, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    void freeBuffer() = delete;
    void setBuffer(s32 limitNum, void* buf) = delete;

protected:
    u8 mWork[N * sizeof(ObjList<T>::Node)];
};

} // namespace sead
