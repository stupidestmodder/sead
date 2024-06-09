#include <controller/win/seadWinJoyPadDeviceWin.h>

namespace sead {

WinJoyPadDevice::WinJoyPadDevice(ControllerMgr* mgr)
    : ControlDevice(mgr)
    , mPads()
{
    mId = ControllerDefine::DeviceId::eWinJoyPad;
}

void WinJoyPadDevice::initialize(Heap* heap)
{
    // TODO
    SEAD_UNUSED(heap);
}

void WinJoyPadDevice::calc()
{
    // TODO
}

} // namespace sead
