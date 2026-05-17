#pragma once

#include <filedevice/sdl/seadSDLFileDeviceSDL.h>

namespace sead {

class SDLContentFileDevice : public SDLFileDevice
{
    SEAD_RTTI_OVERRIDE(SDLContentFileDevice, SDLFileDevice);

public:
    SDLContentFileDevice();
};

} // namespace sead
