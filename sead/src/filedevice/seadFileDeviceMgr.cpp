#include <filedevice/seadFileDeviceMgr.h>

#include <filedevice/seadMainFileDevice.h>
#include <filedevice/seadPath.h>
#include <heap/seadHeapMgr.h>

namespace sead {

SEAD_SINGLETON_DISPOSER_IMPL(FileDeviceMgr);

FileDeviceMgr::FileDeviceMgr()
    : mDeviceList()
    , mDefaultFileDevice(nullptr)
    , mMainFileDevice(nullptr)
{
    HeapMgr* heapMgr = HeapMgr::instance();
    if (!heapMgr)
    {
        SEAD_ASSERT_MSG(false, "FileDeviceMgr need HeapMgr");
        return;
    }

    Heap* heap = heapMgr->findContainHeap(this);
    mount_(heap);

    mMainFileDevice = new(heap) MainFileDevice(heap);
    mount(mMainFileDevice);

    setDefaultFileDevice(mMainFileDevice);
}

FileDeviceMgr::~FileDeviceMgr()
{
    if (mMainFileDevice)
    {
        delete mMainFileDevice;
        mMainFileDevice = nullptr;
    }

    unmount_();
}

FileDevice* FileDeviceMgr::tryOpen(FileHandle* handle, const SafeString& filename, FileDevice::FileOpenFlag flag, u32 divNum)
{
    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(filename, &noDrivePath);
    if (!device)
        return nullptr;

    return device->tryOpen(handle, noDrivePath, flag, divNum);
}

FileDevice* FileDeviceMgr::findDeviceFromPath(const SafeString& path, BufferedSafeString* noDrivePath) const
{
    FixedSafeString<FileDevice::cDriveNameBufferSize> drive;

    bool success = Path::getDriveName(&drive, path);
    FileDevice* device;

    if (success)
    {
        device = findDevice(drive);
    }
    else
    {
        if (!mDefaultFileDevice)
        {
            SEAD_ASSERT_MSG(false, "drive name not found and default file device is null");
            return nullptr;
        }

        device = mDefaultFileDevice;
    }

    if (!device)
        return nullptr;

    if (noDrivePath)
        Path::getPathExceptDrive(noDrivePath, path);

    return device;
}

FileDevice* FileDeviceMgr::tryOpenDirectory(DirectoryHandle* handle, const SafeString& dirname)
{
    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(dirname, &noDrivePath);
    if (!device)
        return nullptr;

    if (!device->isExistDirectory(noDrivePath))
        return nullptr;

    return device->tryOpenDirectory(handle, noDrivePath);
}

u8* FileDeviceMgr::tryLoad(FileDevice::LoadArg& arg)
{
    SEAD_ASSERT_MSG(arg.path != SafeString::cEmptyString, "path is null");

    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(arg.path, &noDrivePath);
    if (!device)
        return nullptr;

    FileDevice::LoadArg arg_(arg);
    arg_.path = noDrivePath.cstr();

    u8* ret = device->tryLoad(arg_);

    arg.read_size = arg_.read_size;
    arg.roundup_size = arg_.roundup_size;
    arg.need_unload = arg_.need_unload;

    return ret;
}

void FileDeviceMgr::unload(u8* data)
{
    SEAD_ASSERT(data);
    if (data)
        delete data;
}

bool FileDeviceMgr::trySave(FileDevice::SaveArg& arg)
{
    SEAD_ASSERT_MSG(arg.path != SafeString::cEmptyString, "path is null");

    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(arg.path, &noDrivePath);
    if (!device)
        return false;

    FileDevice::SaveArg arg_(arg);
    arg_.path = noDrivePath.cstr();

    bool ret = device->trySave(arg_);

    arg.write_size = arg_.write_size;

    return ret;
}

void FileDeviceMgr::mount(FileDevice* device, const SafeString& driveName)
{
    if (driveName != SafeString::cEmptyString)
        device->setDriveName(driveName);

    mDeviceList.pushBack(device);
}

void FileDeviceMgr::unmount(const SafeString& drive)
{
    FileDevice* device = findDevice(drive);
    if (!device)
    {
        SEAD_ASSERT_MSG(false, "drive not found: %s\n", drive.cstr());
        return;
    }

    unmount(device);
}

void FileDeviceMgr::unmount(FileDevice* device)
{
    mDeviceList.erase(device);

    if (device == mDefaultFileDevice)
        mDefaultFileDevice = nullptr;
}

FileDevice* FileDeviceMgr::findDevice(const SafeString& drive) const
{
    for (FileDevice* device : mDeviceList)
    {
        if (device->getDriveName() == drive)
            return device;
    }

    return nullptr;
}

void FileDeviceMgr::traceFilePath(const SafeString& path) const
{
    SEAD_PRINT("[FileDeviceMgr] %s\n", path.cstr());

    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(path, &noDrivePath);
    if (!device)
    {
        SEAD_WARNING("FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->traceFilePath(noDrivePath);
}

void FileDeviceMgr::traceDirectoryPath(const SafeString& path) const
{
    SEAD_PRINT("[FileDeviceMgr] %s\n", path.cstr());

    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(path, &noDrivePath);
    if (!device)
    {
        SEAD_WARNING("FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->traceDirectoryPath(noDrivePath);
}

void FileDeviceMgr::resolveFilePath(BufferedSafeString* out, const SafeString& path) const
{
    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(path, &noDrivePath);
    if (!device)
    {
        SEAD_WARNING("FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->resolveFilePath(out, noDrivePath);
}

void FileDeviceMgr::resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const
{
    FixedSafeString<cNoDrivePathBufferSize> noDrivePath;

    FileDevice* device = findDeviceFromPath(path, &noDrivePath);
    if (!device)
    {
        SEAD_WARNING("FileDevice not found: %s\n", path.cstr());
        return;
    }

    device->resolveDirectoryPath(out, noDrivePath);
}

void FileDeviceMgr::mount_(Heap* heap)
{
#ifdef SEAD_PLATFORM_WINDOWS
    //* Nothing
    SEAD_UNUSED(heap);
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void FileDeviceMgr::unmount_()
{
#ifdef SEAD_PLATFORM_WINDOWS
    //* Nothing
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

} // namespace sead
