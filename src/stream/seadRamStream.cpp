#include <stream/seadRamStream.h>

namespace sead {

RamStreamSrc::RamStreamSrc(void* start, u32 size)
    : mStartAddr(static_cast<u8*>(start))
    , mSize(size)
    , mCurPos(0)
{
}

RamStreamSrc::~RamStreamSrc()
{
}

u32 RamStreamSrc::read(void* dst, u32 size)
{
    if (mCurPos + size > mSize)
    {
        size = mSize - mCurPos;
    }

    sead::MemUtil::copy(dst, mStartAddr + mCurPos, size);
    mCurPos += size;
    return size;
}

u32 RamStreamSrc::write(const void* src, u32 size)
{
    if (mCurPos + size > mSize)
    {
        size = mSize - mCurPos;
    }

    sead::MemUtil::copy(mStartAddr + mCurPos, src, size);
    mCurPos += size;
    return size;
}

u32 RamStreamSrc::skip(s32 byte)
{
    if (byte > 0 && mCurPos + byte > mSize)
    {
        byte = mSize - mCurPos;
    }

    if (byte < 0 && static_cast<s32>(mCurPos) < -byte)
    {
        byte = -static_cast<s32>(mCurPos);
    }

    mCurPos += byte;
    return byte;
}

RamReadStream::RamReadStream(const void* start, u32 size, Modes mode)
    : ReadStream()
    , mRamStreamSrc(const_cast<void*>(start), size)
{
    setSrcStream_(&mRamStreamSrc);
    setMode(mode);
}

RamReadStream::RamReadStream(const void* start, u32 size, StreamFormat* format)
    : ReadStream()
    , mRamStreamSrc(const_cast<void*>(start), size)
{
    setSrcStream_(&mRamStreamSrc);
    setUserFormat(format);
}

RamReadStream::~RamReadStream()
{
    setSrcStream_(nullptr);
}

RamWriteStream::RamWriteStream(void* start, u32 size, Modes mode)
    : WriteStream()
    , mRamStreamSrc(start, size)
{
    setSrcStream_(&mRamStreamSrc);
    setMode(mode);
}

RamWriteStream::RamWriteStream(void* start, u32 size, StreamFormat* format)
    : WriteStream()
    , mRamStreamSrc(start, size)
{
    setSrcStream_(&mRamStreamSrc);
    setUserFormat(format);
}

RamWriteStream::~RamWriteStream()
{
    flush();
    setSrcStream_(nullptr);
}

} // namespace sead
