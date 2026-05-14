#include <controller/sdl/seadSDLJoyPadDeviceSDL.h>

namespace sead {

SDLJoyPadDevice::SDLJoyPadDevice(ControllerMgr* mgr)
    : ControlDevice(mgr)
    , mPads()
{
    mId = ControllerDefine::DeviceId::eSDLJoyPad;
}

void SDLJoyPadDevice::initialize(Heap* heap)
{
    // TODO
    SEAD_UNUSED(heap);
}

void SDLJoyPadDevice::calc()
{
    // TODO
}

} // namespace sead
