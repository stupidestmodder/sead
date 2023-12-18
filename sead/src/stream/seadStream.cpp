#include <stream/seadStream.h>

#include <stream/seadBinaryStreamFormat.h>
#include <stream/seadStreamSrc.h>
#include <stream/seadTextStreamFormat.h>

namespace sead {

BinaryStreamFormat sBinaryStreamInstance;
//TextStreamFormat sTextStreamInstance;

StreamFormat* const Stream::BASIC_STREAM_FORMAT[Stream::Modes::eNum] = {
    &sBinaryStreamInstance,
    // TODO
    //&sTextStreamInstance
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

u8 ReadStream::readU8()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readU8(mSrcStream, mSrcEndian);
}

void ReadStream::readU8(u8& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readU8();
}

u16 ReadStream::readU16()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readU16(mSrcStream, mSrcEndian);
}

void ReadStream::readU16(u16& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readU16();
}

u32 ReadStream::readU32()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readU32(mSrcStream, mSrcEndian);
}

void ReadStream::readU32(u32& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readU32();
}

u64 ReadStream::readU64()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readU64(mSrcStream, mSrcEndian);
}

void ReadStream::readU64(u64& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readU64();
}

s8 ReadStream::readS8()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readS8(mSrcStream, mSrcEndian);
}

void ReadStream::readS8(s8& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readS8();
}

s16 ReadStream::readS16()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readS16(mSrcStream, mSrcEndian);
}

void ReadStream::readS16(s16& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readS16();
}

s32 ReadStream::readS32()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readS32(mSrcStream, mSrcEndian);
}

void ReadStream::readS32(s32& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readS32();
}

s64 ReadStream::readS64()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readS64(mSrcStream, mSrcEndian);
}

void ReadStream::readS64(s64& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readS64();
}

f32 ReadStream::readF32()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readF32(mSrcStream, mSrcEndian);
}

void ReadStream::readF32(f32& dst)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    dst = readF32();
}

void ReadStream::readBit(void* dst, u32 bitnum)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->readBit(mSrcStream, dst, bitnum);
}

void ReadStream::readString(BufferedSafeString* dst, u32 size)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    SEAD_ASSERT(static_cast<u32>(dst->getBufferSize()) >= size);
    mFormat->readString(mSrcStream, dst, size);
}

u32 ReadStream::readMemBlock(void* dst, u32 size)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    return mFormat->readMemBlock(mSrcStream, dst, size);
}

// TODO: Idk if this is correct
f32 ReadStream::readF32BitImpl_(u32 intBitnum, u32 fracBitnum)
{
    u32 ret = 0;
    readBit(&ret, intBitnum + fracBitnum);
    ret = Endian::toHostU32(Endian::Types::eLittle, ret);

    return static_cast<f32>(ret) / static_cast<f32>(1 << (fracBitnum & 0xFF));
}

// TODO: Idk if this is correct
f64 ReadStream::readF64BitImpl_(u32 intBitnum, u32 fracBitnum)
{
    u64 ret = 0;
    readBit(&ret, intBitnum + fracBitnum);
    ret = Endian::toHostU64(Endian::Types::eLittle, ret);

    return static_cast<f64>(ret) / static_cast<f64>(1 << (fracBitnum & 0xFF));
}

void WriteStream::writeU8(u8 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeU8(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeU16(u16 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeU16(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeU32(u32 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeU32(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeU64(u64 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeU64(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeS8(s8 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeS8(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeS16(s16 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeS16(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeS32(s32 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeS32(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeS64(s64 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeS64(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeF32(f32 value)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeF32(mSrcStream, mSrcEndian, value);
}

void WriteStream::writeBit(const void* src, u32 bitnum)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeBit(mSrcStream, src, bitnum);
}

void WriteStream::writeString(const SafeString& src, u32 size)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    SEAD_ASSERT(static_cast<u32>(src.calcLength()) <= size);
    mFormat->writeString(mSrcStream, src, size);
}

void WriteStream::writeMemBlock(const void* src, u32 size)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeMemBlock(mSrcStream, src, size);
}

void WriteStream::writeComment(const SafeString& comment)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeDecorationText(mSrcStream, "/* ");
    mFormat->writeDecorationText(mSrcStream, comment);
    mFormat->writeDecorationText(mSrcStream, " */");
}

void WriteStream::writeLineComment(const SafeString& comment)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeDecorationText(mSrcStream, "// ");
    mFormat->writeDecorationText(mSrcStream, comment);
    mFormat->writeDecorationText(mSrcStream, "\n");
}

void WriteStream::writeDecorationText(const SafeString& text)
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeDecorationText(mSrcStream, text);
}

void WriteStream::writeNullChar()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->writeNullChar(mSrcStream);
}

void WriteStream::flush()
{
    SEAD_ASSERT(mFormat);
    SEAD_ASSERT(mSrcStream);
    mFormat->flush(mSrcStream);
    mSrcStream->flush();
}

// TODO: Idk if this is correct
void WriteStream::writeF32BitImpl_(f32 value, u32 intBitnum, u32 fracBitnum)
{
    SEAD_ASSERT(value >= 0.0f);

    u32 uValue = static_cast<u32>(value * static_cast<f32>(1 << (fracBitnum & 0xFF)) + 0.5f);
    uValue = Endian::fromHostU32(Endian::Types::eLittle, uValue);

    writeBit(&uValue, intBitnum + fracBitnum);
}

// TODO: Idk if this is correct
void WriteStream::writeF64BitImpl_(f64 value, u32 intBitnum, u32 fracBitnum)
{
    SEAD_ASSERT(value >= 0.0);

    u64 uValue = static_cast<u64>(value * static_cast<f64>(1 << (fracBitnum & 0xFF)) + 0.5);
    uValue = Endian::fromHostU64(Endian::Types::eLittle, uValue);

    writeBit(&uValue, intBitnum + fracBitnum);
}

} // namespace sead
