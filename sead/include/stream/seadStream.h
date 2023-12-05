#pragma once

#include <prim/seadEndian.h>
#include <prim/seadSafeString.h>

namespace sead {

class StreamSrc;
class StreamFormat;

// TODO
class Stream
{
public:
    enum Modes
    {
        eBinary = 0,
        eText,
        eNum
    };

    class Text
    {
    public:
        Text(const SafeString&);

        const SafeString& getText() const { return mText; }

    private:
        SafeString mText;
    };

public:
    Stream();
    Stream(StreamSrc*, Modes);
    Stream(StreamSrc*, StreamFormat*);

    virtual ~Stream();

    void skip(u32);
    void skip(u32, u32);
    void rewind();
    bool isEOF();

    void setBinaryEndian(Endian::Types);
    void setMode(Modes);
    void setUserFormat(StreamFormat*);

protected:
    void setSrcStream_(StreamSrc*);

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
    ReadStream(StreamSrc*, Modes);
    ReadStream(StreamSrc*, StreamFormat*);

    ~ReadStream() override;
};

// TODO
class WriteStream : public ReadStream
{
public:
    WriteStream();
    WriteStream(StreamSrc*, Modes);
    WriteStream(StreamSrc*, StreamFormat*);

    ~WriteStream() override;
};

} // namespace sead
