#include <filedevice/sdl/seadSDLNativeFileDeviceSDL.h>

namespace sead {

SDLNativeFileDevice::SDLNativeFileDevice()
    : SDLFileDevice("native", "")
{
}

void SDLNativeFileDevice::doResolvePath_(BufferedSafeString* out, const SafeString& path) const
{
    // TODO: ?
    // CHAR exPath[512];
    // ExpandEnvironmentStringsA(path.cstr(), exPath, 512);

    out->copy(path.cstr());
}

} // namespace sead
