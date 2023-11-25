#pragma once

#include <basis/seadRawPrint.h>
#include <container/seadOffsetList.h>
#include <prim/seadMemUtil.h>
#include <prim/seadPtrUtil.h>

namespace sead {

// TODO
class MemBlock
{
protected:
    MemBlock()
        : mListNode()
        , mHeapCheckTag(0)
        , mOffset(0)
        , mSize(0)
    {
    }

    u8* memory() const
    {
        return static_cast<u8*>(PtrUtil::addOffset(this, mOffset + sizeof(MemBlock)));
    }

    ListNode& getListNode()
    {
        return mListNode;
    }

    void setHeapCheckTag(u16 tag)
    {
        mHeapCheckTag = tag;
    }

    void setSize(size_t size)
    {
        SEAD_ASSERT(size % cPtrSize == 0);
        mSize = size;
    }

    void setOffset(u16 offset)
    {
        SEAD_ASSERT(offset % cPtrSize == 0);
        mOffset = offset;

        if (mOffset != 0)
        {
            uintptr_t* offsetTail = memory() - cPtrSize;
            SEAD_ASSERT(!PtrUtil::isInclude(offsetTail, this, PtrUtil::addOffset(this, sizeof(MemBlock))));
            *offsetTail = PtrUtil::addOffset(this, 1);
        }
    }

    void fill(u8 val)
    {
        MemUtil::fill(memory(), val, mSize);
    }

    bool isInclude(const void*) const;

public:
    size_t getSize() const
    {
        return mSize;
    }

    size_t getSizeWithManage() const
    {
        return mOffset + sizeof(MemBlock) + mSize;
    }

    u32 getOffset() const
    {
        return mOffset;
    }

    bool isValid(const void* heapStart, size_t heapSize) const;

    void dump() const
    {
        SEAD_PRINT("addr: 0x%p\n", this);
        SEAD_PRINT("memory: 0x%p\n", memory());
        SEAD_PRINT("size: 0x%p\n", getSize());
        SEAD_PRINT("prev: 0x%p\n", mListNode.prev());
        SEAD_PRINT("next: 0x%p\n", mListNode.next());
    }

    static MemBlock* FindManageArea(void* ptr);

    static const u32 cPtrSize = sizeof(void*);

protected:
    ListNode mListNode;
    u16 mHeapCheckTag;
    u16 mOffset;
    size_t mSize;
};

using MemBlockList = OffsetList<MemBlock>;

} // namespace sead
