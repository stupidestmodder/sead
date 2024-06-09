#pragma once

#include <filedevice/seadFileDevice.h>
#include <heap/seadDisposer.h>

namespace sead {

class MainFileDevice;

class FileDeviceMgr
{
    SEAD_SINGLETON_DISPOSER(FileDeviceMgr);

public:
    static const s32 cNoDrivePathBufferSize = 256;
    static const s32 cFileOpenNumMax = 16;
    static const s32 cDirectoryOpenNumMax = 16;

private:
    using DeviceList = TList<FileDevice*>;

public:
    FileDeviceMgr();
    ~FileDeviceMgr();

    FileDevice* open(FileHandle* handle, const SafeString& filename, FileDevice::FileOpenFlag flag, u32 divNum)
    {
        FileDevice* ret = tryOpen(handle, filename, flag, divNum);
        SEAD_ASSERT_MSG(ret, "open failed. [%s]", filename.cstr());
        return ret;
    }

    FileDevice* tryOpen(FileHandle* handle, const SafeString& filename, FileDevice::FileOpenFlag flag, u32 divNum);

    FileDevice* findDeviceFromPath(const SafeString& path, BufferedSafeString* noDrivePath) const;

    FileDevice* openDirectory(DirectoryHandle* handle, const SafeString& dirname)
    {
        FileDevice* ret = tryOpenDirectory(handle, dirname);
        SEAD_ASSERT_MSG(ret, "open directory failed. [%s]", dirname.cstr());
        return ret;
    }

    FileDevice* tryOpenDirectory(DirectoryHandle* handle, const SafeString& dirname);

    u8* load(FileDevice::LoadArg& arg)
    {
        u8* ret = tryLoad(arg);
        SEAD_ASSERT_MSG(ret, "load failed. [%s]", arg.path.cstr());
        return ret;
    }

    u8* tryLoad(FileDevice::LoadArg& arg);

    void unload(u8* data);

    // TODO
    u8* load(const SafeString&, Heap* heap);
    u8* tryLoad(const SafeString&, Heap* heap);

    bool save(FileDevice::SaveArg& arg)
    {
        bool success = trySave(arg);
        SEAD_ASSERT_MSG(success, "save failed. [%s]", arg.path.cstr());
        return success;
    }

    bool trySave(FileDevice::SaveArg& arg);

    // TODO
    bool save(const SafeString&, const u8*, u32);
    bool trySave(const SafeString&, const u8*, u32);

    void mount(FileDevice* device, const SafeString& driveName = SafeString::cEmptyString);
    void unmount(const SafeString& drive);
    void unmount(FileDevice* device);

    FileDevice* setDefaultFileDevice(FileDevice* device)
    {
        FileDevice* old = mDefaultFileDevice;
        mDefaultFileDevice = device;
        return old;
    }

    FileDevice* getDefaultFileDevice() const
    {
        return mDefaultFileDevice;
    }

    MainFileDevice* getMainFileDevice() const
    {
        return mMainFileDevice;
    }

    FileDevice* findDevice(const SafeString& drive) const;

    void traceFilePath(const SafeString& path) const;
    void traceDirectoryPath(const SafeString& path) const;
    void resolveFilePath(BufferedSafeString* out, const SafeString& path) const;
    void resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const;

protected:
    void mount_(Heap* heap);
    void unmount_();

protected:
    DeviceList mDeviceList;
    FileDevice* mDefaultFileDevice;
    MainFileDevice* mMainFileDevice;

#ifdef SEAD_PLATFORM_WINDOWS
    //* Nothing
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
};

} // namespace sead
