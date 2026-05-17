#pragma once

#include <container/seadBuffer.h>
#include <controller/seadControlDevice.h>

namespace sead {

// TODO
class SDLJoyPadDevice : public ControlDevice
{
    SEAD_RTTI_OVERRIDE(SDLJoyPadDevice, ControlDevice);

public:
    struct PadStatus
    {
    };

    static const u32 cNumMax = 4;

public:
    explicit SDLJoyPadDevice(ControllerMgr* mgr);

    void initialize(Heap* heap);

    void calc() override;

protected:
    Buffer<PadStatus> mPads;
};

} // namespace sead
