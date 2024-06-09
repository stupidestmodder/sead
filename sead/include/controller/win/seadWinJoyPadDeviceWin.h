#pragma once

#include <container/seadBuffer.h>
#include <controller/seadControlDevice.h>

namespace sead {

// TODO
class WinJoyPadDevice : public ControlDevice
{
    SEAD_RTTI_OVERRIDE(WinJoyPadDevice, ControlDevice);

public:
    struct PadStatus
    {
    };

    static const u32 cNumMax = 4;

public:
    explicit WinJoyPadDevice(ControllerMgr* mgr);

    void initialize(Heap* heap);

    void calc() override;

protected:
    Buffer<PadStatus> mPads;
};

} // namespace sead
