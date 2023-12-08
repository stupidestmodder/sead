#pragma once

#include <prim/seadEndian.h>
#include <prim/seadSafeString.h>

namespace sead {

class StreamSrc;
class StreamFormat;

class Stream
{
public:
    enum Modes
    {
        eBinary = 0,
        eText,
        eNum
    };

    // TODO
    class Text
    {
    public:
        Text(const SafeString&);

        const SafeString& getText() const { return mText; }

    private:
        SafeString mText;
    };

    // TODO
    template <typename T>
    class FloatRangeBitImpl
    {
    public:
        FloatRangeBitImpl(T*, s64, u32, u32, bool);

        T getValue() const;
        T* getValuePtr();

        s64 mMin;
        u32 mIntBitNum;
        u32 mFracBitNum;
        bool mReadOnly;

    private:
        T* mValue;
    };

public:
    Stream();
    Stream(StreamSrc* src, Modes mode);
    Stream(StreamSrc* src, StreamFormat* format);

    virtual ~Stream()
    {
    }

    void skip(u32 size);
    void skip(u32 size, u32 count);
    void rewind();
    bool isEOF();

    void setBinaryEndian(Endian::Types types);
    void setMode(Modes mode);
    void setUserFormat(StreamFormat* format);

protected:
    void setSrcStream_(StreamSrc* src)
    {
        mSrcStream = src;
    }

private:
    static StreamFormat* const BASIC_STREAM_FORMAT[Modes::eNum];

protected:
    StreamFormat* mFormat;
    StreamSrc* mSrcStream;
    Endian::Types mSrcEndian;
};

// TODO
class ReadStream : public Stream
{
public:
    ReadStream();
    ReadStream(StreamSrc* src, Modes mode);
    ReadStream(StreamSrc* src, StreamFormat* format);

    ~ReadStream() override;

    u8 readU8();
    void readU8(u8& dst);
    u16 readU16();
    void readU16(u16& dst);
    u32 readU32();
    void readU32(u32& dst);
    u64 readU64();
    void readU64(u64& dst);

    s8 readS8();
    void readS8(s8& dst);
    s16 readS16();
    void readS16(s16& dst);
    s32 readS32();
    void readS32(s32& dst);
    s64 readS64();
    void readS64(s64& dst);

    f32 readF32();
    void readF32(f32& dst);

    void readBit(void* dst, u32 bitnum);
    void readString(BufferedSafeString* dst, u32 size);
    u32 readMemBlock(void* dst, u32 size);

    ReadStream& operator>>(Text);
    ReadStream& operator>>(FloatRangeBitImpl<f32>);
    ReadStream& operator>>(FloatRangeBitImpl<f64>);

private:
    f32 readF32BitImpl_(u32 intBitnum, u32 fracBitnum);
    f64 readF64BitImpl_(u32 intBitnum, u32 fracBitnum);
};

// TODO
class WriteStream : public ReadStream
{
public:
    WriteStream();
    WriteStream(StreamSrc* src, Modes mode);
    WriteStream(StreamSrc* src, StreamFormat* format);

    ~WriteStream() override;

    void writeU8(u8 value);
    void writeU16(u16 value);
    void writeU32(u32 value);
    void writeU64(u64 value);

    void writeS8(s8 value);
    void writeS16(s16 value);
    void writeS32(s32 value);
    void writeS64(s64 value);

    void writeF32(f32 value);

    void writeBit(const void* src, u32 bitnum);
    void writeString(const SafeString& src, u32 size);
    void writeMemBlock(const void* src, u32 size);
    void writeComment(const SafeString& comment);
    void writeLineComment(const SafeString& comment);
    void writeDecorationText(const SafeString& text);
    void writeNullChar();
    void flush();

private:
    void writeF32BitImpl_(f32 value, u32 intBitnum, u32 fracBitnum);
    void writeF64BitImpl_(f64 value, u32 intBitnum, u32 fracBitnum);

    WriteStream& writeFloatRangeBitImpl(const FloatRangeBitImpl<f32>&);
    WriteStream& writeDoubleRangeBitImpl(const FloatRangeBitImpl<f64>&);
};

} // namespace sead
