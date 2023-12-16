#pragma once

#include <stream/seadStreamSrc.h>

namespace sead {

// TODO: Add remaining classes

class BufferWriteStreamSrc : public StreamSrc
{
public:
    BufferWriteStreamSrc(StreamSrc* src, void* start, u32 size);
    virtual ~BufferWriteStreamSrc();

    u32 read(void* dst, u32 size) override;
    u32 write(const void* src, u32 size) override;
    u32 skip(s32 byte) override;
    void rewind() override;
    bool isEOF() override { return mStreamSrc->isEOF(); }
    bool flush() override;

protected:
    StreamSrc* mStreamSrc;
    u8* mBufferAddr;
    u32 mBufferSize;
    u32 mCurrentPos;
};

class BufferMultiByteTextWriteStreamSrc : public BufferWriteStreamSrc
{
public:
    BufferMultiByteTextWriteStreamSrc(StreamSrc* src, void* start, u32 size);

    ~BufferMultiByteTextWriteStreamSrc() override
    {
    }

    u32 write(const void* src, u32 size) override;
};

class BufferMultiByteNullTerminatedTextWriteStreamSrc : public BufferMultiByteTextWriteStreamSrc
{
public:
    BufferMultiByteNullTerminatedTextWriteStreamSrc(StreamSrc* src, void* start, u32 size)
        : BufferMultiByteTextWriteStreamSrc(src, start, size - 1)
    {
    }

    ~BufferMultiByteNullTerminatedTextWriteStreamSrc() override
    {
    }

    bool flush() override;
};

} // namespace sead
