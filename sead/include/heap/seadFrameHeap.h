#pragma once

#include <heap/seadHeap.h>

namespace sead {

// TODO
class FrameHeap : public Heap
{
    SEAD_RTTI_OVERRIDE(FrameHeap, Heap);

public:
    struct State
    {
        State()
            : mHeadPtr(nullptr)
            , mTailPtr(nullptr)
        {
        }

        void* mHeadPtr;
        void* mTailPtr;
    };

public:
    static FrameHeap* create(size_t size, const SafeString& name, Heap* parent, HeapDirection direction = HeapDirection::eForward, bool enableLock = false);

    static FrameHeap* tryCreate(size_t size, const SafeString& name, Heap* parent, HeapDirection direction = HeapDirection::eForward, bool enableLock = false);

    static size_t getManagementAreaSize(s32 alignment = cMinAlignment);

protected:
    FrameHeap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock);
    ~FrameHeap() override;

public:
    void destroy() override;
    size_t adjust() override;
    void* tryAlloc(size_t size, s32 alignment = cMinAlignment) override;
    void free(void* ptr) override;
    void* resizeFront(void* ptr, size_t newSize) override;
    void* resizeBack(void* ptr, size_t newSize) override;
    void freeAll() override;

    void freeHead();
    void freeTail();

    const void* getStartAddress() const override;
    const void* getEndAddress() const override;
    size_t getSize() const override;
    size_t getFreeSize() const override;
    size_t getMaxAllocatableSize(s32 alignment = cMinAlignment) const override;
    bool isInclude(const void* ptr) const override;
    bool isEmpty() const override;
    bool isFreeable() const override;
    bool isResizable() const override;
    bool isAdjustable() const override;
    void dump() const override;
    void dumpYAML(WriteStream& stream, s32 indent) const override;

    void restoreState(const State& state);

protected:
    void initialize_();

    void* getAreaStart_() const;
    void* getAreaEnd_() const;

    size_t getAreaSize_() const
    {
        return mSize - sizeof(FrameHeap);
    }

    size_t adjustBack_();
    size_t adjustFront_();

protected:
    State mState;
};

} // namespace sead