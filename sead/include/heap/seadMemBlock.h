#pragma once

#include <basis/seadRawPrint.h>
#include <container/seadOffsetList.h>
#include <prim/seadMemUtil.h>
#include <prim/seadPtrUtil.h>

namespace sead {

class MemBlock
{
public:
    static const u32 cPtrSize = sizeof(void*);

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
            intptr_t* offsetTail = reinterpret_cast<intptr_t*>(memory() - cPtrSize);
            SEAD_ASSERT(!PtrUtil::isInclude(offsetTail, this, PtrUtil::addOffset(this, sizeof(MemBlock))));

            //? Huh
            *offsetTail = reinterpret_cast<intptr_t>(this) + 1;
        }
    }

    void fill(u8 val)
    {
        MemUtil::fill(memory(), val, mSize);
    }

    // TODO
    bool isInclude(const void*) const;

    friend class ExpHeap;

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

    bool isValid(const void* heapStart, size_t heapSize) const
    {
        if (mSize == 0)
            return false;

        const u8* heapStartU8 = reinterpret_cast<const u8*>(heapStart);
        const u8* memBlock = reinterpret_cast<const u8*>(this);

        if (memBlock < heapStartU8 || heapStartU8 + heapSize < memBlock + getSizeWithManage())
        {
            SEAD_PRINT("Invalid address: 0x%p\n", memBlock);
            return false;
        }

        u8* next = reinterpret_cast<u8*>(mListNode.next());
        if (next < heapStartU8 || heapStartU8 + heapSize < next + sizeof(MemBlock))
        {
            SEAD_PRINT("Invalid next address: 0x%p\n", next);
            return false;
        }

        u8* prev = reinterpret_cast<u8*>(mListNode.prev());
        if (prev < heapStartU8 || heapStartU8 + heapSize < prev + sizeof(MemBlock))
        {
            SEAD_PRINT("Invalid prev address: 0x%p\n", prev);
            return false;
        }

        if (mListNode.prev()->next() != &mListNode || mListNode.next()->prev() != &mListNode)
        {
            SEAD_PRINT("Invalid bidirectional link\n");
            return false;
        }

        return true;
    }

    void dump() const
    {
        SEAD_PRINT("addr: 0x%p\n", this);
        SEAD_PRINT("memory: 0x%p\n", memory());
        SEAD_PRINT("size: 0x%p\n", getSize());
        SEAD_PRINT("prev: 0x%p\n", mListNode.prev());
        SEAD_PRINT("next: 0x%p\n", mListNode.next());
    }

    static MemBlock* FindManageArea(void* ptr)
    {
        MemBlock* memBlock;

        uintptr_t blockHeaderAddr = *reinterpret_cast<uintptr_t*>(reinterpret_cast<intptr_t>(ptr) - cPtrSize);
        if ((blockHeaderAddr & 1) == 0)
        {
            memBlock = reinterpret_cast<MemBlock*>(reinterpret_cast<intptr_t>(ptr) - sizeof(MemBlock));
            if (memBlock->getOffset() != 0)
            {
                SEAD_PRINT("===================================\n");
                SEAD_PRINT("Invalid pointer: 0x%p\n", ptr);
                SEAD_PRINT("[ block (0x%p) ]\n", memBlock);
                MemUtil::dumpMemoryBinary(memBlock, 16 * 0, 16 * 2, true);
                SEAD_PRINT("\n");
                SEAD_PRINT("[ ptr (0x%p) ]\n", ptr);
                MemUtil::dumpMemoryBinary(ptr, 16 * 2, 16 * 2, true);
                SEAD_PRINT("===================================\n");

                SEAD_ASSERT_MSG(false, "Invalid pointer: 0x%p\n", ptr);
            }
        }
        else
        {
            memBlock = reinterpret_cast<MemBlock*>(blockHeaderAddr - 1);
        }

        return memBlock;
    }

protected:
    ListNode mListNode;
    u16 mHeapCheckTag;
    u16 mOffset;
    size_t mSize;
};

using MemBlockList = OffsetList<MemBlock>;

} // namespace sead
