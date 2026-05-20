#pragma once

#include <basis/win/seadWindows.h>
#include <filedevice/seadFileDevice.h>

namespace sead {

// TODO
class WinFileDevice : public FileDevice
{
    SEAD_RTTI_OVERRIDE(WinFileDevice, FileDevice);

public:
    WinFileDevice(const SafeString& driveName, const SafeString& cwdPath)
        : FileDevice(driveName)
        , mCWD(cwdPath)
        , mLastRawError(0)
    {
    }

    ~WinFileDevice() override
    {
    }

    void changeDir(const SafeString& path)
    {
        mCWD = path;
    }

    const SafeString& getCWD()
    {
        return mCWD;
    }

    static bool setCurrentDirectory(const SafeString& path);

protected:
    bool doIsAvailable_() const override
    {
        return true;
    }

    FileDevice* doOpen_(FileHandle* handle, const SafeString& filename, FileOpenFlag flag) override;
    bool doClose_(FileHandle* handle) override;
    bool doFlush_(FileHandle* handle) override;
    bool doRemove_(const SafeString& path) override;
    bool doRead_(u32* readSize, FileHandle* handle, u8* buf, u32 size) override;
    bool doWrite_(u32* writeSize, FileHandle* handle, const u8* buf, u32 size) override;
    bool doSeek_(FileHandle* handle, s32 offset, SeekOrigin origin) override;
    bool doGetCurrentSeekPos_(u32* pos, FileHandle* handle) override;
    bool doGetFileSize_(u32* size, const SafeString& path) override;
    bool doGetFileSize_(u32* size, FileHandle* handle) override;
    bool doIsExistFile_(bool* isExist, const SafeString& path) override;
    bool doIsExistDirectory_(bool* isExist, const SafeString& path) override;
    FileDevice* doOpenDirectory_(DirectoryHandle* handle, const SafeString& dirname) override;
    bool doCloseDirectory_(DirectoryHandle* handle) override;
    bool doReadDirectory_(u32* readNum, DirectoryHandle* handle, DirectoryEntry* entry, u32 num) override;
    bool doMakeDirectory_(const SafeString& path, u32 permission) override;
    RawErrorCode doGetLastRawError_() const override;
    void doResolvePath_(BufferedSafeString* out, const SafeString& path) const override;

    HANDLE* getHANDLE_(HandleBase* handle) const
    {
        HandleBuffer& handleBuffer = getHandleBaseHandleBuffer_(handle);
        return reinterpret_cast<HANDLE*>(&handleBuffer[0]);
    }

    void setHANDLE_(HandleBase* handle, HANDLE handleInner) const
    {
        HANDLE* ptr = getHANDLE_(handle);
        *ptr = handleInner;
    }

protected:
    SafeString mCWD;
    RawErrorCode mLastRawError;
};

} // namespace sead
