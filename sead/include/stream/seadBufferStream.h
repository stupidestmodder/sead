#pragma once

#include <stream/seadStreamSrc.h>

namespace sead {

// TODO
class BufferWriteStreamSrc : public StreamSrc
{
public:
    BufferWriteStreamSrc(StreamSrc*, void*, u32);
    virtual ~BufferWriteStreamSrc();

    u32 skip(s32) override;
    u32 read(void*, u32) override;
    u32 write(const void*, u32) override;
    void rewind() override;
    bool isEOF() override;
    bool flush() override;

protected:
    StreamSrc* mStreamSrc;
    u8* mBufferAddr;
    u32 mBufferSize;
    u32 mCurrentPos;
};

// TODO
class BufferMultiByteTextWriteStreamSrc : public BufferWriteStreamSrc
{
public:
    BufferMultiByteTextWriteStreamSrc(StreamSrc*, void*, u32);
    ~BufferMultiByteTextWriteStreamSrc() override;

    u32 write(const void*, u32) override;
};

// TODO
class BufferMultiByteNullTerminatedTextWriteStreamSrc : public BufferMultiByteTextWriteStreamSrc
{
public:
    BufferMultiByteNullTerminatedTextWriteStreamSrc(StreamSrc*, void*, u32);
    ~BufferMultiByteNullTerminatedTextWriteStreamSrc() override;

    bool flush() override;
};

} // namespace sead
