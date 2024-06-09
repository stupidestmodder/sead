#include <stream/seadFileDeviceStream.h>

namespace sead {

FileDeviceStreamSrc::FileDeviceStreamSrc()
    : StreamSrc()
    , mHandle(nullptr)
    , mBeginPos(0)
    , mHandleTemp()
    , mNeedClose(false)
    , mFileSize(0)
{
}

FileDeviceStreamSrc::FileDeviceStreamSrc(FileHandle* handle)
    : StreamSrc()
    , mHandle(handle)
    , mBeginPos(handle->getCurrentSeekPos())
    , mHandleTemp()
    , mNeedClose(false)
    , mFileSize(handle->getFileSize())
{
}

FileDeviceStreamSrc::~FileDeviceStreamSrc()
{
    if (mNeedClose && mHandle)
        mHandle->close();
}

u32 FileDeviceStreamSrc::read(void* dst, u32 size)
{
    SEAD_ASSERT(mHandle);
    return mHandle->read(static_cast<u8*>(dst), size);
}

u32 FileDeviceStreamSrc::write(const void* src, u32 size)
{
    SEAD_ASSERT(mHandle);
    return mHandle->write(static_cast<const u8*>(src), size);
}

u32 FileDeviceStreamSrc::skip(s32 byte)
{
    SEAD_ASSERT(mHandle);
    if (!mHandle->seek(byte, FileDevice::SeekOrigin::eCurrent))
        return 0;

    return byte;
}

void FileDeviceStreamSrc::rewind()
{
    SEAD_ASSERT(mHandle);
    mHandle->seek(mBeginPos, FileDevice::SeekOrigin::eBegin);
}

bool FileDeviceStreamSrc::isEOF()
{
    SEAD_ASSERT(mHandle);
    return mHandle->getCurrentSeekPos() >= mFileSize;
}

void FileDeviceStreamSrc::setFileHandle(FileHandle* handle)
{
    mHandle = handle;

    if (mHandle)
    {
        mBeginPos = mHandle->getCurrentSeekPos();
        mFileSize = mHandle->getFileSize();
    }
}

FileDeviceReadStream::FileDeviceReadStream(Modes mode)
    : ReadStream()
    , mFileDeviceStreamSrc()
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setMode(mode);
}

FileDeviceReadStream::FileDeviceReadStream(StreamFormat* format)
    : ReadStream()
    , mFileDeviceStreamSrc()
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setUserFormat(format);
}

FileDeviceReadStream::FileDeviceReadStream(FileHandle* handle, Modes mode)
    : ReadStream()
    , mFileDeviceStreamSrc(handle)
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setMode(mode);
}

FileDeviceReadStream::FileDeviceReadStream(FileHandle* handle, StreamFormat* format)
    : ReadStream()
    , mFileDeviceStreamSrc(handle)
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setUserFormat(format);
}

FileDeviceReadStream::~FileDeviceReadStream()
{
    setSrcStream_(nullptr);
}

void FileDeviceReadStream::setFileHandle(FileHandle* handle)
{
    if (isValid())
        rewind();

    mFileDeviceStreamSrc.setFileHandle(handle);
}

FileDeviceWriteStream::FileDeviceWriteStream(Modes mode)
    : WriteStream()
    , mFileDeviceStreamSrc()
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setMode(mode);
}

FileDeviceWriteStream::FileDeviceWriteStream(StreamFormat* format)
    : WriteStream()
    , mFileDeviceStreamSrc()
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setUserFormat(format);
}

FileDeviceWriteStream::FileDeviceWriteStream(FileHandle* handle, Modes mode)
    : WriteStream()
    , mFileDeviceStreamSrc(handle)
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setMode(mode);
}

FileDeviceWriteStream::FileDeviceWriteStream(FileHandle* handle, StreamFormat* format)
    : WriteStream()
    , mFileDeviceStreamSrc(handle)
{
    setSrcStream_(&mFileDeviceStreamSrc);
    setUserFormat(format);
}

FileDeviceWriteStream::~FileDeviceWriteStream()
{
    flush();
    setSrcStream_(nullptr);
}

void FileDeviceWriteStream::setFileHandle(FileHandle* handle)
{
    if (isValid())
    {
        flush();
        rewind();
    }

    mFileDeviceStreamSrc.setFileHandle(handle);
}

} // namespace sead
