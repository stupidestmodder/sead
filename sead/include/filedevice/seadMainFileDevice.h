#pragma once

#include <filedevice/seadFileDevice.h>

namespace sead {

class MainFileDevice : public FileDevice
{
    SEAD_RTTI_OVERRIDE(MainFileDevice, FileDevice);

public:
    explicit MainFileDevice(Heap* heap);
    ~MainFileDevice() override;

    void traceFilePath(const SafeString& path) const override;
    void traceDirectoryPath(const SafeString& path) const override;
    void resolveFilePath(BufferedSafeString* out, const SafeString& path) const override;
    void resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const override;

protected:
    bool doIsAvailable_() const override
    {
        return mFileDevice->isAvailable();
    }

    FileDevice* doOpen_(FileHandle* handle, const SafeString& filename, FileOpenFlag flag) override
    {
        return mFileDevice->tryOpen(handle, filename, flag, handle->getDivSize());
    }

    bool doClose_(FileHandle* handle) override
    {
        return mFileDevice->tryClose(handle);
    }

    bool doFlush_(FileHandle* handle) override
    {
        return mFileDevice->tryFlush(handle);
    }

    bool doRemove_(const SafeString& path) override
    {
        return mFileDevice->tryRemove(path);
    }

    bool doRead_(u32* readSize, FileHandle* handle, u8* buf, u32 size) override
    {
        return mFileDevice->tryRead(readSize, handle, buf, size);
    }

    bool doWrite_(u32* writeSize, FileHandle* handle, const u8* buf, u32 size) override
    {
        return mFileDevice->tryWrite(writeSize, handle, buf, size);
    }

    bool doSeek_(FileHandle* handle, s32 offset, SeekOrigin origin) override
    {
        return mFileDevice->trySeek(handle, offset, origin);
    }

    bool doGetCurrentSeekPos_(u32* pos, FileHandle* handle) override
    {
        return mFileDevice->tryGetCurrentSeekPos(pos, handle);
    }

    bool doGetFileSize_(u32* size, const SafeString& path) override
    {
        return mFileDevice->tryGetFileSize(size, path);
    }

    bool doGetFileSize_(u32* size, FileHandle* handle) override
    {
        return mFileDevice->tryGetFileSize(size, handle);
    }

    bool doIsExistFile_(bool* isExist, const SafeString& path) override
    {
        return mFileDevice->tryIsExistFile(isExist, path);
    }

    bool doIsExistDirectory_(bool* isExist, const SafeString& path) override
    {
        return mFileDevice->tryIsExistDirectory(isExist, path);
    }

    FileDevice* doOpenDirectory_(DirectoryHandle* handle, const SafeString& dirname) override
    {
        return mFileDevice->tryOpenDirectory(handle, dirname);
    }

    bool doCloseDirectory_(DirectoryHandle* handle) override
    {
        return mFileDevice->closeDirectory(handle);
    }

    bool doReadDirectory_(u32* readNum, DirectoryHandle* handle, DirectoryEntry* entry, u32 num) override
    {
        return mFileDevice->tryReadDirectory(readNum, handle, entry, num);
    }

    bool doMakeDirectory_(const SafeString& path, u32 permission) override
    {
        return mFileDevice->tryMakeDirectory(path, permission);
    }

    bool isMatchDevice_(const HandleBase* handle) const override
    {
        return mFileDevice->isMatchDevice_(handle);
    }

    RawErrorCode doGetLastRawError_() const override
    {
        return mFileDevice->getLastRawError();
    }

protected:
    FileDevice* mFileDevice;
};

} // namespace sead
