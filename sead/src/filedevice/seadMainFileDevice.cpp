#include <filedevice/seadMainFileDevice.h>

#include <basis/seadRawPrint.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <filedevice/win/seadWinContentFileDeviceWin.h>
#endif // SEAD_PLATFORM_WINDOWS

#if defined(SEAD_PLATFORM_SDL)
#include <filedevice/sdl/seadSDLContentFileDeviceSDL.h>
#endif // SEAD_PLATFORM_SDL

namespace sead {

MainFileDevice::MainFileDevice(Heap* heap)
    : FileDevice("main")
    , mFileDevice(nullptr)
{
#if defined(SEAD_PLATFORM_WINDOWS)
    mFileDevice = new(heap) WinContentFileDevice();
#elif defined(SEAD_PLATFORM_SDL)
    mFileDevice = new(heap) SDLContentFileDevice();
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
