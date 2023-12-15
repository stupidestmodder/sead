#include <stream/seadPrintStream.h>

#include <basis/seadRawPrint.h>

namespace sead {

PrintStreamSrc PrintStreamSrc::sPrintStreamSrc;

u32 PrintStreamSrc::read(void*, u32)
{
    return 0;
}

u32 PrintStreamSrc::write(const void* src, u32 size)
{
    system::PrintString(static_cast<const char*>(src), size);
    return size;
}

u32 PrintStreamSrc::skip(s32)
{
    return 0;
}

void PrintStreamSrc::rewind()
{
}

PrintWriteStream::PrintWriteStream(Modes mode)
    : WriteStream()
    , mBufferStreamSrc(PrintStreamSrc::instance(), mBuffer, cBufferSize)
{
    setSrcStream_(&mBufferStreamSrc);
    setMode(mode);
}

PrintWriteStream::PrintWriteStream(StreamFormat* format)
    : WriteStream()
    , mBufferStreamSrc(PrintStreamSrc::instance(), mBuffer, cBufferSize)
{
    setSrcStream_(&mBufferStreamSrc);
    setUserFormat(format);
}

PrintWriteStream::~PrintWriteStream()
{
    flush();
    setSrcStream_(nullptr);
}

} // namespace sead
