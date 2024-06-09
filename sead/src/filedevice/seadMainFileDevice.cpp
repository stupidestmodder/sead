#include <filedevice/seadMainFileDevice.h>

#include <basis/seadRawPrint.h>

#ifdef SEAD_PLATFORM_WINDOWS
#include <filedevice/win/seadWinContentFileDeviceWin.h>
#endif // SEAD_PLATFORM_WINDOWS

namespace sead {

MainFileDevice::MainFileDevice(Heap* heap)
    : FileDevice("main")
    , mFileDevice(nullptr)
{
#ifdef SEAD_PLATFORM_WINDOWS
    mFileDevice = new(heap) WinContentFileDevice();
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

    SEAD_ASSERT(mFileDevice);
}

MainFileDevice::~MainFileDevice()
{
    if (mFileDevice)
    {
        delete mFileDevice;
        mFileDevice = nullptr;
    }
}

void MainFileDevice::traceFilePath(const SafeString& path) const
{
    SEAD_PRINT("[%s] %s\n", mDriveName.cstr(), path.cstr());
    mFileDevice->traceFilePath(path);
}

void MainFileDevice::traceDirectoryPath(const SafeString& path) const
{
    SEAD_PRINT("[%s] %s\n", mDriveName.cstr(), path.cstr());
    mFileDevice->traceDirectoryPath(path);
}

void MainFileDevice::resolveFilePath(BufferedSafeString* out, const SafeString& path) const
{
    mFileDevice->resolveFilePath(out, path);
}

void MainFileDevice::resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const
{
    mFileDevice->resolveDirectoryPath(out, path);
}

} // namespace sead
