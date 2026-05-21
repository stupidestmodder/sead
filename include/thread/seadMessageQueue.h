#pragma once

#include <basis/seadTypes.h>

#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
#include <container/seadRingBuffer.h>
#include <thread/seadCriticalSection.h>
#include <thread/seadEvent.h>
#endif // SEAD_PLATFORM_WINDOWS

namespace sead {

class MessageQueue
{
public:
    enum class BlockType
    {
        eBlock = 0,
        eNoBlock
    };

    using Element = intptr_t;

public:
    MessageQueue();
    ~MessageQueue();

    void allocate(s32 size, Heap* heap);
    void free();

    bool push(Element message, BlockType blockType);
    Element pop(BlockType blockType);
    Element peek(BlockType blockType) const;
    bool jam(Element message, BlockType blockType);

    static const Element cNullElement = 0;

#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
protected:
    bool push_(Element message);
    Element pop_();
    Element peek_() const;
    bool jam_(Element message);
#endif // SEAD_PLATFORM_WINDOWS

protected:
#if defined(SEAD_PLATFORM_WINDOWS) || defined(SEAD_PLATFORM_POSIX)
    mutable CriticalSection mCriticalSection;
    mutable Event mEvent;
    RingBuffer<Element> mBuffer;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead
