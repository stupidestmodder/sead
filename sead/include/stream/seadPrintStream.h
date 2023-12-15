#pragma once

#include <stream/seadBufferStream.h>
#include <stream/seadStream.h>
#include <stream/seadStreamSrc.h>

namespace sead {

class PrintStreamSrc : public StreamSrc
{
public:
    u32 read(void* dst, u32 size) override;
    u32 write(const void* src, u32 size) override;
    u32 skip(s32 byte) override;
    void rewind() override;
    bool isEOF() override { return false; }

    static PrintStreamSrc* instance() { return &sPrintStreamSrc; }

protected:
    static PrintStreamSrc sPrintStreamSrc;
};

class PrintWriteStream : public WriteStream
{
public:
    PrintWriteStream(Modes mode);
    PrintWriteStream(StreamFormat* format);
    ~PrintWriteStream() override;

    static const u32 cBufferSize = 128;

private:
    BufferMultiByteNullTerminatedTextWriteStreamSrc mBufferStreamSrc;
    u8 mBuffer[cBufferSize];
};

} // namespace sead
