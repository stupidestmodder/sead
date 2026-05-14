#pragma once

#include <filedevice/sdl/seadSDLFileDeviceSDL.h>

namespace sead {

class SDLNativeFileDevice : public SDLFileDevice
{
    SEAD_RTTI_OVERRIDE(SDLNativeFileDevice, SDLFileDevice);

public:
    SDLNativeFileDevice();

protected:
    void doResolvePath_(BufferedSafeString* out, const SafeString& path) const override;
};

} // namespace sead
