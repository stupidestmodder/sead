#include <stream/seadStream.h>

#include <stream/seadBinaryStreamFormat.h>
#include <stream/seadStreamSrc.h>
#include <stream/seadTextStreamFormat.h>

namespace sead {

BinaryStreamFormat sBinaryStreamInstance;
TextStreamFormat sTextStreamInstance;

StreamFormat* const Stream::BASIC_STREAM_FORMAT[Stream::Modes::eNum] = {
    &sBinaryStreamInstance,
    &sTextStreamInstance
};

Stream::Stream()
    : mFormat(nullptr)
    , mSrcStream(nullptr)
    , mSrcEndian(Endian::getHostEndian())
{
}

Stream::Stream(StreamSrc* src, Modes mode)
    : mFormat(nullptr)
    , mSrcStream(src)
    , mSrcEndian(Endian::getHostEndian())
{
    setMode(mode);
}

Stream::Stream(StreamSrc* src, StreamFormat* format)
    : mFormat(nullptr)
    , mSrcStream(src)
    , mSrcEndian(Endian::getHostEndian())
{
    setUserFormat(format);
}

void Stream::skip(u32 size)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);

    mFormat->skip(mSrcStream, size);
}

void Stream::skip(u32 size, u32 count)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);

    for (u32 i = 0; i < count; i++)
    {
        mFormat->skip(mSrcStream, size);
    }
}

void Stream::rewind()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);

    mFormat->rewind(mSrcStream);
}

bool Stream::isEOF()
{
    SEAD_ASSERT(mSrcStream);

    return mSrcStream->isEOF();
}

void Stream::setBinaryEndian(Endian::Types types)
{
    mSrcEndian = types;
}

void Stream::setMode(Modes mode)
{
    SEAD_ASSERT(0 <= mode && mode < Modes::eNum);

    setUserFormat(BASIC_STREAM_FORMAT[mode]);
}

void Stream::setUserFormat(StreamFormat* format)
{
    mFormat = format;
}

} // namespace sead
