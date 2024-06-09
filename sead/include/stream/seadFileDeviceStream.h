#pragma once

#include <filedevice/seadFileDevice.h>
#include <stream/seadStream.h>
#include <stream/seadStreamSrc.h>

namespace sead {

class FileDeviceStreamSrc : public StreamSrc
{
public:
    FileDeviceStreamSrc();
    FileDeviceStreamSrc(FileHandle* handle);
    virtual ~FileDeviceStreamSrc();

    u32 read(void* dst, u32 size) override;
    u32 write(const void* src, u32 size) override;
    u32 skip(s32 byte) override;
    void rewind() override;
    bool isEOF() override;

    void setFileHandle(FileHandle* handle);

    bool isValid() const { return mHandle != nullptr; }

protected:
    FileHandle* mHandle;
    u32 mBeginPos;
    FileHandle mHandleTemp;
    bool mNeedClose;
    u32 mFileSize;
};

class FileDeviceReadStream : public ReadStream
{
public:
    FileDeviceReadStream(Modes mode);
    FileDeviceReadStream(StreamFormat* format);
    FileDeviceReadStream(FileHandle* handle, Modes mode);
    FileDeviceReadStream(FileHandle* handle, StreamFormat* format);
    ~FileDeviceReadStream() override;

    void setFileHandle(FileHandle* handle);

    bool isValid() const { return mFileDeviceStreamSrc.isValid(); }

protected:
    FileDeviceStreamSrc mFileDeviceStreamSrc;
};

class FileDeviceWriteStream : public WriteStream
{
public:
    FileDeviceWriteStream(Modes mode);
    FileDeviceWriteStream(StreamFormat* format);
    FileDeviceWriteStream(FileHandle* handle, Modes mode);
    FileDeviceWriteStream(FileHandle* handle, StreamFormat* format);
    ~FileDeviceWriteStream() override;

    void setFileHandle(FileHandle* handle);

    bool isValid() const { return mFileDeviceStreamSrc.isValid(); }

protected:
    FileDeviceStreamSrc mFileDeviceStreamSrc;
};

// TODO: Add other classes

} // namespace sead
