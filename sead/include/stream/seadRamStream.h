#pragma once

#include <stream/seadStream.h>
#include <stream/seadStreamSrc.h>

namespace sead {

class RamStreamSrc : public StreamSrc
{
public:
    RamStreamSrc(void* start, u32 size);
    virtual ~RamStreamSrc();

    u32 read(void* dst, u32 size) override;
    u32 write(const void* src, u32 size) override;
    u32 skip(s32 byte) override;
    void rewind() override { mCurPos = 0; }
    bool isEOF() override { return mCurPos >= mSize; }

    u8* getCurrentAddres() const { return mStartAddr + mCurPos; }
    u32 getCurrentPosition() const { return mCurPos; }

protected:
    u8* mStartAddr;
    u32 mSize;
    u32 mCurPos;
};

class RamReadStream : public ReadStream
{
public:
    RamReadStream(const void* start, u32 size, Modes mode);
    RamReadStream(const void* start, u32 size, StreamFormat* format);
    ~RamReadStream() override;

    u8* getCurrentAddres() const { return mRamStreamSrc.getCurrentAddres(); }
    u32 getCurrentPosition() const { return mRamStreamSrc.getCurrentPosition(); }

protected:
    RamStreamSrc mRamStreamSrc;
};

class RamWriteStream : public WriteStream
{
public:
    RamWriteStream(void* start, u32 size, Modes mode);
    RamWriteStream(void* start, u32 size, StreamFormat* format);
    ~RamWriteStream() override;

    u8* getCurrentAddres() const { return mRamStreamSrc.getCurrentAddres(); }
    u32 getCurrentPosition() const { return mRamStreamSrc.getCurrentPosition(); }

protected:
    RamStreamSrc mRamStreamSrc;
};

} // namespace sead
